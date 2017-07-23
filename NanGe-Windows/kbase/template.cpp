
/**********************************************************
* 程序名:TEMPLATE.C                                       *
* 说  明:完成有关模板的操作                               *
* 时  间:1996.7.4                                         *
**********************************************************/
/*********************************************************
* EDIT HISTORY                                           *
*   1996.7.4   written by CBB							 *											 *
*	1997.1.16  fix a bug in Template::free()			 *
*********************************************************/
#include "template.h"
//#include  <io.h>
#include <iostream>
#include <string.h>

/************************************
* 以下是类FinalTPL的成员函数定义
************************************/
void FinalTPL::free()
{
    AttrList.free();
}
void FinalTPL::combine(FinalTPL *tpl) //完成模板组合
{
	AttrVal *av1,*av2;    
	TargetTPL *ttp1,*ttp2;
	
	av2=tpl->getAttrList().getAttListHead();	
	while ( av2!=NULL )
	{

        av1=AttrList.IsAnAtt(av2);
		
		if(av1!=NULL) //av2指向的属性已经存在
			if(strcmp(av1->getValue(),av2->getValue())!=0)
			{	
				if(strcmp(av1->getValue(),"")!=0 )
				{
					if (strcmp(av2->getValue(),"")!=0)
					{
						strcat(av1->getValue(),"|");
						strcat(av1->getValue(),av2->getValue());
					}
				}
				else
					av1->SetValue(av2->getValue());
			}
		if(av1==NULL) //av2指向的属性尚不存在
		{
			AttrVal * avtemp;
			avtemp = new AttrVal;
			avtemp->SetName(av2->getName());
			avtemp->SetValue(av2->getValue());
			avtemp->SetAttClass(av2->getAttClass());
			avtemp->SetAssAttName(av2->getAssAttName());
			avtemp->SetNext(NULL);	           
		    AttrList.AddAAttrVal(avtemp);
		}
		av2=av2->getNext();
	}
 //以下处理目标模板
	 ttp2=tpl->getTarget().getTargetHead();
	 while (ttp2!=NULL)
	 {
	 	ttp1=Target.IsATarget(ttp2->getName());
	 	if (ttp1==NULL)
	 	{
	 		TargetTPL *ttp;
	 		ttp = new TargetTPL;
	 		ttp->setName(ttp2->getName());
	 		ttp->setNext(NULL);
	 		Target.AddATarget(ttp);
	 	}
	 	ttp2=ttp2->getNext(); 
	 }

}
void FinalTPL::compute(TPL *tpl)
{
	AttrVal *av1,*av2;
	TargetTPL *ttp1,*ttp2;

	while(strcmp(tpl->getName(),"")!=0) //当tpl指向的模板不是根节点时,执行循环
	{
		
		av2=tpl->getAttrList().getAttListHead();
		while (av2!=NULL)
		{
			av1=AttrList.IsAnAtt(av2);
			if(av1==NULL)                //当av2指向的属性尚不存在时,将其加入
			{
				AttrVal * avtemp;                 //增加对关联属性的支持
				avtemp = new AttrVal;
				avtemp->SetName(av2->getName());
				avtemp->SetValue(av2->getValue());
				avtemp->SetAssAttName(av2->getAssAttName());
				avtemp->SetAttClass(av2->getAttClass());
				avtemp->SetNext(NULL);	           
		    	AttrList.AddAAttrVal(avtemp);
		    }
		    av2=av2->getNext();
		 }
		 //以下处理目标模板
		 ttp2=tpl->getTarget().getTargetHead();
		 while (ttp2!=NULL)
		 {
		 	ttp1=Target.IsATarget(ttp2->getName());
		 	if (ttp1==NULL)
		 	{
		 		TargetTPL *ttp;
		 		ttp = new TargetTPL;
		 		ttp->setName(ttp2->getName());
		 		ttp->setNext(NULL);
		 		Target.AddATarget(ttp);
		 	}
		 	ttp2=ttp2->getNext();
		 }
		 tpl=tpl->getParent();
	}
}

/************************************
* 以下是类AttrValList的成员函数定义
************************************/
AttrVal * AttrValList::IsAnAtt(AttrVal *av)
{
	AttrVal *tmp=getAttListHead();
	while (tmp!=NULL)
	{
		if(strcmp(tmp->getName(),av->getName())==0)
			if (strcmp(tmp->getAssAttName(),av->getAssAttName())==0)
			 	break;
		tmp=tmp->getNext();
	}
	return tmp;
}

char * AttrValList::GetValueOfAttribute(const char *szAttributeName)
{
	AttrVal *tmp=getAttListHead();
	while (tmp!=NULL)
	{
		if(strcmp(tmp->getName(),szAttributeName)==0) break;
		tmp=tmp->getNext();
	}
	if (tmp!=NULL) return tmp->getValue();
	else return NULL ;
}

void AttrValList::AddAAttrVal( AttrVal *av)
{
	AttrVal *tmp=getAttListHead();
	if (tmp==NULL)
	{
	    AttListHead=av;
	    return;
	 }
	 while(tmp->getNext()!=NULL) tmp=tmp->getNext();
	 tmp->SetNext(av); 
} 
void AttrValList::free()
{
	AttrVal *tmp;
	
	tmp=AttListHead;	
	while (AttListHead!=NULL)
	{
		tmp = AttListHead;
		AttListHead = AttListHead->getNext();
		delete tmp;
	}
}

/*************************************
* 以下是类TargetList的成员函数定义
*************************************/
int TargetList::DispAllTarget() //显示所有的目标模板，返回目标模板的个数
{
	int count=0;
	TargetTPL *tgt=getTargetHead();
	while(tgt!=NULL)
	{
		count++;
		cout<<"\t"<<count<<"----"<<tgt->getName()<<endl;
		tgt=tgt->getNext();
	}	       
	return count;
}
char *TargetList::findTarget(int no) 
{
	int count=0;
	TargetTPL *tgt=getTargetHead();
	while(tgt!=NULL)
	{
		count++;
		if (count==no) return tgt->getName();
		tgt=tgt->getNext();
	}	       
	return "";
}
TargetTPL * TargetList::IsATarget(const char *Name)
{
	TargetTPL *tmp=getTargetHead();
	while (tmp!=NULL)
	{
		if(strcmp(tmp->getName(),Name)==0) break;
		tmp=tmp->getNext();
	}
	return tmp;
}

void TargetList::AddATarget( TargetTPL *ttpl)
{
	TargetTPL *tmp=getTargetHead();
	if (tmp==NULL) 
	{
		TargetHead=ttpl;
		return;
	}
	while (tmp->getNext()!=NULL) tmp=tmp->getNext();
	tmp->setNext(ttpl);
}
void TargetList::free()
{
	TargetTPL *ttpl;

	ttpl=TargetHead;	
	while (TargetHead!=NULL)
	{
		ttpl = TargetHead;
		TargetHead = TargetHead->getNext();
		delete ttpl;
	}
}


/**************************************
* 以下是类TPL的成员函数的定义
**************************************/

int TPL::DispAllChild()  			//显示本模板的所有子模板
{									//返回值是子模板的个数
	int i=0;
	
	TPL *tmp=firstChild;
	
/*	cout<<Name<<endl;
	cout<<endl;*/
	while(tmp!=NULL)
	{
		i++;
		cout<<'\t'<<i<<"---"<<tmp->getName()<<endl;
		tmp=tmp->getNextBrother();
	}
	return i;
}	
char * TPL::findChild(int no)  	
{								
	int i=0;
	
	TPL *tmp=firstChild;
	
	while(tmp!=NULL)
	{
		i++;
		if (i==no) return tmp->getName();
		tmp=tmp->getNextBrother();
	}
	return "";
}
	

void TPL::AddAChild(  TPL * tpntr) //为该模板增加一个子模板
{
     if ( firstChild==NULL )
     {
     		firstChild = tpntr;
      		return;
      }
      else
      {
       		TPL *tmp = firstChild;
       		tmp->AddABrother(tpntr);
       		return;
       }
}

void TPL::AddABrother( TPL *tpntr) //为该模板增加一个兄弟模板
{
   	if ( nextBrother==NULL)
   	{
   		nextBrother = tpntr;
   		return;
   	}
   	else
   	{
   		TPL *tmp=nextBrother;
   		tmp->AddABrother(tpntr);
   		return;
   	}
}

void TPL::free()
{
     FinalTPL::free();
     Target.free();
}



/**************************************************
* 以下是类Template的成员函数的定义
**************************************************/

/***********************************************************
* 根据参数指定的模板名、语言代码到相应的模板树中查找该模板
* 若找到，返回指向该模板的指针，否则，返回NULL
***********************************************************/

int  Template::DispChild(int Lang,char *tplname)
{
 	TPL *tmp1;
 	tmp1=find(tplname,Lang);
 	if (tmp1==NULL) 
 	{
 		cout<<"指定的模板不存在!"<<endl;
 		return 0;
 	}
    return tmp1->DispAllChild();

}
char * Template::findChild(const char *tplName,int LangCode,int no)
{
	TPL *tmp1;
	tmp1=find(tplName,LangCode);

	return tmp1->findChild(no);
}
TPL * Template::find(const char *tplName,int LangCode)
{
	TPL *tmp1,*tmp2;
	tmp2=&TplTree[LangCode];
	tmp1=preOrder(tplName,tmp2);  //调用先序遍历过程检索指定的模板
	return tmp1;

}

/*****************************************************
* 利用先序方法遍历模板树，并检索指定的模板
* 	若检索失败，返回NULL
* 	若检索成功，返回指向指定模板的指针
*****************************************************/
TPL * Template::preOrder(const char *tplName,TPL * tpl)
{
	if (strcmp(tplName,tpl->getName())==0)
		return tpl;
	if (tpl->getFirstChild()==NULL && tpl->getNextBrother()==NULL)
		return NULL;
	if (tpl->getFirstChild()!=NULL)
	{
		TPL *tmp1;
		tmp1=preOrder(tplName,tpl->getFirstChild());
		if (tmp1!=NULL) return tmp1;	
	}
	if (tpl->getNextBrother()!=NULL)
	{
		TPL *tmp2;
		tmp2=preOrder(tplName,tpl->getNextBrother());
		return tmp2;
	}
	return NULL;
	
}

/*************************************************
* 显示模板树
*************************************************/
void printTree(TPL * tmp,int l)
{
	int i=1;
	for (;i<l;++i) cout<<"|   ";
	for (;i<=l;++i) cout<<"+-- ";
	if ( strcmp(tmp->getName(),""))
		cout<<tmp->getName();
	else
		cout<<"ROOT";
	cout<<endl;
	if ( tmp->getFirstChild()!=NULL )
		printTree(tmp->getFirstChild(),l+1);
	if ( tmp->getNextBrother()!=NULL )
		printTree(tmp->getNextBrother(),l);
}
TPL *Template::TreeAddress(int Langcode)
{
	return &TplTree[Langcode];
}
void Template::printout()
{
	TPL *tmp2;
	for(int i=0;i<LANGNUM;i++)
	{
		tmp2=&TplTree[i];
		printTree(tmp2,0); 
	}
}

/************************************************
* 浏览模板文件(TXT文件)
************************************************/

#ifdef KBMS
/************************************************
* 从tplbase.TXT把某种语言的所有模板读入TxtBuff  *
* 忽略其中的注解信息，并把多个空格压缩为一个空格*
************************************************/
BOOL Template::rdTxtTpl()
{
	return rdTxtRecord("BeginTemplate","EndTemplate");
}
/****************************************************
* 把Template.TXT文件转换为Template.DAT文件
* 同时在内存建立起模板树结构 
*****************************************************/

void Template::Load()
{
	
	wopen(); // 以写方式打开tplbase/tplbase.DAT文件
	if( getStatus()!=WOPEN) 
		error("Template::Load(): cannot load while Template not open");

	beginScan(MAXBUFFER);

	char txtfilename[FILENAMELENGTH];
	getFullName(txtfilename,".TXT");
	/* 利用getPath(),getName()以及提供的后缀生成完整的路径名
	   即 tplbase/tplbase.TXT */
	
	/* 以读方式打开tplbase.TXT 文件*/
	TxtFile.open(txtfilename,ios::in|ios::binary);
	if( !TxtFile ) error("Template::Load(): TXTOPEN",txtfilename);

	CErrorException * err=NULL;
	try
	{
		int i;
		IOToken TheToken;

		BOOL flag;
		flag=TRUE;
		
		/*从tplbase.TXT把某种语言的所有模板读入TxtBuff */
		while(flag=rdTxtTpl())
		{       
			TheToken.setInPntr(TxtBuff);
			TheToken.setOutPntr(DatBuff);
			try
			{
				i=LoadTPLs(TheToken);
			}
			catch (CErrorException *)
			{
				message("Template:",TxtBuff);
				throw;
			}
			wtDatLength(i);
			wtDatBuff();
		}
	}
	catch (CErrorException * e) { err=e; }

	TxtFile.close();

	endScan();
	close();
#ifdef _UNIX
	printout();
#endif
	setBuilt(YES);
	free();

	if (err) throw err;
}

/****************************************************
* 把某种语言的所有模板从TxtBuff读到DatBuff          *
* 在内存中建立起该语言的模板树                      *
****************************************************/
int Template::LoadTPLs(IOToken &TheToken)
{
	char *TempPntr;
	TempPntr=TheToken.getOutPntr();
	TheToken.input();
	if(!TheToken.isEqualTo("BeginTemplate")) 
		error("Template::LoadTPLs(): Need \"BeginTemplate\"",TheToken.getBuffer());

	TheToken.output();  //把"BeginTemplate"输出到DatBuff
	TheToken.input();

	int lang;
	lang=LoadLangCode(TheToken);//读入当前正在处理的模板的语言代码
	TheToken.output('\n');	
	LoadTPLsBody(TheToken,lang);

	if(!TheToken.isEqualTo("EndTemplate")) 
		error("Template::LoadTPLs(): Need \"EndTemplate\"",TheToken.getBuffer());

	TheToken.output(); //把"EndTemplate"输出到DatBuff
	TheToken.output('\n');
	TheToken.output('\n');
	int num=TheToken.getOutPntr()-TempPntr; //计算该语言所有模板的长度
	TheToken.outputNull();
	return num;//返回长度
}

/******************************************************
* 把"{语言名称}"从TxtBuff输出到DatBuff
* 完成从语言名称到语言代码的转换工作
* 并返回这个语言代码
*******************************************************/
int	 Template::LoadLangCode(IOToken &TheToken)
{
	if(!TheToken.isEqualTo("{"))  //当前TOKEN是"{"吗?
		error("Template::LoadLangCode(): Need \"{\"",TheToken.getBuffer());

	TheToken.output(' '); //输出空格到DatBuff,起分割作用
	TheToken.output();   //输出当前TOKEN到DatBuff,即"{"
	TheToken.input();

	int i;  //语言名称到语言代码的转换
	for(i=0;i<LANGNUM;i++) if(TheToken.isEqualTo(LANGSTRING[i])) break;
	if(i>=LANGNUM) error("Template::LoadLangCode(): Invalid LangCode");

	TheToken.output();
	TheToken.input();

	if(!TheToken.isEqualTo("}")) 
		error("Template::LoadLangCode(): Need \"}\"",TheToken.getBuffer());

	TheToken.output();
	TheToken.input();
	return i;//返回语言代码
}

/*****************************************************
* 装入某种语言的所有模板                             *
* 进入此函数时,TxtBuff应为"Name..."                  *
*****************************************************/
void Template::LoadTPLsBody(IOToken &TheToken,int lang)
{
     
     while ( TheToken.isEqualTo("Name") )  //每一次循环处理一个模板
     	LoadATPL( TheToken,lang);          //装入一个模板
}
/*********************************************************
* 装入模板文件中的一个模板，将其加入模板树               *
* 进入此函数时,TxtBuff应为"Name..."                      *
*********************************************************/
void Template::LoadATPL(IOToken &TheToken,int lang)
{
	
	TPL *tpl;
	
	tpl=new TPL;

	loadName(TheToken,tpl);
	loadParent(TheToken,tpl,lang);
	loadTarget(TheToken,tpl);
	loadAttVal(TheToken,tpl);
}
/*******************************************
* 装入Name域 	                           *
* 进入此函数时,TxtBuff应为"Name..."        *
*******************************************/
void Template::loadName(IOToken &TheToken,TPL *tpl)
{

	if (!TheToken.isEqualTo("Name"))   
		error("Template::loadName(): Need \"Name\"",TheToken.getBuffer());
	TheToken.output('\t');
	TheToken.output();  //输出"Name"
	
	TheToken.input();	//输入":"
	if (!TheToken.isEqualTo(":"))   
		error("Template::loadName(): Need \":\"",TheToken.getBuffer());
	TheToken.output();   //输出":"
	
	TheToken.input(); //输入模板名 
	if (TheToken.getType()!=IDENT)
		error("Template::loadName(): Template Name must be a string");
	tpl->setName(TheToken.getBuffer());
	TheToken.output(); //输出模板名
	TheToken.output('\n');

	TheToken.input(); //输入"Parent"或"Target"
}
/**********************************************************
* 装入Parent域                                            *
* 进入此函数时,TxtBuff应为"Parent..."或"Target..."        *
**********************************************************/
void Template::loadParent(IOToken &TheToken,TPL *tpl,int lang)
{
 	if (!TheToken.isEqualTo("Parent")&&!TheToken.isEqualTo("Target"))   
		error("Template::loadParent(): Need \"Parent\"or \"Target\"",TheToken.getBuffer());
	if ( TheToken.isEqualTo("Parent"))
	{
		TheToken.output('\t');	
		TheToken.output(); //输出"Parent"
		
		TheToken.input();  //输入":"
		if ( !TheToken.isEqualTo(":"))
			error("Template::loadParent(): Need \":\"",TheToken.getBuffer());
		TheToken.output();
		
		TheToken.input();  //输入父模板名
		if (TheToken.getType()!=IDENT)
			error("Template::loadParent(): Parent Name must be a string");
		TPL * tmp=NULL;
		tmp=find(TheToken.getBuffer(),lang);//在该语言的模板树中检索父模板
		if ( tmp==NULL )
			error("Template:loadParent():Parent Template does not exist!",TheToken.getBuffer());
		tpl->setParent(tmp);
		tmp->AddAChild(tpl);//把该模板作为父模板的一个子模板加入父模板
		TheToken.output();
		TheToken.output('\n');
		
		TheToken.input();//输入"Target"
	}
	else
	{      
	 //将该模板加入模板树(无父模板的情况)
		TPL * tmp=NULL;
		tmp=find("",lang);//在该语言的模板树中检索父模板
		if ( tmp==NULL )
			error("Template::loadParent():Virtual Parent Template does not exist!","");
		tpl->setParent(tmp);
		tmp->AddAChild(tpl);//把该模板作为父模板的一个子模板加入父模板
	}
}
/*********************************************
* 装入Target域                               *
* 进入此函数时,TxtBuff应为"Target..."        *
*********************************************/
void Template::loadTarget(IOToken &TheToken,TPL *tpl)
{
	if(!TheToken.isEqualTo("Target"))
		error("Template::loadTarget(): Need \"Target\"",TheToken.getBuffer());
	TheToken.output('\t');
	TheToken.output();//输出Target
	
	TheToken.input();//输入":"
	if (!TheToken.isEqualTo(":"))   
		error("Template::loadTarget(): Need \":\"",TheToken.getBuffer());
	TheToken.output();
	
	TheToken.input();//输入"{"或"BEGIN"
	
	int i;
	while (TheToken.isEqualTo("{")) //每循环一次向目标模板链中加入一个目标模板
	{     
		
		i=LoadLangCode(TheToken);
		if(TheToken.getType()!=IDENT )
			error("Template::loadTarget(): Target Name must be a string");
		TargetTPL *ttpl=new TargetTPL;
		ttpl->setName(TheToken.getBuffer());
		ttpl->setLangCode(i);
		tpl->getTarget().AddATarget(ttpl);
		TheToken.output();//输出模板名
		TheToken.input();//输入","或"BEGIN"
		if (!TheToken.isEqualTo(",")&&!TheToken.isEqualTo("BEGIN")) 
			error("Template::loadTarget(): Need \",\"or \"BEGIN\"",TheToken.getBuffer());
		if (TheToken.isEqualTo(",")) 
		{	
			TheToken.output();//输出","
			TheToken.input();//输入"{",准备读入下一目标模板
			if (!TheToken.isEqualTo("{"))   
				error("Template::loadTarget(): Need \"{\"",TheToken.getBuffer());
		}
	}
	TheToken.output('\n');
}
/*********************************************
* 装入各类属性值                             *
* 进入此函数时,TxtBuff应为"BEGIN..."         *			
*********************************************/
void Template::loadAttVal(IOToken &TheToken,TPL *tpl)
{               
	char szAssName[MAXSTRLENGTH];
	strcpy(szAssName,"");

	if (!TheToken.isEqualTo("BEGIN"))   
		error("Template::loadAttval(): Need \"BEGIN\"",TheToken.getBuffer());
	TheToken.output('\t');
	TheToken.output(); 		//输出"BEGIN"
	TheToken.output('\n');
	TheToken.input();  		//输入属性类别标记,如:"DistinguishAttribute"
	Attribute * attr;
	Value * val;
	int nCurAttClass=0;
	int nIsAssAtt=0;
	
	//一次循环处理一个属性
	while ( !TheToken.isEqualTo("END"))
	{
        if (!TheToken.isEqualTo("DistinguishAttribute")
        	&&!TheToken.isEqualTo("NoValueAttribute")
        	&&!TheToken.isEqualTo("CommonAttribute"))
        	error("Template:loadAttVal():Need\"DistinguishAttribute\"or\"NoValue"
        		"Attribute\"or\"CommonAttribute\"",TheToken.getBuffer());
        
        if (TheToken.isEqualTo("DistinguishAttribute"))
        	nCurAttClass=AC_DISTINGUISH;	
        if (TheToken.isEqualTo("NoValueAttribute"))
        	nCurAttClass=AC_NOVALUE;
        if (TheToken.isEqualTo("CommonAttribute"))
        	nCurAttClass=AC_COMMON;
        TheToken.output('\t');
        TheToken.output('\t');
        TheToken.output();
        TheToken.output('\n');
        TheToken.input();     //输入属性名或AssAtttibute
        while (!TheToken.isEqualTo("DistinguishEnd")
        	&&!TheToken.isEqualTo("CommonEnd")
        	&&!TheToken.isEqualTo("NoValueEnd"))
        {	
			if (TheToken.isEqualTo("AssAttribute"))
			{	
				nIsAssAtt = AC_ASSOCIATE;
				TheToken.output('\t');
				TheToken.output('\t');
				TheToken.output('\t');
				TheToken.output(); //输出AssAttribute
				TheToken.input();  //输入关联属性名
				attr = NULL;
				attr = mkModel->getAttribute(TheToken.getBuffer());
				if (attr == NULL)
					error("Template::loadAttVal():Undefined Attribute",
						TheToken.getBuffer());
				strcpy(szAssName,TheToken.getBuffer());
				TheToken.output(' ');
				TheToken.output(); //输出属性名
				TheToken.input();  //输入"with"
				if (!TheToken.isEqualTo("with"))
					error("Template::loadAttVal():Need \"with\"",
					TheToken.getBuffer());
				TheToken.output(' ');
				TheToken.output(); //输出"with"
				TheToken.output('\n'); //输出"with"
				TheToken.input();  //输入属性名
			}
			AttrVal *av = new AttrVal;	

			//检查该属性在语言模型中是否已经定义?
			attr=NULL;
			attr=mkModel->getAttribute(TheToken.getBuffer());
			if (attr==NULL)
				error("Template::loadAttVal():Undefined Attribute ",
					TheToken.getBuffer());
		
			
			av->SetAssAttName(szAssName);
			av->SetName(TheToken.getBuffer());
			av->SetAttClass(nCurAttClass|nIsAssAtt);
			TheToken.output('\t');
			TheToken.output('\t');
			TheToken.output('\t');
			if ( nIsAssAtt == AC_ASSOCIATE ) TheToken.output('\t');
			TheToken.output();		//输出属性名
			TheToken.input();       //输入":"或","
		
			if (av->IsDistinguish()&&!TheToken.isEqualTo(":"))
				error("Template::loadAttVal():Distinguish Attribute must have value!",
					av->getName());
			if (av->IsCommon()&&!TheToken.isEqualTo(":"))
				error("Template::loadAttVal():Common Attribute must have value!",
					av->getName());
			if (av->IsNoValue()&&TheToken.isEqualTo(":"))
				error("Template::loadAttVal():No Value Attribute must not have value!",
					av->getName());
					
			if (!TheToken.isEqualTo(":")
				&&!TheToken.isEqualTo(",")
				&&!TheToken.isEqualTo("DistinguishEnd")
				&&!TheToken.isEqualTo("CommonEnd")
				&&!TheToken.isEqualTo("NoValueEnd")
				&&!TheToken.isEqualTo("AssEnd")) 
				error("Template::loadAttVal(): Need \":\"or \",\"or\"Disting"
					"uishEnd\"or\"NoValueEnd\"or\"CommonEnd\"or\"AssEnd\"",
					TheToken.getBuffer());
			if ( TheToken.isEqualTo(":"))
			{
				TheToken.output();//输出":"
				TheToken.input(); //输入值
				val=NULL;
				if ( attr->getLimit()==1 )
				{
					if ( attr->getNameAlias()!=1)
						val=attr->getValList().find(TheToken.getBuffer());
						//检查该值是否该属性的合法值?
					else
						val=attr->getRefer()->getValList().find(TheToken.getBuffer());
						//检查该值是否该属性的合法值?
					if ( val == NULL )
						error("Template::loadAttVal():Undefined Value ",
							TheToken.getBuffer());
				}
		    	av->SetValue(TheToken.getBuffer());
				TheToken.output();//输出值
				//输入","或"DistinguishEnd"or"CommonEnd"or"NoValueEnd"
				TheToken.input(); 
				if (!TheToken.isEqualTo(",")
					&&!TheToken.isEqualTo("DistinguishEnd")
					&&!TheToken.isEqualTo("CommonEnd")
					&&!TheToken.isEqualTo("NoValueEnd")
					&&!TheToken.isEqualTo("AssEnd")) 
					error("Template::loadAttVal(): Need \",\"or\"DistinguishEnd"
						"\"or\"CommonEnd\"or\"NoValueEnd\"",TheToken.getBuffer());
			}
			tpl->getAttrList().AddAAttrVal(av);
			if (TheToken.isEqualTo("AssEnd"))
			{
				nIsAssAtt = 0;
				strcpy(szAssName,""); 
				TheToken.output('\n');
				TheToken.output('\t');
				TheToken.output('\t');
				TheToken.output('\t');
				TheToken.output();//输出"AssEnd"
				TheToken.input(); //输入后继符号，如：",","DistinguishEnd"等
				if (!TheToken.isEqualTo(",")
					&&!TheToken.isEqualTo("DistinguishEnd")
					&&!TheToken.isEqualTo("CommonEnd")
					&&!TheToken.isEqualTo("NoValueEnd")	) 
					error("Template::loadAttVal(): Need \",\"or\"DistinguishEnd"
						"\"or\"CommonEnd\"or\"NoValueEnd\"",TheToken.getBuffer());
			}
			
			if (TheToken.isEqualTo(","))
			{
				TheToken.output(); //输出","
				TheToken.input();  //输入下一属性
			}
			if (TheToken.isEqualTo("DistinguishEnd")
				||TheToken.isEqualTo("CommonEnd")
				||TheToken.isEqualTo("NoValueEnd"))
				nCurAttClass = 0;
			TheToken.output('\n');
		}
		TheToken.output('\t');
		TheToken.output('\t');
		TheToken.output(); // 输出"DistinguishEnd"、"CommonEnd"、"NoValueEnd"
		TheToken.output('\n');
		TheToken.input();  // 输入"DistinguishAttribute""END"
	}
	TheToken.output('\t');
	TheToken.output(); // 输出"END"
	TheToken.output('\n');
	TheToken.input();//输入"Name"或"EndTemplate"
}
#endif // KBMS
			
void Template::build()
{
	if (getBuilt()==YES ) return;
	ropen();                      //以读方式打开tplbase/tplbase.DAT
	beginScan(MAXBUFFER);

	try
	{
		int leng;          
		while(leng=rdDatLength()){
			rdDatBuff(leng);
			IToken TheToken(DatBuff);
			ReadTPLs(TheToken);
		}
	}
	catch (CErrorException *) { endScan(); close(); throw; }

	endScan();                  
	close();                    //关闭tplbase/tplbase.DAT文件
	setBuilt(YES);
	MKbase::build();
}

void Template::ReadTPLs(IToken &TheToken)
{		
	TheToken.input();           //读入"BeginTemplate"

	TheToken.input();           //读入"{"

	int lang;
	lang=ReadLangCode(TheToken);//读入当前正在处理的模板的语言代码
	ReadTPLsBody(TheToken,lang);

}		
		
int	Template::ReadLangCode(IToken &TheToken)
{
	TheToken.input();	// 读入语言名称
	int i;
	for(i=0;i<LANGNUM;i++) if(TheToken.isEqualTo(LANGSTRING[i])) break;
	TheToken.input();	// 读入"}"
	TheToken.input();	// 读入"Name"
	return i;
}
		
void Template::ReadTPLsBody(IToken &TheToken,int lang)
{		
     
     while ( TheToken.isEqualTo("Name") )  //每一次循环处理一个模板
     	ReadATPL( TheToken,lang);          //装入一个模板
}
void Template::ReadATPL(IToken &TheToken,int lang)
{
	TPL *tpl;
	
	tpl=new TPL;

	ReadName(TheToken,tpl);
	ReadParent(TheToken,tpl,lang);
	ReadTarget(TheToken,tpl);
	ReadAttVal(TheToken,tpl);
}
void Template::ReadName(IToken &TheToken,TPL *tpl)
{
    //此时"Name"已在缓冲区中	
	TheToken.input();	//输入":"
	TheToken.input();   //输入模板名 

	tpl->setName(TheToken.getBuffer());
	
	TheToken.input();   //输入"Parent"或"Target"
}
void Template::ReadParent(IToken &TheToken,TPL *tpl,int lang)
{ 
	//此时TOKEN缓冲区中是"Parent"或"Target"
	
	if ( TheToken.isEqualTo("Parent"))
	{
		TheToken.input();  //输入":"
		TheToken.input();  //输入父模板名

		TPL * tmp=NULL;
		tmp=find(TheToken.getBuffer(),lang);//在该语言的模板树中检索父模板
		if ( tmp==NULL )
			error("Template::ReadParent():Parent Template does not exist!",TheToken.getBuffer());

		tpl->setParent(tmp);
		tmp->AddAChild(tpl);//把该模板作为父模板的一个子模板加入父模板
		
		TheToken.input();//输入"Target"
	}
	else
	{      
	 //将该模板加入模板树(无父模板的情况)
		TPL * tmp=NULL;
		tmp=find("",lang);//在该语言的模板树中检索父模板
		if ( tmp==NULL )
			error("Template::ReadParent():Virtual Parent Template does not exist!","");
		tpl->setParent(tmp);
		tmp->AddAChild(tpl);//把该模板作为父模板的一个子模板加入父模板
	}
}
void Template::ReadTarget(IToken &TheToken,TPL *tpl)
{
	//此时TOKEN缓冲区中为"Target"
	TheToken.input();//输入":"
	TheToken.input();//输入"{"或"BEGIN"
	
	int i;
	while (TheToken.isEqualTo("{")) //每循环一次向目标模板链中加入一个目标模板
	{     
		
		i=ReadLangCode(TheToken);
        //此时TOKEN缓冲区中为目标模板名称
		TargetTPL *ttpl=new TargetTPL;
		ttpl->setName(TheToken.getBuffer());
		ttpl->setLangCode(i);
		tpl->getTarget().AddATarget(ttpl);

		TheToken.input();//输入","或"BEGIN"
		if (TheToken.isEqualTo(",")) TheToken.input();//输入"{",准备读入下一目标模板
	}
}




void Template::ReadAttVal(IToken &TheToken,TPL *tpl)
{                                                 
 
	char szAssName[MAXSTRLENGTH];
	strcpy(szAssName,"");
	int nCurAttClass = 0;
	int nIsAssAtt = 0;
	//此时TOKEN缓冲区中为"BEGIN"
	TheToken.input();  		//输入属性类别标记
	Attribute * attr;
	while ( !TheToken.isEqualTo("END"))
	{
        if (TheToken.isEqualTo("DistinguishAttribute"))
        	nCurAttClass=AC_DISTINGUISH;	
        if (TheToken.isEqualTo("NoValueAttribute"))
        	nCurAttClass=AC_NOVALUE;
        if (TheToken.isEqualTo("CommonAttribute"))
        	nCurAttClass=AC_COMMON;
 	    TheToken.input(); //输入属性名或"AssAttribute" 
        while (!TheToken.isEqualTo("DistinguishEnd")
        	&&!TheToken.isEqualTo("CommonEnd")
        	&&!TheToken.isEqualTo("NoValueEnd"))
        {	
			if (TheToken.isEqualTo("AssAttribute"))
			{	
				nIsAssAtt = AC_ASSOCIATE;
				TheToken.input();  //输入关联属性名
				attr = NULL;
				attr = mkModel->getAttribute(TheToken.getBuffer());
				if (attr == NULL)
					error("Template::ReadAttVal():Undefined Attribute",
						TheToken.getBuffer());
				strcpy(szAssName,TheToken.getBuffer());
				TheToken.input();  //输入"with"
				TheToken.input();  //输入属性名
			}

			attr=NULL;
			attr=mkModel->getAttribute(TheToken.getBuffer());//检查该属性在语言模型中是否已经定义?
			if (attr==NULL)
				error("Template::RaedAttVal():Undefined Attribute ",
					TheToken.getBuffer());
		
			AttrVal *av = new AttrVal;       
			av->SetAssAttName(szAssName);
			av->SetName(TheToken.getBuffer());
			av->SetAttClass(nCurAttClass|nIsAssAtt);
		
			TheToken.input();       //输入":"或","
		
			if ( TheToken.isEqualTo(":"))
			{
				TheToken.input(); //输入值
		    	av->SetValue(TheToken.getBuffer());
				TheToken.input(); //输入","等
			}
			tpl->getAttrList().AddAAttrVal(av);
			if (TheToken.isEqualTo("AssEnd"))
			{
				nIsAssAtt = 0;
				strcpy(szAssName,""); 
				TheToken.input(); //输入后继符号，如：",","DistinguishEnd"等
			}
			if (TheToken.isEqualTo(",")) TheToken.input();  //输入下一属性
			if (TheToken.isEqualTo("DistinguishEnd")
				||TheToken.isEqualTo("CommonEnd")
				||TheToken.isEqualTo("NoValueEnd"))
				nCurAttClass = 0;
		}
		TheToken.input();//输入"DistinguishAttribute"、"END"
	}
	TheToken.input();//输入"Name"或"EndTemplate"
}

void treefree(TPL *tpl);
void Template::free()   		             //释放模板树结构所分配的内存空间
{
	if(getBuilt()==NO) return;
	TPL * tpl;
	int lang;
	for (lang=0;lang<LANGNUM;lang++)
	{
		tpl=TplTree[lang].getFirstChild();
		if(tpl!=NULL) treefree(tpl);
		TplTree[lang].setFirstChild((TPL*)NULL);

	}    
	MKbase::free();
}
void treefree(TPL *tpl)
{
	TPL *left,*right;
	
	left=tpl->getFirstChild();
	right=tpl->getNextBrother();
	
	if (left!=NULL)
	{
		treefree(left);
		tpl->setFirstChild((TPL*)NULL);
	}
	if (right!=NULL)
	{
		treefree(right);
		tpl->setNextBrother((TPL*)NULL);
	}
	tpl->free();
	delete tpl;
}
	
