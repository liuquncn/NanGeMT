#include "../basic/constant.h"
#include "struct.h"
#include <strstream>

void SrcStruct::freeSgmts()
{
	SrcSgmt * sgmt, * sucs;
	for (sgmt=SrcSgmtHead;sgmt;sgmt=sucs)
	{
		if (sgmt->ThisFstSect)
			error("SrcStruct::freeSgmts(): Free Sgmts before Nodes.");
		sucs=sgmt->getSucsSgmt(); delete sgmt;
	}
	SrcSgmtHead=SrcSgmtTail=NULL;
}

void SrcStruct::freeNodes()
{
	SrcSgmt * sgmt;
	for (sgmt=SrcSgmtHead;sgmt;sgmt=sgmt->getSucsSgmt())
	{
		SrcSect * sect, * next;
		for (sect=sgmt->ThisFstSect;sect;sect=next)
		{
			next=sect->getSameFstSect();
			PtlNode * node, * next;
			for (node=sect->getFstTempNode();node;node=next)
				{ next=node->getSameSrcSectNode(); delete node; }
			for (node=sect->getFstNode();node;node=next)
				{ next=node->getSameSrcSectNode(); delete node; }
			delete sect;
		}
		sgmt->ThisFstSect=NULL;
	}
}

void SrcStruct::addSrcSgmtHead(SrcSgmt *sgmt)
{
	sgmt->SucsSgmt=SrcSgmtHead;
	sgmt->PrevSgmt=NULL;
	if (SrcSgmtHead) SrcSgmtHead->PrevSgmt=sgmt;
	SrcSgmtHead=sgmt;
	if (!SrcSgmtTail) SrcSgmtTail=sgmt;
}

void SrcStruct::addSrcSgmtTail(SrcSgmt *sgmt)
{
	sgmt->SucsSgmt=NULL;
	sgmt->PrevSgmt=SrcSgmtTail;
	if (SrcSgmtTail) SrcSgmtTail->SucsSgmt=sgmt;
	SrcSgmtTail=sgmt;
	if (!SrcSgmtHead) SrcSgmtHead=sgmt;
}

void SrcStruct::delSrcSgmtHead()
{
	SrcSgmt * sgmt=SrcSgmtHead;
	SrcSgmtHead=sgmt->SucsSgmt;
	if (SrcSgmtHead) SrcSgmtHead->PrevSgmt=NULL;
	if (SrcSgmtTail==sgmt) SrcSgmtTail=NULL;
	delete sgmt;
}

void SrcStruct::delSrcSgmtTail()
{
	SrcSgmt * sgmt=SrcSgmtTail;
	SrcSgmtTail=sgmt->PrevSgmt;
	if (SrcSgmtTail) SrcSgmtTail->SucsSgmt=NULL;
	if (SrcSgmtHead==sgmt) SrcSgmtHead=NULL;
	delete sgmt;
}

void SrcStruct::delSrcSgmt(SrcSgmt * sgmt)
{
	if (SrcSgmtHead==sgmt) SrcSgmtHead=sgmt->SucsSgmt;
	if (SrcSgmtTail==sgmt) SrcSgmtTail=sgmt->PrevSgmt;
	if (sgmt->PrevSgmt) sgmt->PrevSgmt->SucsSgmt=sgmt->SucsSgmt;
	if (sgmt->SucsSgmt) sgmt->SucsSgmt->PrevSgmt=sgmt->PrevSgmt;
	delete sgmt;
}

void TgtStruct::free()
{
	TgtSgmt * sgmt=TgtSgmtHead;
	while (sgmt)
	{
		TgtSgmt * sucs=sgmt->SucsSgmt;
		delete sgmt;
		sgmt=sucs;
	}
	TgtSgmtHead=TgtSgmtTail=NULL;
}

void TgtStruct::addTgtSgmtHead(TgtSgmt *sgmt)
{
	sgmt->SucsSgmt=TgtSgmtHead;
	sgmt->PrevSgmt=NULL;
	if (TgtSgmtHead) TgtSgmtHead->PrevSgmt=sgmt;
	TgtSgmtHead=sgmt;
	if (!TgtSgmtTail) TgtSgmtTail=sgmt;
}

void TgtStruct::addTgtSgmtTail(TgtSgmt *sgmt)
{
	sgmt->SucsSgmt=NULL;
	sgmt->PrevSgmt=TgtSgmtTail;
	if (TgtSgmtTail) TgtSgmtTail->SucsSgmt=sgmt;
	TgtSgmtTail=sgmt;
	if (!TgtSgmtHead) TgtSgmtHead=sgmt;
}

void TgtStruct::addTgtSgmtBefore(TgtSgmt * cursgmt,TgtSgmt * newsgmt)
{
	if (cursgmt==TgtSgmtHead) { addTgtSgmtHead(newsgmt); return; }
	TgtSgmt * prevsgmt=cursgmt->getPrevSgmt();
	prevsgmt->SucsSgmt=newsgmt;
	newsgmt->PrevSgmt=prevsgmt;
	newsgmt->SucsSgmt=cursgmt;
	cursgmt->PrevSgmt=newsgmt;
}

void TgtStruct::addTgtSgmtAfter(TgtSgmt * cursgmt,TgtSgmt * newsgmt)
{
	if (cursgmt==TgtSgmtTail) { addTgtSgmtTail(newsgmt); return; }
	TgtSgmt * sucssgmt=cursgmt->getSucsSgmt();
	sucssgmt->PrevSgmt=newsgmt;
	newsgmt->SucsSgmt=sucssgmt;
	newsgmt->PrevSgmt=cursgmt;
	cursgmt->SucsSgmt=newsgmt;
}

void TgtStruct::delTgtSgmtHead()
{
	TgtSgmt * sgmt=TgtSgmtHead;
	TgtSgmtHead=sgmt->SucsSgmt;
	if (TgtSgmtHead) TgtSgmtHead->PrevSgmt=NULL;
	if (TgtSgmtTail=sgmt) TgtSgmtTail=NULL;
	delete sgmt;
}

void TgtStruct::delTgtSgmtTail()
{
	TgtSgmt * sgmt=TgtSgmtTail;
	TgtSgmtTail=sgmt->PrevSgmt;
	if (TgtSgmtTail) TgtSgmtTail->SucsSgmt=NULL;
	if (TgtSgmtHead=sgmt) TgtSgmtHead=NULL;
	delete sgmt;
}

SrcSgmt * SrcSgmt::srhBgnSgmt(char * bgnchar)
{
	SrcSgmt * sgmt=this;
	while (sgmt && sgmt->BgnChar!=bgnchar) sgmt=sgmt->PrevSgmt;
	if (!sgmt) error("SrcSgmt::srhBgnSgmt(): Fail to search sgmt with begin char : ",bgnchar);
	return sgmt;
}

SrcSgmt * SrcSgmt::srhEndSgmt(char * endchar)
{
	SrcSgmt * sgmt=this;
	while (sgmt && sgmt->EndChar!=endchar) sgmt=sgmt->SucsSgmt;
	if (!sgmt) error("SrcSgmt::srhEndSgmt(): Fail to search sgmt with end char : ", endchar);
	return sgmt;
}

void SrcSect::addTempNode(PtlNode * node)
{
	node->SameSrcSectNode=FstTempNode;
	FstTempNode=node;
}

int SrcSect::delTempNode(PtlNode * node)
{
	if (FstTempNode=node)
		FstTempNode=node->SameSrcSectNode;
	else
	{
		PtlNode * prevnode=FstTempNode;
		while (prevnode && prevnode->SameSrcSectNode!=node)
			prevnode=prevnode->SameSrcSectNode;
		if (!prevnode) return FAIL;
		prevnode->SameSrcSectNode=node->SameSrcSectNode;
	}
	node->SameSrcSectNode=NULL;
	return SUCC;
}

int TgtSgmt::set_BaseForm()
{
	if (_FormScan && *_FormScan)
	{
		IToken token(_FormScan);
		token.input();
		if (!token.isIdent()) error("TgtSgmt()::set_BaseForm(): Invalid target baseform : ",_FormScan);
		strcpy(_BaseForm,token);
		token.input();
		_FormScan=token.isDelim("|")?token.getForeInPntr():token.getInPntr();
	}
	else *_BaseForm=0; 
	return *_BaseForm;
}

int TgtSgmt::set_VaryForm(char * varyform)
{
	if (varyform) strcpy(_VaryForm,varyform);
	else strcpy(_VaryForm,_BaseForm);
	return *_VaryForm;
}

void TrnStack::free()
{
	TrnField * link=TopField;
	while (link)
	{
		TrnField * next=link->Next;
		delete link; link=next;
	}
	TopField=NULL;
}

void MakStack::free()
{
	MakField * link=TopField;
	while (link)
	{
		MakField * next=link->Next;
		delete link; link=next;
	}
	TopField=NULL;
}


PtlNode::PtlNode()
{
	Age=BABY;
	TheSrcSect=NULL;
	SameSrcSectNode=NULL;
	PtlChild=NULL;
	CplChild=NULL;
	SubNodeNum=0;
	ChildSrcHeadTag=NTAG;
	RuleType=MEANING;
	_Rule=NULL;
	_Scan=NULL;
	_Bind=NULL;
	_BaseForm=NULL;
	_VaryForm=NULL;
	Data=NULL;
	pushUnity();
	useUnity();
#ifdef KBMS
	TheSpyLink=NULL;
#endif
}

PtlNode::PtlNode(PtlNode * ptlnode)
{
	if (ptlnode->hasType()!=PARTIAL)
		error("PtlNode::PtlNode(): move from a non-PARTIAL node.");
	Age=ptlnode->Age;
	TheSrcSect=ptlnode->TheSrcSect;
	if (TheSrcSect->delTempNode(ptlnode)==FAIL)
		error("PtlNode::PtlNode(PtlNode *): change a PtlNode already in SrcStruct to CplNode.");
	else TheSrcSect->addTempNode(this);
	PtlChild=ptlnode->PtlChild;
	CplChild=ptlnode->CplChild;
	SubNodeNum=ptlnode->SubNodeNum;
	ChildSrcHeadTag=ptlnode->ChildSrcHeadTag;
	TheVarListStack.moveFrom(ptlnode->TheVarListStack);
	TheUnityStack.moveFrom(ptlnode->TheUnityStack);
	RuleType=ptlnode->RuleType;
	_Rule=ptlnode->_Rule;
	_Scan=ptlnode->_Scan;
	_Bind=ptlnode->_Bind;
	_BaseForm=ptlnode->_BaseForm;
	_VaryForm=ptlnode->_VaryForm;
	ptlnode->_BaseForm=NULL;
	ptlnode->_VaryForm=NULL;
	Data=ptlnode->Data;
#ifdef KBMS
	TheSpyLink=ptlnode->TheSpyLink;
#endif
}

void PtlNode::free()
{
	if (TheSrcSect) TheSrcSect->delTempNode(this);
	if (_BaseForm) { delete _BaseForm; _BaseForm=NULL; }
	if (_VaryForm) { delete _VaryForm; _VaryForm=NULL; }
}

void PtlNode::addIntoSect(SrcSgmt * fstsgmt,SrcSgmt * lstsgmt, SrcSgmtPairList & matchlist)
{
	char * endchar=lstsgmt->getEndChar();
	SrcSect * pstfstsect=fstsgmt->getThisFstSect();
	SrcSect * prefstsect=NULL;
	while ( pstfstsect && pstfstsect->getEndChar()>endchar )
		{ prefstsect=pstfstsect; pstfstsect=pstfstsect->SameFstSect; }
	if (pstfstsect && pstfstsect->LstSrcSgmt==lstsgmt)
		{ TheSrcSect=pstfstsect; pstfstsect->addTempNode(this); return; }

	SrcSect * sect=new SrcSect;
	TheSrcSect=sect;
	sect->addTempNode(this);
	sect->FstSrcSgmt=fstsgmt;
	sect->LstSrcSgmt=lstsgmt;

	if (fstsgmt!=lstsgmt)
	{
		SrcSgmtPairLink * link=matchlist.head();
		while (link)
		{
			if (fstsgmt!=link->getLeftSgmt() && lstsgmt!=link->getRightSgmt())
			{
				const char * fstchar=fstsgmt->getBgnChar();
				const char * lstchar=lstsgmt->getBgnChar();
				const char * leftchar=link->getLeftSgmt()->getBgnChar();
				const char * rightchar=link->getRightSgmt()->getBgnChar();
				int fstposition=fstchar>=leftchar && fstchar<=rightchar;
				int lstposition=lstchar>=leftchar && lstchar<=rightchar;
				if (fstposition!=lstposition) { sect->Valid=false; break; }
			}
			link=link->getNext();
		}
	}

	sect->SameFstSect=pstfstsect;
	if (prefstsect) prefstsect->SameFstSect=sect;
	else fstsgmt->setThisFstSect(sect);

	char * bgnchar=fstsgmt->getBgnChar();
	SrcSect * pstlstsect=lstsgmt->getThisLstSect();
	SrcSect * prelstsect=NULL;
	while ( pstlstsect && pstlstsect->getBgnChar()<bgnchar )
		{ prelstsect=pstlstsect; pstlstsect=pstlstsect->SameLstSect; }
	sect->SameLstSect=pstlstsect;
	if (prelstsect) prelstsect->SameLstSect=sect;
	else lstsgmt->setThisLstSect(sect);
}

int  PtlNode::addIntoStruct()
{
	if (hasType()==COMPLETE && RuleType!=MEANING)
	{ // 同一SrcSect下同一规则产生的结点限制不能超过一定数目
		int sametypenode=0;
		const char * rule=get_Rule();
		PtlNode * node;
		for (node=TheSrcSect->getFstNode();node;node=node->getSameSrcSectNode())
		{
			if (node->hasType()!=COMPLETE) continue;
			if (node->get_Rule()==rule) sametypenode++;
			if (sametypenode>=5)
				return FAIL;
		}
	}
	if (TheSrcSect->delTempNode(this)==FAIL)
		error("PtlNode::addIntoStruct(): cannot delete tempnode of sect.");
	if (TheSrcSect->FstNode)
	{
		SameSrcSectNode=TheSrcSect->FstNode;
		TheSrcSect->FstNode=this;
	}
	else TheSrcSect->FstNode=TheSrcSect->LstNode=this;
	TheSrcSect->NodeNum++;
	return SUCC;
}

void PtlNode::setFamily()
{
	CplNode * rneigh=NULL;
	PtlNode * ptlnode=this;
	while (ptlnode)
	{
		CplNode * cplnode=ptlnode->getCplChild();
		cplnode->setSrcHeadTag(ptlnode->ChildSrcHeadTag);
		cplnode->setRNeigh(rneigh);
		cplnode->setFamily();
		rneigh=cplnode;
		ptlnode=ptlnode->getPtlChild();
	}
}

int PtlNode::isPrior( PtlNode * node )
{
	int cmp;
	if (cmp=(getEndChar()-getBgnChar())-(node->getEndChar()-node->getBgnChar()))
		return cmp>0;
	else return hasType()-node->hasType()<=0;
}

void PtlNode::setPtlChild(PtlNode * node)
{
	if (PtlChild) error("PtlNode::setPtlChild(): set PtlChild twice.");
	PtlChild=node;
	if (node)
	{
		SubNodeNum+=node->SubNodeNum;
		topVarList()->setMidLink(node->topVarList()->getFstLink());
		topUnity()->addChild(node->topUnity());
	}
}

void PtlNode::setCplChild(CplNode * node)
{
	if (CplChild) error("PtlNode::setCplChild(): set CplChild twice.");
	if (node)
	{
		SubNodeNum+=node->SubNodeNum;
		CplChild=node;
		topUnity()->addChild(node->topUnity());
	}
	else error("PtlNode::setCplChild(): set a NULL CplChild.");
}

CplNode::CplNode(CplNode * rootnode)
{
	SubNodeNum++;
	SrcHeadTag=TgtHeadTag=NTAG;
	SrcHead=TgtHead=NULL;
	SrcLabel=TgtLabel=NULL;
	Transed=UNTRANSED;
	Present=YES;
	Sftable=YES;
	FstTgtSgmt=LstTgtSgmt=NULL;
	TheFest=new Fest;
	if (!rootnode) topUnity()->addFestNew(TheFest);
	else rootnode->topUnity()->addFestNew(TheFest);
	pushRelation();
}

CplNode::CplNode(PtlNode * ptlnode) : PtlNode(ptlnode)
{
	SubNodeNum++;
	SrcHeadTag=TgtHeadTag=NTAG;
	SrcHead=TgtHead=NULL;
	SrcLabel=TgtLabel=NULL;
	Transed=UNTRANSED;
	Present=YES;
	Sftable=YES;
	FstTgtSgmt=LstTgtSgmt=NULL;
	TheFest=new Fest;
	topUnity()->addFestNew(TheFest);
	pushRelation();

	CplNode * cplchild;
	PtlNode * ptlchild;
	for (ptlchild=ptlnode;ptlchild;ptlchild=ptlchild->getPtlChild())
	{
		cplchild=ptlchild->getCplChild();
		cplchild->setParent(this);
		if (cplchild->getSrcHeadTag()==HEAD) setSrcHead(cplchild);
	}
	setLChild(cplchild);
}

CplNode * CplNode::getForward(CplNode * rootnode)
{
	if (getLChild()) return getLChild();
	else if (rootnode==this) return NULL;
	else if (getRNeigh()) return getRNeigh();
	else
	{
		CplNode * parent=getParent();
		while (parent && parent!=rootnode && !parent->getRNeigh())
			parent=parent->getParent();
		if (parent && parent!=rootnode) return parent->getRNeigh();
		else return NULL;
	}
}

void CplNode::setFamily()
{
	CplNode * rneigh=NULL;
	PtlNode * ptlnode=this;
	if (RuleType & MEANING) return;
	while (ptlnode)
	{
		CplNode * cplnode=ptlnode->getCplChild();
		cplnode->setSrcHeadTag(ptlnode->getChildSrcHeadTag());
		cplnode->setParent(this);
		cplnode->setRNeigh(rneigh);
		cplnode->setFamily();
		rneigh=cplnode;
		ptlnode=ptlnode->getPtlChild();
	}
}

void CplNode::popOldNodeList()
{
	CplLink * oldnodelink=topOldNodeList()->getLinkHead();
	while (oldnodelink && oldnodelink->getNode())
	{
		oldnodelink->getNode()->popRelation();
		oldnodelink=oldnodelink->getNext();
	}
	OldNodeListStack.pop();
}

void CplNode::popNewNodeList()
{
	CplLink * newnodelink=topNewNodeList()->getLinkHead();
	while (newnodelink && newnodelink->getNode())
	{
		CplNode * newnode=newnodelink->getNode();
		if (newnode->getTgtHeadTag()==HEAD)
			newnode->getParent()->clrTgtHead();
		if (newnode->getTgtHead())
		{
			newnode->getTgtHead()->setTgtHeadTag(NTAG);
			Unity::clrTgtHerit(newnode->getTgtHead()->getFest());
		}
		newnodelink=newnodelink->getNext();
	}
	newnodelink=topNewNodeList()->getLinkHead();
	while (newnodelink && newnodelink->getNode())
	{
		newnodelink->getNode()->popRelation();
		delete newnodelink->getNode();
		newnodelink=newnodelink->getNext();
	}
	NewNodeListStack.pop();
}

SftNode::SftNode(CplNode * rootnode) : CplNode(rootnode)
{
	 Present=NO;
	 Sftable=NO;
	 SubNodeNum=0;
	 Transed=UNNEEDFUL;
	 SftPntr=NULL;
	 SftRoot=SftFrom=NULL;
}

void PtlList::free()
{
	PtlLink * link=FstLink;
	while (link)
	{
		PtlLink * next=link->getNext();
		delete link;
		link=next;
	}
	FstLink=NULL;
}

void CplList::free()
{
	CplLink * link=LinkHead;
	while (link) { CplLink * next=link->Next; delete link; link=next; }
	LinkHead=NULL;
}

void CplList::add(CplNode * node)
{
	CplLink * link=new CplLink;
	link->Node=node;
	link->Next=LinkHead;
	LinkHead=link;
}

CplNode * CplList::del()
{
	if (!LinkHead) return NULL;
	CplLink * link=LinkHead;
	LinkHead=link->Next;
	CplNode * node=link->Node;
	delete link;
	return node;
}

CplLink * CplList::srh(CplNode * node)
{
	CplLink * link=LinkHead;
	while (link) { if (link->Node==node) return link; else link=link->Next; }
	return NULL;
}

void CplListStack::free()
{
	CplListElem * elem=TopElem;
	while (elem) { CplListElem * next=elem->Next; delete elem; elem=next; }
}

void NodeUnityStack::init(CplNode * node)
{
	if (TopLink) error("NodeUnityStack::init(): init while not empty.");
	BaseNode=node;
}

void NodeUnityStack::free()
{
	CplLink * link, * next;
	for (link=TopLink;link;link=next)
	{
		link->getNode()->popUnity();
		next=link->Next;
		delete link;
	}
	TopLink=NULL;
}

void NodeUnityStack::push(CplNode * node)
{
	CplLink * link=TopLink;
	if (link) node->pushUnity(link->getNode()->topUnity());
	else node->pushUnity(BaseNode->topUnity());
	node->useUnity();
	CplLink * newlink=new CplLink;
	newlink->Node=node;
	newlink->Next=TopLink;
	TopLink=newlink;
}

void NodeUnityStack::pop()
{
	CplLink * link=TopLink;
	if (!link) error("NodeUnityStack::pop(): pop from empty stack.");
	TopLink=link->Next;
	link->getNode()->popUnity();
	delete link;
}

void PrsList::addNode( PtlNode * node )
{
	PtlLink * newlink=new PtlLink;
	newlink->setNode(node);
	PtlLink * pstlink=FstLink, *prelink=NULL;
	while (pstlink && !node->isPrior(pstlink->getNode()))
	{ 
		prelink=pstlink;
		pstlink=pstlink->getNext();
	}
	newlink->setNext(pstlink);
	if (prelink) prelink->setNext(newlink);
	else FstLink=newlink;
}

PtlNode * PrsList::delNode()
{
	if (!FstLink) return NULL;
	PtlLink * link=FstLink;
	PtlNode * node=link->getNode();
	FstLink=link->getNext();
	delete link;
	return node;
}

void SftList::free()
{
	SftLink * link=LinkHead;
	while (link) { SftLink * next=link->Next; delete link; link=next; }
	LinkHead=NULL;
}

void SftList::add(SftNode * node)
{
	SftLink * link=new SftLink;
	link->Node=node;
	link->Next=LinkHead;
	LinkHead=link;
}

SftNode * SftList::del()
{
	if (!LinkHead) return NULL;
	SftLink * link=LinkHead;
	LinkHead=link->Next;
	SftNode * node=link->Node;
	delete link;
	return node;
}

void SftList::delto(const SftLink * bott)
{
	if (!LinkHead)
		{ if (bott) error("SftList::delto(): no bott to delto."); else return; }
	SftLink * link=LinkHead;
	while (link && link!=bott)
	{
		SftLink * next=link->Next;
		delete link;
		link=next;
	}
	if (link!=bott) error("SftList::delto(): no bott to delto.");
	LinkHead=link;
}

void Variable::inputFrom(IToken & token)
{
	const char * vtag=token;
	if (*vtag=='%' || *vtag=='/')
	{
		Order=strlen(token);
		token.input();
		if (token.getType()!=IDENT)
			error("Variable::inputFrom(): Invalid variabel label : ",token);
		strcpy(Label,token);
		token.input();
	}
	else if (*vtag=='$') { token.input(); doRoot(); }
	else error("Variable::inputFrom(): Invalid variable tag : ", token);
}

void Variable::outputTo(OToken & token)
{
	unsigned int i;
	if (isRoot()) token.output("$");
	else { for(i=0;i<Order;i++) token.output("%"); token.output(Label); }
}

int Variable::leadBy(IToken & token)
{
	const char c=*(token.getBuffer());
	if (c=='$' || c=='%' || c=='/') return c;
	else return NO;
}

void Variable::skipOver(IToken & token)
{
	if (token.isDelim("$")) token.input();
	else { token.input(); token.input(); }
}

void VarList::free()
{
	VarLink * link=FstLink;
	while(link!=MidLink) { VarLink * next=link->Next; delete link; link=next; }
	FstLink=MidLink=NULL;
}

VarLink * VarList::srhVar(const char * label)
{
	VarLink * link=FstLink;
	while (link)
	{
		if (!strcmp(link->Var.getLabel(),label)) return link;
		link=link->Next;
	}
	return NULL;
}

VarLink * VarList::srhVarNode(Variable * var,int & here)
{
	if (&here) here=YES;
	VarLink * link=FstLink;
	while (link)
	{
		if ( &here && link==MidLink) here=NO;
		if (link->Var==*var) return link;
		link=link->Next;
	}
	return NULL;
}

VarLink * VarList::addVar(const char * label, int order, int tag)
{
	VarLink * newlink=new VarLink;
	Variable * var=newlink->getVar();
	var->setLabel(label);
	var->setOrder(order);
	var->setTag(tag);
	newlink->Node=NULL;
	newlink->Next=FstLink;
	FstLink=newlink;
	return newlink;
}

VarLink * VarList::addVar(const char * label, int tag)
{
	VarLink * link=srhVar(label);
	int order=link?(link->Var.getOrder()+1):1;
	return addVar(label,order,tag);
}

VarLink * VarList::addVarNode(Variable * var, CplNode * node)
{
	VarLink * link=FstLink;
	while (link) { if (link->Var==*var) break; link=link->Next; }
	if (!link) error("VarList::addVarNode(): No such variable.");
	if (link->Node!=NULL)
		error("VarList::addVarNode(): Add duplicate label variable.");
	link->Node=node;
	return link;
}

void VarList::addVarLeaf(Variable * var)
{
	VarLink * link=FstLink;
	while (link) { if (link->Var==*var) break; link=link->Next; }
	if (!link) error("VarList::addVarLeaf(): No such variable.");
	link->Var.setLeaf(YES);
}

void VarList::clrUsed()
{
	VarLink * link=FstLink;
	while (link) { link->Var.setUsed(NO); link=link->Next; }
}

void VarListStack::free()
{
	VarListElem * elem=TopElem;
	while (elem) { VarListElem * next=elem->Next; delete elem; elem=next; }
}

void RelStack::free()
{
	RelElem * elem=TopElem;
	while (elem) { RelElem * next=elem->Next; delete elem; elem=next; }
}


void TgtRootLink::makTree(CplNode * node,Unity * unity)
{
	if (node==NULL) error("TgtRootLink::makTree(): node is NULL.");
	char tree[TREELENGTH];
	OToken token(tree);
	makTreeNode(node,token,unity);
	token.outputNull();
	if (TgtTree) delete TgtTree;
	TgtTree=new char[strlen(tree)+1];
	strcpy(TgtTree,tree);
}

int TgtRootLink::StaticAttribute;

void TgtRootLink::makTreeNode(CplNode * node,OToken & token,Unity * unity)
{
	if (node->getTgtHeadTag()==HEAD) token.output("!");
	token.output(node->getTgtLabel());
	token.output(":");
	char addr[16];
	//_itoa((int)node,addr,10);
	sprintf(addr,"%d",(int)node);
	token.output(addr);
	char buffer[SENTLENGTH];
	strstream ostr(buffer,SENTLENGTH,ios::out);
	if (node->getLChild())
	{
#ifdef KBMS
		if (StaticAttribute)
			unity->output_fest(ostr,node->getFest(),GlobalTgtLang);
		else ostr << "[]";
#else
		ostr << "[]";
#endif // KBMS
		ostr << '\0';
		token.output(buffer);
		token.output("(");
		CplNode * child;
		for (child=node->getLChild();child;child=child->getRNeigh())
			makTreeNode(child,token,unity);
		token.output(")");
	}
	else
	{
		token.output("<");
		Attribute * baseformatt=GlobalModel->getBaseForm(GlobalTgtLang);
		Atom * baseformval=unity->getIntValue(node->getFest(),baseformatt);
		const char * baseform=baseformval->getNucleus();
		token.output(baseform);
		token.output(">");
#ifdef KBMS
		if (StaticAttribute)
			unity->output_fest(ostr,node->getFest(),GlobalTgtLang);
		else ostr << "[]";
#else
		ostr << "[]";
#endif
		ostr << '\0';
		token.output(buffer);
	}
}

void SrcRootLink::free()
{
	SrcNode=NULL;
	delete SrcTree; SrcTree=NULL;
	TgtRootLink * link, * next;
	for (link=TgtRootHead;link;link=next)
		{ next=link->Next; delete link->TgtText; delete link->TgtTree; delete link; }
	TgtRootHead=TgtRootKnot=NULL;
	Prev=Sucs=NULL;
	TgtRootNumber=0;
}

TgtRootLink * SrcRootLink::getTgtRootTail()
{
	TgtRootLink * link=TgtRootHead;
	if (link) while (link->Next) link=link->Next;
	return link;
}

TgtRootLink * SrcRootLink::addTgtRoot(const char * text)
{
	if (SrcNode==NULL) error("SrcRootLink::addTgtRoot(): Node is NULL.");
	TgtRootLink * newlink=new TgtRootLink, * link=TgtRootHead;
	newlink->TgtText=new char[strlen(text)+1];
	strcpy(newlink->TgtText,text);
	SrcNode->useUnity();
	newlink->makTree(SrcNode,SrcNode->topUnity());
	if (TgtRootHead) { while (link->Next) link=link->Next; link->Next=newlink; }
	else TgtRootHead=newlink;
	TgtRootNumber++;
	return newlink;
}

void SrcRootLink::makTree(Unity * unity)
{
	if (SrcNode==NULL) error("SrcRootLink::makTree(): Node is NULL.");
	char tree[TREELENGTH];
	OToken token(tree);
	SrcNode->setFamily();
	makTreeNode(SrcNode,token,unity);
	token.outputNull();
	if (SrcTree) delete SrcTree;
	SrcTree=new char[strlen(tree)+1];
	strcpy(SrcTree,tree);
}

int SrcRootLink::StaticAttribute;

void SrcRootLink::makTreeNode(CplNode * node,OToken &token,Unity * unity)
{
	if (node->getSrcHeadTag()==HEAD) token.output("!");
	token.output(node->getSrcLabel());
	if (node->getRuleType()==LCPRSRULE) token.output("*");
	token.output(":");
	char addr[16];
	//_itoa((int)node,addr,10);
	sprintf(addr,"%d",(int)node);
	token.output(addr);
	char buffer[SENTLENGTH];
	strstream ostr(buffer,SENTLENGTH,ios::out);
	if (node->getLChild())
	{
#ifdef KBMS
		if (StaticAttribute)
			unity->output_fest(ostr,node->getFest(),GlobalSrcLang);
		else ostr << "[]";
#else
		ostr << "[]";
#endif // KBMS
		ostr << '\0';
		token.output(buffer);
		token.output("(");
		CplNode * child;
		for (child=node->getLChild();child;child=child->getRNeigh())
			makTreeNode(child,token,unity);
		token.output(")");
	}
	else
	{
		token.output("<");
		char text[WORDLENGTH], * t=text;
		const char * begin=node->getBgnChar(), *end=node->getEndChar();
		while (begin!=end) *t++=*begin++;
		*t=0;
		token.setBuffer(text);
		token.setType(IDENT);
		token.output();
		token.output(">");
#ifdef KBMS
		if (StaticAttribute)
			unity->output_fest(ostr,node->getFest(),GlobalSrcLang);
		else ostr << "[]";
#else
		ostr << "[]";
#endif // KBMS
		ostr << '\0';
		token.output(buffer);
	}
}

void RootList::free()
{
	SrcRootLink * link, * next;
	for (link=SrcRootHead;link;link=next) { next=link->Sucs; delete link; }
	SrcRootHead=SrcRootTail=NULL;
	Number=0;
}

SrcRootLink * RootList::addSrcRootHead(CplNode * node)
{
	SrcRootLink * newlink=new SrcRootLink;
	newlink->SrcNode=node;
	node->useUnity();
	newlink->makTree(node->topUnity());
	if (SrcRootHead)
	{
		SrcRootHead->Prev=newlink;
		newlink->Sucs=SrcRootHead;
		SrcRootHead=newlink;
	}
	else SrcRootHead=SrcRootTail=newlink;
	Number++;
	return newlink;
}

SrcRootLink * RootList::addSrcRootTail(CplNode * node)
{
	SrcRootLink * newlink=new SrcRootLink;
	newlink->SrcNode=node;
	node->useUnity();
	newlink->makTree(node->topUnity());
	if (SrcRootTail)
	{
		SrcRootTail->Sucs=newlink;
		newlink->Prev=SrcRootTail;
		SrcRootTail=newlink;
	}
	else SrcRootHead=SrcRootTail=newlink;
	Number++;
	return newlink;
}

void RootList::insertSrcRootBehind(SrcRootLink * link,CplNode * node)
{
	SrcRootLink * newlink=new SrcRootLink;
	newlink->SrcNode=node;
	newlink->Prev=link;
	newlink->Sucs=link?link->Sucs:SrcRootHead;
	node->useUnity();
	newlink->makTree(node->topUnity());
	if (!link) SrcRootHead->Prev=newlink;
	else if (link->Sucs) link->Sucs->Prev=newlink;
	else if (SrcRootTail==link) SrcRootTail=newlink;
	else error("RootList::insertSrcRootBehind(): link is not in the list.");
	if (link) link->Sucs=newlink; else SrcRootHead=newlink;
	Number++;
}

void RootList::insertSrcRootBefore(SrcRootLink * link,CplNode * node)
{
	SrcRootLink * newlink=new SrcRootLink;
	newlink->SrcNode=node;
	newlink->Sucs=link;
	newlink->Prev=link?link->Prev:SrcRootTail;
	node->useUnity();
	newlink->makTree(node->topUnity());
	if (!link) SrcRootTail->Sucs=newlink;
	else if (link->Prev) link->Prev->Sucs=newlink;
	else if (SrcRootHead==link) SrcRootHead=newlink;
	else error("RootList::insertSrcRootBefore(): link is not in the list.");
	if (link) link->Prev=newlink; else SrcRootTail=newlink;
	Number++;
}

void RootList::deleteSrcRootAt(SrcRootLink * link)
{
	if (link->Prev) link->Prev->Sucs=link->Sucs;
	else if (SrcRootHead==link) SrcRootHead=link->Sucs;
	else error("RootList::delete(): link is not in the list.");
	if (link->Sucs) link->Sucs->Prev=link->Prev;
	else if (SrcRootTail==link) SrcRootTail=link->Prev;
	else error("RootList::delete(): link is not in the list.");
	delete link;
	Number--;
}

void RootList::altSrcRoot(SrcRootLink * link,CplNode * node)
{
	link->SrcNode=node;
	node->useUnity();
	link->makTree(node->topUnity());
}

void RootList::initialResult()
{
	if (!SrcRootHead) error("RootList::initialResult(): empty RootList.");
	SrcRootLink * link=SrcRootHead;
	link->TgtRootKnot=link->getTgtRootTail();
	if (!link->TgtRootKnot)
		error("RootList::initialResult(): Root with no target text.");
	for (link=link->Sucs;link;link=link->Sucs)
	{
		link->TgtRootKnot=link->getTgtRootHead();
		if (!link->TgtRootKnot)
			error("RootList::initialResult(): Root with no target text.");
	}
}

int  RootList::composeResult(Result & result)
{
	char tgttext[SENTLENGTH];
	char tgttree[TREELENGTH];
	char srctree[TREELENGTH];
	int sentlength=SENTLENGTH;
	int treelength=TREELENGTH;
	if (!SrcRootHead) error("RootList::composeResult(): empty RootList.");
	char * p=tgttext, * pt=tgttree, * ps=srctree;
	SrcRootLink * link=SrcRootHead;
	int isltnchar=NO;
	for (link=SrcRootHead;link;link=link->Sucs)
	{
		if (!link->TgtRootKnot) return FAIL;
		char * q=link->TgtRootKnot->getTgtText();
		if (p!=tgttext && *q && (isltnchar || isLtnChar(q))) *p++=' ';
		while (*q) { isltnchar=isLtnChar(q); setCharNext(p,q); }	/* text */
		char * qt=link->TgtRootKnot->getTgtTree();
		if (pt!=tgttree) *pt++=' ';
		while (*qt) setCharNext(pt,qt);  /* tgttree */
		char * qs=link->getSrcTree();
		if (ps!=srctree) *ps++=' ';
		while (*qs) setCharNext(ps,qs);  /* srctree */
	}
	*p=0;
	*pt=0;
	*ps=0;
	if (p-tgttext>sentlength) error("RootList::composeResult(): text too long.");
	if (pt-tgttree>treelength) error("RootList::composeResult(): tgttree too long.");
	if (ps-srctree>treelength) error("RootList::composeResult(): srctree too long.");
	result.setTargetSentence(tgttext);
	result.setTargetTree(tgttree);
	result.setSourceTree(srctree);
	return SUCC;
}

void RootList::forwardResult()
{
	if (!SrcRootHead) error("RootList::forwardResult(): empty RootList.");
	if (!SrcRootTail->TgtRootKnot) return;
	SrcRootLink * link=SrcRootTail;
	link->TgtRootKnot=link->TgtRootKnot->getNext();
	while (link && (link->Sucs || !link->TgtRootKnot))
    {
        if (!link->TgtRootKnot)
            { link=link->Prev; if (link) link->TgtRootKnot=link->TgtRootKnot->getNext(); }
        else
            { link=link->getSucs(); link->TgtRootKnot=link->getTgtRootHead(); }
    }
}



#ifdef KBMS

static char * SgmtTypeName[]=
		{ "CHNCHAR","LTNCHAR","DGTCHAR","PCTCHAR","OTHCHAR" };

void SpyNode::read(char * &str)
{
	char * spynode=str;

	while (*str && isascii(*str) && isspace(*str)) str++;
	if (!*str) error("SpyNode::read(): need SrcLabel.",spynode);
	Label=str;
	while (*str && *str!='<' && !(isascii(*str) && isspace(*str))) str++;
	if (!*str) error("SpyNode::read(): need <.",spynode);
	char * end=str;

	while (*str && *str!='<') str++;
	if (!*str) error("SpyNode::read(): need source text.",spynode);
	*end=0;
	str++;
	Text=str;
	while (*str && *str!='>') str++;
	if (!*str) error("SpyNode::read(): error SpyNode: need >.",spynode);
	*str=0;
	str++;

	while (*str && isascii(*str) && isspace(*str)) str++;
	if (*str=='{')
	{
		str++;
		while (*str && isascii(*str) && isspace(*str)) str++;
		if (!*str) error("SpyNode::read(): error SpyNode: need Title of prsrule.",spynode);
		Title=str;
		while (*str && *str!='}' && !(isascii(*str) && isspace(*str))) str++;
		if (!*str) error("SpyNode::read(): error SpyNode: need Title of prsrule.",spynode);
		end=str;
		while (*str && *str!='}') str++;
		if (!*str) error("SpyNode::read(): error SpyNode: need }.",spynode);
		*end=0;
		str++;
		while (*str && isascii(*str) && isspace(*str)) str++;
	}
}

int SpyNode::isNode(CplNode * node)
{
	if (strcmp(Label,node->getSrcLabel())) return NO;
	if (Title)
	{
		const char * r=node->get_Rule();
		while (*r!='{') r++; r++;
		while (isascii(*r) && isspace(*r)) r++;
		char * t=Title;
		while (*t) if (*t++!=*r++) return NO;
		if (*r!='}' && !(isascii(*r) && isspace(*r))) return NO;
	}
	char * bgnchar=node->getBgnChar();
	char * endchar=node->getEndChar();
	char * text=Text;
	while (bgnchar!=endchar) if (*bgnchar++!=*text++) return NO;
	if (*text) return NO;
	return YES;
}

void SpyList::free()
{
	SpyLink * link, * next;
	for (link=FstLink; link; link=next)
		{ next=link->Next; delete link->Node; delete link; }
	FstLink=LstLink=NULL;
}

void SpyList::add(SpyNode * node)
{
	SpyLink * link=new SpyLink;
	link->Node=node;
	link->Next=NULL;
	if (FstLink) { LstLink->Next=link; LstLink=link; }
	else FstLink=LstLink=link;
}

void SpyStruct::free()
{
	*SpyText=*SpyBuffer=0;
	SpyTitle=NULL;
	SpyTrnNode.free();
	SpyPrsList.free();
}

void SpyStruct::read(char * SpyFileName)
{
  fstream SpyFile;
  SpyFile.open(SpyFileName,ios::in);
  if (!SpyFile.is_open()) error("Translator::open(): SpyFile open error.");
  SpyPrsList.free();
  SpyTrnNode.free();
  if (!SpyFile.is_open()) return;
  do { SpyFile.getline(SpyText,SENTLENGTH); }
  while ( *SpyText && strncmp(SpyText,"==>",3) && strncmp(SpyText,"-->",3));
  if (!*SpyText) return;
  strcpy(SpyBuffer,SpyText);
  char * text=SpyBuffer;
  if (!strncmp(text,"==>",3))
    {
      text+=3;
      SpyTrnNode.read(text);
      SpyTitle=NULL;
    }
  else /* (!strncmp(text,"-->",3)) */
    {
      text+=3;
      while (*text && *text!='{') text++;
      if (!*text) error("Translator::SpyText(): need {.",SpyText);
      text++;
      while (*text && isascii(*text) && isspace(*text)) text++;
      if (!*text) error("Translator::SpyText(): need Title of prsrule.",SpyText);
      SpyTitle=text;
      while (*text && *text!='}' && !(isascii(*text) && isspace(*text))) text++;
      if (!*text) error("Translator::SpyText(): need Title of prsrule.",SpyText);
      char * end=text;
      while (*text && *text!='}') text++;
      if (!*text) error("Translator::SpyText(): need }.",SpyText);
      *end=0;
      text++;
      while (*text && isascii(*text) && isspace(*text)) text++;
      while (*text)
	{
	  SpyNode * node=new SpyNode;
	  node->read(text);
	  SpyPrsList.add(node);
	}
    }
  SpyFile.close();
}

#endif

