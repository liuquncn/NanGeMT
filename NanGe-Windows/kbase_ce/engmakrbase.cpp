
#include "engmakrbase.h"

#ifdef KBMS

int EngMakRbase::LoadMakRule(IOToken &TheToken,int tgtlang,TitleList &titlelist)
{
	char *p=TheToken.getOutPntr();
	TheToken.input();
	if( !TheToken.isEqualTo("##") ) 
		error("MakRbase::LoadMakRule(): Should be \"##\"",TheToken.getInPntr() );
	TheToken.output();
	TheToken.output(' ');
	TheToken.input();
	if( !TheToken.isEqualTo("{") ) 
		error("MakRbase::LoadMakRule(): Should be \"{\"",TheToken.getInPntr() );	
	TheToken.output();
	TheToken.input();					// read title
	LoadTitle(TheToken,titlelist);
	if( !TheToken.isEqualTo("}") ) 
		error("MakRbase::LoadMakRule(): Should be \"}\"",TheToken.getInPntr() );
	TheToken.output();
	TheToken.output(' ');
	TheToken.input();
	LoadLMakStruct(TheToken,tgtlang);
	if(! TheToken.isEqualTo("=>") ) 
		error("MakRbase::LoadMakRule(): Should be \"=>\"",TheToken.getInPntr() );	
	TheToken.output(' ');
	TheToken.output(); 
	TheToken.output(' ');
	TheToken.input();
	LoadRMakStruct(TheToken,tgtlang);
	while( TheToken.isEqualTo("=>") )
	{
		TheToken.output(' ');
		TheToken.output();
		TheToken.output(' ');
		TheToken.input();
		LoadRMakStruct(TheToken,tgtlang);
	}
	if( !TheToken.isEqualTo("") ) 
		error("MakRbase::LoadMakRule(): Incorrect end",TheToken.getInPntr() );
	TheToken.output('\n');
	TheToken.output('\n');
	int num=TheToken.getOutPntr()-p;
	TheToken.outputNull();
	return num;
}

void EngMakRbase::LoadLMakStruct(IOToken &TheToken,int tgtlang)
{
	LoadLMakTree(TheToken,tgtlang);
	if( TheToken.isEqualTo("$") || TheToken.getBuffer()[0]=='%' ) 
	{
		TheToken.output(' ');
		LoadLMakBind(TheToken,tgtlang);
	}
}

void EngMakRbase::LoadRMakStruct(IOToken &TheToken,int tgtlang)
{
	LoadRMakTree(TheToken,tgtlang);
	if( TheToken.isEqualTo("$") || TheToken.getBuffer()[0]=='%' )
	{
		TheToken.output(' ');
		LoadRMakBind(TheToken,tgtlang);
	}
}

void EngMakRbase::LoadLMakTree(IOToken &TheToken,int tgtlang)
{
	LoadLeftTree(TheToken, tgtlang );
}

void EngMakRbase::LoadRMakTree(IOToken &TheToken,int tgtlang)
{
	LoadRightTree(TheToken, tgtlang , tgtlang );
}

void EngMakRbase::LoadLMakBind(IOToken &TheToken,int tgtlang)
{
	LoadBind(TheToken, tgtlang );
}

void EngMakRbase::LoadRMakBind(IOToken &TheToken,int tgtlang)
{
	LoadBind(TheToken, tgtlang );
}
#endif // KBMS
