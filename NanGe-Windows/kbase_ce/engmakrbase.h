
#ifndef ENGMAKRBASE_H

#define ENGMAKRBASE_H

#include "../kbase/makrbase.h"

class EngMakRbase : public MakRbase
{
#ifdef KBMS
	static void LoadLMakStruct(IOToken &TheToken,int tgtlang);
	static void LoadRMakStruct(IOToken &TheToken,int tgtlang);
	static void LoadLMakTree(IOToken &TheToken,int tgtlang);
	static void LoadRMakTree(IOToken &TheToken,int tgtlang);
	static void LoadLMakBind(IOToken &TheToken,int tgtlang);
	static void LoadRMakBind(IOToken &TheToken,int tgtlang);
#endif

public:

	int hasTgtLang() { return ENGLISH; }

#ifdef KBMS
	int LoadMakRule(IOToken &TheToken,int tgtlang,TitleList &newtitlelist);
#endif
};

#endif // ENGMAKRBASE_H
