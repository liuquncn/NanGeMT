
#include "lexdbase.h"
#include <memory.h>
#include <stdlib.h>

#ifdef WIN32
#include <direct.h>
#include <io.h>
#endif

void ChnLexDbase::rdSrcLang()
{
	if( !CfgFile.is_open() ) error("ChnLexDbase::rdSrcLang(): CfgFile open fail" );
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

void ChnLexDbase::wtSrcLang()
{
	if( !CfgFile ) error("ChnLexDbase::wtSrcLang(): CfgFile open fail" );
	CfgFile.seekp(0,ios::end);
	SrcLang=hasSrcLang();
	CfgFile << "SrcLang=" << SrcLang << endl;
}

void ChnLexDbase::initCfg()
{
	MKbase::initCfg();
	SrcLang=hasSrcLang();
	if( mkModel->hasLang(SrcLang)==FALSE ) error("ChnLexDbase::initCfg(): SrcLang not exist." );
}

void ChnLexDbase::rdCfg()
{
	MKbase::rdCfg();
	rdSrcLang();
}

void ChnLexDbase::wtCfg()
{
	MKbase::wtCfg();
	wtSrcLang();
}

void ChnLexDbase::testCfg()
{
  //	MKbase::testCfg();
  //	if( SrcLang!=hasSrcLang() ) error("ChnLexDbase::testCfg(): SrcLang error" );
}

#ifdef KBMS
void ChnLexDbase::create(const char *filename)
{
	if( getStatus()!=CLOSED ) error("ChnLexDbase::create(): Cannot init with not closed." );

	makFullPath();
	DirMake(getDir());

	char txtfilename[FILENAMELENGTH];
#ifdef UNIX
	char command[COMMANDLENGTH];
	const char *cp="cp ";
    const char *touch="touch ";
#endif
    getFullName(txtfilename,".TXT");
	if (!*filename) {
#ifdef UNIX
	    strcpy(command,touch);
        strcat(command,txtfilename);
        system(command);
#endif
#ifdef WIN32
	FileTouch(txtfilename);
#endif
}
	else {
#ifdef UNIX
		strcpy(command,cp);
		strcat(command,filename);
		strcat(command," ");
		strcat(command,txtfilename);
		system(command);
#endif
#ifdef WIN32
		FileCopyTo(filename,txtfilename);
#endif
	}
	char statfile[FILENAMELENGTH];
	getFullName(statfile,".STA");
	fstream statstrm(statfile,ios::out);
	statstrm.seekp(0,ios::beg);
	statstrm.put('0');
	statstrm.put('\n');
	statstrm.close();

	char cfgfile[FILENAMELENGTH];
	getFullName(cfgfile,".CFG");
	CfgFile.open(cfgfile,ios::out);
	if( !CfgFile ) error("ChnLexDbase::create(): CfgFile open fail.",cfgfile );

	initCfg();
	wtCfg();
	CfgFile.close();

	char datafile[FILENAMELENGTH];
	getFullName(datafile,".DAT");
	DatFile.open(datafile,ios::out|ios::binary|ios::trunc);
	if (!DatFile ) error("ChnLexDbase::create(): DatFile open fail.",datafile );
	DatFile.close();

}

void ChnLexDbase::clear()
{
	char cfgfilename[FILENAMELENGTH];
	getFullName(cfgfilename,".CFG");
	CfgFile.open(cfgfilename,ios::in);
	rdCfg();
	setModelDate(mkModel->getDate());
	CfgFile.close();
	CfgFile.open(cfgfilename,ios::out|ios::trunc);
	wtCfg();
	CfgFile.close();

	wopen();
	DatFile.close();
	char datafile[FILENAMELENGTH];
	getFullName(datafile,".DAT");
	DatFile.open(datafile,ios::out|ios::binary|ios::trunc);
	if( !DatFile ) error("ChnLexDbase::clear(): DatFile open fail.",datafile );
	close();
}
#endif // KBMS

void ChnLexDbase::ropen()
{
	// MKbase::ropen();
	char tagfile[FILENAMELENGTH];
	char tagtagfile[FILENAMELENGTH];
	char tagwordfile[FILENAMELENGTH];
	getFullName(tagfile,".TAG");
	getFullName(tagtagfile,".TTM");
	getFullName(tagwordfile,".TWM");
#ifdef UNIX
	TagFile.open(tagfile,ios::in);
	TagTagFile.open(tagtagfile,ios::in);
	TagWordFile.open(tagwordfile,ios::in);
#endif
#ifdef WIN32
	TagFile.open(tagfile,ios::in);
	TagTagFile.open(tagtagfile,ios::in);
	TagWordFile.open(tagwordfile,ios::in);
#endif
}

#ifdef KBMS
void ChnLexDbase::wopen()
{
	MKbase::wopen();
	char tagfile[FILENAMELENGTH];
	char tagtagfile[FILENAMELENGTH];
	char tagwordfile[FILENAMELENGTH];
	getFullName(tagfile,".TAG");
	getFullName(tagtagfile,".TTM");
	getFullName(tagwordfile,".TWM");
#ifdef UNIX
	TagFile.open(tagfile,ios::in|ios::out);
	TagTagFile.open(tagtagfile,ios::in|ios::out);
	TagWordFile.open(tagwordfile,ios::in|ios::out);
#endif
#ifdef WIN32
	TagFile.open(tagfile,ios::in|ios::out);
	TagTagFile.open(tagtagfile,ios::in|ios::out);
	TagWordFile.open(tagwordfile,ios::in|ios::out);
#endif
}
#endif // KBMS

void ChnLexDbase::close()
{
	// MKbase::close();
	TagFile.close();
	TagTagFile.close();
	TagWordFile.close();
}

void ChnLexDbase::build()
{
	if( getBuilt()==YES ) return;
	ropen();
	beginScan(MAXBUFFER);
	readTagVector();
	readTagTagMatrix();
	readTagWordMatrix();
	endScan();
	close();
	setBuilt(YES);
	// MKbase::build();
}

void ChnLexDbase::readTagVector()
{
	TagVector.read(TagFile);
	TagNumber=TagVector.getNum();
	if (TagNumber==0)
		error("ChnLexDbase::readTagVector(): no Tags in TagFile.");
}

void ChnLexDbase::readTagTagMatrix()
{
	TagTagMatrix.alloc(TagNumber);
	TagTagMatrix.read(TagTagFile);
}

void ChnLexDbase::readTagWordMatrix()
{
	TagWordMatrix.alloc(TagVector);
	TagWordMatrix.read(TagWordFile);
}

double ChnLexDbase::getTagTagProb(const char * prevtag, const char * tag)
{
	int t1=TagVector.findTag(prevtag);
	int t2=TagVector.findTag(tag);
	double dependency=TagTagMatrix.getDependency(t1,t2);
	double frequency=TagTagMatrix.getFrequency(t1);
	return dependency/frequency;
}

double ChnLexDbase::getTagWordProb(const char * tag, const char * word)
{
	int t=TagVector.findTag(tag);
	double dependency=TagWordMatrix.getDependency(word,t);
	double frequency=TagWordMatrix.getFrequency(t);
	return dependency/frequency;
}

int TVector::findTag(const char * tag) const
{
	int i;
	for (i=0;i<Num && strcmp(tag,Tag[i])!=0;i++);
	if (i==Num) error("TVector::findTag(): tag not found.",tag);
	return i;
}

void TVector::read(istream & is)
{
	const int BufLeng = 200;
	const int SizeStep = 30;
	char Line[BufLeng];
	IToken token;
	if (Num) free();
	while (is.peek()!=EOF)
	{
		is.getline(Line,BufLeng);
		token.setInPntr(Line);
		while (token.input())
		{
			if (Num>=Size)
			{
				int oldsize=Size;
				Size += SizeStep;
				char ** oldtag=Tag;
				Tag=new char * [Size];
				if (oldtag)
				{
					memcpy(Tag,oldtag,oldsize*sizeof(char *));
					delete oldtag;
				}
			}
			Tag[Num]=new char[strlen(token)+1];
			strcpy(Tag[Num],token);
			Num++;
		}
	}
}

void TVector::write(ostream & os)
{
	int i;
	for (i=0;i<Num;i++) { os << Tag[i] << endl; }
}

void TTMatrix::alloc(int d)
{
	if (d<=0) error("TTMatrix::alloc(): Dim is less than zero.");
	Dim=d;
	Dependency=new int * [Dim];
	int i;
	for (i=0;i<d;i++) Dependency[i]=new int[Dim];
	Frequency=new int[Dim];
}

void TTMatrix::read(istream & is)
{
	if (Dim==0) return;
	int i,j;
	for (i=0;i<Dim;i++) Frequency[i]=0;
	for (i=0;i<Dim;i++)
		for (j=0;j<Dim;j++)
			{ is >> Dependency[i][j]; Frequency[i] += Dependency[i][j]; }
}

void TTMatrix::write(ostream & os)
{
	if (Dim==0) return;
	int i,j;
	for (i=0;i<Dim;i++)
		for (j=0;j<Dim;j++)
			os << Dependency[i][j];
}

void TWMatrix::alloc(const TVector & t)
{
	TagVector=&t;
	Frequency=new int[TagVector->getNum()];
}

void TWMatrix::read(istream & is)
{
	int i;
	for (i=0;i<TagVector->getNum();i++) Frequency[i]=0;
	const int BufLeng = 200;
	const int SizeStep = 30000;
	char Buffer[BufLeng];
	IToken token;
	if (Num && Dependency)
	{
		delete[] Dependency;
		Dependency=NULL; Size=0;
	}
	char c, * b;
	while (is.peek()!=EOF)
	{
		b=Buffer;
		while (is.get(c) && c!=EOF && c!='#') *b++=c;
		*b=0;
		token.setInPntr(Buffer);
		token.input();  // sentence, no use
		token.input();  // word
		if (*token==0) break;
		if (Num>=Size)
		{
			int oldsize=Size;
			Size += SizeStep;
			TWWord * oldmatrix=Dependency;
			Dependency=new TWWord[Size];
			if (oldmatrix)
			{
				memcpy(Dependency,oldmatrix,oldsize*sizeof(TWWord));
				memset(oldmatrix,0,oldsize*sizeof(TWWord));
				delete oldmatrix;
			}
		}
		TWWord * word=Dependency+Num;
		word->Word=new char[strlen(token)+1];
		strcpy(word->Word,token);
		while (token.input())
		{
			TWTag * tag= new TWTag;
			tag->Tag=TagVector->findTag(token);
			token.input();
			if (*token==0 || token.getType()!=NUMBER)
				error("TWMatrix::read(): should be a number.",token);
			tag->Times=atoi(token);
			if (!word->TagHead)
				{ word->TagHead=word->TagTail=tag; }
			else
			{
				word->TagTail->Next=tag;
				word->TagTail=tag;
			}
			Frequency[tag->Tag] += tag->Times;
		}
		Num++;
	}
	// qsort(Dependency,Num,sizeof(TWWord),TWWord::compare1);
}

void TWMatrix::write(ostream & os)
{
}

int TWMatrix::getDependency(const char * word, int tag)
{
	TWWord * twword;
	twword=(TWWord *)bsearch(word,Dependency,Num,sizeof(TWWord),TWWord::compare2);
	TWTag * twtag=twword->TagHead;
	for (twtag=twword->TagHead;twtag;twtag=twtag->Next)
		if (twtag->Tag==tag) return twtag->Times;
	return 0;
}

