
#ifndef AGREE_H

#define AGREE_H

#include "token.h"

const static int ATOMLENGTH=128;

enum { HIERAR,SYMBOL,NUMBER,MTBOOL };

inline int isTrue(const char *s)
	{ return !strcmp(s,"YES")||!strcmp(s,"Yes")||!strcmp(s,"ÊÇ"); }

inline int isFalse(const char *s)
	{ return !strcmp(s,"NO")||!strcmp(s,"No")||!strcmp(s,"·ñ"); }

int  agree_Hierar(const char * a1,const char * a2);
void unify_Hierar(OToken & token0, IToken & token1, IToken & token2);

int  agree_Symbol(const char * a1,const char * a2);
void unify_Symbol(OToken & token0, IToken & token1, IToken & token2);

int  agree_Number(const char * a1,const char * a2);
void unify_Number(OToken & token0, IToken & token1, IToken & token2);

int  agree_Boolean(const char * a1,const char * a2);
void unify_Boolean(OToken & token0, IToken & token1, IToken & token2);

#endif // AGREE_H
