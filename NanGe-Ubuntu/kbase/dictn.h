#ifndef DICTN_H

#define DICTN_H

#include "../basic/list.h"
#include "prsrbase.h"

const int MAXIDXFILESIZE=100000;

class IndexBuffUnit
{
	unsigned short DstgValue;
	long  NextIndex;
	long  DataOffset;
	long  SwapOffset;
	friend class IndexBuffList;
	friend class Dictn;
public:
	unsigned short getDstgValue() { return DstgValue; }
	long  getNextIndex() { return NextIndex; }
	long  getDataOffset() { return DataOffset; }
	long  getSwapOffset() { return SwapOffset; }
};

class IndexBuffList
{
	char ** Words;
	IndexBuffUnit * Index;
	char LstFileName[FILENAMELENGTH];
	char IdxFileName[FILENAMELENGTH];
	unsigned long IdxFileTime;
	unsigned char Changed;
	long HashSize;  // Size of Hash Table
	long IdxSize; // Size of Index File
	long LstSize; // size of List File
	long MaxSize;
	friend class Dictn;
public:
	IndexBuffList()
	{
		Index=NULL; Words=NULL; Changed=0; *IdxFileName=*LstFileName=0;
		IdxFileTime=HashSize=IdxSize=LstSize=MaxSize=0;
	}
	~IndexBuffList() { free(); }
	void free();
	void create(long hashsize,const char * idxfilename,const char * lstfilename);
	void open(long hashsize,const char * idxfilename,const char * lstfilename);
	void close() { if (Changed) { WriteFile(); Changed=FALSE; } }
	void change() { Changed=TRUE; }
	void addword(const char * word);
	void delword(const char * word);
	void sortwords() { qsort(Words,LstSize,sizeof(char *),compare); }
	IndexBuffUnit & operator [] (long i) { return Index[i]; }
private:
	void alloc(long hashsize);
	void ReadFile(const char * idxfilename,const char * lstfilename);
	void WriteFile();
	static int compare(const void *arg1,const void *arg2);
};

class EntryBuffUnit
{
	char * Key;
	cRuleList RuleList;
	EntryBuffUnit * Next;

  public:

	EntryBuffUnit(){ Key=NULL; Next=NULL; }
	~EntryBuffUnit() { free(); }

	void free() { delete Key; Key=NULL; RuleList.free(); }

	char *  getKey(){ return Key;}
	class cRuleList & getRuleList(){ return RuleList;}
	EntryBuffUnit *  getNext(){ return Next;}

	void setKey(const char *key)
		{ Key=new char[strlen(key)+1]; strcpy(Key,key); }
	void addRule(const char * rule)
		{ RuleList.addhead((char *)rule); }

	friend class EntryBuffList;
};

class EntryBuffList
{
	cRuleList EntryList;
	EntryBuffUnit * FstMeanUnit;
	EntryBuffUnit * FstRuleUnit;

  public:

	EntryBuffList() { FstMeanUnit=FstRuleUnit=NULL; }
	~EntryBuffList() { free(); }

	void free()
	{
		EntryList.freeRules();
		EntryBuffUnit * unit=FstMeanUnit, * next;
		while (unit)
		{
			next=unit->getNext();
			delete unit;
			unit=next;
		}
		FstMeanUnit=NULL;
		unit=FstRuleUnit;
		while (unit)
		{
			next=unit->getNext();
			delete unit;
			unit=next;
		}
		FstRuleUnit=NULL;
	}

	EntryBuffUnit * srhMeans(const char * key);
	EntryBuffUnit * srhRules(const char * key);
	EntryBuffUnit * addEntry(const char * key, const char * entry);
};

class StrListFile : public StrList
{
	char FileName[FILENAMELENGTH];
public:
	void lock_read(const char * filename);
	void write_unlock();
};

class Dictn : public MKbase
{

  protected:

	int SrcLang;
	int TgtLang;
	int IdxFileSize; 			  // File Size of the Index file
	char ValidTextFile;
	char ValidSwapFile;
	char templatePath[FILENAMELENGTH];
	char templateName[FILENAMELENGTH];
	char * SwpBuff;
	fstream TmpFile;
	fstream SwpFile;
	IndexBuffList IndexBuff;
	EntryBuffList EntryBuff;
#ifdef KBMS
	char WordCurrent[WORDLENGTH];
	unsigned char WordModify;
#endif

	virtual void endScan()
	{
		if (SwpBuff) { delete[] SwpBuff; SwpBuff=NULL; }
		Kbase::endScan();
	}
	virtual void beginScan(int buffersize)
	{
		Kbase::beginScan(buffersize);
		SwpBuff=new char[buffersize];
	}

#ifdef KBMS
	static void LoadSrcMeaning(IOToken &TheToken,int srclang,const char *err);
	static void LoadTgtMeaning(IOToken &TheToken,int srclang,int tgtlang,const char *err);
#endif
	void rdCfg();
	void testCfg();
	void wtCfg();
	void initCfg();
  public:
	void ropen();
  protected:
#ifdef KBMS
	void wopen();
	void copen();
#endif
  public:
	void close();
  protected:

	void rdTgtLang();			// read from CfgFile
	void wtTgtLang();			// write CfgFile

	void rdTemplatePath();      /////////////////////////////
	void wtTemplatePath();      //  从.CFG文件中读或向     //
                                //  .CFG文件中写模板的     //
	void rdTemplateName();      //  的路径及文件名         //
	void wtTemplateName();      /////////////////////////////

	void rdSrcLang();
	void wtSrcLang();

	void rdIdxFileSize();
	void wtIdxFileSize();

	void readValidFile();
	void writeValidFile();

#ifdef KBMS
  public:
	BOOL rdTxtEntry();
	void rdDatEntry(const char *word,char *entry);
	void rdSwpEntry(const char *word,char *entry);
	void wtDatSwpEntry(char * datentry,char * swpentry);
	void delDatSwpEntry(const char *word);
#endif
  protected:
	static void getWord(char *entry,char *word);
	virtual int getDstgVal(const char *word)=0;
	virtual int getHashVal(const char *word,int Len)=0;

  public:

	Dictn()
	{
		IdxFileSize=0; SwpBuff=NULL;
#ifdef KBMS
		*WordCurrent=0; WordModify=FALSE;
#endif
	}
	~Dictn(){ free(); }

	char * hasType() { return "DICTN"; }

	int getSrcLang() { return SrcLang; }
	void setTgtLang(int Language)
	{
		if (getStatus()!=CLOSED) 
			error("ChnDictn:: set TgtLang while OPEN.");
		else TgtLang=Language;
	}
	int getTgtLang() { return TgtLang; }

	const char * getTemplatePath() { return templatePath;}
	const char * getTemplateName() { return templateName;}

	virtual int hasSrcLang()=0;

	int  getIdxFileSize() { return IdxFileSize;}
	int  getValidTextFile() { return ValidTextFile; }
	int  getValidSwapFile() { return ValidSwapFile; }

#ifdef KBMS

	const char * getWordCurrent() { return WordCurrent; }
	unsigned int getWordModify() { return WordModify; }

	BOOL setIdxFileSize(int Size)
	{
		if (testIdxFileSize(Size)==FAIL)
			error("Dictn::setIdxFileSize(): IdxFileSize is not illegal.");
		if(getStatus()!=CLOSED) return FAIL;
		else { IdxFileSize = Size; return SUCC;}
	}

	int  addEditingWords(const char * word=NULL);
	void delEditingWords(const char * word=NULL);

	void setModifyOn();
	void setModifyOff();
	void update();
	void edit();
	void create();
	void Check(const char *filename=NULL);
	void Load();
	void Pack();
	void Restore();
	void clear();
	BOOL testIdxFileSize(int n);

	void edit(char *word);
	void Delete(const char *word);
	void Load(const char * word,const char * file);
	void DeleteTempFile(char *word);

	void setModifyWordOn(const char * word);
	void setModifyWordCurrentOff();
	void ViewWord(const char * word);
	void EditWordCurrent();
	void UndoeditWordCurrent();
	void UpdateWordCurrent();
	void DeleteWordCurrent();

	int  LoadEntry(IOToken &TheToken,int srclang,int tgtlang,TitleList &titlelist);
	void LoadMeaning(IOToken &TheToke,int srclang,int tgtlang,TitleList &titlelist,const char *err);

#endif

	void beginConsult() { ropen(); }
	void endConsult() { close(); }
	void clrConsult() { EntryBuff.free(); }

	cRuleList & rd_Means(char *word);
	cRuleList & rd_PrsRulesPT(const char *pattern);	// return rule pointer

	void build()
	{
		if(getBuilt()==YES) return;
		try
		{
			ropen();
			close();
			MKbase::build();
		}
		catch (CErrorException *) { close(); throw; }
	}
	void free() 
	{ 
		if(getBuilt()==NO) return;
		EntryBuff.free(); 
		MKbase::free();
	}
};


#endif // DICTN_H

