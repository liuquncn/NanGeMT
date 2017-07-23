
//#include <io.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
//#include <windows.h>
//#include <direct.h>
//#include <process.h>
#include "function.h"
#include "error.h"

using namespace std;

void message(const char * msg1, const char * msg2, const char * msg3)
{
	char * errormsg;
	
	int length=strlen(msg1)+(msg2?strlen(msg2):0)+(msg3?strlen(msg3):0);
	errormsg=new char[length+10];
	
	if (msg1) { strcpy(errormsg,msg1);	strcat(errormsg,"\n"); }
	if (msg2) { strcat(errormsg,msg2);	strcat(errormsg,"\n"); }
	if (msg3) { strcat(errormsg,msg3);	strcat(errormsg,"\n"); }

	static char ErrorMsg[256];
	strcpyn(ErrorMsg,errormsg,250);
	if (strlen(ErrorMsg)>245) strcat(ErrorMsg," ...");
	delete[] errormsg;

	//::MessageBox(NULL,ErrorMsg,"Message",MB_OK|MB_ICONEXCLAMATION);
}

int yesno(const char * msg,BOOL def)
{
  //int style=MB_YESNO;
  //	if (def==TRUE) style=style|MB_DEFBUTTON1;
  //	else style=style|MB_DEFBUTTON2;
	//if (::MessageBox(NULL,(char *)msg,"Prompt",style)==IDNO) return FALSE;
	//else return TRUE;
	return 0;
}

void FileCopyTo(const char * lpszSrcFile,const char * lpszDestFile)
{
	//CopyFile(lpszSrcFile,lpszDestFile,FALSE);
}

void FileAppendTo(const char * lpszSrcFile,const char * lpszDestFile)
{
	char lpszSrcTemp[FILENAMELENGTH];
	char lpszDestTemp[FILENAMELENGTH];
	//_fullpath(lpszSrcTemp,lpszSrcFile,FILENAMELENGTH-1);
	//_fullpath(lpszDestTemp,lpszDestFile,FILENAMELENGTH-1);
	//if (strcmpi(lpszSrcTemp,lpszDestTemp)==0) /* ignore the case */
	//	{ FileTouch(lpszDestFile); return; }

	FILE * SrcFile;
	FILE * DestFile;
	unsigned int nCount;

	char lpBuffer[512];

	if (!(SrcFile=fopen(lpszSrcFile,"r")))
		error("FileAppendTo(): Cannot open source file: ",lpszSrcFile);
	if (!(DestFile=fopen(lpszDestFile,"w")))
		error("FileAppendTo(): Cannot open destinate file: ",lpszDestFile);
	fseek(DestFile,0,SEEK_END);

	while((nCount=fread(lpBuffer,512,1,SrcFile))==512)
		fwrite(lpBuffer,nCount,1,DestFile);
	fwrite(lpBuffer,nCount,1,DestFile);

	fclose(DestFile);
	fclose(SrcFile);
}

void FileAppendStringTo(const char * string,const char * lpszDestFile)
{
	char lpszDestTemp[FILENAMELENGTH];
	//_fullpath(lpszDestTemp,lpszDestFile,FILENAMELENGTH-1);

	FILE * DestFile;

	if (!(DestFile=fopen(lpszDestFile,"w")))
		error("FileAppendStringTo(): Cannot open destinate file: ",lpszDestFile);
	fseek(DestFile,0,SEEK_END);

	fwrite(string,strlen(string),1,DestFile);

	fclose(DestFile);
}

void FileTouch(const char * lpszFile)
{
  //if (::access(lpszFile,06)!=0)
  //		error("FileTouch(): file access error.",lpszFile);
	FILE * file;
	char c;
	file=fopen(lpszFile,"rw");
	fseek(file,0,SEEK_SET);
	c=fgetc(file);
	fseek(file,0,SEEK_SET);
	fputc(c,file);
	fclose(file);
}

extern int BeginWaitDialog(const char * waitmessage,const char * waitcaption=NULL);
extern void EndWaitDialog(int wait);

void FileLock(const char * lockfilename,const char * msg)
{
	/*
	char * info=new char[strlen(lockfilename)+strlen(msg)+5];
	strcpy(info,msg);
	strcat(info,"\n");
	strcat(info,lockfilename);
	int fd;
	int wait=0;
	while(TRUE)
	{
		if ((fd=::open(lockfilename,O_CREAT|O_EXCL,S_IREAD|S_IWRITE))!=-1) break;
		//_sleep(1000);
		//wait=BeginWaitDialog(info);
	}
	//if (wait) EndWaitDialog(wait);
	//::close(fd);
	delete[] info;
	*/
}

void FileUnlock(const char * lockfilename)
{
  //	if (unlink(lockfilename)==-1)
  //		error("FileUnlock::write_unlock(): cannot delete file",lockfilename);
  ;	
}

unsigned long FileGetTime(const char * filename)
{
	struct stat filestat;
	stat(filename,&filestat);
	return filestat.st_mtime;
}

void DirMake(const char * dirname)
{
	struct stat dirstat;
	/*
	if( stat(dirname,&dirstat)==-1 )
	{ 
#ifdef UNIX
		if( mkdir(dirname,0755)==-1 ) 
#endif
#ifdef WIN32
		if( mkdir(dirname)==-1 )
#endif
		error("DirMake(): cannot make directory.",dirname ); 
	} 
	else
	{ 
		if( !S_ISDIR(dirstat.st_mode) ) 
			error("DirMake(): dirname exists but is not a directory.", dirname);
	}
	*/
}

int ShellOpen(const char * file)
{
	return 0; //int(ShellExecute(0,NULL,file,NULL,NULL,SW_SHOWNORMAL))>32;
}

static char TextOpenCommand[COMMANDLENGTH];

int ShellTextOpen(const char * file)
{
	if (*TextOpenCommand==0)
	{
		char initempfilename[FILENAMELENGTH],tempfilename[FILENAMELENGTH];
		tmpnam(initempfilename);
		//_fullpath(tempfilename,initempfilename,FILENAMELENGTH);
		strcat(tempfilename,".txt");
		FILE * tempfile=fopen(tempfilename,"wb");
		fclose(tempfile);
		//FindExecutable(tempfilename,".",TextOpenCommand);
		//unlink(tempfilename);
	}
	return 0; //int(ShellExecute(0,NULL,TextOpenCommand,file,NULL,SW_SHOWNORMAL))>32;
}

//#define TESTTIME

#ifdef TESTTIME

long timewhole;
long timepart;
long gettime() { return GetTickCount(); }

#endif//TESTTIME
