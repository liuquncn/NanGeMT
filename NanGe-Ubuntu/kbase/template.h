#ifndef TEMPLATE_H

#define TEMPLATE_H

#include "mkbase.h" 
#include "string.h"

#define MAXSTRLENGTH 32
     
#define AC_DISTINGUISH 0x01; /*  区别属性  */
#define AC_COMMON     0x02;  /*  一般属性  */
#define AC_NOVALUE    0x04;  /*  无值属性  */

#define AC_ASSOCIATE  0x08   /*  关联属性  */


class AttrVal
{
		unsigned int AttClass;
		char AssAttName[MAXSTRLENGTH];
		char AttrName[MAXSTRLENGTH];
		char Value[MAXSTRLENGTH];
		AttrVal *next;
	public:
		AttrVal() 
		{ 
			strcpy(AssAttName,"");
			strcpy(AttrName,"");
			strcpy(Value,"");
			next=NULL; 
		}
		void SetAssAttName(const char *Name) { strcpy(AssAttName,Name);}
		void SetAttClass(unsigned int AttributeClass) 
		{ 
			AttClass = AttributeClass;
		}
		void SetName(const char *Name ) { strcpy(AttrName,Name);}
		void SetValue(const char *val ) { strcpy(Value,val);}
		void SetNext(AttrVal * nextptr) { next = nextptr;}
		AttrVal * getNext() { return next; }
		char * getAssAttName() { return AssAttName;}
		char * getName(){ return AttrName;}
		unsigned int getAttClass() { return AttClass;}
		char * getValue() { return Value;}
		
		BOOL IsDistinguish()
		{
			return AttClass&AC_DISTINGUISH;
		}
		BOOL IsCommon()
		{
			return AttClass&AC_COMMON;  
		}                             
		BOOL IsNoValue()
		{
			return AttClass&AC_NOVALUE;  
		}                                          
		BOOL IsAssociate()
		{
			return AttClass&AC_ASSOCIATE;
		}  
};

class AttrValList
{
		AttrVal * AttListHead;
	public:
		AttrValList() { AttListHead = NULL;}
		AttrVal * getAttListHead() { return AttListHead;}
		AttrVal * IsAnAtt(AttrVal *av);
		void AddAAttrVal( AttrVal *av);
		char * GetValueOfAttribute(const char *szAttributeName);
		void free();	 
};
class TargetTPL
{
		int LangCode;
		char Name[MAXSTRLENGTH];
		TargetTPL *next;
	public:
		TargetTPL() { LangCode=-1;strcpy(Name,"");next=NULL;}
		int getLangCode() { return LangCode;}
		char * getName() { return Name;}
		void setLangCode(const int i) { LangCode=i;}
		void setName(const char *str) { strcpy(Name,str);}
		TargetTPL * getNext() { return next;}		
		void setNext(TargetTPL *ttpl) { next=ttpl;} 
	
};
class TargetList
{
		TargetTPL *TargetHead;
	public:
		TargetList() { TargetHead=NULL;} 
		TargetTPL * getTargetHead() { return TargetHead;}
 		int DispAllTarget(); 		
		char *findTarget(int no);
		TargetTPL * IsATarget(const char *Name);
		void AddATarget( TargetTPL *ttpl);
		void free();		
};

class TPL;
class FinalTPL
{
	protected:
		int LangCode;
		TargetList Target;
		AttrValList AttrList;
	public:
		class AttrValList &getAttrList() { return AttrList;}	
		FinalTPL(){ LangCode = -1;}
		class TargetList &getTarget() { return Target;}	    
		int DispAllTarget() { return Target.DispAllTarget();}
		char *findTarget(int no) { return Target.findTarget(no);}
		void combine(FinalTPL *tpl);
		void compute(TPL *tpl);
		void free();
};

class TPL:public FinalTPL
{
		char Name[MAXSTRLENGTH];
		TPL *Parent;
		TPL *firstChild;
		TPL *nextBrother;
	public:
		TPL() 
		{ 
			strcpy(Name,"");
			Parent=NULL;
			firstChild=nextBrother=NULL;
		}
	    char * getName() { return Name;}
	    TPL * getFirstChild() { return firstChild;}
	    TPL * getNextBrother() { return nextBrother;}
		void setFirstChild(TPL * tpl) { firstChild=tpl;}
		void setNextBrother(TPL *tpl) { nextBrother=tpl;}
		void setName(const char *str) { strcpy(Name,str);}
	    void setParent(  TPL * tpntr) { Parent = tpntr;}
	    TPL * getParent() { return Parent;}
	    void AddAChild(  TPL * tpntr); //为该模板增加一个子模板
        void AddABrother( TPL *tpntr); //为该模板增加一个兄弟模板
        int DispAllChild();
		char * findChild(int no);
        void free();
};
class Template:public MKbase
{
		TPL TplTree[LANGNUM];

	public:

		int  DispChild(int Lang,char *tplname);
		char *findChild(const char *tplName,int LangCode,int no);
		TPL *TreeAddress(int Langcode);

	private:	
		
		TPL * preOrder(const char *tplName,TPL * tpl);

#ifdef KBMS
		int   LoadTPLs(IOToken &TheToken);
		int	  LoadLangCode(IOToken &TheToken);
		void  LoadTPLsBody(IOToken &TheToken,int lang); 

		void  LoadATPL(IOToken &TheToken,int lang);

		void  loadName(IOToken &TheToken,TPL *tpl);
		void  loadParent(IOToken &TheToken,TPL *tpl,int lang);
		void  loadTarget(IOToken &TheToken,TPL *tpl);
		void  loadAttVal(IOToken &TheToken,TPL *tpl);
#endif

		void ReadTPLs(IToken &TheToken);
		int	 ReadLangCode(IToken &TheToken);
		void ReadTPLsBody(IToken &TheToken,int lang);
		void ReadATPL(IToken &TheToken,int lang);
		void ReadName(IToken &TheToken,TPL *tpl);
		void ReadParent(IToken &TheToken,TPL *tpl,int lang);
		void ReadTarget(IToken &TheToken,TPL *tpl);
		void ReadAttVal(IToken &TheToken,TPL *tpl);

	public:
		void printout();
        void build();
		void free();
#ifdef KBMS
		void Load();
		BOOL rdTxtTpl();
#endif

		TPL * find(const char *tplName,int LangCode);
		char * hasType() { return "TEMPLATE";}	
	    Template() { }
	    ~Template(){ free();} 
};
#endif //TEMPLATE_H
