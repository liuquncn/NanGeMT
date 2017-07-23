//#include <io.h>
#include <stdio.h>
#include <signal.h>
#include <iostream>
//#include <direct.h>
#include "kbase.h"
#include "../trans/unify.h"

char Kbase::Home[FILENAMELENGTH];
#ifdef KBMS
BOOL Kbase::MultiUser;
#endif
/****************************************************************
*   Date:             1994.5.20                                 *
*   In parameters:    none                                      *
*   Return Value:     none                                      *
*   Descriptions:     a set of function concerning with config  *
*                     information in config file.including:     *
*                     read config from .CFG                     *
*                     write config to .CFG                      *
*                     test config                               *
*                     initialize config.                        *
*****************************************************************/

/*  The config file is a text file, but it can not edited by the users. It
	can only be written by the program.  Each line of the file consists of
	a attribute-value pairs.  The attributes and values are defined in
	the following:
	# Type: the Type of the knowledgebase. It is one of the following:
			MODEL:      for language model
			CUTRBASE:   for word-cutting rulebase
			PRSRBASE:   for sentence-parsing rulebase
			MAKRBASE:   for sentence-making rulebase
			BLDRBASE:   for word-building rulebase
			DICTN:      for dictionary
	The derived classes of Kbase may have additional variables in config file.
*/
 
void Kbase::testSituation()
{
	if (getBuilt()==YES) return;
	if(getStatus()!=CLOSED)
		error("Kbase::testSituation(): Kbase is not closed");
	char dirfile[FILENAMELENGTH];
	strcpy(dirfile,getHome());
	if (dirfile[strlen(dirfile)-1]!='/') strcat(dirfile,"/");
	strcat(dirfile,getPath());
	//if (_access(dirfile,R_OK|W_OK)!=0)
	//	error("Kbase::testSituation(): Path error.",dirfile);
	char cfgfile[FILENAMELENGTH];	getFullName(cfgfile,".CFG");
	//if (_access(cfgfile,R_OK|W_OK)!=0)
	//	error("Kbase::testSituation(): cannot access config file.",cfgfile);
	char datfile[FILENAMELENGTH];	getFullName(datfile,".DAT");
	//if (_access(datfile,R_OK)!=0)
	//	error("Kbase::testSituation(): cannot access date file.",datfile);
#ifdef KBMS
	char txtfile[FILENAMELENGTH];	getFullName(txtfile,".TXT");
	//if (_access(txtfile,R_OK)!=0)
	//	error("Kbase::testSituation(): cannot access text file.",txtfile);
#endif // KBMS
}

void Kbase::testUpdated()
{
#ifdef KBMS
	char datfile[FILENAMELENGTH]; getFullName(datfile,".DAT");
	char txtfile[FILENAMELENGTH]; getFullName(txtfile,".TXT");
	if (FileGetTime(datfile)+10 < FileGetTime(txtfile)) // +10: win98 error for empty txtfile
		message("Kbase::testUpdated(): the date file "
				"is not up to date, please update it.",	datfile);
#endif KBMS
}

void Kbase::rdCfg()
{	
	rdType();			
}

void Kbase::testCfg()
{
  //	if(strcmp(Type,hasType())!=0) error("Kbase::testCfg(): TYPE");
}

void Kbase::wtCfg()
{
	CfgFile.seekg(0,ios::beg);
	wtType();					
}

void Kbase::initCfg()
{
	strcpy(Type,hasType());
}

void Kbase::rdType()
{
	if(!CfgFile) error("Kbase::rdType(): CFGFILE");
	CfgFile.seekg(0,ios::beg);     
	char tmp[255];
	char *p;
	while(CfgFile.peek()!=EOF)
	{ 
		CfgFile.getline(tmp,255); 
		if(strstr(tmp,"Type"))
		{
			p=strstr(tmp,"=");
		    p++;
			strcpy(Type,p);
			for (p=Type;*p;p++) if (*p==' ') *p=0;
			break;
		}
	}
}

void Kbase::wtType()
{
	if(!CfgFile) error("Kbase::wtType: CFGFILE");
	CfgFile << "Type=" << hasType() << endl;

}


/***************************************************************
*   Date:             1994.5.31                                *
*   In parameters:    file name string,file postfix string     *
*   Return Value:     full file name:including path,name and   *
*                                     postfix                  *
*   Descriptions:     concate path, name,and postfix into one  *
*					  complete file name 		               *
****************************************************************/

char *Kbase::getFullName(char *fullname,const char *pfix,const char *pfix2)
{
	strcpy(fullname,getHome());
	if (fullname[strlen(fullname)-1]!='/') strcat(fullname,"/");
	strcat(fullname,getPath());
	if (fullname[strlen(fullname)-1]!='/') strcat(fullname,"/");  
	strcat(fullname,getName());
	strcat(fullname,pfix);
	if (pfix2) strcat(fullname,pfix2);
	return fullname;
}

char *Kbase::makFullPath()
{
	strcpy(Dir,getHome());
	if (Dir[strlen(Dir)-1]!='/') strcat(Dir,"/");
	strcat(Dir,getPath());
	return Dir;
}

void Kbase::rdLock()
{
#ifdef KBMS
	if (MultiUser==FALSE) return;
	if (Lock==LOCKED) error("Kbase::rdLock(): rdLock while Lock==LOCKED.");

	char lockfile[FILENAMELENGTH];
	getFullName(lockfile,".LOK");
	char statfile[FILENAMELENGTH];
	getFullName(statfile,".STA");

	char ch;
	fstream statstrm;
	while (TRUE)
	{
		/*本段程序循环等待其他进程删除锁定文件*/
		FileLock(lockfile,"ReadLock a Kbase.");
		statstrm.open(statfile,ios::in|ios::out);
		if ( !statstrm )
		{ 
			FileUnlock(lockfile); 
			error("kbase::rdLock(): file cannot open.",statfile); 
		}
		if(statstrm.peek()==EOF) ch='0';
		else statstrm.get(ch);
		if (ch>='0') break;
		statstrm.close();
		char lpBuffer[256];
		sprintf(lpBuffer,"Conflict when ReadLock: %s.STA, Retry? ",getName());
		if (yesno(lpBuffer)==NO)
		{
			FileUnlock(lockfile);
			error("Kbase::rdLock(): user interrupt in rdLock.");
		}
	};

	// if the byte is greater than or equal to zero
	// increase the byte with one and write it to the status file
	// that means increasing one to the user number of currently
	// reading the kbase
	ch++;
	statstrm.seekg(0,ios::beg);
	statstrm.put(ch);
	statstrm.put('\n');
	statstrm.close();
	FileUnlock(lockfile);
	Lock=LOCKED;
#endif //KBMS
}

#ifdef KBMS

//  lock the Kbase for writing 
void Kbase::wtLock() 
{ 
	if (MultiUser==FALSE) return;
	if (Lock==LOCKED) 
		error("Kbase::wtLock(): wtLock while Lock==LOCKED");
   
	char lockfile[FILENAMELENGTH];
	getFullName(lockfile,".LOK");
	char statfile[FILENAMELENGTH];
	getFullName(statfile,".STA");

	char ch;
	fstream statstrm;
	while (TRUE)
	{
		/*本段程序循环等待其他进程删除锁定文件*/
		FileLock(lockfile,"WriteLock a Kbase.");
		statstrm.open(statfile,ios::in|ios::out/**/);
		if( !statstrm )
		{ 
			FileUnlock(lockfile); 
			error("Kbase::wtLock(): STAOPEN",statfile); 
		}
		if(statstrm.peek()==EOF) ch='0';
		else statstrm.get(ch);
		if (ch=='0') break;
		statstrm.close();
		FileUnlock(lockfile);
		char lpBuffer[256];
		sprintf(lpBuffer,"Conflict when WriteLock: %s.STA,Retry? ",getName());
		if (yesno(lpBuffer)==NO)
		{
			FileUnlock(lockfile);
			error("Kbase::rdLock(): user interrupt in wtLock.");
		}
	};

	statstrm.seekg(0,ios::beg);
	ch--;
	statstrm.put(ch);
	statstrm.put('\n');
	statstrm.close();
	FileUnlock(lockfile);
	Lock=LOCKED;
}

#endif /* KBMS */

//  unlock the Kbase
void Kbase::unLock()
{
#ifdef KBMS
	if (MultiUser==FALSE) return;
	if (Lock==UNLOCK) return;

	char lockfile[FILENAMELENGTH];
	getFullName(lockfile,".LOK");
	/*本段程序循环等待其他进程删除锁定文件*/
	FileLock(lockfile,"Unlock a Kbase.");
	char statfile[FILENAMELENGTH];
	getFullName(statfile,".STA");
	fstream statstrm(statfile,ios::in|ios::out);
	if (!statstrm)
	{ 
		FileUnlock(lockfile); 
		error("Kbase::unLock(): STAOPEN",statfile); 
	}

	char ch;
	statstrm.get(ch);
   // open the status file then read the byte in it.
	if (ch<'0')
	{
		ch='0';
		statstrm.seekg(0,ios::beg);
		statstrm.put(ch);
		statstrm.put('\n');
	}
	else if (ch>'0')
	{
		ch--;		// decrease the user number by one.
		statstrm.seekg(0,ios::beg);
		statstrm.put(ch);
	}
	else cerr << "Warning: unLock while status==0." << endl;
	statstrm.close();
	Lock=UNLOCK;
	FileUnlock(lockfile);
#endif //KBMS
}

/***************************************************************
*   Date:             1994.6.7                                 *
*   In parameters:    pointer to new path string               *
*   Return Value:     SUCC or FAIL                             *
*   Descriptions:     set new path to the member Path          *
*   should be used before ropen(),wopen(),create(),clear(), etc. *
****************************************************************/

//setHome, setName and setPath should be used before ropen(),wopen(),create(),clear(), etc.
BOOL Kbase::setHome(const char *newHome)  
{
  //	if (*Home!=0) 
  //		error("Kbase::setHome(): setHome while Home!=NULL");
  //if (_access(newHome,R_OK|W_OK)!=0)
  //	error("Kbase::setHome(): Home directory error.");
  //_fullpath(Home,newHome,FILENAMELENGTH-1);
  //strcpy(Home,"/home/xptao/txp/mtcon/mtoutput/kbase");
  strcpy(Home,newHome);
  return SUCC;
}

void Kbase::setPath(const char *newPath)  
{
	if (getBuilt()==YES) 
		error("Kbase::setPath(): setPath while Built=Yes");
	if(Status!=CLOSED)
		error("KBase::setPath(): setName while Status!=CLOSED");
	strcpy(Path,newPath);
}

void Kbase::setName(const char *newName)                      
{
	if (getBuilt()==YES) 
		error("KBase::setName(): setName while Built=Yes");
	if(Status!=CLOSED)
		error("KBase::setName(): setName while Status!=CLOSED");
	strcpy(Name,newName);
}

#ifdef KBMS
// initialize the Kbase with a new config file and an empty data file

void Kbase::create()
{
  /*
	if (Modify==FALSE)
	{
		message("The Modify Status of the Kbase is OFF.\n"
				"Cannot Create it.");
		return;
	}
  */
	/*
	if (yesno("Do you really want create a new kbase?",NO)==NO)
		return;
	*/
	if (getBuilt()==YES) free();

	if (Status!=CLOSED) 
		error("Kbase::create(): cannot init while not closed");
	makFullPath();
	DirMake(getDir());

	char txtfile[FILENAMELENGTH];
	getFullName(txtfile,".TXT");
	char editfile[FILENAMELENGTH];
	getFullName(editfile,".EDIT");
	/*
	if (::_access(editfile,F_OK)==0)
	{
		if (FileGetTime(txtfile)<FileGetTime(editfile))
		{
			FileCopyTo(editfile,txtfile);
		}
		else FileTouch(txtfile);
	}
	else FileTouch(txtfile);
	*/

	char statfile[FILENAMELENGTH];
	getFullName(statfile,".STA");
	fstream statstrm(statfile,ios::out);
	statstrm.seekg(0,ios::beg); 
	statstrm.put('0');
	statstrm.put('\n');
	statstrm.close();

	// open the CfgFile for write, if error, error(CFGOPEN)
	char cnfgfile[FILENAMELENGTH];
	getFullName(cnfgfile,".CFG");
	CfgFile.open(cnfgfile,ios::out);
	if(!CfgFile) error("Kbase::create(): CFGOPEN",cnfgfile);

	// call initCfg()
	initCfg();
	wtCfg();
	CfgFile.close();

	// open the DatFile for writing, if error, error(DATOPEN)
	char datafile[FILENAMELENGTH];
	getFullName(datafile,".DAT");
	// cut the DatFile to zero length
	DatFile.open(datafile,ios::out|ios::binary|ios::trunc);
	if(!DatFile) error("Kbase::create(): DATOPEN",datafile);

	DatFile.close();
}

void Kbase::update()
{
  try
    {
      create();
      Load();
    }
  catch (CErrorException *) {}
}

void Kbase::view()
{
	char txtfile[FILENAMELENGTH];
	getFullName(txtfile,".TXT");
	char viewfile[FILENAMELENGTH];
	getFullName(viewfile,".VIEW");
	FileCopyTo(txtfile,viewfile);
	ShellTextOpen(viewfile);
}

void Kbase::edit()
{
	if (Modify==FALSE)
	{
		message("The kbase is not on Modify status,\n"
				"Please turn it on.");
		return;
	}
	char txtfile[FILENAMELENGTH];
	getFullName(txtfile,".TXT");
	ShellTextOpen(txtfile);
}

void Kbase::undoedit()
{
	if (Modify==FALSE)
	{
		message("The kbase is not on Modity status.\n"
				"Cannot Undo Edit.");
		return;
	}
	char editfile[FILENAMELENGTH];
	getFullName(editfile,".EDIT");
	char txtfile[FILENAMELENGTH];
	getFullName(txtfile,".TXT");
	if (yesno("Do you really want to undo your change?",NO)==YES)
	{
		FileCopyTo(editfile,txtfile);
	}
}

void Kbase::setModifyOff()
{
	if (Modify==FALSE) return;
	char txtfile[FILENAMELENGTH];
	getFullName(txtfile,".TXT");
	char editfile[FILENAMELENGTH];
	getFullName(editfile,".EDIT");
	/*
	if (::_access(editfile,F_OK)==0)
	{
	  if (remove(editfile)!=0)
	    error("Kbase::setModifyOff(): cannot remove .EDIT file");
	}
	*/
	Modify=FALSE;
}

void Kbase::setModifyOn()
{
	if (Modify!=FALSE) return;
	char editfile[FILENAMELENGTH];
	getFullName(editfile,".EDIT");
	char txtfile[FILENAMELENGTH];
	getFullName(txtfile,".TXT");
	/*
	if (MultiUser==TRUE)
	{
		if (::_access(editfile,F_OK)==0)
		{
			if (yesno("There is a .edit file already.\n"
					  "It seems that some other use is modifying the kbase now.\n"
					  "Go on anyway?",NO)==NO)
			return;
		}
	}
	FileCopyTo(txtfile,editfile);
	if (::_access(editfile,F_OK)!=0)
		error("Kbase::setModifyOn(): cannot create .EDIT file.",editfile);
	*/
	Modify=TRUE;
}

#endif

// open the Kbase for reading
void Kbase::ropen()
{
	// test Path, if error, error(PATH)
	//testSituation();

	if(Status!=CLOSED) error("Kbase::ropen(): cannot ropen while not closed");

	rdLock();
	Status=ROPEN;

	// open the CfgFile, if error, error(CFGOPEN)
	char cnfgfile[FILENAMELENGTH];
	getFullName(cnfgfile,".CFG");
#ifdef UNIX
	CfgFile.open(cnfgfile,ios::in);
#endif
#ifdef WIN32
	CfgFile.open(cnfgfile,ios::in);
#endif
	if(!CfgFile) error("Kbase::ropen(): CFGOPEN",cnfgfile);

	rdCfg();
	//testCfg();
	CfgFile.close();

	// open the DatFile for reading, if error, error(DATOPEN)
	char datafile[FILENAMELENGTH];
	getFullName(datafile,".DAT");
#ifdef UNIX	
	DatFile.open(datafile,ios::in|ios::binary);
#endif
#ifdef WIN32
	DatFile.open(datafile,ios::in|ios::binary);
#endif
	if(!DatFile) error("Kbase::ropen(): DATOPEN",datafile);
}

#ifdef KBMS
// open the Kbase for reading and writing
void Kbase::wopen()
{
	// test Path, if error, error(PATH)
	//testSituation();

	if(Status!=CLOSED) error("Kbase::wopen(): cannot wopen while not closed");

	wtLock();
	Status=WOPEN;

	// open the CfgFile, if error, error(CFGOPEN)
	char cnfgfile[FILENAMELENGTH];

	getFullName(cnfgfile,".CFG");
	CfgFile.open(cnfgfile,ios::in);//,filebuf::sh_read|filebuf::sh_write);
	if(!CfgFile) error("kbase::wopen(): CFGOPEN",cnfgfile);
	rdCfg();
	//testCfg();
	CfgFile.close();

	// open the DatFile for writing, if error, error(DATOPEN)
	char datafile[FILENAMELENGTH];
	getFullName(datafile,".DAT");
	DatFile.open(datafile,ios::in|ios::out|ios::binary);/*);*/
	if(!DatFile) error("Kbase::wopen(): DATOPEN",datafile);
}
#endif

// close the Kbase
void Kbase::close()
{
	if(Status==CLOSED) 
		error("Kbase::close(): cannot close while CLOSE");
	if(CfgFile.is_open()) CfgFile.close();
	if(DatFile.is_open()) DatFile.close();
	if(TxtFile.is_open()) TxtFile.close();
	unLock();
	Status= CLOSED;
}

static /*const*/ char *DELIMLIST[] =
{
	"~~",
	"||",
	"@@",
	">>",
	"=>",
	"==",
	"<<",
	"::",
	"////",
	"///",
	"//",
	"->",
	"--",
	"**",
	"&&",
	"%%%%",
	"%%%",
	"%%",
	"$$",
	"##",
	"!=",
	""   
};

void Kbase::beginScan (int buffersize)
{
	if (!Token::getDelimList()) Token::initDelimList(DELIMLIST);
	BufferSize=buffersize;
	TxtBuff=new char[BufferSize];
	DatBuff=new char[BufferSize];
	*Line=0; LineScan=Line;
}

void Kbase::endScan()		// end of scan
{
	if (TxtBuff) { delete[] TxtBuff; TxtBuff=NULL; }
	if (DatBuff) { delete[] DatBuff; DatBuff=NULL; }
}

BOOL Kbase::rdTxtRecord(const char * begin,const char * end,char * SwpBuff)
{
	if (!TxtFile) return FALSE;
	int endflag=TRUE;
	if (!end || !*end) { endflag=FALSE; end=begin; }
	char * TxtScan=TxtBuff;
	if (SwpBuff) strcpy(SwpBuff,LineScan);
	enum { NOTEBEGIN, BEGIN, NOTEMIDDLE, MIDDLE, END } pos=BEGIN;
	while (pos!=END)
	{
		if (*LineScan==0)
		{
			do
			{
				char * p=Line-1, * q=Line, * end=Line+LINELENGTH-1;
				do { *++p=TxtFile.get(); if (isspace(*p)) q=p; }
				while (*p!='\n' && *p!='\r' && *p!=EOF && p<end);
				if (p>=end) { TxtFile.seekg(q-p,ios::cur); p=q; }
				if (*p==EOF) *p=0; else *++p=0;
				if (SwpBuff && !((pos==NOTEBEGIN || pos==NOTEMIDDLE) &&
								 strstr(Line,"*/")==NULL) )
					strcat(SwpBuff,Line); // the whole note line is omitted
			} while (TxtFile.peek()!=EOF && *Line==0);
			if (*Line==0 && TxtFile.peek()==EOF)
			{
				if (pos==BEGIN) return FAIL;
				else if (pos==MIDDLE && endflag==0) { pos=END; continue; }
				else error("Kbase::rdTxtRecord(): unexpected end of file.");
			}
			LineScan=Line;
		}
		if (pos==NOTEBEGIN || pos==NOTEMIDDLE)
		{
			char * newLineScan=strstr(LineScan,"*/");
			if (newLineScan) 
			{
				LineScan=newLineScan+2;
				*TxtScan++=' ';
				if (pos==NOTEBEGIN) pos=BEGIN;
				else /* pos==NOTEMIDDLE */ pos=MIDDLE;
			}
			else { *LineScan=0; }
		}
		else if (pos==BEGIN)
		{
			while (*LineScan)
			{
				if (isSpcChar(LineScan))
				{
					while(isSpcChar(LineScan)) goNext(LineScan);
					if (*LineScan) *TxtScan++=' ';
					else { *TxtScan++='\n'; continue; }
				}
				if (*LineScan=='/' && *(LineScan+1)=='*')
				{
					LineScan+=2;
					pos=NOTEBEGIN; break;
				}
				int agree=stragree(LineScan,begin);
				if (agree==0 || agree==1)
				{
					while (*begin) { *TxtScan++=*begin++; LineScan++; }
					pos=MIDDLE; break;
				}
				setCharNext(TxtScan,LineScan);
			}
		}
		else // if (pos==MIDDLE)
		{
			while (*LineScan)
			{
				if (isSpcChar(LineScan))
				{
					while(isSpcChar(LineScan)) goNext(LineScan);
					if (*LineScan) *TxtScan++=' ';
					else { *TxtScan++='\n'; continue; }
				}
				if (*LineScan=='/' && *(LineScan+1)=='*')
				{
					LineScan+=2;
					pos=NOTEMIDDLE; break;
				}
				int agree=stragree(LineScan,end);
				if (agree==0 || agree==1)
				{
					if (endflag)
						while (*end) { *TxtScan++=*end++; LineScan++; }
					pos=END; break;	
				}
				setCharNext(TxtScan,LineScan);
			}
		}
	}
	*TxtScan=0;
	if (SwpBuff && *LineScan) SwpBuff[strlen(SwpBuff)-strlen(LineScan)]=0;
	return SUCC;
}

#ifdef KBMS
void Kbase::wtTxtBuff()
{
	if(TxtFile.is_open()) TxtFile << TxtBuff ;
	else error("Kbase::wtTxtBuff(): Output TxtFile");
}

void Kbase::wtDatLengthToFile(int length,fstream & file)
{
	if (!file.is_open())
		error("Kbase::wtDatLengthToFile(): cannot open file.");
	int seed=file.tellp();
	char key[4];
	seedkey(key,4,seed);
	encryption((char *)&length,4,key,4);
	file.write((char *)&length,4);
}	

void Kbase::wtDatBuffToFile(fstream & file,char * buff,int leng)
{
	if (!file.is_open())
		error("Kbase::wtDatBuffToFile(): cannot open file.");
	int seed=file.tellp();
	char key[4];
	seedkey(key,4,seed);
	if (leng==0) leng=strlen(buff);
	encryption(buff,leng,key,4);
	file.write(buff,leng);
}
#endif // KBMS

int Kbase::rdDatLengthFromFile(fstream & file)
{
	if(file.peek()==EOF) return 0;
	int seed=file.tellg();
	char key[4];
	seedkey(key,4,seed);
	int length;
	file.read((char *)&length,4);
	decryption((char *)&length,4,key,4);
#ifdef UNIX
	return (short)length;
#endif
#ifdef WIN32
	return length;
#endif
}
	
// read the DatBuff from DatFile and set InPntr=DatBuff and OutPntr=TxtBuff
void Kbase::rdDatBuffFromFile(int size,fstream & file,char * buffer)
{
	long offset=file.tellg();
	if (size<0) error("Kbase::rdDatBuffFromFile(): size<0.");
	if (size==0) { buffer[0]=0; return; }
	int seed=file.tellp();
	char key[4];
	seedkey(key,4,seed);
	if(file.peek()==EOF) error("Kbase::rdDatBuff(): Unexpected end of DataFile.");
	file.read(buffer,size);
	decryption(buffer,size,key,4);
	buffer[size]='\0';
}

void Kbase::encryption(char * data, int dataleng, char * key, int keyleng)
{
	int i,j;
	for (i=0,j=0; i<dataleng; i++,j++)
	{
		if (j==keyleng) j=0;
		data[i] ^= key[j];
	}
}

void Kbase::decryption(char * data, int dataleng, char * key, int keyleng)
{
	int i,j;
	for (i=0,j=0; i<dataleng; i++,j++)
	{
		if (j==keyleng) j=0;
		data[i] ^= key[j];
	}
}

void Kbase::seedkey(char * key, int keyleng, int seed)
{
	int delta=97;
	int alpha=seed;
	int i;
	for (i=0;i<keyleng;i++) 
	{
		key[i]=(char)alpha;
		alpha *= delta;
		alpha ^= delta;
	}
}

#ifdef KBMS
void Kbase::LoadAtom(IOToken &TheToken,unsigned type,unsigned limit,cValList &vallist,const char *err)
{
//  Load Atom from InPntr to OutPntr
//  examine the format of the Atom according the argument type
//  examine if the value are defined in the argument vallist
//  substitute the alias value with the name value
//	to keep read toke ahead after loadatom
	switch (type) {
		case SYMBOL:
			LoadSymbolAtom(TheToken,limit,vallist,err);
			break;

		case HIERAR:
			LoadHierarAtom(TheToken,limit,vallist,err);
			break;

		case NUMBER:
			LoadNumberAtom(TheToken,limit,vallist,err);
			break;

		case MTBOOL: 
			LoadBooleanAtom(TheToken,vallist,err); 
			break;
		default:
			error("Kbase::LoadAtom(): Invalid Atom Type");
	   }

}

void Kbase::LoadFest(IOToken &TheToken,SortedAttList &list,const char *err)
{
	char * begin=TheToken.getOutPntr();
	if(TheToken.isEqualTo("无")||TheToken.isEqualTo("NONE")||TheToken.isEqualTo("None")) { 
		TheToken.output();
		TheToken.input(); 
	}
	else if(TheToken.isEqualTo("[")) { 	
		TheToken.output(); 
		TheToken.input(); 	
		if(TheToken.isEqualTo("]")) { 
			TheToken.output(); 
			TheToken.input();
			if(TheToken.isEqualTo("{" )) {
				TheToken.output(); 
				TheToken.input();
				if(!TheToken.isEqualTo("}")) 
					error("Kbase::LoadFest(): need \"}\"",TheToken,err);
				TheToken.output(); 
				TheToken.input();
			}
			return;
		}
		LoadIntFest(TheToken,list,err);
		if(!TheToken.isEqualTo("]")) 
			error("Kbase::LoadFest(): need \"]\"",TheToken,err);
		TheToken.output(); 
		TheToken.input();
		if(TheToken.isEqualTo("{")) {
			TheToken.output(); 
			TheToken.input();
			LoadExtFest(TheToken,list,err);
			if(!TheToken.isEqualTo("}")) 
				error("Kbase::LoadFest(): need \"}\"",TheToken,err);
			TheToken.output(); 
			TheToken.input();
 		}
	}
   	else if(TheToken.isEqualTo("{")) {
		TheToken.output(); 
		TheToken.input();
		if(TheToken.isEqualTo("}")) { 
			TheToken.output(); 
			TheToken.input(); 
			return; 
		}
		else {
			LoadExtFeat(TheToken,list,err);
			if(!TheToken.isEqualTo("}")) 
				error("Kbase::LoadFest(): need \"}\"",TheToken,err);
			TheToken.output(); 
			TheToken.input();
		}
	}
	else error("Kbase::LoadFest(): should begin with a \"[\" or \"{\"",TheToken,err);
	char * end=TheToken.getOutPntr();
	char * buffer=new char[end-begin+1];
	strncpy(buffer,begin,end-begin);
	buffer[end-begin]=0;
	IToken token(buffer);
	token.input();
	Fest fest;
	try { fest.inputFrom(token); }
	catch (CErrorException * ) { delete buffer; throw; }
	delete buffer;
}

void Kbase::LoadIntFest(IOToken &TheToken,SortedAttList &list,const char *err)
{
	LoadIntFeat(TheToken,list,err);
	while(TheToken.isEqualTo(",")) {
		TheToken.output();
		TheToken.input();
		LoadIntFeat(TheToken,list,err);
	}
	while(TheToken.isEqualTo(";")) {
		TheToken.output();
		LoadIntFeat(TheToken,list,err);
		while(TheToken.isEqualTo(",")) {
			TheToken.output();
			TheToken.input();
			LoadIntFeat(TheToken,list,err);
		}
	}
}

void Kbase::LoadIntFeat(IOToken &TheToken,SortedAttList &list,const char *err)
{
	Attribute *atomattr;
	atomattr=LoadIntAtt(TheToken,list,err);
	if(!TheToken.isEqualTo(":")) 
		error("Kbase::LoadIntFeat(): need \":\"",TheToken,err);
	TheToken.output();
	TheToken.input();
	LoadIntVal(TheToken,atomattr->getAttType(),atomattr->getLimit(),atomattr->getValList(),err);
}

Attribute *Kbase::LoadIntAtt(IOToken &TheToken,SortedAttList &list,const char *err)
{
	if(TheToken.getType()!=IDENT) 
		error("Kbase::LoadIntAtt(): IntAtt is not IDENT",TheToken,err);

	AttLink* link;
	link=list.getAttLinkHead();

	while(link && !TheToken.isEqualTo(link->getAttr()->getString()))
		link=link->getNext();  
	if(!link) 
		error("Kbase::LoadIntAtt(): Undefined Attribute",TheToken,err);

	Attribute * attr;
	if(link->getAttr()->getNameAlias()==ALIAS)
		attr=link->getAttr()->getRefer();
	else attr=link->getAttr();
	if( attr->getIntExt()!=INTERNAL) 
		error("Kbase::LoadIntAtt(): must be INTERNAL Attribute",TheToken,err );

	TheToken.setBuffer(attr->getString());
	TheToken.output();
	TheToken.input();
	return attr;
}

void Kbase::LoadIntVal(IOToken &TheToken,unsigned type,unsigned limit,class cValList &vallist,const char *err)
{
	LoadAtom(TheToken,type,limit,vallist,err);
}

void Kbase::LoadExtFest(IOToken &TheToken,SortedAttList &list,const char *err)
{
	LoadExtFeat(TheToken,list,err);
	while(TheToken.isEqualTo(",")){
		TheToken.output();
		TheToken.input();
		LoadExtFeat(TheToken,list,err);
	}
}

void Kbase::LoadExtFeat(IOToken &TheToken,SortedAttList &list,const char *err)
{
	LoadExtAtt(TheToken,list,err);
	if(!TheToken.isEqualTo(":")) 
		error("Kbase::LoadExtFeat(): need \":\"", TheToken,err);

	TheToken.output();
	TheToken.input();
	LoadExtVal(TheToken,list,err);
}

void Kbase::LoadExtAtt(IOToken &TheToken,SortedAttList &list,const char *err)
{
	AttLink* link;
	if(TheToken.getType()!=IDENT) 
		error("Kbase::LoadExtAtt(): ExtAtt is not IDENT",TheToken,err );

	//find SemAss in SortedAttList
	link=list.getAttLinkHead();
	while(link && !TheToken.isEqualTo(link->getAttr()->getString()))
		link=link->getNext();
	if(!link) 
		error("Kbase::LoadExtAtt(): Undefined Attribute",TheToken,err);

	Attribute * attr;
	if(link->getAttr()->getNameAlias()==ALIAS) attr=link->getAttr()->getRefer();
	else attr=link->getAttr();
	if( attr->getIntExt()!=EXTERNAL) 
		error("Kbase::LoadExtAtt(): Should be EXTERNAL Attribute",TheToken, err );

	TheToken.setBuffer(attr->getString());
	TheToken.output();
	TheToken.input();
}

void Kbase::LoadExtVal(IOToken &TheToken,SortedAttList &list,const char *err)
{
	LoadFest(TheToken,list,err);
}

void Kbase::LoadSymbolAtom(IOToken &TheToken,unsigned limit,cValList &vallist,const char *err)
{
	if(TheToken.isEqualTo("~")) LoadSymbol_NOT(TheToken,limit,vallist,err);
	else LoadSymbol_OR(TheToken,limit,vallist,err);
}

void Kbase::LoadSymbol_OR(IOToken &TheToken,unsigned limit,cValList &vallist,const char *err)
{
	LoadSymbol_SMP(TheToken,limit,vallist,err);
	while(TheToken.isEqualTo("|")) {
		TheToken.output();
		TheToken.input();
		LoadSymbol_SMP(TheToken,limit,vallist,err);
	}
}

void Kbase::LoadSymbol_NOT(IOToken &TheToken,unsigned limit,cValList &vallist,const char *err)
{
	while(TheToken.isEqualTo("~")) {
		TheToken.output();
		TheToken.input();
		LoadSymbol_SMP(TheToken,limit,vallist,err);
	}
}

void Kbase::LoadSymbol_SMP(IOToken &TheToken,unsigned limit,cValList &vallist,const char *err)
{
   if(TheToken.getType()!=IDENT)
		error("Kbase::LoadSymbol_SMP: NOT IDENT",TheToken,err);

   // the token has been saved in the TokenBuff
   //look for the value in the vallist.
   ValLink *vlink;
   vlink=vallist.getValLinkHead();	
   while(vlink&&strcmp(vlink->getVal()->getString(),TheToken.getBuffer()))
		vlink=vlink->getNext();
   if(!vlink && (limit==LIMITED))  
		error("Kbase::LoadSymbol_SMP(): Undefined Atom in ValList",TheToken,err);

   if(vlink && vlink->getVal()->getNameAlias() == ALIAS)
	   TheToken.setBuffer(vlink->getVal()->getRefer()->getString());

   TheToken.output();
   TheToken.input();
}

void Kbase::LoadHierarAtom(IOToken &TheToken,unsigned limit,cValList &vallist,const char *err)
{
	if(TheToken.isEqualTo("~")) LoadHierar_NOT(TheToken,limit,vallist,err);
	else LoadHierar_OR(TheToken,limit,vallist,err);

}

void Kbase::LoadHierar_OR(IOToken &TheToken,unsigned limit,cValList &vallist,const char *err)
{
	LoadHierar_AND(TheToken,limit,vallist,err);
	while(TheToken.isEqualTo("|")) {
		TheToken.output();
		TheToken.input();
		LoadHierar_AND(TheToken,limit,vallist,err);
	}
}

void Kbase::LoadHierar_NOT(IOToken &TheToken,unsigned limit,cValList &vallist,const char *err)
{
	while(TheToken.isEqualTo("~")) {
		TheToken.output();
		TheToken.input();
		LoadHierar_SMP(TheToken,limit,vallist,err);
	}
}

void Kbase::LoadHierar_AND(IOToken &TheToken,unsigned limit,cValList &vallist,const char *err)
{
	LoadHierar_SMP(TheToken,limit,vallist,err);
	while(TheToken.isEqualTo("~")) {
		TheToken.output();
		TheToken.input();
		LoadHierar_SMP(TheToken,limit,vallist,err);
	}
}

void Kbase::LoadHierar_SMP(IOToken &TheToken,unsigned limit,cValList &vallist,const char *err)
{
	if(TheToken.getType()!=IDENT)
		error("Kbase::LoadHierar_SMP(): NOT IDENT",TheToken,err);

    ValLink  * vlink;
    vlink=vallist.getValLinkHead(); 
    while(vlink&&strcmp(vlink->getVal()->getString(),TheToken.getBuffer()))
 		vlink=vlink->getNext();
    if(!vlink && (limit==LIMITED)) 
 		error("Kbase::LoadHierar_SMP(): Undefined Atom in ValList",TheToken,err);
	if(vlink && vlink->getVal()->getNameAlias() == ALIAS)
	   TheToken.setBuffer(vlink->getVal()->getRefer()->getString());

	 TheToken.output();
	 TheToken.input();
}

void Kbase::LoadNumberAtom(IOToken &TheToken,unsigned limit,cValList &vallist,const char *err)
{
	if(TheToken.isEqualTo("~")) LoadNumber_NOT(TheToken,limit,vallist,err);
	else LoadNumber_OR(TheToken,limit,vallist,err);
}

void Kbase::LoadNumber_OR(IOToken &TheToken,unsigned limit,cValList &vallist,const char *err)
{
	LoadNumber_SMP(TheToken,limit,vallist,err);
	while(TheToken.isEqualTo("|")) {
		TheToken.output();
		TheToken.input();
		LoadNumber_SMP(TheToken,limit,vallist,err);
	}
}

void Kbase::LoadNumber_NOT(IOToken &TheToken,unsigned limit,cValList &vallist,const char *err )
{
	while(TheToken.isEqualTo("~")) {
		TheToken.output();
		TheToken.input();
		LoadNumber_SMP(TheToken,limit,vallist,err);
	}
}

void Kbase::LoadNumber_SMP(IOToken &TheToken,unsigned limit,cValList &vallist,const char *err)
{
	if(TheToken.getType()!=INTEG && TheToken.getType()!=IDENT)
		error("Kbase::LoadNumber_SMP(): Invalid Integ and Ident",TheToken,err);

	if(TheToken.getType() ==IDENT) {
		ValLink *vlink;
		vlink=vallist.getValLinkHead();
	    while( vlink&&strcmp(vlink->getVal()->getString(),TheToken.getBuffer())) 
			vlink=vlink->getNext();

	   if(!vlink && (limit==LIMITED)) 
	   	error("Kbase::LoadNumbet_SMP(): Undefined Atom in ValList",TheToken,err);

	   if(vlink && vlink->getVal()->getNameAlias() == ALIAS)
			TheToken.setBuffer(vlink->getVal()->getRefer()->getString()); 
	}

    TheToken.output();
	TheToken.input();
}

void Kbase::LoadBooleanAtom(IOToken &TheToken,cValList &vallist,const char *err)
{
	if (TheToken.getType()!=IDENT) error("Type:IDENT",err);

	if (TheToken!="是" && TheToken!="YES" && TheToken!="Yes" &&
		TheToken!="否" && TheToken!="NO"  && TheToken!="No"     )
	{
		ValLink * vlink;
		vlink=vallist.getValLinkHead();
		if (vlink)
		{
    		while(vlink&&strcmp(vlink->getVal()->getString(),TheToken.getBuffer()))
				vlink=vlink->getNext();
		}
		if(!vlink) error("Kbase::LoadBooleanAtom(): Undefined Boolean in ValList",TheToken,err);
		if(vlink->getVal()->getNameAlias() == ALIAS)
			TheToken.setBuffer(vlink->getVal()->getRefer()->getString());
	}
	if(TheToken.isEqualTo("是") || TheToken.isEqualTo("YES") || TheToken.isEqualTo("Yes"))
		TheToken.output("YES");
	else if(TheToken.isEqualTo("否") || TheToken.isEqualTo("NO")  || TheToken.isEqualTo("No"))
		TheToken.output("NO");
	else error("Kbase::LoadBooleanAtom(): not a Boolean value.",TheToken);

	TheToken.input();
}

#endif

void Kbase::ReadFest(IToken &TheToken)
{
	if( TheToken.isEqualTo("无") || TheToken.isEqualTo("NONE") || TheToken.isEqualTo("None") ) 
		TheToken.input(); 
	else {
		if(TheToken.isEqualTo("[")){
			TheToken.input(); 	
			while(!TheToken.isEqualTo("]")) TheToken.input(); 
			TheToken.input();
		}
		if(TheToken.isEqualTo("{")){
				int num=1;
				TheToken.input();
				while(num==0) {
					TheToken.input();
					if (TheToken.isEqualTo("}")) num--;
					else if (TheToken.isEqualTo("{")) num++;
			} 
			TheToken.input();
		}
	}
}

void Kbase::ReadAtom(IToken &TheToken,unsigned type)
{
	switch (type) {
		case SYMBOL:
			ReadSymbolAtom(TheToken);
			break;

		case HIERAR:
			ReadHierarAtom(TheToken);
			break;

		case NUMBER:
			ReadNumberAtom(TheToken);
			break;
		case MTBOOL: 
			ReadBooleanAtom(TheToken); 
			break;
	   }
}

void Kbase::ReadSymbolAtom(IToken &TheToken)
{
	if(TheToken.isEqualTo("~")) ReadSymbol_NOT(TheToken);
	else ReadSymbol_OR(TheToken);
}


void Kbase::ReadSymbol_OR(IToken &TheToken)
{
	ReadSymbol_SMP(TheToken);
	while(TheToken.isEqualTo("|")) {
		TheToken.input();
		ReadSymbol_SMP(TheToken);
	}
}

void Kbase::ReadSymbol_NOT(IToken &TheToken)
{
	while(TheToken.isEqualTo("~")) {
		TheToken.input();
		ReadSymbol_SMP(TheToken);
	}

}

void Kbase::ReadSymbol_SMP(IToken &TheToken)
{
   TheToken.input();
}

void Kbase::ReadHierarAtom(IToken &TheToken)
{
	if(TheToken.isEqualTo("~")) ReadHierar_NOT(TheToken);
	else ReadHierar_OR(TheToken);
}

void Kbase::ReadHierar_OR(IToken &TheToken)
{
	ReadHierar_AND(TheToken);
	while(TheToken.isEqualTo("|")) {
		TheToken.input();
		ReadHierar_AND(TheToken);
	}
}

void Kbase::ReadHierar_NOT(IToken &TheToken)
{
	while(TheToken.isEqualTo("~")) {
		TheToken.input();
		ReadHierar_SMP(TheToken);
	}
}

void Kbase::ReadHierar_AND(IToken &TheToken)
{
	ReadHierar_SMP(TheToken);
	while(TheToken.isEqualTo("~")) {
		TheToken.input();
		ReadHierar_SMP(TheToken);
	}
}

void Kbase::ReadHierar_SMP(IToken &TheToken)
{
   TheToken.input();
}

void Kbase::ReadNumberAtom(IToken &TheToken)
{
	if(TheToken.isEqualTo("~")) ReadNumber_NOT(TheToken);
	else ReadNumber_OR(TheToken);
}

void Kbase::ReadNumber_OR(IToken &TheToken)
{
	ReadNumber_SMP(TheToken);
	while(TheToken.isEqualTo("|")) {
		TheToken.input();
		ReadNumber_SMP(TheToken);
	}
}

void Kbase::ReadNumber_NOT(IToken &TheToken)
{
	while(TheToken.isEqualTo("~")) {
		TheToken.input();
		ReadNumber_SMP(TheToken);
	}
}

void Kbase::ReadNumber_SMP(IToken &TheToken)
{
	TheToken.input();
}

void Kbase::ReadBooleanAtom(IToken &TheToken)
{
	TheToken.input();
}

/********************************************************************
*   Function Description:                                           *
*			add the attribute node to the link.						*
*			For UnsortAttList,insert it to the head of the link     *
*			For sortedAttLink,insert it to the right position to    *
*				keep the link remaining String ascending order      *
*********************************************************************/

Attribute * AttList::find(const char *tokenbuff)
{
	AttLink * p;
	p=attLinkHead;
	while( p && strcmp(p->getAttr()->getString(),tokenbuff) )
		 p=p->getNext();
	
	if(p && (!strcmp(p->getAttr()->getString(),tokenbuff)))  
				return ((p->getAttr()));
	else return NULL;
} 

//add att to the tail of the link
void UnsortAttList ::addto(Attribute * attr)
{
	AttLink  * alink;
	alink= new AttLink (attr);      

	if(!alink) 
		error("UnsortAttList::addto(Attribute *): Allocation error");

	if(attLinkHead) {
		attLinkTail->setNext(alink);
		attLinkTail=alink;
	}
	else {
		attLinkHead = alink;
		attLinkTail = alink;
	}
}

void SortedAttList::makeindex()
{
	attarray = new Attribute * [number];
	AttLink * link;
	int i;
	for (link=attLinkHead,i=0;link;link=link->getNext(),i++)
		attarray[i]=link->getAttr();
}

Attribute * SortedAttList::find(const char *tokenbuff)
{
	Attribute * attr;
	if (attarray==NULL)
	{
		attr=AttList::find(tokenbuff);
		return attr;
	}
	int begin=0, end=number-1, current;
	while (TRUE)
	{
		int width=end-begin;
		if (width<0) { attr=NULL; break; }
		current=begin+width/2;
		int delta=strcmp(tokenbuff,attarray[current]->getString());
		if (delta==0) { attr=attarray[current]; break; }
		else if (delta>0) begin=current+1;
		else end=current-1;
	}
	return attr;
}

//insert to list by order
void SortedAttList ::addto(Attribute * attr)
{
	if (attarray!=NULL)
		error("SortedAttList::addto(): add attribute after makeindex().");

	AttLink * alink;
	alink=new AttLink (attr);      

	if(!alink) 
		error("SortedAttList::add(Attirbute *): Allocation error");

	if(attLinkHead) {  			
		if(strcmp(alink->getAttr()->getString(),attLinkHead->getAttr()->getString())<0) {
			alink->setNext ( attLinkHead);
			attLinkHead = alink;
		}       
		else {
			AttLink *p;
			p=attLinkHead;
			while((p->getNext()) && strcmp(alink->getAttr()->getString(),
				p->getNext()->getAttr()->getString())>0 )
					p= p->getNext();					    
			// point to the pre position that currently comparing
			if(!(p->getNext())) {                
				p->setNext ( alink);
				attLinkTail = alink;
			}										
			else {								
				alink->setNext ( p->getNext());
				p->setNext (alink);
			}
		}
	}
	else {								
		attLinkHead = alink;
		attLinkTail = alink;
	}
	number++;
}

CondLink * CondList::addto(char *Condi)
{
	CondLink * clink;
	clink =new CondLink(Condi);
	
	if(!clink) 
		error("CondList::addto(char *): Allocation error");
	if(CondLinkHead){
		CondLinkTail->setNext( clink);
		CondLinkTail=clink;
	}
	else{
		CondLinkHead=clink;
		CondLinkTail=clink;
		}
	return clink;
}
				
CondLink * CondList::addto(char *Condi,UnsortAttList &condattlist)
{
	CondLink * clink;
	clink =new CondLink(Condi,condattlist);
	
	if(!clink) 
		error("CondList::addto(char *,UnsortAttList &): Allocation error");
	if(CondLinkHead){
		CondLinkTail->setNext( clink);
		CondLinkTail=clink;
	}
	else{
		CondLinkHead=clink;
		CondLinkTail=clink;
	}
	return clink;
}
				
//add val to the tail of link
void cValList::addto(Value * val)
{
	ValLink   * vlink;

	vlink=new ValLink (val);
	if(!vlink) 
		error("ValList::addto(Value *): Allocation error");

	if(ValLinkHead){
		ValLinkTail->setNext(vlink);
		ValLinkTail=vlink;
	}
	else{
		ValLinkHead=vlink;
		ValLinkTail=vlink;
	}
}

//find value in ValList
Value *cValList::find(const char * tokenbuff)
{
	ValLink  *p;
	p=ValLinkHead;
	while( p && strcmp(p->getVal()->getString(),tokenbuff) )
		 p=p->getNext();
	if( p && (!strcmp(p->getVal()->getString(),tokenbuff)))
 		return((p->getVal()));
	else  return NULL;
}

