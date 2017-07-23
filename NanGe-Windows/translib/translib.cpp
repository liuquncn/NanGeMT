#include "../trans/trans.h"

int Translator::prsForest(PtlNode *rootnode, CplNode *justnode,
							  IToken & token, const char * prsbind)
{
	if (token.isDelim("<"))
	{
		token.input();
		if (justnode->getLChild()) return FAIL;
		if (unifySrcBaseForm(justnode,token)==FAIL) return FAIL;
		else { token.input(); return SUCC; }
	}
	else { if (!token.isDelim("(")) return SUCC; }
	token.input();
	CplNode * child=justnode->getLChild();
	while (!token.isDelim(")") && child)
	{
		int tag;
		if (token.isDelim("!")) { tag=HEAD; token.input(); }
		else tag=NTAG;
		if (tag!=child->getSrcHeadTag()) return FAIL;
		if (strcmp(token,child->getSrcLabel())) return FAIL;
		Variable * var=rootnode->addVar(token,tag)->getVar();
		rootnode->addVarNode(var,child);
		token.input();
		if (!token.isDelim("(")) rootnode->addVarLeaf(var);
		if (prsForest(rootnode,child,token,prsbind)==FAIL) return FAIL;
		child=child->getRNeigh();
	}
	if (!token.isDelim(")") || child) return FAIL;
	token.input();
	return SUCC;
}

int Translator::trnTree(CplNode * rootnode, const char * &trnpart, const SftLink * sftbott)
{
	IToken token(trnpart);
	token.input();
	if (token.isDelim("||")) return FAIL;
	if (!token.isDelim("=>"))
		error("Translator::trnTree(): not a TrnPart while trnTree().",trnpart);
	token.input();
	pushNodeUnity(rootnode);
	rootnode->pushVarList();
	rootnode->setTransed(TRANSED);
	rootnode->pushNodeList();
	rootnode->addOldNode(rootnode);
	if (unifyTgtLabel(rootnode,token)==FAIL) goto fail;
	if (trnForest(rootnode,rootnode,token)==FAIL) goto fail;
	rootnode->set_Bind(token.getInPntr());
	/* set Target Herit */
	if (rootnode->getFest()->getTgtHerit()!=NULL)
	{
		CplNode * parent, * child;
		for (child=rootnode,parent=child->getParent();
			 parent && parent->getTgtHead()==child;
			 child=parent,parent=child->getParent())
		{
			if (setTgtHerit(parent,child)==FAIL)
				error("Translator::treTree(): setTgtHerit() failed.",parent->get_Rule());
		}
	}
	/* do Bind */
	if (trnUnneedful(rootnode->getLChild()))
	{
		if (unifyBind(rootnode,token)==FAIL)
		{
#ifdef KBMS
			output_trnNodeFail(rootnode,token);
			WchTrnFail=YES;
#endif
			goto fail;
		}
#ifdef KBMS
		output_trnNodeSucc(rootnode);
#endif
		Unity::setDefaultSwitch(SEM|SRC|TGTLEXSTA);
		CplNode * parent, * child;
		for (child=rootnode,parent=child->getParent();
			 parent && trnUnneedful(child->getRNeigh());
			 child=parent,parent=child->getParent())
		{
			if (parent->getTransed()==UNNEEDFUL) continue;
			IToken bindtoken(parent->get_Bind());
			bindtoken.input();
  			if (unifyBind(parent,bindtoken)==FAIL)
			{
#ifdef KBMS
				output_trnNodeFail(parent,bindtoken);
				WchTrnFail=YES;
#endif
				Unity::setDefaultSwitch(SEM|SRC);
				goto fail;
			}
#ifdef KBMS
			output_trnNodeSucc(parent);
#endif
		}
#ifdef KBMS
		WchTrnFail=NO;
#endif
		Unity::setDefaultSwitch(SEM|SRC);
	}
#ifdef KBMS
	else { output_trnNode(rootnode); WchTrnFail=NO; }
#endif
	/* set the shift-root node for shift node */
	SftLink * sftlink;
	for (sftlink=TheSftList.getLinkHead();sftlink;sftlink=sftlink->getNext())
	{
		SftNode * sftnode=sftlink->getNode();
		if (sftnode->getSftRoot()) break;
		IToken sfttoken(sftnode->getSftPntr());
		sfttoken.input();
		Variable sftvar;
		sftvar.inputFrom(sfttoken);
		VarLink * varlink=rootnode->srhVarNode(&sftvar);
		if (!varlink) error("Translator::trnTree(): Undefined shift variable.",trnpart);
		CplNode * sftroot=varlink->getNode();
		if (!sftroot) error("Translator::trnTree(): Undefined shift rootnode.",trnpart);
		sftnode->setSftRoot(sftroot);
	}
	while ( !token.isDelim("||") && !token.isDelim("=>") && !token.isEmpty() )
		token.input();
	trnpart=token.getInPntr();
	return SUCC;
  fail:
	while ( !token.isDelim("||") && !token.isDelim("=>") && !token.isEmpty() )
		token.input();
	trnpart=token.getInPntr();
	trnTreeBack(rootnode,sftbott);
	return FAIL;
}

void Translator::trnTreeBack(CplNode * rootnode, const SftLink * sftbott)
{
	if (rootnode->getTransed()==UNNEEDFUL) return;
	if (rootnode->getRuleType() & PRSRULE) rootnode->clrVarUsed();

	CplLink * link=rootnode->topOldNodeList()->getLinkHead();
	CplNode * node;
	for (;link && (node=link->getNode())!=rootnode;link=link->getNext())
	{
		if (node->getTgtHeadTag()==HEAD)
		{
			node->setTgtHeadTag(NTAG);
			node->getParent()->clrTgtHead();
		}
		if (node->getTransed()!=UNNEEDFUL)
			{ node->setTgtLabel(NULL); node->setTransed(UNTRANSED); }
		if (node->getFest()->getTgtHerit()!=NULL)
			clrTgtHerit(node);
	}
	if (rootnode->getFest()->getTgtHerit()!=NULL)
	{
		clrTgtHerit(rootnode);
		CplNode * parent, * child;
		for (child=rootnode,parent=child->getParent();
			 parent && parent->getTgtHead()==child;
			 child=parent,parent=child->getParent())
			clrTgtHerit(parent);
	}
	rootnode->setTransed(SWITCHED);
	if (rootnode==TheRootNode) rootnode->setTgtLabel(NULL);

	rootnode->popNodeList();
	rootnode->popVarList();
	rootnode->clrVarUsed();
	popNodeUnity();
	TheSftList.delto(sftbott);
}

int Translator::trnSwitch(CplNode * rootnode, const char * &trnpart)
{
	if (rootnode->getTransed()!=UNTRANSED && rootnode->getTransed()!=SEMITRANSED)
		error("Translator::trnSwitch(): on a node which is not UNTRANSED.");
	if (!trnpart || !*trnpart) return FAIL;
	pushNodeUnity(rootnode);
	rootnode->setTransed(SWITCHED);
	IToken token(trnpart); token.input();
	if (!token.isDelim("||")) return SUCC; /* PrsRule with no TrnSwitch */
	while (token.isDelim("||"))
	{
		UnityMark mark;
		recordUnity(mark);
		token.input();
		if (unifyBind(rootnode,token)==SUCC)
			{ trnpart=token.getInPntr(); return SUCC; }
#ifdef KBMS
		else if (rootnode->getSpyLink())
			{ IToken bindtoken(token); output_spyBind(rootnode,token); }
#endif
		recoverUnity(mark);
		token.goupto("||");
	}
	trnpart=token.getInPntr();
	trnSwitchBack(rootnode);
	return FAIL;
}

void Translator::trnSwitchBack(CplNode * rootnode)
{
	if (rootnode->getTransed()!=SWITCHED) return;
	if (rootnode==TheRootNode) rootnode->setTransed(UNTRANSED);
	else rootnode->setTransed(SEMITRANSED);
	popNodeUnity();
}

int Translator::trnUnneedful(CplNode * node)
{
	if (node==NULL) return TRUE;
	if (node->getTransed()!=UNNEEDFUL) return FALSE;
	return (trnUnneedful(node->getLChild()) && trnUnneedful(node->getRNeigh()));
}

int Translator::trnForest(CplNode * rootnode, CplNode * justnode, IToken & token)
{
	const char * prsrule=rootnode->get_Rule(); 
	if (token.isDelim("<"))
	{
		token.input();
		if (unifyTgtBaseForm(justnode,token)==FAIL) return FAIL;
		else { justnode->setLChild(NULL); token.input(); }
		setTgtHerit(justnode,(CplNode *)NONE);
	}
	else if (token=="(")
	{
		CplNode * lnode, * rnode, *lchild;
		lnode=rnode=lchild=NULL;
		token.input();
		while (!token.isDelim(")"))
		{
			int tag;
			if (token.isDelim("!")) { tag=HEAD; token.input(); }
			else if (token.isDelim("#")) { tag=MOVE; token.input(); }
			else if (token.isDelim("&")) { tag=REMV; token.input(); }
			else if (token.isDelim("*")) { tag=COPY; token.input(); }
			else tag=NTAG;

			IToken tgttoken=token;
			token.input();
			if (tag==MOVE || tag==REMV || tag==COPY) /* shift node */
			{
				SftNode * snode=new SftNode(rootnode);
				snode->setAge(ADULT);
				snode->setSftTag(tag);
				snode->setSftPntr(token.getInPntr());
				rootnode->addNewNode(snode);
				token.input();
				token.input();
				TheSftList.add(snode);
				rnode=snode;
			}
			else if (Variable::leadBy(token)!='/') /* new node */
			{
				rnode=new CplNode(rootnode);
				rnode->setAge(ADULT);
				rootnode->addNewNode(rnode);
				if (tag==HEAD)
				{
					if (justnode->getTgtHead())
						error("Translator::trnForest(): set TgtHead twice.",
							  prsrule);
					justnode->setTgtHead(rnode);
					rnode->setTgtHeadTag(HEAD);
				}
				rnode->setTransed(UNNEEDFUL);
			}
			else /* old node */
			{
				Variable var;
				var.inputFrom(token);
				VarLink * varlink;
				if (!(varlink=rootnode->srhLeftVarNode(&var)))
					error("Translator::trnForest(): label variable not found.",
						  prsrule);
				if (token.isDelim("(") && varlink->getVar()->getLeaf()==YES)
					error("Translator::trnForest(): leaf node trans to "
						  "a non-leaf node.",prsrule);
				if (!token.isDelim("(") && varlink->getVar()->getLeaf()==NO)
					error("Translator::trnForest(): non-leaf node trans to "
						  "a leaf node.",prsrule);
				if (varlink->getVar()->getUsed()==YES)
					error("Translator::trnForest(): duplicate transform of "
						  "a node.",prsrule);
				varlink->getVar()->setUsed(YES);
				if (varlink->getNode()->getTransed()==UNTRANSED)
				{
					if (varlink->getVar()->getLeaf()==YES)
						varlink->getNode()->setTransed(SEMITRANSED);
					else varlink->getNode()->setTransed(TRANSED);
				}
				rnode=varlink->getNode();
				rootnode->addOldNode(rnode);
				if (tag==HEAD)
				{
					if (justnode->getTgtHead())
						error("Translator::trnForest(): set TgtHead twice.",
							  prsrule);
					justnode->setTgtHead(rnode);
					rnode->setTgtHeadTag(HEAD);
				}
			}
			if (lnode) lnode->setRNeigh(rnode);
			else justnode->setLChild(rnode);
			rnode->setParent(justnode);
			char tgtlabel[TOKENLENGTH];
			strcpy(tgtlabel,tgttoken);
			if (unifyTgtLabel(rnode,tgttoken)==FAIL) return FAIL;
			if (token=="(") rnode->setTransed(UNNEEDFUL);
			if (trnForest(rootnode,rnode,token)==FAIL) return FAIL;
			VarLink * varlink=rootnode->addVar(tgtlabel,tag);
			rootnode->addVarNode(varlink->getVar(),rnode);
			lnode=rnode;
		}
		if (justnode->getTgtHead()==NULL)
			setTgtHerit(justnode,(CplNode *)NONE);
		else if (justnode->getTgtHead()->getFest()->getTgtHerit()!=NULL)
			setTgtHerit(justnode,justnode->getTgtHead());
		if (!lnode) justnode->setLChild(NULL);
		else lnode->setRNeigh(NULL);
		token.input();
	}
	else
	{
		if (justnode->hasType()!=SHIFTTO && justnode->getTransed()==UNNEEDFUL)
			setTgtHerit(justnode,(CplNode *)NONE);
	}
	return SUCC;
}

int Translator::makLTree(CplNode * rootnode, const char * rule,
						 const char * &rightpart)
{
	Variable var;
	pushNodeUnity(rootnode);
	rootnode->pushVarList();
	IToken token(rule);
	token.goover("}");
	IToken tokenbind=token;
	tokenbind.input();
	if (tokenbind.isDelim("(") || tokenbind.isDelim("<")) tokenbind.gouptomatch();
	tokenbind.input();
	const char * bind=tokenbind.getInPntr();
	if (!token.isIdent(rootnode->getTgtLabel())) goto fail;
	var.doRoot();
	token.input();
	if (!token.isDelim("("))
		error("Translator::makLTree(): left-side of makrule shoud be a tree.",rule);
	if (makLForest(rootnode,rootnode,token,bind)==FAIL) goto fail;
	if (unifyBind(rootnode,token)==FAIL) goto fail;
	token.goupto("=>");
	rightpart=token.getInPntr();
	return SUCC;
  fail:
	makLTreeBack(rootnode);
	return FAIL;
}

void Translator::makLTreeBack(CplNode * rootnode)
{
	rootnode->popVarList();
	popNodeUnity();
}

int Translator::makLForest(CplNode * rootnode, CplNode * justnode,
						   IToken & token, const char * bind)
{
	if (token.isDelim("<"))
	{
		token.input();
		if (justnode->getLChild()) return FAIL;
		if (unifyTgtBaseForm(justnode,token)==FAIL) return FAIL;
		else { token.input(); return SUCC; }
	}
	else { if (!token.isDelim("(")) return SUCC; }
	token.input();
	CplNode * child=justnode->getLChild();
	while (!token.isDelim(")") && child)
	{
		int tag;
		if (token.isDelim("!")) { tag=HEAD; token.input(); }
		else tag=NTAG;
		if (tag!=child->getTgtHeadTag()) return FAIL;
		if (strcmp(token,child->getTgtLabel())) return FAIL;
		Variable * var=rootnode->addVar(token,tag)->getVar();
		rootnode->addVarNode(var,child);
		token.input();
		if (!token.isDelim("(")) rootnode->addVarLeaf(var);
		if (makLForest(rootnode,child,token,bind)==FAIL) return FAIL;
		child=child->getRNeigh();
	}
	if (!token.isDelim(")") || child) return FAIL;
	token.input();
	return SUCC;
}

int Translator::makRTree(CplNode * rootnode, const char * &rightpart,
						 const SftLink * sftbott)
{
	IToken token(rightpart);
	token.input();
	if (!token.isDelim("=>"))
		error("Translator::makRTree(): missing rightpart: ",rightpart);
	token.input();
	pushNodeUnity(rootnode);
	rootnode->pushVarList();
	rootnode->pushNodeList();
	rootnode->addOldNode(rootnode);
	if (!token.isIdent(rootnode->getTgtLabel())) goto fail;
	token.input();
	if (!token.isDelim("("))
		error("Translator::makRTree(): right-side of makrule "
			  "shoud be a tree.",rightpart);
	if (makRForest(rootnode,rootnode,token)==FAIL) goto fail;
	if (unifyBind(rootnode,token)==FAIL) goto fail;
	/* set the shift-root node for shift node */
	SftLink * sftlink;
	for(sftlink=TheSftList.getLinkHead();sftlink;sftlink=sftlink->getNext())
	{
		SftNode * sftnode=sftlink->getNode();
		if (sftnode->getSftRoot()) break;
		IToken sfttoken(sftnode->getSftPntr());
		sfttoken.input();
		Variable sftvar;
		sftvar.inputFrom(sfttoken);
		VarLink * varlink=rootnode->srhVarNode(&sftvar);
		if (!varlink)
			error("Translator::makRTree(): Undefined shift variable.",rightpart);
		CplNode * sftroot=varlink->getNode();
		if (!sftroot)
			error("Translator::makRTree(): Undefined shift rootnode.",rightpart);
		sftnode->setSftRoot(sftroot);
	}
	token.goupto("=>");
	rightpart=token.getInPntr();
	rootnode->popVarList();
	return SUCC;
  fail:
	token.goupto("=>");
	rightpart=token.getInPntr();
	rootnode->popVarList();
	makRTreeBack(rootnode,sftbott);
	return FAIL;
}

void Translator::makRTreeBack(CplNode * rootnode, const SftLink * sftbott)
{
	rootnode->clrVarUsed();
	rootnode->popNodeList();
	popNodeUnity();
	TheSftList.delto(sftbott);
}

int Translator::makRForest(CplNode * rootnode, CplNode * justnode,IToken & token)
{
	const char * forest=token.getInPntr();
	if (token.isDelim("<"))
	{
		token.input();
		if (unifyTgtBaseForm(justnode,token)==FAIL) return FAIL;
		else { justnode->setLChild(NULL); token.input(); }
	}
	else if (token.isDelim("("))
	{
		CplNode * lnode, * rnode, *child;
		lnode=rnode=child=NULL;
		token.input();
		while (!token.isDelim(")"))
		{
			int tag;
			if (token.isDelim("!")) { tag=HEAD; token.input(); }
			else if (token.isDelim("#")) { tag=MOVE; token.input(); }
			else if (token.isDelim("&")) { tag=REMV; token.input(); }
			else if (token.isDelim("*")) { tag=COPY; token.input(); }
			else tag=NTAG;

			IToken tgttoken=token;
			token.input();
			if (tag==MOVE || tag==REMV || tag==COPY)
			{
				SftNode * snode=new SftNode(rootnode);
				snode->setAge(ADULT);
				snode->setSftTag(tag);
				snode->setSftPntr(token.getInPntr());
				rootnode->addNewNode(snode);
				token.input();
				token.input();
				TheSftList.add(snode);
				rnode=snode;
			}
			else if (Variable::leadBy(token)!='/') /* new node */
			{
				rnode=new CplNode(rootnode);
				rnode->setAge(ADULT);
				rootnode->addNewNode(rnode);
				if (tag==HEAD)
				{
					if (justnode->getTgtHead())
						error("Translator::makRForest(): set TgtHead twice.",
							  forest);
					justnode->setTgtHead(rnode);
					rnode->setTgtHeadTag(HEAD);
					setTgtHerit(rnode,justnode);
				}
				else setTgtHerit(rnode,(CplNode *)NONE);
			}
			else /* old node */
			{
				Variable var;
				var.inputFrom(token);
				VarLink * varlink;
				if (!(varlink=rootnode->srhLeftVarNode(&var)))
					error("Translator::makRForest(): label variable not found.",
						  forest);
				if (token.isDelim("(") && varlink->getVar()->getLeaf()==YES)
					error("Translator::makRForest(): leaf node trans to "
						  "a non-leaf node.", forest);
				if (!token.isDelim("(") && varlink->getVar()->getLeaf()==NO)
					error("Translator::makRForest(): non-leaf node trans to "
						  "a leaf node.", forest);
				if (varlink->getVar()->getUsed()==YES)
					error("Translator::makRForest(): duplicate transform of "
						  "a node.", forest);
				varlink->getVar()->setUsed(YES);
				rnode=varlink->getNode();
				rootnode->addOldNode(rnode);
				int oldtag=rnode->getTgtHeadTag();
				if (tag==HEAD && oldtag==HEAD && justnode->getTgtHead()!=rnode)
					error("Translator::makRForest(): cannot change head node",
						  forest);
				if (tag!=HEAD && oldtag==HEAD || tag==HEAD && oldtag!=HEAD)
					error("Translator::makRForest(): cannot change the tag.",
						  forest);
			}
			if (lnode) lnode->setRNeigh(rnode);
			else justnode->setLChild(rnode);
			rnode->setParent(justnode);
			char tgtlabel[TOKENLENGTH];
			strcpy(tgtlabel,tgttoken);
			if (unifyTgtLabel(rnode,tgttoken)==FAIL)
				return FAIL;
			if (makRForest(rootnode,rnode,token)==FAIL)
				return FAIL;
			VarLink * varlink=rootnode->addVar(tgtlabel,tag);
			rootnode->addVarNode(varlink->getVar(),rnode);

			lnode=rnode;
		}
		if (!lnode) justnode->setLChild(NULL);
		else lnode->setRNeigh(NULL);
		token.input();
	}
	return SUCC;
}

