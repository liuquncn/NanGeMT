#ifndef CETRANS_H

#define CETRANS_H

#include "../trans/trans.h"
#include "../basic/list.h"

#include "../kbase_ce/chndictn.h"
#include "../kbase_ce/chncutrbase.h"
#include "../kbase_ce/chnprsrbase.h"
#include "../kbase_ce/engmakrbase.h"
#include "../kbase_ce/engbldrbase.h"
#include "../kbase_ce/lexdbase.h"

class ChnXXXTranslator;
class XXXEngTranslator;
class ChnEngTranslator;

class SectData
{
	char ccat[TOKENLENGTH];
	char yx[TOKENLENGTH];
	char yj[3];
	int valid;
public:
	SectData()
	{
		*ccat=*yx=*yj=0;
		valid=0;
	}
	friend class LexAmbiguation;
	friend class ChnXXXTranslator;
};

class SectRouteLink
{
	SrcSect * Sect;
	SectRouteLink * Prev;
	SectRouteLink * Next;
	int StepNum;
	friend class SectRouteList;
  public:
	SectRouteLink() { Sect=NULL; Prev=Next=NULL; StepNum=0; }
	SrcSect * getSect() { return Sect; }
	SectRouteLink * getPrev() { return Prev; }
	SectRouteLink * getNext() { return Next; }
	int getStepNum() { return StepNum; }
};

class SectRouteList
{
	SectRouteLink * Head;
	SectRouteLink * Tail;
  public:
	SectRouteList() { Head=Tail=NULL; }
	~SectRouteList() { free(); }
	void free()
	{
		SectRouteLink * step, * next;
		for (step=Head;step;step=next) { next=step->getNext(); delete step; }
		Head=Tail=NULL;
	}
	SectRouteLink * head() { return Head; }
	SectRouteLink * tail() { return Tail; }
	SectRouteLink * addTail(SrcSect * sect,SectRouteLink * prev)
	{
		SectRouteLink * step=new SectRouteLink;
		step->Sect=sect; step->Prev=prev; step->Next=NULL;
		step->StepNum=prev?prev->StepNum+1:1;
		if (Head) { Tail->Next=step; Tail=step; } else Head=Tail=step;
		return step;
	}
};

class SectVariable
{
	char Label[10];
	SrcSect * Sect;
public:
	SectVariable() { *Label=0; Sect=NULL; }
	SectVariable(const char * l, SrcSect * s) { setLabel(l); setSect(s); }
	~SectVariable() { free(); }
	char * getLabel() { return Label; }
	SrcSect * getSect() { return Sect; }
	void setLabel(const char *label)
	{
		if (strlen(label)>9)
			error("SectVariable::setLabel(): label too long.",label);
		strcpy(Label,label);
	}
	void setSect(SrcSect * sect) { Sect=sect; }
	void free() { *Label=0; Sect=NULL; }
	int operator == (const SectVariable & var) const
		{ return strcmp(Label,var.Label)==0 && Sect==var.Sect; }
	int operator != (const SectVariable & var) const
		{ return !operator==(var); }
};

typedef ObjLink<SectVariable> SectVarLink;

class SectVarList : public ObjList<SectVariable,SectVarLink>
{
public:
	int operator == (const SectVarList & list) const
	{
		if (GetNumber()!=list.GetNumber()) return FALSE;
		SectVarLink * link1, * link2;
		for (link1=GetHead(),link2=list.GetHead();link1;
			 link1=link1->GetNext(),link2=link2->GetNext())
			if (*link1->GetData()!=*link2->GetData()) return FALSE;
		return TRUE;
	}
	int operator != (const SectVarList & list) const
		{ return !operator==(list); }
	int through(SectRouteLink * step)
	{
		SectVarLink * link;
		for (link=GetTail();link && step;link=link->GetPrev(),step=step->getPrev())
			if (step->getSect()!=link->GetData()->getSect()) return FALSE;
		return link==NULL;
	}
};

class LexAmbiguation
{
protected:
	SrcSgmt * FstSrcSgmt;
	SrcSgmt * LstSrcSgmt;
public:
	SrcSgmt * GetFstSrcSgmt() { return FstSrcSgmt; }
	SrcSgmt * GetLstSrcSgmt() { return LstSrcSgmt; }
	void SetFstSrcSgmt(SrcSgmt * sgmt) { FstSrcSgmt=sgmt; }
	void SetLstSrcSgmt(SrcSgmt * sgmt) { LstSrcSgmt=sgmt; }
	int TestCtxFunction(IToken & token);
	int TestCtxCondition(SectVariable * ctxvar,IToken & token);
	int TestSectAttribute(SrcSect * sect,IToken & token);
	int TestLeftContext(SrcSgmt * sgmt,SectVarLink * ctxvarlink,IToken & token);
	int TestRightContext(SrcSgmt * sgmt,SectVarLink * ctxvarlink,IToken & token);

	static SrcSgmt * GetMaxPrevSgmt(SrcSgmt * sgmt);
	static SrcSgmt * GetMaxSucsSgmt(SrcSgmt * sgmt);
	static const char * get_sect_ccat(SrcSect * sect)
		{ return ((SectData *)sect->getData())->ccat; }
	static const char * get_sect_yx(SrcSect * sect)
		{ return ((SectData *)sect->getData())->yx; }
	static const char * get_sect_yj(SrcSect * sect)
		{ return ((SectData *)sect->getData())->yj; }
	static int get_sect_valid(SrcSect * sect)
		{ return ((SectData *)sect->getData())->valid; }
	static void set_sect_ccat(SrcSect * sect, const char * ccat)
		{ strcpy(((SectData *)sect->getData())->ccat,ccat); }
};

class SegAmbiguation : public LexAmbiguation
{
	char SegPattern[TOKENLENGTH];
	char SegSelect1[TOKENLENGTH];
	char SegSelect2[TOKENLENGTH];
	SectVarList SectVarList1;
	SectVarList SectVarList2;
	unsigned char Validate1;
	unsigned char Validate2;
public:
	SegAmbiguation()
	{
		*SegPattern=*SegSelect1=*SegSelect2=0;
		FstSrcSgmt=LstSrcSgmt=NULL;
		Validate1=Validate2=0;
	}
	~SegAmbiguation() { free(); }
	void free()
	{
		*SegPattern=*SegSelect1=*SegSelect2=0;
		FstSrcSgmt=LstSrcSgmt=NULL;
		Validate1=Validate2=0;
		SectVarList1.free();
		SectVarList2.free();
	}
	char * GetSegPattern() { return SegPattern; }
	char * GetSegSelect1() { return SegSelect1; }
	char * GetSegSelect2() { return SegSelect2; }
	SrcSgmt * GetFstSrcSgmt() { return FstSrcSgmt; }
	SrcSgmt * GetLstSrcSgmt() { return LstSrcSgmt; }
	int GetValidate1() { return Validate1; }
	int GetValidate2() { return Validate2; }
	void AddSectVar1(int len,SrcSect * sect);
	void AddSectVar2(int len,SrcSect * sect);
	void SetPattern();
	void SetValidate1() { Validate1=1; }
	void SetValidate2() { Validate2=1; }
	SectVarList & GetSectVarList1() { return SectVarList1; }
	SectVarList & GetSectVarList2() { return SectVarList2; }
	SrcSect * SrhSectVarList(const char * label);
	int  TestSegPattern(IToken & token,int & exchange);
	int  TestSegCondition(IToken & token);
	int  TestSegEquation(IToken & token);
	void DoSegSelection(IToken & token,int exchange);
	int operator == (const SegAmbiguation & amb) const
	{
		if (FstSrcSgmt!=amb.FstSrcSgmt || LstSrcSgmt!=amb.LstSrcSgmt) return FALSE;
		//return (strcmp(SegSelect1,amb.SegSelect1)==0 &&
		//		strcmp(SegSelect2,amb.SegSelect2)==0 ||
		//		strcmp(SegSelect1,amb.SegSelect2)==0 &&
		//		strcmp(SegSelect2,amb.SegSelect1)==0    );
		return (SectVarList1==amb.SectVarList1 && SectVarList2==amb.SectVarList2 ||
				SectVarList1==amb.SectVarList2 && SectVarList2==amb.SectVarList1);
	}
	int operator != (const SegAmbiguation & amb) const
		{ return !operator==(amb); }
};

class TagAmbiguation : public LexAmbiguation
{
	SrcSect * Sect;
public:
	TagAmbiguation(SrcSect * sect) { SetSect(sect); }
	SrcSect * GetSect() { return Sect; }
	void SetSect(SrcSect * sect)
	{
		Sect=sect;
		FstSrcSgmt=sect->getFstSrcSgmt();
		LstSrcSgmt=sect->getLstSrcSgmt();
	}
	int  TestTagPattern(IToken & token,char * pattern);
	int  TestTagCondition(IToken & token);
	int  TestTagEquation(IToken & token);
	void DoTagSelection(IToken & token,const char * pattern);
};

typedef PtrLink<SegAmbiguation> SegAmbLink;
class SegAmbList : public PtrList<SegAmbiguation,SegAmbLink>
{
public:
	~SegAmbList() { freeData(); free(); }
	void AddSegAmbiguation(SegAmbiguation * amb)
	{
		SegAmbLink * link, * newlink;
		char * bgn=amb->GetFstSrcSgmt()->getBgnChar();
		char * end=amb->GetLstSrcSgmt()->getEndChar();
		if (GetHead()==NULL) { AddHead(amb); return; }
		for (link=GetHead();link;link=link->GetNext())
		{
			SegAmbiguation * amb2=(SegAmbiguation *)link->GetData();
			char * bgn2=amb2->GetFstSrcSgmt()->getBgnChar();
			char * end2=amb2->GetLstSrcSgmt()->getEndChar();
			if (bgn < bgn2) continue;
			if (bgn==bgn2 && end < end2) continue;
			if (*amb==*amb2) return;
		}
		if (link) newlink=AddBefore(amb,link);
		else newlink=AddTail(amb);
		/*
		SegAmbLink * next;
		for (link=GetHead();link;link=next)
		{
			next=link->GetNext();
			SegAmbiguation * amb2=(SegAmbiguation *)link->GetData();
			char * bgn2=amb2->GetFstSrcSgmt()->getBgnChar();
			char * end2=amb2->GetLstSrcSgmt()->getEndChar();
			if (bgn <= bgn2 && end >= end2) Delete(link);
			if (bgn >= bgn2 && end <= end2) { Delete(newlink); break; }
		}
		*/
	}
};

const int STEPNUMBERDELTA=1;

class ChnXXXTranslator : virtual public Translator
{
  protected:

	static char Yinjie[TOKENLENGTH];
	static char Hanzi[TOKENLENGTH];
	//static char SrcJianlei[TOKENLENGTH];
	//static char TgtJianlei[TOKENLENGTH];

	ChnDictn    * TheChnDictn;
	ChnDictn    * TheChnDictnref;
	ChnPrsRbase * TheChnPrsRbase;
	ChnOvlRbase * TheChnOvlRbase;
	ChnSegRbase * TheChnSegRbase;
	ChnTagRbase * TheChnTagRbase;
	ChnLexDbase * TheChnLexDbase;

	virtual void open()
	{
		TheChnDictn->use();
		TheChnDictnref->use();
		TheChnPrsRbase->use();

		TheChnOvlRbase->use();
		TheChnSegRbase->use();
		TheChnTagRbase->use();
		TheChnLexDbase->use();

		if (!TheChnDictn || TheChnDictn->getBuilt()==NO)
			error("ChnXXXTranslator::open(): Dictn not open.");
		if (!TheChnDictn->getTgtLang()==TheTgtLang)
			error("ChnXXXTranslator::open(): Dictn has error TgtLang.");
		if (!TheChnDictnref || TheChnDictnref->getBuilt()==NO)
			error("ChnXXXTranslator::open(): Dictnref not open.");
		if (!TheChnDictnref->getTgtLang()==TheTgtLang)
			error("ChnXXXTranslator::open(): Dictnref has error TgtLang.");
		if (!TheChnPrsRbase || TheChnPrsRbase->getBuilt()==NO)
			error("ChnXXXTranslator::open(): PrsRbase not open.");
		if (!TheChnPrsRbase->getTgtLang()==TheTgtLang)
			error("ChnXXXTranslator::open(): PrsRbase has error TgtLang.");

		if (!TheChnOvlRbase || TheChnOvlRbase->getBuilt()==NO)
			error("ChnXXXTranslator::open(): OvlRbase not open.");
		if (!TheChnSegRbase || TheChnSegRbase->getBuilt()==NO)
			error("ChnXXXTranslator::open(): SegRbase not open.");
		if (!TheChnTagRbase || TheChnTagRbase->getBuilt()==NO)
			error("ChnXXXTranslator::open(): TagRbase not open.");
		if (!TheChnLexDbase || TheChnLexDbase->getBuilt()==NO)
			error("ChnXXXTranslator::open(): LexDbase not open.");

		Attribute * attr;
		attr=TheModel->getAttribute("音节");
		if (attr==NULL) error("ChnXXXTranslator::open(): Model should should have a attribute named 音节.");
		if (attr->getNameAlias()==ALIAS) strcpy(Yinjie,attr->getRefer()->getString());
		else strcpy(Yinjie,attr->getString());
		attr=TheModel->getAttribute("汉字");
		if (attr==NULL) error("ChnXXXTranslator::open(): Model should should have a attribute named 汉字.");
		if (attr->getNameAlias()==ALIAS) strcpy(Hanzi,attr->getRefer()->getString());
		else strcpy(Hanzi,attr->getString());
		//attr=TheModel->getAttribute("源文兼类");
		//if (attr==NULL) error("ChnXXXTranslator::open(): Model should should have a attribute named 源文兼类.");
		//if (attr->getNameAlias()==ALIAS) strcpy(Hanzi,attr->getRefer()->getString());
		//else strcpy(SrcJianlei,attr->getString());
		//attr=TheModel->getAttribute("译文兼类");
		//if (attr==NULL) error("ChnXXXTranslator::open(): Model should should have a attribute named 译文兼类.");
		//if (attr->getNameAlias()==ALIAS) strcpy(Hanzi,attr->getRefer()->getString());
		//else strcpy(TgtJianlei,attr->getString());
	}

	virtual void close()
	{
		TheChnDictn->unuse();
		TheChnDictnref->unuse();
		TheChnPrsRbase->unuse();
		TheChnOvlRbase->unuse();
		TheChnSegRbase->unuse();
		TheChnTagRbase->unuse();
		TheChnLexDbase->unuse();
	}

	void cutWord1();
	void cutWordBack1();
	void bldWord1()=0;
	void bldWordBack1()=0;

	void disambiguation_segmentation();
	void disambiguation_tagging();
	void disambiguation_statistical();
	void disambiguation_shortest();

	void amblist_generation(SegAmbList & ambbist,SectRouteList & routelist,SrcSgmt * lastsgmt,int laststepnum);
	void amblist_processing(SegAmbList & ambbist);
	void amblist_using(SegAmbList & ambbist,SectRouteList & routelist,SrcSgmt * lastsgmt,int laststepnum);

	void setSectData(CplNode * node,const char * base,const char * vary);
	void clrSectDataAll();

	void cutSgmt(SrcSgmt * sgmt);

	static int cutText(char * text, const char * ovlrule,
				char * baseform, char * varyform);
	int cutNode(CplNode *node, const char * ovlrule);
	void adjNode(CplNode * node, const char * baseform, const char * varyform,const char * meaning);
	static void addYinjie(char * &fest, const char * varyform);
	static void addHanzi(char * &fest, const char * baseform);
	//static void addSrcJianlei(char * &fest, const char * label);
	//static void addTgtJianlei(char * &fest, const char * meaning);

	void preProcess() {};
	int  isSrcSentence(CplNode * node)
	{
		const char * label=node->getSrcLabel();
		return label && (strcmp(label,"dj")==0 ||
						 strcmp(label,"fj")==0 ||
						 strcmp(label,"zj")==0    );
	}
	//int isLegalSrcSect(SrcSect * sect);

  public:

	virtual int hasSrcLang() { return CHINESE; }
	virtual int hasTgtLang()=0;

	ChnDictn * getChnDictn() { return TheChnDictn; }
	ChnDictn * getChnDictnref() { return TheChnDictnref; }
	ChnPrsRbase * getChnPrsRbase() { return TheChnPrsRbase; }
	ChnOvlRbase * getChnOvlRbase() { return TheChnOvlRbase; }
	ChnSegRbase * getChnSegRbase() { return TheChnSegRbase; }
	ChnTagRbase * getChnTagRbase() { return TheChnTagRbase; }
	ChnLexDbase * getChnLexDbase() { return TheChnLexDbase; }

	void setChnDictn(ChnDictn * dictn)
	{
		if (getStatus()!=CLOSED)
			error("ChnXXXTranslator::setDictn(): the Translator is opened.");
		TheChnDictn=dictn;
		Translator::setDictn(dictn);
	}
	void setChnDictnref(ChnDictn * dictnref)
	{
		if (getStatus()!=CLOSED)
			error("ChnXXXTranslator::setDictnref(): the Translator is opened.");
		TheChnDictnref=dictnref;
		Translator::setDictnref(dictnref);
	}
	void setChnPrsRbase(ChnPrsRbase * rbase)
	{
		if (getStatus()!=CLOSED)
			error("ChnXXXTranslator::setPrsRbase(): the Translator is opened.");
		TheChnPrsRbase=rbase;
		Translator::setPrsRbase(rbase);
	}

	void setChnOvlRbase(ChnOvlRbase * rbase)
	{
		if (getStatus()!=CLOSED)
			error("ChnXXXTranslator::setOvlRbase(): the Translator is opened.");
		TheChnOvlRbase=rbase;
	}
	void setChnSegRbase(ChnSegRbase * rbase)
	{
		if (getStatus()!=CLOSED)
			error("ChnXXXTranslator::setSegRbase(): the Translator is opened.");
		TheChnSegRbase=rbase;
	}
	void setChnTagRbase(ChnTagRbase * rbase)
	{
		if (getStatus()!=CLOSED)
			error("ChnXXXTranslator::setTagRbase(): the Translator is opened.");
		TheChnTagRbase=rbase;
	}
	void setChnLexDbase(ChnLexDbase * dbase)
	{
		if (getStatus()!=CLOSED)
			error("ChnXXXTranslator::setLexDbase(): the Translator is opened.");
		TheChnLexDbase=dbase;
	}

	static const char * getYinjie() { return Yinjie; }
	static const char * getHanzi() { return Hanzi; }

	static const char * get_sect_ccat(SrcSect * sect)
		{ return ((SectData *)sect->getData())->ccat; }
	static const char * get_sect_yx(SrcSect * sect)
		{ return ((SectData *)sect->getData())->yx; }
	static const char * get_sect_yj(SrcSect * sect)
		{ return ((SectData *)sect->getData())->yj; }
	static int get_sect_valid(SrcSect * sect)
		{ return ((SectData *)sect->getData())->valid; }
	static void set_sect_valid(SrcSect * sect,int v)
		{ ((SectData *)sect->getData())->valid=v; }

	double compute_lex_score(CplNode * node,NodeRouteLink * step);
	double compute_lex_score_begin(CplNode * node);
	double compute_lex_score_end(CplNode * node);

#ifdef KBMS
	void output_nopathfind();
	void output_tagging(TagAmbiguation & amb, const char * rule,
						const char * pntr1, const char * pntr2);
	void output_segmentation(SegAmbiguation & amb, const char * rule=NULL,
							 const char * pntr1=NULL, const char * pntr2=NULL);
#endif

};

class XXXEngTranslator : virtual public Translator
{

  protected:

	EngMakRbase * TheEngMakRbase;
	EngBldRbase * TheEngBldRbase;

	virtual void open()
	{
		TheEngMakRbase->use();
		TheEngBldRbase->use();

		if (!TheEngMakRbase || TheEngMakRbase->getBuilt()==NO)
			error("XXXEngTranslator::open(): MakRbase not open.");
		if (!TheEngBldRbase || TheEngBldRbase->getBuilt()==NO)
			error("XXXEngTranslator::open(): BldRbase not open.");
	}

	virtual void close()
	{
		TheEngMakRbase->unuse();
		TheEngBldRbase->unuse();
	}

	void cutWord1()=0;
	void cutWordBack1()=0;
	void bldWord1();
	void bldWordBack1();

	int bldSgmt(TgtSgmt * sgmt,const char * bldrule);
	void bldText(TgtSgmt * sgmt,char * &text);

	void postProcess(char * text);
	int  isTgtSentence(CplNode * node)
	{
		const char * label=node->getTgtLabel();
		return label && (strcmp(label,"CS")==0 ||
						 strcmp(label,"SS")==0    );
	}

  public:

	virtual int hasSrcLang()=0;
	virtual int hasTgtLang() { return ENGLISH; }

	EngMakRbase * getEngMakRbase() { return TheEngMakRbase; }
	EngBldRbase * getEngBldRbase() { return TheEngBldRbase; }

	void setEngMakRbase(EngMakRbase * rbase)
	{
		if (getStatus()!=CLOSED)
			error("XXXEngTranslator::setMakRbase(): the Translator is opened.");
		TheEngMakRbase=rbase;
		Translator::setMakRbase(rbase);
	}

	void setEngBldRbase(EngBldRbase * rbase)
	{
		if (getStatus()!=CLOSED)
			error("XXXEngTranslator::setBldRbase(): the Translator is opened.");
		TheEngBldRbase=rbase;
	}

};

class ChnEngTranslator : public ChnXXXTranslator, public XXXEngTranslator
{

  protected:

	void cutWord1() { ChnXXXTranslator::cutWord1(); }
	void cutWordBack1() { ChnXXXTranslator::cutWordBack1(); }
	void bldWord1() { XXXEngTranslator::bldWord1(); }
	void bldWordBack1() { XXXEngTranslator::bldWordBack1(); }

	void preProcess() { ChnXXXTranslator::preProcess(); }
	void postProcess(char * text) { XXXEngTranslator::postProcess(text); }

	int  isSrcSentence(CplNode * node) { return ChnXXXTranslator::isSrcSentence(node); }
	int  isTgtSentence(CplNode * node) { return XXXEngTranslator::isTgtSentence(node); }
	int  isLegalSrcSect(SrcSect * sect) { return ChnXXXTranslator::isLegalSrcSect(sect); }

  public:

	virtual void open()
	{
		if (getStatus()!=CLOSED) return;
		try
		{
			Translator::open();
			try
			{
				ChnXXXTranslator::open();
				try
					{ XXXEngTranslator::open(); }
				catch (CErrorException *)
					{ XXXEngTranslator::close(); throw; }
			}
			catch (CErrorException *)
				{ ChnXXXTranslator::close(); throw; }
		}
		catch (CErrorException *)
			{ Translator::close(); throw; }
	}

	virtual void close()
	{
		if (getStatus()==CLOSED) return;
		XXXEngTranslator::close();
		ChnXXXTranslator::close();
		Translator::close();
	}

	virtual int hasSrcLang() { return ChnXXXTranslator::hasSrcLang(); }
	virtual int hasTgtLang() { return XXXEngTranslator::hasTgtLang(); }

};

#endif /* CETRANS_H */
