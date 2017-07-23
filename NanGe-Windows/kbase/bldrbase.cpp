#ifdef WIN32
#include <direct.h>
#include <io.h>
#endif
#include "bldrbase.h"
void BldRbase::initCfg()
{
	MKbase::initCfg();
	TgtLang=hasTgtLang();
	if( mkModel->hasLang(TgtLang)==FALSE ) 
		error("BldRbase::initCfg(): TGTLANG" );
}

void BldRbase::rdCfg()
{
	MKbase::rdCfg();
	rdTgtLang();
}

void BldRbase::wtCfg()
{
	MKbase::wtCfg();
	wtTgtLang();
}

void BldRbase::testCfg()
{
  //	MKbase::testCfg();
  //	if( TgtLang!=hasTgtLang() ) error("BldRbase::testCfg(): TGTLANG" );
}

void BldRbase::rdTgtLang()
{
	if( !CfgFile ) error("BldRbase::rdTgtLang(): CFGFILE CANNOT OPEN " );
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

void BldRbase::wtTgtLang()
{
	if( !CfgFile ) error("BldRbase::wtTgtLang(): CFGFILE CANNOT OPEN" );
	CfgFile.seekp(0,ios::end);
	CfgFile << "TgtLang=" << LANGSTRING[TgtLang] << endl;
}

void BldRbase::ropen()
{
	Kbase::ropen();
}

#ifdef KBMS
void BldRbase::wopen()
{
	Kbase::wopen();
}
#endif

void BldRbase::close()
{
	Kbase::close();
}

#ifdef KBMS
BOOL BldRbase::rdTxtBldRule()
{
	return rdTxtRecord("@@");
}
#endif

void BldRbase::build()
{
	if( getBuilt()==YES) return;
	ropen();

	// read all bldrule from open BldRbase to the BldRuleList
	beginScan(MAXBUFFER);
	try
	{
		while( DatFile.peek()!=EOF )
		{
			int length;
			length=rdDatLength();
			rdDatBuff(length);

			char *end=DatBuff+length-1;
			while( isascii(*end) && isspace(*end) ) end--;
			*++end=0;

			char key[KEYLENGTH+1];
			static char lastkey[KEYLENGTH+1];
			getKey( DatBuff,key );
			if (*key) BldRuleList.addUnitTail( key,DatBuff ); 
			else BldRuleList.addUnitTail(lastkey,DatBuff);
			strcpy(lastkey,key);
		}
	}
	catch (CErrorException *) { endScan(); close(); throw; }

	endScan();
	close();
	setBuilt(YES);
	MKbase::build();
}

void BldRbase::free()
{
	if( getBuilt()==NO) return;
	BldRuleList.free();
	MKbase::free();
}

void BldRbase::getKey(const char *bldrule,char key[])
{
	IToken token( bldrule );
	token.goover( "--" );
	strncpy( key,token,KEYLENGTH );
	key[KEYLENGTH]='\0';
}
	
#ifdef KBMS

void BldRbase::Load()
{
	wopen();
	beginScan(MAXBUFFER);
	
	char textfilename[FILENAMELENGTH];
	getFullName(textfilename,".TXT");
	TxtFile.open(textfilename,ios::in|ios::binary);

	CErrorException * err=NULL;
	try
	{
		int i;
		BOOL bldflag;
		bldflag=TRUE;
		IOToken TheToken;
		while( bldflag=rdTxtBldRule() )
		{
			TheToken.setInPntr(TxtBuff);
			TheToken.setOutPntr(DatBuff);
			try
			{
				i=LoadBldRule(TheToken,TgtLang);
			}
			catch (CErrorException *)
			{
				message("BldRule:",TxtBuff);
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

	if (err) throw err;
}
	
#endif

cRuleList & BldRbase::rd_BldRules(const char *key)
{
	PTIdxBuffUnit *brp=BldRuleList.srhUnit( key );
	if( brp ) return brp->getRuleList();
	else return *(cRuleList *)NULL;
}

