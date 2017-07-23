
#ifndef BLDRBASE_H

#define BLDRBASE_H

#include "mkbase.h"

const int KEYLENGTH=1;

class BldRbase : public MKbase
{
  protected:

	int TgtLang;
	PTIdxBuffList BldRuleList;

	void rdCfg();
	void testCfg();
	void wtCfg();
	void initCfg();

	void rdTgtLang();
	void wtTgtLang();

	void ropen();

#ifdef KBMS
	void wopen();

	BOOL rdTxtBldRule();
#endif

	void close();

#ifdef KBMS
	virtual int LoadBldRule(IOToken &TheToken,int tgtlang)=0; 
#endif

  public:
	
	BldRbase() { }
	~BldRbase() { free(); }

	char * hasType() { return "BLDRBASE"; }

	virtual int hasTgtLang() = 0;

	static void getKey(const char *bldrule,char key[]);

#ifdef KBMS
	void Load();
#endif

	void build();
	void free();

	cRuleList & rd_BldRules(const char *key);

};


#endif // BLDRBASE_H

