#ifdef WIN32
#include <direct.h>
#include <io.h>
#endif
#include "makrbase.h"

void MakRbase::initCfg()
{
	MKbase::initCfg();
	TgtLang=hasTgtLang();
	if( mkModel->hasLang(TgtLang)==FALSE ) 
		error("MakRbase::initCfg(): TGTLANG NOT EXIST " );
}

void MakRbase::rdCfg()
{
	MKbase::rdCfg();
	rdTgtLang();
}

void MakRbase::wtCfg()
{
	MKbase::wtCfg();
	wtTgtLang();
}

void MakRbase::testCfg()
{
  //	MKbase::testCfg();
  //	if( TgtLang!=hasTgtLang() ) error("MakRbase::testCfg(): TGTLANG" );
}

void MakRbase::rdSrcLang()
{
    if( !CfgFile ) error("MakRbase::rdSrcLang(): CFGFILE CANNOT OPEN" );
    CfgFile.seekg(0,ios::beg);
    char Line[256];
    char *p;
    while( CfgFile.peek()!=EOF )
    {
        CfgFile.getline(Line,255);
        if( strstr(Line,"SrcLang") )
        {
            p=strstr(Line,"=");
            p++;
            int i;
            for( i=0; i<2; i++ ) if( !strcmp(LANGSTRING[i],p) ) break;
            SrcLang=i;
            return;
        }
    }
}

void MakRbase::rdTgtLang()
{
	if( !CfgFile ) error("MakRbase::rdTgtLang(): CFGFILE CANNOT OPEN" );
	CfgFile.seekg(0,ios::beg);
	char Line[256];
	char *p;
	while( CfgFile.peek()!=EOF )
	{
		CfgFile.getline(Line,255);
		if( strstr(Line,"TgtLang") )
		{
			p=strstr(Line,"=");
			p++;
			int i;
			for( i=0; i<2; i++ ) if( !strcmp(LANGSTRING[i],p) ) break;
			TgtLang=i;
			return;
		}
	}
}

void MakRbase::wtSrcLang()
{
	if( !CfgFile ) error("MakRbase::wtSrcLang(): CFGFILE CANNOT OPEN" );
	CfgFile.seekp(0,ios::end);
	CfgFile << "SrcLang=" << LANGSTRING[SrcLang] << endl;
}

void MakRbase::wtTgtLang()
{
	if( !CfgFile ) error("MakRbase::wtTgtLang(): CFGFILE CANNOT OPEN" );
	CfgFile.seekp(0,ios::end);
	CfgFile << "TgtLang=" << LANGSTRING[TgtLang] << endl;
}

void MakRbase::ropen()
{
	Kbase::ropen();
}

#ifdef KBMS
void MakRbase::wopen()
{
	Kbase::wopen();
}
#endif

void MakRbase::close()
{
	Kbase::close();
}

#ifdef KBMS
BOOL MakRbase::rdTxtMakRule()
{
	return rdTxtRecord("##");
}
#endif
	
void MakRbase::build()
{
	if( getBuilt()==YES) return;
	ropen();
	beginScan( MAXBUFFER );
	try
	{
		while( DatFile.peek()!=EOF )
		{
			int length;
			length=rdDatLength();
			rdDatBuff( length );
			
			char *end=DatBuff+length-1;
			while( isascii(*end) && isspace(*end) ) end--;
			*++end=0;

			char pattn[PATTNLENGTH+1];
			getPattn(DatBuff,pattn);

			MakRuleList.addUnitTail( pattn,DatBuff );
		}
	}
	catch (CErrorException *) { endScan(); close(); throw; }

	endScan();
	close();
	setBuilt(YES);
	MKbase::build();
}

void MakRbase::free()
{
	if (getBuilt()==NO) return;
	MakRuleList.free();
	MKbase::free();
}

#ifdef KBMS

void MakRbase::Load()
{
	wopen();
	beginScan( MAXBUFFER );

	char textfilename[FILENAMELENGTH];
	getFullName(textfilename,".TXT");
	TxtFile.open( textfilename,ios::in|ios::binary);
	
	CErrorException * err=NULL;
	try
	{
		int i;
		BOOL makflag;
		IOToken TheToken;
		makflag=TRUE;
		while( makflag=rdTxtMakRule() )
		{
			TitleList newtitlelist;
			TheToken.setInPntr(TxtBuff);
			TheToken.setOutPntr(DatBuff);
			try
			{
				i=LoadMakRule(TheToken,TgtLang,newtitlelist);
			}
			catch (CErrorException *)
			{
				message("MakRule:",TxtBuff);
				throw;
			}
			wtDatLength(i);
			wtDatBuff();
			newtitlelist.free();
		}
	}
	catch (CErrorException *e) { err=e; }
	
	TxtFile.close();

	endScan();
	close();
	if (err) throw err;
}
#endif

void MakRbase::getPattn(const char *makrule,char pattn[])
{
	IToken token(makrule);
	token.goover("}");
	strncpy( pattn,token,PATTNLENGTH );
	pattn[PATTNLENGTH]='\0';
}

cRuleList & MakRbase::rd_MakRulesPT(const char *pattn)
{
	PTIdxBuffUnit *mrp=MakRuleList.srhUnit( pattn );
	if( mrp ) return mrp->getRuleList();
	else return *(cRuleList *)NULL;
}



	
	
	
	
