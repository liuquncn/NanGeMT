
#ifndef CHNDICTN_H

#define CHNDICTN_H

#include "../kbase/dictn.h"

//data structure of Chinese Dictionary
class ChnDictn : public Dictn
{

  protected:

	int getDstgVal(const char *word);
	int getHashVal(const char *word,int Len);

  public:

	int hasSrcLang(){ return CHINESE;}

	cRuleList & rd_MeansWord(char *word,const char *text=NULL);
};

#endif // CHIDICTN_H
