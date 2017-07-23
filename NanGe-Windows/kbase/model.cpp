//#include <unistd.h>
#include "model.h"

/********************************************************************
*   Date:                 1994.6.7                                  *
*   Module Description:     	                                    *
*			This file contains member functions of Class Model      *
*			Every knowledge base has a Model .						*
*********************************************************************/

#ifdef KBMS

void Model::Load()
{
	wopen();
	if( getStatus()!=WOPEN) 
		error("Model::Load(): cannot load while Model not open");

	beginScan(MAXBUFFER);

	char txtfilename[FILENAMELENGTH];
	getFullName(txtfilename,".TXT");
	TxtFile.open(txtfilename,ios::in|ios::binary);
	if( !TxtFile ) error("Model::Load(): TXTOPEN",txtfilename);

	CErrorException * err=NULL;
	try
	{
		int i;
		IOToken TheToken;
		rdTxtSemModel(); 	
		TheToken.setInPntr(TxtBuff);
		TheToken.setOutPntr(DatBuff);
		i=LoadSemModel(TheToken);  	
		wtDatLength(i);
		wtDatBuff();		

		BOOL flag;
		flag=TRUE;
		while(flag=rdTxtSynModel()){
			TheToken.setInPntr(TxtBuff);
			TheToken.setOutPntr(DatBuff);
			int i;
			i=LoadSynModel(TheToken);
			wtDatLength(i);
			wtDatBuff();
		}
	}
	catch (CErrorException * e) { err=e; }

	TxtFile.close();

	endScan();
	close();

	SortedAttList.makeindex();

	setBuilt(YES);
	if (err) throw err;
}
#endif

void Model::build()
{
	if (getBuilt()==YES ) return;
	ropen();
	beginScan(MAXBUFFER);

	try
	{
		int leng;
		leng=rdDatLength();
		if (leng<=0 || leng>=MAXBUFFER)
			error("Model::build(): Model is invalid. Please recreate it.");
		rdDatBuff(leng);
		IToken TheToken(DatBuff);
		ReadSemModel(TheToken);

		while(leng=rdDatLength()){
			TheToken.setInPntr(DatBuff);
			rdDatBuff(leng);
			ReadSynModel(TheToken);
		}
	}
	catch (CErrorException *) { endScan(); close(); throw; }

	endScan();
	close();

	SortedAttList.makeindex();

	setBuilt(YES);
	Kbase::build();
}

void Model::free()
{
	if (getBuilt()==NO) return;
	SortedAttList.free();
	asemModel.free();
	int lang;
	for (lang=0;lang<LANGNUM;lang++)
		asynModel[lang].free();
	Kbase::free();
}
	
// open the Model for reading and set the date, and read the definition of symbols
void Model::ropen()
{
	Kbase::ropen();

	char datafile[FILENAMELENGTH];
	getFullName(datafile,".DAT");
	setDate(FileGetTime(datafile));
/*
	beginScan(MAXBUFFER);

	int leng;
	leng=rdDatLength();
	rdDatBuff(leng);
	IToken TheToken(DatBuff);
	ReadSemModel(TheToken);

	while(leng=rdDatLength()){
		rdDatBuff(leng);
		TheToken.setInPntr(DatBuff);
		ReadSynModel(TheToken);
	}

	endScan();
*/
}

#ifdef KBMS
// open the Model for writing and should be used before Load() and edit()
void Model::wopen()
{
	Kbase::wopen();
}
#endif

void Model::close()
{
	Kbase::close();
}

/********************************************************************
*   Date:                 1994.5.20                                 *
*   Function Description:                                           *
*                         Check if the model contains the specified *
*                         languge.                                  *
*********************************************************************/

// open the data file. check the file's language name and if yes,return TRUE
BOOL Model ::hasLang(int LangCode)
{
	if (LangCode<0||LangCode>=LANGNUM) return FALSE;
	else if(asynModel[LangCode].getHas()==YES)return TRUE;
	else return FALSE;
}

#ifdef KBMS
// read SemModel from TxtFile to TxtBuff and filter extra blanks, tabs,carriage returns,and notes.
void Model::rdTxtSemModel()
{
	rdTxtRecord("SemModel","EndModel");
}

/********************************************************************
*   Function Description:                                           *
*				 read SynModel from TxtFile to TxtBuff				*
*				 and set InPntr=TxtBuff and OutPntr=DatBuff         *
*********************************************************************/

BOOL   Model::rdTxtSynModel()
	// read SynModel from TxtFile to TxtBuff
	// and set InPntr=TxtBuff and OutPntr=DatBuff
{
	return rdTxtRecord("SynModel","EndModel");
}
 
#endif

/********************************************************************
*Note:at the end of every load process, keep reading one			*
*	  token ahead.So at the beginning of every load process,		*
*	  don't Need to read a token to see what action should take,	*
*	  also don't Need to withdraw a token at the end of the load	*
*	  process to avoid reading an extra token not 					*
*	  belonging to current object.									*
*********************************************************************/

Attribute * Model::getAttribute(const char string[])
{
	Attribute *attr=SortedAttList.find(string);
	return attr;
}

Attribute * Model::getLexCat(int i)
{
	Attribute *attr;
	attr=asynModel[i].getLexCat().getAttLinkHead()->getAttr(); 
	return attr;
}

Attribute * Model::getPhrCat(int i)
{
	Attribute *attr;
	attr=asynModel[i].getPhrCat().getAttLinkHead()->getAttr();
	return attr;
}

Attribute * Model::getBaseForm(int i)
{
	Attribute *attr;
	attr=asynModel[i].getBaseForm().getAttLinkHead()->getAttr();
	return attr;
}

Attribute * Model::getVaryForm(int i)
{
	Attribute *attr;
	attr=asynModel[i].getVaryForm().getAttLinkHead()->getAttr();
	return attr;
}

Attribute * Model::getSemCat()
{
	Attribute *attr;
	attr=asemModel.getSemCat().getAttLinkHead()->getAttr();
	return attr;
}

#ifdef KBMS
int Model::LoadSemModel(IOToken &TheToken)
{
 	char *TempPntr;
 	TempPntr=TheToken.getOutPntr();    

	TheToken.input();
	if(!TheToken.isEqualTo("SemModel"))
		error("Model::LoadSemModel(): Should be \"SemModel\"",TheToken);	
	TheToken.output();
	TheToken.output('\n');

	TheToken.input();
	LoadSemModelBody(TheToken);

	TheToken.output('\n');
	if(!TheToken.isEqualTo("EndModel")) 
		error("Model::LoadSemModel(): Should be \"EndModel\"",TheToken );
	TheToken.output();

	TheToken.output('\n');
	TheToken.output('\n');

	int num=TheToken.getOutPntr()-TempPntr;
	TheToken.outputNull();
	return num;
}

void Model::LoadSemModelBody(IOToken &TheToken)
{
	LoadSemCat(TheToken);
	LoadSemFeat(TheToken);
	LoadSemAss(TheToken);
}

void Model::LoadSemCat(IOToken &TheToken)
{
	if(!TheToken.isEqualTo("SemCat")) 
		error("Model::LoadSemCat(): Should be \"SemCat\"",TheToken );
	TheToken.output('\t');
	TheToken.output();
	TheToken.output(' ');
	TheToken.input();
	if(!TheToken.isEqualTo(":")) 
		error("Model::LoadSemCat(): Should be \":\"",TheToken );
	TheToken.output();
	TheToken.output(' ');
	TheToken.input();
	LoadAttribute(TheToken,SortedAttList,asemModel.getSemCat(),NO,SEMANTIC,LEXICAL,INTERNAL,STATIC,MTRELATIVE,HIERAR,LIMITED);
	if(!TheToken.isEqualTo(":")) 
		error("Model::LoadSemCat(): Should be \":\"",TheToken);
	TheToken.output(' ');
	TheToken.output();
	TheToken.output('\n');
	TheToken.input();
	if(!TheToken.isEqualTo("{")) 
		error("Model::LoadSemCat(): Should be \"{\"",TheToken);

	TheToken.output('\t');
	TheToken.output();						// output "{"
	TheToken.output('\n');
 	TheToken.input();
	TheToken.output('\t');
	TheToken.output('\t');
	LoadHieSym_Set(TheToken,asemModel.getSemCat().getAttLinkHead()->getAttr()->getValList());
	//after load , ValList of SemCat is build

	if(!TheToken.isEqualTo("}")) 
		error("Model::LoadSemCat()", "Wrong: Should be \"}\"",TheToken);

	TheToken.output('\n');
	TheToken.output('\t');
	TheToken.output();						// output "}"
	TheToken.output('\n');
	TheToken.input();
}

void Model::LoadHieSym_Set(IOToken &TheToken,class cValList &vallist)
{
	LoadHieSym_Atom(TheToken,vallist);
	while(TheToken.isEqualTo(","))
	{
		 TheToken.output();
		 TheToken.output('\n');
		 TheToken.output('\t');
		 TheToken.output('\t');
		 TheToken.input();
		 LoadHieSym_Atom(TheToken,vallist);
	}
}

void Model::LoadHieSym_Atom(IOToken &TheToken,class cValList &vallist)
{
	Value *val=new Value;
	LoadAtom_Name(TheToken,val);
	vallist.addto(val);
	int firstalias=1;
	while(TheToken.isEqualTo("=")){
		TheToken.output();
		TheToken.input();
		Value *valalias=new Value;
		LoadAtom_Alias(TheToken,valalias);

		Value *p;
		p=vallist.find(valalias->getString());
		if(p) 
			error("Model::LoadHieSym_Atom(): the same Alias",TheToken);

		vallist.addto(valalias);
		if(firstalias==1) val->setRefer(valalias);
		else firstalias=0;
		valalias->setRefer(val);
	}
}

void Model::LoadAtom_Name(IOToken &TheToken,Value *val)
{
	if(TheToken.getType()!=IDENT) 
		error("Model::LoadAtom_Name(): Invalid Atom_Name",TheToken);
	val->setNameAlias(NAME);
	val->setString(TheToken.getBuffer());
	TheToken.output();
	TheToken.input();
}

void Model::LoadAtom_Alias(IOToken &TheToken,Value *val)
{
	if(TheToken.getType()!=IDENT) 
		error("Model::LoadAtom_Alias(): Invalid Atom_Alias",TheToken);
	val->setNameAlias(ALIAS);
	val->setString(TheToken.getBuffer());
	TheToken.output();
	TheToken.input();
}

void Model::LoadSemFeat(IOToken &TheToken)
{
	while(TheToken.isEqualTo("SemAtt") || TheToken.isEqualTo("SemVal"))
	{
		const char * errorpos=TheToken.getInPntr();
		try
		{
			TheToken.output('\t');
			if(TheToken.isEqualTo("SemAtt")){
				TheToken.output();
				TheToken.input();
				LoadSemAtt(TheToken);
			}
			else if(TheToken.isEqualTo("SemVal")) {
				TheToken.output();
				TheToken.input();
				LoadSemVal(TheToken);
			}
		}
		catch (CErrorException *)
		{
			message("Position:",errorpos);
			throw;
		}
	}
}
 
void Model::LoadSemAtt(IOToken &TheToken)
{
	TheToken.output(' ');
	char *cond;
	if( TheToken.isEqualTo("[")) 
		cond=LoadCondition(TheToken,SortedAttList);
	else { cond=new char[3]; strcpy(cond,"[]"); }
	CondLink *link=asemModel.getSemAttCondList().addto( cond );
	// after loadcondition , condition is added to the tail of conditionlist

	if(!TheToken.isEqualTo(":")) 
		error("Model::LoadSemAtt(): Need \":\"",TheToken);

	TheToken.output(' ');
	TheToken.output();
	TheToken.output('\n');
	TheToken.input();
	if(!TheToken.isEqualTo("{")) 
		error("Model::LoadSemAtt(): Need \"{\"",TheToken);

	TheToken.output('\t');
	TheToken.output();
	TheToken.output('\n');
	TheToken.input();
	TheToken.output('\t');
	TheToken.output('\t');
	LoadAtt_Set(TheToken,SortedAttList,link->getCondAttList(),SEMANTIC,LEXICAL,INTERNAL,STATIC,MTRELATIVE);

	if(!TheToken.isEqualTo("}"))  
		error("Model::LoadSemAtt(): Need \"}\"",TheToken);

	TheToken.output('\n');
	TheToken.output('\t');
	TheToken.output();
	TheToken.output('\n');
	TheToken.input();
}

char *Model::LoadCondition(IOToken &TheToken,class SortedAttList & list)
{
	const char *p;
	p=TheToken.getInPntr();
	LoadFest(TheToken,list);
	const char *q;
	q=TheToken.getInPntr();
	int size=q-p;
	char *TempBuff;
	TempBuff=new char[size+1];
	strncpy(TempBuff,p,size);
	TempBuff[size]='\0';
	return (TempBuff);
}

void Model::LoadAtt_Set(IOToken &TheToken,class SortedAttList &attlist,class UnsortAttList &unattlist,int lang,unsigned lexphr,unsigned intext,unsigned stadyn,unsigned relative,unsigned atttype,unsigned limit)
{
	LoadAttribute(TheToken,attlist,unattlist,YES,lang,lexphr,intext,stadyn,relative,atttype,limit);
	while(TheToken.isEqualTo(",")){
		TheToken.output();
		TheToken.output('\n');
		TheToken.output('\t');
		TheToken.output('\t');
		TheToken.input();
		LoadAttribute(TheToken,attlist,unattlist,YES,lang,lexphr,intext,stadyn,relative,atttype,limit);
	}
}

void Model::LoadAttribute(IOToken &TheToken,class SortedAttList &attlist,class UnsortAttList &unattlist,unsigned duppermit,int lang,unsigned lexphr,unsigned intext,unsigned stadyn,unsigned relative,unsigned atttype,unsigned limit)
{
	Attribute *p;
	Attribute *attr;
	unsigned exist;
	//not find the name in SortedAttList and add to the SortedAttList
	p=attlist.find(TheToken.getBuffer());
	exist=p?YES:NO;

	if(exist==NO){
		attr=new Attribute;
		attr=LoadAtt_Name(TheToken,attr);
		attr->setLang(lang);
		attr->setIntExt(intext);
		attr->setStaDyn(stadyn);
		attr->setLexPhr(lexphr);
		attr->setRelative(relative);
		attr->setAttType(atttype);
		attr->setLimit(limit);
		attlist.addto(attr);
		unattlist.addto(attr);
		int firstalias=1;
		while(TheToken.isEqualTo("=")){
			TheToken.output();
			TheToken.input();

			Attribute *pp;
			pp=attlist.find(TheToken.getBuffer());
			if(pp)
				error("Model::LoadAttribute(): Ambiguous Alias",TheToken);

			Attribute *aliasattr;
			aliasattr=new Attribute;
			aliasattr=LoadAtt_Alias(TheToken,aliasattr);
			aliasattr->setLang(lang);
			aliasattr->setIntExt(intext);
			aliasattr->setStaDyn(stadyn);
			aliasattr->setLexPhr(lexphr);
			aliasattr->setRelative(relative);
			aliasattr->setAttType(atttype);
			aliasattr->setLimit(limit);
			attlist.addto(aliasattr);
			unattlist.addto(aliasattr);
			aliasattr->setRefer(attr);
			if (firstalias==1) {
				attr->setRefer(aliasattr);
				firstalias=0;
			}	
		}
	}
	else { /* exist==YES find the name in SortedAttList */ 
		if (duppermit==NO)
			error("Model::LoadAttribute(): Unpermitted duplicat Attribute Name",TheToken);

		if ( p->getLang()!=lang || p->getIntExt()!=intext || 
			 p->getLimit()!=limit || p->getNameAlias()!=NAME ||
			 p->getLexPhr()!=lexphr || p->getStaDyn()!=stadyn )
			error("Model::LoadAttribute(): Redefined attribute",TheToken);

		attr=p;
		unattlist.addto(attr);

		if (TheToken.isEqualTo("="))
			error("Model::LoadAttribute(): Redefined Alias of Duplicat Name",TheToken);
		
		TheToken.output();
		TheToken.input();
	}
}


Attribute *Model::LoadAtt_Name(IOToken &TheToken,Attribute *attr)
{
	if(TheToken.getType()!=IDENT) 
		error("Model::LoadAtt_Name(): Not Ident",TheToken,TheToken.getInPntr());
	attr->setNameAlias(NAME);
	attr->setString(TheToken.getBuffer());
	TheToken.output();
	TheToken.input();
	return attr;
}

Attribute *Model::LoadAtt_Alias(IOToken &TheToken,Attribute * attr)
{
	if(TheToken.getType()!=IDENT) 
		error("Model::LoadAtt_Alias(): Not Ident",TheToken,TheToken.getInPntr());
	attr->setNameAlias(ALIAS);
	attr->setString(TheToken.getBuffer());
	TheToken.output();
	TheToken.input();
	return attr;
}

void Model::LoadSemVal(IOToken &TheToken)
{
	if(!TheToken.isEqualTo("(")) 
		error("Model::LoadSemVal(): Need \"(\"",TheToken,TheToken.getInPntr());

	TheToken.output(' ');
	TheToken.output();
	TheToken.input();

	if(TheToken.getType()!=IDENT) 
		error("Model::LoadSemVal(): Not Ident",TheToken);

	Attribute *p;
	p=SortedAttList.find(TheToken.getBuffer());
	if(!p) 
		error("Model::LoadSemVal(): Undefined Attribute",TheToken);

	Attribute *q;
	q=asemModel.getSemCat().find(TheToken.getBuffer());
	if( q ) 
		error("Model::LoadSemVal(): Should be SemAtt Attribute,not SemCat Attribute",TheToken);

	if( p->getLang()==SEMANTIC) {
		if( p->getIntExt()!=INTERNAL ) 
		 error("Model::LoadSemVal(): Should be SemAtt Attribute,not SemAssAtt Attribute",TheToken);
	}
	else error("Model::LoadSemVal(): Should be SemAtt Attribute ,not LexPhr Attriubte.",TheToken);
		
	// if namealias is alias ,you Need convert it to name
	Attribute *pp;
    if(p->getNameAlias()==ALIAS) pp=p->getRefer();
	else pp=p;

	TheToken.output();
	TheToken.input();
	if(!TheToken.isEqualTo(")")) 
		error("Model::LoadSemVal(): Need \")\"",TheToken,TheToken.getInPntr());
	TheToken.output();
	TheToken.input();
	if(!TheToken.isEqualTo(":")) 
		error("Model::LoadSemVal(): Need \":\"",TheToken,TheToken.getInPntr());

	TheToken.output(' ');
	TheToken.output();
	TheToken.output(' ');
	TheToken.input();

	LoadAtomVal(TheToken,pp);
}

void Model::LoadAtomVal(IOToken &TheToken,Attribute *attr)
{
	LoadAtomValType(TheToken,attr);
	if( TheToken.isEqualTo("Static") || TheToken.isEqualTo("Dynamic") || 
		TheToken.isEqualTo("RelMode") ) LoadValMode(TheToken,attr);
}

void Model::LoadAtomValType(IOToken &TheToken,Attribute *attr)
{
	unsigned type;
	type=LoadAtomType(TheToken,attr);
	if(TheToken.isEqualTo("Default")) LoadDefAtom(TheToken,type,attr); 
}

void Model::LoadValMode(IOToken &TheToken,Attribute *attr)
{
	if( TheToken.isEqualTo("RelMode") ) LoadRelMode(TheToken,attr);
	else {
		LoadActMode(TheToken,attr);
		if( TheToken.isEqualTo("RelMode") ) LoadRelMode(TheToken,attr);
	}
}

void Model::LoadActMode(IOToken &TheToken,Attribute *attr)
{
	if( !TheToken.isEqualTo("Static") && !TheToken.isEqualTo("Dynamic") )
		error("Model::LoadActMode(): Invalid ActMode,Need \"Static\" or \"Dynamic\"",TheToken,TheToken.getInPntr());
	if( TheToken.isEqualTo("Static") ) attr->setStaDyn(STATIC);
	else attr->setStaDyn(DYNAMIC);
	TheToken.output(' ');
	TheToken.output();
	TheToken.input();
}

void Model::LoadRelMode(IOToken &TheToken,Attribute *attr)
{
	 if( !TheToken.isEqualTo("RelMode") ) 
		error("Mode::LoadRelMode(): Need \"RelMode\"",TheToken,TheToken.getInPntr()); 
	TheToken.output(' '); 
	TheToken.output(); 
	TheToken.input(); 
	if(!TheToken.isEqualTo("=")) 
		error("Model::LoadRelMode(): Need \"=\"",TheToken);
	TheToken.output();

	TheToken.input();
	if(!TheToken.isEqualTo("0") && !TheToken.isEqualTo("1")) 
		error("Model::LoadRelMode(): Invalid Relative",TheToken);

	if(TheToken.isEqualTo("0")) attr->setRelative(IRRELATIVE);
	else attr->setRelative(MTRELATIVE);

	TheToken.output();
	TheToken.input();
}

unsigned Model::LoadAtomType(IOToken &TheToken,Attribute * attr)
{
	unsigned Type;
	if(TheToken.isEqualTo("Hierar")) Type=HIERAR;
	else if(TheToken.isEqualTo("Symbol")) Type=SYMBOL;
	else if(TheToken.isEqualTo("Number")) Type=NUMBER;
	else if(TheToken.isEqualTo("Boolean")) Type=MTBOOL;
	else error("Model::LoadAtomType(): Invalid AttType",TheToken);

	attr->setAttType(Type);

	TheToken.output();
	TheToken.input();

	if( Type==HIERAR || Type==SYMBOL || Type==NUMBER ) 
		if( !TheToken.isEqualTo("Limited") && !TheToken.isEqualTo("Unlimited") )
			error("Model::LoadAtomType(): Need \"Limited\" or \"Unlimited\"",TheToken,TheToken.getInPntr());

	if(TheToken.isEqualTo("Limited") || TheToken.isEqualTo("Unlimited")) {
		unsigned limit;
		if(TheToken.isEqualTo("Limited")) limit=LIMITED;
		else limit=UNLIMITED;
		attr->setLimit(limit);

		TheToken.output(' ');
		TheToken.output();
		TheToken.input();
	}
	if(TheToken.isEqualTo("{")) {
		TheToken.output('\n');
		TheToken.output('\t');
		TheToken.output();
		TheToken.output('\n');
		TheToken.output('\t');
		TheToken.output('\t');
		if(Type==HIERAR || Type==SYMBOL){
			TheToken.input();
			LoadHieSym_Set(TheToken,attr->getValList());
		}
		else if(Type==NUMBER) {
			TheToken.input();
			LoadNumber_Set(TheToken,attr->getValList());
		}
	 	else if(Type==MTBOOL) {
			TheToken.input();
			LoadBoolean_Set(TheToken,attr->getValList());
		}
		if(!TheToken.isEqualTo("}")) 
			error("Model::LoadAtomType(): Need \"}\"",TheToken,TheToken.getInPntr());

		TheToken.output('\n');
		TheToken.output('\t');
		TheToken.output();
		TheToken.input();
	}
	return Type;
}

void Model::LoadNumber_Set(IOToken &TheToken,class cValList &vallist)
{
	LoadNumber_Atom(TheToken,vallist);
	while(TheToken.isEqualTo(","))
	{
		TheToken.output();
		TheToken.output('\n');
		TheToken.output('\t');
		TheToken.output('\t');
		TheToken.input();
		LoadNumber_Atom(TheToken,vallist);
	}
}

void Model::LoadNumber_Atom(IOToken &TheToken,class cValList &vallist)
{
	if(TheToken.getType()!=INTEG) 
		error("Model::LoadNumber_Atom(): Should be Integ",TheToken,TheToken.getInPntr());

	Value *val=new Value;
	val->setNameAlias(NAME);
	val->setString(TheToken.getBuffer());
	vallist.addto(val);

	TheToken.output();
	TheToken.input();

	int firstalias=1;
	while(TheToken.isEqualTo("="))
	{
		TheToken.output();
		TheToken.input();
		Value *valalias=new Value;
		LoadAtom_Alias(TheToken,valalias);
		Value *p;
		p=vallist.find(valalias->getString());

		if(p) error("Model::LoadNumber_Atom(): the same Alias",TheToken);
	 	vallist.addto(valalias);

		if(firstalias==1) val->setRefer(valalias);
		else firstalias=0;
		valalias->setRefer(val);
	}	
}

void Model::LoadBoolean_Set(IOToken &TheToken,class cValList &vallist)
{
	LoadBoolean_Atom(TheToken,vallist);
	while(TheToken.isEqualTo(","))
	{
		TheToken.output();
		TheToken.output('\n');
		TheToken.output('\t');
		TheToken.output('\t');
		TheToken.input();
		LoadBoolean_Atom(TheToken,vallist);
	}
}

void Model::LoadBoolean_Atom(IOToken &TheToken,class cValList &vallist)
{
	if(TheToken.isEqualTo("ÊÇ") || TheToken.isEqualTo("·ñ")||
	   TheToken.isEqualTo("YES")|| TheToken.isEqualTo("NO"))
	{
		Value *val=new Value;
		val->setNameAlias(NAME);
		val->setString(TheToken.getBuffer());
		vallist.addto(val);
		TheToken.output();
		TheToken.input();
		int firstalias=1;
		while(TheToken.isEqualTo("=")){
			TheToken.output();
			TheToken.input();
			Value *valalias= new Value;
			LoadAtom_Alias(TheToken,valalias);

			Value *p;
			p=vallist.find(valalias->getString());
			if(p) error("Model::LoadBoolean_Atom(): the same Alias",TheToken);
			vallist.addto(valalias);

			if (firstalias==1) val->setRefer(valalias);
			else firstalias=0;
			valalias->setRefer(val);
		}
	}
}

void Model::LoadDefAtom(IOToken &TheToken,unsigned type,Attribute *attr)
{
	TheToken.output(' ');
	TheToken.output();			// output "Default"
	TheToken.input();
	if(!TheToken.isEqualTo("=")) 
		error("Model::LoadDefAtom(): Need \"=\"",TheToken);
	TheToken.output();
	TheToken.input();
	const char *p;
	const char *q;
	p=TheToken.getInPntr();
	LoadAtom(TheToken,type,attr->getLimit(),attr->getValList());
	q=TheToken.getInPntr();
	if (*(q-1)=='\n') q--;
	int size;
	size=q-p-1;
	char * TempBuff=new char [size+1];	
	strncpy(TempBuff,p,size);
	TempBuff[size]='\0';
	if (attr->getDefault()) delete attr->getDefault();
	attr->setDefault(TempBuff);	
}

void Model::LoadDefFest(IOToken &TheToken,Attribute *attr)
{
	TheToken.output(' ');
	TheToken.output();
	TheToken.input();
	if( !TheToken.isEqualTo("=") )
		error("Model::LoadDefFest(): Need \"=\"",TheToken );
	TheToken.output();
	TheToken.input();
	const char *p,*q;
	p=TheToken.getInPntr();
	LoadFest(TheToken,SortedAttList);
	q=TheToken.getInPntr();
	int len=q-p;
	char *TempBuff=new char[len+1];
	strncpy(TempBuff,p,len);
	TempBuff[len]='\0';
	if (attr->getDefault()) delete attr->getDefault();
	attr->setDefault(TempBuff);
}

void Model::LoadSemAss(IOToken &TheToken)
{
	if( !TheToken.isEqualTo("SemAssAtt") && !TheToken.isEqualTo("SemAssVal") )
		error("Model::LoadSemAss(): Should be \"SemAssAtt\" or \"SemAssVal\"",TheToken );
	while( TheToken.isEqualTo("SemAssAtt") || TheToken.isEqualTo("SemAssVal") )
	{
		TheToken.output("\n\t");
		if( TheToken.isEqualTo("SemAssAtt") ) 
		{
			TheToken.output();
			TheToken.input();
			LoadSemAssAtt(TheToken);
		}
		else 
		{
			TheToken.output();
			TheToken.input();
			LoadSemAssVal(TheToken);
		}
	}
}

void Model::LoadSemAssAtt(IOToken &TheToken) 
{ 
	char *cond;
	if( TheToken.isEqualTo("[")) 
		cond=LoadCondition(TheToken,SortedAttList);
	else { cond=new char[3]; strcpy(cond,"[]"); }

	CondLink *link=asemModel.getSemAssCondList().addto(cond);
	if(!TheToken.isEqualTo(":"))	
		error("Model::LoadSemAssAtt(): Need \":\"",TheToken);

	TheToken.output(' ');
	TheToken.output();
	TheToken.output('\n');
	TheToken.input();
	if(!TheToken.isEqualTo("{")) 
		error("Model::LoadSemAssAtt(): Should be \"{\"",TheToken);

	TheToken.output('\t');
	TheToken.output();
	TheToken.output('\n');
	TheToken.output('\t');
	TheToken.output('\t');
	TheToken.input();
	LoadAtt_Set(TheToken,SortedAttList, link->getCondAttList(),SEMANTIC,LEXICAL,EXTERNAL,DYNAMIC,MTRELATIVE);

	if(!TheToken.isEqualTo("}")) 
		error("Model::LoadSemAssAtt(): Need \"}\"",TheToken.getBuffer());

	TheToken.output('\n');
	TheToken.output('\t');
	TheToken.output();
	TheToken.output('\n');
	TheToken.input();
}

void Model::LoadSemAssVal(IOToken &TheToken)
{
	if( !TheToken.isEqualTo("(") ) 
		error("Model::LoadSemAssVal(): Should be \"(\"",TheToken );
	TheToken.output(' ');
	TheToken.output();

	TheToken.input();
	if( TheToken.getType()!=IDENT ) 
		error("Model::LoadSemAssVal(): Should be IDENT",TheToken );
	Attribute *attr;
	attr=SortedAttList.find(TheToken.getBuffer());
	if( !attr ) error("Model::LoadSemAssVal(): Undefined SemAssAtt Attribute",TheToken );

	if( attr->getLang()==SEMANTIC )
	{
		if( attr->getIntExt()!=EXTERNAL ) 
			error("Model::LoadSemAssVal()", "Wrong: Should be SemAssAtt Attribute",TheToken );
	}
	else error("Model::LoadSemAssVal()", "Wrong: Should be SemAssAtt Attribute,not LexPhr Attribute",TheToken);

	if( attr->getNameAlias()==ALIAS ) attr=attr->getRefer();

	TheToken.output();
	TheToken.input();
	if( !TheToken.isEqualTo(")") )
		error("Model::LoadSemAssVal(): Should be \")\"",TheToken );
	TheToken.output();
	TheToken.output(' ');
	
	TheToken.input();
	if( !TheToken.isEqualTo(":") )
		error("Model::LoadSemAssVal()", "Wrong: Should be \":\"",TheToken );
	TheToken.output();
	TheToken.output(' ');
	TheToken.input();

	LoadFestVal(TheToken,attr);
}

void Model::LoadFestVal(IOToken &TheToken,Attribute *attr)
{
	if( TheToken.isEqualTo("RelMode") ||  TheToken.isEqualTo("Static") ||
		TheToken.isEqualTo("Dynamic") ) LoadValMode(TheToken,attr);
	else if( TheToken.isEqualTo("Default") ) {
		LoadDefFest(TheToken,attr);
		if( TheToken.isEqualTo("RelMode") || TheToken.isEqualTo("Dynamic") || 
		TheToken.isEqualTo("Static") ) LoadValMode(TheToken,attr);
	}
}

int Model::LoadSynModel(IOToken &TheToken)
{
	char *TempPntr;
	TempPntr=TheToken.getOutPntr();
	TheToken.input();
	if(!TheToken.isEqualTo("SynModel")) 
		error("Model::LoadSynModel(): Need \"SynModel\"",TheToken.getBuffer());

	TheToken.output();
	TheToken.input();

	int lang;
	lang=LoadLangCode(TheToken);
	LoadSynModelBody(TheToken,lang);

	if(!TheToken.isEqualTo("EndModel")) 
		error("Model::LoadSynModel(): Need \"EndModel\"",TheToken.getBuffer());

	TheToken.output('\n');
	TheToken.output();
	TheToken.output('\n');
	TheToken.output('\n');
	int num=TheToken.getOutPntr()-TempPntr;
	TheToken.outputNull();
	return num;
}

void Model::LoadSynModelBody(IOToken &TheToken,int lang)
{
	LoadLexCat(TheToken,lang);
	LoadLexFeat(TheToken,lang);
	LoadPhrCat(TheToken,lang);
	LoadPhrFeat(TheToken,lang);
	LoadSynAss(TheToken,lang);
	LoadSpecialField(TheToken,lang);
}

int	 Model::LoadLangCode(IOToken &TheToken)
{
	if(!TheToken.isEqualTo("("))
		error("Model::LoadLangCode(): Need \"(\"",TheToken.getBuffer());

	TheToken.output(' ');
	TheToken.output();
	TheToken.input();

	int i;
	for(i=0;i<LANGNUM;i++) if(TheToken.isEqualTo(LANGSTRING[i])) break;
	if(i<LANGNUM) asynModel[i].setHas(YES);
	else error("Model::LoadLangCode(): Invalid LangCode");

	TheToken.output();
	TheToken.input();

	if(!TheToken.isEqualTo(")")) 
		error("Model::LoadLangCode(): Need \")\"",TheToken.getBuffer());

	TheToken.output();
	TheToken.output('\n');
	TheToken.input();
	return i;
}

void Model::LoadLexCat(IOToken &TheToken,int lang)
{
	if(!TheToken.isEqualTo("LexCat")) 
	 	error("Model::LoadLexCat(): Need \"LexCat\"",TheToken.getBuffer());

	TheToken.output('\t');
	TheToken.output();
	TheToken.input();
	if(!TheToken.isEqualTo(":")) 
		error("Model::LoadLexCat(): Need \":\"",TheToken.getBuffer());

	TheToken.output(' ');
	TheToken.output();
	TheToken.input();
	LoadAttribute(TheToken,SortedAttList,asynModel[lang].getLexCat(),NO,lang,LEXICAL,INTERNAL,STATIC,MTRELATIVE,SYMBOL,LIMITED);

	if(!TheToken.isEqualTo(":"))
		error("Model::LoadLexCat(): Need \":\"",TheToken.getBuffer());

	TheToken.output(' ');
 	TheToken.output();
	TheToken.output(' ');
   	TheToken.input();
	if(!TheToken.isEqualTo("{")) 
		error("Model::LoadLexCat(): Need \"{\"",TheToken.getBuffer());

	TheToken.output('\n');
	TheToken.output('\t');
  	TheToken.output();
	TheToken.output('\n');
	TheToken.output('\t');
	TheToken.output('\t');
	TheToken.input();
	// add the vallist of LexCat 
	LoadHieSym_Set(TheToken,asynModel[lang].getLexCat().getAttLinkHead()->getAttr()->getValList());

  	if(!TheToken.isEqualTo("}"))  
		error("Model::LoadLexCat(): Need \"}\"",TheToken.getBuffer());
	TheToken.output('\n');
	TheToken.output('\t');
	TheToken.output();
	TheToken.input();

}

void Model::LoadLexFeat(IOToken &TheToken,int lang)
{
	if( !TheToken.isEqualTo("LexAtt") && !TheToken.isEqualTo("LexVal") )
		error("Model::LoadLexFeat(): Should be LexAtt or LexVal Attribute",TheToken);
	while(TheToken.isEqualTo("LexAtt")|| TheToken.isEqualTo("LexVal"))
	{
		const char * errorpos=TheToken.getInPntr();
		try
		{
			TheToken.output('\n');
	  		TheToken.output('\t');
			if(TheToken.isEqualTo("LexAtt")) {
				TheToken.output();
				TheToken.input();
				LoadLexAtt(TheToken,lang);
			}
			else {
				TheToken.output();
				TheToken.input();
				LoadLexVal(TheToken,lang);
			}
		}
		catch (CErrorException *)
		{
			message("Position:",errorpos);
			throw;
		}
	}
}

void Model::LoadLexAtt(IOToken &TheToken,int lang)
{
	char buff[TOKENLENGTH];
	if(TheToken.isEqualTo("("))
	{
		TheToken.output();
		TheToken.input();
		if(!TheToken.isEqualTo("BaseForm") && !TheToken.isEqualTo("VaryForm")) 
			error("Model::LoadLexAtt(): Need \"BaseForm\" or \"VaryForm\"",TheToken,TheToken.getInPntr());

		if(TheToken.isEqualTo("BaseForm")){
	   		TheToken.output();
			TheToken.input();
			if(!TheToken.isEqualTo(")")) 
				error("Model::LoadLexAtt(): Need \")\"",TheToken,TheToken.getInPntr());

			TheToken.output();
			TheToken.input();
			if(!TheToken.isEqualTo(":")) 
				error("Model::LoadLexAtt(): Need \":\"",TheToken,TheToken.getInPntr());

	 		TheToken.output(' ');
			TheToken.output();
	 		TheToken.output(' ');
			TheToken.input();
			strcpy(buff,TheToken.getBuffer());
			LoadAttribute(TheToken,SortedAttList,asynModel[lang].getBaseForm(),YES,lang,LEXICAL,INTERNAL,STATIC,MTRELATIVE,SYMBOL,UNLIMITED);
			Attribute *attr=SortedAttList.find(buff);
			// attr->setDefault("NONE");
		 }
		else if(TheToken.isEqualTo("VaryForm")){
			TheToken.output();
			TheToken.input();
			if(!TheToken.isEqualTo(")")) 
				error("Model::LoadLexAtt(): Need \")\"",TheToken,TheToken.getInPntr());

			TheToken.output();
			TheToken.input();
			if(!TheToken.isEqualTo(":")) 
				error("Model::LoadLexAtt(): Need \":\"",TheToken,TheToken.getInPntr());

	 		TheToken.output(' ');
			TheToken.output();
	 		TheToken.output(' ');
			TheToken.input();
			strcpy(buff,TheToken.getBuffer());
			LoadAttribute(TheToken,SortedAttList,asynModel[lang].getVaryForm(),YES,lang,LEXICAL,INTERNAL,STATIC,MTRELATIVE,SYMBOL,UNLIMITED);
			Attribute *attr=SortedAttList.find(buff);
			// attr->setDefault("NONE");
		}
	}
	else
	{
	 	TheToken.output(' ');
		char *cond;
		if( TheToken.isEqualTo("["))
			cond=LoadCondition(TheToken,SortedAttList);
		else { cond=new char[3]; strcpy(cond,"[]"); }
	 	TheToken.output(' ');

		CondLink *link=asynModel[lang].getLexStaDynCondList().addto(cond);
		if(!TheToken.isEqualTo(":")) 
			error("Model::LoadLexAtt(): Need \":\"",TheToken,TheToken.getInPntr());
		TheToken.output();

	 	TheToken.output('\n');
		TheToken.input();
		if(!TheToken.isEqualTo("{")) 
			error("Model::LoadLexAtt(): Need \"{\"",TheToken,TheToken.getInPntr());

 		TheToken.output('\t');
		TheToken.output();
 		TheToken.output('\n');
 		TheToken.output('\t');
 		TheToken.output('\t');
		TheToken.input();
		LoadAtt_Set(TheToken,SortedAttList,link->getCondAttList(),lang,LEXICAL,INTERNAL,STATIC,MTRELATIVE);
		if(!TheToken.isEqualTo("}")) 
			error("Model::LoadLexAtt(): Need \"}\"",TheToken,TheToken.getInPntr());

	 	TheToken.output('\n');
	 	TheToken.output('\t');
		TheToken.output();
		TheToken.input();
			
	}
}

void Model::LoadLexVal(IOToken &TheToken,int lang)
{
	TheToken.output(' ');
	if(!TheToken.isEqualTo("(")) 
		error("Model::LoadLexVal(): Need \"(\"",TheToken,TheToken.getInPntr());

	TheToken.output();
	TheToken.input();
	if(TheToken.getType()!=IDENT)
		error("Model::LoadLexVal(): Not Indent",TheToken,TheToken.getInPntr());

	Attribute *p;
	p=SortedAttList.find(TheToken.getBuffer());
	if(!p) error("Model::LoadLexVal(): Undefined LexAtt",TheToken); 

	Attribute *q;
	q=asynModel[lang].getLexCat().find(TheToken.getBuffer());
	if( q ) error("Model::LoadLexVal(): Should be LexAtt,not LexCat Attribute",TheToken);

	Attribute *pp;
	if(p->getNameAlias()==ALIAS) pp=p->getRefer();
	else pp=p;

	TheToken.output();
	TheToken.input();
	if(!TheToken.isEqualTo(")")) 
		error("Model::LoadLexVal(): Need \")\"",TheToken,TheToken.getInPntr());

	TheToken.output();
	TheToken.input();
	if(!TheToken.isEqualTo(":")) 
		error("Model::LoadLexVal()\nWorng: Need \":\"",TheToken,TheToken.getInPntr());

	TheToken.output(' ');
	TheToken.output();
	TheToken.output(' ');
	TheToken.input();	

	LoadAtomVal(TheToken,pp);
}

void Model::LoadPhrCat(IOToken &TheToken,int lang)
{
	TheToken.output('\n');
	TheToken.output('\t');
	if(!TheToken.isEqualTo("PhrCat")) 
		error("Model::LoadPhrCat(): Need \"PhrCat\"",TheToken);

	TheToken.output();
	TheToken.input();
	if(!TheToken.isEqualTo(":")) 
		error("Model::LoadPhrCat(): Need \":\"",TheToken.getBuffer());

	TheToken.output(' '); 
	TheToken.output();
	TheToken.output(' ');
	TheToken.input();
	LoadAttribute(TheToken,SortedAttList,asynModel[lang].getPhrCat(),NO,lang,PHRASAL,
				  INTERNAL,STATIC,IRRELATIVE,SYMBOL,LIMITED);

	if(!TheToken.isEqualTo(":")) 
		error("Model::LoadPhrCat(): Need \":\"",TheToken.getBuffer());

	TheToken.output(' ');
	TheToken.output();
	TheToken.output('\n');
	TheToken.input();
	if(!TheToken.isEqualTo("{"))
		error("Model::LoadPhrCat(): Need \"{\"",TheToken.getBuffer());

	TheToken.output('\t');
	TheToken.output();
	TheToken.output('\n');
	TheToken.input();
	TheToken.output('\t');
	TheToken.output('\t');
	LoadHieSym_Set(TheToken,asynModel[lang].getPhrCat().getAttLinkHead()->getAttr()->getValList());

	if(!TheToken.isEqualTo("}")) 
		error("Model::LoadPhrCat(): Need \"}\"",TheToken.getBuffer());

	TheToken.output('\n');
	TheToken.output('\t');
	TheToken.output();
	TheToken.input();
}

void Model::LoadPhrFeat(IOToken &TheToken,int lang)
{
	if( !TheToken.isEqualTo("PhrAtt") && !TheToken.isEqualTo("PhrVal") )
		error("Model::LoadPhrFeat: Should be \"PhrAtt\" or \"PhrVal\"",TheToken );
	while(TheToken.isEqualTo("PhrAtt") || TheToken.isEqualTo("PhrVal"))
	{
		const char * errorpos=TheToken.getInPntr();
		try
		{
			TheToken.output('\n');
			TheToken.output('\t');
			if(TheToken.isEqualTo("PhrAtt")) {
				TheToken.output();
				TheToken.input();
				LoadPhrAtt(TheToken,lang);
			}
			else {
				TheToken.output();
				TheToken.input();
				LoadPhrVal(TheToken,lang);
			}
		}
		catch (CErrorException *)
		{
			message("Position:",errorpos);
			throw;
		}
	}
}

void Model::LoadPhrAtt(IOToken &TheToken,int lang)
{
	char *cond;
	if( TheToken.isEqualTo("[") )
		cond=LoadCondition(TheToken,SortedAttList);
	else { cond=new char[3]; strcpy(cond,"[]"); }

	CondLink *link =asynModel[lang].getPhrCondList().addto(cond);

	if(!TheToken.isEqualTo(":"))
		error("Model::LoadPhrAtt(): Need \":\"",TheToken,TheToken.getInPntr());

	TheToken.output(' ');
	TheToken.output();
	TheToken.input();
	if(!TheToken.isEqualTo("{"))
		error("Model::LoadPhrAtt(): Need \"{\"",TheToken,TheToken.getInPntr());

	TheToken.output('\n');
	TheToken.output('\t');
	TheToken.output();
	TheToken.output('\n');
	TheToken.output('\t');
	TheToken.output('\t');
	TheToken.input();
	LoadAtt_Set(TheToken,SortedAttList,link->getCondAttList(),lang,PHRASAL,INTERNAL,STATIC,IRRELATIVE);
		
	if(!TheToken.isEqualTo("}"))  
		error("Model::LoadPhrAtt(): Need \"}\"",TheToken,TheToken.getInPntr());

	TheToken.output('\n');
	TheToken.output('\t');
	TheToken.output();
	TheToken.input();
}

void Model::LoadPhrVal(IOToken &TheToken,int lang)
{
	TheToken.output(' ');
	if(!TheToken.isEqualTo("(")) 
		error("Model::LoadPhrVal(): Should be \"(\"",TheToken,TheToken.getInPntr());

	TheToken.output();
	TheToken.input();
	if(TheToken.getType()!=IDENT) 
		error("Model::LoadPhrVal(): Invalid Ident",TheToken,TheToken.getInPntr());

	Attribute *p;
	Attribute *pp;
	p=SortedAttList.find(TheToken.getBuffer());
	if(!p) error("Model::LoadPhrVal(): Undefined PhrAtt",TheToken,TheToken.getInPntr()); 

	Attribute *q;
	q=asynModel[lang].getPhrCat().find(TheToken.getBuffer());
	if( q ) error("Model::LoadPhrVal(): Should be PhrAtt,not PhrCat Attriubte.",TheToken );

	if(p->getNameAlias()==ALIAS) pp=p->getRefer();
	else pp=p;

	TheToken.output();
	TheToken.input();
	if(!TheToken.isEqualTo(")")) 
		error("Model::LoadPhrVal(): Should be \")\"",TheToken,TheToken.getInPntr());

	TheToken.output();
	TheToken.input();
	if(!TheToken.isEqualTo(":")) 
		error("Model::LoadPhrVal(): Should be \":\"",TheToken,TheToken.getInPntr());

	TheToken.output(' ');
	TheToken.output();
	TheToken.output(' ');
	TheToken.input();
	LoadAtomVal(TheToken,pp);
}

void Model::LoadSynAss(IOToken &TheToken,int lang)
{
	if( !TheToken.isEqualTo("SynAssAtt") && !TheToken.isEqualTo("SynAssVal") )
		error("Model::LoadSynAss(): Should be \"SynAssAtt\" or \"SynAssVal\"",TheToken );
	while( TheToken.isEqualTo("SynAssAtt") || TheToken.isEqualTo("SynAssVal") )
	{
		TheToken.output("\n\t");
		if( TheToken.isEqualTo("SynAssAtt") )  
		{
			TheToken.output();
			TheToken.output(' ');
			TheToken.input();
			LoadSynAssAtt(TheToken,lang);
		}
		else 
		{
			TheToken.output();
			TheToken.output(' ');
			TheToken.input();
			LoadSynAssVal(TheToken);
		}
	}
}

void Model::LoadSynAssAtt(IOToken &TheToken,int lang)
{
	char *cond;
	if( TheToken.isEqualTo("[")) 
		cond=LoadCondition(TheToken,SortedAttList);
	else { cond=new char[3]; strcpy(cond,"[]"); }
	
	CondLink *link=asynModel[lang].getSynAssCondList().addto(cond);
	if( !TheToken.isEqualTo(":") )
		error("Model::LoadSynAssAtt():  Need \":\"",TheToken );
	TheToken.output(' ');
	TheToken.output();
	TheToken.output('\n');
	TheToken.input();
	if( !TheToken.isEqualTo("{") )
		error("Model::LoadSynAssAtt(): Need \"{\"",TheToken );
	TheToken.output('\t');
	TheToken.output();
	TheToken.output("\n\t\t");
	TheToken.input();
	LoadAtt_Set(TheToken,SortedAttList,link->getCondAttList(),lang,PHRASAL,EXTERNAL,STATIC,IRRELATIVE);
	if( !TheToken.isEqualTo("}") )
		error("Model::LoadSynAssAtt()", "Wrong: Need \"}\"",TheToken );
	
	TheToken.output("\n\t");
	TheToken.output();
	TheToken.input();
}

void Model::LoadSynAssVal(IOToken &TheToken)
{
	if( !TheToken.isEqualTo("(") ) 
		error("Model::LoadSynAssVal(): Should be \"(\"",TheToken );
	TheToken.output(' ');
	TheToken.output();

	TheToken.input();
	if( TheToken.getType()!=IDENT ) 
		error("Model::LoadSynAssVal(): Should be IDENT",TheToken );
	Attribute *attr;
	attr=SortedAttList.find(TheToken.getBuffer());
	if( !attr ) error("Model::LoadSynAssVal(): Undefined SynAssAtt",TheToken );

	if( attr->getIntExt()==EXTERNAL ) {
		if( attr->getLang()==SEMANTIC )
		 error("Model::LoadSynAssVal()", "Wrong: Should be SynAssAtt,not SemAssAtt",TheToken );
	}
	else error("Model::LoadSynAssVal()", "Wrong: Should be SynAssAtt,not LexPhrAtt.",TheToken );

	if( attr->getNameAlias()==ALIAS ) attr=attr->getRefer();

	TheToken.output();
	TheToken.input();
	if( !TheToken.isEqualTo(")") ) 
		error("Model::LoadSynAssVal()", "Wrong: should be \")\"",TheToken );
	TheToken.output();
	TheToken.output(' ');
	
	TheToken.input();
	if( !TheToken.isEqualTo(":") )
		error("Model::LoadSynAssVal()", "Wrong: Should be \":\"",TheToken );
	TheToken.output();
	TheToken.output(' ');
	TheToken.input();

	LoadFestVal(TheToken,attr);
}

void Model::LoadSpecialField(IOToken &TheToken,int lang)
{
	LoadDigitField(TheToken,lang);
	LoadPunctField(TheToken,lang);
	LoadOtherField(TheToken,lang);
}

void Model::LoadDigitField(IOToken &TheToken,int lang)
{
	if (!TheToken.isIdent("Field"))
		error("Model::LoadDigit(): Should be \"Field\"",TheToken);
	TheToken.output("\n\t");
	TheToken.output();

	TheToken.input();
	if( !TheToken.isEqualTo("(")) 
		error("Model::LoadDigit()", "Wrong: Should be \"(\"",TheToken );
	TheToken.output();

	TheToken.input();
	if( !TheToken.isEqualTo("Digit")) 
		error("Model::LoadDigit():  Should be \"Digit\"",TheToken);
	TheToken.output();
	
	TheToken.input();
	if( !TheToken.isEqualTo(")"))
		error("Model::LoadDigit():  Should be \")\"", TheToken );
	TheToken.output();

	TheToken.input();
	if (!TheToken.isDelim(":"))
		error("Model::LoadDigit(): Should be \":\"",TheToken);
	TheToken.output(" : ");
	TheToken.input();		// read ident 

	cValList & vallist=getLexCat(lang)->getValList();

	const char * p=TheToken.getInPntr();

	if (!vallist.find(TheToken))
		error("Model::LoadDigit(): Invalid Label",TheToken);
	TheToken.output();
	TheToken.output(' ');
	TheToken.input();

	if( TheToken.isEqualTo("[") || TheToken.isEqualTo("{") ) 
		LoadFest(TheToken,SortedAttList);

	const char *q=TheToken.getInPntr();

	int size=q-p;
	char * field=new char[size+1];
	strncpy(field,p,size);
	field[size]=0;
	asynModel[lang].setDgtCat(field);
}

void Model::LoadOtherField(IOToken &TheToken,int lang)
{
	if (!TheToken.isIdent("Field"))
		error("Model::LoadOtherField(): Should be \"Field\"",TheToken);
	TheToken.output("\n\t");
	TheToken.output();

	TheToken.input();
	if( !TheToken.isEqualTo("(")) 
		error("Model::LoadOtherField(): Should be \"(\"",TheToken );
	TheToken.output();

	TheToken.input();
	if( !TheToken.isEqualTo("Other")) 
		error("Model::LoadOtherField()", "Wrong: Should be \"Other\"",TheToken );
	TheToken.output();

	TheToken.input();
	if( !TheToken.isEqualTo(")"))
		error("Model::LoadOtherField(): Should be \")\"", TheToken );
	TheToken.output();

	TheToken.input();
	if (!TheToken.isDelim(":"))
		error("Model::LoadOtherField(): Should be \":\"",TheToken);
	TheToken.output(" : ");
	TheToken.input();			// read ident

	cValList & vallist=getLexCat(lang)->getValList();

	const char * p=TheToken.getInPntr();

	if (!vallist.find(TheToken))
		error("Model::LoadOtherField(): Invalid Label",TheToken);
	TheToken.output();
	TheToken.output(' ');
	TheToken.input();

	if( TheToken.isEqualTo("[") || TheToken.isEqualTo("{") ) 
		LoadFest(TheToken,SortedAttList);

	const char *q=TheToken.getInPntr();

	int size=q-p;
	char * field=new char[size+1];
	strncpy(field,p,size);
	field[size]=0;
	asynModel[lang].setOthCat(field);
}

void Model::LoadPunctField(IOToken &TheToken,int lang)
{
	if (!TheToken.isIdent("Field"))
		error("Model::LoadPunctField(): Should be \"Field\"",TheToken);
	TheToken.output("\n\t");
	TheToken.output();

	TheToken.input();
	if( !TheToken.isEqualTo("(")) 
		error("Model::LoadPunctField(): Should be \"(\"",TheToken );
	TheToken.output();

	TheToken.input();
	if( !TheToken.isEqualTo("Punct")) 
		error("Model::LoadPunctField(): Should be \"Punct\"",TheToken );
	TheToken.output();

	TheToken.input();
	if( !TheToken.isEqualTo(")"))
		error("Model::LoadPunctField(): Should be \")\"", TheToken);
	TheToken.output();

	TheToken.input();
	if (!TheToken.isDelim(":"))
		error("Model::LoadPunctField(): Should be \":\"",TheToken);
	TheToken.output(" : ");
	TheToken.input();		// read ident

	cValList & vallist=getLexCat(lang)->getValList();

	const char * p=TheToken.getInPntr();

	if (!vallist.find(TheToken))
		error("Model::LoadPunctField(): Invalid Label",TheToken);
	TheToken.output();
	TheToken.output(' ');
	TheToken.input();

	if( TheToken.isEqualTo("[") || TheToken.isEqualTo("{") ) 
		LoadFest(TheToken,SortedAttList);

	const char *q=TheToken.getInPntr();

	int size=q-p;
	char * field=new char[size+1];
	strncpy(field,p,size);
	field[size]=0;
	asynModel[lang].setPctCat(field);
}
#endif 

void Model::ReadSemModel(IToken &TheToken)
{
	TheToken.input();	// read "SemModel"
	TheToken.input();	// read "SemCat"
	ReadSemModelBody(TheToken);
}

void Model::ReadSemModelBody(IToken &TheToken)
{
	ReadSemCat(TheToken);
	ReadSemFeat(TheToken);
	ReadSemAss(TheToken);
}

void Model::ReadSemCat(IToken &TheToken)
{
	TheToken.input();	 // read ":"
	TheToken.input();  	 // read SemCatAttribute
	ReadAttribute(TheToken,SortedAttList,asemModel.getSemCat(),NO,SEMANTIC,LEXICAL,INTERNAL,STATIC,MTRELATIVE,HIERAR,LIMITED);
	// has  read ":"
	
	TheToken.input(); 	// read "{"
  	TheToken.input();	// read SemCatAttribute's ValList
	ReadHieSym_Set(TheToken,asemModel.getSemCat().getAttLinkHead()->getAttr()->getValList());

	//during ReadHieSym_Set ,read "}"
	TheToken.input();				 
}

void Model::ReadHieSym_Set(IToken &TheToken,class cValList &vallist)
{
	ReadHieSym_Atom(TheToken,vallist);
	while(TheToken.isEqualTo(","))
	{
		 TheToken.input();
		 ReadHieSym_Atom(TheToken,vallist);
	}
}

void Model::ReadHieSym_Atom(IToken &TheToken, class cValList &vallist)
{
	Value *val=new Value;
	ReadAtom_Name(TheToken,val);
	vallist.addto(val);
	int firstalias=1;
	while(TheToken.isEqualTo("=")){
		TheToken.input();
		Value *valalias=new Value; 
		ReadAtom_Alias(TheToken,valalias);
		vallist.addto(valalias);
		if(firstalias==1) val->setRefer(valalias);
		else firstalias=0;
		valalias->setRefer(val);
	}
}

void Model::ReadAtom_Name(IToken &TheToken,Value *val)
{
	val->setNameAlias(NAME);
	val->setString(TheToken.getBuffer());
	TheToken.input();
}

void Model::ReadAtom_Alias(IToken &TheToken,Value *val)
{
	val->setNameAlias(ALIAS);
	val->setString(TheToken.getBuffer());
	TheToken.input();
}

void Model::ReadSemFeat(IToken &TheToken)
{
	while(TheToken.isEqualTo("SemAtt") || TheToken.isEqualTo("SemVal"))
	{
		if(TheToken.isEqualTo("SemAtt")){
			TheToken.input();				
			ReadSemAtt(TheToken);
		}
		else if(TheToken.isEqualTo("SemVal")) {
			TheToken.input();
			ReadSemVal(TheToken);
		}
	}
}

void Model::ReadSemAtt(IToken &TheToken)
{
	char *cond;
	if( TheToken.isEqualTo("[")) 
		cond=ReadCondition(TheToken);
	else { cond=new char[3]; strcpy(cond,"[]"); }

	CondLink *link=asemModel.getSemAttCondList().addto( cond );
	// during rdCondition,read ":"

	TheToken.input();	// read "{"
	TheToken.input();	// read AttributeSet
	ReadAtt_Set(TheToken,SortedAttList, link->getCondAttList(),SEMANTIC,LEXICAL,INTERNAL,STATIC,MTRELATIVE);
	// during rdAtt_Set ,read "}"

	TheToken.input();
}

char *Model::ReadCondition(IToken &TheToken)
{
	const char *p;
	const char *q;
	p=TheToken.getInPntr();
	ReadFest(TheToken);
	q=TheToken.getInPntr();
	int size=q-p;
	char *TempBuff;
	TempBuff=new char[size+1];
	strncpy(TempBuff,p,size);
	TempBuff[size]='\0';
	return (TempBuff);
}

void Model::ReadAtt_Set(IToken &TheToken,class SortedAttList &attlist,class UnsortAttList &unattlist,unsigned lang,unsigned lexphr,unsigned intext,unsigned stadyn,unsigned relative,unsigned atttype,unsigned limit)
{
	ReadAttribute(TheToken,attlist,unattlist,YES,lang,lexphr,intext,stadyn,relative,atttype,limit);
	while(TheToken.isEqualTo(",")){
		TheToken.input();
		ReadAttribute(TheToken,attlist,unattlist,YES,lang,lexphr,intext,stadyn,relative,atttype,limit);
	}
}

void Model::ReadAttribute(IToken &TheToken,class SortedAttList &attlist,class UnsortAttList &unattlist,unsigned duppermit,unsigned lang,unsigned lexphr,unsigned intext,unsigned stadyn,unsigned relative,unsigned atttype,unsigned limit)
{
	unsigned exist;
	Attribute *p;
	Attribute *attr;

	p=SortedAttList.find(TheToken.getBuffer());
	exist=p?YES :NO;

	int firstalias=1;
	if(exist==NO){
		attr=new Attribute;
		attr=ReadAtt_Name(TheToken,attr);
		attr->setLang(lang);
		attr->setIntExt(intext);
		attr->setStaDyn(stadyn);
		attr->setLexPhr(lexphr);
		attr->setRelative(relative);
		attr->setAttType(atttype);
		attr->setLimit(limit);
		attlist.addto(attr);
		unattlist.addto(attr);
		while(TheToken.isEqualTo("=")){
			TheToken.input();
			Attribute *aliasattr;
			aliasattr=new Attribute;
			aliasattr=ReadAtt_Alias(TheToken,aliasattr);
			attr->setLang(lang);
			aliasattr->setIntExt(intext);
			aliasattr->setStaDyn(stadyn);
			aliasattr->setLexPhr(lexphr);
			aliasattr->setRelative(relative);
			aliasattr->setAttType(atttype);
			aliasattr->setLimit(limit);
			attlist.addto(aliasattr);
			unattlist.addto(aliasattr);
			aliasattr->setRefer(attr);
			if (firstalias==1) {
				attr->setRefer(aliasattr);
				firstalias=0;
			}	
		}
	}
	else { /* exist==YES find the name in SortedAttList */ 
		if(duppermit==YES){
			attr=p;
			unattlist.addto(attr);
			TheToken.input();
		}
	}
}


Attribute *Model::ReadAtt_Name(IToken &TheToken,Attribute * attr)
{
	attr->setNameAlias(NAME);
	attr->setString(TheToken.getBuffer());
	TheToken.input();
	return attr;
}

Attribute *Model::ReadAtt_Alias(IToken &TheToken,Attribute * attr)
{
	attr->setNameAlias(ALIAS);
	attr->setString(TheToken.getBuffer());
	TheToken.input();
	return attr;
}

void Model::ReadSemVal(IToken &TheToken)
{
	// has read "("

	TheToken.input(); 	// read SemAtt in SemVal
	Attribute *pp;
	pp=SortedAttList.find(TheToken.getBuffer());
	if(pp->getNameAlias()==ALIAS) pp=pp->getRefer();
	TheToken.input();	// read ")"
	TheToken.input();	// read ":"
	TheToken.input();	// read SemAtt's ValList
	ReadAtomVal(TheToken,pp);
}

void Model::ReadAtomVal(IToken &TheToken,Attribute *attr)
{
	ReadAtomValType(TheToken,attr);
	if( TheToken.isEqualTo("RelMode") || TheToken.isEqualTo("Static") ||
		TheToken.isEqualTo("Dynamic") ) 
		ReadValMode(TheToken,attr);
}

void Model::ReadAtomValType(IToken &TheToken,Attribute *attr)
{
	unsigned type;
	type=ReadAtomType(TheToken,attr);
	if(TheToken.isEqualTo("Default")) ReadDefAtom(TheToken,type,attr);
}

void Model::ReadValMode(IToken &TheToken,Attribute *attr)
{
	if( TheToken.isEqualTo("RelMode") ) ReadRelMode(TheToken,attr);
	else {
		ReadActMode(TheToken,attr);
		if( TheToken.isEqualTo("RelMode") ) ReadRelMode(TheToken,attr);
	}
}

void Model::ReadActMode(IToken &TheToken,Attribute *attr)
{
	if( TheToken.isEqualTo("STATIC") ) attr->setStaDyn(STATIC);
	else attr->setStaDyn(DYNAMIC);
	TheToken.input();
}

void Model::ReadRelMode(IToken &TheToken,Attribute *attr)
{
	TheToken.input();  // input "="
	TheToken.input();
	if(TheToken.isEqualTo("0")) attr->setRelative(IRRELATIVE);
	else  attr->setRelative(MTRELATIVE);
	TheToken.input();
}

unsigned Model::ReadAtomType(IToken &TheToken,Attribute * attr)
{
	unsigned Type;
	if(TheToken.isEqualTo("Hierar")) Type=HIERAR;
	else if(TheToken.isEqualTo("Symbol")) Type=SYMBOL;
	else if(TheToken.isEqualTo("Number")) Type=NUMBER;
	else if(TheToken.isEqualTo("Boolean")) Type=MTBOOL;
	TheToken.input();
	attr->setAttType(Type);
	if(TheToken.isEqualTo("Limited") || TheToken.isEqualTo("Unlimited"))
	{
		unsigned limit;
		if(TheToken.isEqualTo("Limited")) limit=LIMITED;
		else limit=UNLIMITED;
		attr->setLimit(limit);
		TheToken.input();
	}
	if(TheToken.isEqualTo("{"))
	{
		if((Type==HIERAR)||(Type==SYMBOL)){
			TheToken.input();
			ReadHieSym_Set(TheToken,attr->getValList());
		}
		else if(Type==NUMBER) {
			TheToken.input();
			ReadNumber_Set(TheToken,attr->getValList());
		}
	  	else if(Type==MTBOOL) {
			TheToken.input();
			ReadBoolean_Set(TheToken,attr->getValList());
		}
		// has read "}"

		TheToken.input();
 	}
	return Type;
}

void Model::ReadNumber_Set(IToken &TheToken,class cValList &vallist)
{
	ReadNumber_Atom(TheToken,vallist);
	while(TheToken.isEqualTo(","))
	{
		TheToken.input();
		ReadNumber_Atom(TheToken,vallist);
	}
}

void Model::ReadNumber_Atom(IToken &TheToken,class cValList &vallist)
{
	Value *val=new Value;
	val->setNameAlias(NAME);
	val->setString(TheToken.getBuffer());
	vallist.addto(val);
	TheToken.input();
	int firstalias=1;
	while(TheToken.isEqualTo("="))
	{
		TheToken.input();
		Value *valalias=new Value;
		ReadAtom_Alias(TheToken,valalias);
	 	vallist.addto(valalias);
		if(firstalias==1) val->setRefer(valalias);
		else firstalias=0;
		valalias->setRefer(val);
	}	
}

void Model::ReadBoolean_Set(IToken &TheToken,class cValList &vallist)
{
	ReadBoolean_Atom(TheToken,vallist);
	while(TheToken.isEqualTo(","))
	{
		TheToken.input();
		ReadBoolean_Atom(TheToken,vallist);
	}
}

void Model::ReadBoolean_Atom(IToken &TheToken,class cValList &vallist)
{
	if(TheToken.isEqualTo("ÊÇ")||TheToken.isEqualTo("·ñ")||
		TheToken.isEqualTo("YES")|| TheToken.isEqualTo("NO"))
	{
		Value *val=new Value;
		val->setNameAlias(NAME);
		val->setString(TheToken.getBuffer());
		vallist.addto(val);
		TheToken.input();
		int firstalias=1;
		while(TheToken.isEqualTo("=")){
			TheToken.input();
			Value *valalias= new Value;
			ReadAtom_Alias(TheToken,valalias);
			vallist.addto(valalias);
			if (firstalias==1) val->setRefer(valalias);
			else firstalias=0;
			valalias->setRefer(val);
		}
	}
}

void Model::ReadDefAtom(IToken &TheToken,unsigned type,Attribute *attr)
{
	TheToken.input();	// input "="
	TheToken.input();
	const char *p,*q;
	char *end,*beg;
	p=TheToken.getInPntr();
	beg=(char *)p;
	ReadAtom(TheToken,type);
	q=TheToken.getInPntr();
	end=(char *)q;
	end--;
	while (isascii(*end) && isspace(*end)) end--;
	int size;
	size=end-beg+1;
	char * TempBuff=new char [size+1];	
	strncpy(TempBuff,p,size);
	TempBuff[size]='\0';
	if (attr->getDefault()) delete attr->getDefault();
	attr->setDefault(TempBuff);	
}

void Model::ReadDefFest(IToken &TheToken,Attribute *attr)
{
	TheToken.input();
	TheToken.input();
	const char *p,*q;
	p=TheToken.getInPntr();
	ReadFest(TheToken);
	q=TheToken.getInPntr();
	int size=q-p;
	char *TempBuff=new char[size+1];
	strncpy(TempBuff,p,size);
	TempBuff[size]='\0';
	if (attr->getDefault()) delete attr->getDefault();
	attr->setDefault(TempBuff);
}

void Model::ReadSemAss(IToken &TheToken)
{
	while( TheToken.isEqualTo("SemAssAtt") || TheToken.isEqualTo("SemAssVal") )
	{	
		if( TheToken.isEqualTo("SemAssAtt") ) 
		{ 
			TheToken.input();
			ReadSemAssAtt(TheToken); 
		} 
		else { 
			TheToken.input();
			ReadSemAssVal(TheToken); 
		}
	}
}

void Model::ReadSemAssAtt(IToken &TheToken)
{
	CondLink *link;
	char *cond;
	if( TheToken.isEqualTo("["))  
		cond=ReadCondition(TheToken);
	else { cond=new char[3]; strcpy(cond,"[]"); }
	link=asemModel.getSemAssCondList().addto(cond);
	// has read ":"

	TheToken.input();	// read "{"
	TheToken.input();	// read AttributeSet
	ReadAtt_Set(TheToken,SortedAttList,link->getCondAttList(),SEMANTIC,LEXICAL,EXTERNAL,DYNAMIC,MTRELATIVE);
			  
	// has read "}"

	TheToken.input();	// read "EndModel"
}

void Model::ReadSemAssVal(IToken &TheToken)
{
	TheToken.input(); 
	Attribute *attr=SortedAttList.find(TheToken.getBuffer());
	if( attr->getNameAlias()==ALIAS ) attr=attr->getRefer();
	TheToken.input();	// read ")"
	TheToken.input();	// read ":"
	TheToken.input();
	ReadFestVal(TheToken,attr);
}

void Model::ReadFestVal(IToken &TheToken,Attribute *attr)
{
	if( TheToken.isEqualTo("RelMode") || TheToken.isEqualTo("Static") || 
		TheToken.isEqualTo("Dynamic") ) 
			ReadValMode(TheToken,attr);
	else if( TheToken.isEqualTo("Default") ) {
		ReadDefFest(TheToken,attr);
		if( TheToken.isEqualTo("RelMode") || TheToken.isEqualTo("Static") || TheToken.isEqualTo("Dynamic") ) 
			ReadValMode(TheToken,attr);
	}
}

void Model::ReadSynModel(IToken &TheToken)
{
	TheToken.input();	// read "SynModel"
	TheToken.input();	// read LangCode
	int lang;
	lang=ReadLangCode(TheToken);
	ReadSynModelBody(TheToken,lang);
}

void Model::ReadSynModelBody(IToken &TheToken,int lang)
{
	ReadLexCat(TheToken,lang);
	ReadLexFeat(TheToken,lang);
	ReadPhrCat(TheToken,lang);
	ReadPhrFeat(TheToken,lang);
	ReadSynAss(TheToken,lang);
	ReadSpecialField(TheToken,lang);

	// has read "EndModel"
}

void Model::ReadSpecialField(IToken &TheToken,int lang)
{
	ReadDigitField(TheToken,lang);
	ReadPunctField(TheToken,lang);
	ReadOtherField(TheToken,lang);
}

int	Model::ReadLangCode(IToken &TheToken)
{
	// has read "("

	TheToken.input();	// read LANGSTRING[LANGNUM]
	int i;
	for(i=0;i<LANGNUM;i++) if(TheToken.isEqualTo(LANGSTRING[i])) break;
	asynModel[i].setHas(YES);
	TheToken.input();	// read ")"
	TheToken.input();	// read "LexCat"
	return i;
}

void Model::ReadLexCat(IToken &TheToken,int lang)
{
	// has read "LexCat"

	TheToken.input();		// read ":"
	TheToken.input();		// read LexCatAttribute
	ReadAttribute(TheToken,SortedAttList,asynModel[lang].getLexCat(),NO,lang,LEXICAL,INTERNAL,STATIC,MTRELATIVE,SYMBOL,LIMITED);
	// has read ":"

   	TheToken.input();		// read "{"
	TheToken.input();		// read  LexCat's ValList
	ReadHieSym_Set(TheToken,asynModel[lang].getLexCat().getAttLinkHead()->getAttr()->getValList());
	// has read "}"

	TheToken.input();
}

void Model::ReadLexFeat(IToken &TheToken,int lang)
{
	while(TheToken.isEqualTo("LexAtt") || TheToken.isEqualTo("LexVal"))
	{
		if(TheToken.isEqualTo("LexAtt")) {
			TheToken.input();
			ReadLexAtt(TheToken,lang);
		}
		else {
			TheToken.input();
			ReadLexVal(TheToken);
		}
	}
}

void Model::ReadLexAtt(IToken &TheToken,int lang)
{
	char buff[TOKENLENGTH];
	if(TheToken.isEqualTo("("))
	{
		TheToken.input();		// read "BaseForm" or "VaryForm"
		if(TheToken.isEqualTo("BaseForm")){
			TheToken.input();	// read ")"
			TheToken.input();	// read ":"
			TheToken.input();	// read LexAttribute
			strcpy(buff,TheToken.getBuffer());
			ReadAttribute(TheToken,SortedAttList,asynModel[lang].getBaseForm(),YES,lang,LEXICAL,INTERNAL,STATIC,MTRELATIVE,SYMBOL,UNLIMITED);
			Attribute *attr=SortedAttList.find(buff);
			// attr->setDefault("NONE");
		 }
		else if(TheToken.isEqualTo("VaryForm")){
			TheToken.input();	// read "("
			TheToken.input();	// read ":"
			TheToken.input();	// read LexAttribute
			strcpy(buff,TheToken.getBuffer());
			ReadAttribute(TheToken,SortedAttList,asynModel[lang].getVaryForm(),YES,lang,LEXICAL,INTERNAL,STATIC,MTRELATIVE,SYMBOL,UNLIMITED);
			Attribute *attr=SortedAttList.find(buff);
			// attr->setDefault("NONE");
		}
	}
	else
	{
		char *cond;
		if( TheToken.isEqualTo("[")) 
			cond=ReadCondition(TheToken);
		else { cond=new char[3]; strcpy(cond,"[]"); }

		CondLink *link=asynModel[lang].getLexStaDynCondList().addto(cond);
		// has read ":"
		TheToken.input();	// read "{"
		TheToken.input();	// read AttributeSet
		ReadAtt_Set(TheToken,SortedAttList,link->getCondAttList(),lang,LEXICAL,INTERNAL,STATIC,MTRELATIVE);
			// has read "}"
	
		TheToken.input();
	}
}

void Model::ReadLexVal(IToken &TheToken)
{
	// has read "("

	TheToken.input();
	Attribute *p;
	p=SortedAttList.find(TheToken.getBuffer());
	Attribute *pp;
	if(p->getNameAlias()==ALIAS) pp=p->getRefer();
	else pp=p;

	TheToken.input();		// read ")"
	TheToken.input();		// read ":"
	TheToken.input();		// read LexAtt's ValList
	ReadAtomVal(TheToken,pp);
}

void Model::ReadPhrCat(IToken &TheToken,int lang)
{
	// has read "PhrCat"
	
	TheToken.input();	// read ":"
	TheToken.input();	// read PhrCatAttribute
	ReadAttribute(TheToken,SortedAttList,asynModel[lang].getPhrCat(),NO,lang,PHRASAL,INTERNAL,STATIC,IRRELATIVE,SYMBOL,LIMITED);
	// has read ":"

	TheToken.input();	// read "{"
	TheToken.input();	// read PhrCat's ValList
	ReadHieSym_Set(TheToken,asynModel[lang].getPhrCat().getAttLinkHead()->getAttr()->getValList());
	// has read "}"

	TheToken.input();
}

void Model::ReadPhrFeat(IToken &TheToken,int lang)
{
	while(TheToken.isEqualTo("PhrAtt") || TheToken.isEqualTo("PhrVal"))
	{
		if(TheToken.isEqualTo("PhrAtt")){
			TheToken.input();
			ReadPhrAtt(TheToken,lang);
		}
		else {
			TheToken.input();
			ReadPhrVal(TheToken);
		}
	}
}

void Model::ReadPhrAtt(IToken &TheToken,int lang)
{
	char *cond;
	if( TheToken.isEqualTo("[")) 
		cond=ReadCondition(TheToken);
	else { cond=new char[3]; strcpy(cond,"[]"); }
	CondLink *link=asynModel[lang].getPhrCondList().addto(cond);
	// has read ":"
	
	TheToken.input();	// read "{"
	TheToken.input();	// read AttributeSet
	ReadAtt_Set(TheToken,SortedAttList,link->getCondAttList(),lang,PHRASAL,INTERNAL,STATIC,IRRELATIVE);
	// has read "}"
		
	TheToken.input();
}

void Model::ReadPhrVal(IToken &TheToken)
{
	// has read "("
	TheToken.input();
	Attribute *pp;
	pp=SortedAttList.find(TheToken.getBuffer());
	if(pp->getNameAlias()==ALIAS) pp=pp->getRefer();

	TheToken.input();		// read ")"
	TheToken.input();		// read ":"
	TheToken.input();		// read PhrAtt's ValList
	ReadAtomVal(TheToken,pp);
}

void Model::ReadSynAss(IToken &TheToken,int lang)
{
	while( TheToken.isEqualTo("SynAssAtt") || TheToken.isEqualTo("SynAssVal") )
	{
		if( TheToken.isEqualTo("SynAssAtt") ) 
		{ 
			TheToken.input();
			ReadSynAssAtt(TheToken,lang); 
		}
		else { 
			TheToken.input(); 
			ReadSynAssVal(TheToken); 
		}
	}
}

void Model::ReadSynAssAtt(IToken &TheToken,int lang)
{
	char *cond;
	if( TheToken.isEqualTo("[")) 
		cond=ReadCondition(TheToken);
	else { cond=new char[3]; strcpy(cond,"[]"); }

	CondLink *link=asynModel[lang].getSynAssCondList().addto(cond);
	// has read ":"

	TheToken.input();	// read "{"
	TheToken.input();	// read AttributeSet
	ReadAtt_Set(TheToken,SortedAttList,link->getCondAttList(),lang,PHRASAL,EXTERNAL,STATIC,IRRELATIVE);
			 
	// has read "}"

	TheToken.input();	
}

void Model::ReadSynAssVal(IToken &TheToken)
{
	TheToken.input(); 
	Attribute *attr=SortedAttList.find(TheToken.getBuffer());
	if( attr->getNameAlias()==ALIAS ) attr=attr->getRefer();
	TheToken.input();	// read ")"
	TheToken.input();	// read ":"
	TheToken.input();
	ReadFestVal(TheToken,attr);
}

void Model::ReadDigitField(IToken &TheToken,int lang)
{
	TheToken.input();		// read "("
	TheToken.input();		// read "Digit"
	TheToken.input();		// read ")"
	TheToken.input();		// read ":"
	TheToken.input();       // read ident

	const char * p=TheToken.getInPntr();

	TheToken.input();
	if( TheToken.isEqualTo("[") || TheToken.isEqualTo("{") ) 
		ReadFest(TheToken);

	const char *q=TheToken.getInPntr();

	int size=q-p;
	char * field=new char[size+1];
	strncpy(field,p,size);
	field[size]=0;
	asynModel[lang].setDgtCat(field);
}

void Model::ReadPunctField(IToken &TheToken,int lang)
{
	TheToken.input();		// read "("
	TheToken.input();		// read "Punct"
	TheToken.input();		// read ")"
	TheToken.input();		// read ":"
	TheToken.input();       // read ident

	const char * p=TheToken.getInPntr();

	TheToken.input();

	if( TheToken.isEqualTo("[") || TheToken.isEqualTo("{") ) 
		ReadFest(TheToken);

	const char *q=TheToken.getInPntr();

	int size=q-p;
	char * field=new char[size+1];
	strncpy(field,p,size);
	field[size]=0;
	asynModel[lang].setPctCat(field);
}

void Model::ReadOtherField(IToken &TheToken,int lang)
{
	TheToken.input();     // read "("
	TheToken.input();	  // read "Other"
	TheToken.input();		// read ")"
	TheToken.input();		// read ":"
	TheToken.input();       // read ident

	const char * p=TheToken.getInPntr();

	TheToken.input();

	if( TheToken.isEqualTo("[") || TheToken.isEqualTo("{") ) 
		ReadFest(TheToken);

	const char *q=TheToken.getInPntr();

	int size=q-p;
	char * field=new char[size+1];
	strncpy(field,p,size);
	field[size]=0;
	asynModel[lang].setOthCat(field);
}
