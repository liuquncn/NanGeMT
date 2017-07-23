#include "trans.h"
#include <strstream>
#include <stdlib.h>

int Translator::Opened;
int Translator::UseLexdbase;
int Translator::UseDictnref;
int Translator::UsePreEdit;
void (*Translator::UpdateStatus)() ;

void Translator::init()
{
	Status=CLOSED;
	Direction=NOACTION;
	TheSrcLang=-1;
	TheTgtLang=-1;
	TheModel=NULL;
	TheDictn=NULL;
	TheDictnref=NULL;
	ThePrsRbase=NULL;
	TheMakRbase=NULL;
}

void Translator::free()
{
	init();
	*SourceSentence=0;
	TheRootNode=NULL;
	TheUnity=NULL;
	TheResultNumber=0;
	TheMatchList.free();
	TheResultList.free();
	TheSftList.free();
	TheNodeUnityStack.free();
	TheMakStack.free();
	TheTrnStack.free();
	TheRootList.free();
	ThePrsList.free();
	TheTgtStruct.free();
	TheSrcStruct.free();
#ifdef KBMS
	closeSpy();
	//TheSpyStruct.free();
	//SpyFileName=WchFileName=NULL;
	//if (SpyFile.is_open()) SpyFile.close();
	//if (WchFile.is_open()) WchFile.close();
#endif
}

void Translator::reset()
{
	Status=ROPEN;
	Opened=1;
	UseDictnref=0;
	UseLexdbase=0;

	Stage=BEGIN;
	Stop=FALSE;
	Pause=FALSE;
	Direction=NOACTION;

	*SourceSentence=0;
	TheRootNode=NULL;
	TheUnity=NULL;
	TheResultNumber=0;
	TheMatchList.free();
	TheResultList.free();
	TheSftList.free();
	TheNodeUnityStack.free();
	TheMakStack.free();
	TheTrnStack.free();
	TheRootList.free();
	ThePrsList.free();
	TheTgtStruct.free();
	TheSrcStruct.free();
#ifdef KBMS
	closeSpy();
	//TheSpyStruct.free();
	//SpyFileName=WchFileName=NULL;
	//if (SpyFile.is_open()) SpyFile.close();
	//if (WchFile.is_open()) WchFile.close();
#endif
}

int Translator::translateSentence(int endstage)
{
  if (Stage==BEGIN) 
    Direction=PROGRESS;
  else 
    Direction=RETRIEVE;
  //Stage=BEGIN;
  while (TRUE)
    {
      /*
      if (Stop==TRUE) { translateSentenceBack(); return FALSE; }
      while (Pause==TRUE) _sleep(1000);
      */
      if (Direction==PROGRESS) 
	Stage++;
      else 
	Stage--;
      /*
      if (UpdateStatus) 
	(*UpdateStatus)();
      */
      if (Stage==BEGIN || Stage==endstage) break;
      //Stage++;
      //if (Stage==endstage) break;
      switch (Stage)
	{
	case CUTWORD0:   cutWord0();   break;
	case CUTWORD1:   cutWord1();   break;
	case PRSSENT:    prsSent();    break;
	case TRANSFORM0: transform0(); break;
	case TRANSFORM1: transform1(); break;
	case MAKSENT:    makSent();    break;
	case BLDWORD0:   bldWord0();   break;
	case BLDWORD1:   bldWord1();   break;
	case BLDWORD2:   bldWord2();   break;
	}
    }
  Direction=NOACTION;
  if (Stage==BEGIN) return FALSE;
  else /* Stage==END */ return SUCC;
}

void Translator::translateSentenceBack()
{
	if (Status!=ROPEN) 
		error("Translator::translateSentenceBack(): cannot translate while not open.");
	Stop=FALSE;
	if (Stage==BEGIN) return;
	if (Direction==PROGRESS) { Stage=Stage+1; Direction=RETRIEVE; }
	else if (Direction==NOACTION) { Direction=RETRIEVE; }
	while (TRUE)
	{
		Stage--;
		if (UpdateStatus) (*UpdateStatus)();
		if (Stage==BEGIN) break;
		switch (Stage)
		{
			case BLDWORD2:   bldWordBack2();   break;
			case BLDWORD1:   bldWordBack1();   break;
			case BLDWORD0:   bldWordBack0();   break;
			case MAKSENT:    makSentBack();    break;
			case TRANSFORM1: transformBack1(); break;
			case TRANSFORM0: transformBack0(); break;
			case PRSSENT:    prsSentBack();    break;
			case CUTWORD1:   cutWordBack1();   break;
			case CUTWORD0:   cutWordBack0();   break;
		}
	}
}

void Translator::setSourceSentence(char * text)
{
	/*
	if (Status!=ROPEN || Stage!=BEGIN)
		error("Translator::setSourceSentence(): unopen or Stage is not BEGIN.");
	*/

	strcpy(SourceSentence,text);
	TheResultNumber=0;
	TheResultList.free();
	preProcess();
}

void Translator::clrSourceSentence()
{
	if (Status==ROPEN && Stage==BEGIN) return;
	translateSentenceBack();
	TheResultNumber=0;
	TheResultList.free();
	TheMatchList.free();
#ifdef KBMS
	TheSpyStruct.free();
	if (WchFile.is_open()) WchFile.flush();
#endif
}

int Translator::makTargetSentence()
{
	ResultLink * link=TheResultList.GetHead();
	if (link==NULL) return FALSE;
	strcpy(TargetSentence,link->GetData()->getTargetSentence());
	strcpy(TargetTree,link->GetData()->getTargetTree());
	strcpy(SourceTree,link->GetData()->getSourceTree());
	TheResultList.Delete(link);
	postProcess(TargetSentence);
	return TRUE;
}

void Translator::makSourceSegment()
{
	char * p=SourceSegment;
	SrcSgmt * sgmt=TheSrcStruct.getSrcSgmtHead();
	for (;sgmt;sgmt=sgmt->getSucsSgmt())
	{
		char * q=p;
		SrcSect * sect=sgmt->getThisFstSect();
		for (;sect;sect=sect->getSameFstSect())
		{
			PtlNode * node=sect->getFstNode();
			int first=TRUE;
			for (;node;node=node->getSameSrcSectNode())
			{
				if (node->hasType()==COMPLETE && node->getRuleType()==MEANING
					&& (node->getAge()==ADULT || node->getAge()==YOUTH))
				{
					const char * scan;
					if (first)
					{
						first=FALSE;
						scan=sect->getBgnChar();
						while (*scan && scan<sect->getEndChar()) *p++=*scan++;
						*p++='/';
					}
					else *p++='-';
					scan=((CplNode *)node)->getSrcLabel();
					while (*scan) *p++=*scan++;
				}
			}
			if (p!=q) *p++=' ';
		}
	}
	*p++=0;
}
/*
int Translator::makSourceParsing() // similar with transform0()
{
	static PtlNode * Node;
	if (Node==NULL)
	{
		SrcSgmt * sgmthead=TheSrcStruct.getSrcSgmtHead();
		SrcSgmt * sgmttail=TheSrcStruct.getSrcSgmtTail();
		SrcSect * sect=sgmthead->getThisFstSect();
		while (sect && sect->getLstSrcSgmt()!=sgmttail)
			sect=sect->getSameFstSect();
		if (sect==NULL) return FAIL;
		Node=sect->getFstNode();
	}
	else Node=Node->getSameSrcSectNode();
	while (Node && Node->hasType()!=COMPLETE)
		Node=Node->getSameSrcSectNode();
	if (Node==NULL) return FAIL;
	((CplNode *)Node)->setFamily();
	OToken TText(TargetSentence);
	OToken TTree(TargetTree);
	OToken STree(SourceTree);
	makSourceParsingTree((CplNode *)Node,TText,TTree,STree);
	TText.outputNull();
	//TTree.output("分析树");
	TTree.outputNull();
	STree.outputNull();
	return SUCC;
}
*/

int Translator::makSourceParsing() // similar with transform0()
{
	static int Number;
	if (Number>=2) { Number=0; return FAIL; }
	if (Number==0) Number=TheRootList.getNumber();
	SrcRootLink * Link;
	OToken TText(TargetSentence);
	OToken TTree(TargetTree);
	OToken STree(SourceTree);
	for (Link=TheRootList.getSrcRootHead();Link;Link=Link->getSucs())
	{
		CplNode * Node=Link->getSrcNode();
		Node->setFamily();
		makSourceParsingTree((CplNode *)Node,TText,TTree,STree);
	}
	TText.outputNull();
	TTree.outputNull();
	STree.outputNull();
	if (Number==1)
	{
		PtlNode * Node=TheRootList.getSrcRootHead()->getSrcNode();
		Node=Node->getSameSrcSectNode();
		while (Node)
		{
			if (Node->hasType()==COMPLETE)
			{
				TheRootList.free();
				TheRootList.addSrcRootHead((CplNode *)Node);
				break;
			}
			Node=Node->getSameSrcSectNode();
		}
		if (Node==NULL) Number=2;
	}
	return SUCC;
}

void Translator::makSourceParsingTree(CplNode * node,
			  OToken &ttext,OToken &ttree,OToken &stree)
{
	if (node->getSrcHeadTag()==HEAD) { stree.output("!"); ttext.output("!"); }
	stree.output(node->getSrcLabel());
	ttext.output(node->getSrcLabel());
	if (node->getRuleType()==LCPRSRULE) stree.output("*");
	stree.output(":");
	char addr[16];
	//_itoa((int)node,addr,10);
	sprintf(addr,"%d",(int)node);
	stree.output(addr);
	char buffer[SENTLENGTH];
	strstream ostr(buffer,SENTLENGTH,ios::out);
	if (node->getLChild())
	{
		ostr << '\0';
		stree.output(buffer);
		stree.output("(");
		ttext.output("(");
		CplNode * child;
		for (child=node->getLChild();child;child=child->getRNeigh())
		{
			if (child->hasType()==SHIFTTO) continue;
			makSourceParsingTree(child,ttext,ttree,stree);
		}
		stree.output(")");
		ttext.output(")");
	}
	else
	{
		char text[WORDLENGTH], * t=text;
		const char * begin=node->getBgnChar(), *end=node->getEndChar();
		while (begin!=end) *t++=*begin++;
		*t=0;
		stree.output("<");
		stree.setBuffer(text);
		stree.setType(IDENT);
		stree.output();
		stree.output(">");
		ostr << '\0';
		stree.output(buffer);
		ttext.output("<");
		ttext.setBuffer(text);
		ttext.setType(IDENT);
		ttext.output();
		ttext.output(">");
		ostr << '\0';
		ttext.output(buffer);
		ttree.output(text);
		ttree.output("/");
		ttree.output(node->getSrcLabel());
		ttree.output(" ");
	}
}

void Translator::cutWord0()
{
	if (Stage!=CUTWORD0) return;
	if (Direction==RETRIEVE) { cutWordBack0(); return; }

#ifdef KBMS
	if (WchFile.is_open()) WchFile << "\n=== 原文 SourceSentence ==: " << SourceSentence << endl << endl;
#endif

#ifdef KBMS
	if (WchFile.is_open()) WchFile << "\n----------词法分析 cutWord----------\n";
#endif

	Unity::setDefaultSwitch(SEMSTA|SRCLEXSTA);

	char * text=SourceSentence;
	while (*text)
	{
		SrcSgmt * sgmt;
		if (isChnChar(text))
		{
			sgmt=new SrcSgmt;
			sgmt->setSgmtType(CHNCHAR);
			sgmt->setBgnChar(text);
			goNext(text);
			sgmt->setEndChar(text);
			TheSrcStruct.addSrcSgmtTail(sgmt);
		}
		else if (isLtnChar(text))
		{
			sgmt=new SrcSgmt;
			sgmt->setSgmtType(LTNCHAR);
			sgmt->setBgnChar(text);
			while (isLtnChar(text)) goNext(text);
			sgmt->setEndChar(text);
			TheSrcStruct.addSrcSgmtTail(sgmt);
		}
		else if (isDgtChar(text))
		{
			sgmt=new SrcSgmt;
			sgmt->setSgmtType(DGTCHAR);
			sgmt->setBgnChar(text);
			while (isDgtChar(text)) goNext(text);
			sgmt->setEndChar(text);
			TheSrcStruct.addSrcSgmtTail(sgmt);
		}
		else if (isPctChar(text))
		{
			sgmt=new SrcSgmt;
			sgmt->setSgmtType(PCTCHAR);
			sgmt->setBgnChar(text);
			goNext(text);
			sgmt->setEndChar(text);
			TheSrcStruct.addSrcSgmtTail(sgmt);
		}
		else if (isSpcChar(text)) goNext(text);
		else
		{
			sgmt=new SrcSgmt;
			sgmt->setSgmtType(OTHCHAR);
			sgmt->setBgnChar(text);
			goNext(text);
			sgmt->setEndChar(text);
			TheSrcStruct.addSrcSgmtTail(sgmt);
		}
	}

	SrcSgmt * head=TheSrcStruct.getSrcSgmtHead();
	SrcSgmt * tail=TheSrcStruct.getSrcSgmtTail();
	SrcSgmt * unmatchlist[SENTLENGTH];
	int unmatchindex=0;
	const char * leftlist ="[{(<“‘《〔〈「『〖【（［｛";
	const char * rightlist="]})>”’》〕〉」』〗】）］｝";
	SrcSgmt * sgmt;
	for (sgmt=head;sgmt;sgmt=sgmt->getSucsSgmt())
	{
		const char * leftpunct=leftlist;
		while (*leftpunct && !isEqual(leftpunct,sgmt->getBgnChar()))
			goNext(leftpunct);
		if (*leftpunct)
		{
			unmatchlist[unmatchindex++]=sgmt;
			continue;
		}
		const char * rightpunct=rightlist;
		while (*rightpunct && !isEqual(rightpunct,sgmt->getBgnChar())) goNext(rightpunct);
		if (*rightpunct)
		{
			int unmatchpoint;
			for (unmatchpoint=unmatchindex-1;unmatchpoint>=0;unmatchpoint--)
			{
				if (isEqual(unmatchlist[unmatchpoint]->getBgnChar(),leftlist+(rightpunct-rightlist)))
				{
					TheMatchList.add(unmatchlist[unmatchpoint],sgmt);
					unmatchindex=unmatchpoint;
					break;
				}
			}
			continue;
		}
	}
	for (sgmt=head;sgmt!=NULL;sgmt=sgmt->getSucsSgmt())
	{
		int TheLevel;
		const char * bgnchar=sgmt->getBgnChar();
		if (isEqual(bgnchar,"。") ||
			*bgnchar=='!' || isEqual(bgnchar,"！") ||
			*bgnchar=='?' || isEqual(bgnchar,"？"))
			TheLevel=90;
		else if (*bgnchar==':' || isEqual(bgnchar,"："))
			TheLevel=80;
		else if (*bgnchar==';' || isEqual(bgnchar,"；"))
			TheLevel=70;
		else if (*bgnchar==',' || isEqual(bgnchar,"，"))
			TheLevel=60;
		else TheLevel=0;
		if (sgmt->getLevel()<TheLevel) 
		  sgmt->setLevel(TheLevel);
		/*
		SrcSgmt * prev=sgmt->getPrevSgmt();
		if (prev && prev->getLevel()<TheLevel) prev->setLevel(TheLevel);
		*/
	}
	tail->setLevel(100);
	if (UsePreEdit)
	{
		SrcSgmtPairLink * link=TheMatchList.head();
		while (link)
		{
			if (*(link->getLeftSgmt()->getBgnChar())=='[')
				TheMatchList.shrink(link);
			link=link->getNext();
		}
		SrcSgmt * sgmt=TheSrcStruct.getSrcSgmtHead(), * next;
		while (sgmt)
		{
			next=sgmt->getSucsSgmt();
			if (*(sgmt->getBgnChar())=='[' || *(sgmt->getBgnChar())==']')
			{
				*(sgmt->getBgnChar())=' ';
				TheSrcStruct.delSrcSgmt(sgmt);
			}
			sgmt=next;
		}
	}
	if (TheSrcStruct.getSrcSgmtHead()==NULL) { cutWordBack0(); return; }
	Direction=PROGRESS;
}

void Translator::cutWordBack0()
{
	if (Stage!=CUTWORD0) return;
	TheMatchList.free();
	TheSrcStruct.freeSgmts();
	Direction=RETRIEVE;
}

void Translator::prsSent()
{
	if (Stage!=PRSSENT) return;
#ifdef KBMS
	if (WchFile.is_open()) WchFile << "\n----------句法分析 prsSent----------\n";
#endif

	Unity::setDefaultSwitch(SEMSTA|SRCLEX|SRCPHRSTA);

	if (Direction==PROGRESS)
	{
		PtlNode * ptlnode;
		while (ptlnode=ThePrsList.delNode())
		{
			if (ptlnode->getAge()!=YOUTH) continue;
			ptlnode->setAge(ADULT);
			prsNode2(ptlnode);
			if (ptlnode->hasType()==COMPLETE) prsNode1((CplNode *)ptlnode);
		}
	}

	if (findRootList()==SUCC)
		Direction=PROGRESS;
	else
		prsSentBack();
}

void Translator::prsSentBack()
{
	if (Stage!=PRSSENT) return;
	ThePrsList.free();
	TheNodeUnityStack.free();
	TheRootNode=NULL;
	Direction=RETRIEVE;
}

void Translator::transform0()
{
	if (Stage!=TRANSFORM0) return;

	const  int FAILURENUMBER=6;
	static int failurenumber;

	if (Direction==PROGRESS)
	{
		TheRootLink=TheRootList.getSrcRootTail();
		failurenumber=0;
	}
	else // Direction==RETRIEVE
	{
		int rootnumber=TheRootList.getNumber();
		if (rootnumber==1) TheRootNode->setAge(DEAD);
		if (TheRootLink->getTgtRootNumber()==0)
		{
			failurenumber++;
			TheRootNode->setAge(DEAD);
			if (alterRootList(TheRootLink,failurenumber,FAILURENUMBER/rootnumber)==FAIL)
			{
				failurenumber=0;
				if (TheRootList.getNumber()!=1) TheRootLink=splitRootList(TheRootLink);
				else TheRootLink=NULL;
			}
		}
		else failurenumber=0;
	}

	while (TheRootLink && TheRootLink->getTgtRootNumber()!=0)
		TheRootLink=TheRootLink->getPrev();
	if (TheRootLink==NULL)
	{
		transformBack0();
		return;
	}
	TheRootNode=TheRootLink->getSrcNode();
	useNode(TheRootNode);
	TheRootNode->setParent(NULL);
	initNodeUnity(TheRootNode);
#ifdef KBMS
	if (WchFile.is_open()) WchFile << "\n=== 源文根结点树 srcroottree ==";
	output_SrcRootNode(TheRootNode);
	if (WchFile.is_open()) WchFile << endl << endl;
#endif
	Direction=PROGRESS;
}

void Translator::transformBack0()
{
	if (Stage!=TRANSFORM0) return;
	Direction=RETRIEVE;
}

void Translator::transform1()
{
	if (Stage!=TRANSFORM1) return;
#ifdef KBMS
	if (WchFile.is_open()) WchFile << "\n----------词句转换 transform----------\n";
	WchTrnFail=NO;
#endif

	Unity::setDefaultSwitch(SEM|SRC);

	CplNode * node;
	const char * trnpart;
	const SftLink * sftbott;
	if (Direction==PROGRESS)
	{
		node=TheRootNode;
		trnpart=node->get_Scan();
	}
	else // Direction==RETRIEVE
	{
		if (TheTrnStack.pop(node,trnpart,sftbott)==FAIL)
			{ transformBack1(); return; }
		trnNodeBack(node,trnpart,sftbott);
	}

	while (TRUE)
	{
		if (node->getTransed()==UNTRANSED || node->getTransed()==SWITCHED ||
			node->getTransed()==SEMITRANSED)
		{
			sftbott=TheSftList.getLinkHead();
			if (trnNode(node,trnpart,sftbott)==FAIL)
			{
				if (TheTrnStack.pop(node,trnpart,sftbott)==FAIL)
					{ transformBack1(); return; }
				else { trnNodeBack(node,trnpart,sftbott); continue; }
			}
			TheTrnStack.push(node,trnpart,sftbott);
		}
		do { node=node->getForward(TheRootNode); }
		while (node && node->hasType()==SHIFTTO);
		if (!node)
		{
#ifdef KBMS
			if (WchFile.is_open()) WchFile << "\n=== 译文根结点树 tgtroottree ==";
			pushNodeUnity(TheRootNode);
			output_TgtRootNode(TheRootNode);
			popNodeUnity();
			if (WchFile.is_open()) WchFile << endl << endl;
#endif
			Direction=PROGRESS;
			return;
		}
		trnpart=node->get_Scan();
	}
}

void Translator::transformBack1()
{
	if (Stage!=TRANSFORM1) return;
	CplNode * node;
	const char * trnpart;
	const SftLink * sftbott;
	while (TheTrnStack.pop(node,trnpart,sftbott)==SUCC)
		{ trnTreeBack(node,sftbott); trnSwitchBack(node); }
	Direction=RETRIEVE;
}

void Translator::makSent()
{
	if (Stage!=MAKSENT) return;
#ifdef KBMS
	if (WchFile.is_open()) WchFile << "\n----------句法生成 makSent----------\n";
#endif

	Unity::setDefaultSwitch(SEM|SRC|TGTSTA);

	CplNode * node;
	RuleLink * rulelink;
	const char * rightpart;
	const SftLink * sftbott;
	if (Direction==PROGRESS)
		node=TheRootNode;
	else
	{
		do
		{
			if (TheMakStack.pop(node,rulelink,rightpart,sftbott)==FAIL)
				{ makSentBack(); return; }
			makNodeBack(node,rulelink,rightpart,sftbott);
		}
		while (makNode(node,rulelink,rightpart,sftbott)==FAIL);
#ifdef KBMS
		output_makNode(node,rulelink->getRule());
#endif
		TheMakStack.push(node,rulelink,rightpart,sftbott);
	}

	while (TRUE)
	{
		cRuleList * rulelist=&TheMakRbase->rd_MakRulesPT(node->getTgtLabel());
		rulelink=rulelist?rulelist->getRuleLinkHead():NULL;
		rightpart=NULL;
		sftbott=TheSftList.getLinkHead();
		if (makNode(node,rulelink,rightpart,sftbott)==FAIL)
		{
			do { node=node->getForward(TheRootNode); }
			while (node && node->hasType()==SHIFTTO);
			if (!node)
			{
				Direction=PROGRESS;
				return;
			}
		}
		else
		{
#ifdef KBMS
			output_makNode(node,rulelink->getRule());
#endif
			TheMakStack.push(node,rulelink,rightpart,sftbott);
		}
	}
}


void Translator::makSentBack()
{
	if (Stage!=MAKSENT) return;
	CplNode * node;
	RuleLink * rulelink;
	const char * rightpart;
	const SftLink * sftbott;
	while (TheMakStack.pop(node,rulelink,rightpart,sftbott)==SUCC)
		{ makRTreeBack(node,sftbott); makLTreeBack(node); }
	Direction=RETRIEVE;
}

void Translator::bldWord0()
{
	if (Stage!=BLDWORD0) return;
	if (Direction==RETRIEVE) { bldWordBack0(); return; }

#ifdef KBMS
	if (WchFile.is_open()) WchFile << "\n----------词法生成 bldWord----------\n";
#endif

	Unity::setDefaultSwitch(SEM|SRC|TGT);

	pushNodeUnity(TheRootNode);  /* for shift operation */

	CplNode * node;
	for (node=TheRootNode;node;node=node->getForward(TheRootNode))
	{
		if (node->getLChild()) continue;
		TgtSgmt * sgmt=new TgtSgmt;
		TheTgtStruct.addTgtSgmtTail(sgmt);
		sgmt->setNode(node);
		Attribute * attr=TheModel->getBaseForm(TheTgtLang);
		sgmt->set_FormPntr(getIntValue(node,attr)->getNucleus());
		const char * formpntr=sgmt->get_FormPntr();
		if (node->hasType()==COMPLETE && 
			(!formpntr || !*formpntr || *formpntr=='~' || strcmp(formpntr,"NONE")==0))
		{
#ifdef KBMS
			output_bldWordFail(node,formpntr);
#endif
			bldWordBack0();
			return;
		}
		CplNode * up, * down, * sfttemp;
		node->setFstTgtSgmt(sgmt);
		for (down=node,up=down->getParent();
			 up && down!=TheRootNode && up->getLChild()==down;
			 down=up,up=up->getParent()) up->setFstTgtSgmt(sgmt);
		node->setLstTgtSgmt(sgmt);
		for (down=node,up=down->getParent();
			 up && down!=TheRootNode && down->getRNeigh()==NULL;
			 down=up,up=up->getParent()) up->setLstTgtSgmt(sgmt);
		if (node->hasType()==SHIFTTO)
			for (sfttemp=node;sfttemp && sfttemp->getSftable();
				 sfttemp=sfttemp->getParent()) sfttemp->setSftable(NO);
	}
	for (node=TheRootNode;node;node=node->getForward(TheRootNode))
	{
		if (node->getSrcSect()==NULL) continue;
		if (node->getLChild()==NULL || node->getLChild()->getRNeigh()==NULL) continue;
		SrcSgmt * fstsrcsgmt=node->getFstSrcSgmt();
		SrcSgmt * lstsrcsgmt=node->getLstSrcSgmt();
		TgtSgmt * fsttgtsgmt=node->getFstTgtSgmt();
		TgtSgmt * lsttgtsgmt=node->getLstTgtSgmt();
		if (fstsrcsgmt==NULL || lstsrcsgmt==NULL) continue;
		SrcSgmtPairLink * link=TheMatchList.head();
		while (link)
		{
			if (link->getLeftSgmt()==fstsrcsgmt && link->getRightSgmt()==lstsrcsgmt &&
				(fstsrcsgmt->getSgmtType()!=PCTCHAR || lstsrcsgmt->getSgmtType()!=PCTCHAR))
			{
				TgtSgmt * leftsgmt=new TgtSgmt;
				leftsgmt->set_VaryForm(" [");
				TheTgtStruct.addTgtSgmtBefore(fsttgtsgmt,leftsgmt);
				TgtSgmt * rightsgmt=new TgtSgmt;
				rightsgmt->set_VaryForm(" ]");
				TheTgtStruct.addTgtSgmtAfter(lsttgtsgmt,rightsgmt);
			}
			link=link->getNext();
		}
		unsigned int fstlevel=fstsrcsgmt->getPrevSgmt()?fstsrcsgmt->getPrevSgmt()->getLevel():100;
		unsigned int lstlevel=lstsrcsgmt->getLevel();
		unsigned int minlevel=fstlevel<lstlevel?fstlevel:lstlevel;
		if (minlevel==0) continue;
		/*
		if (fsttgtsgmt->getPrevSgmt() && fsttgtsgmt->getPrevSgmt()->getLevel()<minlevel)
			fsttgtsgmt->getPrevSgmt()->setLevel(minlevel);
		*/
	}

	SftLink * sftlink;
	for (sftlink=TheSftList.getLinkHead();sftlink;sftlink=sftlink->getNext())
	{
		SftNode * sftnode=sftlink->getNode();
		CplNode * sftroot=sftnode->getSftRoot(), * sftfrom=NULL, * sfttemp;
		/* search the last suitable shift-from node */
		for (sfttemp=sftroot;sfttemp;sfttemp=sfttemp->getForward(sftroot))
		{
			if (!sfttemp->getPresent() || !sfttemp->getSftable()) continue;
			if (strcmp(sftnode->getTgtLabel(),sfttemp->getTgtLabel())) continue;
			UnityMark mark;
			recordUnity(mark);
			if (unifyNode(sftnode,sfttemp)==SUCC) sftfrom=sfttemp;
			recoverUnity(mark);
		}
		if (sftfrom)
		{
#ifdef KBMS
			output_sftWord(sftnode,sftfrom);
#endif
			unifyNode(sftnode,sftfrom);
		}
		else
		{
#ifdef KBMS
			output_sftWordFail(sftnode);
#endif
			bldWordBack0();
			return;
		}
		/* do the shift operation */
		int tag=sftnode->getSftTag();
		if (tag==MOVE || tag==REMV)
			{ sftfrom->setPresent(NO); sftfrom->setSftable(NO); }
		if (tag==MOVE || tag==COPY)
			{ sftnode->setPresent(YES); sftnode->setSftable(YES); }
		int present=(tag==COPY)?YES:NO;
		for (sfttemp=sftfrom->getLChild();sfttemp;
			 sfttemp=sfttemp->getForward(sftfrom))
				{ sfttemp->setSftable(NO); sfttemp->setPresent(present); }
		for (sfttemp=sftfrom->getParent();sfttemp && sfttemp->getSftable();
			 sfttemp=sfttemp->getParent())
				{ sfttemp->setSftable(NO); }
		sftnode->setSftFrom(sftfrom);
	}

	Direction=PROGRESS;
}

void Translator::bldWordBack0()
{
	if (Stage!=BLDWORD0) return;
	CplNode * node;
	for (node=TheRootNode;node;node=node->getForward(TheRootNode))
	{
		node->clrFstTgtSgmt();
		node->clrLstTgtSgmt();
		if (node->hasType()==COMPLETE)
			{ node->setPresent(YES); node->setSftable(YES); }
		else if (node->hasType()==SHIFTTO)
			{ node->setPresent(NO); node->setSftable(NO);
				((SftNode *)node)->setSftFrom(NULL); }
	}
	TheTgtStruct.free();
	popNodeUnity(); 	/* for shift operation */
	Direction=RETRIEVE;
}

void Translator::bldWord2()
{
	if (Stage!=BLDWORD2) return;
	if (Direction==RETRIEVE) { bldWordBack2(); return; }

#ifdef KBMS
	if (WchFile.is_open()) WchFile << "\n=== 译文 TargetSentence ==: " << TargetSentence << endl << endl;
#endif
	TheRootLink->addTgtRoot(TargetSentence);
	if (TheRootNode->getFstSrcSgmt()->getPrevSgmt()==NULL)
	{
		TheRootList.initialResult();
		Result result;
		while (TheRootList.composeResult(result))
		{
			TheRootList.forwardResult();
			TheResultNumber++;
			ResultLink * resultlink=TheResultList.AddTail(result);
			strcpy(TargetSentence,resultlink->GetData()->getTargetSentence());
			resultlink->GetData()->setTargetSentence(TargetSentence);
		}
	}
	else { bldWordBack2(); return; }
	Direction=PROGRESS;
}

void Translator::bldWordBack2()
{
	if (Stage!=BLDWORD2) return;
	Direction=RETRIEVE;
}

void Translator::submit(PtlNode * newnode)
{
	newnode->setAge(YOUTH);
	if (newnode->hasType()==PARTIAL)
		{ ThePrsList.addNode(newnode); return; }
	if (((CplNode *)newnode)->getSrcSect()==
		((CplNode *)newnode)->getLChild()->getSrcSect())
		{ ThePrsList.addNode(newnode); return; }
		/* the newnode has only one child */
	SrcSgmt * lstsgmt=newnode->getLstSrcSgmt();	 /* last segment   */
	PtlNode * refnode;
	for (refnode=newnode->getSrcSect()->getFstNode();refnode;
		 refnode=refnode->getSameSrcSectNode())
	{
		if (refnode==newnode) continue;
		if (refnode->hasType()==PARTIAL) continue;
		if (refnode->getAge()!=YOUTH && refnode->getAge()!=ADULT) continue;
		CplNode * deadnode=compare((CplNode *)newnode,(CplNode *)refnode);
#ifdef KBMS
		if (deadnode) output_killnode(deadnode);
#endif
		if (deadnode==newnode) newnode->setAge(DEAD);
		else if (deadnode==refnode) killnode(refnode);
	}
	if (newnode->getAge()==YOUTH) ThePrsList.addNode(newnode);
}

CplNode * Translator::compare(CplNode * newnode,CplNode * oldnode)
{
	int ruletype1=newnode->getRuleType();
	int ruletype2=oldnode->getRuleType();
	if (ruletype1==GLPRSRULE && ruletype2==LCPRSRULE)
		return newnode;
	else if (ruletype1==LCPRSRULE && ruletype2==GLPRSRULE)
		return oldnode;
	else return NULL;
}

void Translator::killnode(PtlNode * node)
{
	node->setAge(DEAD);
	if (node->hasType()==PARTIAL)
	{
		SrcSgmt * fstsgmt=node->getFstSrcSgmt();
		PtlNode * parent=fstsgmt->getThisFstNode();
		for (;parent!=node;parent=parent->getSameFstNode())
		{
			if (parent->getPtlChild()!=node) continue;
			if (parent->getAge()!=YOUTH && parent->getAge()!=ADULT) continue;
			killnode(parent);
#ifdef KBMS
			if (parent->hasType()==COMPLETE) output_killnode((CplNode *)parent);
#endif
		}
	}
	else
	{
		SrcSgmt * lstsgmt=node->getLstSrcSgmt();
		PtlNode * parent=lstsgmt->getThisLstNode();
		for (;parent!=node;parent=parent->getSameLstNode())
		{
			if (parent->getCplChild()!=node) continue;
			if (parent->getAge()!=YOUTH && parent->getAge()!=ADULT) continue;
			killnode(parent);
#ifdef KBMS
			if (parent->hasType()==COMPLETE) output_killnode((CplNode *)parent);
#endif
		}
	}
}

CplNode * Translator::createNode1(IToken &token,
						 const char * baseform, const char * varyform)
{
	CplNode * node=new CplNode;
	node->set_BaseForm(baseform);
	node->set_VaryForm(varyform);
	useNode(node);
	setSrcHerit(node,(CplNode *)NONE);
	node->pushVarList();
	node->setRuleType(MEANING);
	const char * prsrule=token.getInPntr();
	node->set_Rule(prsrule);
	goover(prsrule,"::");
	node->set_Bind(prsrule);
	token.input();	// skip **
	token.input();	// skip {
	token.input();	// skip title
	token.input();	// skip }
	if (prsReductn(node,token,token.getForeInPntr())==FAIL)
		error("Translator::createNode1(): Invalid Meaning: error SrcMeaning: ", token.getInPntr());
	char form[TOKENLENGTH];
	char * p=form; const char * q=baseform;
	*p++='"'; while (*q) { if (*q=='"') *p++='"'; *p++=*q++; } *p++='"'; *p=0;
	IToken tokenbase(form); tokenbase.input();
	if (unifySrcBaseForm(node,tokenbase)==FAIL)
		error("Translator::createNode1(): Invalid Meaning: error baseform: ",baseform);
	p=form; q=varyform;
	*p++='"'; while (*q) { if (*q=='"') *p++='"'; *p++=*q++; } *p++='"'; *p=0;
	IToken tokenvary(form); tokenvary.input();
	if (unifySrcVaryForm(node,tokenvary)==FAIL)
		error("Translator::createNode1(): Invalid Meaning: error varyform: ",varyform);
	while (!token.isDelim("||") &&!token.isDelim("=>") && !token.isDelim("$$") &&
		   !token.isDelim("**") && !token.isDelim("&&") && !token.isEmpty())
		token.input();
	if (token.isDelim("||") || token.isDelim("=>"))
		node->set_Scan(token.getInPntr());
	else
	{
		node->set_Scan(NULL);
		IToken token2(TheModel->getOthCat(TheTgtLang));
		token2.input();
		unifyTgtLabel(node,token2);
		/*
		char * p=form; const char * q=baseform;
		*p++='"'; while(*q) { if (*q=='"') *p++='"'; *p++=*q++; } *p++='"'; *p=0;
		IToken token4(form);
		token4.input();
		unifyTgtBaseForm(node,token4);
		*/
		node->setTransed(UNNEEDFUL);
	}
	return node;
}

CplNode * Translator::createNode2(SrcSgmt * bgnsgmt,SrcSgmt * endsgmt,
								  const char * srclabel,const char * tgtlabel)
{
	CplNode * node=new CplNode;
	useNode(node);
	setSrcHerit(node,(CplNode *)NONE);
	IToken token1(srclabel);
	token1.input();
	unifySrcLabel(node,token1);
	if (*token1) unifyNode(node,token1);
	IToken token2(tgtlabel);
	token2.input();
	unifyTgtLabel(node,token2);
	node->addIntoSect(bgnsgmt,endsgmt,TheMatchList);
	node->addIntoStruct();
	char form[TOKENLENGTH];
	char * p=form, *q=bgnsgmt->getBgnChar();
	*p++='"';
	if (GlobalTgtLang==MONGOLIAN) { *p++='{'; *p++='*'; }
	while (q!=endsgmt->getEndChar()) { if (*q=='"') *p++='"'; *p++=*q++; }
	if (GlobalTgtLang==MONGOLIAN) { *p++='*'; *p++='}'; }
	*p++='"';
	*p=0;
	IToken token3(form);
	token3.input();
	unifySrcBaseForm(node,token3);
	if (node->getSrcSect()->getNodeNum()!=1) strcpy(form,"_");
	IToken token4(form);
	token4.input();
	unifyTgtBaseForm(node,token4);
	node->setTransed(UNNEEDFUL);
	return node;
}

void Translator::prsNode1(CplNode * node)
{
	const char * pattern=node->getSrcLabel();
	cRuleList * rulelist=&ThePrsRbase->rd_PrsRulesPT(pattern);
	RuleLink * rulelink=rulelist?(rulelist->getRuleLinkHead()):NULL;
	while (rulelink)
	{
		PtlNode * newnode=prsTree1(node,rulelink->getRule(),GLPRSRULE);
		if (newnode)
		{
			// newnode->addIntoStruct();
			// submit(newnode);
			if (newnode->addIntoStruct()==SUCC) submit(newnode);
#ifdef KBMS
			else output_limitsametypenode(newnode);
#endif//KBMS
		}
		rulelink=rulelink->getNext();
	}
	rulelist=&TheDictn->rd_PrsRulesPT(pattern);
	rulelink=rulelist?(rulelist->getRuleLinkHead()):NULL;
	while (rulelink)
	{
		PtlNode * newnode=prsTree1(node,rulelink->getRule(),LCPRSRULE);
		if (newnode)
		{
			if (newnode->addIntoStruct()==SUCC) submit(newnode);
#ifdef KBMS
			else output_limitsametypenode(newnode);
#endif//KBMS
		}
		rulelink=rulelink->getNext();
	}
	if (UseDictnref==0) return;
	rulelist=&TheDictnref->rd_PrsRulesPT(pattern);
	rulelink=rulelist?(rulelist->getRuleLinkHead()):NULL;
	while (rulelink)
	{
		PtlNode * newnode=prsTree1(node,rulelink->getRule(),LCPRSRULE);
		if (newnode)
		{
			if (newnode->addIntoStruct()==SUCC) submit(newnode);
#ifdef KBMS
			else output_limitsametypenode(newnode);
#endif//KBMS
		}
		rulelink=rulelink->getNext();
	}
}

void Translator::prsNode2(PtlNode * node)
{
	if (node->hasType()==PARTIAL)
	{
		SrcSgmt * sucssgmt=node->getSucsSrcSgmt();
		if (sucssgmt)
		{
			PtlNode * auxnode=sucssgmt->getThisFstNode();
			while (auxnode && auxnode->getAge()!=ADULT)
				auxnode=auxnode->getSameFstNode();
			while (auxnode)
			{
				if (auxnode->hasType()==COMPLETE)
				{
					PtlNode * newnode=prsTree2((CplNode *)auxnode,node);
					if (newnode)
					{
						if (newnode->addIntoStruct()==SUCC) submit(newnode);
#ifdef KBMS
						else output_limitsametypenode(newnode);
#endif//KBMS
					}
				}
				do { auxnode=auxnode->getSameFstNode(); }
				while (auxnode && auxnode->getAge()!=ADULT);
			}
		}
	}
	else
	{
		SrcSgmt * prevsgmt=node->getPrevSrcSgmt();
		if (prevsgmt)
		{
			PtlNode * auxnode=prevsgmt->getThisLstNode();
			while (auxnode && auxnode->getAge()!=ADULT)
				auxnode=auxnode->getSameLstNode();
			while (auxnode)
			{
				if (auxnode->hasType()==PARTIAL)
				{
					PtlNode * newnode=prsTree2((CplNode *)node,auxnode);
					if (newnode)
					{
						if (newnode->addIntoStruct()==SUCC) submit(newnode);
#ifdef KBMS
						else output_limitsametypenode(newnode);
#endif//KBMS
					}
				}
				do { auxnode=auxnode->getSameLstNode(); }
				while (auxnode && auxnode->getAge()!=ADULT);
			}
		}
	}
}

int  Translator::findRootList()	/* Find a shortest RootList */
{
	TheRootList.free();
	SrcSgmt * bgnsgmt=TheSrcStruct.getSrcSgmtHead();
	SrcSgmt * endsgmt=TheSrcStruct.getSrcSgmtTail();
	NodeRouteList routelist;
	SrcSgmt * stopsgmt, * oversgmt, * lastsgmt=NULL;
	PtlNode * stopnode;
	NodeRouteLink * stopstep, * overstep;
	int laststepnum=0;
	for (stopsgmt=bgnsgmt;stopsgmt;stopsgmt=stopsgmt->getSucsSgmt())
		stopsgmt->setStepNum(0);
	for (stopnode=bgnsgmt->getThisFstNode();stopnode/* && lastsgmt!=endsgmt*/;
		 stopnode=stopnode->getSameFstNode())
	{
		oversgmt=stopnode->getLstSrcSgmt();
		if (stopnode->hasType()==PARTIAL) continue;
		if (stopnode->getAge()!=ADULT) continue;
		routelist.addTail((CplNode *)stopnode,NULL);
		if (!lastsgmt || oversgmt->getEndChar()>lastsgmt->getEndChar())
			{ laststepnum=1; lastsgmt=oversgmt; }
		oversgmt->setStepNum(1);
	}
	if (lastsgmt!=endsgmt && TheResultNumber!=0) return FAIL;
	stopstep=routelist.head();
	while (lastsgmt!=endsgmt)
	{
		if (!stopstep)
		{
			stopsgmt=lastsgmt?lastsgmt->getSucsSgmt():bgnsgmt;
			stopnode=createNode2(stopsgmt,stopsgmt,
								 TheModel->getOthCat(TheSrcLang),
								 TheModel->getOthCat(TheTgtLang));
			stopnode->setAge(ADULT);
			if (overstep=routelist.head())
			{
				for (;overstep;overstep=overstep->getNext())
				{
					if (overstep->getStepNum()!=laststepnum) continue;
					if (overstep->getNode()->getLstSrcSgmt()!=lastsgmt) continue;
					routelist.addTail((CplNode *)stopnode,overstep);
					if (!stopstep) stopstep=routelist.tail();
				}
			}
			else
			{
				routelist.addTail((CplNode *)stopnode,NULL);
				if (!stopstep) stopstep=routelist.tail();
			}
			lastsgmt=stopsgmt;
			laststepnum=stopstep->getStepNum();
			stopsgmt->setStepNum(laststepnum);
		}
		for (;stopstep && lastsgmt!=endsgmt;stopstep=stopstep->getNext())
		{
			stopsgmt=stopstep->getNode()->getSucsSrcSgmt();
			int stepnum=stopstep->getStepNum()+1;
			for (stopnode=stopsgmt->getThisFstNode();stopnode /*&& lastsgmt!=endsgmt*/;
				 stopnode=stopnode->getSameFstNode())
			{
				oversgmt=stopnode->getLstSrcSgmt();
				if (stopnode->hasType()==PARTIAL) continue;
				if (stopnode->getAge()!=ADULT) continue;
				int oldstepnum=oversgmt->getStepNum();
				if (oldstepnum && oldstepnum<stepnum) continue;
				routelist.addTail((CplNode *)stopnode,stopstep);
				if (!lastsgmt || oversgmt->getEndChar()>lastsgmt->getEndChar())
					{ laststepnum=stepnum; lastsgmt=oversgmt; }
				oversgmt->setStepNum(stepnum);
			}
		}
	}
	// use sentence node first
	NodeRouteLink * sentstep=NULL, * validstep=NULL;
	for (stopstep=routelist.head();stopstep;stopstep=stopstep->getNext())
	{
		if (stopstep->getNode()->getLstSrcSgmt()!=endsgmt) continue;
		if (validstep==NULL) validstep=stopstep;
		if (sentstep==NULL && isSrcSentence(stopstep->getNode()))
			{ sentstep=stopstep; break; }
	}
	// no sentence node can be use
	if (sentstep) stopstep=sentstep;
	else stopstep=validstep;
	for (;stopstep;stopstep=stopstep->getPrev())
		TheRootList.addSrcRootHead(stopstep->getNode());
#ifdef KBMS
	if (WchFile.is_open()) WchFile << "\n=== 源文根结点表 srcrootlist ==\n";
	output_RootList();
	if (WchFile.is_open()) WchFile << endl << endl;
#endif
	return SUCC;
}

// change the Node in the RootLink
int Translator::alterRootList(SrcRootLink * link, int failurenumber, int maxfailurenumber)
{
	if (link->getTgtRootNumber()!=0)
		error("Rootlist::altSrcRoot(): the TgtRootNumber of the SrcRootLink is not 0.");
	SrcSect * sect=link->getSrcNode()->getSrcSect();
	PtlNode * node, * sentnode=NULL, * validnode=NULL;
	if (failurenumber<maxfailurenumber)
	{
		for (node=sect->getFstNode();node;node=node->getSameSrcSectNode())
		{
			if (node->hasType()!=COMPLETE || node->getAge()!=ADULT) continue;
			if (validnode==NULL) validnode=node;
			if (sentnode==NULL && isSrcSentence((CplNode *)node))
				{ sentnode=node; break; }
		}
		if (sentnode) node=sentnode;
		else if (validnode) node=validnode;
		else return FAIL;
		TheRootList.altSrcRoot(link,(CplNode *)node);
		return SUCC;
	}
	else
	{
		for (node=sect->getFstNode();node;node=node->getSameSrcSectNode())
			if (node->hasType()==COMPLETE && node->getAge()==ADULT) node->setAge(DEAD);
		return FAIL;
	}
}

// Split a Node in the RootList
SrcRootLink * Translator::splitRootList(SrcRootLink * rootlink)
{
	SrcRootLink * rootlinkprev=rootlink->getPrev();
	SrcRootLink * rootlinksucs=rootlink->getSucs();
	CplNode * rootnode=rootlink->getSrcNode();
	CplNode * child=rootnode->getLChild();
	SrcSgmt * fstsgmt=rootnode->getFstSrcSgmt(),
			* lstsgmt=rootnode->getLstSrcSgmt();
	if (!child)
	{
		// find a substitute node
		const char * endchar=lstsgmt->getEndChar();
		PtlNode * ptlnode;
		for (ptlnode=fstsgmt->getThisFstNode();ptlnode;
			 ptlnode=ptlnode->getSameFstNode())
		{
			if (ptlnode->hasType()!=COMPLETE) continue;
			if (ptlnode->getAge()!=ADULT) continue;
			if (ptlnode->getLstSrcSgmt()==lstsgmt &&
				((CplNode *)ptlnode)->getLChild()==NULL)
				{ child=(CplNode *)ptlnode; child->setRNeigh(NULL); break; }
			if (ptlnode->getLstSrcSgmt()->getEndChar()<endchar) break;
		}
	}
	if (!child)
	{
		// create a new node
		child=createNode2(fstsgmt,lstsgmt,
						  TheModel->getOthCat(TheSrcLang),
						  TheModel->getOthCat(TheTgtLang));
		child->setAge(ADULT);
	}
	for (;child;child=child->getRNeigh())
		TheRootList.insertSrcRootBefore(rootlinksucs,child);
	TheRootList.deleteSrcRootAt(rootlink);
#ifdef KBMS
	if (WchFile.is_open()) WchFile << "\n=== 源文根结点表 srcrootlist ==\n";
	output_RootList();
	if (WchFile.is_open()) WchFile << endl << endl;
#endif
	return rootlinksucs?rootlinksucs->getPrev():TheRootList.getSrcRootTail();
}

int Translator::trnNode(CplNode * node, const char * &trnpart, const SftLink * sftbott)
{
#ifdef KBMS
	if (TheSpyStruct.getSpyTrnNode()->getLabel() &&
		TheSpyStruct.getSpyTrnNode()->isNode(node))
		node->setSpyLink((SpyLink *)NONE);
#endif
	if (node->getTransed()==SWITCHED)
	{
		while (trnpart && *trnpart)
		{
			IToken token(trnpart); token.input();
			if (token.isDelim("||")) break;
			if (trnTree(node,trnpart,sftbott)==SUCC) return SUCC;
		}
		trnSwitchBack(node);
	}
	while (trnSwitch(node,trnpart)==SUCC)
	{
		while (trnpart && *trnpart)
		{
			IToken token(trnpart); token.input();
			if (token.isDelim("||")) break;
			if (trnTree(node,trnpart,sftbott)==SUCC) return SUCC;
		}
		trnSwitchBack(node);
	}
#ifdef KBMS
	if (WchTrnFail==NO) { WchTrnFail=YES; output_trnNodeFail(node); }
#endif
	return FAIL;
}

void Translator::trnNodeBack(CplNode * node, const char * trnpart, const SftLink * sftbott)
{
	trnTreeBack(node,sftbott);
	if (trnpart && *trnpart)
	{
		IToken token(trnpart); token.input();
		if (!token.isDelim("||")) return;
	}
	trnSwitchBack(node);
}

int Translator::makNode(CplNode * node, RuleLink * &rulelink,
						const char * &rightpart, const SftLink * sftbott)
{
	if (rightpart && *rightpart)
		{ if (makNode1(node,rulelink,rightpart,sftbott)==SUCC) return SUCC; }
	while (rulelink)
	{
		if (makNode0(node,rulelink,rightpart)==SUCC)
			{ if (makNode1(node,rulelink,rightpart,sftbott)==SUCC) return SUCC; }
	}
	return FAIL;
}

void Translator::makNodeBack(CplNode * node, RuleLink * &rulelink, const char * rightpart, const SftLink * sftbott)
{
	makRTreeBack(node,sftbott);
	if (!rightpart || !*rightpart)
		{ makLTreeBack(node); rulelink=rulelink->getNext(); }
}

int Translator::makNode0(CplNode * node, RuleLink * &rulelink, const char * &rightpart)
{
	while (rulelink)
	{
		if (makLTree(node,rulelink->getRule(),rightpart)==SUCC) break;
		rulelink=rulelink->getNext();
	}
	if (rulelink) return SUCC;
	else return FAIL;
}

int Translator::makNode1(CplNode * node, RuleLink * &rulelink, const char * &rightpart, const SftLink * sftbott)
{
	while (rightpart && *rightpart)
		if (makRTree(node,rightpart,sftbott)==SUCC) return SUCC;
	makLTreeBack(node);
	rulelink=rulelink->getNext();
	return FAIL;
}

int Translator::prsReductn(CplNode *rootnode, IToken &token, const char *prsbind)
{
	if (rootnode->getSrcLabel() || !rootnode->getFest()->isEmpty())
		error("Translator::prsReductn(): on a node with non-empty Fest.");
	if (unifySrcLabel(rootnode,token)==FAIL) return FAIL;
	IToken bindtoken(prsbind);
	bindtoken.input();
	if (unifyCplBind(rootnode,bindtoken)==FAIL)
	{
#ifdef KBMS
		if (rootnode->getSpyLink()) output_spyBind(rootnode,bindtoken);
#endif
		return FAIL;
	}
	if (rootnode->getRuleType() & PRSRULE)
	{
		PtlNode * ptlnode=rootnode;
		while (ptlnode)
		{
			ptlnode->getCplChild()->setParent(rootnode);
			ptlnode=ptlnode->getPtlChild();
		}
	}
	return SUCC;
}

PtlNode * Translator::prsTree1(CplNode *curnode, const char * prsrule, int ruletype)
{
	const char * prsbind=prsrule;
	goover(prsbind,"::");
	IToken token(prsrule);
	token.goover("->");
	PtlNode * ptlnode=new PtlNode;
	ptlnode->pushVarList();
	ptlnode->setRuleType(ruletype);
	ptlnode->setPtlChild(NULL);
	ptlnode->setCplChild(curnode);
	ptlnode->set_Rule(prsrule);
	ptlnode->set_Bind(prsbind);
	ptlnode->addIntoSect(curnode->getFstSrcSgmt(),curnode->getLstSrcSgmt(),TheMatchList);
	if (ptlnode->getSrcSect()->getValid()==FALSE)
		{ delete ptlnode; return NULL; }
	useNode(ptlnode);
#ifdef KBMS
	SpyLink * spylink=TheSpyStruct.getSpyPrsList().getFstLink();
	if (spylink && spylink->getNode()->isNode(curnode))
	{
		char prstitle[TITLELENGTH+1];
		PrsRbase::getTitle(prsrule,prstitle);
		if (!strncmp(prstitle,TheSpyStruct.getSpyTitle(),TITLELENGTH))
			ptlnode->setSpyLink(spylink);
	}
#endif
	try
	{
		if (prsTree0(ptlnode,token,prsbind)==FAIL)
		{
			delete ptlnode;
			return NULL;
		}
	}
	catch (...) { delete ptlnode; throw; }
	if (token.getType()==IDENT||token.isDelim("!"))
	{
		ptlnode->set_Scan(token.getInPntr());
#ifdef KBMS
//		output_prsPtlNode(ptlnode);
#endif
		return ptlnode;
	}
	CplNode * cplnode=new CplNode(ptlnode);
	if (cplnode->getSrcHead())
		setSrcHerit(cplnode,cplnode->getSrcHead());
	else setSrcHerit(cplnode,(CplNode *)NONE);
	delete ptlnode;
	IToken tokenrdn(prsrule);
	tokenrdn.goover("}");
	try
	{
		if (prsReductn(cplnode,tokenrdn,prsbind)==FAIL)
			{ delete cplnode; return NULL; }
	}
	catch (...) { delete cplnode; throw; }
	if (cplnode->get_Rule()==curnode->get_Rule())
		error("Translator::prsTree1(): PrsRule causing dead loop.",curnode->get_Rule());
	while ( !token.isDelim("||") && !token.isDelim("=>")
		 && !token.isDelim("$$") && !token.isDelim("**")
		 && !token.isDelim("&&") && !token.isEmpty() ) token.input();
	if (token.isDelim("||") || token.isDelim("=>"))
		cplnode->set_Scan(token.getInPntr());
	else cplnode->set_Scan(NULL);
	const char * trnpart=cplnode->get_Scan();
	TheRootNode=cplnode;
	initNodeUnity(TheRootNode);
	try
	{
		if (trnSwitch(cplnode,trnpart)==FAIL)
			{ delete cplnode; return NULL; }
	}
	catch (...) { delete cplnode; throw; }
	trnSwitchBack(cplnode);
	TheRootNode=NULL;
#ifdef KBMS
	output_prsCplNode(cplnode);
#endif
	return cplnode;
}

PtlNode * Translator::prsTree2(CplNode *curnode, PtlNode * auxnode)
{
	const char * prsbind=auxnode->get_Bind();
	IToken token(auxnode->get_Scan()); token.input();
	PtlNode * ptlnode=new PtlNode;
	ptlnode->pushVarList();
	ptlnode->setRuleType(auxnode->getRuleType());
	ptlnode->set_Rule(auxnode->get_Rule());
	ptlnode->set_Bind(auxnode->get_Bind());
	ptlnode->setPtlChild(auxnode);
	ptlnode->setCplChild(curnode);
	ptlnode->addIntoSect(auxnode->getFstSrcSgmt(),curnode->getLstSrcSgmt(),TheMatchList);
	if (ptlnode->getSrcSect()->getValid()==FALSE)
		{ delete ptlnode; return NULL; }
	useNode(ptlnode);
#ifdef KBMS
	SpyLink * spylink=auxnode->getSpyLink(), * spynext;
	if (spylink) spynext=spylink->getNext(); else spynext=NULL;
	if (spynext && spynext->getNode()->isNode(curnode))
		ptlnode->setSpyLink(spynext);
#endif
	try
	{
		if (prsTree0(ptlnode,token,prsbind)==FAIL)
		{
			delete ptlnode;
			return NULL;
		}
	}
	catch (...) { delete ptlnode; throw; }
	if ( token.isIdent()||token.isDelim("!") )
	{
		ptlnode->set_Scan(token.getInPntr());
#ifdef KBMS
//		output_prsPtlNode(ptlnode);
#endif
		return ptlnode;
	}
	CplNode * cplnode=new CplNode(ptlnode);
	if (cplnode->getSrcHead())
		setSrcHerit(cplnode,cplnode->getSrcHead());
	else setSrcHerit(cplnode,(CplNode *)NONE);
	delete ptlnode;
	IToken tokenrdn(auxnode->get_Rule());
	tokenrdn.goover("}");
	try
	{
		if (prsReductn(cplnode,tokenrdn,prsbind)==FAIL)
			{ delete cplnode; return NULL; }
	}
	catch (...) { delete cplnode; throw; }
	while ( !token.isDelim("||") && !token.isDelim("=>")
		 && !token.isDelim("$$") && !token.isDelim("**")
		 && !token.isDelim("&&") && !token.isEmpty() ) token.input();
	if (token.isDelim("||") || token.isDelim("=>"))
		cplnode->set_Scan(token.getInPntr());
	else cplnode->set_Scan(NULL);
	const char * trnpart=cplnode->get_Scan();
	TheRootNode=cplnode;
	initNodeUnity(TheRootNode);
	try
	{
		if (trnSwitch(cplnode,trnpart)==FAIL)
			{ delete cplnode; return NULL; }
	}
	catch (...) { delete cplnode; throw; }
	trnSwitchBack(cplnode);
	TheRootNode=NULL;
#ifdef KBMS
	output_prsCplNode(cplnode);
#endif
	return cplnode;
}

int Translator::prsTree0(PtlNode *rootnode, IToken &token, const char * prsbind)
{
	if (token.isDelim("^"))
	{
		if (rootnode->getPrevSrcSgmt() &&
			rootnode->getPrevSrcSgmt()->getSgmtType()!=PCTCHAR)
			return FAIL;
		token.input();
	}
	int tag;
	if (token.isDelim("!")) { tag=HEAD; token.input(); }
	else tag=NTAG;
	rootnode->setChildSrcHeadTag(tag);
	rootnode->getCplChild()->setSrcHeadTag(tag);
	if (strcmp(token,rootnode->getCplChild()->getSrcLabel())) return FAIL;
	useNode(rootnode);
	Variable * var=rootnode->addVar(token,tag)->getVar();
	rootnode->addVarNode(var,rootnode->getCplChild());
	token.input();
	if (!token.isDelim("(")) rootnode->addVarLeaf(var);
	if (prsForest(rootnode,rootnode->getCplChild(),token,prsbind)==FAIL)
		return FAIL;
	IToken bindtoken(prsbind);
	bindtoken.input();
	if (unifyPtlBind(rootnode,bindtoken)==FAIL)
	{
#ifdef KBMS
		if (rootnode->getSpyLink()) output_spyBind(rootnode,bindtoken);
#endif
		return FAIL;
	}
	if (rootnode->getSucsSrcSgmt()==NULL &&
		(token.isDelim("!") || token.isIdent()))
		return FAIL;
	return SUCC;
}

int Translator::unifySrcLabel(CplNode * node, IToken & token)
{
	if (node->getSrcLabel())
	{
		if (token.isIdent(node->getSrcLabel()))
			{ token.input(); return SUCC; }
		else return FAIL; 
	}
	Attribute * phrcatatt=TheModel->getPhrCat(TheSrcLang);
	Attribute * lexcatatt=TheModel->getLexCat(TheSrcLang);
	Value * value;
	if (value=phrcatatt->getValList().find(token))
	{
		Atom * phrcatval=getIntValue(node,phrcatatt);
		if (TheUnity->unify(phrcatval,token)==FAIL) return FAIL;
		node->setSrcLabel(value->getString());
	}
	else if (value=lexcatatt->getValList().find(token))
	{
		Atom * lexcatval=getIntValue(node,lexcatatt);
		if (TheUnity->unify(lexcatval,token)==FAIL) return FAIL;
		node->setSrcLabel(value->getString());
	}
	else error("Translator::unifySrcLabel(): Invalid SrcLabel: ",token);
	return SUCC;
}

int Translator::unifyTgtLabel(CplNode * node, IToken & token)
{
	if (node->getTgtLabel())
	{
		if (token.isIdent(node->getTgtLabel()))
			{ token.input(); return SUCC; }
		else return FAIL;
	}
	Attribute * phrcatatt=TheModel->getPhrCat(TheTgtLang);
	Attribute * lexcatatt=TheModel->getLexCat(TheTgtLang);
	Value * value;
	if (value=phrcatatt->getValList().find(token))
	{
		Atom * phrcatval=getIntValue(node,phrcatatt);
		if (TheUnity->unify(phrcatval,token)==FAIL) return FAIL;
		node->setTgtLabel(value->getString());
	}
	else if (value=lexcatatt->getValList().find(token))
	{
		Atom * lexcatval=getIntValue(node,lexcatatt);
		if (TheUnity->unify(lexcatval,token)==FAIL) return FAIL;
		node->setTgtLabel(value->getString());
	}
	else error("Translator::unifyTgtLabel(): Invalid TgtLabel: ",token);
	return SUCC;
}

int Translator::unifySrcBaseForm(CplNode * node, IToken & token)
{
	Attribute * baseformatt=TheModel->getBaseForm(TheSrcLang);
	Atom * baseformval=getIntValue(node,baseformatt);
	return TheUnity->unify(baseformval,token);
}

int Translator::unifySrcVaryForm(CplNode * node, IToken & token)
{
	Attribute * varyformatt=TheModel->getVaryForm(TheSrcLang);
	Atom * varyformval=getIntValue(node,varyformatt);
	return TheUnity->unify(varyformval,token);
}

int Translator::unifyTgtBaseForm(CplNode * node, IToken & token)
{
	Attribute * baseformatt=TheModel->getBaseForm(TheTgtLang);
	Atom * baseformval=getIntValue(node,baseformatt);
	return TheUnity->unify(baseformval,token);
}

int Translator::unifyTgtVaryForm(CplNode * node, IToken & token)
{
	Attribute * varyformatt=TheModel->getVaryForm(TheTgtLang);
	Atom * varyformval=getIntValue(node,varyformatt);
	return TheUnity->unify(varyformval,token);
}

int Translator::unifyPtlBind(PtlNode * rootnode, IToken & token1)
{
	while (isBind(token1))
	{
		IToken token2=token1;
		if (ignorePtlBind(rootnode,token1)) continue;
		token1=token2;
		if (doBind(rootnode,token1)==FAIL)
			{ token1=token2; return FAIL; }
	}
	return SUCC;
}

int Translator::unifyCplBind(CplNode * rootnode, IToken & token1)
{
	while (isBind(token1))
	{
		IToken token2=token1;
		if (ignoreCplBind(rootnode,token1)) continue;
		token1=token2;
		if (doBind(rootnode,token1)==FAIL)
			{ token1=token2; return FAIL; }
	}
	return SUCC;
}

int Translator::unifyBind(CplNode * rootnode, IToken & token)
{
	while (isBind(token))
	{
#ifdef KBMS
		IToken oldtoken=token;
#endif
		if (doBind(rootnode,token)==FAIL)
		{
#ifdef KBMS
			token=oldtoken;
#endif
			return FAIL;
		}
	}
	return SUCC;
}

const static int NVAR=-2,PAST=-1,PRES=0,FUTR=1;

int Translator::ignorePtlBind(PtlNode * rootnode, IToken & token)
{
	int retval;
	if (!token.isIdent("IF"))
	{
		int stat1,stat2,here;
		Variable var,var2;
		var.inputFrom(token);
		if (var.isRoot()) stat1=FUTR;
		else
		{
			if (!rootnode->srhVarNode(&var,here)) stat1=FUTR;
			else if (here) stat1=PRES; else stat1=PAST;
		}
		while (!token.isDelim("=") && !token.isDelim("!=") &&
			   !token.isDelim("==")) token.input();
		token.input();
		if (Atom::leadBy(token) || Fest::leadBy(token)) stat2=NVAR;
		else
		{
			var2.inputFrom(token);
			if (var2.isRoot()) stat2=FUTR;
			else
			{
				if (!rootnode->srhVarNode(&var2,here)) stat2=FUTR;
				else if (here) stat2=PRES; else stat2=PAST;
			}
		}
		if (stat1==FUTR || stat2==FUTR) retval=FUTR;
		else if (stat1==PRES || stat2==PRES) retval=PRES;
		else retval=PAST;
		if (stat2==NVAR && Atom::leadBy(token)) Atom::skipOver(token);
		else if (stat2==NVAR && Fest::leadBy(token)) Fest::skipOver(token);
		else while (token.isDelim(".") || token.isDelim("@"))
				{ token.input(); token.input(); }
	}
	else
	{
		retval=PAST;
		token.input();
		while (TRUE)
		{
			if (retval==FUTR)
			{
				while (!token.isIdent("TRUE") && !token.isIdent("FALSE") &&
					   !token.isIdent("ENDIF"))
				{
					if (isBind(token)) skipBind(token);
					else if (token.isIdent("THEN")||token.isIdent("ELSE"))
						token.input();
					else error("Translator::ignorePtlBind(): illegal Bind.",token.getInPntr());
				}
				token.input();
				break;
			}
			if (token.isIdent("FALSE") || token.isIdent("TRUE") ||
				token.isIdent("ENDIF"))
				{ token.input(); break; }
			if (token.isIdent("THEN") || token.isIdent("ELSE"))
				{ token.input(); continue; }
			int stat2=ignorePtlBind(rootnode,token);
			if (stat2==FUTR) retval=FUTR;
			else if (stat2==PRES) retval=PRES;
		}
	}
	if (token.isDelim(",")) token.input();
	return retval;
}

int Translator::ignoreCplBind(CplNode * rootnode, IToken & token)
{
	int retval;
	if (!token.isIdent("IF"))
	{
		int stat1,stat2,here;
		Variable var,var2;
		var.inputFrom(token);
		if (var.isRoot()) stat1=PRES;
		else
		{
			if (rootnode->srhVarNode(&var,here)) stat1=PAST;
			else error("Translator::ignoreCplBind(): Variable not found.",rootnode->get_Rule());
		}
		while (!token.isDelim("=") && !token.isDelim("!=") &&
			   !token.isDelim("==")) token.input();
		token.input();
		if (Atom::leadBy(token) || Fest::leadBy(token)) stat2=NVAR;
		else
		{
			var2.inputFrom(token);
			if (var2.isRoot()) stat2=PRES;
			else
			{
				if (rootnode->srhVarNode(&var2,here)) stat2=PAST;
				else error("Translator::ignoreCplBind(): Variable not found.",rootnode->get_Rule());
			}
		}
		if (stat1==PRES || stat2==PRES) retval=PRES;
		else retval=PAST;
		if (stat2==NVAR && Atom::leadBy(token)) Atom::skipOver(token);
		else if (stat2==NVAR && Fest::leadBy(token)) Fest::skipOver(token);
		else while (token.isDelim(".") || token.isDelim("@"))
				{ token.input(); token.input(); }
	}
	else
	{
		retval=PAST;
		token.input();
		while (TRUE)
		{
			if (token.isIdent("FALSE") || token.isIdent("TRUE") ||
				token.isIdent("ENDIF")) { token.input(); break; }
			if (token.isIdent("THEN") || token.isIdent("ELSE"))
				{ token.input(); continue; }
			int stat2=ignoreCplBind(rootnode,token);
			if (stat2==PRES) retval=PRES;
		}
	}
	if (token.isDelim(",")) token.input();
	return retval;
}

int Translator::isBind(IToken & token)
{
	return Variable::leadBy(token) || token.isIdent("IF");
}

int Translator::doBind(PtlNode * rootnode,IToken & token)
{
	int retval=SUCC;
	if (!token.isIdent("IF"))
	{
		Variable var,var2;
		CplNode * node, *node2;
		var.inputFrom(token);
		node=rootnode->srhNode(&var);
		IToken token2(token);
		while (!token2.isDelim("=") && !token2.isDelim("!=") &&
			   !token2.isDelim("==")) token2.input();
		token2.input();
		if (Variable::leadBy(token2))
		{
			var2.inputFrom(token2);
			node2=rootnode->srhNode(&var2);
			retval=unifyNode(node,token,node2,token2);
		}
		else retval=unifyNode(node,token,token2);
		token=token2;
	}
	else
	{
		int test=SUCC;
		token.input();
		UnityMark mark;
		recordUnity(mark);
		while (test && isBind(token)) test=doBind(rootnode,token);
		if (!test) while (isBind(token)) skipBind(token);
		/* if(test==FAIL) */ recoverUnity(mark);
		if (token.isIdent("TRUE")) retval=test;
		if (token.isIdent("FALSE")) retval=test?FAIL:SUCC;
		if (token.isIdent("THEN"))
		{
			token.input();
			if (test==SUCC)
				while (retval && isBind(token)) retval=doBind(rootnode,token);
			if (test==FAIL || !retval) while (isBind(token)) skipBind(token);
		}
		if (token.isIdent("ELSE"))
		{
			token.input();
			if (test==FAIL)
				while (retval && isBind(token)) retval=doBind(rootnode,token);
			if (test==SUCC || !retval) while (isBind(token)) skipBind(token);
		}
		token.input();
	}
	if (token.isDelim(",")) token.input();
	return retval;
}

void Translator::skipBind(IToken & token)
{
	if (!token.isIdent("IF"))
	{
		while (!token.isDelim("=") && !token.isDelim("!=") &&
			   !token.isDelim("==")) token.input();
		token.input();
		if (Variable::leadBy(token))
		{
			Variable::skipOver(token); 
			while (token.isDelim(".")||token.isDelim("@"))
				{ token.input(); token.input(); }
		}
		else if (Fest::leadBy(token)) Fest::skipOver(token);
		else if (Atom::leadBy(token)) Atom::skipOver(token);
		else error("Translator::skipBind(): illegal Bind.",token.getInPntr());
	}
	else
	{
		token.input();
		while (!token.isIdent("TRUE") && !token.isIdent("FALSE") &&
			   !token.isIdent("ENDIF"))
		{
			if (isBind(token)) skipBind(token);
			else if (token.isIdent("THEN")||token.isIdent("ELSE")) token.input();
			else error("Translator::skipBind(): illegal Bind.",token.getInPntr());
		}
		token.input();
	}
	if (token.isDelim(",")) token.input();
}

#ifdef KBMS


void Translator::output_spyBind(PtlNode * node,IToken & itoken)
{
	if (!WchFile.is_open()) return;
	const char * bind=itoken.getInPntr();
	skipBind(itoken);
	const char * end=itoken.getInPntr();
	if (node->getSpyLink()) WchFile << "合一失败: " << TheSpyStruct.getSpyText() << endl;
	WchFile << "	失败原因: ";
	IToken token(bind); token.input();
	int isatt=0, isval=0, brace=0;
	cValList * vallist;
	for ( ; token.getInPntr()<end; token.input() )
	{
		if (token.isIdent("THEN") || token.isIdent("ELSE") ||
			token.isIdent("TRUE") || token.isIdent("FALSE") ||
			token.isIdent("ENDIF") || token.isDelim("@") )
			WchFile << " " << token << " ";
		else if (isatt && token.isIdent())
		{
			Attribute * attrb=TheModel->getAttribute(token), * refer;
			vallist=&attrb->getValList();
			if (refer=attrb->getRefer()) attrb=refer;
			WchFile << attrb->getString();
		}
		else if (isval && (token.isIdent() || token.isInteg()))
		{
			Value * value, * refer;
			if (vallist) value=vallist->find(token); else value=NULL;
			if (value && (refer=value->getRefer())) value=refer;
			if (value) WchFile << value->getString(); else WchFile << token;
		}
		else WchFile << token;
		if (token.isIdent("IF") || token.isIdent("THEN") ||
			token.isIdent("ELSE")) WchFile << " ";
		if (token.isDelim("[") || token.isDelim("{")) brace++;
		if (token.isDelim("]") || token.isDelim("}")) brace--;
		if (brace)
		{
			if (token.isDelim("[") || token.isDelim("{") || token.isDelim(",") )
				{ isatt=1; isval=0; }
			else if (token.isDelim(":")) { isatt=0; isval=1; }
		}
		else
		{
			if (token.isDelim(".") || token.isDelim("@")) { isatt=1; isval=0; }
			else if (token.isDelim("=") || token.isDelim("==") ||
					 token.isDelim("!=")) { isatt=0; isval=1; }
			else if (token.isDelim(",") || *token=='%') { isval=0; isatt=0; }
		}
	}
	WchFile << endl;
}

void Translator::output_killnode(CplNode * node)
{
	if (!WchFile.is_open()) return;
	WchFile << hex << "屏蔽结点(" << (int)node << ") "
				<< node->getSrcLabel() << ": ";
	if (node->getSrcSect())
	{
		const char * begin=node->getBgnChar(), *end=node->getEndChar();
		while (begin!=end) WchFile << *begin++;
	}
	WchFile << endl;
}

void Translator::output_RootList()
{
	if (!WchFile.is_open()) return;
	class SrcRootLink * link;
	for (link=TheRootList.getSrcRootHead();link;link=link->getSucs())
	{
		CplNode * node=link->getSrcNode();
		char title[TITLELENGTH+1];
		const char * prsrule=node->get_Rule();
		if (prsrule) PrsRbase::getTitle(prsrule,title);
		else *title=0;
		WchFile << hex << (int)node << "\t{";
		char * t=title;
		while (*t) WchFile << *t++;
		WchFile << "}:";
 		int i; for (i=TITLELENGTH-(t-title);i>0;i--) WchFile << ' ';
		WchFile << node->getSrcLabel() << "<";
		const char * begin=node->getBgnChar(), *end=node->getEndChar();
		while (begin!=end) WchFile << *begin++;
		WchFile << ">" << endl;
	}
}

void Translator::output_SrcRootNode(CplNode * node,int depth)
{
	if (!WchFile.is_open()) return;
	char title[TITLELENGTH+1];
	const char * prsrule=node->get_Rule();
	if (prsrule) PrsRbase::getTitle(prsrule,title);
	else *title=0;
	WchFile << '\n';
	WchFile << hex << (int)node << "\t{";
	char * t=title;
	while (*t) WchFile << *t++;
	WchFile << "}:";
 	int i;
	for (i=TITLELENGTH-(t-title);i>0;i--) WchFile << ' ';
	for (i=1;i<depth;i++) WchFile << '\t';
	if (depth==1) WchFile << '\t';
	else if (node->getSrcHeadTag()==HEAD) WchFile << "---!";
	else WchFile << "----";
	WchFile << node->getSrcLabel();
	CplNode * child;
	if (child=node->getLChild())
	{
		while (child)
		{
			output_SrcRootNode(child,depth+1);
			child=child->getRNeigh();
		}
	}
	else
	{
		WchFile << "<";
		const char * begin=node->getBgnChar(), *end=node->getEndChar();
		while (begin!=end) WchFile << *begin++;
		WchFile << ">";
	}
}

void Translator::output_TgtRootNode(CplNode * node,int depth)
{
	if (!WchFile.is_open()) return;
	char title[TITLELENGTH+1];
	const char * prsrule=node->get_Rule();
	if (prsrule) PrsRbase::getTitle(prsrule,title);
	else *title=0;
	WchFile << '\n';
	WchFile << hex << (int)node << "\t{";
	char * t=title;
	while (*t) WchFile << *t++;
	WchFile << "}:";
	int i;
	for (i=TITLELENGTH-(t-title);i>0;i--) WchFile << ' ';
	for (i=1;i<depth;i++) WchFile << '\t';
	if (depth==1) WchFile << '\t';
	else if (node->getTgtHeadTag()==HEAD) WchFile << "---!";
	else if (node->hasType()==SHIFTTO)
	{
		if (((SftNode *)node)->getSftTag()==MOVE) WchFile << "---#";
		else if (((SftNode *)node)->getSftTag()==REMV) WchFile << "---&";
		else /* (((SftNode *)node)->getSftTag()==COPY) */ WchFile << "---*";
	}
	else WchFile << "----";
	WchFile << node->getTgtLabel();
	CplNode * child=node->getLChild();
	if (child)
	{
		while (child)
		{
			output_TgtRootNode(child,depth+1);
			child=child->getRNeigh();
		}
	}
	else
	{
		WchFile << "<";
		Attribute * baseformatt=TheModel->getBaseForm(TheTgtLang);
		Atom * baseformval=getIntValue(node,baseformatt);
		const char * baseform=baseformval->getNucleus();
//		const char * baseform=node->get_BaseForm();
		if (baseform) WchFile << baseform;
		WchFile << "/";
		if (node->getSrcSect())
		{
			const char * begin=node->getBgnChar(), *end=node->getEndChar();
			while (begin!=end) WchFile << *begin++;
		}
		WchFile << ">";
	}
}

void Translator::output_prsCplTree(CplNode * node)
{
	if (!WchFile.is_open()) return;
	if (node->getSrcHeadTag()==HEAD) WchFile << "!";
	WchFile << node->getSrcLabel();
	if (node->getLChild())
	{
		WchFile << "(";
		CplNode * child=node->getLChild();
		while (child)
		{
			if (child->getSrcHeadTag()==HEAD) WchFile << "!";
			CplNode * grandchild=child->getLChild();
			WchFile << child->getSrcLabel();
			if (grandchild) WchFile << "("; else WchFile << "<";
			const char * begin=child->getBgnChar(), *end=child->getEndChar();
			while (begin!=end) WchFile << *begin++;
			if (grandchild) WchFile << ")"; else WchFile << ">";
			if (child=child->getRNeigh()) WchFile << " ";
		}
		WchFile << ")";
	}
	else
	{
		WchFile << "<";
		const char * begin=node->getBgnChar(), *end=node->getEndChar();
		while (begin!=end) WchFile << *begin++;
		WchFile << ">";
	}
}

void Translator::output_prsPtlTree(PtlNode * node)
{
	if (!WchFile.is_open()) return;
	CplList list;
	PtlNode * ptlchild=node;
	while (ptlchild)
	{
		list.add(ptlchild->getCplChild());
		ptlchild=ptlchild->getPtlChild();
	}
	while (list.getLinkHead()) output_prsCplTree(list.del());
	WchFile << "...";
}

void Translator::output_trnTree(CplNode * node)
{
	if (!WchFile.is_open()) return;
	const char * label;
	if (node->getTgtHeadTag()==HEAD) WchFile << "!";
	else if (node->hasType()==SHIFTTO)
	{
		if (((SftNode *)node)->getSftTag()==MOVE) WchFile << "#";
		else if (((SftNode *)node)->getSftTag()==REMV) WchFile << "&";
		else /* (((SftNode *)node)->getSftTag()==COPY) */ WchFile << "*";
	}
	if (label=node->getTgtLabel()) WchFile << label;
	if (label=node->getSrcLabel()) WchFile << '/' << label;
	CplNode * child=node->getLChild();
	if (node->getTransed() && child)
	{
		WchFile << "(";
		while (child)
		{
			output_trnTree(child);
			if (child=child->getRNeigh()) WchFile << ' ';
		}
		WchFile << ")";
	}
	else if (node->getSrcLabel())
	{
		WchFile << "<";
		const char * begin=node->getBgnChar(), *end=node->getEndChar();
		while (begin!=end) WchFile << *begin++;
		WchFile << ">";
	}
}

void Translator::output_makTree(CplNode * node)
{
	if (!WchFile.is_open()) return;
	const char * label;
	if (node->getTgtHeadTag()==HEAD) WchFile << "!";
	else if (node->hasType()==SHIFTTO)
	{
		if (((SftNode *)node)->getSftTag()==MOVE) WchFile << "#";
		else if (((SftNode *)node)->getSftTag()==REMV) WchFile << "&";
		else /* (((SftNode *)node)->getSftTag()==COPY) */ WchFile << "*";
	}
	if (label=node->getTgtLabel()) WchFile << label;
	CplNode * child=node->getLChild();
	if (child)
	{
		WchFile << "(";
		while (child)
		{
			output_makTree(child);
			if (child=child->getRNeigh()) WchFile << ' ';
		}
		WchFile << ")";
	}
	else
	{
		WchFile << "<";
//		Attribute * baseformatt=TheModel->getBaseForm(TheTgtLang);
//		Atom * baseformval=getIntValue(node,baseformatt);
//		const char * baseform=baseformval->getNucleus();
		const char * baseform=node->get_BaseForm();
		if (baseform) WchFile << baseform;
		WchFile << ">";
	}
}

void Translator::output_cutNode(CplNode * node)
{
	if (!WchFile.is_open()) return;
	WchFile << hex << "源文单词("
				<< (int)node << ") " << node->getSrcLabel() << ": ";
	const char * begin=node->getBgnChar(), *end=node->getEndChar();
	while (begin!=end) WchFile << *begin++; 
	WchFile << endl;
	char title[TITLELENGTH+1];
	if (node->get_Rule())
	{
		PrsRbase::getTitle(node->get_Rule(),title);
		WchFile << "\t单词义项: " << "{" << title << "}" << endl;
	}
	Fest * fest=node->getFest();
	Fest::map(fest);
	WchFile << "\t特征结构: ";
	TheUnity->output_fest(WchFile,fest,TheSrcLang);
	WchFile << endl;
}

void Translator::output_prsPtlNode(PtlNode * node)
{
	if (!WchFile.is_open()) return;
	WchFile << hex	<< "局部分析("
				<< (int)node << ":" << (int)node->getPtlChild()
				<< "--" << (int)node->getCplChild() << ") ";
	/*
	if (node->getSrcSect())
	{
		const char * begin=node->getBgnChar(), *end=node->getEndChar();
		while (begin!=end) WchFile << *begin++;
	}
	*/
	WchFile << endl;
	/*
	if (node->getRuleType()==GLPRSRULE) WchFile << "\t全局规则: ";
	else (node->getRuleType()==LCPRSRULE) WchFile << "\t局部规则: ";
	char title[TITLELENGTH+1];
	PrsRbase::getTitle(node->get_Rule(),title);
	WchFile << "{" << title << "}" << endl;
	WchFile << "\t树形结构: ";
	output_prsPtlTree(node);
	WchFile << endl;
	*/
}

void Translator::output_prsCplNode(CplNode * node)
{
	if (!WchFile.is_open()) return;
	CplNode * child=node->getLChild();
	WchFile << hex << "产生结点(" << (int)node << ":" << (int)child;
	child=child->getRNeigh();
	while (child) { WchFile << "--" << (int)child; child=child->getRNeigh(); }
	WchFile << ") " << node->getSrcLabel() << ": ";
	if (node->getSrcSect())
	{
		const char * begin=node->getBgnChar(), *end=node->getEndChar();
		while (begin!=end) WchFile << *begin++;
	}
	WchFile << endl;
	if (node->getRuleType()==GLPRSRULE) WchFile << "\t全局规则: ";
	else /* (node->getRuleType()==LCPRSRULE) */ WchFile << "\t局部规则: ";
	char title[TITLELENGTH+1];
	PrsRbase::getTitle(node->get_Rule(),title);
	WchFile << "{" << title << "}" << endl;
	WchFile << "\t树形结构: ";
	output_prsCplTree(node);
	WchFile << endl;
	Fest * fest=node->getFest();
	Fest::map(fest);
	WchFile << "\t特征结构: ";
	TheUnity->output_fest(WchFile,fest,TheSrcLang);
	WchFile << endl;
}

void Translator::output_limitsametypenode(PtlNode * node)
{
	if (!WchFile.is_open()) return;
	if (node->hasType()!=COMPLETE) error("output_limitsametypenode(): not a COMPLETE node.");
	CplNode * child=((CplNode *)node)->getLChild();
	WchFile << hex << "限制冲突(" << (int)node << ":" << (int)child;
	child=child->getRNeigh();
	while (child) { WchFile << "--" << (int)child; child=child->getRNeigh(); }
	WchFile << ") " << ((CplNode *)node)->getSrcLabel() << ": ";
	if (node->getSrcSect())
	{
		const char * begin=node->getBgnChar(), *end=node->getEndChar();
		while (begin!=end) WchFile << *begin++;
	}
	WchFile << endl;
}

void Translator::output_trnNode(CplNode * node)
{
	if (!WchFile.is_open()) return;
	WchFile << hex
		 << "转换结点(" << (int)node << ") " << node->getSrcLabel()
		 << "=>" << node->getTgtLabel() << ": ";
	if (node->getSrcSect())
	{
		const char * begin=node->getBgnChar(), *end=node->getEndChar();
		while (begin!=end) WchFile << *begin++;
	}
	WchFile << endl;
	if (node->getRuleType()==MEANING) WchFile << "\t单词义项: ";
	else if (node->getRuleType()==GLPRSRULE) WchFile << "\t全局规则: ";
	else /* (node->getRuleType()==LCPRSRULE) */ WchFile << "\t局部规则: ";
	char title[TITLELENGTH+1];
	PrsRbase::getTitle(node->get_Rule(),title);
	WchFile << "{" << title << "}" << endl;
	WchFile << "\t树形结构: ";
	output_trnTree(node);
	WchFile << endl;
	Fest * fest=node->getFest();
	Fest::map(fest);
	WchFile << "\t特征结构: ";
	TheUnity->output_fest(WchFile,fest,TheSrcLang);
	WchFile<< endl;
}

void Translator::output_trnNodeSucc(CplNode * node)
{
	if (!WchFile.is_open()) return;
	WchFile << hex
		 << "转换成功(" << (int)node << ") " << node->getSrcLabel()
		 << "=>" << node->getTgtLabel() << ": ";
	if (node->getSrcSect())
	{
		const char * begin=node->getBgnChar(), *end=node->getEndChar();
		while (begin!=end) WchFile << *begin++;
	}
	WchFile << endl;
	if (node->getRuleType()==MEANING) WchFile << "\t单词义项: ";
	else if (node->getRuleType()==GLPRSRULE) WchFile << "\t全局规则: ";
	else /* (node->getRuleType()==LCPRSRULE) */ WchFile << "\t局部规则: ";
	char title[TITLELENGTH+1];
	PrsRbase::getTitle(node->get_Rule(),title);
	WchFile << "{" << title << "}" << endl;
	WchFile << "\t树形结构: ";
	output_trnTree(node);
	WchFile << endl;
	Fest * fest=node->getFest();
	Fest::map(fest);
	WchFile << "\t特征结构: ";
	TheUnity->output_fest(WchFile,fest,TheTgtLang);
	WchFile << endl;
}

void Translator::output_trnNodeFail(CplNode * node,IToken &token)
{
	if (!WchFile.is_open()) return;
	WchFile << hex << "转换失败(" << (int)node << ") " << node->getSrcLabel();
	if (node->getTgtLabel()) WchFile << "=>" << node->getTgtLabel();
	WchFile << ": ";
	if (node->getSrcSect())
	{
		const char * begin=node->getBgnChar(), *end=node->getEndChar();
		while (begin!=end) WchFile << *begin++;
	}
	WchFile << endl;
	if (&token==NULL) return;
	output_spyBind(node,token);
}

void Translator::output_makNode(CplNode * node, const char * makrule)
{
	if (!WchFile.is_open()) return;
	WchFile << hex
		 << "变换结点(" << (int)node << ") "
		 << node->getTgtLabel() << ": ";
	if (node->getSrcSect())
	{
		const char * begin=node->getBgnChar(), *end=node->getEndChar();
		while (begin!=end) WchFile << *begin++;
	}
	WchFile << endl;
	char title[TITLELENGTH+1];
	PrsRbase::getTitle(makrule,title);
	WchFile << "\t造句规则: " << "{" << title << "}" << endl;
	WchFile << "\t树形结构: ";
	pushNodeUnity(node);
	output_makTree(node);
	popNodeUnity();
	WchFile << endl;
	Fest * fest=node->getFest();
	Fest::map(fest);
	WchFile << "\t特征结构: ";
	TheUnity->output_fest(WchFile,fest,TheTgtLang);
	WchFile << endl;
}

void Translator::output_bldWordFail(CplNode * node,const char * form)
{
	if (!WchFile.is_open()) return;
	WchFile << hex << "构词失败(" << (int)node << "): "
		 << node->getTgtLabel();
	WchFile << "<";
	if (form && *form) WchFile << form;
	else WchFile << "空";
	WchFile << ">";
	WchFile<< endl;
}

void Translator::output_sftWordFail(SftNode * sftnode)
{
	if (!WchFile.is_open()) return;
	WchFile << hex << "位移失败(" << (int)sftnode << "): "
		 << sftnode->getTgtLabel() << endl;
}

void Translator::output_sftWord(SftNode * sftnode,CplNode * sftfrom)
{
	if (!WchFile.is_open()) return;
	WchFile << hex << "位移成功(" << (int)sftnode << "): "
		 << sftnode->getTgtLabel() << " 来源: " << (int)sftfrom << endl;
}

void Translator::output_bldNode(TgtSgmt * sgmt, const char * bldrule)
{
	if (!WchFile.is_open()) return;
	CplNode * node=sgmt->getNode();
	WchFile << hex
		 << "译文单词(" << (int)node << ") "
		 << node->getTgtLabel() << ": ";
	WchFile << sgmt->get_BaseForm() << ">> " << sgmt->get_VaryForm() << endl;
	WchFile << "构词规则: " << bldrule;
}

#endif
