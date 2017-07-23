
#ifndef MKBASE_H

#define MKBASE_H

#include "model.h"
#include <time.h>

const int TITLELENGTH=8;
const int PATTNLENGTH=8;

class RuleLink
{
	char * Rule;
	RuleLink * Next;

public:

	RuleLink() { Rule=NULL; Next=NULL; }
	RuleLink(char *rule) { Rule=rule; Next=NULL;}

	const char * getRule(){ return Rule;}
	RuleLink * getNext(){ return Next;}

	void setRule(char *rule) { Rule=rule; }
	void setNext(RuleLink *nxtrule){ Next=nxtrule; }

	friend class cRuleList;

};

class cRuleList
{
	RuleLink *RuleLinkHead;
	RuleLink *RuleLinkTail;

	cRuleList(cRuleList &) { error("RuleList: cannot copy to other list."); }
	void operator=(cRuleList &) {error("RuleList: cannot copy to other list.");}

public:

	cRuleList(){ RuleLinkHead=NULL; RuleLinkTail=NULL; }
	~cRuleList() { free(); }

	void free();
	void freeRules();

	RuleLink *getRuleLinkHead(){ return RuleLinkHead; }
	void setRuleLinkHead(RuleLink *rulelink) { RuleLinkHead=rulelink; }

	RuleLink *find(char *title); 
	void del(char *rule);
	void addhead(char *rule);
	void addtail(char *rule);
	void OutPut(OToken &token);
};

class PTIdxBuffUnit
{
	char *Pattn;
	cRuleList RuleList;
	PTIdxBuffUnit * Next;

public:

	PTIdxBuffUnit(){ Pattn=NULL; Next=NULL; }
	~PTIdxBuffUnit() { free(); }

	void free() { delete Pattn; Pattn=NULL; RuleList.freeRules(); }

	const char * getPattn(){ return Pattn; }
	class cRuleList & getRuleList(){ return RuleList; }
	PTIdxBuffUnit * getNext() { return Next; }

	void setPattn(const char *pattn)
		{ Pattn=new char[strlen(pattn)+1]; strcpy(Pattn,pattn); }
	void addRuleHead(const char * rule)
		{ char * r=new char[strlen(rule)+1]; strcpy(r,rule); RuleList.addhead(r); }
	void addRuleTail(const char * rule)
		{ char * r=new char[strlen(rule)+1]; strcpy(r,rule); RuleList.addtail(r); }

	friend class PTIdxBuffList;
};

class PTIdxBuffList
{
	PTIdxBuffUnit * FstUnit;

  public:

	PTIdxBuffList() { FstUnit=NULL; }
	~PTIdxBuffList() { free(); }

	void free();

	PTIdxBuffUnit * fstUnit() { return FstUnit; }
	PTIdxBuffUnit * srhUnit(const char * pattn);
	PTIdxBuffUnit * addUnitHead(const char * pattn,const char * rule);
	PTIdxBuffUnit * addUnitTail(const char * pattn,const char * rule);
};

class TitleLink
{
	char Title[TOKENLENGTH];
	TitleLink *Next;
public:
	TitleLink(char *title) { strcpy(Title,title); Next=NULL; }
	~TitleLink() { *Title=0;}
	void setTitle(const char *title) { strcpy(Title,title); }
	void setNext(TitleLink *next) { Next=next; }
	const char *getTitle() { return Title; }
	TitleLink *getNext() { return Next; }
};

class TitleList
{
	TitleLink *TitleLinkHead;
	TitleLink *TitleLinkTail;
public:
	TitleList() { TitleLinkHead=TitleLinkTail=NULL; }
	~TitleList() { free(); }
	TitleLink *getTitleLinkHead() { return TitleLinkHead; }
	 void free() 
		{
			TitleLink *link=TitleLinkHead;
			TitleLink *next;
			while (link) {
				next=link->getNext();
				delete link;
				link=next;
			}		
			TitleLinkHead=TitleLinkTail=NULL;
		}
	void addto(char *title,const char *err=NULL) 
		{
			if(find(title))  error("TitleList: the same titlename.",title,err);
			TitleLink *titlelink=new TitleLink(title);
			if(TitleLinkHead) {
				TitleLinkTail->setNext(titlelink);
				TitleLinkTail=titlelink;
				TitleLinkTail->setNext(NULL);
			}
			else TitleLinkHead=TitleLinkTail=titlelink;
		}
	void del(char *title)
		{
			if (!find(title)) return;
			TitleLink *link=TitleLinkHead;
			TitleLink *prelink=link;
			while (link) {
				if (strcmp(link->getTitle(),title)) {
					prelink=link;
					link=link->getNext();
				}
				else break;
			}
			if (link==TitleLinkHead) {
				if (TitleLinkHead==TitleLinkTail) 
					TitleLinkHead=TitleLinkTail=NULL;
				else TitleLinkHead=link->getNext(); 
			}
			else if (link==TitleLinkTail) {
				TitleLinkTail=prelink;
				TitleLinkTail->setNext(NULL);	
			}
			else prelink->setNext(link->getNext());
			delete link;
		}
	TitleLink *find(char *title) 
		{
			TitleLink *link=TitleLinkHead;
			while (link) {
				if (!strcmp(link->getTitle(),title))  return link;
				else link=link->getNext();
			}
			return NULL;
		}
};

extern Model * GlobalModel;

class MKbase : public Kbase
{
	char      ModelPath[FILENAMELENGTH];   
	char      ModelName[FILENAMELENGTH];  
	time_t    ModelDate; 		
protected:
	static	Model*  mkModel;

	virtual void setBuilt(int built)
	{
		Kbase::setBuilt(built);
		if (built) mkModel->use(); else mkModel->unuse();
	};

	virtual void rdCfg();
	virtual void testCfg();
	virtual void wtCfg();
	virtual void initCfg();

	virtual void rdModelPath();
	virtual void rdModelName();
	virtual void rdModelDate();

	virtual void wtModelPath();
	virtual void wtModelName();
	virtual void wtModelDate();
public:
	MKbase() { if (!mkModel) mkModel=GlobalModel; };
	~MKbase(){ free(); };

	virtual char * hasType()=0;

	virtual void testSituation();

#ifdef KBMS
	static void LoadWordItem_OR(IOToken &TheToken,const char *err=NULL);
	static void LoadWordItem(IOToken &TheToken,const char *err=NULL);
	static void LoadLabelItem_OR(IOToken &TheToken,int lang,const char *err=NULL);
	static void LoadLeftSubTree(IOToken &TheToken,int lang,const char *err=NULL);
	static void LoadRightSubTree(IOToken &TheToken,int lang,int prelang,const char *err=NULL);
	static void LoadLeftSubForest(IOToken &TheToken,int lang,const char *err=NULL);
	static void LoadRightSubForest(IOToken &TheToken,int lang,int prelang,const char *err=NULL);
	static void LoadLeftLabelItem(IOToken &TheToken,int lang,const char *err=NULL);
	static void LoadRightLabelItem(IOToken &TheToken,int lang,int prelang,const char *err=NULL);
	static void LoadShiftLabelItem(IOToken &TheToken,int lang,const char *err=NULL);
	static void LoadAt(IOToken &TheToken,const char *err=NULL);
	static void LoadHeadTag(IOToken &TheToken);
	static void LoadMoveTag(IOToken &TheToken);
	static void LoadCopyTag(IOToken &TheToken);
	static void LoadRemvTag(IOToken &TheToken);
	static void LoadRootVar(IOToken &TheToken);
	static void LoadLabelVarTag(IOToken &TheToken);
	static void LoadTransVarTag(IOToken &TheToken,const char *err=NULL);
	static void LoadLabelVar(IOToken &TheToken,int lang,const char *err=NULL);
	static void LoadTransVar(IOToken &TheToken,int lang,const char *err=NULL);
	static BOOL LoadExtIntVar(IOToken &TheToken,int lang,Attribute *&attr,const  char *err=NULL);

    static void LoadPrsStruct(IOToken &TheToken,int lang, const char *err=NULL);
    static void LoadPrsReductn(IOToken &TheToken,int lang, const char *err=NULL);
    static void LoadPrsBind(IOToken &TheToken,int lang, const char *err=NULL);
    static void LoadPrsForest(IOToken &TheToken,int lang, const char * err=NULL);

	static void LoadTitle(IOToken &TheToken,TitleList &titlelist,const char *err=NULL);
    static void LoadTrnTree(IOToken &TheToken,int lang,int prelang, const char *err=NULL);
    static void LoadTrnBind(IOToken &TheToken,int lang, const char *err=NULL);
	static void LoadTrnSwitch(IOToken &TheToken,int lang, const char *err=NULL);
	static void LoadWord(IOToken &TheToken, const char *err=NULL);
	static void LoadNote(IOToken &TheToken, const char *err=NULL);
	static void LoadLabelItem(IOToken &TheToken,int lang, const char *err=NULL);
	static void LoadLeftForest(IOToken &TheToken,int lang, const char *err=NULL);
	static void LoadRightForest(IOToken &TheToken,int lang,int prelang, const char *err=NULL);
	static void LoadLeftTree(IOToken &TheToken,int lang, const char *err=NULL);
	static void LoadRightTree(IOToken &TheToken,int lang,int prelang, const char *err=NULL);
	static void LoadEquation(IOToken &TheToken,int lang, const char *err=NULL);
	static void LoadTest(IOToken &TheToken,int lang, const char *err=NULL);
	static void LoadBind(IOToken &TheToken,int lang, const char *err=NULL);
	static void LoadFest(IOToken &TheToken, const char *err=NULL);
	static int  LoadPrsRule(IOToken &TheToken,int lang,int prelang,TitleList &titlelist,const char *err=NULL);
    static void LoadTrnStruct(IOToken &TheToken,int lang,int prelang, const char *err=NULL);

	virtual void Load()=0;
protected:
	virtual void ropen()
	{
		try { mkModel->use(); Kbase::ropen(); }
		catch (CErrorException *)
			{ if (getStatus()==CLOSED) mkModel->unuse(); throw; }
	}
	virtual void close()
	{
		if (getStatus()!=CLOSED) mkModel->unuse();
		Kbase::close();
	}
#endif

public:
	virtual void build() { Kbase::build(); };
	void free()
	{
		if (getBuilt()==NO) return;
		if (mkModel && mkModel->getUsernum()==0) mkModel=NULL;
		Kbase::free();
	};

	char  *	getModelPath() { return ModelPath;};
	char  *	getModelName() { return ModelName;};
	long    getModelDate() { return ModelDate;};
	void    setModelDate(time_t date) { ModelDate=date; }
#ifdef KBMS
protected:
	virtual void wopen()
	{
		try { mkModel->use(); Kbase::wopen(); }
		catch (CErrorException *)
			{ if (getStatus()==CLOSED) mkModel->unuse(); throw; }
	}
#endif /* KBMS */
};

#endif

