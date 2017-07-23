#include "agree.h"

enum { EQUALAGREE, SUPERAGREE, SUFERAGREE, DISAGREE };

void unify_Hierar_Or_Or(OToken &,IToken &,IToken &);
void unify_Hierar_Not_Not(OToken &,IToken &,IToken &);
void unify_Hierar_And_Not(OToken &,IToken &,IToken &);
void unify_Hierar_And_And(OToken &,IToken &,IToken &);
int  agree_Hierar_Smp(Token &,Token &);

void unify_Symbol_Not_Not(OToken &,IToken &,IToken &);
void unify_Symbol_Or_Not(OToken &,IToken &,IToken &);
void unify_Symbol_Or_Or(OToken &,IToken &,IToken &);
int  agree_Symbol_Smp(Token &,Token &);

void unify_Number_Not_Not(OToken &,IToken &,IToken &);
void unify_Number_Or_Not(OToken &,IToken &,IToken &);
void unify_Number_Or_Or(OToken &,IToken &,IToken &);
int  agree_Number_Smp(Token &,Token &);

int agree_Hierar(const char * a1,const char *a2)
{
	char buffer[ATOMLENGTH];
	OToken token0(buffer);
	IToken token1(a1),token2(a2);
	token1.input(); token2.input();
	unify_Hierar_Or_Or(token0,token1,token2);
	return (token0.getOutPntr()!=buffer);
}

void unify_Hierar(OToken & token0, IToken & token1, IToken & token2)
{
	unify_Hierar_Or_Or(token0,token1,token2);
}

void unify_Hierar_Or_Or(OToken & token0, IToken & token1, IToken & token2)
{
	const char * begin=token0.getOutPntr();
	IToken t2;
	while (TRUE)
	{
		t2=token2;
		IToken t1;
		while (TRUE)
		{
			t1=token1;
			OToken t0=token0;
			if (token0.getOutPntr()!=begin) token0.output("|");
			const char * end=token0.getOutPntr();
			if (t2.isDelim("~"))
			{
				if (t1.isDelim("~")) unify_Hierar_Not_Not(token0,t1,t2);
				else unify_Hierar_And_Not(token0,t1,t2);
			}
			else if (t1.isDelim("~")) unify_Hierar_And_Not(token0,t2,t1);
			else unify_Hierar_And_And(token0,t1,t2);
			if (token0.getOutPntr()==end) token0=t0;
			if (!t2.isDelim("|")) break;
			t2.input();
		}
		token1=t1;
		if (!token1.isDelim("|")) break;
		token1.input();
	}
	token2=t2;
}

void unify_Hierar_Not_Not(OToken & token0, IToken & token1, IToken & token2)
{
	while (token1.isDelim("~"))
	{
		token1.input();
		token0.output("~");
		token0.output(token1);
		token1.input();
	}
	while (token2.isDelim("~"))
	{
		token2.input();
		token0.output("~");
		token0.output(token2);
		token2.input();
	}
}

void unify_Hierar_And_Not(OToken & token0, IToken & token1, IToken & token2)
{
	OToken t0=token0;
	IToken t1=token1;
	token0.setBuffer(token1);
	token0.output();
	token1.input();
	while (token1.isDelim("~"))
	{
		token1.input();
		token0.output("~");
		token0.output(token1);
		token1.input();
	}
	while (token2.isDelim("~"))
	{
		token2.input();
		int agree=agree_Hierar_Smp(token2,t1);
		if (agree==SUPERAGREE || agree==EQUALAGREE)
		{
			token2.input();
			while (token2.isDelim("~")) { token2.input(); token2.input(); }
			token0=t0;
			break;
		}
		else if (agree==SUFERAGREE)
		{
			token0.output("~");
			token0.output(token2);
		}
		token2.input();
	}
}

void unify_Hierar_And_And(OToken & token0, IToken & token1, IToken & token2)
{
	int agree=agree_Hierar_Smp(token1,token2);
	if (agree==EQUALAGREE)
	{
		token0.output(token1);
		token1.input();
		token2.input();
		unify_Hierar_Not_Not(token0,token1,token2);
	}
	else if (agree==SUPERAGREE)
	{
		token1.input();
		unify_Hierar_And_Not(token0,token2,token1);
	}
	else if (agree==SUFERAGREE)
	{
		token2.input();
		unify_Hierar_And_Not(token0,token1,token2);
	}
	else
	{
		token1.input();
		while (token1.isDelim("~")) { token1.input(); token1.input(); }
		token2.input();
		while (token2.isDelim("~")) { token2.input(); token2.input(); }
	}
}

int  agree_Hierar_Smp(Token & token1, Token & token2)
{
	const char * s1=token1, * s2=token2;
	while (*s1==*s2 && *s1 && *s2) { s1++; s2++; }
	if (*s1 && *s2) return DISAGREE;
	else if (*s1) return SUFERAGREE;
	else if (*s2) return SUPERAGREE;
	else return EQUALAGREE;
}

int agree_Symbol(const char * a1, const char * a2)
{
	char buffer[ATOMLENGTH];
	OToken token0(buffer);
	IToken token1(a1),token2(a2);
	token1.input(); token2.input();
	if (token1.isDelim("~"))
	{
		if (token2.isDelim("~")) unify_Symbol_Not_Not(token0,token1,token2);
		else unify_Symbol_Or_Not(token0,token2,token1);
	}
	else if (token2.isDelim("~")) unify_Symbol_Or_Not(token0,token1,token2);
	else unify_Symbol_Or_Or(token0,token1,token2);
	return (token0.getOutPntr()!=buffer);
}

void unify_Symbol(OToken & token0, IToken & token1, IToken & token2)
{
	if (token1.isDelim("~"))
	{
		if (token2.isDelim("~")) unify_Symbol_Not_Not(token0,token1,token2);
		else unify_Symbol_Or_Not(token0,token2,token1);
	}
	else if (token2.isDelim("~")) unify_Symbol_Or_Not(token0,token1,token2);
	else unify_Symbol_Or_Or(token0,token1,token2);
}

void unify_Symbol_Not_Not(OToken & token0, IToken & token1, IToken & token2)
{
	while (token1.isDelim("~"))
	{
		token1.input();
		token0.output("~");
		token0.output(token1);
		token1.input();
	}
	while (token2.isDelim("~"))
	{
		token2.input();
		token0.output("~");
		token0.output(token1);
		token2.input();
	}
}

void unify_Symbol_Or_Not(OToken & token0, IToken & token1, IToken & token2)
{
	const char * begin=token0.getOutPntr();
	int agree;
	IToken t2;
	while (TRUE)
	{
		t2=token2;
		agree=1;
		while (t2.isDelim("~"))
		{
			t2.input();
			if (agree_Symbol_Smp(token1,t2)==EQUALAGREE) { agree=0; break; }
			t2.input();
		}
		if (agree)
		{
			if (token0.getOutPntr()!=begin) token0.output("|");
			token0.output(token1);
		}
		token1.input();
		if (!token1.isDelim("|")) break;
		token1.input();
	}
	if (!agree)
		{ t2.input(); while (t2.isDelim("~")) { t2.input(); t2.input(); } };
	token2=t2;
}

void unify_Symbol_Or_Or(OToken & token0, IToken & token1, IToken & token2)
{
	const char * begin=token0.getOutPntr();
	int agree;
	IToken t2;
	while(TRUE)
	{
		t2=token2;
		agree=0;
		while (TRUE)
		{
			if (agree_Symbol_Smp(token1,t2)==EQUALAGREE) { agree=1; break; }
			t2.input();
			if (!t2.isDelim("|")) break;
			t2.input();
		}
		if (agree)
		{
			if (token0.getOutPntr()!=begin) token0.output("|");
			token0.output(token1);
		}
		token1.input();
		if (!token1.isDelim("|")) break;
		token1.input();
	}
	if (agree)
		{ t2.input(); while (t2.isDelim("|")) { t2.input(); t2.input(); } }
	token2=t2;
}

int  agree_Symbol_Smp(Token & token1, Token & token2)
{
	if (strcmp(token1,token2)) return DISAGREE;
	else return EQUALAGREE;
}

int agree_Number(const char * a1, const char * a2)
{
	char buffer[ATOMLENGTH];
	OToken token0(buffer);
	IToken token1(a1),token2(a2);
	token1.input(); token2.input();
	if (token1.isDelim("~"))
	{
		if (token2.isDelim("~")) unify_Number_Not_Not(token0,token1,token2);
		else unify_Number_Or_Not(token0,token2,token1);
	}
	else if (token2.isDelim("~")) unify_Number_Or_Not(token0,token1,token2);
	else unify_Number_Or_Or(token0,token1,token2);
	return (token0.getOutPntr()!=buffer);
}

void unify_Number(OToken & token0, IToken & token1, IToken & token2)
{
	if (token1.isDelim("~"))
	{
		if (token2.isDelim("~")) unify_Number_Not_Not(token0,token1,token2);
		else unify_Number_Or_Not(token0,token2,token1);
	}
	else if (token2.isDelim("~")) unify_Number_Or_Not(token0,token1,token2);
	else unify_Number_Or_Or(token0,token1,token2);
}

void unify_Number_Not_Not(OToken & token0, IToken & token1, IToken & token2)
{
	while (token1.isDelim("~"))
	{
		token1.input();
		token0.output("~");
		token0.output(token1);
		token1.input();
	}
	while (token2.isDelim("~"))
	{
		token2.input();
		token0.output("~");
		token0.output(token1);
		token2.input();
	}
}

void unify_Number_Or_Not(OToken & token0, IToken & token1, IToken & token2)
{
	const char * begin=token0.getOutPntr();
	IToken t2;
	int agree;
	while (TRUE)
	{
		t2=token2;
		agree=1;
		while (t2.isDelim("~"))
		{
			t2.input();
			if (agree_Number_Smp(token1,t2)==EQUALAGREE) { agree=0; break; }
			t2.input();
		}
		if (agree)
		{
			if (token0.getOutPntr()!=begin) token0.output("|");
			token0.output(token1);
		}
		token1.input();
		if (!token1.isDelim("|")) break;
		token1.input();
	}
	if (!agree)
		{ t2.input(); while (t2.isDelim("~")) { t2.input(); t2.input(); } };
	token2=t2;
}

void unify_Number_Or_Or(OToken & token0, IToken & token1, IToken & token2)
{
	const char * begin=token0.getOutPntr();
	int agree;
	IToken t2;
	while(TRUE)
	{
		t2=token2;
		agree=0;
		while (TRUE)
		{
			if (agree_Number_Smp(token1,t2)==EQUALAGREE) { agree=1; break; }
			t2.input();
			if (!t2.isDelim("|")) break;
			t2.input();
		}
		if (agree)
		{
			if (token0.getOutPntr()!=begin) token0.output("|");
			token0.output(token1);
		}
		token1.input();
		if (!token1.isDelim("|")) break;
		token1.input();
	}
	if (agree)
		{ t2.input(); while (t2.isDelim("|")) { t2.input(); t2.input(); } }
	token2=t2;
}

int  agree_Number_Smp(Token & token1, Token & token2)
{
	if (atoi(token1)==atoi(token2)) return EQUALAGREE;
	else return DISAGREE;
}

int agree_Boolean(const char * a1, const char * a2)
{
	int b1,b2;
	IToken token1(a1),token2(a2);
	token1.input(); token2.input();
	if (isTrue(token1)) b1=1; else b1=0;
	if (isTrue(token2)) b2=1; else b2=0;
	return (b1==b2);
}

void unify_Boolean(OToken & token0, IToken & token1, IToken & token2)
{
	int b1,b2;
	if (isTrue(token1)) b1=1; else b1=0;
	if (isTrue(token2)) b2=1; else b2=0;
	if (b1==b2) token0.output(token1);
	token1.input();
	token2.input();
}

