
#ifndef LEXDBASE_H

#define LEXDBASE_H

#include "../kbase/kbase.h"
#include "../kbase/mkbase.h"
#include "../basic/list.h"

class TVector // Tag Vector
{
	char ** Tag;
	int Num;       // the number of valid member
	int Size;      // the number of memory allocated
public:
	TVector() { Tag=NULL; Num=Size=0; }
	~TVector() { free(); }
	void free()
	{
		if (Num==0) return;
		int i;
		for (i=0; i<Num; i++) delete Tag[i];
		delete Tag;
		Tag=NULL;
		Size=0;
		Num=0;
	}
	void read(istream & s);
	void write(ostream & s);
	int getNum() const { return Num; };
	int findTag(const char * tag) const;
	const char * operator [] (int i)
	{
		if (i<0 || i>=Num) error("TVector()::operator[]: overflow.");
		return Tag[i];
	}
};

class TTMatrix // Tag-Tag Dependency Matrix
{
	int Dim;
	int ** Dependency;
	int * Frequency;
public:
	TTMatrix() { Dim=0; Dependency=NULL; Frequency=NULL; }
	~TTMatrix() { free(); }
	void alloc(int d);
	void free()
	{
		if (Dim==0) return;
		int i;
		for (i=0;i<Dim;i++) delete Dependency[i];
		delete Dependency;
		delete Frequency;
		Dependency=NULL;
		Frequency=NULL;
		Dim=0;
	}
	void read(istream & i);
	void write(ostream & o);
	int getDependency(int i,int j)
	{
		if (i<0 || i>=Dim || j<0 || j>=Dim)
			error("TTMatrix()::getDependency(): overflow.");
		return Dependency[i][j];
	}
	int getFrequency(int i)
	{
		if (i<0 || i>=Dim)
			error("TTMatrix()::getFrequency(): overflow.");
		return Frequency[i];
	}
};

class TWTag  // Tag-Word Dependency
{
	int Tag;
	int Times;
	TWTag * Next;
public:
	TWTag() { Tag=0; Times=0; Next=NULL; }
	~TWTag() { free(); }
	void free() { Tag=0; Times=0; Next=NULL; }
	friend class TWWord;
	friend class TWMatrix;
};

class TWWord // Tag-Word Dependency Vector
{
	char * Word;
	TWTag * TagHead;
	TWTag * TagTail;
public:
	TWWord() { Word=NULL; TagHead=TagTail=NULL; }
	~TWWord() { free(); }
	void free()
	{
		if (Word) { delete Word; Word=NULL; }
		TWTag * tag=TagHead, * next;
		while (tag) { next=tag->Next; delete tag; tag=next; }
		TagHead=TagTail=NULL;
	}
	static int compare1(const void * mem1,const void * mem2)
		{ return strcmp(((const TWWord *)mem1)->Word,((const TWWord *)mem2)->Word); }
	static int compare2(const void * mem1,const void * mem2)
		{ return strcmp((const char *)mem1,((const TWWord *)mem2)->Word); }
	friend class TWMatrix;
};

class TWMatrix // Tag-Word Dependency Matrix
{
	const TVector * TagVector;
	TWWord * Dependency;
	int * Frequency;
	int Num;       // the number of valid TWWord member
	int Size;      // the number of TWWord memory allocated
public:
	TWMatrix() { Dependency=NULL; Frequency=NULL; Num=Size=0; }
	~TWMatrix() { free(); }
	void alloc(const TVector & t);
	void free()
	{
	  if (Dependency) { delete[] Dependency; Dependency=NULL; }
	  if (Frequency) { delete Frequency; Frequency=NULL; }
	  Num=Size=0;
	}
	void read(istream & i);
	void write(ostream & o);
	int getDependency(const char * word, int tag);
	int getFrequency(int tag)
	{
		if (tag<0 || tag>=TagVector->getNum())
			error("TTMatrix()::getFrequency(): overflow.");
		return Frequency[tag];
	}
};

class ChnLexDbase : public MKbase
{

  protected:

	int SrcLang;

	void rdCfg();
	void testCfg();
	void wtCfg();
	void initCfg();

	void rdSrcLang();
	void wtSrcLang();

	void ropen();

#ifdef KBMS
	void wopen();
#endif

	void close();

	fstream TagFile;
	fstream TagTagFile;
	fstream TagWordFile;

	int TagNumber;
	TVector TagVector;
	TTMatrix TagTagMatrix;
	TWMatrix TagWordMatrix;

public:
	
	ChnLexDbase() { }
	~ChnLexDbase() { free(); }

	char * hasType() { return "LEXDBASE"; }

	virtual int hasSrcLang() { return CHINESE; }

#ifdef KBMS
	void create(const char *);
	void clear();
	void Load() {}
#endif

	void build();
	void free()
	{
		MKbase::free();
		if( getBuilt()==NO ) return;
		setBuilt(NO);
	}

	void readTagVector();
	void readTagTagMatrix();
	void readTagWordMatrix();

	double getTagTagProb(const char * prevtag, const char * tag);
	double getTagWordProb(const char * tag, const char * word);
};

#endif // LEXDBASE_H
