
#ifndef STRUCT_H

#define STRUCT_H

#include "unify.h"
#include "../kbase/mkbase.h"
#include "../basic/list.h"

class SrcStruct;
class TgtStruct;
class SrcSgmt;
class SrcSect;
class TgtSgmt;
class TrnField;
class TrnStack;
class MakField;
class MakStack;
class Variable;
class VarList;
class VarListElem;
class VarListStack;
class Relation;
class RelStack;
class PtlNode;
class PtlLink;
class PtlList;
class PrsList;
class CplLink;
class CplList;
class CplListElem;
class CplListStack;
class NodeUnityStack;
class TextLink;
class TextList;
class CplNode;
class SftNode;
class SftLink;
class SftList;
class NodeRouteLink;
class NodeRouteList;
class SrcRootLink;
class RootList;
#ifdef KBMS
class SpyNode;
class SpyLink;
class SpyList;
#endif

extern int GlobalSrcLang;
extern int GlobalTgtLang;
extern Model * GlobalModel;

class SrcStruct
{
	SrcSgmt * SrcSgmtHead;
	SrcSgmt * SrcSgmtTail;

  public:

	SrcStruct()
	{
		SrcSgmtHead=SrcSgmtTail=NULL;
	}
	~SrcStruct() { free(); }

	void free() { freeNodes(); freeSgmts(); }
	void freeNodes();
	void freeSgmts();
	SrcSgmt * getSrcSgmtHead() { return SrcSgmtHead; }
	SrcSgmt * getSrcSgmtTail() { return SrcSgmtTail; }

	void addSrcSgmtHead(SrcSgmt *);
	void addSrcSgmtTail(SrcSgmt *);

	void delSrcSgmtHead();
	void delSrcSgmtTail();
	void delSrcSgmt(SrcSgmt * sgmt);
};

class TgtStruct
{
	TgtSgmt * TgtSgmtHead;
	TgtSgmt * TgtSgmtTail;

  public:

	TgtStruct()
	{
		TgtSgmtHead=TgtSgmtTail=NULL;
	}
	~TgtStruct() { free(); }

	void free();
	TgtSgmt * getTgtSgmtHead() { return TgtSgmtHead; }
	TgtSgmt * getTgtSgmtTail() { return TgtSgmtTail; }

	void addTgtSgmtHead(TgtSgmt *);
	void addTgtSgmtTail(TgtSgmt *);
	void addTgtSgmtBefore(TgtSgmt * cursgmt,TgtSgmt * newsgmt);
	void addTgtSgmtAfter(TgtSgmt * cursgmt,TgtSgmt * newsgmt);

	void delTgtSgmtHead();
	void delTgtSgmtTail();
};

enum { CHNCHAR, LTNCHAR, DGTCHAR, PCTCHAR, OTHCHAR };

class SrcSgmt
{
	char * BgnChar;
	char * EndChar;
	SrcSect * ThisFstSect;
	SrcSect * ThisLstSect;
	SrcSgmt * PrevSgmt;
	SrcSgmt * SucsSgmt;
	unsigned char LeftMatch;
	unsigned char RightMatch;
	unsigned char SgmtType;
	unsigned int Level;
	unsigned int StepNum;
	unsigned int SectNum;
	unsigned int NodeNum;

  public:

	SrcSgmt()
	{
		BgnChar=EndChar=NULL;
		ThisFstSect=ThisLstSect=NULL;
		PrevSgmt=SucsSgmt=NULL;
		LeftMatch=RightMatch=FALSE;
		SgmtType=OTHCHAR;
		StepNum=0; /* the minimal step number from the head to the end of this SrcSgmt */
		Level=0;
	}

	char * getBgnChar() { return BgnChar; }
	char * getEndChar() { return EndChar; }
	SrcSgmt * getPrevSgmt() { return PrevSgmt; }
	SrcSgmt * getSucsSgmt() { return SucsSgmt; }
	int getLeftMatch() { return LeftMatch; }
	int getRightMatch() { return RightMatch; }
	int getSgmtType() { return SgmtType; }
	int getLevel() { return Level; }
	int getStepNum() { return StepNum; }
	int getSectNum() { return SectNum; }
	int getNodeNum() { return NodeNum; }
	SrcSect * getThisFstSect() { return ThisFstSect; }
	SrcSect * getThisLstSect() { return ThisLstSect; }
	PtlNode * getThisFstNode();
	PtlNode * getThisLstNode();

	void setBgnChar(char *bgn) { BgnChar=bgn; }
	void setEndChar(char *end) { EndChar=end; }
	void setLeftMatch(int l) { LeftMatch=l; }
	void setRightMatch(int r) { RightMatch=r; }
	void setSgmtType(int t) { SgmtType=t; }
	void setLevel(int l) { Level=l; }
	void setStepNum(int n) { StepNum=n; }
	void setSectNum(int n) { SectNum=n; }
	void setNodeNum(int n) { NodeNum=n; }

	void setThisFstSect(SrcSect * sect) { ThisFstSect=sect; }
	void setThisLstSect(SrcSect * sect) { ThisLstSect=sect; }

	SrcSgmt * srhBgnSgmt(char * bgnchar);
	SrcSgmt * srhEndSgmt(char * endchar);

	friend class SrcStruct;
};

class SrcSgmtPairLink
{
	SrcSgmt * LeftSgmt;
	SrcSgmt * RightSgmt;
	SrcSgmtPairLink * Next;
public:
	SrcSgmt * getLeftSgmt() { return LeftSgmt; }
	SrcSgmt * getRightSgmt() { return RightSgmt; }
	SrcSgmtPairLink * getNext() { return Next; }
	friend class SrcSgmtPairList;
};

class SrcSgmtPairList
{
	SrcSgmtPairLink * Head;
public:
	SrcSgmtPairList() { Head=NULL; }
	~SrcSgmtPairList() { free(); }
	void free()
	{
		SrcSgmtPairLink * link=Head, * next;
		while (link) { next=link->Next; delete link; link=next; }
		Head=NULL;
	}
	SrcSgmtPairLink * head() { return Head; }
	void shrink(SrcSgmtPairLink * link)
	{
		while (*(link->LeftSgmt->getBgnChar())=='[')
			link->LeftSgmt=link->LeftSgmt->getSucsSgmt();
		while (*(link->RightSgmt->getBgnChar())==']')
			link->RightSgmt=link->RightSgmt->getPrevSgmt();
	}
	void add(SrcSgmt * l, SrcSgmt * r)
	{
		SrcSgmtPairLink * link=new SrcSgmtPairLink;
		link->LeftSgmt=l;
		link->RightSgmt=r;
		link->Next=Head;
		Head=link;
		l->setLeftMatch(TRUE);
		r->setRightMatch(TRUE);
	}
};

class SrcSect
{
protected:
	SrcSgmt * FstSrcSgmt;
	SrcSgmt * LstSrcSgmt;

	SrcSect * SameFstSect;
	SrcSect * SameLstSect;

	PtlNode * FstNode;
	PtlNode * LstNode;
	PtlNode * FstTempNode;  /* node has not been added into SrcStruct */

	int NodeNum;

	void * Data;

	bool Valid;

public:
	SrcSect()
	{
		FstSrcSgmt=LstSrcSgmt=NULL;
		SameFstSect=SameLstSect=NULL;
		FstNode=LstNode=FstTempNode=NULL;
		NodeNum=0;
		Data=NULL;
		Valid=true;
	}
	~SrcSect() {};

	SrcSgmt * getFstSrcSgmt() { return FstSrcSgmt; }
	SrcSgmt * getLstSrcSgmt() { return LstSrcSgmt; }
	SrcSgmt * getPrevSrcSgmt() { return FstSrcSgmt->getPrevSgmt(); }
	SrcSgmt * getSucsSrcSgmt() { return LstSrcSgmt->getSucsSgmt(); }
	int getNodeNum() { return NodeNum; }
	char * getBgnChar() { return FstSrcSgmt->getBgnChar(); }
	char * getEndChar() { return LstSrcSgmt->getEndChar(); }
	void * getData() { return Data; }
	bool getValid() { return Valid; }

	SrcSect * getSameFstSect() { return SameFstSect; }
	SrcSect * getSameLstSect() { return SameLstSect; }

	PtlNode * getFstNode() { return FstNode; }
	PtlNode * getLstNode() { return LstNode; }
	PtlNode * getFstTempNode() { return FstTempNode; }

	void setData(void * i) { Data=i; }

	void addTempNode(PtlNode * node);
	int delTempNode(PtlNode * node);

	friend class SrcSgmt;
	friend class PtlNode;
};

inline PtlNode * SrcSgmt::getThisFstNode()
{
	SrcSect * sect=ThisFstSect;
	while (sect && !sect->FstNode) sect=sect->getSameFstSect();
	return sect?sect->FstNode:NULL;
}
inline PtlNode * SrcSgmt::getThisLstNode()
{
	SrcSect * sect=ThisLstSect;
	while (sect && !sect->FstNode) sect=sect->getSameLstSect();
	return sect?sect->FstNode:NULL;
}

class TgtSgmt
{
	CplNode	* TheNode;

	const char * _FormPntr;
	const char * _FormScan;
	char _BaseForm[TOKENLENGTH];
	char _VaryForm[TOKENLENGTH];

	TgtSgmt * PrevSgmt;
	TgtSgmt * SucsSgmt;

  public:

	TgtSgmt()
	{
		TheNode=NULL;
		_FormPntr=_FormScan=NULL;
		*_BaseForm=*_VaryForm=0;
		PrevSgmt=SucsSgmt=NULL;
	}

	CplNode * getNode() { return TheNode; }
	const char * get_FormPntr() { return _FormPntr; }
	const char * get_FormScan() { return _FormScan; }
	char * get_BaseForm() { return _BaseForm; }
	char * get_VaryForm() { return _VaryForm; }
	TgtSgmt * getPrevSgmt() { return PrevSgmt; }
	TgtSgmt * getSucsSgmt() { return SucsSgmt; }

	void setNode(CplNode *node) { TheNode=node; }

	void set_FormPntr(const char * pntr) { _FormPntr=pntr; }
	void set_FormScan() { _FormScan=_FormPntr; }
	int set_BaseForm();
	int set_VaryForm(char * varyform=NULL);

	friend class TgtStruct;
};

class TrnField
{
	CplNode * Node;
	const char * TrnPart;
	const SftLink * SftBott;
	TrnField * Next;

	friend class TrnStack;
};

class TrnStack
{
	TrnField * TopField;

  public:

	TrnStack() { TopField=NULL; }
	~TrnStack() { free(); }

	void free();

	void push(CplNode * node, const char * trnpart, const SftLink * sftbott)
	{
		TrnField * field=new TrnField;
		field->Node=node;
		field->TrnPart=trnpart;
		field->SftBott=sftbott;
		field->Next=TopField;
		TopField=field;
	}
	int pop(CplNode * &node, const char * &trnpart, const SftLink * &sftbott)
	{
		if (!TopField) return FAIL;
		else
		{
			TrnField * field=TopField;
			TopField=field->Next;
			node=field->Node;
			trnpart=field->TrnPart;
			sftbott=field->SftBott;
			delete field;
			return SUCC;
		}
	}
	TrnField * top() { return TopField; }
	CplNode * topNode() { return TopField->Node; }
};

class MakField
{
	CplNode * Node;
	RuleLink * TheRuleLink;
	const char * RightPart;
	const SftLink * SftBott;
	MakField * Next;

	friend class MakStack;
};

class MakStack
{
	MakField * TopField;

  public:

	MakStack() { TopField=NULL; }
	~MakStack() { free(); }

	void free();

	void push(CplNode * node, RuleLink * rulelink, const char * rightpart, const SftLink * sftbott)
	{
		MakField * field=new MakField;
		field->Node=node;
		field->TheRuleLink=rulelink;
		field->RightPart=rightpart;
		field->SftBott=sftbott;
		field->Next=TopField;
		TopField=field;
	}
	int pop(CplNode * &node, RuleLink * &rulelink, const char * &rightpart, const SftLink * &sftbott)
	{
		if (!TopField) return FAIL;
		else
		{
			MakField * field=TopField;
			TopField=field->Next;
			node=field->Node;
			rulelink=field->TheRuleLink;
			rightpart=field->RightPart;
			sftbott=field->SftBott;
			delete field;
			return SUCC;
		}
	}
	MakField * top() { return TopField; }
	CplNode * topNode() { return TopField->Node; }
};

class Variable
{
	unsigned int Tag:3;
	unsigned int Used:1;
	unsigned int Leaf:1;
	unsigned int Order:3;
	char Label[TOKENLENGTH];

  public:

	Variable()
	{
		Tag=Used=Leaf=Order=0;
		*Label=0;
	}

	int getTag() { return Tag; }
	int getUsed() { return Used; }
	int getLeaf() { return Leaf; }
	int getOrder() { return Order; }
	const char * getLabel() { return Label; }

	void setTag(int tag) { Tag=tag; }
	void setUsed(int used) { Used=used; }
	void setLeaf(int leaf) { Leaf=leaf; }
	void setOrder(int order) { Order=order; }
	void setLabel(const char *label)
		{ if (label) strcpy(Label,label); else *Label=0; }

	int isRoot() { return Order==1 && *Label==0; }
	void doRoot() { Order=1; *Label=0; }

	int isNull() { return Order==0; }
	void doNull() { Order=0; }

	int isEqualTo(Variable & var)
	{
		return Order==var.Order && !strcmp(Label,var.Label);
	}
	int operator == (Variable &var) { return isEqualTo(var); }
	int operator != (Variable &var) { return !isEqualTo(var); }

	void inputFrom(IToken & token);
	void outputTo(OToken & token);

	static int leadBy(IToken & token);
	static void skipOver(IToken & token);
};

class VarLink
{
	Variable Var;
	CplNode * Node;
	VarLink * Next;

  public:

	VarLink() { Node=NULL; Next=NULL; }
	Variable * getVar() { return &Var; }
	CplNode * getNode() { return Node; }
	VarLink * getNext() { return Next; }

	friend class VarList;
};

class VarList
{
	VarLink * FstLink;
	VarLink * MidLink;

  public:

	VarList() { FstLink=NULL; MidLink=NULL; }
	~VarList() { free(); }

	void free();

	VarLink * getFstLink() { return FstLink; }
	VarLink * getMidLink() { return MidLink; }

	VarLink * srhVar(const char * label);
	VarLink * srhVarNode(Variable * var, int &here=*(int *)NULL);

	void setMidLink(VarLink * link)
	{
		if (FstLink) error("VarList::setMidLink(): set MidLink while FstLink!=NULL.");
		FstLink=MidLink=link;
	}

	VarLink * addVar(const char * label, int order, int tag);
	VarLink * addVar(const char * label, int tag);
	VarLink * addVarNode(Variable * var, CplNode * node);
	void addVarLeaf(Variable * var);

	void clrUsed();

	void moveFrom(VarList & varlist)
	{
		if (FstLink || MidLink) error("VarList::moveFrom(): move to a unempty VarList.");
		FstLink=varlist.FstLink;
		MidLink=varlist.MidLink;
		varlist.FstLink=varlist.MidLink=NULL;
	}
};

class VarListElem : public VarList
{
	VarListElem * Next;

  public:

	VarListElem() { Next=NULL; }

	VarListElem * getNext() { return Next; }

	friend class VarListStack;

};

class VarListStack
{
	VarListElem * TopElem;

  public:

	VarListStack() { TopElem=NULL; }
	~VarListStack() { free(); }

	void free();

	void push()
	{
		VarListElem * link=new VarListElem; 
		link->Next=TopElem;
		TopElem=link;
	}
	void pop()
	{
		if (!TopElem) error("VarListStack::pop(): underflow.");
		VarListElem * link=TopElem;
		TopElem=link->Next;
		delete link;
	}
	VarListElem * top() { return TopElem; }

	void moveFrom(VarListStack & stack)
	{
		if (TopElem) error("VarListStack::moveFrom(): move to a unempty VarListStack.");
		TopElem=stack.TopElem;
		stack.TopElem=NULL;
	}

};

class Relation
{
	CplNode * Parent;
	CplNode * RNeigh;
	CplNode * LChild;

  public:

	Relation() { Parent=RNeigh=LChild=NULL; }

	CplNode * getParent() { return Parent; }
	CplNode * getRNeigh() { return RNeigh; }
	CplNode * getLChild() { return LChild; }

	void setParent(CplNode * node) { Parent=node; }
	void setRNeigh(CplNode * node) { RNeigh=node; }
	void setLChild(CplNode * node) { LChild=node; }
};

class RelElem : public Relation
{
	RelElem * Next;

  public:

	RelElem() { Next=NULL; }

	friend class RelStack;
};

class RelStack
{
	RelElem * TopElem;

  public:

	RelStack() { TopElem=NULL; }
	~RelStack() { free(); }

	void free();

	void push()
	{
		RelElem * elem=new RelElem();
		if (TopElem) *elem=*TopElem;
		elem->Next=TopElem;
		TopElem=elem;
	}
	void pop()
	{
		if (!TopElem) error("RelStack::pop(): underflow.");
		RelElem * elem=TopElem;
		TopElem=elem->Next;
		delete elem;
	}
	RelElem * top() { return TopElem; }
};

enum { PARTIAL, COMPLETE, SHIFTTO };
enum { NTAG=0, HEAD=1, MOVE=2, COPY=4, REMV=8 };
enum { MEANING=0x1, PRSRULE=0x6, GLPRSRULE=0x4, LCPRSRULE=0x2 };
enum { BABY, YOUTH, ADULT, DEAD };

class PtlNode
{
  protected:

	unsigned char Age;

	SrcSect * TheSrcSect;
	PtlNode * SameSrcSectNode;

	PtlNode * PtlChild;
	CplNode * CplChild;

	int SubNodeNum;

	unsigned char ChildSrcHeadTag;	// if the CplChild is a SrcHead: NTAG or HEAD

	VarListStack TheVarListStack;

	UnityStack TheUnityStack;

	unsigned char RuleType;

	const char * _Rule;

	const char * _Scan;

	const char * _Bind;

	char * _BaseForm;

	char * _VaryForm;

	void * Data;

	PtlNode(PtlNode *);

#ifdef KBMS
	SpyLink * TheSpyLink;
#endif

  public:

	PtlNode();
	virtual ~PtlNode() { free(); }
	
	void free();

	virtual int hasType() { return PARTIAL; }

	int getAge() { return Age; }
	void setAge(int age) { Age=age; }

	SrcSgmt * getFstSrcSgmt() { return TheSrcSect->FstSrcSgmt; }
	SrcSgmt * getLstSrcSgmt() { return TheSrcSect->LstSrcSgmt; }
	SrcSgmt * getPrevSrcSgmt() { return TheSrcSect->getFstSrcSgmt()->getPrevSgmt(); }
	SrcSgmt * getSucsSrcSgmt() { return TheSrcSect->getLstSrcSgmt()->getSucsSgmt(); }
	char * getBgnChar() { return TheSrcSect->getBgnChar(); }
	char * getEndChar() { return TheSrcSect->getEndChar(); }
	void * getData() { return Data; }

	void setData(void * i) { Data=i; }

	PtlNode * getSameFstNode()
	{
		if (SameSrcSectNode) return SameSrcSectNode;
		SrcSect * sect=TheSrcSect->SameFstSect;
		while (sect && !sect->FstNode) sect=sect->getSameFstSect();
		return sect?sect->FstNode:NULL;
	}
	PtlNode * getSameLstNode()
	{
		if (SameSrcSectNode) return SameSrcSectNode;
		SrcSect * sect=TheSrcSect->SameLstSect;
		while (sect && !sect->FstNode) sect=sect->getSameLstSect();
		return sect?sect->FstNode:NULL;
	}

	SrcSect * getSrcSect() { return TheSrcSect; }
	PtlNode * getSameSrcSectNode() { return SameSrcSectNode; }
	PtlNode * getPtlChild() { return PtlChild; }
	CplNode * getCplChild() { return CplChild; }

	int getSubNodeNum() { return SubNodeNum; }

	const VarListStack & getVarListStack() { return TheVarListStack; }

	void pushVarList() { TheVarListStack.push(); }
	void popVarList() { TheVarListStack.pop(); }
	VarList * topVarList() { return TheVarListStack.top(); }
	VarList * leftVarList() { return TheVarListStack.top()->getNext(); }

	const UnityStack & getUnityStack() { return TheUnityStack; }

	void pushUnity(Unity * unity=NULL)
	{
		TheUnityStack.push();
		if (unity) { topUnity()->addChild(unity); }
	}
	void popUnity() { TheUnityStack.pop(); }
	Unity * topUnity() { return TheUnityStack.top(); }

	void useUnity() { topUnity()->use(); }

	int getChildSrcHeadTag() { return ChildSrcHeadTag; }

	int getRuleType() { return RuleType; }

	const char * get_Rule() { return _Rule; }
	const char * get_Scan() { return _Scan; }
	const char * get_Bind() { return _Bind; }
	const char * get_BaseForm() { return _BaseForm; }
	const char * get_VaryForm() { return _VaryForm; }

	void addIntoSect(SrcSgmt * fstsgmt,SrcSgmt * lstsgmt, SrcSgmtPairList & matchlist);
	int  addIntoStruct();

	void setChildSrcHeadTag(int tag)
	{
		if (!ChildSrcHeadTag) ChildSrcHeadTag=tag;
		else error("PtlNode::setChildSrcHeadTag(): twice.");
	}

	void setRuleType(int i) { RuleType=i; }

	void set_Rule(const char * rule) { _Rule=rule; }
	void set_Scan(const char * scan) { _Scan=scan; }
	void set_Bind(const char * bind) { _Bind=bind; }
	void set_BaseForm(const char * base)
	{
		if (_BaseForm) error("PtlNode::set_BaseForm(): set _BaseForm twice.",base);
		_BaseForm=new char[strlen(base)+1];
		strcpy(_BaseForm,base);
	}
	void set_VaryForm(const char * vary)
	{
		if (_VaryForm) error("PtlNode::set_VaryForm(): set _VaryForm twice.",vary);
		_VaryForm=new char[strlen(vary)+1];
		strcpy(_VaryForm,vary);
	}

	void setPtlChild(PtlNode * node);
	void setCplChild(CplNode * node);

	void setFamily();

	virtual int isPrior(PtlNode * node);

	VarLink * addVar(const char * label, int tag)
		{ return topVarList()->addVar(label,tag); }
	VarLink * addVarNode(Variable * var,CplNode *node)
		{ return topVarList()->addVarNode(var,node); }
	VarLink * srhVarNode(Variable * var,int &here=*(int *)NULL)
		{ return topVarList()->srhVarNode(var,here); }
	VarLink * srhLeftVarNode(Variable * var)
		{ return leftVarList()->srhVarNode(var); }
	void addVarLeaf(Variable * var)
		{ topVarList()->addVarLeaf(var); }

	CplNode * srhNode(Variable * var)
	{
		if (var->isRoot()) 
		{
			if (hasType()!=COMPLETE)
				error("PtlNode::srhNode(): search root variable in PtlNode.");
			return (CplNode *)this;
		}
		else
		{
			VarLink * varlink=srhVarNode(var);
			if (!varlink) error("PtlNode::srhNode(): variable not found.",
				var->getLabel(),get_Rule());
			return varlink->getNode();
		}
	}

#ifdef KBMS

	SpyLink * getSpyLink() { return TheSpyLink; }
	void setSpyLink(SpyLink * link) { TheSpyLink=link; }
#endif

	friend void SrcSect::addTempNode(PtlNode *);
	friend int  SrcSect::delTempNode(PtlNode *);
};

class PtlLink
{
	PtlNode * Node;
	PtlLink * Next;

  public:

	PtlNode * getNode() { return Node; }
	PtlLink * getNext() { return Next; }

	void setNode( PtlNode * node ) { Node=node; }
	void setNext( PtlLink * link ) { Next=link; }

	friend class PtlList;
};

class PtlList
{
  protected:

	PtlLink * FstLink;

  public:

	PtlList() { FstLink=NULL; }
	~PtlList() { free(); }

	void free();

	PtlLink * getFstLink() { return FstLink; }

	void add(PtlNode *node)
	{
		if (!node) error("PtlList::add(): cannot add NULL node.");
		PtlLink *link=new PtlLink;
		link->setNode(node);
		link->setNext(FstLink);
		FstLink=link;
	}
	PtlNode * get()
		{ if (FstLink) return FstLink->getNode(); else return NULL; }
	PtlNode * del()
	{
		if (!FstLink) error("PtlList::del(): underflow.");
		PtlLink * link=FstLink;
		FstLink=FstLink->getNext();
		PtlNode * node=link->Node;
		delete link;
		return node;
	}
};

class PrsList : public PtlList
{
  public:

	void addNode( PtlNode * node );
	PtlNode * delNode();
};

class CplLink
{
	CplNode * Node;
	CplLink * Next;

  public:

	CplLink() { Node=NULL; Next=NULL; }

	CplNode * getNode() { return Node; }
	CplLink * getNext() { return Next; }

	void setNode(CplNode * node) { Node=node; }
	void setNext(CplLink * next) { Next=next; }

	friend class CplList;
	friend class NodeUnityStack;
};

class CplList
{
  protected:

	CplLink * LinkHead;

  public:

	CplList() { LinkHead=NULL; }
	~CplList() { free(); }

	void free();

	CplLink * getLinkHead() { return LinkHead; }

	void add(CplNode * node);
	CplNode * del();
	CplLink * srh(CplNode * node);
};

class CplListElem : public CplList
{
	CplListElem * Next;

  public:

	CplListElem() { Next=NULL; }

	friend class CplListStack;
};

class CplListStack
{
	CplListElem * TopElem;

  public:

	CplListStack() { TopElem=NULL; }
	~CplListStack() { free(); }

	void free();

	void push()
	{
		CplListElem * link=new CplListElem; 
		link->Next=TopElem;
		TopElem=link;
	}
	void pop()
	{
		if (!TopElem) error("CplListStack::pop(): underflow.");
		CplListElem * link=TopElem;
		TopElem=link->Next;
		delete link;
	}
	CplListElem * top() { return TopElem; }

};

class NodeUnityStack
{
  protected:

	CplLink * TopLink;
	CplNode * BaseNode;

  public:

	NodeUnityStack() { TopLink=NULL; }
	~NodeUnityStack() { free(); }

	void free();

	CplLink * top() { return TopLink; }
	CplNode * base() { return BaseNode; }

	void init(CplNode * node);
	void push(CplNode * node);
	void pop();
};

enum { UNTRANSED, SWITCHED, SEMITRANSED, TRANSED, UNNEEDFUL };

class CplNode: public PtlNode
{
  protected:

	unsigned char SrcHeadTag;		// NTAG or HEAD
	unsigned char TgtHeadTag;		// NTAG or HEAD
	CplNode * SrcHead;
	CplNode * TgtHead;

	unsigned char Transed;

	unsigned char Present;		/* if the node has been shifted to or shifted from */
	unsigned char Sftable;		/* if the node can be shifted */

	const char * SrcLabel;
	const char * TgtLabel;

	Fest * TheFest;

	RelStack TheRelStack;

	CplListStack NewNodeListStack;
	CplListStack OldNodeListStack;

	TgtSgmt * FstTgtSgmt;
	TgtSgmt * LstTgtSgmt;

  public:

	CplNode(CplNode * rootnode=NULL);
	CplNode(PtlNode * node);
	virtual ~CplNode() { free(); }

	void free() { }

	int hasType() { return COMPLETE; }

	int getValid() { return topUnity()->getValid(); }

	int getSrcHeadTag() { return SrcHeadTag; }
	int getTgtHeadTag() { return TgtHeadTag; }
	CplNode * getSrcHead() { return SrcHead; }
	CplNode * getTgtHead() { return TgtHead; }

	void setSrcHeadTag(int tag) { SrcHeadTag=tag; }
	void setTgtHeadTag(int tag) { TgtHeadTag=tag; }

	void setSrcHead(CplNode * head)
		{ if (!SrcHead) SrcHead=head; else error("CplNode::setSrcHead(): twice."); }
	void setTgtHead(CplNode * head)
		{ if (!TgtHead) TgtHead=head; else error("CplNode::setTgtHead(): twice."); }
	void clrSrcHead() { SrcHead=NULL; }
	void clrTgtHead() { TgtHead=NULL; }

	const char * getSrcLabel() { return SrcLabel; }
	const char * getTgtLabel() { return TgtLabel; }

	void setSrcLabel(const char * label) { SrcLabel=label; }
	void setTgtLabel(const char * label) { TgtLabel=label; }

	int getTransed() { return Transed; }

	int getPresent() { return Present; }
	int getSftable() { return Sftable; }

	TgtSgmt * getFstTgtSgmt() { return FstTgtSgmt; }
	TgtSgmt * getLstTgtSgmt() { return LstTgtSgmt; }

	void setTransed(int i) { Transed=i; }

	void setPresent(int i) { Present=i; }
	void setSftable(int i) { Sftable=i; }

	Fest * getFest() { return TheFest; }

	void setFest(Fest * fest) { TheFest=fest; }

	const RelStack & getRelStack() { return TheRelStack; }

	void pushRelation() { TheRelStack.push(); }
	void popRelation() { TheRelStack.pop(); }
	Relation * topRelation() { return TheRelStack.top(); }

	CplNode * getParent() { return topRelation()->getParent(); }
	CplNode * getLChild() { return topRelation()->getLChild(); }
	CplNode * getRNeigh() { return topRelation()->getRNeigh(); }

	CplNode * getForward(CplNode * rootnode=NULL);

	void setParent( CplNode * node ) { topRelation()->setParent(node); }
	void setLChild( CplNode * node ) { topRelation()->setLChild(node); }
	void setRNeigh( CplNode * node ) { topRelation()->setRNeigh(node); }

	void setFamily();

	const CplListStack & getNewNodeListStack() { return NewNodeListStack; }

	void pushNewNodeList() { NewNodeListStack.push(); }
	void popNewNodeList();
	CplList * topNewNodeList() { return NewNodeListStack.top(); }

	void addNewNode(CplNode * node)
		{ topNewNodeList()->add(node); node->pushRelation(); }
	CplNode * delNewNode()
		{ return topNewNodeList()->del(); }
	int srhNewNode(CplNode * node)
		{ return topNewNodeList()->srh(node)?SUCC:FAIL; }

	const CplListStack & getOldNodeListStack() { return OldNodeListStack; }

	void pushOldNodeList() { OldNodeListStack.push(); }
	void popOldNodeList();
	CplList * topOldNodeList() { return OldNodeListStack.top(); }

	void addOldNode(CplNode * node)
		{ topOldNodeList()->add(node); node->pushRelation(); }
	CplNode * delOldNode()
		{ return topOldNodeList()->del(); }
	int srhOldNode(CplNode * node)
		{ return topOldNodeList()->srh(node)?SUCC:FAIL; }

	void pushNodeList() { pushNewNodeList(); pushOldNodeList(); }
	void popNodeList() { popOldNodeList(); popNewNodeList(); }

	void clrVarUsed() { topVarList()->clrUsed(); }

	void clrFstTgtSgmt() { FstTgtSgmt=NULL; }
	void clrLstTgtSgmt() { LstTgtSgmt=NULL; }
	void setFstTgtSgmt(TgtSgmt *sgmt)
		{ if(!FstTgtSgmt) FstTgtSgmt=sgmt; else error("CplNode::setFstTgtSgmt(): twice."); }
	void setLstTgtSgmt(TgtSgmt *sgmt)
		{ if(!LstTgtSgmt) LstTgtSgmt=sgmt; else error("CplNode::setLstTgtSgmt(): twice."); }

	friend class PtlNode;
};

class SftNode: public CplNode
{
  protected:

	unsigned char SftTag;	/* MOVE or REMV or COPY */
	const char * SftPntr;
	CplNode * SftRoot;
	CplNode * SftFrom;

  public:

	SftNode(CplNode * node);
	int hasType() { return SHIFTTO; }

	int getSftTag() { return SftTag; }
	const char * getSftPntr() { return SftPntr; }
	CplNode * getSftRoot() { return SftRoot; }
	CplNode * getSftFrom() { return SftFrom; }

	void setSftTag(int tag) { SftTag=tag; }
	void setSftPntr(const char * pntr) { SftPntr=pntr; }
	void setSftRoot(CplNode * root) { SftRoot=root; }
	void setSftFrom(CplNode * from) { SftFrom=from; }
};

class SftLink
{
	SftNode * Node;
	SftLink * Next;

  public:

	SftLink() { Node=NULL; Next=NULL; }

	SftNode * getNode() { return Node; }
	SftLink * getNext() { return Next; }

	void setNode(SftNode * node) { Node=node; }
	void setNext(SftLink * next) { Next=next; }

	friend class SftList;
};

class SftList
{
  protected:

	SftLink * LinkHead;

  public:

	SftList() { LinkHead=NULL; }
	~SftList() { free(); }

	void free();

	SftLink * getLinkHead() { return LinkHead; }

	void add(SftNode * node);
	SftNode * del();
	void delto(const SftLink * bott);
};

class NodeRouteLink
{
	CplNode * Node;
	NodeRouteLink * Prev;
	NodeRouteLink * Next;
	int StepNum;
	double Score;
	friend class NodeRouteList;
public:
	NodeRouteLink() { Node=NULL; Prev=Next=NULL; StepNum=0; }
	CplNode * getNode() { return Node; }
	NodeRouteLink * getPrev() { return Prev; }
	NodeRouteLink * getNext() { return Next; }
	int getStepNum() { return StepNum; }
	double getScore() { return Score; }
	void setScore(double s) { Score=s; }
};

class NodeRouteList
{
	NodeRouteLink * Head;
	NodeRouteLink * Tail;
  public:
	NodeRouteList() { Head=Tail=NULL; }
	~NodeRouteList() { free(); }
	void free()
	{
		NodeRouteLink * step, * next;
		for (step=Head;step;step=next) { next=step->getNext(); delete step; }
		Head=Tail=NULL;
	}
	NodeRouteLink * head() { return Head; }
	NodeRouteLink * tail() { return Tail; }
	NodeRouteLink * addTail(CplNode * node,NodeRouteLink * prev)
	{
		NodeRouteLink * step=new NodeRouteLink;
		step->Node=node; step->Prev=prev; step->Next=NULL;
		step->StepNum=prev?prev->StepNum+1:1;
		if (Head) { Tail->Next=step; Tail=step; } else Head=Tail=step;
		return step;
	}
	NodeRouteLink * addSort(CplNode * node,NodeRouteLink * prev,double score)
	{
		NodeRouteLink * step=new NodeRouteLink;
		step->Node=node; step->Prev=prev; step->Score=score;
		step->StepNum=prev?prev->StepNum+1:1;
		if (!Head) { Head=Tail=step; return step; }
		NodeRouteLink * first, * second;
		for (second=prev?prev:Head, first=second->Prev;
			 second && second->Score>score;
			 first=second, second=second->Next);
		if (first) first->Next=step; else Head=step;
		step->Next=second;
		if (second==NULL) Tail=step;
		return step;
	}
};

class Result
{
	char * TargetSentence;
	char * TargetTree;
	char * SourceTree;
public:
	Result() { TargetSentence=TargetTree=SourceTree=NULL; }
	Result(const Result & r) { TargetSentence=TargetTree=SourceTree=NULL; *this=r; }
	~Result() { free(); }
	void free()
	{
		if (TargetSentence) delete TargetSentence; TargetSentence=NULL;
		if (TargetTree) delete TargetTree; TargetTree=NULL;
		if (SourceTree) delete SourceTree; SourceTree=NULL;
	}
	const char * getTargetSentence() { return TargetSentence; }
	const char * getTargetTree() { return TargetTree; }
	const char * getSourceTree() { return SourceTree; }
	void setTargetSentence(const char * t)
	{
		if (TargetSentence) delete TargetSentence;
		if (t) { TargetSentence=new char[strlen(t)+1]; strcpy(TargetSentence,t); }
		else TargetSentence=NULL;
	}
	void setTargetTree(const char * t)
	{
		if (TargetTree) delete TargetTree;
		if (t) { TargetTree=new char[strlen(t)+1]; strcpy(TargetTree,t); }
		else TargetTree=NULL;
	}
	void setSourceTree(const char * s)
	{
		if (SourceTree) delete SourceTree;
		if (s) { SourceTree=new char[strlen(s)+1]; strcpy(SourceTree,s); }
		else SourceTree=NULL;
	}
	const Result & operator = (const Result & r)
	{
		setTargetSentence(r.TargetSentence);
		setTargetTree(r.TargetTree);
		setSourceTree(r.SourceTree);
		return r;
	}
	int operator == (const Result & r) const
	{
		return strcpy(TargetSentence,r.TargetSentence)==0; 
	}
};

//class ResultLink : ObjLink<Result> {};
//class ResultList : ObjList<Result,ResultLink> {};
typedef ObjLink <Result> ResultLink;
typedef ObjList <Result,ResultLink> ResultList;

class TgtRootLink
{
	char * TgtText;
	char * TgtTree;
	TgtRootLink * Next;
  public:
	TgtRootLink() { TgtText=TgtTree=NULL; Next=NULL; }
	char * getTgtText() { return TgtText; }
	char * getTgtTree() { return TgtTree; }
	TgtRootLink * getNext() { return Next; }
	void makTree(CplNode * node,Unity * unity);
	void makTreeNode(CplNode * node,OToken & token,Unity * unity);
	static int StaticAttribute;
	friend class SrcRootLink;
};

class SrcRootLink
{
	CplNode * SrcNode;
	char * SrcTree;
	TgtRootLink * TgtRootHead;
	TgtRootLink * TgtRootKnot; /* temp variable to compose the result */
	int TgtRootNumber;
	SrcRootLink * Prev;
	SrcRootLink * Sucs;
  public:
	SrcRootLink()
	{
		SrcNode=NULL; SrcTree=NULL;
		TgtRootHead=TgtRootKnot=NULL;
		TgtRootNumber=0;
		Prev=Sucs=NULL;
	}
	~SrcRootLink() { free(); }
	void free();
	CplNode * getSrcNode() { return SrcNode; }
	char * getSrcTree() { return SrcTree; }
	SrcRootLink * getPrev() { return Prev; }
	SrcRootLink * getSucs() { return Sucs; }
	TgtRootLink * getTgtRootHead() { return TgtRootHead; }
	TgtRootLink * getTgtRootTail();
	TgtRootLink * getTgtRootKnot() { return TgtRootKnot; }
	int getTgtRootNumber() { return TgtRootNumber; }
	TgtRootLink * addTgtRoot(const char *text);
	void makTree(Unity * unity);
	void makTreeNode(CplNode * node,OToken & token,Unity * unity);
	static int StaticAttribute;
	friend class RootList;
};

class RootList
{
	SrcRootLink * SrcRootHead;
	SrcRootLink * SrcRootTail;
	int Number;
  public:
	RootList() { SrcRootHead=SrcRootTail=NULL; Number=0; }
	~RootList() { free(); }
	void free();
	int getNumber() { return Number; }
	SrcRootLink * getSrcRootHead() { return SrcRootHead; }
	SrcRootLink * getSrcRootTail() { return SrcRootTail; }
	SrcRootLink * addSrcRootHead(CplNode * node);
	SrcRootLink * addSrcRootTail(CplNode * node);
	void insertSrcRootBehind(SrcRootLink * link,CplNode * node); /* add behind the link */
	void insertSrcRootBefore(SrcRootLink * link,CplNode * node); /* add before the link */
	void deleteSrcRootAt(SrcRootLink * link);
	static void altSrcRoot(SrcRootLink * link,CplNode * node);
	void initialResult();
	int  composeResult(Result & result);
	void forwardResult();
};

#ifdef KBMS

class SpyNode
{
	char * Label;	/* The SrcLabel of the SpyNode */
	char * Text;	/* The source text of the SpyNode */
	char * Title;	/* The prsrule title of the SpyNode */

  public:

	SpyNode() { Label=Text=Title=NULL; }
	~SpyNode() { free(); }

	void free() { Label=Text=Title=NULL; }

	char * getLabel() { return Label; }
	char * getText() { return Text; }
	char * getTitle() { return Title; }

	void read(char * &str);
	int isNode(CplNode * node);
};

class SpyLink
{
	SpyNode * Node;
	SpyLink * Next;
	SpyLink() { Node=NULL; Next=NULL; }

  public:

	SpyNode * getNode() { return Node; }
	SpyLink * getNext() { return Next; }

	friend class SpyList;
};

class SpyList
{
	SpyLink * FstLink;
	SpyLink * LstLink;

  public:

	SpyList() { FstLink=LstLink=NULL; }
	~SpyList() { free(); }
	void free();
	SpyLink * getFstLink() { return FstLink; }
	SpyLink * getLstLink() { return LstLink; }
	void add(SpyNode * node);
};

class SpyStruct
{
	char SpyText[SENTLENGTH+1];
	char SpyBuffer[SENTLENGTH+1];
	char * SpyTitle;
	SpyNode SpyTrnNode;
	SpyList SpyPrsList;
public:
	SpyStruct() { *SpyText=*SpyBuffer=0; SpyTitle=NULL; }
	~SpyStruct() { free(); }
	const char * getSpyText() { return SpyText; }
	const char * getSpyTitle() { return SpyTitle; }
	SpyNode * getSpyTrnNode() { return & SpyTrnNode; }
	SpyList & getSpyPrsList() { return SpyPrsList; }
	void free();
	void read(char * filename);
};
#endif

#endif
