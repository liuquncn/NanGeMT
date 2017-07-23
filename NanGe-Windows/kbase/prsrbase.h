
#ifndef PRSRBASE_H

#define PRSRBASE_H

#include "mkbase.h"

class PrsRbase: public MKbase
{

  protected:

	int SrcLang;
	int TgtLang;
	PTIdxBuffList PTIdxBuff;

	void rdCfg();
	void testCfg();
	void wtCfg();
	void initCfg();

	void ropen();

#ifdef KBMS
	void wopen();
#endif

	void close();

#ifdef KBMS
    BOOL rdTxtPrsRule();
#endif

	void rdSrcLang();
	void rdTgtLang();

	void wtSrcLang();
	void wtTgtLang();

public:

	PrsRbase() { }
	~PrsRbase() { free(); }

	char * hasType(){ return "PRSRBASE"; }

	virtual int hasSrcLang()=0;

	void setTgtLang(int lang)
		 {
			if (getStatus()!=CLOSED) error("PrsRbase:: set TgtLang while OPEN");
			TgtLang=lang;
		 };
	int getTgtLang(){ return TgtLang; };

	static void getPattn(const char *prsrule,char pattn[]);

	static void getTitle(const char *prsrule,char title[]);

#ifdef KBMS
	void Load();
#endif

	void build();
	void free();

	cRuleList & rd_PrsRulesPT(const char *pattn);
	const char * rd_PrsRulesTT(const char *title);
};


#endif // PRSRBASE_H

