#include "cetrans.h"
#include "../kbase_ce/lexdbase.h"

SrcSgmt * LexAmbiguation::GetMaxPrevSgmt(SrcSgmt * sgmt)
{
	SrcSect * sect;
	sect=sgmt->getThisLstSect();
	while (sect && sect->getNodeNum()==0)
		sect=sect->getSameLstSect();
	if (sect) return sect->getPrevSrcSgmt();
	else return NULL;
}

SrcSgmt * LexAmbiguation::GetMaxSucsSgmt(SrcSgmt * sgmt)
{
	SrcSect * sect;
	sect=sgmt->getThisFstSect();
	while (sect && sect->getNodeNum()==0)
		sect=sect->getSameFstSect();
	if (sect) return sect->getSucsSrcSgmt();
	else return NULL;
}

int LexAmbiguation::TestCtxFunction(IToken & token)
{
	enum { FIND,NOFIND } func;
	if (token=="FIND") func=FIND;
	else if (token=="NOFIND") func=NOFIND;
	else error("SegAmbiguation::TestCtxFunction(): should be FIND or NOFIND.",token);
	token.input(); // skip FIND or NOFIND
	token.input(); // skip "("
	enum { DIR_L,DIR_R,DIR_LR } direction;
	if (token=="L") direction=DIR_L;
	else if (token=="R") direction=DIR_R;
	else if (token=="LR") direction=DIR_LR;
	else error("SegAmbiguation::TestCtxFunction(): should be L or R or LR.",token);
	token.input(); // skip direction
	token.input(); // skip ","
	enum { DST_NEXT,DST_NEAR, DST_FAR } distance;
	if (token=="NEXT") distance=DST_NEXT;
	else if (token=="NEAR") distance=DST_NEAR;
	else if (token=="FAR") distance=DST_FAR;
	else error("SegAmbiguation::TestCtxFunction(): should be ONE,TWO,FAR or END.",token);
	token.input(); // skip distance
	token.input(); // skip ","
	SectVarList ctxvarlist;
	while (TRUE)
	{
		SectVariable segvar;
		segvar.setLabel(token);
		ctxvarlist.AddTail(segvar);
		token.input();
		if (token=="+") token.input();
		else break;
	}
	token.input(); // skip ")"
	token.input(); // skip "{"
	SrcSgmt * sgmt;
	SectVarLink * ctxvarlink;
	int found=FALSE;
	if (direction==DIR_L || direction==DIR_LR)
	{
		ctxvarlink=ctxvarlist.GetTail();
		if (strcmp(ctxvarlink->GetData()->getLabel(),"$")==0)
			return ctxvarlink->GetPrev()==NULL
				&& FstSrcSgmt->getPrevSgmt()==NULL;
		// use max-matching
		for (sgmt=FstSrcSgmt->getPrevSgmt();sgmt;sgmt=GetMaxPrevSgmt(sgmt))
		{
			if (sgmt==FstSrcSgmt->getPrevSgmt() && distance!=DST_NEXT) continue;
			if (sgmt->getSgmtType()==PCTCHAR && distance==DST_NEAR) break;
			if (TestLeftContext(sgmt,ctxvarlink,token)==SUCC) { found=TRUE; break; }
			if (sgmt==FstSrcSgmt->getPrevSgmt() && distance==DST_NEXT) break;
		}
		/* use all-matching
		for (sgmt=FstSrcSgmt->getPrevSgmt();sgmt;sgmt=sgmt->getPrevSgmt())
		{
			if (sgmt==FstSrcSgmt->getPrevSgmt() && distance!=DST_NEXT) continue;
			if (sgmt->getSgmtType()==PCTCHAR && distance==DST_NEAR) break;
			if (TestLeftContext(sgmt,ctxvarlink,token)==SUCC) { found=TRUE; break; }
			if (sgmt==FstSrcSgmt->getPrevSgmt() && distance==DST_NEXT) break;
		}
		*/
	}
	if (!found && (direction==DIR_R || direction==DIR_LR))
	{
		ctxvarlink=ctxvarlist.GetHead();
		if (strcmp(ctxvarlink->GetData()->getLabel(),"$")==0)
			return ctxvarlink->GetNext()==NULL
				&& FstSrcSgmt->getSucsSgmt()==NULL;
		// use max-matching
		for (sgmt=LstSrcSgmt->getSucsSgmt();sgmt;sgmt=GetMaxSucsSgmt(sgmt))
		{
			if (sgmt==LstSrcSgmt->getSucsSgmt() && distance!=DST_NEXT) continue;
			if (sgmt->getSgmtType()==PCTCHAR && distance==DST_NEAR) break;
			if (TestRightContext(sgmt,ctxvarlink,token)==SUCC) { found=TRUE; break; }
			if (sgmt==LstSrcSgmt->getSucsSgmt() && distance==DST_NEXT) break;
		}
		/* use all-matching
		for (sgmt=LstSrcSgmt->getSucsSgmt();sgmt;sgmt=sgmt->getSucsSgmt())
		{
			if (sgmt==LstSrcSgmt->getSucsSgmt() && distance!=DST_NEXT) continue;
			if (sgmt->getSgmtType()==PCTCHAR && distance==DST_NEAR) break;
			if (TestRightContext(sgmt,ctxvarlink,token)==SUCC) { found=TRUE; break; }
			if (sgmt==LstSrcSgmt->getSucsSgmt() && distance==DST_NEXT) break;
		}
		*/
	}
	token.goover("}");
	return func==FIND && found || func==NOFIND && !found;
}

int LexAmbiguation::TestLeftContext(SrcSgmt * sgmt,SectVarLink * ctxvarlink,IToken & token)
{
	if (sgmt==NULL)
		return strcmp(ctxvarlink->GetData()->getLabel(),"$")==0
			&& ctxvarlink->GetPrev()==NULL;
	SrcSect * sect;
	// use max-matching
	sect=sgmt->getThisLstSect();
	while (sect && get_sect_ccat(sect)==NULL) sect=sect->getSameLstSect();
	if (sect==NULL) return FAIL;
	SectVariable * ctxvar=ctxvarlink->GetData();
	ctxvar->setSect(sect);
	if (TestCtxCondition(ctxvar,token)==FAIL) return FAIL;
	if (ctxvarlink->GetPrev()==NULL) return SUCC;
	return TestLeftContext(sect->getPrevSrcSgmt(),ctxvarlink->GetPrev(),token);
	/* use all-matching
	for (sect=sgmt->getThisLstSect();sect;sect=sect->getSameLstSect())
	{
		if (get_sect_ccat(sect)==NULL) continue;
		SectVariable * ctxvar=ctxvarlink->GetData();
		ctxvar->setSect(sect);
		if (TestCtxCondition(ctxvar,token)==FAIL) continue;
		if (ctxvarlink->GetPrev()==NULL) return SUCC;
		if (TestLeftContext(sect->getPrevSrcSgmt(),ctxvarlink->GetPrev(),token)==FAIL) continue;
	}
	return FAIL;
	*/
}

int LexAmbiguation::TestRightContext(SrcSgmt * sgmt,SectVarLink * ctxvarlink,IToken & token)
{
	if (sgmt==NULL)
		return strcmp(ctxvarlink->GetData()->getLabel(),"$")==0
			&& ctxvarlink->GetNext()==NULL;
	SrcSect * sect;
	// use max-matching
	sect=sgmt->getThisFstSect();
	while (sect && get_sect_ccat(sect)==NULL) sect=sect->getSameFstSect();
	if (sect==NULL) return FAIL;
	SectVariable * ctxvar=ctxvarlink->GetData();
	ctxvar->setSect(sect);
	if (TestCtxCondition(ctxvar,token)==FAIL) return FALSE;
	if (ctxvarlink->GetNext()==NULL) return SUCC;
	return TestRightContext(sect->getSucsSrcSgmt(),ctxvarlink->GetNext(),token);
	/* use all-matching
	for (sect=sgmt->getThisFstSect();sect;sect=sect->getSameFstSect())
	{
		if (get_sect_ccat(sect)==NULL) continue;
		SectVariable * ctxvar=ctxvarlink->GetData();
		ctxvar->setSect(sect);
		if (TestCtxCondition(ctxvar,token)==FAIL) continue;
		if (ctxvarlink->GetNext()==NULL) return SUCC;
		if (TestRightContext(sect->getSucsSrcSgmt(),ctxvarlink->GetNext(),token)==FAIL) continue;
	}
	return FAIL;
	*/
}

int LexAmbiguation::TestCtxCondition(SectVariable * ctxvar,IToken & token)
{
	IToken ctxtoken(token);
	while (TRUE)
	{
		ctxtoken.input(); // skip "%"
		if (strcmp(ctxvar->getLabel(),ctxtoken)==0)
		{
			ctxtoken.input();
			if (TestSectAttribute(ctxvar->getSect(),ctxtoken)==FAIL) return FAIL;
		}
		while (*ctxtoken && ctxtoken!="," && ctxtoken!="}") ctxtoken.input();
		ctxtoken.goupto(",","}");
		if (ctxtoken=="}") break;
		ctxtoken.input();
	}
	return SUCC;
}

int LexAmbiguation::TestSectAttribute(SrcSect * sect,IToken & token)
{
	if (token!=".")
		error("SegAmbiguation::TestSectAttribute(): should be \".\".",token.getInPntr());
	token.input(); // skip "."
	char ctxattr[TOKENLENGTH];
	strcpy(ctxattr,token);
	token.input(); // skip attr
	token.input(); // skip "="
	char output[ATOMLENGTH];
	OToken token0(output);
	IToken token2;
	if (strcmp(ctxattr,"ccat")==0)
	{
		token2.setInPntr(get_sect_ccat(sect)); token2.input();
		unify_Hierar(token0,token,token2);
	}
	else if (strcmp(ctxattr,"yx")==0)
	{
		token2.setInPntr(get_sect_yx(sect)); token2.input();
		unify_Symbol(token0,token,token2);
	}
	else if (strcmp(ctxattr,ChnXXXTranslator::getYinjie())==0)
	{
		token2.setInPntr(get_sect_yj(sect)); token2.input();
		unify_Number(token0,token,token2);
	}
	else error("SegAmbiguation::TestSectAttribute(): should be a sect attribute.",ctxattr);
	return (token0.getOutPntr()!=output);
}

void SegAmbiguation::AddSectVar1(int len,SrcSect * sect)
{
	char label[TOKENLENGTH];
	if (*SegSelect1) strcat(SegSelect1,"+");
	*label=0; int i; for (i=0;i<len;i++) strcat(label,"A");
	strcat(SegSelect1,label);
	SectVariable var(label,sect);
	SectVarList1.AddHead(var);
}

void SegAmbiguation::AddSectVar2(int len,SrcSect * sect)
{
	char label[TOKENLENGTH];
	if (*SegSelect2) strcat(SegSelect2,"+");
	*label=0; int i; for (i=0;i<len;i++) strcat(label,"A");
	strcat(SegSelect2,label);
	SectVariable var(label,sect);
	SectVarList2.AddHead(var);
}

void SegAmbiguation::SetPattern()
{
	int i;
	char label[TOKENLENGTH], * p, *q;
	// reverse SegSelect1 and SegSelect2, and change "A+AA" to "A+BC"
	p=SegSelect1; while (*p) p++; p--;
	q=label; i=0; while (p>=SegSelect1) { *q=*p; if (*q!='+') *q+=i++; q++; p--; }
	*q=0; strcpy(SegSelect1,label);
	p=SegSelect2; while (*p) p++; p--;
	q=label; i=0; while (p>=SegSelect2) { *q=*p; if (*q!='+') *q+=i++; q++; p--; }
	*q=0; strcpy(SegSelect2,label);
	// set SegPattern
	p=FstSrcSgmt->getBgnChar(); q=SegPattern;
	while (p<LstSrcSgmt->getEndChar()) *q++=*p++;
	*q=0;
	// change label in SectVarList1 and SectVarList2 from "A", "AA" to "A","BC"
	SectVarLink * link;
	for (i=0,link=SectVarList1.GetHead();link;link=link->GetNext())
		{ p=((SectVariable *)link->GetData())->getLabel(); while (*p) { *p++ += i++; } }
	for (i=0,link=SectVarList2.GetHead();link;link=link->GetNext())
		{ p=((SectVariable *)link->GetData())->getLabel(); while (*p) { *p++ += i++; } }
}

SrcSect * SegAmbiguation::SrhSectVarList(const char * label)
{
	SectVarLink * link;
	for (link=SectVarList1.GetHead();link;link=link->GetNext())
		if (strcmp(link->GetData()->getLabel(),label)==0) return link->GetData()->getSect();
	for (link=SectVarList2.GetHead();link;link=link->GetNext())
		if (strcmp(link->GetData()->getLabel(),label)==0) return link->GetData()->getSect();
	return NULL;
}
	
int SegAmbiguation::TestSegPattern(IToken & token,int & exchange)
{
	if (strcmp(token,"*")!=0 &&
		strcmp(SegPattern,token)!=0) return FALSE;
	token.input(); // skip SegPattern
	token.input(); // skip "("
	char segselect1[TOKENLENGTH],segselect2[TOKENLENGTH];
	const char * bgn, * end;
	// set segselect1
	bgn=token.getInPntr();
	token.goupto(",");
	end=token.getInPntr();
	strncpy(segselect1,bgn,end-bgn);
	segselect1[end-bgn]=0;
	token.input(); // skip ","
	// set segselect2
	bgn=token.getInPntr();
	token.goupto(")");
	end=token.getInPntr();
	strncpy(segselect2,bgn,end-bgn);
	segselect2[end-bgn]=0;
	token.input(); // skip ")"
	if (strcmp(segselect1,SegSelect1)==0 &&
		strcmp(segselect2,SegSelect2)==0) exchange=0;
	else if (strcmp(segselect1,SegSelect2)==0 &&
			 strcmp(segselect2,SegSelect1)==0) exchange=1;
	else return FALSE;
	return TRUE;
}

int SegAmbiguation::TestSegCondition(IToken & token)
{
	if (token!="CONDITION" && token!="OTHERWISE")
		error("SegAmbiguation::TestSegCondition(): is not SegCondition.",token.getInPntr());
	if (token=="OTHERWISE") { token.input(); return TRUE; }
	token.input(); // skip "CONDITION"
	while (TRUE)
	{
		if (token=="%")
			{ if (TestSegEquation(token)==FALSE) return FALSE; }
		else if (token=="FIND" || token=="NOFIND")
			{ if (TestCtxFunction(token)==FALSE) return FALSE; }
		else error("SegAmbiguation::TestSegCondition(): should be \"%\", or FIND or NOFIND.", token);
		if (token==",") token.input();
		else if (token=="SELECT") break;
		else error("SegAmbiguation::TestSegCondition(): should be \",\", or SELECT.", token);
	}
	return TRUE;
}

int SegAmbiguation::TestSegEquation(IToken & token)
{
	token.input(); // skip "%"
	SrcSect * sect=SrhSectVarList(token);
	if (sect==NULL)
		error("SegAmbiguation::TestSegEquation(): SegVariable not found.",token.getInPntr());
	token.input(); // skip label
	return TestSectAttribute(sect,token);
}

void SegAmbiguation::DoSegSelection(IToken & token,int exchange)
{
	token.input(); // skip "SELECT"
	if (token!=1 && token!=2)
		{ SetValidate1(); SetValidate2(); }
	else if (token==1 && exchange==0 ||
			 token==2 && exchange==1) SetValidate1();
	else SetValidate2();
	token.input(); // skip 1 or 2
}

int TagAmbiguation::TestTagPattern(IToken & token,char * pattern)
{
	const char * yx=get_sect_yx(Sect);
	if (token!="*" && strcmp(yx,token)!=0) return FALSE;
	const char * ccat=get_sect_ccat(Sect);
	token.input(); // skip TagWord
	token.input(); // skip "("
	*pattern=0;
	while (TRUE)
	{
		if (agree_Symbol(ccat,token)==FALSE) return FALSE;
		strcat(pattern,token); token.input();
		if (token=="-") { strcat(pattern,"|"); token.input(); continue; }
		else break;
	}
	token.input(); // skip ")"
	return TRUE;
}

int TagAmbiguation::TestTagCondition(IToken & token)
{
	if (token!="CONDITION" && token!="OTHERWISE")
		error("TagAmbiguation::TestTagCondition(): is not TagCondition.",token.getInPntr());
	if (token=="OTHERWISE") { token.input(); return TRUE; }
	token.input();  // skip "CONDITION"
	while (TRUE)
	{
		if (token=="$")
			{ if (TestTagEquation(token)==FALSE) return FALSE; }
		else if (token=="FIND" || token=="NOFIND")
			{ if (TestCtxFunction(token)==FALSE) return FALSE; }
		else error("TagAmbiguation::TestTagCondition(): should be \"$\", or FIND or NOFIND.", token);
		if (token==",") token.input();
		else if (token=="SELECT") break;
		else error("TagAmbiguation::TestTagCondition(): should be \",\", or SELECT.", token);
	}
	return TRUE;
}

int TagAmbiguation::TestTagEquation(IToken & token)
{
	token.input();
	return TestSectAttribute(Sect,token);
}

void TagAmbiguation::DoTagSelection(IToken & token,const char * pattern)
{
	char select[TOKENLENGTH];
	*select=0;
	token.input();
	while (TRUE)
	{
		strcat(select,token);
		token.input();
		if (token=="-") { strcat(select,"|"); token.input(); continue; }
		else break;
	}
	const char * ccat=get_sect_ccat(Sect);
	char newccat[TOKENLENGTH]; *newccat=0;
	IToken tokenccat(ccat); tokenccat.input();
	while (TRUE)
	{
		if (agree_Symbol(tokenccat,select) || !agree_Symbol(tokenccat,pattern))
			{ if (*newccat) strcat(newccat,"|"); strcat(newccat,tokenccat); }
		tokenccat.input();
		if (tokenccat=="|") tokenccat.input();
		else break;
	}
	set_sect_ccat(Sect,newccat);
}

char ChnXXXTranslator::Yinjie[TOKENLENGTH];
char ChnXXXTranslator::Hanzi[TOKENLENGTH];
//char ChnXXXTranslator::SrcJianlei[TOKENLENGTH];
//char ChnXXXTranslator::TgtJianlei[TOKENLENGTH];

void ChnXXXTranslator::cutWord1()
{
	if (Stage!=CUTWORD1) return;
	if (Direction==RETRIEVE) { cutWordBack1(); return; }

	// consult dictionary and use DupRbase
	SrcSgmt * sgmt;
	TheChnDictn->beginConsult();
	try
	{
		for (sgmt=TheSrcStruct.getSrcSgmtHead();sgmt;
			 sgmt=sgmt->getSucsSgmt()) cutSgmt(sgmt);
	}
	catch (CErrorException *) { TheChnDictn->endConsult(); throw; }
	TheChnDictn->endConsult();

	disambiguation_segmentation();
	disambiguation_tagging();
	if (UseLexdbase) disambiguation_statistical();
	else disambiguation_shortest();

#ifdef KBMS
	PtlNode * node;
	for (sgmt=TheSrcStruct.getSrcSgmtHead();sgmt;sgmt=sgmt->getSucsSgmt())
	{
		for (node=sgmt->getThisFstNode();node;node=node->getSameFstNode())
		{
			if (node->hasType()!=COMPLETE) continue;
			if (node->getAge()!=YOUTH) continue;
			useNode((CplNode *)node);
			output_cutNode((CplNode *)node);
		}
	}
#endif

	clrSectDataAll();
	Direction=PROGRESS;
}

void ChnXXXTranslator::cutWordBack1()
{
	if (Stage!=CUTWORD1) return;
	ThePrsList.free();
	TheSrcStruct.freeNodes();
	TheChnDictn->clrConsult();
	Direction=RETRIEVE;
}

void ChnXXXTranslator::disambiguation_segmentation()
{
	// find the shortest route and add the node in the route to ThePrsList
	SrcSgmt * bgnsgmt=TheSrcStruct.getSrcSgmtHead();
	SrcSgmt * endsgmt=TheSrcStruct.getSrcSgmtTail();
	SrcSgmt * stopsgmt, * oversgmt, * firstsgmt,  * lastsgmt=NULL;
	SrcSect * stopsect;
	int laststepnum=0;
	for (stopsgmt=bgnsgmt;stopsgmt;stopsgmt=stopsgmt->getSucsSgmt())
		{ stopsgmt->setStepNum(0); stopsgmt->setSectNum(0); }
	// set SectNum for each SrcSgmt
	for (stopsgmt=bgnsgmt;stopsgmt;stopsgmt=stopsgmt->getSucsSgmt())
	{
		for (stopsect=stopsgmt->getThisFstSect();stopsect;
			 stopsect=stopsect->getSameFstSect())
		{
			if (stopsect->getNodeNum()==0) continue;
			oversgmt=stopsect->getLstSrcSgmt();
			char * overchar=oversgmt->getEndChar();
			SrcSgmt * sgmt;
			for (sgmt=stopsgmt;sgmt && sgmt->getEndChar()<=overchar;sgmt=sgmt->getSucsSgmt())
				sgmt->setSectNum(sgmt->getSectNum()+1);
		}
	}
	while (lastsgmt!=endsgmt)
	{
		SectRouteList routelist;
		SectRouteLink * stopstep;
		// find the first SrcSgmt of this session
		for (firstsgmt=lastsgmt?lastsgmt->getSucsSgmt():bgnsgmt;
			 firstsgmt;firstsgmt=firstsgmt->getSucsSgmt())
		{
			// pass throught the sgmt with no segmentation ambiguation
			if (firstsgmt->getSectNum()<=1)
			{
				for (stopsect=firstsgmt->getThisFstSect();stopsect;stopsect=stopsect->getSameFstSect())
					if (stopsect->getNodeNum()!=0) break;
				if (stopsect) set_sect_valid(stopsect,TRUE);
				continue;
			}
			else break;
		}
		if (!firstsgmt) break;
		// initial the routelist
		for (stopsect=firstsgmt->getThisFstSect();stopsect;
			 stopsect=stopsect->getSameFstSect())
		{
			if (stopsect->getNodeNum()<=0) continue;
			oversgmt=stopsect->getLstSrcSgmt();
			routelist.addTail((SrcSect *)stopsect,NULL);
   			if (!lastsgmt || oversgmt->getEndChar()>lastsgmt->getEndChar())
		   		{ laststepnum=1; lastsgmt=oversgmt; }
			oversgmt->setStepNum(1);
		}
		// loop through the routelist
		for (stopstep=routelist.head();stopstep;stopstep=stopstep->getNext())
		{
			stopsgmt=stopstep->getSect()->getSucsSrcSgmt();
			// skip the SrcSgmt with no segmentation ambiguation
			if (!stopsgmt || stopsgmt->getSectNum()==1) continue; 
			int stepnum=stopstep->getStepNum()+1;
			for (stopsect=stopsgmt->getThisFstSect();stopsect;
				 stopsect=stopsect->getSameFstSect())
			{
				if (stopsect->getNodeNum()<=0) continue;
				oversgmt=stopsect->getLstSrcSgmt();
				int oldstepnum=oversgmt->getStepNum();
				if (oldstepnum && stepnum>oldstepnum+STEPNUMBERDELTA) continue;
				routelist.addTail((SrcSect *)stopsect,stopstep);
				if (!lastsgmt || oversgmt->getEndChar()>lastsgmt->getEndChar())
					{ laststepnum=stepnum; lastsgmt=oversgmt; }
				if (stepnum<oldstepnum) oversgmt->setStepNum(stepnum);
			}
		}

		SegAmbList amblist;
		// generate segmantation-ambiguation list by compare route each other
		amblist_generation(amblist,routelist,lastsgmt,laststepnum);
		// process segmantation-ambiguation list
		amblist_processing(amblist);
		// find all valid route
		amblist_using(amblist,routelist,lastsgmt,laststepnum);
	}
}

void ChnXXXTranslator::amblist_generation(SegAmbList & amblist,SectRouteList & routelist,SrcSgmt * lastsgmt,int laststepnum)
{
	SectRouteLink * routeone, * routetwo;
	for (routeone=routelist.head();routeone;routeone=routeone->getNext())
	{
		if (routeone->getSect()->getLstSrcSgmt()!=lastsgmt) continue;
		if (routeone->getStepNum()>laststepnum+STEPNUMBERDELTA) break;
		for (routetwo=routeone->getNext();routetwo;routetwo=routetwo->getNext())
		{
			if (routetwo->getSect()->getLstSrcSgmt()!=lastsgmt) continue;
			if (routetwo->getStepNum()>laststepnum+1) break;
			SectRouteLink * stepone=routeone, * steptwo=routetwo;
			while (stepone && steptwo)
			{
				if (stepone->getSect()==steptwo->getSect())
					{ stepone=stepone->getPrev(); steptwo=steptwo->getPrev(); continue; }
				SegAmbiguation * segamb=new SegAmbiguation;
				int len1=0, len2=0;
				segamb->SetLstSrcSgmt(stepone->getSect()->getLstSrcSgmt());
				while (stepone->getSect()->getFstSrcSgmt()!=steptwo->getSect()->getFstSrcSgmt())
				{
					len1++;
					len2++;
					if (stepone->getSect()->getBgnChar() > steptwo->getSect()->getBgnChar())
					{
						segamb->AddSectVar1(len1,stepone->getSect());
						stepone=stepone->getPrev(); len1=0;
					}
					else // (stepone->getSect()->getBgnChar() < steptwo->getSect()->getBgnChar())
					{
						segamb->AddSectVar2(len2,steptwo->getSect());
						steptwo=steptwo->getPrev(); len2=0;
					}
				}
				len1++; len2++;
				segamb->AddSectVar1(len1,stepone->getSect());
				segamb->AddSectVar2(len2,steptwo->getSect());
				segamb->SetFstSrcSgmt(stepone->getSect()->getFstSrcSgmt());
				segamb->SetPattern();
				amblist.AddSegAmbiguation(segamb);
				stepone=stepone->getPrev();
				steptwo=steptwo->getPrev();
			}
		}
	}
}

void ChnXXXTranslator::amblist_processing(SegAmbList & amblist)
{
	SegAmbLink * amblink;
	for (amblink=amblist.GetHead();amblink;amblink=amblink->GetNext())
	{
		SegAmbiguation * amb;
		amb=amblink->GetData();
		cRuleList & rulelist=TheChnSegRbase->rd_SegRules("");
		RuleLink * rulelink;
		for (rulelink=&rulelist?rulelist.getRuleLinkHead():NULL;
			 rulelink;rulelink=rulelink->getNext())
		{
			const char * rule=rulelink->getRule();
			IToken token(rule); token.input();
			token.input(); // skip "@@"
			int exchange;
			if (amb->TestSegPattern(token,exchange)==FALSE) continue;
#ifdef KBMS
			const char * pntr1,*pntr2;
#endif
			while (token=="CONDITION" || token=="OTHERWISE")
			{
#ifdef KBMS
				pntr1=token.getInPntr();
#endif
				if (amb->TestSegCondition(token)==FALSE)
					token.goupto("CONDITION","OTHERWISE"); // skip "SELECT x"
			}
			if (token=="SELECT")
			{
				amb->DoSegSelection(token,exchange);
#ifdef KBMS
				pntr2=token.getInPntr();
				output_segmentation(*amb,rule,pntr1,pntr2);
#endif
				break;
			}
		}
		if (amb->GetValidate1()==0 && amb->GetValidate2()==0)
		{
			int len1=strlen(amb->GetSegSelect1());
			int len2=strlen(amb->GetSegSelect2());
			if (len1<len2) amb->SetValidate1();
			else if (len1>len2) amb->SetValidate2();
			else { amb->SetValidate1(); amb->SetValidate2(); }
#ifdef KBMS
			if (len1!=len2) output_segmentation(*amb);
#endif
		}
	}
}

void ChnXXXTranslator::amblist_using(SegAmbList & amblist,SectRouteList & routelist,SrcSgmt * lastsgmt,int laststepnum)
{
	SectRouteLink * routelink;
	int succnumber=0;
	for (routelink=routelist.head();routelink;routelink=routelink->getNext())
	{
		if (routelink->getSect()->getLstSrcSgmt()!=lastsgmt) continue;
		if (routelink->getStepNum()>laststepnum+1) break;
		SegAmbLink * amblink;
		SectRouteLink * step;
		int valid=TRUE;
		for (amblink=amblist.GetHead();amblink;amblink=amblink->GetNext())
		{
			SegAmbiguation * amb=amblink->GetData();
			for (step=routelink;step;step=step->getPrev())
			{
				char * stepend=step->getSect()->getEndChar();
				char * ambend=amb->GetLstSrcSgmt()->getEndChar();
				if (stepend > ambend) continue;
				if (stepend < ambend) break;
				if (amb->GetValidate1()==FALSE &&
					amb->GetSectVarList1().through(step)) valid=FALSE;
				if (amb->GetValidate2()==FALSE &&
					amb->GetSectVarList2().through(step)) valid=FALSE;
			}
			if (valid==FALSE) break;
		}
		if (valid==FALSE) continue;
		succnumber++;
		for (step=routelink;step;step=step->getPrev())
			set_sect_valid(step->getSect(),TRUE);
	}
	if (succnumber!=0) return;
	// no success path found, use shortest path
#ifdef KBMS
	output_nopathfind();
#endif
	for (routelink=routelist.head();routelink;routelink=routelink->getNext())
	{
		if (routelink->getSect()->getLstSrcSgmt()!=lastsgmt) continue;
		if (routelink->getStepNum()>laststepnum) break;
		SectRouteLink * step;
		for (step=routelink;step;step=step->getPrev())
			set_sect_valid(step->getSect(),TRUE);
	}
}

void ChnXXXTranslator::disambiguation_tagging()
{
	SrcSgmt * sgmt;
	for (sgmt=TheSrcStruct.getSrcSgmtHead();sgmt;sgmt=sgmt->getSucsSgmt())
	{
		SrcSect * sect;
		for (sect=sgmt->getThisFstSect();sect;sect=sect->getSameFstSect())
		{
			if (sect->getData()==NULL || get_sect_valid(sect)==FALSE) continue;
			TagAmbiguation amb(sect);
			char ccat[TOKENLENGTH];
			while (strstr(get_sect_ccat(sect),"|")!=NULL)
			{
				strcpy(ccat,get_sect_ccat(sect));
				cRuleList & rulelist=TheChnTagRbase->rd_TagRules("");
				RuleLink * rulelink;
				for (rulelink=&rulelist?rulelist.getRuleLinkHead():NULL;
					 rulelink;rulelink=rulelink->getNext())
				{
					const char * rule=rulelink->getRule();
					IToken token(rule); token.input();
					token.input(); // skip "@@"
					char pattern[TOKENLENGTH];
					if (amb.TestTagPattern(token,pattern)==FALSE) continue;
#ifdef KBMS
					const char * pntr1,*pntr2;
#endif
					while (token=="CONDITION" || token=="OTHERWISE")
					{
#ifdef KBMS
						pntr1=token.getInPntr();
#endif
						if (amb.TestTagCondition(token)==FALSE)
							token.goupto("CONDITION","OTHERWISE"); // skip "SELECT x"
					}
					if (token=="SELECT")
					{
						amb.DoTagSelection(token,pattern);
						if (*get_sect_ccat(sect)==0)
							error("ChnXXXTranslator::disambiguation_tagging(): tagrbase error.",
							      "SELECT label is not in pattern.",rule);
#ifdef KBMS
						pntr2=token.getInPntr();
						output_tagging(amb,rule,pntr1,pntr2);
#endif
					}
				}
				if (strlen(get_sect_ccat(sect))>=strlen(ccat)) break;
			}
			strcpy(ccat,get_sect_ccat(sect));
			PtlNode * node;
			for (node=sect->getFstNode();node;node=node->getSameSrcSectNode())
			{
				if (node->hasType()!=COMPLETE) continue;
				if (agree_Symbol(ccat,((CplNode *)node)->getSrcLabel())==FALSE)
					node->setAge(DEAD);
			}
		}
	}
}

void ChnXXXTranslator::disambiguation_shortest()
{
	/* find the shortest route and add the node in the route to ThePrsList */
	SrcSgmt * bgnsgmt=TheSrcStruct.getSrcSgmtHead();
	SrcSgmt * endsgmt=TheSrcStruct.getSrcSgmtTail();
	SrcSgmt * stopsgmt, * oversgmt, * firstsgmt,  * lastsgmt=NULL;
	PtlNode * stopnode;
	int laststepnum=0;
	for (stopsgmt=bgnsgmt;stopsgmt;stopsgmt=stopsgmt->getSucsSgmt())
		{ stopsgmt->setStepNum(0); stopsgmt->setNodeNum(0); }
	// set NodeNum for each SrcSgmt
	for (stopsgmt=bgnsgmt;stopsgmt;stopsgmt=stopsgmt->getSucsSgmt())
	{
		for (stopnode=stopsgmt->getThisFstNode();stopnode;
			 stopnode=stopnode->getSameFstNode())
		{
			if (get_sect_valid(stopnode->getSrcSect())==FALSE ||
				stopnode->getAge()!=BABY) continue;
			oversgmt=stopnode->getLstSrcSgmt();
			char * overchar=oversgmt->getEndChar();
			SrcSgmt * sgmt;
			for (sgmt=stopsgmt;sgmt && sgmt->getEndChar()<=overchar;sgmt=sgmt->getSucsSgmt())
				sgmt->setNodeNum(sgmt->getNodeNum()+1);
		}
	}
	while (lastsgmt!=endsgmt)
	{
		NodeRouteList routelist;
		NodeRouteLink * stopstep, * overstep;
		/* find the first SrcSgmt of this session */
		for (firstsgmt=lastsgmt?lastsgmt->getSucsSgmt():bgnsgmt;
			 firstsgmt;firstsgmt=firstsgmt->getSucsSgmt())
		{
			// pass throught the sgmt with no ambiguation
			if (firstsgmt->getNodeNum()<=1)
			{
				for (stopnode=firstsgmt->getThisFstNode();stopnode;
					 stopnode=stopnode->getSameFstNode())
				{
					if (get_sect_valid(stopnode->getSrcSect())==TRUE &&
							stopnode->getAge()==BABY) break;
				}
				/* add the node with no ambiguation to the PrsList */
				if (stopnode)
				{
					ThePrsList.addNode(stopnode);
					stopnode->setAge(YOUTH);
				}
				continue;
			}
			else break;
		}
		if (!firstsgmt) break;
		/* initial the routelist */
		for (stopnode=firstsgmt->getThisFstNode();stopnode;
			 stopnode=stopnode->getSameFstNode())
		{
			if (get_sect_valid(stopnode->getSrcSect())==FALSE) continue;
			if (stopnode->getAge()!=BABY) continue;
			oversgmt=stopnode->getLstSrcSgmt();
			routelist.addTail((CplNode *)stopnode,NULL);
   			if (!lastsgmt || oversgmt->getEndChar()>lastsgmt->getEndChar())
		   		{ laststepnum=1; lastsgmt=oversgmt; }
			oversgmt->setStepNum(1);
		}
		/* loop through the routelist */
		for (stopstep=routelist.head();stopstep;stopstep=stopstep->getNext())
		{
			stopsgmt=stopstep->getNode()->getSucsSrcSgmt();
			// skip the SrcSgmt with no segmentation ambiguation
			if (!stopsgmt || stopsgmt->getNodeNum()==1) continue; 
			int stepnum=stopstep->getStepNum()+1;
			for (stopnode=stopsgmt->getThisFstNode();stopnode;
				 stopnode=stopnode->getSameFstNode())
			{
				if (get_sect_valid(stopnode->getSrcSect())==FALSE) continue;
				if (stopnode->getAge()!=BABY) continue;
				oversgmt=stopnode->getLstSrcSgmt();
				int oldstepnum=oversgmt->getStepNum();
				if (oldstepnum && stepnum>oldstepnum+STEPNUMBERDELTA) continue;
				routelist.addTail((CplNode *)stopnode,stopstep);
				if (!lastsgmt || oversgmt->getEndChar()>lastsgmt->getEndChar())
					{ laststepnum=stepnum; lastsgmt=oversgmt; }
				if (stepnum<oldstepnum) oversgmt->setStepNum(stepnum);
			}
		}
		/* add the node in the shortest route to the PrsList */
		for (overstep=routelist.head();overstep;overstep=overstep->getNext())
		{
			if (overstep->getNode()->getLstSrcSgmt()!=lastsgmt) continue;
			if (overstep->getStepNum()>laststepnum+STEPNUMBERDELTA) break;
			if (overstep->getStepNum()<laststepnum) continue;
			for (stopstep=overstep;stopstep;stopstep=stopstep->getPrev())
			{
				stopnode=stopstep->getNode();
				if (stopnode->getAge()==BABY)
				{
					ThePrsList.addNode(stopnode);
					stopnode->setAge(YOUTH);
				}
			}
		}
	}
}

void ChnXXXTranslator::disambiguation_statistical()
{
	// find the shortest route and add the node in the route to ThePrsList
	SrcSgmt * bgnsgmt=TheSrcStruct.getSrcSgmtHead();
	SrcSgmt * endsgmt=TheSrcStruct.getSrcSgmtTail();
	SrcSgmt * stopsgmt, * oversgmt, * firstsgmt,  * lastsgmt=NULL;
	PtlNode * stopnode;
	double topscore, curscore, threshold=0.5;
	for (stopsgmt=bgnsgmt;stopsgmt;stopsgmt=stopsgmt->getSucsSgmt())
		{ stopsgmt->setStepNum(0); stopsgmt->setNodeNum(0); }
	// set NodeNum for each SrcSgmt
	for (stopsgmt=bgnsgmt;stopsgmt;stopsgmt=stopsgmt->getSucsSgmt())
	{
		for (stopnode=stopsgmt->getThisFstNode();stopnode;
			 stopnode=stopnode->getSameFstNode())
		{
			if (get_sect_valid(stopnode->getSrcSect())==FALSE ||
				stopnode->getAge()!=BABY) continue;
			oversgmt=stopnode->getLstSrcSgmt();
			char * overchar=oversgmt->getEndChar();
			SrcSgmt * sgmt;
			for (sgmt=stopsgmt;sgmt && sgmt->getEndChar()<=overchar;sgmt=sgmt->getSucsSgmt())
				sgmt->setNodeNum(sgmt->getNodeNum()+1);
		}
	}
	while (lastsgmt!=endsgmt)
	{
		NodeRouteList routelist;
		NodeRouteLink * stopstep, * overstep;
		// find the first SrcSgmt of this session
		for (firstsgmt=lastsgmt?lastsgmt->getSucsSgmt():bgnsgmt;
			 firstsgmt;firstsgmt=firstsgmt->getSucsSgmt())
		{
			// pass throught the sgmt with no ambiguation
			if (firstsgmt->getNodeNum()<=1)
			{
				for (stopnode=firstsgmt->getThisFstNode();stopnode;
					 stopnode=stopnode->getSameFstNode())
				{
					if (get_sect_valid(stopnode->getSrcSect())==TRUE &&
							stopnode->getAge()==BABY) break;
				}
				/* add the node with no ambiguation to the PrsList */
				if (stopnode)
				{
					ThePrsList.addNode(stopnode);
					stopnode->setAge(YOUTH);
				}
				continue;
			}
			else break;
		}
		if (!firstsgmt) break;
		// initial the routelist
		topscore = -1;
		for (stopnode=firstsgmt->getThisFstNode();stopnode;
			 stopnode=stopnode->getSameFstNode())
		{
			if (get_sect_valid(stopnode->getSrcSect())==FALSE) continue;
			if (stopnode->getAge()!=BABY) continue;
			oversgmt=stopnode->getLstSrcSgmt();
			curscore=compute_lex_score_begin((CplNode *)stopnode);
			routelist.addSort((CplNode *)stopnode,NULL,curscore);
   			if (!lastsgmt || oversgmt->getEndChar()>lastsgmt->getEndChar())
		   		{ lastsgmt=oversgmt; }
		}
		// loop through the routelist
		for (stopstep=routelist.head();stopstep;stopstep=stopstep->getNext())
		{
			stopsgmt=stopstep->getNode()->getSucsSrcSgmt();
			if (stopsgmt==NULL || stopsgmt->getThisFstNode()==NULL ||
				stopsgmt->getNodeNum()==1)
			{
				curscore=stopstep->getScore();
				if (topscore<0) topscore=curscore;  // set top score
				else if (curscore<topscore*threshold) break;
				else continue;
			}
			for (stopnode=stopsgmt->getThisFstNode();stopnode;
				 stopnode=stopnode->getSameFstNode())
			{
				if (get_sect_valid(stopnode->getSrcSect())==FALSE) continue;
				if (stopnode->getAge()!=BABY) continue;
				oversgmt=stopnode->getLstSrcSgmt();
				curscore=compute_lex_score((CplNode *)stopnode,stopstep);
				if (stopnode->getSucsSrcSgmt()==NULL ||
					stopnode->getSucsSrcSgmt()->getThisFstNode()==NULL)
					curscore=curscore*compute_lex_score_end((CplNode *)stopnode);
				routelist.addSort((CplNode *)stopnode,stopstep,curscore);
				if (!lastsgmt || oversgmt->getEndChar()>lastsgmt->getEndChar())
					{ lastsgmt=oversgmt; }
			}
		}
		// add the node in the routes with high score to the PrsList
		for (overstep=routelist.head();overstep;overstep=overstep->getNext())
		{
			stopsgmt=overstep->getNode()->getSucsSrcSgmt();
			if (stopsgmt && stopsgmt->getThisFstNode()) continue;
			curscore=overstep->getScore();
			if (curscore<topscore*threshold) break;
			for (stopstep=overstep;stopstep;stopstep=stopstep->getPrev())
			{
				if (stopstep->getScore()==0) continue;
				ThePrsList.addNode(stopstep->getNode());
				stopstep->getNode()->setAge(YOUTH);
				stopstep->setScore(0);
			}
		}
	}
}

double ChnXXXTranslator::compute_lex_score(CplNode * node,NodeRouteLink * step)
{
	double score=step->getScore();
	const char * word=node->get_BaseForm();
	const char * label=node->getSrcLabel();
	const char * prevlabel=step->getNode()->getSrcLabel();
	score *= TheChnLexDbase->getTagTagProb(prevlabel,label);
	score *= TheChnLexDbase->getTagWordProb(label,word);
	return score;
}

double ChnXXXTranslator::compute_lex_score_begin(CplNode * node)
{
	double score;
	const char * word=node->get_BaseForm();
	const char * label=node->getSrcLabel();
	score=TheChnLexDbase->getTagTagProb("^",label)*
		  TheChnLexDbase->getTagWordProb(label,word);
	return score;
}

double ChnXXXTranslator::compute_lex_score_end(CplNode * node)
{
	double score;
	const char * word=node->get_BaseForm();
	const char * label=node->getSrcLabel();
	score=TheChnLexDbase->getTagTagProb(label,"$");
	return score;
}

void ChnXXXTranslator::setSectData(CplNode * node,const char * base,const char * vary)
{
	SectData * information;
	information=(SectData *)node->getSrcSect()->getData();
	if (information==NULL)
	{
		SectData * info=new SectData;
		OToken token(info->yx);
		token.setType(IDENT);
		token.setBuffer(base);
		token.output();
		token.outputNull();
		// strcpy(info->yx,base);

		int i=0;
		const char * p=vary;
		while (*p) { if (isChnChar(p)) { i++; p++; p++; } else break; }
		if (*p) i=0;
		//_itoa(i,info->yj,10);
		sprintf(info->yj,"%d",i);

		strcpy(info->ccat,node->getSrcLabel());

		node->getSrcSect()->setData(info);
	}
	else
	{
		IToken token1(information->ccat); token1.input();
		IToken token2(node->getSrcLabel()); token2.input();
		if (agree_Hierar(token1,token2)) return;
		strcat(information->ccat,"|");
		strcat(information->ccat,node->getSrcLabel());
	}
}

void ChnXXXTranslator::clrSectDataAll()
{
	SrcSgmt * sgmt;
	for (sgmt=TheSrcStruct.getSrcSgmtHead();sgmt;sgmt=sgmt->getSucsSgmt())
	{
		SrcSect * sect;
		for (sect=sgmt->getThisFstSect();sect;sect=sect->getSameFstSect())
		{
			if (sect->getData())
			{
				delete (SectData *)sect->getData();
				sect->setData(NULL);
			}
		}
	}
}

void ChnXXXTranslator::cutSgmt(SrcSgmt * sgmt)
{
	char * text=sgmt->getBgnChar();
	char word[TOKENLENGTH];
	cRuleList * meanlist;
	RuleLink * meanlink;

	meanlist=&TheChnDictn->rd_MeansWord(word,text);
	while (meanlist)
	{
		meanlink=meanlist->getRuleLinkHead();
		while (meanlink)
		{
			IToken tokenm(meanlink->getRule());
			tokenm.input();
			CplNode * node=createNode1(tokenm,word,word);
			adjNode(node,word,word,meanlink->getRule());
			SrcSgmt * endsgmt=sgmt->srhEndSgmt(text+strlen(word));
			node->addIntoSect(sgmt,endsgmt,TheMatchList);
			node->addIntoStruct();
			setSectData(node,word,word);
			meanlink=meanlink->getNext();
		}
		meanlist=&TheChnDictn->rd_MeansWord(word);
	}

	if (UseDictnref==0) meanlist=NULL;
	else meanlist=&TheChnDictnref->rd_MeansWord(word,text);
	while (meanlist)
	{
		meanlink=meanlist->getRuleLinkHead();
		while (meanlink)
		{
			IToken tokenm(meanlink->getRule());
			tokenm.input();
			CplNode * node=createNode1(tokenm,word,word);
			adjNode(node,word,word,meanlink->getRule());
			SrcSgmt * endsgmt=sgmt->srhEndSgmt(text+strlen(word));
			node->addIntoSect(sgmt,endsgmt,TheMatchList);
			node->addIntoStruct();
			setSectData(node,word,word);
			meanlink=meanlink->getNext();
		}
		meanlist=&TheChnDictnref->rd_MeansWord(word);
	}

	if (isChnChar(text))
	{
		cRuleList * ovlrulelist=&TheChnOvlRbase->rd_OvlRules("");
		RuleLink * ovlrulelink;
		for (ovlrulelink=ovlrulelist?ovlrulelist->getRuleLinkHead():NULL;
			 ovlrulelink;ovlrulelink=ovlrulelink->getNext())
		{
			const char * ovlrule=ovlrulelink->getRule();
			char base[TOKENLENGTH],vary[TOKENLENGTH];
			if (cutText(text,ovlrule,base,vary)==FAIL) continue;
			if ((meanlist=&TheChnDictn->rd_Means(base))!=NULL)
			{
				meanlink=meanlist->getRuleLinkHead();
				while(meanlink)
				{
					IToken tokenm(meanlink->getRule());
					tokenm.input();
					CplNode * node=createNode1(tokenm,base,vary);
					node->addIntoSect(sgmt,sgmt->srhEndSgmt(text+strlen(vary)),TheMatchList);
					node->pushUnity();
					useNode(node);
					adjNode(node,base,vary,meanlink->getRule());
					try
					{
						if (cutNode(node,ovlrule)==SUCC)
						{
							node->addIntoStruct();
							setSectData(node,base,vary);
						}
						else delete node;
					}
					catch (...) { delete node; throw; }
					meanlink=meanlink->getNext();
				}
			}
			if (UseDictnref && (meanlist=&TheChnDictnref->rd_Means(base))!=NULL)
			{
				meanlink=meanlist->getRuleLinkHead();
				while(meanlink)
				{
					IToken tokenm(meanlink->getRule());
					tokenm.input();
					CplNode * node=createNode1(tokenm,base,vary);
					node->addIntoSect(sgmt,sgmt->srhEndSgmt(text+strlen(vary)),TheMatchList);
					node->pushUnity();
					useNode(node);
					adjNode(node,base,vary,meanlink->getRule());
					try
					{
						if (cutNode(node,ovlrule)==SUCC)
						{
							node->addIntoStruct();
							setSectData(node,base,vary);
						}
						else delete node;
					}
					catch (...) { delete node; throw; }
					meanlink=meanlink->getNext();
				}
			}
		}
	}
	else
	{
		if (sgmt->getThisFstNode()) return; 
		const char * srccat, * tgtcat;
		if (isPctChar(text))
		{
			srccat=TheModel->getPctCat(TheSrcLang);
			tgtcat=TheModel->getPctCat(TheTgtLang);
		}
		else if (isDgtChar(text))
		{
			srccat=TheModel->getDgtCat(TheSrcLang);
			tgtcat=TheModel->getDgtCat(TheTgtLang);
		}
		else // isOthChar(text)
		{
			srccat=TheModel->getOthCat(TheSrcLang);
			tgtcat=TheModel->getOthCat(TheTgtLang);
		}
		CplNode * node=createNode2(sgmt,sgmt,srccat,tgtcat);
		char * p=word, * q=sgmt->getBgnChar();
		while (q<sgmt->getEndChar()) *p++=*q++;
		*p=0;
		setSectData(node,word,word);
	}
}

int ChnXXXTranslator::cutText(char *text, const char *ovlrule,
							   char * baseform, char * varyform)
{
	IToken token1(ovlrule);
	token1.input();
	token1.input(); // skip @@
	IToken token2=token1;
	token2.goover("<<");
	const char * varypatt=token1.getInPntr();
	const char * basepatt=token2.getInPntr();
	Code lexvar[26];
	int i;
	for (i=0;i<26;i++) lexvar[i]=Code(0,0);
	while (TRUE)
	{
		Code varypattchar=getCodeNext(varypatt);
		Code varyformchar=getCodeNext(text);
		if (isSglByte(varypattchar) && varypattchar.LoByte>='A' && varypattchar.LoByte<='Z')
		{
			if (varyformchar==Code(0,0)) return FAIL;
			int index=varypattchar.LoByte-'A';
			if (lexvar[index]==Code(0,0)) lexvar[index]=varyformchar;
			else if (lexvar[index]!=varyformchar) return FAIL;
		}
		else if (isChnChar(varypattchar))
			{ if (varypattchar!=varyformchar) return FAIL; }
		else break;
		setCharNext(varyform,varyformchar);
	}
	*varyform=0;
	while (TRUE)
	{
		Code basepattchar=getCodeNext(basepatt);
		Code baseformchar;
		if (isSglByte(basepattchar) && basepattchar.LoByte>='A' && basepattchar.LoByte<='Z')
			baseformchar=lexvar[basepattchar.LoByte-'A'];
		else if (isChnChar(basepattchar))
			baseformchar=basepattchar;
		else break;
		if (baseformchar==Code(0,0))
			error("ChnXXXTranslator::cutText(): undefined LexVar in ovlrule.",ovlrule);
		setCharNext(baseform,baseformchar);
	}
	*baseform=0;
	return SUCC;
}

int ChnXXXTranslator::cutNode(CplNode *node, const char * ovlrule)
{
	IToken token1(ovlrule);
	token1.goover("--");
	IToken token2=token1;
	token2.goover("--");
	if (!token2.isIdent(node->getSrcLabel())) return FAIL;
	token2.input();
	if (unifyNode(node,token2)==FAIL) return FAIL;
	if (unifySrcLabel(node,token1)==FAIL) return FAIL;
	if (unifyNode(node,token1)==FAIL) return FAIL;
	return SUCC;
}

void ChnXXXTranslator::adjNode(CplNode * node, const char * baseform, const char * varyform,const char * meaning)
{
	char buffer[20*TOKENLENGTH];
	char * fest=buffer;
	*fest++='[';
	addYinjie(fest,varyform);
	*fest++=',';
	addHanzi(fest,baseform);
	//*fest++=',';
	//addSrcJianlei(fest,get_sect_ccat(node->getSrcSect()));
	//*fest++=',';
	//addTgtJianlei(fest,meaning);
	*fest++=']';
	*fest++=0;
	IToken token(buffer); token.input();
	if (unifyNode(node,token)==FAIL)
		error("ChnXXXTranslator::adjNode(): set 音节 or 汉字 or 兼类 error.", varyform);
}

void ChnXXXTranslator::addYinjie(char * &fest, const char * varyform)
{
	char * fest2=Yinjie;
	while (*fest2) *fest++=*fest2++; *fest++=':';
	int i=0;
	const char * p=varyform;
	while (*p) { if (isChnChar(p)) { i++; p++; p++; } else break; }
	if (*p) i=0;
	*fest++='0'+i;
}

void ChnXXXTranslator::addHanzi(char * &fest, const char * baseform)
{
	char * fest2=Hanzi;
	while (*fest2) *fest++=*fest2++; *fest++=':';
	char * fest1=fest;
	const char * p=baseform;
	int flag=0;
	while (*p)
	{
		if (isChnChar(p))
			{ if (flag) *fest++='|'; else flag=1; *fest++=*p++; *fest++=*p++; }
		else goNext(p);
	}
	if (fest==fest1) for (p="NONE";*p;*fest++=*p++);
}
/*
void ChnXXXTranslator::addSrcJianlei(char * &fest, const char * label)
{
	char * fest2=SrcJianlei;
	while (*fest2) *fest++=*fest2++; *fest++=':';
	int i=0;
	const char * p=label;
	while (*p) { if (isChnChar(p)) { i++; p++; p++; } else break; }
	if (*p) i=0;
	*fest++='0'+i;
}

void ChnXXXTranslator::addTgtJianlei(char * &fest, const char * meaning)
{
	char * fest1=fest;
	char * fest2=TgtJianlei;
	while (*fest2) *fest1++=*fest2++; *fest1++=':'; *fest1=0;
	IToken token(meaning); token.input();
	token.goover("=>");
	while (*token)
	{
		strcat(fest,token);
		token.goover("=>");
	}
}

int ChnXXXTranslator::isLegalSrcSect(SrcSect * sect)
{
	int Legal;
	SrcSgmt * fstsgmt=sect->getFstSrcSgmt();
	SrcSgmt * lstsgmt=sect->getLstSrcSgmt();
	SrcSgmt * prevsgmt=fstsgmt->getPrevSgmt();
	SrcSgmt * sucssgmt=lstsgmt->getSucsSgmt();
	if (fstsgmt==NULL || lstsgmt==NULL)
		error("ChnXXXTranslator::isLegalSrcSect(): FstSrcSgmt or LstSrcSgmt is NULL.");
	enum { NOPUNCT,ATPUNCT,BYPUNCT } left,right,middle;
	if (fstsgmt->getSgmtType()==PCTCHAR) left=ATPUNCT;
	else if (prevsgmt==NULL || prevsgmt->getSgmtType()==PCTCHAR)
		left=BYPUNCT;
	else left=NOPUNCT;
	if (lstsgmt->getSgmtType()==PCTCHAR) right=ATPUNCT;
	else if (sucssgmt==NULL || sucssgmt->getSgmtType()==PCTCHAR)
		right=BYPUNCT;
	else right=NOPUNCT;
	middle=NOPUNCT;
	SrcSgmt * sgmt;
	if (fstsgmt!=lstsgmt)
	{
		for (sgmt=fstsgmt->getSucsSgmt();sgmt!=lstsgmt;sgmt=sgmt->getSucsSgmt())
			if (sgmt->getSgmtType()==PCTCHAR && !isEqual(sgmt->getBgnChar(),"、"))
				{ middle=ATPUNCT; break; }
	}
	if (middle==NOPUNCT) Legal=TRUE;
	else if (left==BYPUNCT && right==BYPUNCT) Legal=TRUE;
	else if (left==ATPUNCT && right==ATPUNCT) Legal=TRUE;
	else if (left==BYPUNCT && right==ATPUNCT) Legal=TRUE;
	else Legal=FALSE;
	return Legal;
}
*/

void XXXEngTranslator::bldWord1()
{
	if (Stage!=BLDWORD1) return;
	if (Direction==RETRIEVE) { bldWordBack1(); return; }

	TgtSgmt * sgmt;
	CplNode * node;
	sgmt=TheTgtStruct.getTgtSgmtHead();
	while (sgmt && sgmt->getNode() && sgmt->getNode()->hasType()==SHIFTTO)
		sgmt=sgmt->getSucsSgmt();
	node=sgmt->getNode();
	sgmt->set_FormScan();
	while (TRUE)
	{
		if (node==NULL)
		{
			do { sgmt=sgmt->getSucsSgmt(); }
			while (sgmt && sgmt->getNode() && sgmt->getNode()->hasType()==SHIFTTO);
			if (!sgmt) break;
			node=sgmt->getNode();
			sgmt->set_FormScan();
		}
		else if (sgmt->set_BaseForm())
		{
			pushNodeUnity(node);
			const char * label=node->getTgtLabel();
			cRuleList * rulelist=&TheEngBldRbase->rd_BldRules(label);
			RuleLink * rulelink=rulelist?rulelist->getRuleLinkHead():NULL;
			if (!strcmp(sgmt->get_BaseForm()," ") ||
				!strcmp(sgmt->get_BaseForm(),"_") ) rulelink=NULL;
			while (rulelink)
			{
				if (bldSgmt(sgmt,rulelink->getRule())) break; /* use top Unity */
				rulelink=rulelink->getNext();
			}
			if (!rulelink) sgmt->set_VaryForm(sgmt->get_BaseForm());
#ifdef KBMS
			else output_bldNode(sgmt,rulelink->getRule());
#endif
			do { sgmt=sgmt->getSucsSgmt(); }
			while (sgmt && sgmt->getNode() && sgmt->getNode()->hasType()==SHIFTTO);
			if (!sgmt) break;
			node=sgmt->getNode();
			sgmt->set_FormScan();
		}
		else
		{
			do { sgmt=sgmt->getPrevSgmt(); }
			while (sgmt && sgmt->getNode() && sgmt->getNode()->hasType()==SHIFTTO);
			if (!sgmt) { bldWordBack1(); return; }
			node=sgmt->getNode();
			popNodeUnity();
		}
	}

	char * text=TargetSentence;
	for(sgmt=TheTgtStruct.getTgtSgmtHead();sgmt;sgmt=sgmt->getSucsSgmt())
	{
		node=sgmt->getNode();
		if (node==NULL)
		{
			bldText(sgmt,text);
			continue;
		}
		else if (node->getPresent()==NULL) continue;
		else if (node->hasType()==COMPLETE) bldText(sgmt,text);
		else
		{
			CplNode * sftfrom=((SftNode *)node)->getSftFrom();
			while (sftfrom->hasType()==SHIFTTO)
				sftfrom=((SftNode *)sftfrom)->getSftFrom();
			TgtSgmt * sftsgmt;
			TgtSgmt * fstsgmt=sftfrom->getFstTgtSgmt();
			TgtSgmt * lstsgmt=sftfrom->getLstTgtSgmt()->getSucsSgmt();
			if (!fstsgmt) { bldWordBack1(); return; }
			for (sftsgmt=fstsgmt;sftsgmt!=lstsgmt;sftsgmt=sftsgmt->getSucsSgmt())
				bldText(sftsgmt,text);
		}
	}
	*text++='\0';
	Direction=PROGRESS;
}

void XXXEngTranslator::bldWordBack1()
{
	if (Stage!=BLDWORD1) return;
	*TargetSentence=0;
	TgtSgmt * sgmt;
	for (sgmt=TheTgtStruct.getTgtSgmtTail();sgmt;sgmt=sgmt->getPrevSgmt())
		if (sgmt->getNode() && sgmt->getNode()->hasType()!=SHIFTTO) popNodeUnity();
	Direction=RETRIEVE;
}

// use the bldrule to the sgmt
int XXXEngTranslator::bldSgmt(TgtSgmt * sgmt,const char * bldrule)
{
	IToken token(bldrule);
	token.input();
	token.input();			// skip <<
	const char * basepatt=token.getInPntr();
	char * baseform=sgmt->get_BaseForm();
	char lexvar[TOKENLENGTH];

	if (*basepatt=='*')
	{
		basepatt++;
		char * baseformpntr=baseform;
		const char * basepattpntr=basepatt;
		int basepattleng=0;
		int baseformleng=0;
		while (*baseformpntr) { baseformleng++; goNext(baseformpntr); }
		while (isLtnChar(basepattpntr)) { basepattleng++; goNext(basepattpntr); }
		if (basepattleng>baseformleng) return FAIL;
		int i;
		char * lexvarpntr=lexvar;
		for (i=0;i<baseformleng-basepattleng;i++) setCharNext(lexvarpntr,baseform);
		*lexvarpntr=0;
	}
	else *lexvar=0;
	while (isLtnChar(basepatt) && isEqual(basepatt,baseform))
		{ goNext(basepatt); goNext(baseform); }
	if (isLtnChar(basepatt) || isLtnChar(baseform)) return FAIL;

	token.goover("--");
	CplNode * node=sgmt->getNode();
	if (!token.isIdent(node->getTgtLabel())) return FAIL;
	token.input();
	if (unifyNode(node,token)==FAIL) return FAIL;

	token.goover(">>");
	const char * varypatt=token.getInPntr();
	char * varyform=sgmt->get_VaryForm();
	if (token.isDelim("{"))
	{
		token.input();
		Attribute * attrb=TheModel->getAttribute(token);
		Atom * value=getIntValue(node,attrb);
		if (value->getNucleus()) strcpy(varyform,value->getNucleus());
		else strcpy(varyform,sgmt->get_BaseForm());
	}
	else
	{
		if (*varypatt=='*')
		{
			char * lexvarpntr=lexvar;
			while (*lexvarpntr) setCharNext(varyform,lexvarpntr);
			varypatt++;
		}
		while (isLtnChar(varypatt)) setCharNext(varyform,varypatt);
		*varyform=0;
	}

	return SUCC;
}

// make small changes to the text of the sgme
void XXXEngTranslator::bldText(TgtSgmt * sgmt,char * &text)
{
	const char * form=sgmt->get_VaryForm();
	if (strcmp(form," ") && strcmp(form,"_"))
	{
		if (text!=TargetSentence)
		{
			if (*(text-1)=='.' && !strcmp(form,".")) *(text-1)=' ';
			if (!isPctChar(form)) *text++=' ';
		}
		while (*form) *text++=*form++;
	}
	if (sgmt->getNode()==NULL) return;
	if (!strcmp(sgmt->getNode()->getTgtLabel(),"T") &&
		!strcmp(sgmt->get_VaryForm(),"a") )
	{
		TgtSgmt * sucssgmt=sgmt->getSucsSgmt();
		if (sucssgmt)
		{
			const char * sucsform=sucssgmt->get_VaryForm();
			if (*sucsform=='a' || *sucsform=='A' ||
				*sucsform=='e' || *sucsform=='E' ||
				*sucsform=='i' || *sucsform=='I' ||
				*sucsform=='o' || *sucsform=='O' ||
				*sucsform=='u' && strncmp(sucsform,"uni",3)!=0 ||
				*sucsform=='U' && strncmp(sucsform,"Uni",3)!=0 ||
				strncmp(sucsform,"hour",4)==0 ||
				strncmp(sucsform,"hono",4)==0   )
				*text++='n';
		}
	}
	if (!strcmp(sgmt->getNode()->getTgtLabel(),"N") &&
		!strcmp(sgmt->get_VaryForm(),"'s") )
	{
		TgtSgmt * prevsgmt=sgmt->getPrevSgmt();
		if (prevsgmt)
		{
			const char * prevform=prevsgmt->get_VaryForm();
			while (*prevform) prevform++;
			prevform--;
			if (*prevform=='s' || *prevform=='z' ||
				(*prevform=='e' && *(prevform-1)=='s') ||
				(*prevform=='e' && *(prevform-1)=='z') )
				text--;
		}
	}
}

/* remove the single Chinese characters from the TargetSentence text */
void XXXEngTranslator::postProcess(char * text)
{
	char * p=text;
	while (*p) p++;
	char endchar=*(p-1);
	if (endchar=='.' || endchar=='?' || endchar=='!')
		*text=toupper(*text);
}

#ifdef KBMS

void ChnXXXTranslator::output_nopathfind()
{
	WchFile << "切分规则过强，导致无可选路径，改用双向最大匹配算法。" << endl << endl;
}

void ChnXXXTranslator::output_segmentation(SegAmbiguation & amb, const char * rule, const char * pntr1, const char * pntr2)
{
	if (!WchFile.is_open()) return;
	WchFile << "字段：" << amb.GetSegPattern() << endl;
	if (rule)
	{
		WchFile << "使用：切分排歧规则：" << rule << endl;
		WchFile << "使用：分支：";
		const char * p=pntr1;
		for (p=pntr1;p<pntr2;p++) if (*p!='\r' && *p!='\n') WchFile << *p;
		WchFile << endl;
	}
	else WchFile << "使用：无切分规则可用，选用较短分支。" << endl;
	WchFile << "结果：";
	if (amb.GetValidate1()) WchFile << amb.GetSegSelect1() << "  ";
	if (amb.GetValidate2()) WchFile << amb.GetSegSelect2() << "  ";
	WchFile << endl << endl;
}

void ChnXXXTranslator::output_tagging(TagAmbiguation & amb, const char * rule, const char * pntr1, const char * pntr2)
{
	if (!WchFile.is_open()) return;
	WchFile << "词语：" << LexAmbiguation::get_sect_yx(amb.GetSect()) << endl;
	WchFile << "使用：标注排歧规则：" << rule << endl;
	WchFile << "使用：分支：";
	const char * p=pntr1;
	for (p=pntr1;p<pntr2;p++) if (*p!='\r' && *p!='\n') WchFile << *p;
	WchFile << endl;
	WchFile << "结果：" << LexAmbiguation::get_sect_ccat(amb.GetSect()) << endl;
	WchFile << endl;
}

#endif
