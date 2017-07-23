
#ifndef MODEL_H

#define MODEL_H


#include "kbase.h"

class Model;
class SemModel;
class SynModel;

class SemModel 
{
	unsigned Has;
    UnsortAttList SemCat;   			
	CondList SemAttCondList;			 
	CondList SemAssCondList;
public:
	SemModel(){Has=NO;}
	~SemModel() { free(); }
	void free()
	{
		SemCat.free();
		SemAttCondList.free();
		SemAssCondList.free();
	}
	void setHas(unsigned ch){ Has=ch;}
	void setSemCat(class UnsortAttList &semcat){ SemCat=semcat;}
	unsigned getHas(){ return Has;}
	class UnsortAttList & getSemCat(){ return SemCat; }
	class CondList &getSemAttCondList(){ return SemAttCondList; }
	class CondList &getSemAssCondList(){ return SemAssCondList; }
};

class SynModel 
{
	unsigned Has;
	UnsortAttList LexCat;
	UnsortAttList PhrCat;
	UnsortAttList BaseForm;
	UnsortAttList VaryForm;
	char *DgtCat;
	char *PctCat;
	char *OthCat;
	CondList LexStaDynCondList;
	CondList SynAssCondList;
	CondList PhrCondList;
public:
	SynModel(){ Has=NO; DgtCat=PctCat=OthCat=NULL; }
	~SynModel() { free(); }
	void free()
	{
		LexCat.free();
		PhrCat.free();
		BaseForm.free();
		VaryForm.free();
		LexStaDynCondList.free();
		PhrCondList.free();
		SynAssCondList.free();
		if (DgtCat) { delete DgtCat; DgtCat=NULL; }
		if (PctCat) { delete PctCat; PctCat=NULL; }
		if (OthCat) { delete OthCat; OthCat=NULL; }
	}
	void setHas(unsigned ch){ Has=ch;}
	void setDgtCat(char * field) { DgtCat=field; }
	void setPctCat(char * field) { PctCat=field; }
	void setOthCat(char * field) { OthCat=field; }
	unsigned getHas(){ return Has;}
	class UnsortAttList &getLexCat(){ return LexCat;}
	class UnsortAttList &getPhrCat(){ return PhrCat;}
	class UnsortAttList &getBaseForm(){ return BaseForm;}
	class UnsortAttList &getVaryForm(){ return VaryForm;}
	class CondList &getLexStaDynCondList(){ return LexStaDynCondList;}
	class CondList &getSynAssCondList(){ return SynAssCondList;}
	class CondList &getPhrCondList(){ return PhrCondList;}
	char *getDgtCat() { return DgtCat; }
	char *getPctCat() { return PctCat; }
	char *getOthCat() { return OthCat; }
};

class Model :public Kbase
{
	SemModel asemModel;					
	SynModel asynModel[LANGNUM];	
	class SortedAttList SortedAttList;
protected:
	void ropen(); 
	void close();
#ifdef KBMS 
	void wopen();
	// read SemModel or SynModel from TxtFile to TxtBuff 
	// and set InPntr=TxtBuff OutPntr=DatBuff
 	BOOL rdTxtSynModel();	
	void rdTxtSemModel();	
#endif // KBMS
public:
	Model(){}    					
	~Model() { free(); } 			       		
	char * hasType() { return  "MODEL"; }
	BOOL hasLang(int LangCode);
	class SemModel &getasemModel(){ return asemModel;}
	class SynModel &getasynModel(int lang){ return asynModel[lang];}
	class SortedAttList &getSortedAttList(){ return SortedAttList;}
	Attribute * getAttribute(const char string[]);
	Attribute * getLexCat(int i);
	Attribute * getPhrCat(int i);
	Attribute * getBaseForm(int i);
	Attribute * getVaryForm(int i);
	Attribute * getSemCat();
	char * getDgtCat(int i) { return asynModel[i].getDgtCat(); }
	char * getPctCat(int i) { return asynModel[i].getPctCat(); }
	char * getOthCat(int i) { return asynModel[i].getOthCat(); }
#ifdef KBMS
	void LoadSemModelBody(IOToken &TheToken);
	void LoadSemFeat(IOToken &TheToken);
	void LoadSemCat(IOToken &TheToken);
	void LoadSemAtt(IOToken &TheToken);
	void LoadSemVal(IOToken &TheToken);
	void LoadSemAss(IOToken &TheToken);
	void LoadSemAssAtt(IOToken &TheToken);
	void LoadSemAssVal(IOToken &TheToken);
	int	 LoadLangCode(IOToken &TheToken);
	void LoadSynModelBody(IOToken &TheToken,int lang);
	void LoadLexCat(IOToken &TheToken,int lang);
	void LoadLexFeat(IOToken &TheToken,int lang);
	void LoadLexAtt(IOToken &TheToken,int lang);
	void LoadLexVal(IOToken &TheToken,int lang);
	void LoadPhrCat(IOToken &TheToken,int lang);
	void LoadPhrFeat(IOToken &TheToken,int lang);
	void LoadPhrAtt(IOToken &TheToken,int lang);
	void LoadPhrVal(IOToken &TheToken,int lang);
	void LoadSynAss(IOToken &TheToken,int lang);
	void LoadSynAssAtt(IOToken &TheToken,int lang);
	void LoadSynAssVal(IOToken &TheToken);
	void LoadSpecialField(IOToken &TheToken,int lang);
	void LoadDigitField(IOToken &TheToken,int lang);
	void LoadPunctField(IOToken &TheToken,int lang);
	void LoadOtherField(IOToken &TheToken,int lang);

	void LoadAtomVal(IOToken &TheToken,Attribute *attr);
	void LoadAtomValType(IOToken &TheToken,Attribute *attr);
	void LoadValMode(IOToken &TheToken,Attribute *attr);
	void LoadActMode(IOToken &TheToken,Attribute *attr);
	void LoadRelMode(IOToken &TheToken,Attribute *attr);
	void LoadFestVal(IOToken &TheToken,Attribute *attr);
	void LoadAtt_Set(IOToken &TheToken,
					 class SortedAttList &attlist,
					 class UnsortAttList &unattlist,
					 int lang,unsigned lexphr,unsigned intext,
					 unsigned stadyn,unsigned relative, 
					 unsigned atttype=0,unsigned limit=0);
	void LoadAttribute(IOToken &TheToken,
					   class SortedAttList &attlist,
					   class UnsortAttList &unattlist,
					   unsigned duppermit,int lang,
					   unsigned lexphr,unsigned intext,
					   unsigned stadyn,unsigned relative,
					   unsigned atttype=0,unsigned limit=0);
	Attribute *LoadAtt_Name(IOToken &TheToken,Attribute  *attr);
	Attribute *LoadAtt_Alias(IOToken &TheToken,Attribute *attr);
	void LoadHieSym_Set(IOToken &TheToken,class cValList &vallist);
	void LoadHieSym_Atom(IOToken &TheToken,class cValList &vallist);
	void LoadAtom_Name(IOToken &TheToken,Value *val);
	void LoadAtom_Alias(IOToken &TheToken,Value *val);
	void LoadNumber_Set(IOToken &TheToken,class cValList &vallist);
	void LoadBoolean_Set(IOToken &TheToken,class cValList &vallist);
	void LoadNumber_Atom(IOToken &TheToken,class cValList &vallist);
	void LoadBoolean_Atom(IOToken &TheToken,class cValList &vallist);
	char *LoadCondition(IOToken &TheToken,class SortedAttList &list);
	unsigned LoadAtomType(IOToken &TheToken,Attribute *attr);
	void LoadDefAtom(IOToken &TheToken,unsigned type,Attribute *attr);
	void LoadDefFest(IOToken &TheToken,Attribute *attr);
	int  LoadSemModel(IOToken &TheToken);				
	int  LoadSynModel(IOToken &TheToken);			
#endif // KBMS
	void ReadSemModelBody(IToken &TheToken);
	void ReadSynModelBody(IToken &TheToken,int lang);
	void ReadSemFeat(IToken &TheToken);
	void ReadSemCat(IToken &TheToken);
	void ReadSemAtt(IToken &TheToken);
	void ReadSemVal(IToken &TheToken);
	void ReadSemAss(IToken &TheToken);
	void ReadSemAssAtt(IToken &TheToken);
	void ReadSemAssVal(IToken &TheToken);
	int	 ReadLangCode(IToken &TheToken);
	void ReadLexFeat(IToken &TheToken,int lang);
	void ReadLexCat(IToken &TheToken,int lang);
	void ReadLexAtt(IToken &TheToken,int lang);
	void ReadLexVal(IToken &TheToken);
	void ReadPhrFeat(IToken &TheToken,int lang);
	void ReadPhrCat(IToken &TheToken,int lang);
	void ReadPhrAtt(IToken &TheToken,int lang);
	void ReadPhrVal(IToken &TheToken);
	void ReadSynAss(IToken &TheToken,int lang);
	void ReadSynAssAtt(IToken &TheToken,int lang);
	void ReadSynAssVal(IToken &TheToken);
	void ReadSpecialField(IToken &TheToken,int lang);
	void ReadDigitField(IToken &TheToken,int lang);
	void ReadPunctField(IToken &TheToken,int lang);
	void ReadOtherField(IToken &TheToken,int lang);
	void ReadAtomVal(IToken &TheToken,Attribute *attr);
	void ReadAtomValType(IToken &TheToken,Attribute *attr);
	void ReadValMode(IToken &TheToken,Attribute *attr);
	void ReadActMode(IToken &TheToken,Attribute *attr);
	void ReadRelMode(IToken &TheToken,Attribute *attr);
	void ReadFestVal(IToken &TheToken,Attribute *attr);
	void ReadAtt_Set(IToken &TheToken,class SortedAttList &attlist,class UnsortAttList &unattlist,unsigned lang,unsigned intext,unsigned lexphr,unsigned stadyn=3, unsigned relative=0,unsigned atttype=0,unsigned limit=0);
	void ReadAttribute(IToken &TheToken,class SortedAttList &attlist,class UnsortAttList &unattlist,unsigned lang,unsigned duppermit,unsigned intext, unsigned lexphr,unsigned stadyn=3,unsigned relative=0, unsigned atttype=0,unsigned limit=0);
	Attribute *ReadAtt_Name(IToken &TheToken,Attribute  *attr);
	Attribute *ReadAtt_Alias(IToken &TheToken,Attribute *attr);
	void ReadHieSym_Set(IToken &TheToken,class cValList &vallist);
	void ReadHieSym_Atom(IToken &TheToken,class cValList &vallist);
	void ReadAtom_Name(IToken &TheToken,Value *val);
	void ReadAtom_Alias(IToken &TheToken,Value *val);
	void ReadNumber_Set(IToken &TheToken,class cValList &vallist);
	void ReadBoolean_Set(IToken &TheToken,class cValList &vallist);
	void ReadNumber_Atom(IToken &TheToken,class cValList &vallist);
	void ReadBoolean_Atom(IToken &TheToken,class cValList &vallist);
	char *ReadCondition(IToken &TheToken);
	unsigned ReadAtomType(IToken &TheToken,Attribute *attr);
	void ReadDefAtom(IToken &TheToken,unsigned type,Attribute *attr);
	void ReadDefFest(IToken &TheToken,Attribute *attr);
	void ReadSemModel(IToken &TheToken);					
	void ReadSynModel(IToken &TheToken);				
#ifdef KBMS
	void Load();
#endif // KBMS
    void build();     		
    void free();
};


#endif // MODEL_H
