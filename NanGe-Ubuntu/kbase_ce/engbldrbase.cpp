
#include "engbldrbase.h"

#ifdef KBMS

int EngBldRbase::LoadBldRule(IOToken &TheToken,int tgtlang)
{
	char *p=TheToken.getOutPntr();
	TheToken.input();
	if( !TheToken.isEqualTo("@@") )
		error("BldRbase::LoadBldRule(): Should be \"@@\"",TheToken.getInPntr() ); 
	TheToken.output();
	TheToken.output(' ');
	TheToken.input();
	LoadBldLexBaseStruct(TheToken,tgtlang);
	if( !TheToken.isEqualTo(">>") ) 
		error("BldRbase::LoadBldRule(): Should be \">>\"", TheToken.getInPntr());
	TheToken.output(' ');
	TheToken.output();
	TheToken.output(' ');
	TheToken.input(); 
	LoadBldLexVaryStruct(TheToken);
	if( !TheToken.isEqualTo("") )
		error("BldRbase::LoadBldRule(): Incorrect end",TheToken.getInPntr() );
	TheToken.output('\n');
	TheToken.output('\n');
	int num=TheToken.getOutPntr()-p;
	TheToken.outputNull();
	return num;
}

void EngBldRbase::LoadBldLexBaseStruct(IOToken &TheToken,int tgtlang)
{
	LoadBldLexBase(TheToken);
	if( !TheToken.isEqualTo("--") ) 
		error("BldRbase::LoadBldLexBaseStruct(): Should be \"--\"", TheToken.getInPntr());
	TheToken.output(' ');
	TheToken.output();
	TheToken.output(' ');
	TheToken.input();
	LoadBldLexStruct(TheToken,tgtlang);
}

void EngBldRbase::LoadBldLexVaryStruct(IOToken &TheToken) 
{
	if( TheToken.isEqualTo("{") ) 
	{
		TheToken.output(' ');
		TheToken.output();
		TheToken.input();
		if( TheToken.getType()!=IDENT )
			error("BldRbase::LoadBldLexVaryStruct(): Invalid IDENT",TheToken.getInPntr());

		AttLink *link;
		link=mkModel->getSortedAttList().getAttLinkHead();
		while( link && !TheToken.isEqualTo(link->getAttr()->getString()) ) 
			link=link->getNext();
		if( !link ) 
			error("BldRbase::LoadBldLexVaryStruct(): Undefined LexAtt",TheToken.getBuffer() );
		if( link->getAttr()->getLang()!=ENGLISH ) 
			error("BldRbase::LoadBldLexVaryStruct(): Not English Attribute", TheToken );
		if( link->getAttr()->getLexPhr()!=LEXICAL ) 
			error("BldRbase::LoadBldLexVaryStruct(): Not LEXICAL Attribute", TheToken );
		Attribute *attr;
		if( link->getAttr()->getNameAlias()==ALIAS )
			attr=link->getAttr()->getRefer();
		else attr=link->getAttr();
		TheToken.setBuffer( attr->getString() );
		TheToken.output();
		TheToken.input();
		if( !TheToken.isEqualTo("}") ) 
			error("BldRbase::LoadBldLexVaryStruct(): Should be \"}\"", 
				   TheToken.getInPntr() );
		TheToken.output();
		TheToken.output(' ');
		TheToken.input();
	}
	else LoadBldLexVary(TheToken);
	
}

void EngBldRbase::LoadBldLexStruct(IOToken &TheToken,int tgtlang)
{
	LoadLabelItem(TheToken, tgtlang );
	if( TheToken.isEqualTo("{") || TheToken.isEqualTo("[")) LoadFest(TheToken);
}

void EngBldRbase::LoadBldLexBase(IOToken &TheToken)
{
	const char *in=TheToken.getInPntr();
	char *out=TheToken.getOutPntr();
	if( *in=='*' ) *out++=*in++;
	else if( !isLtnChar(in) ) 
		error("BldRbase::LoadBldLexBase(): EngBldRule error",in );
	while( isLtnChar(in) ) setCharNext(out,in);
	TheToken.setInPntr(in); TheToken.input();
	TheToken.setOutPntr(out);
}

void EngBldRbase::LoadBldLexVary(IOToken &TheToken)
{
	const char *in=TheToken.getInPntr();
	char *out=TheToken.getOutPntr();
	if( *in=='*' ) *out++=*in++;
	else if( !isLtnChar(in) ) error("BldRbase::LoadBldLexVary(): EngBldRule error.",in );
	while( isLtnChar(in) ) setCharNext(out,in);
	TheToken.setInPntr(in); TheToken.input();
	TheToken.setOutPntr(out);
}
#endif // KBMS