
#ifndef CUTRBASE_H

#define CUTRBASE_H

#include "kbase.h"
#include "mkbase.h"

class CutRbase : public MKbase
{

  protected:

	int SrcLang;
	PTIdxBuffList CutRuleList;

#ifdef KBMS
	virtual BOOL rdTxtCutRule()=0;

	virtual int LoadCutRule(IOToken &TheToken,int srclang) = 0;
#endif

	void rdCfg();
	void testCfg();
	void wtCfg();
	void initCfg();

	void rdSrcLang();
	void wtSrcLang();

	void ropen();

#ifdef KBMS
	void wopen();
#endif

	void close();

  public:
	
	CutRbase() { }
	~CutRbase() { free(); }

	char * hasType() { return "CUTRBASE"; }

	virtual int hasSrcLang() = 0;

#ifdef KBMS
	void Load();
#endif

	void build();
	void free();

	cRuleList & rd_CutRules(const char *key);

};
#endif // CUTRBASE_H

