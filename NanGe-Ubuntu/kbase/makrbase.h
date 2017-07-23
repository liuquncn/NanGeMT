
#ifndef MAKRBASE_H

#define MAKRBASE_H

#include "mkbase.h"

class MakRbase : public MKbase
{
protected:
	static MakRbase * TheMakRbase;
	int SrcLang;
	int TgtLang;
	PTIdxBuffList  MakRuleList;
	void rdCfg();
	void testCfg();
	void wtCfg();
	void initCfg();
	void rdSrcLang();
	void wtSrcLang();
	void rdTgtLang();
	void wtTgtLang();
	void ropen();
	void close();
#ifdef KBMS
	BOOL rdTxtMakRule();
	virtual int LoadMakRule(IOToken &TheToken,int tgtlang,TitleList &newtitlelist) = 0;
	void wopen();
#endif
public:
	MakRbase() { }
	~MakRbase() { free(); }
	char * hasType() { return "MAKRBASE"; }
	static MakRbase * getMakRbase() { return TheMakRbase; }
	int getSrcLang() { return SrcLang; }
	int getTgtLang() { return TgtLang;}
	BOOL setSrcLang(int lang)
		 { 
			if( getStatus()!=CLOSED ) return FAIL;
			 else { SrcLang=lang; return SUCC; }
		 } 	

	virtual int hasTgtLang() = 0;
	static void getPattn( const char *makrule,char pattn[]);
	void build();
	void free();
	cRuleList & rd_MakRulesPT(const char *);

#ifdef KBMS
	void Load();
#endif 
};

#endif

