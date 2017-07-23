#ifndef CHNCUTRBASE_H

#define CHNCUTRBASE_H

#include "../kbase/cutrbase.h"

class ChnOvlRbase : public CutRbase
{

public:

	ChnOvlRbase() { }
	~ChnOvlRbase() { free(); }

	char * hasType() { return "OVLRBASE"; }
	int hasSrcLang() { return CHINESE; }

	cRuleList & rd_OvlRules(const char *key) { return rd_CutRules(key); }
#ifdef KBMS
	BOOL rdTxtCutRule() { return rdTxtOvlRule(); }
	int LoadCutRule(IOToken &TheToken,int srclang) { return LoadOvlRule(TheToken,srclang); }

	BOOL rdTxtOvlRule();
	int LoadOvlRule(IOToken &TheToken,int srclang);

protected:

	static void LoadOvlLexBaseStruct(IOToken &TheToken,char *cutrule,int srclang);
	static void LoadOvlLexVaryStruct(IOToken &TheToken,char *cutrule,int srclang);
	static void LoadOvlLexStruct(IOToken &TheToken,char *cutrule,int srclang);
	static void LoadOvlLexVary(IOToken &TheToken,char *cutrule);
	static void LoadOvlLexBase(IOToken &TheToken,char *cutrule);
#endif
};


class ChnSegRbase: public CutRbase
{
public:

	ChnSegRbase() { }
	~ChnSegRbase() { free(); }

	char * hasType() { return "SEGRBASE"; }
	int hasSrcLang() { return CHINESE; }

	cRuleList & rd_SegRules(const char *key) { return rd_CutRules(key); }
#ifdef KBMS
	BOOL rdTxtCutRule() { return rdTxtSegRule(); }
	int LoadCutRule(IOToken &TheToken,int srclang) { return LoadSegRule(TheToken); }

	BOOL rdTxtSegRule();
	int LoadSegRule(IOToken &TheToken);

protected:
	static void LoadSegStruct(IOToken &TheToken,char *cutrule);
	static void LoadSegBody(IOToken &TheToken,char *cutrule);
	static void LoadSegEnd(IOToken &TheToken,char *cutrule);
   	static void LoadSegBranch(IOToken &TheToken,char *cutrule);
	static void LoadSegCondition(IOToken &TheToken,char *cutrule);
	static void LoadSegPattern(IOToken &TheToken,char *cutrule);
	static void LoadSegSelection(IOToken &TheToken,char *cutrule);
	static char * LoadSegSelect(IOToken &TheToken,char *cutrule);
	static void LoadSegResult(IOToken &TheToken,char *cutrule);
	static void LoadSegEquation(IOToken &TheToken,char *cutrule);
	static void LoadSegVar(IOToken &TheToken,char *cutrule);
	static void LoadSegString(IOToken &TheToken,char *cutrule);

	static void LoadCtxFunction(IOToken &TheToken,char *cutrule);
	static void LoadCtxFunc(IOToken &TheToken,char *cutrule);
	static void LoadCtxCondition(IOToken &TheToken,char *cutrule);
	static void LoadCtxEquation(IOToken &TheToken,char *cutrule);
	static void LoadCtxVar(IOToken &TheToken,char *cutrule);
	static void LoadCtxAttr(IOToken &TheToken,Attribute *&attr,char *cutrule);
	static void LoadCtxPattern(IOToken &TheToken,char *cutrule);
	static void LoadCtxString(IOToken &TheToken,char *cutrule);

	static void LoadDirection(IOToken &TheToken,char *cutrule);
	static void LoadDistance(IOToken &TheToken,char *cutrule);
#endif
};


class ChnTagRbase: public CutRbase
{
public:

	ChnTagRbase() { }
	~ChnTagRbase() { free(); }

	char * hasType() { return "TAGRBASE"; }
	int hasSrcLang() { return CHINESE; }

	cRuleList & rd_TagRules(const char *key) { return rd_CutRules(key); }
#ifdef KBMS
	BOOL rdTxtCutRule() { return rdTxtTagRule(); }
	int LoadCutRule(IOToken &TheToken,int srclang) { return LoadTagRule(TheToken); }

	BOOL rdTxtTagRule();
	int LoadTagRule(IOToken &TheToken);

protected:
	static void LoadTagStruct(IOToken &TheToken,char *cutrule);
	static void LoadTagBody(IOToken &TheToken,char *cutrule);
	static void LoadTagEnd(IOToken &TheToken,char *cutrule);
    static void LoadTagBranch(IOToken &TheToken,char *cutrule);
	static void LoadTagCondition(IOToken &TheToken,char *cutrule);
	static void LoadTagResult(IOToken &TheToken,char *cutrule);
    static void LoadTagEquation(IOToken &TheToken,char *cutrule);
	static void LoadTagVar(IOToken &TheToken,char *cutrule);
	static void LoadTagWord(IOToken &TheToken,char *cutrule);
	static void LoadTagLabel(IOToken &TheToken,char *cutrule);

	static void LoadCtxAttr(IOToken &TheToken,Attribute *&attr,char *cutrule);
	static void LoadCtxFunction(IOToken &TheToken,char *cutrule);
	static void LoadCtxFunc(IOToken &TheToken,char *cutrule);
	static void LoadCtxCondition(IOToken &TheToken,char *cutrule);
	static void LoadCtxEquation(IOToken &TheToken,char *cutrule);
	static void LoadCtxVar(IOToken &TheToken,char *cutrule);
	static void LoadCtxString(IOToken &TheToken,char *cutrule);
	static void LoadCtxPattern(IOToken &TheToken,char *cutrule);

	static void LoadDirection(IOToken &TheToken,char *cutrule);
	static void LoadDistance(IOToken &TheToken,char *cutrule);
#endif
};
#endif // CHNCUTRBASE_H
