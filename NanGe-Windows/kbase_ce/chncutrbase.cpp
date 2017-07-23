
#include "chncutrbase.h"

#ifdef KBMS

BOOL ChnOvlRbase::rdTxtOvlRule()
{
	return rdTxtRecord("@@");
}

int ChnOvlRbase::LoadOvlRule(IOToken &TheToken,int srclang)
{
	int len=strlen(TheToken.getInPntr());
	char *cutrule=new char[len+1];
	strcpy(cutrule,TheToken.getInPntr());
	char *p;
	p=TheToken.getOutPntr();
	TheToken.input();
	if( !TheToken.isEqualTo("@@" ) )
		error("ChnOvlRbase::LoadOvlRule()\nWrong: Should be \"@@\"",TheToken,cutrule);
	TheToken.output();
	TheToken.output(' ');
	TheToken.input();
	LoadOvlLexVaryStruct(TheToken,cutrule,srclang);
	if( !TheToken.isEqualTo("<<") )
		error("ChnOvlRbase::LoadOvlRule()\nWrong: Should be \"||\"",TheToken,cutrule);
	TheToken.output(' ');
	TheToken.output();
	TheToken.output(' ');
	TheToken.input();
	LoadOvlLexBaseStruct(TheToken,cutrule,srclang);
	TheToken.output('\n');
	int num;
	num=TheToken.getOutPntr()-p;
	TheToken.outputNull();
	delete [] cutrule;
	return num;
}

void ChnOvlRbase::LoadOvlLexVaryStruct(IOToken &TheToken,char *cutrule,int srclang)
{
	LoadOvlLexVary(TheToken,cutrule);
	if( !TheToken.isEqualTo("--") )
		error("ChnOvlRbase::LoadOvlLexVaryStruct()\nWrong: Should be \"--\"",TheToken,cutrule);
	TheToken.output(' ');
	TheToken.output();
	TheToken.output(' ');
	TheToken.input();
	LoadOvlLexStruct(TheToken,cutrule,srclang);
}

void ChnOvlRbase::LoadOvlLexBaseStruct(IOToken &TheToken,char *cutrule,int srclang)
{
	LoadOvlLexBase(TheToken,cutrule);
	if( !TheToken.isEqualTo("--") )
		error("ChnOvlRbase::LoadOvlLexBaseStruct()\nWrong: Should be \"--\"",TheToken,cutrule);
	TheToken.output(' ');
	TheToken.output();
	TheToken.output(' ');
	TheToken.input();
	LoadOvlLexStruct(TheToken,cutrule,srclang);
}

void ChnOvlRbase::LoadOvlLexStruct(IOToken &TheToken,char *cutrule,int srclang)
{
	LoadLabelItem(TheToken,srclang,cutrule);
	if( TheToken.isEqualTo("[") || TheToken.isEqualTo("{") ) LoadFest(TheToken,cutrule);
}

void ChnOvlRbase::LoadOvlLexVary(IOToken &TheToken,char *cutrule)
{
	const char *in=TheToken.getInPntr();
	char *out=TheToken.getOutPntr();
	if( !isChnChar(in) && !isupper(*in) ) 
		error("ChnOvlRbase::LoadOvlLexVary()\nWrong: Should be ChnChar or A-Z",in,cutrule);
	while( isChnChar(in) || isupper(*in) ) setCharNext(out,in);
	TheToken.setInPntr(in); TheToken.input();
	TheToken.setOutPntr(out);
}

void ChnOvlRbase::LoadOvlLexBase(IOToken &TheToken,char *cutrule)
{
	const char *in=TheToken.getInPntr();
	char *out=TheToken.getOutPntr();
	if( !isChnChar(in) && !isupper(*in) ) 
		error("ChnOvlRbase::LoadOvlLexBase()\nWrong: Should be ChnChar or A-Z",in,cutrule);
	while( isChnChar(in) || isupper(*in) ) setCharNext(out,in);
	TheToken.setInPntr(in); TheToken.input();
	TheToken.setOutPntr(out);
}
#endif // KBMS

#ifdef KBMS
BOOL ChnSegRbase::rdTxtSegRule()
{
	return rdTxtRecord("@@");
}

int ChnSegRbase::LoadSegRule(IOToken &TheToken)
{
	int len=strlen(TheToken.getInPntr());
	char *cutrule=new char[len+1];
	strcpy(cutrule,TheToken.getInPntr());
	char *p;
	p=TheToken.getOutPntr();
	TheToken.input();
	if( !TheToken.isEqualTo("@@") ) 
		error("ChnSegRbase::LoadSegRule()\nWrong: Should be \"@@\"",TheToken,cutrule);
	TheToken.output();  //output @@
	TheToken.output(' ');
	TheToken.input();
	LoadSegStruct(TheToken,cutrule);
	LoadSegBody(TheToken,cutrule);
	if( TheToken.isEqualTo("OTHERWISE") ) 
		LoadSegEnd(TheToken,cutrule);
	TheToken.output('\n');
	int num;
	num=TheToken.getOutPntr()-p;
	TheToken.outputNull();
	delete [] cutrule;
	return num;
}

void ChnSegRbase::LoadSegEnd(IOToken &TheToken,char *cutrule)
{
	TheToken.output("\n   ");
	TheToken.output();
	TheToken.input();
	if( !TheToken.isEqualTo("SELECT") ) 
		error("ChnSegRbase::LoadSegRule()\nWrong: Should be \"SELECT\"",TheToken,cutrule);
	TheToken.output(' ');
	TheToken.output();
	TheToken.input();
	TheToken.output(' ');
	LoadSegResult(TheToken,cutrule);
}

void ChnSegRbase::LoadSegStruct(IOToken &TheToken,char *cutrule)
{
	LoadSegPattern(TheToken,cutrule);
	if(!TheToken.isEqualTo("("))
		error("ChnSegRbase::LoadSegStruct()\nWrong:should be \"(\"",TheToken,cutrule);
	TheToken.output();
	TheToken.input();
    LoadSegSelection(TheToken,cutrule);
    if(!TheToken.isEqualTo(")"))
		error("ChnSegRbase::LoadSegStruct()\nWrong:should be \")\"",TheToken,cutrule);
	TheToken.output();
	TheToken.input();

}


void ChnSegRbase::LoadSegPattern(IOToken &TheToken,char *cutrule)
{
   if (TheToken.getType()!=IDENT &&!TheToken.isEqualTo("*"))
	  error("ChnSegRbase::LoadSegPattern()\nWrong:should be IDENT or \"*\"",TheToken,cutrule); 
   TheToken.output();
   TheToken.input();
 }

void ChnSegRbase::LoadSegSelection(IOToken &TheToken,char *cutrule)
{  
   char * s1=LoadSegSelect(TheToken,cutrule);
   int len1=strlen(s1);
   if(!TheToken.isEqualTo(","))
		error("ChnSegRbase::LoadSegSelection()\nWrong:should be \",\"",TheToken,cutrule);
   TheToken.output();  //output ,
   TheToken.input();
   char *s2=LoadSegSelect(TheToken,cutrule);
   int len2=strlen(s2);
   char *s3=s1;
   char *s4=s2;
   while((*s1)||(*s2))
   { if(strcmp(s1,s2)<0 ||strcmp(s1,s2)>0)
           error("ChnSegRbase::LoadSegSelect()\nWrong: Should be segstring+segstring=SegString+Segstring",TheToken,cutrule);
     else 
	 {
	   s1++;
	   s2++;
	  }
   }
   delete [] s3;
   delete [] s4;
   // delete s1;
   //delete s2;
}

char * ChnSegRbase::LoadSegSelect(IOToken &TheToken,char *cutrule)
{
   char *s=new char[WORDLENGTH];
   strcpy(s,TheToken);
   LoadSegString(TheToken,cutrule);
   while (TheToken.isEqualTo("+"))
   {
     TheToken.output();  //output +
     TheToken.input();
     strcat(s,TheToken);
     LoadSegString(TheToken,cutrule);
   }
   return s;
}

void ChnSegRbase::LoadSegString(IOToken &TheToken,char *cutrule)
{
   if (TheToken.getType()!=IDENT)
	  error("ChnSegRbase::LoadSegString()\nWrong:should be IDENT",TheToken,cutrule); 
   TheToken.output();
   TheToken.input();
}

void ChnSegRbase::LoadSegBody(IOToken &TheToken,char *cutrule)
{   
	if(!TheToken.isEqualTo("CONDITION"))
		   error("ChnSegRbase::LoadSegBody()\nWrong:should be \"CONDITION\"",TheToken,cutrule);
    while(TheToken.isEqualTo("CONDITION"))
	   LoadSegBranch(TheToken,cutrule);
 }

void ChnSegRbase::LoadSegBranch(IOToken &TheToken,char *cutrule)
{  
   TheToken.output('\n');
   TheToken.output(' ');
   TheToken.output(' ');
   TheToken.output(' ');
   TheToken.output();  //output CONDITION
   TheToken.output(' ');
   TheToken.input();
   LoadSegCondition(TheToken,cutrule);
   if(!TheToken.isEqualTo("SELECT"))
		error("ChnSegRbase::LoadSegBranch()\nWrong:should be \"SELECT\"",TheToken,cutrule);
   TheToken.output(' ');
   TheToken.output();  //output SELECT
   TheToken.output(' ');
   TheToken.input();
   LoadSegResult(TheToken,cutrule);
}


void ChnSegRbase::LoadSegCondition(IOToken &TheToken,char *cutrule)
{  
	if(!TheToken.isEqualTo("%")&&!TheToken.isEqualTo("FIND")&&!TheToken.isEqualTo("NOFIND"))
		error("ChnSegRbase::LoadSegCondition()\nWrong:should be \"%\" or \"FIND\"or\"NOFIND\"",TheToken,cutrule);
	if (TheToken.isEqualTo("%"))
	     LoadSegEquation(TheToken,cutrule);
	else if (TheToken.isEqualTo("FIND")||TheToken.isEqualTo("NOFIND"))
		LoadCtxFunction(TheToken,cutrule);
	while(TheToken.isEqualTo(",")){
		TheToken.output();
		TheToken.input();
		if (TheToken.isEqualTo("%"))
	         LoadSegEquation(TheToken,cutrule);
	      else if (TheToken.isEqualTo("FIND")||TheToken.isEqualTo("NOFIND"))
			  LoadCtxFunction(TheToken,cutrule);
    }
}

void ChnSegRbase::LoadSegEquation(IOToken &TheToken,char *cutrule)
{
	Attribute *attr;
	if(!TheToken.isEqualTo("%"))
		error("ChnSegRbase::LoadSegEquation()\nWrong:should be \"%\"",TheToken,cutrule);
		LoadSegVar(TheToken,cutrule);
	    if(!TheToken.isEqualTo("."))
		error("ChnSegRbase::LoadSegEquation()\nWrong:should be \".\"",TheToken,cutrule);
		TheToken.output();
		TheToken.input();
		LoadCtxAttr(TheToken,attr,cutrule);
        if(!TheToken.isEqualTo("=")&&!TheToken.isEqualTo("==")&&!TheToken.isEqualTo("!=")&&!TheToken.isEqualTo("<=")&&!TheToken.isEqualTo(">="))
		  error("ChnSegRbase::LoadSegEquation()\nWrong:should be \"=\"or \"==\"or \"!=\" or \"<=\" or \">=\"",TheToken,cutrule);
		TheToken.output();
		TheToken.input();
	    LoadAtom(TheToken,attr->getAttType(),attr->getLimit(),attr->getValList(),cutrule);
						
}

void ChnSegRbase::LoadCtxFunction(IOToken &TheToken,char *cutrule)
{
  	if(!TheToken.isEqualTo("FIND")&&!TheToken.isEqualTo("NOFIND"))
		error("ChnSegRbase::LoadSegEquation()\nWrong:should be \"FIND\"or\"NOFIND\"",TheToken,cutrule);
	 	LoadCtxFunc(TheToken,cutrule);
	    if(!TheToken.isEqualTo("{"))
		error("ChnSegRbase::LoadSegEquation()\nWrong:should be \"{\"",TheToken,cutrule);
		TheToken.output();
		TheToken.input();
		LoadCtxCondition(TheToken,cutrule);
        if(!TheToken.isEqualTo("}"))
		  error("ChnSegRbase::LoadSegEquation()\nWrong:should be \"}\"",TheToken,cutrule);
		TheToken.output();
		TheToken.input();
}

void ChnSegRbase::LoadSegVar(IOToken &TheToken,char *cutrule)
{
	if(!TheToken.isEqualTo("%"))
		error("ChnSegRbase::LoadSegVar()\nWrong:should be \"%\"",TheToken,cutrule);
	TheToken.output();
	TheToken.input();
	LoadSegString(TheToken,cutrule);
}

void ChnSegRbase::LoadCtxAttr(IOToken &TheToken,Attribute *&attr,char *cutrule)
{
	if( TheToken.getType()!=IDENT ) 
			error("ChnSegRbase::LoadCtxAttr(): Should be IDENT",TheToken,cutrule);
	if(TheToken.getType()==IDENT) {
		// check if the identifier is defined in SortedAttList
		// if yes, find out its type and vallist for searching its value.
		AttLink * link;
  	   	link=(mkModel->getSortedAttList()).getAttLinkHead();
		while(link && 
			  strcmp(TheToken.getBuffer(),link->getAttr()->getString())) 
				link=link->getNext();
		if(!link) 
			error("ChnTagRbase::LoadCtxAttr(): Undefined Attribute",TheToken,cutrule);

		if(link->getAttr()->getIntExt()==INTERNAL){
			attr=link->getAttr();
		}
	

		// if it is alias, find its real name.
		if(link->getAttr()->getNameAlias()==ALIAS){
			TheToken.setBuffer(link->getAttr()->getRefer()->getString());
			attr=link->getAttr()->getRefer();
		}

	}
	Attribute * yjattr;
	char Yinjie[TOKENLENGTH];
	yjattr=GlobalModel->getAttribute("音节");
	if (yjattr==NULL) error("ChnXXXTranslator::open(): Model should should have a  attribute named 音节.");
	if (yjattr->getNameAlias()==ALIAS) strcpy(Yinjie,yjattr->getRefer()->getString());
	else strcpy(Yinjie,yjattr->getString());
	if(!TheToken.isEqualTo("ccat")&&!TheToken.isEqualTo("yx")&&!TheToken.isEqualTo(Yinjie))
		error("ChnSegRbase::LoadCtxAttr()\nWrong:should be \"ccat\" or \"yx\"or \"音节\"",TheToken,cutrule);
	TheToken.output();
	TheToken.input();
	
}

void ChnSegRbase::LoadCtxFunc(IOToken &TheToken,char *cutrule)
{
	if(!TheToken.isEqualTo("FIND")&&!TheToken.isEqualTo("NOFIND"))
		error("ChnSegRbase::LoadCtxFunc()\nWrong:should be \"FIND\" or \"NOFIND\"",TheToken,cutrule);
	TheToken.output(' ');
    TheToken.output(' ');
	TheToken.output();
	TheToken.input();
    if(!TheToken.isEqualTo("(")&&!TheToken.isEqualTo("（"))
		error("ChnSegRbase::LoadCtxFunc()\nWrong:should be \"(\" or \"（\"",TheToken,cutrule);
	TheToken.output();
	TheToken.input();
	LoadDirection(TheToken,cutrule);
    if(!TheToken.isEqualTo(","))
		error("ChnSegRbase::LoadCtxFunc()\nWrong:should be \",\" ",TheToken,cutrule);
	TheToken.output();
	TheToken.input();
	LoadDistance(TheToken,cutrule);
	if(!TheToken.isEqualTo(","))
		error("ChnSegRbase::LoadCtxFunc()\nWrong:should be \",\" ",TheToken,cutrule);
	TheToken.output();
	TheToken.input();
	LoadCtxPattern(TheToken,cutrule);
    if(!TheToken.isEqualTo(")")&&!TheToken.isEqualTo("）"))
		error("ChnSegRbase::LoadCtxFunc()\nWrong:should be \")\" or \"）\"",TheToken,cutrule);
	TheToken.output();
	TheToken.input();
}

void ChnSegRbase::LoadDirection(IOToken &TheToken,char *cutrule)
{
	if(!TheToken.isEqualTo("L")&&!TheToken.isEqualTo("R")&&!TheToken.isEqualTo("LR"))
		error("ChnSegRbase::LoadDirection()\nWrong:should be \"L\" or \"R\" or \"LR\"",TheToken,cutrule);
	TheToken.output();
	TheToken.input();

}

void ChnSegRbase::LoadDistance(IOToken &TheToken,char *cutrule)
{
	if(!TheToken.isEqualTo("NEXT")&&!TheToken.isEqualTo("NEAR")&&!TheToken.isEqualTo("FAR"))
		error("ChnSegRbase::LoadDistance()\nWrong:should be \"NEXT\" or \"NEAR\" or \"FAR\" ",TheToken,cutrule);
	TheToken.output();
	TheToken.input();

}

void ChnSegRbase::LoadCtxPattern(IOToken &TheToken,char *cutrule)
{
	if (TheToken.getType()!=IDENT)
	  error("ChnSegRbase::LoadCtxPattern()\nWrong:should be IDENT",TheToken,cutrule); 
    LoadCtxString(TheToken,cutrule);
	while (TheToken.isEqualTo("+"))
	{
		TheToken.output();
		TheToken.input();
        if (TheToken.getType()!=IDENT)
  	       error("ChnSegRbase::LoadCtxPattern()\nWrong:should be IDENT",TheToken,cutrule); 
        LoadCtxString(TheToken,cutrule);
	}
}

void ChnSegRbase::LoadCtxString(IOToken &TheToken,char *cutrule)
{
    if (TheToken.getType()!=IDENT &&!TheToken.isEqualTo("$"))
	    error("ChnSegRbase::LoadCtxString()\nWrong:should be IDENT or \"$\"",TheToken,cutrule); 
    TheToken.output();
    TheToken.input();
}

void ChnSegRbase::LoadCtxCondition(IOToken &TheToken,char *cutrule)
{
	if(!TheToken.isEqualTo("%"))
		error("ChnSegRbase::LoadCtxCondition()\nWrong:should be \"%\"",TheToken,cutrule);
	   LoadCtxEquation(TheToken,cutrule);
	   while (TheToken.isEqualTo(","))
	   {
		   TheToken.output();
		   TheToken.input();
		   if(!TheToken.isEqualTo("%"))
	          error("ChnSegRbase::LoadCtxCondition()\nWrong:should be \"%\"",TheToken,cutrule);
	        LoadCtxEquation(TheToken,cutrule);
	   }
}

void ChnSegRbase::LoadCtxEquation(IOToken &TheToken,char *cutrule)
{
	  Attribute *attr;
	  LoadCtxVar(TheToken,cutrule);
	  if(!TheToken.isEqualTo("."))
	          error("ChnSegRbase::LoadCtxEquation()\nWrong:should be \".\"",TheToken,cutrule);
	  TheToken.output();
	  TheToken.input();
	  LoadCtxAttr(TheToken,attr,cutrule);
	  if(!TheToken.isEqualTo("=")&&!TheToken.isEqualTo("==")&&!TheToken.isEqualTo("!=")&&!TheToken.isEqualTo("<=")&&!TheToken.isEqualTo(">="))
		  error("ChnSegRbase::LoadCtxEquation()\nWrong:should be \"=\"or \"==\"or \"!=\" or \"<=\" or \">=\"",TheToken,cutrule);
	  TheToken.output();
	  TheToken.input();
	  LoadAtom(TheToken,attr->getAttType(),attr->getLimit(),attr->getValList(),cutrule);
	  

}

void ChnSegRbase::LoadCtxVar(IOToken &TheToken,char *cutrule)
{
	if(!TheToken.isEqualTo("%"))
		error("ChnSegRbase::LoadCtxCtx()\nWrong:should be \"%\"",TheToken,cutrule);
	 TheToken.output();
	 TheToken.input();
	 LoadCtxString(TheToken,cutrule);
}


void ChnSegRbase::LoadSegResult(IOToken &TheToken,char *cutrule)
{
	if (!TheToken.isEqualTo("1")&&!TheToken.isEqualTo("2")&&!TheToken.isEqualTo("3"))
		error("ChnSegRbase::LoadSegResult()\nWrong: Should be \"1\" or \"2\" or \"3\".",TheToken,cutrule);
	TheToken.output();  //output result
	TheToken.input();
	if (TheToken!="" && TheToken!="CONDITION" &&
		TheToken!="OTHERWISE" && TheToken!="@@")
		error("ChnSegRbase::LoadSegResult()\nWrong: Should be \"CONDITION\" or \"OTHERWISE\" or \"@@\".",TheToken,cutrule);
}
#endif  //KBMS

#ifdef KBMS
BOOL ChnTagRbase::rdTxtTagRule()
{
	return rdTxtRecord("@@");
}

int ChnTagRbase::LoadTagRule(IOToken &TheToken)
{
    int len=strlen(TheToken.getInPntr());
	char *cutrule=new char[len+1];
	strcpy(cutrule,TheToken.getInPntr());
	char *p;
	p=TheToken.getOutPntr();
	TheToken.input();        //input @@
	if( !TheToken.isEqualTo("@@") ) 
		error("ChnTagRbase::LoadTagRule()\nWrong: Should be \"@@\"",TheToken,cutrule);
	TheToken.output();
	TheToken.output(' ');
	TheToken.input();     // input word or *
	LoadTagStruct(TheToken,cutrule);
	LoadTagBody(TheToken,cutrule);
	if (TheToken.isEqualTo("OTHERWISE"))
		LoadTagEnd(TheToken,cutrule);
	TheToken.output('\n');
	int num;
	num=TheToken.getOutPntr()-p;
	TheToken.outputNull();
	delete [] cutrule;
	return num;
}

void ChnTagRbase::LoadTagEnd(IOToken &TheToken,char *cutrule)
{
	TheToken.output("\n   ");
	TheToken.output();
	TheToken.input();
	if( !TheToken.isEqualTo("SELECT") ) 
		error("ChnTagRbase::LoadTagRule()\nWrong: Should be \"SELECT\"",TheToken,cutrule);
	TheToken.output(' ');
	TheToken.output();
	TheToken.input();
	TheToken.output(' ');
	LoadTagResult(TheToken,cutrule);
}
	
void ChnTagRbase::LoadTagStruct(IOToken &TheToken,char *cutrule)
{
    LoadTagWord(TheToken,cutrule);
	if(!TheToken.isEqualTo("("))
		error("ChnSegRbase::LoadTagStruct()\nWrong:should be \"(\"",TheToken,cutrule);
	TheToken.output();  //output "("
	TheToken.input();   // input Part-of-speech
    LoadTagLabel(TheToken,cutrule);
    if(!TheToken.isEqualTo(")"))
		error("ChnTagRbase::LoadTagStruct()\nWrong:should be \")\"",TheToken,cutrule);
	TheToken.output();   // output ")"
	TheToken.input();

}

void ChnTagRbase::LoadTagWord(IOToken &TheToken,char *cutrule)
{
   if (TheToken.isEqualTo("*"))
      {   TheToken.output();
          TheToken.input();
       }
   else LoadWord(TheToken,cutrule);
}

void ChnTagRbase::LoadTagLabel(IOToken &TheToken,char *cutrule)
{
   	if(TheToken.getType()!=IDENT) 
			error("MKbase::LoadTagLabel(): Should be IDENT",TheToken,cutrule);
	AttLink * link;
	link=mkModel->getasynModel(CHINESE).getLexCat().getAttLinkHead();
	ValLink * vlink;
	vlink=link->getAttr()->getValList().getValLinkHead();
	while(vlink && strcmp(TheToken.getBuffer(),vlink->getVal()->getString()))
	     vlink=vlink->getNext();
	if(!vlink)
		error("MKbase::LoadTagLabel(): Undefined Attribute  in LexCat",TheToken,cutrule); 
	TheToken.output();
	TheToken.input();
	//if(!TheToken.isEqualTo("-"))
	//	error("ChnTagRbase::LoadTagLabel()\nWrong:should be \"-\"",TheToken,cutrule);
   while (TheToken.isEqualTo("-"))
	{  TheToken.output();    //output "-"
       TheToken.input();
	   if(TheToken.getType()!=IDENT) 
			error("MKbase::LoadTagLabel(): Should be IDENT",TheToken,cutrule);
    	link=mkModel->getasynModel(CHINESE).getLexCat().getAttLinkHead();
	 	vlink=link->getAttr()->getValList().getValLinkHead();
    	while(vlink && strcmp(TheToken.getBuffer(),vlink->getVal()->getString()))
	       vlink=vlink->getNext();
	   if(!vlink)
	     	error("MKbase::LoadTagLabel(): Undefined Attribute  in LexCat",TheToken,cutrule); 
	   TheToken.output();
       TheToken.input();
     }
 }

void ChnTagRbase::LoadTagBody(IOToken &TheToken,char *cutrule)
{
   if(!TheToken.isEqualTo("CONDITION"))
		   error("ChnTagRbase::LoadTagBody()\nWrong:should be \"CONDITION\"",TheToken,cutrule);
    while(TheToken.isEqualTo("CONDITION"))
	   LoadTagBranch(TheToken,cutrule);
        
}


void ChnTagRbase::LoadTagBranch(IOToken &TheToken,char *cutrule)
{
   TheToken.output('\n');
   TheToken.output(' ');
   TheToken.output(' ');
   TheToken.output(' ');
   TheToken.output();  //output CONDITION
   TheToken.output(' ');
   TheToken.input();
   LoadTagCondition(TheToken,cutrule);
   if(!TheToken.isEqualTo("SELECT"))
		error("ChnSegRbase::LoadSegBranch()\nWrong:should be \"SELECT\"",TheToken,cutrule);
   TheToken.output(' ');
   TheToken.output();  //output SELECT
   TheToken.output(' ');
   TheToken.input();
   LoadTagResult(TheToken,cutrule);
}

void ChnTagRbase::LoadTagCondition(IOToken &TheToken,char *cutrule)
{
	if(!TheToken.isEqualTo("$")&&!TheToken.isEqualTo("FIND")&&!TheToken.isEqualTo("NOFIND"))
		error("ChnSegRbase::LoadTagCondition()\nWrong:should be \"$\" or \"FIND\"or\"NOFIND\"",TheToken,cutrule);
	if (TheToken.isEqualTo("$"))
			LoadTagEquation(TheToken,cutrule);
	else if (TheToken.isEqualTo("FIND")||TheToken.isEqualTo("NOFIND")) 
			LoadCtxFunction(TheToken,cutrule);
	while(TheToken.isEqualTo(",")){
		TheToken.output();
		TheToken.input();
		if (TheToken.isEqualTo("$"))
			LoadTagEquation(TheToken,cutrule);
		else if (TheToken.isEqualTo("FIND")||TheToken.isEqualTo("NOFIND")) 
			LoadCtxFunction(TheToken,cutrule);
    }
   
}

void ChnTagRbase::LoadTagEquation(IOToken &TheToken,char *cutrule)
{
	    Attribute *attr;
	   	LoadTagVar(TheToken,cutrule);
	    if(!TheToken.isEqualTo("."))
		error("ChnSegRbase::LoadTagEquation()\nWrong:should be \".\"",TheToken,cutrule);
		TheToken.output();
		TheToken.input();
		LoadCtxAttr(TheToken,attr,cutrule);
        if(!TheToken.isEqualTo("=")&&!TheToken.isEqualTo("==")&&!TheToken.isEqualTo("!=")&&!TheToken.isEqualTo("<=")&&!TheToken.isEqualTo(">="))
		  error("ChnSegRbase::LoadTagEquation()\nWrong:should be \"=\"or \"==\"or \"!=\" or \"<=\" or \">=\"",TheToken,cutrule);
		TheToken.output();
		TheToken.input();
	    LoadAtom(TheToken,attr->getAttType(),attr->getLimit(),attr->getValList(),cutrule);
		
}

void ChnTagRbase::LoadTagVar(IOToken &TheToken,char *cutrule)
{
	if(!TheToken.isEqualTo("$"))
		error("ChnSegRbase::LoadTagVar()\nWrong:should be \"$\"",TheToken,cutrule);
		TheToken.output();
		TheToken.input();
}

void ChnTagRbase::LoadCtxAttr(IOToken &TheToken,Attribute *&attr,char *cutrule)
{
	if( TheToken.getType()!=IDENT ) 
			error("ChnTagRbase::LoadCtxAttr(): Should be IDENT",TheToken,cutrule);
	if(TheToken.getType()==IDENT) {

			// check if the identifier is defined in SortedAttList
			// if yes, find out its type and vallist for searching its value.
			AttLink * link;
  	   		link=(mkModel->getSortedAttList()).getAttLinkHead();
			while(link && 
				  strcmp(TheToken.getBuffer(),link->getAttr()->getString())) 
					link=link->getNext();
			if(!link) 
				error("ChnTagRbase::LoadExtIntVar(): Undefined Attribute",TheToken,cutrule);

			if(link->getAttr()->getIntExt()==INTERNAL){
				attr=link->getAttr();
			}
		

			// if it is alias, find its real name.
			if(link->getAttr()->getNameAlias()==ALIAS){
				TheToken.setBuffer(link->getAttr()->getRefer()->getString());
				attr=link->getAttr()->getRefer();
			}

		}
	Attribute * yjattr;
	char Yinjie[TOKENLENGTH];
	yjattr=GlobalModel->getAttribute("音节");
	if (yjattr==NULL) error("ChnXXXTranslator::open(): Model should should have a  attribute named 音节.");
	if (yjattr->getNameAlias()==ALIAS) strcpy(Yinjie,yjattr->getRefer()->getString());
	else strcpy(Yinjie,yjattr->getString());
	if(!TheToken.isEqualTo("ccat")&&!TheToken.isEqualTo("yx")&&!TheToken.isEqualTo(Yinjie))
		error("ChnTagRbase::LoadCtxAttr()\nWrong:should be \"ccat\" or \"yx\"or \"音节\"",TheToken,cutrule);
	TheToken.output();
	TheToken.input();
	
}

void ChnTagRbase::LoadCtxFunction(IOToken &TheToken,char *cutrule)
{
  	if(!TheToken.isEqualTo("FIND")&&!TheToken.isEqualTo("NOFIND"))
		  error("ChnSegRbase::LoadTagEquation()\nWrong:should be \"FIND\"or\"NOFIND\"",TheToken,cutrule);
	 	LoadCtxFunc(TheToken,cutrule);
	    if(!TheToken.isEqualTo("{"))
		error("ChnSegRbase::LoadTagEquation()\nWrong:should be \"{\"",TheToken,cutrule);
		TheToken.output();
		TheToken.input();
		LoadCtxCondition(TheToken,cutrule);
        if(!TheToken.isEqualTo("}"))
		  error("ChnSegRbase::LoadTagEquation()\nWrong:should be \"}\"",TheToken,cutrule);
		TheToken.output();
		TheToken.input();
}


void ChnTagRbase::LoadCtxFunc(IOToken &TheToken,char *cutrule)
{
	if(!TheToken.isEqualTo("FIND")&&!TheToken.isEqualTo("NOFIND"))
		error("ChnTagRbase::LoadCtxFunc()\nWrong:should be \"FIND\" or \"NOFIND\"",TheToken,cutrule);
	TheToken.output(' ');
    TheToken.output(' ');
	TheToken.output();
	TheToken.input();
    if(!TheToken.isEqualTo("(")&&!TheToken.isEqualTo("（"))
		error("ChnTagRbase::LoadCtxFunc()\nWrong:should be \"(\" or \"（\"",TheToken,cutrule);
	TheToken.output();
	TheToken.input();
	LoadDirection(TheToken,cutrule);
    if(!TheToken.isEqualTo(","))
		error("ChnTagRbase::LoadCtxFunc()\nWrong:should be \",\" ",TheToken,cutrule);
	TheToken.output();
	TheToken.input();
	LoadDistance(TheToken,cutrule);
	if(!TheToken.isEqualTo(","))
		error("ChnTagRbase::LoadCtxFunc()\nWrong:should be \",\" ",TheToken,cutrule);
	TheToken.output();
	TheToken.input();
	LoadCtxPattern(TheToken,cutrule);
    if(!TheToken.isEqualTo(")")&&!TheToken.isEqualTo("）"))
		error("ChnTagRbase::LoadCtxFunc()\nWrong:should be \")\" or \"）\"",TheToken,cutrule);
	TheToken.output();
	TheToken.input();
}

void ChnTagRbase::LoadDirection(IOToken &TheToken,char *cutrule)
{
	if(!TheToken.isEqualTo("L")&&!TheToken.isEqualTo("R")&&!TheToken.isEqualTo("LR"))
		error("ChnTagRbase::LoadDirection()\nWrong:should be \"L\" or \"R\" or \"LR\"",TheToken,cutrule);
	TheToken.output();
	TheToken.input();

}

void ChnTagRbase::LoadDistance(IOToken &TheToken,char *cutrule)
{
	if(!TheToken.isEqualTo("NEXT")&&!TheToken.isEqualTo("NEAR")&&!TheToken.isEqualTo("FAR"))
		error("ChnTagRbase::LoadDistance()\nWrong:should be \"NEXT\" or \"NEAR\" or \"FAR\" ",TheToken,cutrule);
	TheToken.output();
	TheToken.input();

}

void ChnTagRbase::LoadCtxPattern(IOToken &TheToken,char *cutrule)
{
	if (TheToken.getType()!=IDENT)
	  error("ChnTagRbase::LoadCtxPattern()\nWrong:should be IDENT",TheToken,cutrule); 
    LoadCtxString(TheToken,cutrule);
	while (TheToken.isEqualTo("+"))
	{
		TheToken.output();
		TheToken.input();
        if (TheToken.getType()!=IDENT)
  	       error("ChnTagRbase::LoadCtxPattern()\nWrong:should be IDENT",TheToken,cutrule); 
        LoadCtxString(TheToken,cutrule);
	}
}

void ChnTagRbase::LoadCtxString(IOToken &TheToken,char *cutrule)
{
    if (TheToken.getType()!=IDENT)
	    error("ChnTagRbase::LoadCtxString()\nWrong:should be IDENT",TheToken,cutrule); 
    TheToken.output();
    TheToken.input();
}

void ChnTagRbase::LoadCtxCondition(IOToken &TheToken,char *cutrule)
{
	if(!TheToken.isEqualTo("%"))
		error("ChnTagRbase::LoadCtxCondition()\nWrong:should be \"%\"",TheToken,cutrule);
	   LoadCtxEquation(TheToken,cutrule);
	   while (TheToken.isEqualTo(","))
	   {
		   TheToken.output();
		   TheToken.input();
		   if(!TheToken.isEqualTo("%"))
	          error("ChnTagRbase::LoadCtxCondition()\nWrong:should be \"%\"",TheToken,cutrule);
	        LoadCtxEquation(TheToken,cutrule);
	   }
}

void ChnTagRbase::LoadCtxEquation(IOToken &TheToken,char *cutrule)
{
	  Attribute *attr;
	  LoadCtxVar(TheToken,cutrule);
	  if(!TheToken.isEqualTo("."))
	          error("ChnTagRbase::LoadCtxEquation()\nWrong:should be \".\"",TheToken,cutrule);
	  TheToken.output();
	  TheToken.input();
	  LoadCtxAttr(TheToken,attr,cutrule);
	  if(!TheToken.isEqualTo("=")&&!TheToken.isEqualTo("==")&&!TheToken.isEqualTo("!=")&&!TheToken.isEqualTo("<=")&&!TheToken.isEqualTo(">="))
		  error("ChnTagRbase::LoadCtxEquation()\nWrong:should be \"=\"or \"==\"or \"!=\" or \"<=\" or \">=\"",TheToken,cutrule);
	  TheToken.output();
	  TheToken.input();
	  LoadAtom(TheToken,attr->getAttType(),attr->getLimit(),attr->getValList(),cutrule);
	  

}

void ChnTagRbase::LoadCtxVar(IOToken &TheToken,char *cutrule)
{
	if(!TheToken.isEqualTo("%"))
		error("ChnTagRbase::LoadCtxVar()\nWrong:should be \"%\"",TheToken,cutrule);
	 TheToken.output();
	 TheToken.input();
	 LoadCtxString(TheToken,cutrule);
}

void ChnTagRbase::LoadTagResult(IOToken &TheToken,char *cutrule)
{
   if(TheToken.getType()!=IDENT) 
			error("MKbase::LoadTagResult(): Should be IDENT",TheToken,cutrule);
	AttLink * link;
	link=mkModel->getasynModel(CHINESE).getLexCat().getAttLinkHead();
	ValLink * vlink;
	vlink=link->getAttr()->getValList().getValLinkHead();
	while(vlink && strcmp(TheToken.getBuffer(),vlink->getVal()->getString()))
	     vlink=vlink->getNext();
	if(!vlink)
		error("MKbase::LoadTagResult(): Undefined Attribute  in LexCat",TheToken,cutrule); 
	TheToken.output();
	TheToken.input();
   while (TheToken.isEqualTo("-"))
	{  TheToken.output();    //output "-"
       TheToken.input();
	   if(TheToken.getType()!=IDENT) 
			error("MKbase::LoadTagResult(): Should be IDENT",TheToken,cutrule);
	link=mkModel->getasynModel(CHINESE).getLexCat().getAttLinkHead();
	vlink=link->getAttr()->getValList().getValLinkHead();
	while(vlink && strcmp(TheToken.getBuffer(),vlink->getVal()->getString()))
	     vlink=vlink->getNext();
	if(!vlink)
		error("MKbase::LoadTagResult(): Undefined Attribute  in LexCat",TheToken,cutrule); 
	TheToken.output();
	TheToken.input();
    }
}
#endif //KBMS
