
#ifndef ENGBLDRBASE_H

#define ENGBLDRBASE_H

#include "../kbase/bldrbase.h"

class EngBldRbase : public BldRbase
{
  protected:

#ifdef KBMS
	static void LoadBldLexBaseStruct(IOToken &TheToken,int tgtlang);
	static void LoadBldLexVaryStruct(IOToken &TheToken);
	static void LoadBldLexBase(IOToken &TheToken);
	static void LoadBldLexVary(IOToken &TheToken);
	static void LoadBldLexStruct(IOToken &TheToken,int tgtlang);
#endif

  public:

	int hasTgtLang() { return ENGLISH; }

#ifdef KBMS
	int LoadBldRule(IOToken &TheToken,int tgtlang); 
#endif

};

#endif // ENGBLDRBASE_H
