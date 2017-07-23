#ifdef WIN32
#include <direct.h>
#include <io.h>
#endif
#include "prsrbase.h"

void PrsRbase::rdCfg()
{
	MKbase::rdCfg();
	rdSrcLang();
	rdTgtLang();
}

void PrsRbase::testCfg()
{
  //	MKbase::testCfg();
  //	if( SrcLang!=hasSrcLang() ) error("PrsRbase::testCfg(): SRCLANG" );
}

void PrsRbase::wtCfg()
{
	MKbase::wtCfg();
	wtSrcLang();
	wtTgtLang();
}

void PrsRbase::initCfg()
{
	MKbase::initCfg();
	SrcLang=hasSrcLang();
	TgtLang=getTgtLang();
	if( TgtLang==SrcLang ) 
		error("PrsRbase::initCfg(): TgtLang is the same as SrcLang.");
	if( mkModel->hasLang(SrcLang)==FALSE ) 
		error("PrsRbase::initCfg()", "Wrong: SrcLang outside Model." );
	if( mkModel->hasLang(TgtLang)==FALSE ) 
		error("PrsRbase::initCfg()", "Wrong: TgtLang outside Model." );
}

void PrsRbase::rdSrcLang()
{
	if( !CfgFile ) error("PrsRbase::rdSrcLang(): CFGOPEN" );
	CfgFile.seekg(0,ios::beg);
	char line[256];
	char *p;
	while(CfgFile.peek()!=EOF){
		CfgFile.getline(line,255);
		if( strstr(line,"SrcLang") ){
			p=strstr(line,"=");
			p++;
			int i;
			for( i=0; i<2; i++ ) if( !strcmp(LANGSTRING[i],p) ) break;
			SrcLang=i;
			return;
		}
	}
}

void PrsRbase::rdTgtLang()
{
    if( !CfgFile ) error("PrsRbase::rdTgtLang(): CFGOPEN" ); 
	CfgFile.seekg(0,ios::beg); 
	char line[256];
    char *p;
    while(CfgFile.peek()!=EOF){
        CfgFile.getline(line,255);
        if( strstr(line,"TgtLang") ){
            p=strstr(line,"=");
            p++;
            int i;
            for( i=0; i<2; i++ ) if( !strcmp(LANGSTRING[i],p) ) break;
            TgtLang=i;
            return;
        }
    }
}

void PrsRbase::wtSrcLang()
{
    if( !CfgFile ) error("PrsRbase::wtSrcLang(): CFGOPEN" );
    CfgFile.seekp(0,ios::end);
    CfgFile<<"SrcLang="<<LANGSTRING[SrcLang]<<endl;
}

void PrsRbase::wtTgtLang()
{
	if( !CfgFile ) error("PrsRbase::wtTgtLang(): CFGOPEN" );
	CfgFile.seekp(0,ios::end);
	CfgFile<<"TgtLang="<<LANGSTRING[TgtLang]<<endl;
}


#ifdef KBMS
BOOL PrsRbase::rdTxtPrsRule()
{
	return rdTxtRecord("&&");
}

#endif // KBMS

void PrsRbase::build()
{
	if( getBuilt()==YES) return;
	ropen();

	// read all the rules from the open PrsRbase to the ptidxbuff(sorted)
	beginScan(MAXBUFFER);
	try
	{
		int n=0; /* the number of the patterns */
		while( DatFile.peek()!=EOF )
		{
			int length;
			length=rdDatLength();

			if (length<0)
			{
				length=-length;
				DatFile.seekg(length,ios::cur);
			}
			else
			{
				if (DatFile.peek()==EOF) break;
				rdDatBuff(length);

				char * end=DatBuff+length-1;
				while (isascii(*end) && isspace(*end)) end--;
				*++end=0;

				char pattn[PATTNLENGTH+1];
				getPattn(DatBuff,pattn);

				PTIdxBuff.addUnitHead(pattn,DatBuff);
			}
		}
	}
	catch (CErrorException *) { endScan(); close(); throw; }

	endScan();
	close();
	setBuilt(YES);
	MKbase::build();
}

void PrsRbase::free()
{
	if( getBuilt()==NO) return;
	PTIdxBuff.free();
	MKbase::free();
}

void PrsRbase::ropen()
{
	Kbase::ropen();
}

#ifdef KBMS
void PrsRbase::wopen()
{
	Kbase::wopen();
}
#endif // KBMS

void PrsRbase::close()
{
	Kbase::close();
}

#ifdef KBMS

void PrsRbase::Load()
{
	wopen();
	beginScan(MAXBUFFER);

	char textfilename[FILENAMELENGTH];
	getFullName(textfilename,".TXT");
	TxtFile.open(textfilename,ios::in|ios::binary);
	if( !TxtFile ) error("PrsRbase::Load(): TXTOPEN",textfilename);

	DatFile.close();

	char datafilename[FILENAMELENGTH];
	getFullName(datafilename,".DAT");
	DatFile.open(datafilename,ios::out|ios::binary|ios::trunc);
	if( !DatFile ) error("PrsRbase::Load(): DATOPEN",datafilename);

	DatFile.seekp(0,ios::beg);

	TitleList newtitlelist;
	CErrorException * err=NULL;
	try
	{
		BOOL flag=TRUE;
		IOToken TheToken;
		while( flag=rdTxtPrsRule() ){
			TheToken.setInPntr(TxtBuff);
			TheToken.setOutPntr(DatBuff);
			int i;
			try
			{
				i=LoadPrsRule(TheToken,TgtLang,SrcLang,newtitlelist);
			}
			catch (CErrorException *)
			{
				message("PrsRule:",TxtBuff);
				throw;
			}
			wtDatLength(i);
			wtDatBuff();
		}
	}
	catch (CErrorException * e) { err=e; }

	newtitlelist.free();
	TxtFile.close();

	endScan();
	close();

	if (err) throw err;
}

#endif // KBMS

void PrsRbase::getTitle(const char *prsrule,char title[])
{
	IToken token(prsrule);
	token.input();
	token.input();
	token.input();								// read title
	strncpy(title,token,TITLELENGTH);
	title[TITLELENGTH]='\0';
}

void PrsRbase::getPattn(const char *prsrule,char pattn[])
{
	IToken token(prsrule);
	token.goover("->");
	if (token.isDelim("^")) token.input();
	if (token.isDelim("!")) token.input();
	strncpy(pattn,token,PATTNLENGTH);
	pattn[PATTNLENGTH]='\0';
}

cRuleList & PrsRbase::rd_PrsRulesPT(const char * pattn)
{
	PTIdxBuffUnit *tbp=PTIdxBuff.srhUnit(pattn);
	if (tbp) return tbp->getRuleList();
	else return *(cRuleList *)NULL;
}

const char * PrsRbase::rd_PrsRulesTT(const char * title)
{
	PTIdxBuffUnit * buffunit;
	for (buffunit=PTIdxBuff.fstUnit();buffunit;buffunit=buffunit->getNext())
	{
		cRuleList * rulelist=&buffunit->getRuleList();
		RuleLink * rulelink;
		for (rulelink=rulelist->getRuleLinkHead();rulelink;
			 rulelink=rulelink->getNext())
		{
			const char * rule=rulelink->getRule();
			char ruletitle[TITLELENGTH];
			PrsRbase::getTitle(rule,ruletitle);
			if (!strcmp(title,ruletitle)) return rule;
		}
	}
	return NULL;
}
