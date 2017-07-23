#include "mkbase.h"

/*
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <fstream.h>
#include <iostream.h>
#include <fcntl.h>
*/

RuleLink *cRuleList::find(char *title)
{
	RuleLink *rulelink=RuleLinkHead;
	char *rule;
	while (rulelink) {
		rule=(char *)rulelink->getRule();
		IToken token(rule);
		token.input();
		token.input();
		token.input();
		if (!strcmp(title,token.getBuffer())) return rulelink;
		else rulelink=rulelink->getNext();
	}
	return NULL;
}

void cRuleList::del(char *rule)
{
	RuleLink *link=RuleLinkHead;
	RuleLink *prelink=RuleLinkHead;
	while (link && strcmp((char *)link->getRule(),rule)) {
		prelink=link;	
		link=link->getNext();
	}
	if (link==RuleLinkHead) {
		if (RuleLinkHead!=RuleLinkTail) RuleLinkHead=link->getNext();
		else RuleLinkTail=NULL;
	}
	else if (link==RuleLinkTail) prelink->setNext(NULL); 
	else prelink->setNext(link->getNext());
	delete link;
}

void cRuleList::addhead(char *rule)
{
	RuleLink *rulelink=new RuleLink(rule);
	if (!RuleLinkHead) RuleLinkHead=RuleLinkTail=rulelink;
	else {
		rulelink->setNext(RuleLinkHead);
		RuleLinkHead=rulelink;
	}
}

void cRuleList::addtail(char *rule)
{
	RuleLink *rulelink=new RuleLink(rule);
	if (!RuleLinkHead) RuleLinkHead=RuleLinkTail=rulelink;
	else {
		RuleLinkTail->setNext(rulelink);
		RuleLinkTail=rulelink;
	}
}

void cRuleList::OutPut(OToken &token) 
{
	RuleLink *rulelink=RuleLinkHead;
	while (rulelink) {
		token.output(rulelink->getRule());
		rulelink=rulelink->getNext();
	}
}

void cRuleList::free()
{
	RuleLink *link;
	link=RuleLinkHead;
	while( link )
	{
		RuleLink *next=link->getNext();
		delete link;
		link=next;
	}
	RuleLinkHead=RuleLinkTail=NULL;
}

void cRuleList::freeRules()
{
	RuleLink *link;
	link=RuleLinkHead;
	while( link )
	{
		RuleLink *next=link->getNext();
		delete link->Rule;
		delete link;
		link=next;
	}
	RuleLinkHead=RuleLinkTail=NULL;
}

void PTIdxBuffList::free()
{
	PTIdxBuffUnit * unit=FstUnit, * next;
	while (unit)
	{
		next=unit->getNext();
		delete unit;
		unit=next;
	}
	FstUnit=NULL;
}

PTIdxBuffUnit * PTIdxBuffList::srhUnit(const char *pattn)
{
	PTIdxBuffUnit * unit=FstUnit;
	while (unit)
	{
		int cmp=strcmp(unit->getPattn(),pattn);
		if (cmp>0) return NULL;
		else if (cmp==0) return unit;
		else unit=unit->getNext();
	}
	return NULL;
}

PTIdxBuffUnit * PTIdxBuffList::addUnitHead(const char *pattn,const char * rule)
{
	PTIdxBuffUnit * unit=FstUnit, * prev=NULL, *theunit;
	int cmp;
	while (unit)
	{
		cmp=strcmp(unit->getPattn(),pattn);
		if (cmp<0) { prev=unit; unit=unit->getNext(); }
		else break;
	}
	if ( unit && !cmp ) theunit=unit;
	else
	{
		theunit=new PTIdxBuffUnit;
		theunit->setPattn(pattn);
		theunit->Next=unit;
		if (prev) prev->Next=theunit;
		else FstUnit=theunit;
	}
	theunit->addRuleHead(rule);
	return theunit;
}

PTIdxBuffUnit * PTIdxBuffList::addUnitTail(const char *pattn,const char * rule)
{
	PTIdxBuffUnit * unit=FstUnit, * prev=NULL, *theunit;
	int cmp;
	while (unit)
	{
		cmp=strcmp(unit->getPattn(),pattn);
		if (cmp<0) { prev=unit; unit=unit->getNext(); }
		else break;
	}
	if ( unit && !cmp ) theunit=unit;
	else
	{
		theunit=new PTIdxBuffUnit;
		theunit->setPattn(pattn);
		theunit->Next=unit;
		if (prev) prev->Next=theunit;
		else FstUnit=theunit;
	}
	theunit->addRuleTail(rule);
	return theunit;
}

/****************************************************************
*   In parameters:    none                                      *
*   Return Value:     none                                      *
*   Descriptions:     a set of function concerning with config  *
*                     information in config file.including:     *
*                     read config from .CFG                     *
*                     write config to .CFG                      *
*                     test config                               *
*                     initialize config.                        *
*****************************************************************/

void MKbase::testSituation()
{
  //Kbase::testSituation();
	rdLock();
	// open the CfgFile, if error, error(CFGOPEN)
	char cfgfile[FILENAMELENGTH];
	getFullName(cfgfile,".CFG");
#ifdef UNIX
	CfgFile.open(cfgfile,ios::in);
#endif
#ifdef WIN32
	CfgFile.open(cfgfile,ios::in);
#endif
	try { rdCfg(); }
	catch (CErrorException *) {};
	CfgFile.close();
	unLock();
	if(mkModel->getBuilt()!=YES)
		error("MKbase::testSituation(): GlobalModel is not built.");
	//if(stricmp(ModelPath,mkModel->getPath()))
	//	error("MKbase::testSituation(): ModelPath error.",getPath(),getName());
	//if(stricmp(ModelName,mkModel->getName()))
	//	error("MKbase::testSituation(): ModelName error.",getPath(),getName());
#ifdef KBMS
	//if(labs(ModelDate-mkModel->getDate())>10)
	//	message("Warning:\nMKbase::testSituation(): ModelDate error.",getPath(),getName());
#endif
}

void MKbase::rdCfg()
{
	Kbase::rdCfg();
	rdModelPath();
	rdModelName();
	rdModelDate();
}

void MKbase::testCfg()
{
  /*
	Kbase::testCfg();
	if(strcmp(getType(),hasType())!=0) error("MKbase::testCfg(): TYPE",getName());
	if(mkModel->getBuilt()!=YES) error("MKbase::testCfg(): MODEL",getName());
 	if(stricmp(ModelPath,mkModel->getPath())) 
		error("MKbase::testCfg(): Model path mismatch",getName());
 	if(stricmp(ModelName,mkModel->getName())) 
		error("MKbase::testCfg(): Model name mismatch",getName());
#ifdef KBMS
    if(labs(ModelDate-mkModel->getDate())>10) 
		error("MKbase::testCfg(): Model date mismatch",getName());
#endif
  */
}

void MKbase::wtCfg()
{
	Kbase::wtCfg();
	wtModelPath();
	wtModelName();
	wtModelDate();
}


void MKbase::initCfg()
{
	Kbase::initCfg();
	if(mkModel->getBuilt()!=YES) error("MKbase::initCfg(): MODELOPEN",getName());
	strcpy(ModelPath,mkModel->getPath());
	strcpy(ModelName,mkModel->getName());
	ModelDate=mkModel->getDate();
}

/************************************************************
*   In parameters:  none                                    *
*   Return Value:   none                                    *
*   Descriptions:   read config from .CFG                   *
*************************************************************/

void MKbase::rdModelPath()
{
	if(!CfgFile) error("Mkbase::rdModelPath(): CFGOPEN",getName());
	CfgFile.seekg(0,ios::beg);     
	char tmp[256];
	char *p;
	while(CfgFile.peek()!=EOF) { 
	      CfgFile.getline(tmp,255);
		  if(strstr(tmp,"ModelPath")) {
			   p=strstr(tmp,"=");
			   p++;
			   strcpy(ModelPath,p);
			   break;
		  }
	}
}

void MKbase::rdModelName()
{
	if(!CfgFile) error("Mkbase::rdModelName(): CFGOPEN",getName());
	CfgFile.seekg(0,ios::beg);     
	char tmp[256];
	char *p;
	while(CfgFile.peek()!=EOF) { 
	      CfgFile.getline(tmp,255);
		  if(strstr(tmp,"ModelName")) {
			   p=strstr(tmp,"=");
			   p++;
			   strcpy(ModelName,p);
			   break;
		  }
	}
}

void MKbase::rdModelDate()
{
	if(!CfgFile) error("MKbase::rdModelDate(): CFGOPEN",getName());
	CfgFile.seekg(0,ios::beg);    
	static char Date[]="ModelDate";
	char *date;
	char ch;
	BOOL find=FALSE;
	while(!find)		 //寻找"ModelDate"
	{
		CfgFile.get(ch);
		if (ch==EOF) break;
		for(date=Date;*date;date++)
		{	
			if(ch!=*date) break;
			CfgFile.get(ch);
		}
		if(!(*date)) find=TRUE;
	}
	if (find==FALSE) error("MKbase::rdModelDate(): no modeldate found in CfgFile.",getName());

	struct tm mdate;
	CfgFile >>mdate.tm_year>>ch
			>>mdate.tm_mon>>ch
			>>mdate.tm_mday
			>>mdate.tm_hour>>ch
			>>mdate.tm_min>>ch
			>>mdate.tm_sec
			>>mdate.tm_isdst;
	mdate.tm_mon--;
#ifdef UNIX
	ModelDate=timelocal(&mdate);
#endif
#ifdef WIN32
	ModelDate=mktime(&mdate);
#endif
}


/************************************************************
*   In parameters: none                                     *
*   Return Value:  none                                     *
*   Descriptions:  write current config to .CFG file        *
*************************************************************/

//write to CfgFile
void MKbase::wtModelPath()
{
	if(!CfgFile) error("MKbase::wtModelPath(): CFGOPEN",getName());
	CfgFile<<"ModelPath="<<ModelPath<<endl;

}

void MKbase::wtModelName()
{
	if(!CfgFile) error("MKbase::wtModleName(): CFGOPEN",getName());
	CfgFile<<"ModelName="<<ModelName<<endl;
}

void MKbase::wtModelDate()
{
	if(!CfgFile) error("MKbase::wtModelDate(): CFGOPEN",getName());
	struct tm *Date;
	Date=localtime(&ModelDate);
	CfgFile <<"ModelDate="
			<<Date->tm_year<<'-'
			<<(Date->tm_mon+1)<<'-'
			<<Date->tm_mday<<' '
			<<Date->tm_hour<<':'
			<<Date->tm_min<<':'
			<<Date->tm_sec<<' '
			<<Date->tm_isdst<<endl;
}



/***************************************************************
*   Descriptions: 	  Load Atom, Feature Structure,Forest and  *
*					  LeftTree and RightTree  form InPntr to   *
*					  OutPntr								   *
****************************************************************/

Model * MKbase::mkModel;

#ifdef KBMS
void MKbase::LoadFest(IOToken &TheToken,const char *err)
{
	Kbase::LoadFest(TheToken,mkModel->getSortedAttList(),err);
}

void MKbase::LoadLeftForest(IOToken &TheToken,int lang,const char *err)
{
	if(TheToken.getBuffer()[0]!='!' && TheToken.getType()!=IDENT)
		 error("Mkbase::LoadLeftForest(): Should be \"!\" or IDENT",TheToken,err);

	while(TheToken.getBuffer()[0]=='!' || TheToken.getType()==IDENT)
	{
		if(TheToken.getBuffer()[0]=='!') {
			LoadHeadTag(TheToken);
			LoadLeftTree(TheToken,lang,err);
		}
		else LoadLeftTree(TheToken,lang,err);
		TheToken.output(' ');
	}
}

void MKbase::LoadRightForest(IOToken &TheToken,int lang,int prelang,const char *err)
{
	if(TheToken.getBuffer()[0]!='!' && TheToken.getType()!=IDENT )
		 error("MKbase::LoadRightForest(): Should be \"!\" or IDENT",TheToken,err);

	while(TheToken.getBuffer()[0]=='!' || TheToken.getType()==IDENT )
	{
		if(TheToken.getBuffer()[0]=='!') {
			LoadHeadTag(TheToken);
			LoadRightTree(TheToken,lang,prelang,err);
		}
		else LoadRightTree(TheToken,lang,prelang,err);
		TheToken.output(' ');
	}
}

void MKbase::LoadRightTree(IOToken &TheToken,int lang,int prelang,const char *err)
{
	LoadLabelItem(TheToken,lang,err);
	if(TheToken.isEqualTo("<")) {
		TheToken.output();
		TheToken.input();
		LoadWordItem_OR(TheToken,err);
		if(!TheToken.isEqualTo(">")) 
			error("MKbase::LoadRightTree(): Need \">\"",TheToken,err );
		TheToken.output();
		TheToken.input();
	}
	else if(TheToken.isEqualTo("(")) {
		TheToken.output();
		TheToken.input();
		LoadRightSubForest(TheToken,lang,prelang,err);
		if(!TheToken.isEqualTo(")")) 
			error("MKbase::LoadRightTree(): Need \")\"",TheToken,err );
		TheToken.output();
		TheToken.input();
	}
}

// Load left tree
void MKbase::LoadLeftTree(IOToken &TheToken,int lang,const char *err)
{
	LoadLabelItem(TheToken,lang,err);
	if(TheToken.isEqualTo("<")) {
		TheToken.output();
		TheToken.input();
		LoadWordItem_OR(TheToken,err);
		if(!TheToken.isEqualTo(">"))
			error("MKbase::LoadLeftTree(): Should be \">\"",TheToken,err);
		TheToken.output();
		TheToken.input();
	}
	else if(TheToken.isEqualTo("(")) {
		TheToken.output();
		TheToken.input();
		LoadLeftSubForest(TheToken,lang,err);
		if(!TheToken.isEqualTo(")")) 
			error("MKbase::LoadLeftTree(): Should be \")\"",TheToken,err);
		TheToken.output();
		TheToken.input();
	}
}

void MKbase::LoadWordItem_OR(IOToken &TheToken,const char *err)
{
	LoadWordItem(TheToken,err);
	while(TheToken.isEqualTo("|")) {
		TheToken.output();
		TheToken.input();
		LoadWordItem(TheToken,err);
	}
}

void MKbase::LoadWordItem(IOToken &TheToken,const char *err)
{
	if(TheToken.isEqualTo("~")) {
		TheToken.output();
		TheToken.input();
 	}
	else LoadWord(TheToken,err);
}

void MKbase::LoadWord(IOToken &TheToken,const char *err)
{
	if(TheToken.getType()!=IDENT) 
		error("MKbase::LoadWord(): Not IDENT",TheToken,err);

	TheToken.output();
	TheToken.input();
}

void MKbase::LoadHeadTag(IOToken &TheToken)
{
	if(TheToken.isEqualTo("!") ){ 
		TheToken.output();
		TheToken.input();
	}
}

void MKbase::LoadMoveTag(IOToken &TheToken)
{
	if( TheToken.isEqualTo("#") ){
		TheToken.output();
		TheToken.input();
	}
}

void MKbase::LoadCopyTag(IOToken &TheToken)
{
	if( TheToken.isEqualTo("*") ) {
		TheToken.output();
		TheToken.input();
	}
}

void MKbase::LoadRemvTag(IOToken &TheToken)
{
 	TheToken.output();
	TheToken.input();
}
		
void MKbase::LoadLeftSubForest(IOToken &TheToken,int lang,const char *err)
{
	if( TheToken.isEqualTo(")") ) return ;

	if( TheToken.getType()!=IDENT &&TheToken.getBuffer()[0]!='!') 
		error("MKbase::LoadLeftSubForest(): Should be \"!\" or IDENT",TheToken,err); 

	TheToken.output(' ');
	while( TheToken.getBuffer()[0]=='!' || TheToken.getType()==IDENT) 
	{ 
		if(TheToken.getBuffer()[0]=='!'){
			LoadHeadTag(TheToken);
			LoadLeftSubTree(TheToken,lang,err);
		}
		else LoadLeftSubTree(TheToken,lang,err);
		TheToken.output(' ');
	}
}


void MKbase::LoadRightSubForest(IOToken &TheToken,int lang, int prelang,const char *err)
{	
	if( TheToken.isEqualTo(")") ) return; 

	if(TheToken.getType()!=IDENT && TheToken.getBuffer()[0]!='!' && 
		!TheToken.isEqualTo("*") && !TheToken.isEqualTo("#") && !TheToken.isEqualTo("&")) 
		error("MKbase::LoadRightSubForest(): Should be \"!\" or IDENT",TheToken,err ); 
	TheToken.output(' ');

	while( TheToken.getType()==IDENT || TheToken.getBuffer()[0]=='!' ||
		   TheToken.isEqualTo("#") || TheToken.isEqualTo("*") || TheToken.isEqualTo("&") ) 
	{
		if(TheToken.getBuffer()[0]=='!'){
			LoadHeadTag(TheToken);
			LoadRightSubTree(TheToken,lang,prelang,err);
		}
		else if( TheToken.isEqualTo("#") ) {
			LoadMoveTag(TheToken);
			LoadShiftLabelItem(TheToken,lang,err);
		}
		else if( TheToken.isEqualTo("*") ) {
			LoadCopyTag(TheToken);
			LoadShiftLabelItem(TheToken,lang,err);
		}
		else if( TheToken.isEqualTo("&") ) {
			LoadRemvTag(TheToken);
			LoadShiftLabelItem(TheToken,lang,err);
		}
		else LoadRightSubTree(TheToken,lang,prelang,err);

		TheToken.output(' ');
	}
}

void MKbase::LoadLeftSubTree(IOToken &TheToken,int lang,const char *err)
{
	LoadLeftLabelItem(TheToken,lang,err);
	if(TheToken.isEqualTo("<")) {
		TheToken.output();
		TheToken.input();
		LoadWordItem_OR(TheToken,err);
		if(!TheToken.isEqualTo(">")) 
			error("MKase::LoadLeftSubTree(): Need \">\"",TheToken,err);
		TheToken.output();
		TheToken.input();
	}
	else if(TheToken.isEqualTo("(")) {
		TheToken.output();
		TheToken.input();
		LoadLeftSubForest(TheToken,lang,err);
		if(!TheToken.isEqualTo(")")) 
			error("MKbase::LoadLeftSubTree(): Need \")\"",TheToken,err );
		TheToken.output();
		TheToken.input();
	}
}

void MKbase::LoadRightSubTree(IOToken &TheToken,int lang, int prelang,const char *err)
{
	LoadRightLabelItem(TheToken,lang,prelang,err);
	if(TheToken.isEqualTo("<")) {
		TheToken.output();
		TheToken.input();
		LoadWordItem_OR(TheToken,err);
		if(!TheToken.isEqualTo(">"))
			error("MKbase::LoadRightSubTree(): Should be \">\"",TheToken,err);
		TheToken.output();
		TheToken.input();
	}
	else if(TheToken.isEqualTo("(")) {
		TheToken.output();
		TheToken.input();
		LoadRightSubForest(TheToken,lang,prelang,err);
		if(!TheToken.isEqualTo(")")) 
			error("MKbase::LoadRightSubTree(): Should be \")\"",TheToken,err );
		TheToken.output();
		TheToken.input();
	}
}

void MKbase::LoadShiftLabelItem(IOToken &TheToken,int lang,const char *err)
{
	LoadLabelItem(TheToken,lang,err);
	LoadLabelVar(TheToken,lang,err);
}

void MKbase::LoadLeftLabelItem(IOToken &TheToken,int lang,const char *err)
{
		LoadLabelItem(TheToken,lang,err);
}

void MKbase::LoadRightLabelItem(IOToken &TheToken,int lang,int prelang,const char *err)
{
		LoadLabelItem(TheToken,lang,err);
		if(TheToken.getBuffer()[0]=='/') 
			LoadTransVar(TheToken,prelang,err);
}

void MKbase::LoadLabelItem_OR(IOToken &TheToken,int lang,const char *err)
{
	LoadLabelItem(TheToken,lang,err);
	while(TheToken.isEqualTo("|")) {
		TheToken.output();
		TheToken.input();
		LoadLabelItem(TheToken,lang,err);
	}
}

void MKbase::LoadTransVar(IOToken &TheToken,int lang,const char *err)
{
	LoadTransVarTag(TheToken,err);
	LoadLabelItem(TheToken,lang,err);
}

void MKbase::LoadTransVarTag(IOToken &TheToken,const char *err)
{
	if(!TheToken.isEqualTo("/") && !TheToken.isEqualTo("//") && 
	   !TheToken.isEqualTo("///") && !TheToken.isEqualTo("////"))
		error("MKbase::LoadTransVarTag(): Should be \"/\" or \"//\" or \"///\" or \"////\"",TheToken,err);

	if(TheToken.isEqualTo("/") || TheToken.isEqualTo("//") || 
		TheToken.isEqualTo("///") || TheToken.isEqualTo("////"))
	{
		TheToken.output();
		TheToken.input();
	}
}
	
void MKbase::LoadEquation(IOToken &TheToken,int lang,const char *err)
{
	BOOL bExt;
	Attribute *attr;

	//  try to Load ExtVar,if last attrbute is ExtAtt,then return TRUE.
	// if last attribute is IntVar,return FALSE,and return the pointer
	// to the attribute for Loading atom.

	bExt=LoadExtIntVar(TheToken,lang,attr,err);
	if(!TheToken.isEqualTo("=") && !TheToken.isEqualTo("!=") && !TheToken.isEqualTo("==") ) 
		error("MKbase::LoadEquation(): Should be \"=\" or \"!=\" or \"==\"",TheToken,err); 

	if( TheToken.isEqualTo("=")) {
		TheToken.output(); 
		TheToken.input();
		if( bExt ) {	// <ExtVar>=<Fest> or <ExtVar>=<ExtVar>
			if(!TheToken.isEqualTo("[") && !TheToken.isEqualTo("{") && !TheToken.isEqualTo("NONE") && !TheToken.isEqualTo("None") && !TheToken.isEqualTo("无") && !TheToken.isEqualTo("$") && !TheToken.isEqualTo("是") && !TheToken.isEqualTo("Yes") && !TheToken.isEqualTo("YES")  && !TheToken.isEqualTo("否") && !TheToken.isEqualTo("No") && !TheToken.isEqualTo("NO") && TheToken.getBuffer()[0]!='%')  
				error("MKbase::LoadEquation(): Invalid right of Equation ",TheToken,err );

			if( TheToken.isEqualTo("是") || TheToken.isEqualTo("Yes") || TheToken.isEqualTo("YES") || TheToken.isEqualTo("否") || TheToken.isEqualTo("No") || TheToken.isEqualTo("NO") ) {
				TheToken.output();
				TheToken.input();
			}
			else if( TheToken.isEqualTo("[") || TheToken.isEqualTo("{") || 
					 TheToken.isEqualTo("NONE") || TheToken.isEqualTo("None") 
					 || TheToken.isEqualTo("无") ) 
				LoadFest(TheToken,err);
			 else {
				 BOOL tExt=LoadExtIntVar(TheToken,lang,attr,err);
				 if( !tExt ) 
					 error("MKbase::LoadEquation():  the right of = can't be <IntVar>",attr->getString(),err);
			  }
		}
		else  {     // <IntVal>=<IntVal> or <IntVal>=<Atom>
			if( TheToken.isEqualTo("$") || TheToken.getBuffer()[0]=='%' ) {
				BOOL bInt;
				//LoadIntVal,bInt==FALSE 
				bInt=LoadExtIntVar(TheToken,lang,attr,err);	
				if( bInt )
					error("MKbase::LoadEquation():  the right of = can't be <ExtVar>",attr->getString(),err);
			}
			else 
		  		LoadAtom(TheToken,attr->getAttType(),attr->getLimit(),attr->getValList(),err);
		
		}
	}
	else if( TheToken.isEqualTo("==") ) {
		if( !bExt ) 
			error("MKbase::LoadEquation(): the left of Equation is ExtVar",attr->getString(),err);
		TheToken.output();
		TheToken.input();
		BOOL tExt=LoadExtIntVar(TheToken,lang,attr,err);
		if( !tExt ) 
			 error("MKbase::LoadEquation(): the right of Equation  can't be <IntVar>",attr->getString(),err);
		if( TheToken.isEqualTo("@") ) LoadAt(TheToken,err);
	}
	else  if( TheToken.isEqualTo("!=") ) /* TheToken="!=" */
	{
		if( !bExt ) 
			error("MKbase::LoadEquation(): the left of Equation is ExtVar",attr->getString());
		TheToken.output();
		TheToken.input();
		BOOL tExt=LoadExtIntVar(TheToken,lang,attr,err);
		if( !tExt ) 
			 error("MKbase::LoadEquation(): the right of Equation  can't be <IntVar>",attr->getString(),err);
		LoadAt(TheToken,err);
	}
}

void MKbase::LoadTest(IOToken &TheToken,int lang,const char *err)
{
	TheToken.output();
	TheToken.output(' ');
	TheToken.input();
	LoadBind(TheToken,lang,err);
	if( TheToken.isEqualTo("TRUE") || TheToken.isEqualTo("FALSE") )
	{	
		TheToken.output(' ');
		TheToken.output(); 
		TheToken.input();
	}
	else if( TheToken.isEqualTo("THEN") )
	{
		TheToken.output(' ');
		TheToken.output();
		TheToken.output(' ');
		TheToken.input();
		LoadBind(TheToken,lang,err);
		if( TheToken.isEqualTo("ELSE") )
		{
			TheToken.output(' ');
			TheToken.output();
			TheToken.output(' ');
			TheToken.input();
			LoadBind(TheToken,lang,err);
		}
		if( TheToken.isEqualTo("ENDIF") )
		{
			TheToken.output(' ');
			TheToken.output();
			TheToken.input();
		}
		else error("MKbase::LoadTest(): Invalid End of Test and need ENDIF",TheToken,err);
	}
	else if( TheToken.isEqualTo("ELSE") )
	{
		TheToken.output(' ');
		TheToken.output();
		TheToken.output(' ');
		TheToken.input();
		LoadBind(TheToken,lang,err);
		if( TheToken.isEqualTo("ENDIF") )
		{
			TheToken.output(' ');
			TheToken.output();
			TheToken.input();
		}
		else error("MKbase::LoadTest(): Invalid End of Test and need ENDIF",TheToken,err);
	}
	else error("MKbase::LoadEquation(): Invalid End of Test and need TRUE or FALSE",TheToken,err );
}
	
void MKbase::LoadBind(IOToken &TheToken,int lang,const char *err)
{
	if( TheToken.isEqualTo("IF") )  LoadTest(TheToken,lang,err);
	else LoadEquation(TheToken,lang,err);
	while(TheToken.isEqualTo(",")){
		TheToken.output();
		TheToken.input();
		if( TheToken.isEqualTo("IF") ) LoadTest(TheToken,lang,err);
		else LoadEquation(TheToken,lang,err);
	}
}

BOOL MKbase::LoadExtIntVar(IOToken &TheToken,int lang,Attribute *&attr,const char *err)
// when last attribute is INT return FALSE . if it is EXT,return TRUE
{
	BOOL bExt=TRUE;

	if(!TheToken.isEqualTo("$") && TheToken.getBuffer()[0]!='%') 
		error("MKbase::LoadExtIntVar(): Should be \"$\" or \"%\"",TheToken,err);
	if(TheToken.isEqualTo("$")) LoadRootVar(TheToken);
	else LoadLabelVar(TheToken,lang,err);

	while(TheToken.isEqualTo(".")) {
		TheToken.output();
		TheToken.input();
		if( TheToken.getType()!=IDENT ) 
			error("MKbase::LoadExtIntVar(): Should be IDENT",TheToken,err);
		if(TheToken.getType()==IDENT) {

			//IntAtt can not appear in the middle of the ExtVar
			if(!bExt) 
				error("MKbase::LoadExtIntVar():Should be ExtVar",TheToken,err);

			// check if the identifier is defined in SortedAttList
			// if yes, find out its type and vallist for searching its value.
			AttLink * link;
  	   		link=(mkModel->getSortedAttList()).getAttLinkHead();
			while(link && 
				  strcmp(TheToken.getBuffer(),link->getAttr()->getString())) 
					link=link->getNext();
			if(!link) 
				error("MKbase::LoadExtIntVar(): Undefined Attribute",TheToken,err);

			if(link->getAttr()->getIntExt()==INTERNAL){
				bExt=FALSE; 
				attr=link->getAttr();
			}
			else bExt=TRUE;

			// if it is alias, find its real name.
			if(link->getAttr()->getNameAlias()==ALIAS){
				TheToken.setBuffer(link->getAttr()->getRefer()->getString());
				attr=link->getAttr()->getRefer();
			}

			TheToken.output();
			TheToken.input();
		}
	}
	return bExt;
}

void MKbase::LoadAt(IOToken &TheToken,const char *err)
{
	Attribute *attr;
	while( TheToken.isEqualTo("@") )
	{
		TheToken.output(' ');TheToken.output(); TheToken.input();
		if( TheToken.getType()!=IDENT )
			error("MKbase::LoadAt(): should be IDENT",TheToken,err );
		attr=mkModel->getSortedAttList().find(TheToken.getBuffer());
		if( !attr ) error( "MKbase::LoadAt(): Undefined Attribute",TheToken,err );
		if(attr->getNameAlias()==ALIAS)
			TheToken.setBuffer(attr->getRefer()->getString());
		TheToken.output(); TheToken.input();
	}
}

void MKbase::LoadRootVar(IOToken &TheToken)
{
	if(TheToken.isEqualTo("$")) {
		TheToken.output();
		TheToken.input();
	}
}

void MKbase::LoadLabelVar(IOToken &TheToken,int lang,const char *err)
{
	LoadLabelVarTag(TheToken);
	LoadLabelItem(TheToken,lang,err);
}

/*void MKbase::LoadWildVar(IOToken &TheToken)
{
	if(TheToken.isEqualTo("?") || TheToken.isEqualTo("??")
		|| TheToken.isEqualTo("???") || TheToken.isEqualTo("????"))
	TheToken.output();
	TheToken.input();
}
*/

void MKbase::LoadLabelVarTag(IOToken &TheToken)
{
	if(TheToken.isEqualTo("%") || TheToken.isEqualTo("%%")
		|| TheToken.isEqualTo("%%%") || TheToken.isEqualTo("%%%%"))
	TheToken.output();
	TheToken.input();
}

void MKbase::LoadLabelItem(IOToken &TheToken,int lang,const char *err)
{
		if(TheToken.getType()!=IDENT) 
			error("MKbase::LoadLabelItem(): Should be IDENT",TheToken,err);

		AttLink * link;
		link=mkModel->getasynModel(lang).getLexCat().getAttLinkHead();
		ValLink * vlink;
		vlink=link->getAttr()->getValList().getValLinkHead();
		while(vlink && strcmp(TheToken.getBuffer(),vlink->getVal()->getString()))
			vlink=vlink->getNext();

		if(!vlink) {  // no in LexCat
			link=mkModel->getasynModel(lang).getPhrCat().getAttLinkHead();
			vlink=link->getAttr()->getValList().getValLinkHead();
			while(vlink && 
				  strcmp(TheToken.getBuffer(),vlink->getVal()->getString())) 
				vlink=vlink->getNext();
		  	if(!vlink)
				error("MKbase::LoadLabelItem(): Undefined Attribute  in LexCat and PhrCat",TheToken.getBuffer(),err); 
		}

		// if it is alias, find its real name
		if(vlink->getVal()->getNameAlias()==ALIAS)
			TheToken.setBuffer(vlink->getVal()->getRefer()->getString());
		TheToken.output();
		// keep one token read ahead
		TheToken.input();

}

int MKbase::LoadPrsRule(IOToken &TheToken,int lang,int prelang,TitleList &titlelist,const char *err)
{
	if (!err) err=TheToken.getInPntr();
    char *p;
    p=TheToken.getOutPntr();
	TheToken.output(' ');
	TheToken.input();
    if(!TheToken.isEqualTo("&&"))
		error("MKbase::LoadPrsRule(): Should be \"&&\"",TheToken.getBuffer(),err); 
    TheToken.output();
    TheToken.output(' ');
    TheToken.input();
    if(!TheToken.isEqualTo("{")) 
		error("MKbase::LoadPrsRule(): Should be \"{\"", TheToken.getBuffer(),err);
    TheToken.output();
    TheToken.input();
    LoadTitle(TheToken,titlelist,err);
    if(!TheToken.isEqualTo("}"))
		error("MKbase::LoadPrsRule(): Should be \"}\" ", TheToken.getBuffer(),err);
    TheToken.output();
    TheToken.output(' ');
    TheToken.input();
    LoadPrsStruct(TheToken,prelang,err);
	TheToken.output("\n\t");
	while( TheToken.isEqualTo("||") ) 
	{ 
		TheToken.output(); 
		TheToken.output(' ');
		TheToken.input();
		LoadTrnSwitch(TheToken,prelang,err);
	}
	if( !TheToken.isEqualTo("=>") ) 
		error("MKbase::LoadPrsRule(): Need \"=>\"",TheToken.getBuffer(),err);
    while(TheToken.isEqualTo("=>")) {
        TheToken.output();
        TheToken.input();
        LoadTrnStruct(TheToken,lang,prelang,err);
		TheToken.output("\n\t");
		while( TheToken.isEqualTo("||") ) 
		{
			TheToken.output();
			TheToken.output(' ');
			TheToken.input();
			LoadTrnSwitch(TheToken,prelang,err);
		}
    }
	TheToken.redrawout();
	if(!TheToken.isEqualTo("")&&!TheToken.isEqualTo("&&")&&!TheToken.isEqualTo("**"))
		error("MKbase::LoadPrsRule(): Incorrect end of prsrule ",TheToken.getBuffer(),err);
	TheToken.outputNewLine();
    int num;
    num=TheToken.getOutPntr()-p;
	TheToken.outputNull();
    return num;
}
 
void MKbase::LoadPrsStruct(IOToken &TheToken,int lang,const char *err)
{
    LoadPrsReductn(TheToken,lang,err);
    if(!TheToken.isEqualTo("->"))
		error("MKbase::LoadPrsStruct(): Shoul be \"->\"",TheToken.getBuffer(),err);
    TheToken.output();
    TheToken.input();
    LoadPrsForest(TheToken,lang,err);
    if(TheToken.isEqualTo("::")) {
        TheToken.output();
        TheToken.input();
        LoadPrsBind(TheToken,lang,err);
    }
}
 
void MKbase::LoadPrsReductn(IOToken &TheToken,int lang,const char *err)
{
    LoadLabelItem(TheToken,lang,err);
}

void MKbase::LoadPrsBind(IOToken &TheToken,int lang,const char *err)
{
    LoadBind(TheToken,lang,err);
}
 
void MKbase::LoadTrnSwitch(IOToken &TheToken,int lang,const char *err)
{
	LoadBind(TheToken,lang,err);
}

void MKbase::LoadPrsForest(IOToken &TheToken,int lang,const char *err)
{
    if(TheToken.isEqualTo("^")) {
        TheToken.output(' ');
        TheToken.output();
        TheToken.input();
        LoadLeftForest(TheToken,lang,err);
        if(TheToken.isEqualTo("^")) {
            TheToken.output();
            TheToken.input();
        }
    }
    else {
        LoadLeftForest(TheToken,lang,err);
        if(TheToken.isEqualTo("^")) {
            TheToken.output();
            TheToken.input();
        }
    }
}
 
void MKbase::LoadTrnStruct(IOToken &TheToken,int lang,int prelang,const char *err)
{
 
    LoadTrnTree(TheToken,lang,prelang,err);
    if(TheToken.isEqualTo("$") || TheToken.getBuffer()[0]=='?'|| 
		TheToken.getBuffer()[0]=='%' || TheToken.isEqualTo("IF") )
    {
        TheToken.output(' ');
        LoadTrnBind(TheToken,lang,err);
    }
}
 
void MKbase::LoadTrnTree(IOToken &TheToken,int lang,int prelang,const char *err)
{
    LoadRightTree(TheToken,lang,prelang,err);
}

void MKbase::LoadTrnBind(IOToken &TheToken,int lang,const char *err)
{
    LoadBind(TheToken,lang,err);
}

void MKbase::LoadTitle(IOToken &TheToken,TitleList &titlelist,const char *err)
{
    if(TheToken.getType()!=IDENT) 
 		error("MKbase::LoadTitle(): Should be IDENT",TheToken.getBuffer(),err);

	char title[20];
	strcpy(title,TheToken.getBuffer());
	title[strlen(title)]=0;
	titlelist.addto(title,err);

    TheToken.output();
    TheToken.input();
	if( TheToken.isEqualTo("/") ) 
	{ 
		TheToken.output(); 
		TheToken.input();
		LoadNote(TheToken,err);
	}
}

void MKbase::LoadNote(IOToken &TheToken,const char *err)
{
	if(TheToken.getType()!=IDENT) 
		error("MKbase::LoadNote():  Should be IDENT",TheToken.getBuffer(),err);
	TheToken.output();
	TheToken.input();
	while( TheToken.isEqualTo(",") || TheToken.getType()==IDENT )
	{
		if( TheToken.isEqualTo(",") )
		{
			TheToken.output();
			TheToken.input();
			if( TheToken.getType()!=IDENT)
				error("MKbase::LoadNote():  Should be IDENT",TheToken.getBuffer(),err);
			TheToken.output();
			TheToken.input();
		}			
		else { TheToken.output(' '); TheToken.output(); TheToken.input(); } 
	}

}			

#endif
