#ifdef WIN32
#include <direct.h>
#include <io.h>
#endif
#include "cutrbase.h"
void CutRbase::rdSrcLang()
{
	if( !CfgFile.is_open() ) error("CutRbase::rdSrcLang(): CfgFile open fail" );
	CfgFile.seekg(0,ios::beg);
	char line[256],*p;
	while( CfgFile.peek()!=EOF )
	{
		CfgFile.getline(line,255);
		if( strstr(line,"SrcLang") )
		{
			p=strstr(line,"=");
			p++;
			int i;
			for( i=0; i<2; i++ ) if( !strcmp(LANGSTRING[i],p) ) break;
			SrcLang=i;
			return;
		}
	}
}

void CutRbase::wtSrcLang()
{
	if( !CfgFile ) error("CutRbase::wtSrcLang(): CfgFile open fail" );
	CfgFile.seekp(0,ios::end);
	CfgFile << "SrcLang=" << LANGSTRING[SrcLang] << endl;
}

void CutRbase::initCfg()
{
	MKbase::initCfg();
	SrcLang=hasSrcLang();
	if( mkModel->hasLang(SrcLang)==FALSE ) error("CutRbase::initCfg(): SrcLang not exist." );
}

void CutRbase::rdCfg()
{
	MKbase::rdCfg();
	rdSrcLang();
}

void CutRbase::wtCfg()
{
	MKbase::wtCfg();
	wtSrcLang();
}

void CutRbase::testCfg()
{
  //	MKbase::testCfg();
  //	if( SrcLang!=hasSrcLang() ) error("CutRbase::testCfg(): SrcLang error" );
}

void CutRbase::ropen()
{
	Kbase::ropen();
}

#ifdef KBMS
void CutRbase::wopen()
{
	Kbase::wopen();
}
#endif // KBMS

void CutRbase::close()
{
	Kbase::close();
}

void CutRbase::build()
{
	if( getBuilt()==YES ) return;
	ropen();
	beginScan(MAXBUFFER);
	try
	{
		while (DatFile.peek()!=EOF)
		{
			int length;
			length=rdDatLength();
			rdDatBuff(length);

			char * end=DatBuff+length-1;
			while (isascii(*end) && isspace(*end)) end--;
			*++end=0;

			const char * key="";
			CutRuleList.addUnitTail(key,DatBuff);
		}
	}
	catch (CErrorException * ) { endScan(); close(); throw; }
	endScan();
	close();
	setBuilt(YES);
	MKbase::build();
}

void CutRbase::free()
{
	if( getBuilt()==NO ) return;
	CutRuleList.free();
	MKbase::free();
}

cRuleList & CutRbase::rd_CutRules(const char *key)
{
	PTIdxBuffUnit *crp=CutRuleList.srhUnit(key);
	if( crp ) return crp->getRuleList();
	else return *(cRuleList*)NULL;
}

#ifdef KBMS
void CutRbase::Load()
{
	wopen();
	beginScan(MAXBUFFER);

	char txtfilename[FILENAMELENGTH];
	getFullName(txtfilename,".TXT");
	TxtFile.open(txtfilename,ios::in|ios::binary);

	CErrorException * err=NULL;
	try
	{
		int i;
		BOOL flag=TRUE;
		IOToken TheToken;
		while( flag=rdTxtCutRule() )
		{
			TheToken.setInPntr(TxtBuff);
			TheToken.setOutPntr(DatBuff);
			try
			{
				i=LoadCutRule(TheToken,SrcLang);
			}
			catch (CErrorException *)
			{
				message("CutRule:",TxtBuff);
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
#endif // KBMS
