#ifdef WIN32
#include <direct.h> //1996.11.13 10:18 am
#include <io.h>
#include <stdio.h>
#include <search.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif //WIN32

#include <fstream>
#include <stdlib.h>
#include <search.h>
#include <string.h>
#include "dictn.h"
#include "template.h"

int IndexBuffList::compare( const void *arg1, const void *arg2 )
{
   /* Compare all of both strings: */
   return stricmp( *(char**)arg1, *(char**)arg2 );
}

void IndexBuffList::create(long hashsize,const char * idxfilename,const char * lstfilename)
{
	alloc(hashsize);
	int i;
	for (i=0;i<HashSize;i++) 
	{
		Index[i].DstgValue=0;
		Index[i].NextIndex=Index[i].DataOffset=Index[i].SwapOffset=-1;
	}
	strcpy(IdxFileName,idxfilename);
	strcpy(LstFileName,lstfilename);
	WriteFile();
}

void IndexBuffList::open(long hashsize,const char * idxfilename,const char * lstfilename)
{
#ifdef KBMS
	if (Index!=NULL && HashSize==hashsize &&
		strcmp(IdxFileName,idxfilename)==0 &&
		IdxFileTime==FileGetTime(idxfilename)) return;
#else
	if (Index!=NULL) return;
#endif
	free();
	alloc(hashsize);
	ReadFile(idxfilename,lstfilename);
}

void IndexBuffList::alloc(long hashsize)
{
	free();
	HashSize=hashsize;
	IdxSize=hashsize;
	MaxSize=HashSize+(HashSize>>1);
	Index=new IndexBuffUnit[MaxSize];
	LstSize=0;
	Words=new char * [MaxSize];
}

void IndexBuffList::free()
{
	if (Index) delete[] Index;
	if (Words)
	{
		long i;
		for (i=0;i<LstSize;i++) { delete Words[i]; }
		delete Words;
	}
	Index=NULL; Words=NULL; Changed=0; *IdxFileName=*LstFileName=0;
	IdxFileTime=HashSize=IdxSize=LstSize=MaxSize=0;
}

void IndexBuffList::addword(const char * word)
{
#ifdef TEST
	Words[LstSize]=new char[strlen(word)+1];
#else
	Words[LstSize]=new char[TOKENLENGTH+1];
#endif
	strcpy(Words[LstSize],word);
	/*
	char n[TOKENLENGTH];
	itoa(LstSize,n,10);
	Words[LstSize]=new char[strlen(n)+1];
	strcpy(Words[LstSize],n);
	*/
	LstSize++;
}

void IndexBuffList::delword(const char * word)
{
	long i;
	for (i=0;i<LstSize;i++)
	{
		if (strcmp(word,Words[i])==0)
		{
			delete Words[i];
			LstSize--;
			Words[i]=Words[LstSize];
			return;
		}
	}
}

void IndexBuffList::ReadFile(const char * idxfilename,const char * lstfilename)
{
	strcpy(IdxFileName,idxfilename);
	strcpy(LstFileName,lstfilename);
	IdxFileTime=FileGetTime(idxfilename);
	fstream idxfile(idxfilename,ios::in|ios::binary);
	if (!idxfile) error("IndexBuffList::ReadFile(): cannot open file.",idxfilename);
	idxfile.seekg(0,ios::end);
	IdxSize=idxfile.tellg()/sizeof(IndexBuffUnit);
	if (IdxSize > MaxSize)
		error("IndexBuffList::ReadFile(): IdxSize > MaxSize.");
	idxfile.seekg(0,ios::beg);
	idxfile.read((char *)Index,IdxSize*sizeof(IndexBuffUnit));
	idxfile.close();
	fstream lstfile(lstfilename,ios::in|ios::binary);
	if (!lstfile) error("IndexBuffList::ReadFile(): cannot open file.",lstfilename);
	lstfile.seekg(0,ios::beg);
	while (lstfile.peek()!=EOF)
	{
		char word[TOKENLENGTH+1];
		lstfile.getline(word,TOKENLENGTH);
		addword(word);
	}
	lstfile.close();
}

void IndexBuffList::WriteFile()
{
	fstream idxfile(IdxFileName,ios::out|ios::binary);
	if (!idxfile) error("IndexBuffList::WriteFile(): cannot open file.",IdxFileName);
	idxfile.seekp(0,ios::beg);
	idxfile.write((char *)Index,IdxSize*sizeof(IndexBuffUnit));
	idxfile.close();
	IdxFileTime=FileGetTime(IdxFileName);
	sortwords();
	fstream lstfile(LstFileName,ios::out|ios::binary);
	if (!lstfile) error("IndexBuffList::WriteFile(): cannot open file.",LstFileName);
	long i;
	for (i=0;i<LstSize;i++) lstfile << Words[i] << endl;
	lstfile.close();
}

/************************************************************
*   Descriptions:   read config information from .CFG       *
* 				    Name.CFG: the config file				*
*					There are additional variables:         *
*				    # SrcLang: source language              *
*					# TgtLang: target language				*
*					# IdxFileSize: the number of index    *
*					  units in the word index file          *
*************************************************************/

EntryBuffUnit * EntryBuffList::srhMeans(const char *word)
{
	EntryBuffUnit * unit=FstMeanUnit;
	while (unit)
	{
		int cmp=strcmp(unit->getKey(),word);
		if (cmp>0) return NULL;
		else if (cmp==0) return unit;
		else unit=unit->getNext();
	}
	return NULL;
}

EntryBuffUnit * EntryBuffList::srhRules(const char *pattn)
{
	EntryBuffUnit * unit=FstRuleUnit;
	while (unit)
	{
		int cmp=strcmp(unit->getKey(),pattn);
		if (cmp>0) return NULL;
		else if (cmp==0) return unit;
		else unit=unit->getNext();
	}
	return NULL;
}

EntryBuffUnit * EntryBuffList::addEntry(const char *word,const char * entry)
{
	EntryBuffUnit * unit=FstMeanUnit, * prev=NULL;
	int cmp;
	while (unit)
	{
		cmp=strcmp(unit->getKey(),word);
		if (cmp<0) { prev=unit; unit=unit->getNext(); }
		else break;
	}
	if (unit && !cmp) return unit;

	char * e=new char[strlen(entry)+1];
	strcpy(e,entry);
	EntryList.addtail(e);

	EntryBuffUnit * meanunit=new EntryBuffUnit;
	meanunit->setKey(word);
	meanunit->Next=unit;
	if (prev) prev->Next=meanunit;
	else FstMeanUnit=meanunit;

	IToken token(e);
	while (token && !token.isDelim("**") && !token.isDelim("&&")) token.input();
	char * end=(char *)token.getInPntr(); end--;
	while (isascii(*end) && isspace(*end)) end--;
	*++end=0;

	while (*token)
	{
		if (token.isDelim("**"))
			{ meanunit->addRule(token.getInPntr()); }
		else
		{
			char pattn[PATTNLENGTH+1];
			PrsRbase::getPattn(token.getInPntr(),pattn);
			unit=FstRuleUnit; prev=NULL;
			while (unit)
			{
				cmp=strcmp(unit->getKey(),pattn);
				if (cmp<0) { prev=unit; unit=unit->getNext(); }
				else break;
			}
			EntryBuffUnit *ruleunit;
			if (unit && !cmp) ruleunit=unit;
			else
			{
				ruleunit=new EntryBuffUnit;
				ruleunit->setKey(pattn);
				ruleunit->Next=unit;
				if (prev) prev->Next=ruleunit;
				else FstRuleUnit=ruleunit;
			}
			ruleunit->addRule(token.getInPntr());
		}
		while (token.input() && !token.isDelim("**") && !token.isDelim("&&"));
		char * end=(char *)token.getInPntr(); end--;
		while (isascii(*end) && isspace(*end)) end--;
		*++end=0;
	}
	return meanunit;
}

void StrListFile::lock_read(const char * filename)
{
	strcpy(FileName,filename);

	char lockfilename[FILENAMELENGTH];
	strcpy(lockfilename,FileName);
	strcat(lockfilename,"_");
	FileLock(lockfilename,"Word List file open Lock.");

	free();
	ifstream file;
	file.open(FileName);
	char line[TOKENLENGTH];
	while (file.peek()!=EOF)
	{
		file.getline(line,TOKENLENGTH-1);
		AddTail(line);
	}
	file.close();
}
void StrListFile::write_unlock()
{
	ofstream file;
	file.open(FileName);
	int i;
	StrLink * link;
	for (i=0,link=GetHead();i<GetNumber();i++,link=link->GetNext())
		file << link->GetData() << endl;
	file.close();

	char lockfilename[FILENAMELENGTH];
	strcpy(lockfilename,FileName);
	strcat(lockfilename,"_");
	FileUnlock(lockfilename);
}

void Dictn::rdCfg()
{

	MKbase::rdCfg();
	rdSrcLang();
	rdTgtLang();
	rdIdxFileSize();
	rdTemplatePath();
	rdTemplateName();

}

void Dictn::rdTemplatePath()
{
	if(!CfgFile) error("Dictn::rdTemplatePath(): CFGOPEN");
	CfgFile.seekg(0,ios::beg);
	char tmp[256];
	char *p;   
	while(CfgFile.peek()!=EOF)
	{
		CfgFile.getline(tmp,255);
		if(strstr(tmp,"TemplatePath"))
		{
			p=strstr(tmp,"=");
			p++;
			strcpy(templatePath,p);
			break;
		}
	}
}

void Dictn::rdTemplateName()
{
	if(!CfgFile) error("Dictn::rdTemplateName(): CFGOPEN");
	CfgFile.seekg(0,ios::beg);
	char tmp[256];
	char *p;   
	while(CfgFile.peek()!=EOF)
	{
		CfgFile.getline(tmp,255);
		if(strstr(tmp,"TemplateName"))
		{
			p=strstr(tmp,"=");
			p++;
			strcpy(templateName,p);
			break;
		}
	}      
}

void Dictn::testCfg()
{
  //	MKbase::testCfg();
  //	if(SrcLang!=hasSrcLang()) error("Dictn::testCfg(): SRCLANG");
}

void Dictn::wtCfg()
{
	MKbase::wtCfg();
	wtSrcLang();
	wtTgtLang();
	wtIdxFileSize();
	wtTemplatePath();
	wtTemplateName();
}

void Dictn::initCfg()
{
	MKbase::initCfg();
	SrcLang=hasSrcLang();
	ValidTextFile=1;
	ValidSwapFile=0;
	/*
	if (TgtLang==SrcLang ) 
		error("Dictn::initCfg(): TgtLang is same as SrcLang.");
	if (mkModel->hasLang(SrcLang)==FALSE) 
		error("Dictn::initCfg(): SrcLang outside Model.");
	if (mkModel->hasLang(TgtLang)==FALSE) 
		error("Dictn::initCfg(): TgtLang outside Model.");
	*/
	if(IdxFileSize>MAXIDXFILESIZE) 
		error("Dictn::initCfg(): IdxFileSize is too big.");
}

void Dictn::rdTgtLang()
{
	if(!CfgFile) error("Dictn::rdTgtLang(): CFGOPEN");
	CfgFile.seekg(0,ios::beg);
	char tmp[256];
	char *p;
	while(CfgFile.peek()!=EOF) { 
		  CfgFile.getline(tmp,255);
		  if(strstr(tmp,"TgtLang")) {
			   p=strstr(tmp,"=");
			   p++;
			   int i;
			   for(i=0;i<2;i++) 
			     if(strstr(p,LANGSTRING[i])) 
			       break;
			   TgtLang=i;
			   return;
		  }
	}
}

void Dictn::wtTgtLang()
{
	//write to CfgFile
	if(!CfgFile) error("Dictn::wtTgtLang(): CFGOPEN");
	CfgFile<<"TgtLang="<<LANGSTRING[TgtLang]<<endl;
}

/************************************************************
*   Descriptions:  read information from .CFG file          *
*				   the definition in .CFG file is in this   *
*				   way:  SrcLang=1							*
*				   1 is the langcode						*
*************************************************************/

void Dictn::rdSrcLang()
{

	if(!CfgFile) error("Dictn::rdSrcLang(): CFGOPEN");
	CfgFile.seekg(0,ios::beg);
	char tmp[256];
	char *p;
	while(CfgFile.peek()!=EOF)
	{ 
		CfgFile.getline(tmp,255);	
		if(strstr(tmp,"SrcLang"))
		{   
			p=strstr(tmp,"=");
			p++;
			int i;
			for(i=0;i<2;i++) 
			  if(strstr(p,LANGSTRING[i])) 
			    break;
			SrcLang=i;
			return;
		}
	}
}

void  Dictn::rdIdxFileSize()
{
	if(!CfgFile) error("Dictn::rdWIdxFileSize(): CFGOPEN");
	CfgFile.seekg(0,ios::beg);
	char tmp[256];
	char *p;
	while(CfgFile.peek()!=EOF)
	{ 
		CfgFile.getline(tmp,255);
		if(strstr(tmp,"IdxFileSize"))
		{
			p=strstr(tmp,"=");
			p++;
			IdxFileSize = atoi(p);
			return;
		}
	}
}

/************************************************************
*   Descriptions: write SrcLang or IdxFileSize			* 
*				  in the CfgFile							*
*************************************************************/

void  Dictn::wtSrcLang()
{
	if(!CfgFile) error("Dictn::wtSrcLang(): CFGOPEN");
	CfgFile.seekp(0,ios::end); 
	CfgFile<<"SrcLang="<<LANGSTRING[SrcLang]<<endl;

}
void Dictn::wtTemplatePath()
{
	if(!CfgFile) error("Dictn::wtTemplatePath():CFGOPEN");
	CfgFile.seekp(0,ios::end);
	CfgFile<<"TemplatePath="<<"template"<<endl;
}
void Dictn::wtTemplateName()
{
	if(!CfgFile) error("Dictn::wtTemplateName():CFGOPEN");
	CfgFile.seekp(0,ios::end);
	CfgFile<<"TemplateName="<<"template"<<endl;
}

void  Dictn::wtIdxFileSize()
{
	if(!CfgFile) error("Dictn::wtIdxFileSize(): CFGOPEN");
	CfgFile.seekp(0,ios::end); 
	CfgFile<<"IdxFileSize="<<IdxFileSize<<endl;
}

void Dictn::readValidFile()
{
	char valfilename[FILENAMELENGTH];
	getFullName(valfilename,".VAL");
	ifstream valfile(valfilename,ios::in|ios::binary);
	valfile >> ValidTextFile >> ValidSwapFile;
	ValidTextFile-='0'; ValidSwapFile-='0';
	valfile.close();
}

void Dictn::writeValidFile()
{
	char valfilename[FILENAMELENGTH];
	getFullName(valfilename,".VAL");
	ofstream valfile(valfilename,ios::out|ios::binary);
	valfile << (char)(ValidTextFile+'0') << endl
			<< (char)(ValidSwapFile+'0') << endl;
	valfile.close();
}

/***************************************************************
*   Descriptions: open the dictionary for read	should be      *
*                 used before rd_Means(),rd_PrsRulesPT(), etc. *
****************************************************************/

void Dictn::ropen()
{
	Kbase::ropen();

	char swpfile[FILENAMELENGTH];
	getFullName(swpfile,".SWP");
	SwpFile.open(swpfile,ios::in|ios::binary);

	char widxfile[FILENAMELENGTH];					
	getFullName(widxfile,".WID");
	char wlstfile[FILENAMELENGTH];					
	getFullName(wlstfile,".LST");
	IndexBuff.open(IdxFileSize,widxfile,wlstfile);
}

/***************************************************************
*   Descriptions:	                             	           *
*      open the dictionary for writing and reading             *
*
      when succeed, the index file and data file are open for *
*      writing should be used with a close()                   *
****************************************************************/


#ifdef KBMS
void Dictn::wopen()
{
	Kbase::wopen();

	char swpfile[FILENAMELENGTH];
	getFullName(swpfile,".SWP");
	SwpFile.open(swpfile,ios::in|ios::out|ios::binary);

	char widxfile[FILENAMELENGTH];	
	getFullName(widxfile,".WID");
	char wlstfile[FILENAMELENGTH];	
	getFullName(wlstfile,".LST");
	IndexBuff.open(IdxFileSize,widxfile,wlstfile);
}
#endif // KBMS

/***************************************************************
*   In parameters:  none  	                                   *
*   Return Value:   none   	                                   *
*   Descriptions:	close the dictionary           	           *
****************************************************************/

void Dictn::close()
{
	Kbase::close();
	IndexBuff.close();
	SwpFile.close();
}

/***************************************************************
*   In parameters:   none                                      *
*   Return Value:    none                                      *
*   Descriptions:	initialize the dictionary      	           *
****************************************************************/

#ifdef KBMS
void Dictn::create()
{
  /*
	if (getModify()==FALSE)
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
	//if (getBuilt()==YES) free();

	//if (getStatus()!=CLOSED)
	//	error("Dictn::create(): cannot create() while not closed.");
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

	// open the CfgFile for writing
	char cnfgfile[FILENAMELENGTH];				
	getFullName(cnfgfile,".CFG");
	CfgFile.open(cnfgfile,ios::out);
	if(!CfgFile) error("Dictn::create(): CFGOPEN");

	initCfg(); // call initCfg
	wtCfg();
	CfgFile.close();

	if (testIdxFileSize(IdxFileSize)==FAIL)
	  error("Dictn::setIdxFileSize(): IdxFileSize is not illegal.");

	char widxfile[FILENAMELENGTH];					
	getFullName(widxfile,".WID");
	char wlstfile[FILENAMELENGTH];					
	getFullName(wlstfile,".LST");
	IndexBuff.create(IdxFileSize,widxfile,wlstfile);

	// open the DatFile for writing, cut the DatFile to zero length
	char datafile[FILENAMELENGTH];			
	getFullName(datafile,".DAT");	
	DatFile.open(datafile,ios::out|ios::binary|ios::trunc);
	if(!DatFile) error("Dictn::create(): cannot open file.",datafile);
	DatFile.close();

	// open the SwpFile for writing, cut the DatFile to zero length
	char swapfile[FILENAMELENGTH];			
	getFullName(swapfile,".SWP");	
	SwpFile.open(swapfile,ios::out|ios::binary|ios::trunc);
	if(!SwpFile) error("Dictn::create(): cannot open file.",swapfile);
	SwpFile.close();
}

void Dictn::DeleteTempFile(char *word)
{
	char wordtmpfile[FILENAMELENGTH],errorfile[FILENAMELENGTH];
	getFullName(wordtmpfile,".TXT.",word);
	getFullName(errorfile,".ERR.",word);
	//if( !access(wordtmpfile,R_OK|W_OK) ) unlink(wordtmpfile);
	//if( !access(errorfile,R_OK|W_OK) ) unlink(errorfile);
}


/***************************************************************
*    Descriptions:clear an old dictionary                      *
****************************************************************/

void Dictn::clear()
{
	char cfgfilename[FILENAMELENGTH];
	getFullName(cfgfilename,".CFG");
	CfgFile.open(cfgfilename,ios::in|ios::out);
	rdCfg();
	setModelDate(mkModel->getDate());
	//CfgFile.close();
	//CfgFile.open(cfgfilename,ios::trunc|ios::out);
	wtCfg();
	CfgFile.close();

	wopen();

	char cnfgfile[FILENAMELENGTH];				
	getFullName(cnfgfile,".CFG");
	CfgFile.open(cnfgfile,ios::out);
	if( !CfgFile ) error("Dictn::clear(): CFGOPEN", cnfgfile );

	initCfg();
	wtCfg();
	CfgFile.close();

	char widxfile[FILENAMELENGTH];					
	getFullName(widxfile,".WID");
	char wlstfile[FILENAMELENGTH];					
	getFullName(wlstfile,".LST");
	IndexBuff.create(IdxFileSize,widxfile,wlstfile);

	// open the DatFile for writing
	char datafile[FILENAMELENGTH];	
	getFullName(datafile,".DAT");		

	// cut the DatFile to zero length
	DatFile.close();
	DatFile.open(datafile,ios::out|ios::binary|ios::trunc);
	if(!DatFile) error("Dictn::clear(): DATOPEN");

	close();
	//unLock();
}

BOOL Dictn::testIdxFileSize(int n)
{
	if (n<=0)
	{
		message("IdxFileSize must be a positive integer.");
		return FAIL;
	}
	int i,j;
	for (i=2,j=n/i;i<j;i++,j=n/i)
	{
		if (n%i==0)
		{
			message("IdxFileSize must be a prime number.");
			return FAIL;
		}
	}
	return SUCC;
}

/***************************************************************
*   Descriptions: Load TxtFile to DatBuff 	 		           *
*				  write DatBuff to DatFile 					   *
****************************************************************/

void Dictn::Check(const char *filename)
{
	try
	{
		ropen();
		beginScan(MAXBUFFER);

		char txtfilename[FILENAMELENGTH];
		if (filename==NULL)
		{
			getFullName(txtfilename,".TXT");
			filename=txtfilename;
		}

		TxtFile.open(filename,ios::in|ios::binary);
		if( !TxtFile ) error("Dictn::Check(): TXTOPEN" ,filename );
		
		CErrorException * err=NULL;
		BOOL entryflag;
		entryflag=TRUE;
		IOToken TheToken;
		while(entryflag=rdTxtEntry()) {
			TitleList titlelist;
			TheToken.setInPntr(TxtBuff);
			TheToken.setOutPntr(DatBuff);
			LoadEntry(TheToken,SrcLang,TgtLang,titlelist);
			titlelist.free();
		}
		TxtFile.close();
		endScan();
		close();
	}
	catch (CErrorException * e)
	{
		TxtFile.close();
		endScan();
		close();
		throw e;
	}
}

void Dictn::Load()
{
  /*
	if (ValidTextFile==0)
		error("Dictn::Load(): The ValidTextFile is FALSE.\n","Cannot Load TXT file.");
  */
  TitleList titlelist;
  try
    {
      wopen(); //以写方式打开.DAT文件
      beginScan(MAXBUFFER);
      char txtfilename[FILENAMELENGTH];
      getFullName(txtfilename,".TXT");     //形成完整的文件路径dictn/dictn.TXT
      TxtFile.open(txtfilename,ios::in|ios::binary);  //以读方式打开dictn.dictn.TXT
      if( !TxtFile ) error("Dictn::Load(): TXTOPEN" ,txtfilename );
      int i;
      char word[TOKENLENGTH];
      IOToken TheToken;
      BOOL entryflag=TRUE;
      while(entryflag=rdTxtEntry())
	{	//把一个词条读入缓冲区TxtBuff
	  //char * wordfile[FILENAMELENGTH];
	  //getFullNameForWord(wordfile,word,TXT);
	  TheToken.setInPntr(TxtBuff);
	  TheToken.setOutPntr(DatBuff);
	  try
	    {
	      i=LoadEntry(TheToken,SrcLang,TgtLang,titlelist);  //装入一个词条
	    }
	  catch (CErrorException *)
	    {
	      message("Entry:",TxtBuff);
	      throw;
	    }
	  DatBuff[i]=0;
	  getWord(DatBuff,word);  //把装入的词条的词语写入词表文件
	  char datentry[MAXBUFFER];
	  char swpentry[MAXBUFFER];
	  strcpy(datentry,DatBuff); 
	  strcpy(swpentry,SwpBuff);
	  wtDatSwpEntry(datentry,swpentry);
	  ValidSwapFile=1;
	  titlelist.free();
	}
      TxtFile.close();
      endScan();
      close();
    }
  catch (CErrorException *)
    {
      titlelist.free();
      TxtFile.close();
      endScan();
      close();
      throw;
    }
}

void Dictn::Load(const char *word,const char * wordtmpfile)
{
	TitleList titlelist;
	try
	{
		TxtFile.open(wordtmpfile,ios::in|ios::binary);          //以读方式打开dictn.WORD文件
		if( !TxtFile || TxtFile.peek()==EOF ) { TxtFile.close(); return; }
		wopen();                                         //以写方式打开.DAT文件
		beginScan(MAXBUFFER);
		IOToken TheToken;                                //把词条读入TxtBuff
		rdTxtEntry(); 
		TheToken.setInPntr(TxtBuff);
		TheToken.setOutPntr(DatBuff);
		if( *TxtBuff ) 
		{
			int i=LoadEntry(TheToken,SrcLang,TgtLang,titlelist); //把该词的词条装入DatBuff
			char Word[TOKENLENGTH];
			getWord(DatBuff,Word);
			if( strcmp(word,Word ) ) //不允许修改词语本身
				error("Dictn::LoadWord(char *): inputting word is not word in File." );
			char datentry[MAXBUFFER];
			char swpentry[MAXBUFFER];
			strcpy(datentry,DatBuff);
			strcpy(swpentry,SwpBuff);
			wtDatSwpEntry(datentry,swpentry);                    // 写入.DAT文件 
		}
		titlelist.free();
		TxtFile.close();
		endScan();
		close();
	}
	catch (CErrorException *)
	{
		titlelist.free();
		TxtFile.close();
		endScan();
		close();
		throw;
	}
}

BOOL Dictn::rdTxtEntry()
{
	return rdTxtRecord("$$",NULL,SwpBuff);
}

void Dictn::Delete(const char *word)
{
	char wordtextfile[FILENAMELENGTH],wordeditfile[FILENAMELENGTH];
	getFullName(wordtextfile,".TXT.",word);
	getFullName(wordeditfile,".ERR.",word);

	wopen();
	beginScan(MAXBUFFER);
	try { delDatSwpEntry(word); }
	catch(CErrorException *) { endScan(); close(); throw; };
	endScan();
	close();
}

/***************************************************************
*   Descriptions: Pack DatFile	                   	           *
****************************************************************/

void Dictn::Pack()
{
	if(Status!=CLOSED) error("Kbase::wopen(): cannot wopen while not closed");

	wtLock();

	// open the CfgFile, if error, error(CFGOPEN)
	char cnfgfile[FILENAMELENGTH];
	getFullName(cnfgfile,".CFG");
	CfgFile.open(cnfgfile,ios::in); //,filebuf::sh_read|filebuf::sh_write);
	if(!CfgFile) error("kbase::wopen(): CFGOPEN",cnfgfile);
	rdCfg();
	//Kbase::testCfg();
	CfgFile.close();

	// open the DatFile for writing, if error, error(DATOPEN)
	char datafile[FILENAMELENGTH];
	getFullName(datafile,".DAT");
	DatFile.open(datafile,ios::in|ios::out|ios::binary);/*);*/
	if(!DatFile) error("Kbase::wopen(): DATOPEN",datafile);

	char swpfile[FILENAMELENGTH];
	getFullName(swpfile,".SWP");
	SwpFile.open(swpfile,ios::in|ios::out|ios::binary);

	char widxfile[FILENAMELENGTH];	
	getFullName(widxfile,".WID");
	char wlstfile[FILENAMELENGTH];	
	getFullName(wlstfile,".LST");
	IndexBuff.open(IdxFileSize,widxfile,wlstfile);

	char dattmpfile[FILENAMELENGTH];
	getFullName(dattmpfile,".DAT.TMP");
	fstream DatTmpFile(dattmpfile,ios::out|ios::binary|ios::trunc);
	if (!DatTmpFile) error("Dictn::Pack(): file cannot open.",dattmpfile);

	char swptmpfile[FILENAMELENGTH];
	getFullName(swptmpfile,".SWP.TMP");
	fstream SwpTmpFile(swptmpfile,ios::out|ios::binary|ios::trunc);
	if (!SwpTmpFile) error("Dictn::Pack(): file cannot open.",swptmpfile);

	char txttmpfile[FILENAMELENGTH];
	getFullName(txttmpfile,".TXT.TMP");
	fstream TxtTmpFile(txttmpfile,ios::out|ios::binary|ios::trunc);
	if (!TxtTmpFile) error("Dictn::Pack(): file cannot open.",txttmpfile);

	DatTmpFile.seekp(0,ios::beg);
	SwpTmpFile.seekp(0,ios::beg);
	TxtTmpFile.seekp(0,ios::beg);

	beginScan(MAXBUFFER);
	long l;
	for (l=0;l<IndexBuff.LstSize;l++)
	{
		char word[TOKENLENGTH];
		strcpy(word,IndexBuff.Words[l]);

		long dvalue=getDstgVal(word);
		long hvalue=getHashVal(word,IdxFileSize);
		long i=hvalue;
		while (i!=-1)
		{
			IndexBuffUnit & index=IndexBuff[i];
			if (index.DstgValue==dvalue && index.DataOffset!=-1)
			{
				DatFile.seekg(index.DataOffset,ios::beg);
				int j=rdDatLength();
				rdDatBuff(j);
				char Word[TOKENLENGTH];
				getWord(DatBuff,Word);
				if(strcmp(Word,word)==0)
				{
					index.DataOffset=DatTmpFile.tellp();
					wtDatLengthToFile(j,DatTmpFile);
					wtDatBuffToFile(DatTmpFile,DatBuff);
					SwpFile.seekg(index.SwapOffset,ios::beg);
					int k=rdDatLengthFromFile(SwpFile);
					rdDatBuffFromFile(k,SwpFile,SwpBuff);
					TxtTmpFile<<SwpBuff;
					index.SwapOffset=SwpTmpFile.tellp();
					wtDatLengthToFile(k,SwpTmpFile);
					wtDatBuffToFile(SwpTmpFile,SwpBuff);
					break;
				}
			}
			i=index.NextIndex;
		}
		if (i==-1)
		{
			endScan();
			DatTmpFile.close();
			SwpTmpFile.close();
			TxtTmpFile.close();
			IndexBuff.free();
			unlink(dattmpfile);
			unlink(swptmpfile);
			unlink(txttmpfile);
			error("Dictn::Pack(): entry not found.",word);
		}
	}
	endScan();
	DatTmpFile.close();
	SwpTmpFile.close();
	TxtTmpFile.close();

	IndexBuff.close();
	SwpFile.close();
	DatFile.close();
	unLock();

	char datfile[FILENAMELENGTH]; getFullName(datfile,".DAT");
	if (unlink(datfile)!=0)
		error("Dictn::Pack(): cannot remove file.",datfile);
	if(rename(dattmpfile,datfile)!=0)
		error("Dictn::Pack(): cannot rename file",dattmpfile,datfile);
	if (unlink(swpfile)!=0)
		error("Dictn::Pack(): cannot remove file.",swpfile);
	if(rename(swptmpfile,swpfile)!=0)
		error("Dictn::Pack(): cannot rename file",swptmpfile,swpfile);
	char txtfile[FILENAMELENGTH]; getFullName(txtfile,".TXT");
	if (unlink(txtfile)!=0)
		error("Dictn::Pack(): cannot remove file.",txtfile);
	if(rename(txttmpfile,txtfile)!=0)
		error("Dictn::Pack(): cannot rename file",swptmpfile,swpfile);
}

void Dictn::rdDatEntry(const char *word,char *entry) //从.DAT文件中读出指定的单词
{
	if (getStatus()==CLOSED) error("Dictn::rdDatEntry(): cannot operate while Dictn is closed.");
	long dvalue=getDstgVal(word);
	long hvalue=getHashVal(word,IdxFileSize);
	long i=hvalue;
	while (i!=-1)
	{
		IndexBuffUnit & index=IndexBuff[i];
		if (index.DstgValue==dvalue && index.DataOffset!=-1)
		{
			DatFile.seekg(index.DataOffset,ios::beg);
			int l=rdDatLength();
			rdDatBuff(l);
			char Word[TOKENLENGTH];
			getWord(DatBuff,Word);
			if(strcmp(Word,word)==0)
			{
				strcpy(entry,DatBuff);
				break;
			}
		}
		i=index.NextIndex;
	}
	if (i==-1) error("Dictn::rdDatEntry(): entry not found.",word);
}

void Dictn::rdSwpEntry(const char *word,char *entry) //从.SWAP文件中读出指定的单词
{
	if (getStatus()==CLOSED) error("Dictn::rdSwpEntry(): cannot operate while Dictn is closed.");
	long dvalue=getDstgVal(word);
	long hvalue=getHashVal(word,IdxFileSize);
	long i=hvalue;
	beginScan(MAXBUFFER);
	char SwpFileName[FILENAMELENGTH];
	getFullName(SwpFileName,".SWP");
	fstream SwpFile(SwpFileName,ios::in|ios::binary);
	while (i!=-1)
	{
		IndexBuffUnit & index=IndexBuff[i];
		if (index.DstgValue==dvalue && index.DataOffset!=-1)
		{
			SwpFile.seekg(index.SwapOffset,ios::beg);
			int l=rdDatLengthFromFile(SwpFile);
			rdDatBuffFromFile(l,SwpFile,SwpBuff);
			char Word[TOKENLENGTH];
			getWord(SwpBuff,Word);
			if(strcmp(Word,word)==0)
			{
				strcpy(entry,SwpBuff);
				break;
			}
		}
		i=index.NextIndex;
	}
	if (i==-1)
	{
		message("The word is not found in the Dictn.",word);
		sprintf(entry,"$$ %s\n\t** {} \n",word);
	}
	SwpFile.close();
	endScan();
}

#endif // KBMS

cRuleList & Dictn::rd_Means(char *word)
{
	if( getBuilt()==NO) 
		error("Dictn::rd_Means(): rd_Means while Built=NO");
	if (getStatus()!=ROPEN) 
		error("Dictn::rd_Means(): rd_Means while not ROPEN.");

	EntryBuffUnit * entry=EntryBuff.srhMeans(word);
	if (entry) return entry->getRuleList();

	int dstgval=getDstgVal(word);
	int hashval=getHashVal(word,IdxFileSize);
	long idxoffs=hashval;

	BOOL Find=FALSE;
	char Word[TOKENLENGTH];
	beginScan(MAXBUFFER);
	while (idxoffs!=-1 && !Find)
	{
		IndexBuffUnit & index=IndexBuff[idxoffs];
		if(index.DstgValue==dstgval  && index.DataOffset!=-1)
		{
			DatFile.seekg(index.DataOffset,ios::beg);
			int i=rdDatLength();
			rdDatBuff(i);
			getWord(DatBuff,Word);
			if(!strcmp(Word,word)) Find=TRUE;
		}
		idxoffs=index.NextIndex;
	}

	cRuleList * rulelist;
	if(Find) rulelist=&EntryBuff.addEntry(Word,DatBuff)->getRuleList();
	else rulelist=(cRuleList *)NULL;

	endScan();
	return *rulelist;
}

/***************************************************************
*	In parameters:	char *pattern,default is NULL			   *
*	Returnvalue:    a _PrsRule 	     						   *
*	Descriptions:	read _PrsRulse from the buffer  		   * 
*					through the PTIdxBuff					   *
****************************************************************/

cRuleList & Dictn::rd_PrsRulesPT(const char *pattern)
{
	EntryBuffUnit * unit=EntryBuff.srhRules(pattern);
	if (unit) return unit->getRuleList();
	return *(cRuleList *)NULL;
}

/***************************************************************
*   In parameters:   char * entry;                             *
*   			     string pointer to the word's entry        *
*   Descriptions:	 write the entry in memory to dictionary   *
*                    and modify the index file correspondently *
****************************************************************/

#ifdef KBMS
void Dictn::wtDatSwpEntry(char * DatEntry,char * SwpEntry)
{
  if (getStatus()==CLOSED) error("Dictn::wtDatSwpEntry(): cannot operate while Dictn is closed.");
  // offset in DatFile
  DatFile.seekp(0,ios::end);
  long DatEntryOffs=DatFile.tellp();
  SwpFile.seekp(0,ios::end);
  long SwpEntryOffs=SwpFile.tellp();
  // delete ending spaces
  char * end=DatEntry+strlen(DatEntry)-1;
  while( isascii(*end) && isspace(*end) ) { end--; }
  *++end=0;
  // read word from DatEntry
  char word[TOKENLENGTH];
  getWord(DatEntry,word);
  // get the word'DstgVal and the word'HashVal 
  int  DstgVal=getDstgVal(word);    		
  long HashVal=getHashVal(word,IdxFileSize); 
  // calculate Offset(IdxPos) in IdxFile
  // according to IdxPos, read the IdxFileUnit(idx) from IdxFile
  long IdxPos=HashVal;
  // Compare the DstgVal,find the right index unit
  char Word[TOKENLENGTH]; 
  while (TRUE)
    {
      IndexBuffUnit & index=IndexBuff[IdxPos];
      // if IdxFileUnit(idx) is empty,add DstgVal and DatEntryOffs to IdxFile
      if(index.DataOffset==-1)
	{
	  index.DstgValue=DstgVal;
	  index.DataOffset=DatEntryOffs;
	  index.SwapOffset=SwpEntryOffs;
	  index.NextIndex=-1;
	  strcpy(DatBuff,DatEntry);
	  DatFile.seekp(0,ios::end);
	  wtDatLength(strlen(DatEntry));
	  wtDatBuff();
	  strcpy(SwpBuff,SwpEntry);
	  SwpFile.seekp(0,ios::end);
	  wtDatLengthToFile(strlen(SwpBuff),SwpFile);
	  wtDatBuffToFile(SwpFile,SwpBuff);
	  IndexBuff.change();
	  IndexBuff.addword(word);
	  /*
	   */
	  return;
	}
      if(index.DstgValue==DstgVal)
	{
	  // according to DatEntryOffs , read the DatEntry from DatFile 
	  DatFile.seekg(index.DataOffset,ios::beg);
	  int i=rdDatLength();
	  rdDatBuff(i);
	  getWord(DatBuff,Word);
	  if(strcmp(Word,word)==0)
	    {
	      if( strcmp(DatEntry,DatBuff)!=0 )
		{
		  // if find the DatEntry in DatFile 
		  // According to the DataOff find the Entry in DatFile
		  // find out the newLen and compare it with oldLen
		  strcpy(DatBuff,DatEntry);
		  int newLen=strlen(DatEntry);
		  DatFile.seekp(index.DataOffset,ios::beg);
		  int oldLen=rdDatLength();
		  if(oldLen>=newLen)
		    {
		      // fill the end of DatBuff with blanks
		      char * p=DatBuff+newLen;
		      while (p<DatBuff+oldLen) *p++='\0';
		      *p=0;
		      // write the DatEntry to the original position in data file
		      DatFile.seekp(index.DataOffset,ios::beg);
		      wtDatLength(oldLen);
		      wtDatBuff(oldLen);
		    }
		  else
		    {
		      // change the length to its negative to show 
		      // that the DatEntry has been changed and moved elsewhere
		      DatFile.seekp(index.DataOffset,ios::beg);
		      wtDatLength(-oldLen);
		      // get the position in DatFile
		      DatFile.seekp(0,ios::end);
		      // write the new Entry to the end of the DatFile
		      wtDatLength(newLen);
		      wtDatBuff();
		      // change the index unit's DatEntryOffs to the new position
		      index.DataOffset=DatEntryOffs;
		      IndexBuff.change();
		    }
		}
	      SwpFile.seekg(index.SwapOffset,ios::beg);
	      int j=rdDatLengthFromFile(SwpFile);
	      rdDatBuffFromFile(j,SwpFile,SwpBuff);
	      if( strcmp(SwpEntry,SwpBuff)!=0 )
		{
		  // if find the DatEntry in DatFile 
		  // According to the DataOff find the Entry in DatFile
		  // find out the newLen and compare it with oldLen
		  strcpy(SwpBuff,SwpEntry);
		  int newLen=strlen(SwpEntry);
		  SwpFile.seekp(index.SwapOffset,ios::beg);
		  int oldLen=rdDatLengthFromFile(SwpFile);
		  if(oldLen>=newLen)
		    {
		      // fill the end of DatBuff with blanks
		      char * p=SwpBuff+newLen;
		      while (p<SwpBuff+oldLen) *p++='\0';
		      *p=0;
		      // write the DatEntry to the original position in data file
		      SwpFile.seekp(index.SwapOffset,ios::beg);
		      wtDatLengthToFile(oldLen,SwpFile);
		      wtDatBuffToFile(SwpFile,SwpBuff,oldLen);
		    }
		  else
		    {
		      // change the length to its negative to show 
		      // that the DatEntry has been changed and moved elsewhere
		      SwpFile.seekp(index.SwapOffset,ios::beg);
		      wtDatLengthToFile(-oldLen,SwpFile);
		      // get the position in DatFile
		      SwpFile.seekp(0,ios::end);
		      // write the new Entry to the end of the DatFile
		      wtDatLengthToFile(newLen,SwpFile);
		      wtDatBuffToFile(SwpFile,SwpBuff);
		      // change the index unit's DatEntryOffs to the new position
		      index.SwapOffset=SwpEntryOffs;
		      IndexBuff.change();
		    }
		}
	      return;
	    }
	}
      if(index.NextIndex==-1)
	{
	  index.NextIndex=IndexBuff.IdxSize;
	  IndexBuffUnit & newindex=IndexBuff[IndexBuff.IdxSize];
	  newindex.DstgValue=DstgVal;
	  newindex.DataOffset=DatEntryOffs;
	  newindex.SwapOffset=SwpEntryOffs;
	  newindex.NextIndex=-1;
	  IndexBuff.IdxSize++;
	  if (IndexBuff.IdxSize>=IndexBuff.MaxSize)
	    error("Dictn::wtDatSwpEntry(): too many entries.",
		  "Please select a bigger HashSize.");
	  strcpy(DatBuff,DatEntry);
	  DatFile.seekp(0,ios::end);
	  wtDatLength(strlen(DatEntry));
	  wtDatBuff();
	  strcpy(SwpBuff,SwpEntry);
	  SwpFile.seekp(0,ios::end);
	  wtDatLengthToFile(strlen(SwpBuff),SwpFile);
	  wtDatBuffToFile(SwpFile,SwpBuff);
	  IndexBuff.change();
	  IndexBuff.addword(word);
	  return;
	}
      else IdxPos=index.NextIndex;
    }
}

void Dictn::delDatSwpEntry(const char *word)
{
	int DstgVal=getDstgVal(word);
	int HashVal=getHashVal(word,IdxFileSize);

	long idxoffs=HashVal; 

	char Word[TOKENLENGTH];
	BOOL find=FALSE;
	while (idxoffs!=-1)
	{
		IndexBuffUnit & index=IndexBuff[idxoffs];
		if( index.DstgValue==DstgVal && index.DataOffset!=-1) 
		{
			DatFile.seekg(index.DataOffset,ios::beg);
			int i=rdDatLength();
			rdDatBuff(i);
			getWord(DatBuff,Word);
			if( strcmp(Word,word)==0 )
			{
				char message[255];
				sprintf(message,"Do you really want to delete the word %s ?",word);
				if (yesno(message)==FALSE) return;

				DatFile.seekp(index.DataOffset,ios::beg);
				wtDatLength(-i);

				SwpFile.seekg(index.SwapOffset,ios::beg);
				int j=rdDatLengthFromFile(SwpFile);
				rdDatBuffFromFile(j,SwpFile,SwpBuff);
				SwpFile.seekp(index.SwapOffset,ios::beg);
				wtDatLengthToFile(-j,SwpFile);

				char delfilename[FILENAMELENGTH];
				getFullName(delfilename,".DEL");
				fstream delfile(delfilename,ios::in|ios::app|ios::binary);
				delfile << SwpBuff << endl << endl;
				delfile.close();

				long next=IndexBuff[idxoffs].NextIndex;
				while (next!=-1)
				{
					IndexBuff[idxoffs].DstgValue=IndexBuff[next].DstgValue;
					IndexBuff[idxoffs].DataOffset=IndexBuff[next].DataOffset;
					IndexBuff[idxoffs].SwapOffset=IndexBuff[next].SwapOffset;
					idxoffs=next;
					next=IndexBuff[idxoffs].NextIndex;
				}
				IndexBuff[idxoffs].DstgValue=0;
				IndexBuff[idxoffs].DataOffset=-1;
				IndexBuff[idxoffs].SwapOffset=-1;
				IndexBuff.change();
				IndexBuff.delword(word);
				return;
			}
		}
		idxoffs=IndexBuff[idxoffs].NextIndex;
	}
}
#endif // KBMS

void Dictn::getWord(char *entry, char *word)
{
	enum { BEGIN, NOTE, WORD, END } state=BEGIN, oldstate;
	char * p=entry;
	while ( *p && state!=END)
	{
		if (state==BEGIN)
		{
			if (*p=='$' && *(p+1)=='$')
				{ state=WORD; p+=2; continue; }
			if (*p=='/' && *(p+1)=='*')
				{ oldstate=state; state=NOTE; p+=2; continue; }
		}
		else if (state==NOTE)
		{
			if (*p=='*' && *(p+1)=='/')
				{ state=oldstate; p+=2; continue; }
		}
		else // (state==WORD)
		{
			while (isascii(*p) && isspace(*p)) { p++; }
			if (*p=='/' && *(p+1)=='*')
				{ oldstate=state; state=NOTE; p+=2; continue; }
			IToken token(p); token.input();
			if (!token.isIdent())
				error("Dictn::getWord(): entry syntax error.",entry);
			strcpy(word,token);
			state=END;
		}
		p++;
	}
	if (*p==0) error("Dictn::getWord(): entry syntax error.",entry);
}

#ifdef KBMS
int Dictn::LoadEntry(IOToken &TheToken,int srclang,int tgtlang,TitleList &titlelist)
{
	char word[TOKENLENGTH];
	const char *errmsg=TheToken.getInPntr();
	char *p=TheToken.getOutPntr();
	TheToken.input();
	if(!TheToken.isEqualTo("$$")) 
		error("Dictn::LoadEntry()\nWrong: Should be \"$$\"",TheToken.getBuffer(),errmsg);
	TheToken.output();
	TheToken.output(' ');
	TheToken.input(); //输入词语
	strcpy(word,TheToken.getBuffer());
	LoadWord(TheToken,errmsg);
	if(!TheToken.isEqualTo("**")) 
		error("Dictn::LoadEntry()\nWrong: Should be \"**\"",TheToken.getBuffer(),errmsg);
	try
	{
		TheToken.outputNewLine();
		LoadMeaning(TheToken,srclang,tgtlang,titlelist,errmsg);
		while(TheToken.isEqualTo("**") || TheToken.isEqualTo("&&"))
			if(TheToken.isEqualTo("**")) 
				LoadMeaning(TheToken,srclang,tgtlang,titlelist,errmsg);
			else { 
				TheToken.redrawin();
				LoadPrsRule(TheToken,tgtlang,srclang,titlelist,errmsg);
				TheToken.redrawout();
			}
	}
	catch (CErrorException *)
	{
		error("Dictn::LoadEntry(): error occur.",word);
	}
	if(!TheToken.isEqualTo(""))
		error("Dictn::LoadEntry()\nWrong: Incorrect end",TheToken.getBuffer(),errmsg);
	TheToken.outputNewLine();
	int num=TheToken.getOutPntr()-p;
	TheToken.outputNull();
	return num;
}

void Dictn::LoadMeaning(IOToken &TheToken,int srclang,int tgtlang,TitleList &titlelist,const char *err)
{
	TheToken.output(' ');
	TheToken.output();
	TheToken.input();
	if(!TheToken.isEqualTo("{")) 
		error("Dictn::LoadMeaning()\nWrong: Should be \"{\"",TheToken.getBuffer(),err);
	TheToken.output(' ');
	TheToken.output();
	TheToken.input();
	LoadTitle(TheToken,titlelist,err);
	if(!TheToken.isEqualTo("}")) 
		error("Dictn::LoadMeaning()\nWrong: Should be \"}\"",TheToken.getBuffer(),err);
	TheToken.output();
	TheToken.output(' ');
	TheToken.input();
	LoadSrcMeaning(TheToken,srclang,err);
	if( TheToken.isEqualTo("||") ) 
	{
		TheToken.output(' ');
		TheToken.output();
		TheToken.output(' ');
		TheToken.input();
		LoadBind(TheToken,srclang,err);
	}
	while( TheToken.isEqualTo("=>") ) 
	{
		TheToken.output();
		TheToken.input();
  	 	LoadTgtMeaning(TheToken,srclang,tgtlang,err);
		while( TheToken.isEqualTo("||") )
		{
			TheToken.output(' ');
			TheToken.output();
			TheToken.output(' ');
			TheToken.input();
			LoadBind(TheToken,srclang,err);
		}
	}
	TheToken.outputNewLine();
}

void Dictn::LoadSrcMeaning(IOToken &TheToken,int srclang,const char *err)
{
	LoadLabelItem(TheToken,srclang,err);
	if(TheToken.isEqualTo("$") || TheToken.getBuffer()[0]=='%')
	{	
		TheToken.output(' ');
		LoadBind(TheToken,srclang,err);
	}
}

void Dictn::LoadTgtMeaning(IOToken &TheToken,int srclang,int tgtlang,const char *err)
{
	LoadTrnStruct(TheToken,tgtlang,srclang,err); 
}

void Dictn::setModifyOn()
{
	if (WordModify==TRUE) return;
	readValidFile();
	if (ValidTextFile==FALSE)
	{
		message("The Text File is not valid.","Please Restore the Dictn.");
		return;
	}
	if (addEditingWords()==FAIL)
	{
		message("Conflict on Dictn editing.");
		return;
	}
	Kbase::setModifyOn();
}

void Dictn::setModifyOff()
{
	if (Modify==FALSE) return;
	try { delEditingWords(); }
	catch (CErrorException *) {}
	Kbase::setModifyOff();
}

void Dictn::update()
{
  /*
	if (Modify==FALSE)
	{
		message("The kbase is not on Modify status,",
				"Please turn it on.");
		return;
	}
  */
	try
	{
	  create();
	  Load();
	  ValidTextFile=TRUE;
	  ValidSwapFile=TRUE;
	  writeValidFile();
	}
	catch (CErrorException *) {}
}

void Dictn::Restore()
{
	readValidFile();
	if (ValidTextFile==TRUE)
	{
		if (yesno("The Text File is valid.\nDo you still want to restore the Dictn?",FALSE)==FALSE)
		return;
	}
	if (ValidSwapFile==FALSE)
	{
		message("The Swap File is not valid.","Cannot Restore.");
		return;
	}
	if (WordModify==TRUE || Modify==TRUE)
	{
		message("Cannot Restore while Modify or WordModify is On.");
		return;
	}
	if (addEditingWords()==FAIL)
	{
		message("Conflict on Dictn editing.");
		return;
	}
	try
	{
		Pack();
		ValidTextFile=TRUE;
		ValidSwapFile=TRUE;
		writeValidFile();
	}
	catch (CErrorException *) {}
	delEditingWords();
}

void Dictn::edit()
{
	if (ValidSwapFile==TRUE)
	{
		ValidSwapFile=FALSE;
		writeValidFile();
	}
	Kbase::edit();
}

void Dictn::setModifyWordOn(const char * word)
{
	if (*word==0) return;
	if (Modify==TRUE) return;
	readValidFile();
	if (ValidSwapFile==FALSE)
	{
		message("The Swap File is not valid.","Please update the Dictn.");
		return;
	}
	strcpy(WordCurrent,word);
	if (addEditingWords(WordCurrent)==FAIL)
	{
		message("Conflict on Dictn editing.");
		return;
	}
	char entry[MAXBUFFER];
	ropen();
	try
	{
		rdSwpEntry(WordCurrent,entry);
		char textfilename[FILENAMELENGTH];
		getFullName(textfilename,".TXT.",WordCurrent);
		ofstream textfile(textfilename,ios::out|ios::binary);
		textfile << entry;
		textfile.close();
		char editfilename[FILENAMELENGTH];
		getFullName(editfilename,".EDIT.",WordCurrent);
		ofstream editfile(editfilename,ios::out|ios::binary);
		editfile << entry;
		editfile.close();
		WordModify=TRUE;
	}
	catch (CErrorException *) {}
	close();
}

void Dictn::setModifyWordCurrentOff()
{
	if (WordModify==FALSE) return;
	delEditingWords(WordCurrent);
	WordModify=FALSE;
	char textfilename[FILENAMELENGTH];
	getFullName(textfilename,".TXT.",WordCurrent);
	unlink(textfilename);
	char editfilename[FILENAMELENGTH];
	getFullName(editfilename,".EDIT.",WordCurrent);
	unlink(editfilename);
}

void Dictn::ViewWord(const char * word)
{
	ropen();
	char entry[MAXBUFFER];
	try
	{
		rdSwpEntry(word,entry);
		char viewfilename[FILENAMELENGTH];
		getFullName(viewfilename,".WORD");
		ofstream viewfile(viewfilename,ios::out|ios::binary|ios::trunc);
		viewfile << entry;
		viewfile.close();
		ShellTextOpen(viewfilename);
	}
	catch(CErrorException *) {}
	close();
}

void Dictn::EditWordCurrent()
{
	if (WordModify==FALSE)
	{
		message("The kbase is not on WordModify status,\n"
				"Please turn it on.");
		return;
	}
	char textfilename[FILENAMELENGTH];
	getFullName(textfilename,".TXT.",WordCurrent);
	ShellTextOpen(textfilename);
}

void Dictn::UndoeditWordCurrent()
{
	if (WordModify==FALSE)
	{
		message("The kbase is not on Modity status.\n"
				"Cannot Undo Edit.");
		return;
	}
	char editfile[FILENAMELENGTH];
	getFullName(editfile,".EDIT.",WordCurrent);
	char textfile[FILENAMELENGTH];
	getFullName(textfile,".TXT.",WordCurrent);
	if (yesno("Do you really want to undo your change?",NO)==YES)
		FileCopyTo(editfile,textfile);
}

void Dictn::UpdateWordCurrent()
{
	if (WordModify==FALSE)
	{
		message("The kbase is not on Modity status.\n"
				"Cannot Undo Edit.");
		return;
	}
	if (ValidTextFile==TRUE)
	{
		ValidTextFile=FALSE;
		writeValidFile();
	}
	try
	{
		char textfile[FILENAMELENGTH];
		getFullName(textfile,".TXT.",WordCurrent);
		Load(WordCurrent,textfile);
	}
	catch (CErrorException *) {}
}

void Dictn::DeleteWordCurrent()
{
	if (WordModify==FALSE)
	{
		message("The kbase is not on Modity status.\n"
				"Cannot Undo Edit.");
		return;
	}
	if (ValidTextFile==TRUE)
	{
		ValidTextFile=FALSE;
		writeValidFile();
	}
	try
	{
		Delete(WordCurrent);
		setModifyWordCurrentOff();
		*WordCurrent=0;
	}
	catch (CErrorException *) { }
}

int  Dictn::addEditingWords(const char * word)
{
	if (MultiUser==FALSE) return SUCC;
	int returnvalue=SUCC;
	if (word && strcmp(word,"*")==0)
		error("Dictn::addEditingWordsI(): You cannot edit a word \"*\".");
	char wordfilename[FILENAMELENGTH];
	getFullName(wordfilename,".WDS");
	StrListFile list;
	list.lock_read(wordfilename);
	if (list.GetNumber()!=0 && strcmp(list.GetHead()->GetData(),"*")==0)
		returnvalue=FAIL;
	else if (word==NULL && list.GetNumber()==0)
		list.AddHead("*");
	else if (word!=NULL && list.Find(word)==NULL)
		list.AddHead(word);
	else
		returnvalue=FAIL;
	list.write_unlock();
	return returnvalue;
}

void Dictn::delEditingWords(const char * word)
{
	if (MultiUser==FALSE) return;
	char wordfilename[FILENAMELENGTH];
	getFullName(wordfilename,".WDS");
	StrListFile list;
	list.lock_read(wordfilename);
	if (word==NULL) word="*";
	list.Delete(list.Find(word));
	list.write_unlock();
}

#endif // KBMS

