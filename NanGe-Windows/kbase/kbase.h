
#ifndef KBASE_H
#define KBASE_H

/*#include <unistd.h>*/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <fstream>
#include <iomanip>
#include "../basic/agree.h"
#include "../basic/constant.h"

using namespace std;

const int MAXBUFFER=40000;
const int OFFSETLENGTH=8;

class Attribute;
class AttList;
class UnsortAttList;
class SortedAttList;
class CondLink;
class CondList;
class Value;
class ValLink;
class cValList;

enum { NAME, ALIAS };
enum { UNLIMITED,LIMITED };
enum { INTERNAL,EXTERNAL };
enum { LEXICAL,PHRASAL };
enum { IRRELATIVE, MTRELATIVE};

enum { UNLOCK, LOCKED };
enum { STATIC, DYNAMIC };
enum { CLOSED, ROPEN, WOPEN };


class Value
{
	unsigned NameAlias: 1;
	char String[TOKENLENGTH];	
    Value *Refer;
public:
	Value() { Refer=NULL; }
	void setNameAlias(unsigned name){ NameAlias=name; }
	void setString(const char *string){ strcpy(String,string); }
	void setRefer(Value *value){ Refer=value; }
	unsigned getNameAlias(){ return NameAlias; }
	const char *getString(){ return String; }
	Value *getRefer(){ return Refer; }
};

class ValLink
{
	Value   *Val;	
	ValLink *Next;
public:
	ValLink(Value *val){ Val=val;Next=NULL; }
	void setNext(ValLink *next){ Next=next; }
	Value *getVal(){ return Val; }
	ValLink *getNext(){ return Next; }
};

class cValList
{
	ValLink	*ValLinkHead;
	ValLink	*ValLinkTail;
public:
	cValList() { ValLinkHead=NULL; ValLinkTail=NULL; }
	~cValList() { free(); }
	void free()
		 {
			ValLink *link, *next;
			link=ValLinkHead;
			while(link) {
				next=link->getNext();
				delete link->getVal();
				delete link;
				link=next;
			}
			ValLinkHead=ValLinkTail=NULL;
		 }
	void setValLinkHead(ValLink *vallink){ ValLinkHead=vallink; }
	void setValLinkTail(ValLink *vallink){ ValLinkTail=vallink; }
	ValLink	*getValLinkHead(){ return ValLinkHead; }
	ValLink *getValLinkTail(){ return ValLinkTail; }
	void addto(Value * val);  // add the Value to the head of the ValList
 	Value *	find(const char *tokenbuff);
};

class Attribute
{
	int Lang:3;
	unsigned Limit:1;
	unsigned NameAlias: 1;
	unsigned AttType: 2;
	unsigned IntExt: 1;
	unsigned Relative:2;
	unsigned StaDyn:1;
	unsigned LexPhr:1;
	char String[FILENAMELENGTH];	
	Attribute *	Refer;
						// the pointer to the Alias for NameAlias==NAME
						// or the pointer to the Name for NameAlias==ALIAS
	char * Default;
	cValList ValList;	// the value list (only for NameAlias==NAME)
public:
	Attribute(){ Default=NULL;Refer=NULL; }
	~Attribute() { free(); }
	void free()
		 {
		   if (Default)
		     delete[] Default;
		   ValList.free();
		 }
	void setLang(int lang){ Lang=lang; }
	void setLimit(unsigned limit){ Limit=limit; }
	void setNameAlias(unsigned name){ NameAlias=name; }
	void setAttType(unsigned type){ AttType=type; }
	void setIntExt(unsigned intext){ IntExt=intext; }
	void setRelative(unsigned relative){ Relative=relative; }
	void setStaDyn(unsigned stadyn) { StaDyn=stadyn; }
	void setLexPhr(unsigned lexphr) { LexPhr=lexphr; }
	void setString(const char *string){ strcpy(String,string); }
	void setRefer(Attribute *attr){ Refer=attr; }
	void setValList(class cValList &vallist){ ValList=vallist; }
	void setDefault(char *_atomdefault){ Default=_atomdefault; }
	int getLang(){ return Lang; }
	unsigned getLimit(){ return Limit; }
	unsigned getNameAlias(){ return NameAlias; }
	unsigned getAttType(){ return AttType; }
	unsigned getIntExt(){ return IntExt; }
	unsigned getRelative(){ return Relative; }
	unsigned getStaDyn() { return StaDyn; }
	unsigned getLexPhr() { return LexPhr; }
    const char * getString(){ return String; }
	Attribute * getRefer(){ return Refer; }
	class cValList & getValList(){ return ValList; }
	char * getDefault(){ return Default; }
};

class AttLink
{
	Attribute *Attr;
	AttLink	*Next;	
public:
	AttLink(Attribute *attri) { Attr = attri; Next=NULL; }	
	void setNext(AttLink *next){ Next=next; }
	Attribute *getAttr(){ return Attr; }
	AttLink *getNext(){ return Next; }
};

class AttList
{
protected:
	AttLink *attLinkHead;
	AttLink *attLinkTail;
public:
	AttList() { attLinkHead=NULL; attLinkTail=NULL; }
	~AttList() { free(); }
	virtual void addto(Attribute *attr)=0;
	virtual Attribute *find(const char *tokenbuff);
	void setAttLinkHead(AttLink *attlink){ attLinkHead=attlink; }
	void setAttLinkTail(AttLink *attlink){ attLinkTail=attlink; }
	AttLink	*getAttLinkHead() { return attLinkHead; };
	AttLink	*getAttLinkTail() { return attLinkTail; };
	virtual	void free(){};
};

class UnsortAttList : public AttList
{
public:
	// insert the attribute to the tail of the  attribute list
	virtual void addto(Attribute *attr);
	virtual void free()
		{
			AttLink	*link,*next;
			link=getAttLinkHead();
			while (link) {
				next=link->getNext();
				delete link;
				link=next;
			}
			attLinkHead=attLinkTail=NULL;
		}
};

class SortedAttList : public AttList
{
	int number;
	Attribute ** attarray;
public:
	SortedAttList() { number=0; attarray=NULL; }
	~SortedAttList() { free(); }
	void makeindex();
	virtual void addto(Attribute *attr);
	virtual Attribute *find(const char *tokenbuff);
	virtual void free()
		{
			AttLink	*link,*next;
			link=getAttLinkHead();
			while (link) {
				next=link->getNext();
				delete link->getAttr();
				delete link;
				link=next;
			}
			attLinkHead=attLinkTail=NULL;
			if (attarray) { delete attarray; attarray=NULL; }
			number=0;
		}
};

class CondLink
{
	char * Condition; 				
	UnsortAttList CondAttList;
	CondLink * Next;  				
public:
	CondLink(char * Condi) { Condition=Condi; Next=NULL; }
	CondLink(char * Condi, class UnsortAttList &condattlist)
		{
			Condition=Condi;
			CondAttList=condattlist;
			Next=NULL;
		}
	~CondLink() { free(); }
	void free()
		 {
		   if (Condition)
		     delete[] Condition;
		   CondAttList.free();
		 }
	void setNext(CondLink *next){ Next=next; }
	char *getCondition(){ return Condition; }
	CondLink *getNext(){ return Next; }
	class UnsortAttList &getCondAttList(){ return CondAttList; };
};

class CondList
{
	CondLink *CondLinkHead;
	CondLink *CondLinkTail;
public:
	CondList() { CondLinkHead=NULL; CondLinkTail=NULL; }
	~CondList() { free(); }
	void free()
		 {
			CondLink *link,*next;
			link=CondLinkHead;
			while (link) {
				next=link->getNext();
				delete link;
				link=next;
			}
			CondLinkHead=CondLinkTail=NULL;
		 }
	CondLink *addto(char *Condi);
	CondLink *addto(char *Condi,class UnsortAttList &condattlist);
	CondLink *getCondLinkHead(){ return CondLinkHead; }
	CondLink *getCondLinkTail(){ return CondLinkTail; }
};

class Wait
{
public:
	virtual void initialwait(const char * message)=0;
	virtual void wait()=0;
	virtual void cancelwait()=0;
	virtual BOOL retry(const char * message)=0;
};

class Kbase
{
protected:
	static char Home[FILENAMELENGTH];
	char Path[FILENAMELENGTH];
	char Name[FILENAMELENGTH];
	char Dir[FILENAMELENGTH];
	time_t Date; 			
	char Type[TOKENLENGTH];
	char Line[LINELENGTH];
	char * LineScan;
	unsigned char Status;
	unsigned char Lock;
	unsigned char Scan;
	unsigned char Built;	
	unsigned char Usernum;
	fstream CfgFile;
	fstream DatFile;
	fstream TxtFile;
	int BufferSize;							
	char *TxtBuff;							
	char *DatBuff;
#ifdef KBMS
	static BOOL MultiUser;
	unsigned char Modify;
#endif

	virtual void endScan();							
	virtual void beginScan(int buffersize);				
	BOOL rdTxtRecord(const char * begin,const char * end=NULL,char * SwpBuff=NULL);
	int  rdDatLength() { return rdDatLengthFromFile(DatFile); }
	void rdDatBuff(int size) { rdDatBuffFromFile(size,DatFile,DatBuff); }
	int	 rdDatLengthFromFile(fstream & file);
	void rdDatBuffFromFile(int size,fstream & file,char * buffer);					
	void rdType();
	void wtType();
	virtual void rdCfg();
	virtual void testCfg();
	virtual void wtCfg();
	virtual void initCfg();
	virtual void setBuilt(int built) { Built=built; }
	void setStatus(int status){ Status=status;}
	void rdLock();
	void unLock();
	virtual void ropen();
	virtual void close();
	virtual void encryption(char * data, int dataleng, char * key, int keyleng);
	virtual void decryption(char * data, int dataleng, char * key, int keyleng);
	virtual void seedkey(char * key, int keyleng, int seed);
public:
	char * getFullName(char *fullname,const char *pfix,const char *pfix2=NULL);
	char * makFullPath();
#ifdef KBMS
protected:
	void wtLock();     					
	virtual void wopen(); 					
	void wtDatLength(int leng) { wtDatLengthToFile(leng,DatFile); }
	void wtDatBuff(int leng=0) { wtDatBuffToFile(DatFile,DatBuff,leng); }						
	void wtDatLengthToFile(int leng,fstream & file);
	void wtDatBuffToFile(fstream & file,char * buff,int leng=0);							
	void wtTxtBuff();						
#endif
public:
	Kbase()
	{
		memset(Line,'\0',LINELENGTH); // txp add
		*Path=*Name=*Dir=0; TxtBuff=DatBuff=NULL;
		Status=CLOSED; Lock=UNLOCK;Built = NO; Usernum=0;
#ifdef KBMS
		Modify=FALSE;
#endif
	};
	~Kbase()
	{
		free();
#ifdef KBMS
		if (Modify==TRUE) setModifyOff();
#endif
	}
	virtual char * hasType()=0;
	static BOOL setHome(const char*);
	void setPath(const char*);
	void setName(const char*);
	void setDate(time_t t){ Date=t;}
#ifdef KBMS
	static void setMultiUser(BOOL m) { MultiUser=m; }
#endif
	void use()
	{
		if (Usernum>20) error("Kbase::use(): too many users.");
		Usernum++;
	}
	void unuse()
	{
		if (Usernum<=0) error("Kbase::unuse(): usernum is zero.");
		Usernum--;
	}
	virtual void testSituation();
	void testUpdated();
	unsigned int getStatus() { return Status; };   		
	unsigned int getLock() { return Lock; };	
	unsigned int getBuilt() { return Built; };
	static char *getHome() { return Home; }
	char *getPath() { return Path;};    	
	char *getName() { return Name;};
	char *getDir() { return Dir; };
	time_t getDate(){ return Date;};
	char * getType() { return Type;}
	unsigned getUsernum() { return Usernum; }
#ifdef KBMS
	unsigned int getModify() { return Modify; }
	BOOL getMultiUser() { return MultiUser; }
#endif
	virtual	void build()
	{
		if (getBuilt()==YES) return;
		testUpdated();
		setBuilt(YES);
	}
    virtual	void free()
	{
		if (getBuilt()==NO) return;
		if (getStatus()!=CLOSED) close();
		if (getUsernum()) error("Kbase::free(): the Kbase is in use.",Path,Name);
		setBuilt(NO);
	}

#ifdef KBMS
	static void LoadAtom(IOToken &TheToken,unsigned type,unsigned limit,
						 class cValList &vallist,const char *err=NULL);
	static void LoadFest(IOToken &TheToken,class SortedAttList &list,
						 const char *err=NULL);
	static void LoadSymbolAtom(IOToken &TheToken,unsigned limit,
						 class cValList &vallist,const char *err=NULL);
	static void LoadSymbol_OR(IOToken &TheToken,unsigned limit,
						 class cValList &vallist,const char *err=NULL);
	static void LoadSymbol_NOT(IOToken &TheToken,unsigned limit,
						 class cValList &vallist,const char *err=NULL);
	static void LoadSymbol_SMP(IOToken &TheToken,unsigned limit,
						 class cValList &vallist,const char *err=NULL);
	static void LoadHierarAtom(IOToken &TheToken,unsigned limit,
						 class cValList &vallist,const char *err=NULL);
	static void LoadHierar_OR(IOToken &TheToken,unsigned limit,
						 class cValList &vallist,const char *err=NULL);
	static void LoadHierar_NOT(IOToken &TheToken,unsigned limit,
						 class cValList &vallist,const char *err=NULL);
	static void LoadHierar_AND(IOToken &TheToken,unsigned limit,
						 class cValList &vallist,const char *err=NULL);
	static void LoadHierar_SMP(IOToken &TheToken,unsigned limit,
						 class cValList &vallist,const char *err=NULL);
	static void LoadNumberAtom(IOToken &TheToken,unsigned limit,
						 class cValList &vallist,const char *err=NULL);
	static void LoadNumber_OR(IOToken &TheToken,unsigned limit,
						 class cValList &vallist,const char *err=NULL);
	static void LoadNumber_NOT(IOToken &TheToken,unsigned limit,
						 class cValList &vallist,const char *err=NULL);
	static void LoadNumber_SMP(IOToken &TheToken,unsigned limit,
						 class cValList &vallist,const char *err=NULL);
	static void LoadBooleanAtom(IOToken &TheToken,
						 class cValList &vallist,const char *err=NULL);

	static void LoadIntFest(IOToken &TheToken,SortedAttList &list,
							const char *err=NULL);
	static void LoadIntFeat(IOToken &TheToken,SortedAttList &list,
							const char *err=NULL);
	static void LoadExtFest(IOToken &TheToken,SortedAttList &list,
							const char *err=NULL);
	static void LoadExtFeat(IOToken &TheToken,SortedAttList &list,
							const char *err=NULL);
	static Attribute *LoadIntAtt(IOToken &TheToken,SortedAttList &list,
							const char *err=NULL);
	static void LoadExtAtt(IOToken &TheToken,SortedAttList &list,
							const char *err=NULL);
	static void LoadIntVal(IOToken &TheToken,unsigned type,unsigned limit,
							class  cValList & vallist,const char *err=NULL);
	static void LoadExtVal(IOToken &TheToken,SortedAttList &list,
							const char *err=NULL);
#endif

	static void ReadSymbolAtom(IToken &TheToken);
	static void ReadSymbol_OR(IToken &TheToken);
	static void ReadSymbol_NOT(IToken &TheToken);
	static void ReadSymbol_SMP(IToken &TheToken);
	static void ReadHierarAtom(IToken &TheToken);
	static void ReadHierar_OR(IToken &TheToken);
	static void ReadHierar_NOT(IToken &TheToken);
	static void ReadHierar_AND(IToken &TheToken);
	static void ReadHierar_SMP(IToken &TheToken);
	static void ReadNumberAtom(IToken &TheToken);
	static void ReadNumber_OR(IToken &TheToken);
	static void ReadNumber_NOT(IToken &TheToken);
	static void ReadNumber_SMP(IToken &TheToken);
	static void ReadBooleanAtom(IToken &TheToken);
	static void ReadFest(IToken &TheToken);
	static void ReadAtom(IToken &TheToken,unsigned type);

#ifdef KBMS
	virtual void view();
	virtual void setModifyOn();
	virtual void setModifyOff();
	virtual void update();
	virtual void edit();
	virtual void create();
	virtual void clear() {} ;
	virtual void undoedit();
	virtual void Load()=0;
#endif // KBMS
};

#endif

