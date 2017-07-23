// Result.cpp : implementation file
//
#include "../basic/code.h"
#include "../basic/constant.h"
#include "result.h"

ResultRecord & ResultRecord::operator=(const ResultRecord & sent)
{
	if (sent.Lead) { Lead=new char[strlen(sent.Lead)+1]; strcpy(Lead,sent.Lead); }
	else Lead=NULL;
	if (sent.Trail) { Trail=new char[strlen(sent.Trail)+1]; strcpy(Trail,sent.Trail); }
	else Trail=NULL;
	if (sent.SrcText) { SrcText=new char[strlen(sent.SrcText)+1]; strcpy(SrcText,sent.SrcText); }
	else SrcText=NULL;
	ResultSelected=sent.ResultSelected;
	ResultNumber=0;
	TheResultList.free();
	ResultLink * link;
	for (link=sent.GetResultHead();link;link=link->GetNext())
		AddResult(link->GetData()->getTargetSentence(),
				  link->GetData()->getTargetTree(),
				  link->GetData()->getSourceTree());
	return *this;
}

int ResultRecord::operator == (const ResultRecord & record) const
{
	return	strcmp(Lead,record.Lead)==0 &&
			strcmp(Trail,record.Trail)==0 &&
			strcmp(SrcText,record.SrcText)==0;
}

void ResultRecord::free()
{
	if (Lead) delete Lead;
	if (Trail) delete Trail;
	if (SrcText) delete SrcText;
	Lead=Trail=SrcText=NULL;
	ResultSelected=0;
	ResultNumber=0;
	TheResultList.free();
}

int ResultRecord::isSpaceLine(const char * line)
{
	return *line==0 || isSpcChar(line);
}

int ResultRecord::isEmptyLine(const char * line)
{
	while (*line && isSpcChar(line)) goNext(line);
	return *line==0;
}

extern unsigned char OptionNewLine;
extern unsigned char OptionSpaceLine;
extern unsigned char OptionEmptyLine;
extern unsigned char OptionCommaColon;

BOOL ResultRecord::MakSrcText(const char * & begin, const char * end)
{
	char source[SENTLENGTH+1], * s=source;
	while (*begin!=0 && begin<end)
	{
		// copy to sentence
		char * t=s;
		setCharNext(s,begin);
		if (isEqual(t,"¡£") ||
			isEqual(t,"£¿") || isEqual(t,"?") ||
			isEqual(t,"£¡") || isEqual(t,"!") ||
			isEqual(t,",")  && OptionCommaColon ||
			isEqual(t,"£¬") && OptionCommaColon ||
			isEqual(t,";")  && OptionCommaColon ||
			isEqual(t,"£»") && OptionCommaColon ||
			isEqual(t,":")  && OptionCommaColon ||
			isEqual(t,"£º") && OptionCommaColon ||
			*t=='\n' && OptionNewLine ||
			*t=='\n' && isSpaceLine(begin) && OptionSpaceLine ||
			*t=='\n' && isEmptyLine(begin) && OptionEmptyLine)
			break;
	}
	*s=0;
	if (s==source) return FALSE;

	char lead[TEXTLENGTH+1],trail[TEXTLENGTH+1];
	*lead=*trail=0;
	char *p,*q;
	// Get lead spaces 
	for (p=source,q=lead;*p && isSpcChar(p);setCharNext(q,p));
	*q=0;
	if (p!=source) strcpy(source,p);
	Lead=new char[strlen(lead)+1];
	strcpy(Lead,lead);
	if (*p==0) // empty sentence
	{
		SrcText=new char[1];
		Trail=new char[1];
		*SrcText=*Trail=0;
	}
	else
	{
		// Get trail spaces
		for (p=source,q=NULL;*p;goNext(p))
		{
			if (isSpcChar(p)) { if (q==NULL) q=p; }
			else { if (q!=NULL) q=NULL; }
		}
		if (q) { strcpy(trail,q); *q=0; }
		SrcText=new char[strlen(source)+1];
		strcpy(SrcText,source);
		Trail=new char[strlen(trail)+1];
		strcpy(Trail,trail);
	}
	return TRUE;
}

ResultLink * ResultRecord::GetResult()
{
	static char * ErrorText="(** none **)";
	static ResultList ErrorResultList;
	if (ResultSelected<0 || ResultSelected>=ResultNumber)
	{
		if (ErrorResultList.GetNumber()==0)
		{
			Result ErrorResult;
			ErrorResult.setTargetSentence(ErrorText);
			ErrorResultList.AddHead(ErrorResult);
		}
		return ErrorResultList.GetHead();
	}
	ResultLink * link;
	int i=0;
	for (link=GetResultHead(); link && i!=ResultSelected;
		 link=link->GetNext(),i++);
	return link;
}

void ResultRecord::AddResult(const char * tgttext,const char * tgttree,const char * srctree)
{
	Result result;
	result.setTargetSentence(tgttext);
	result.setTargetTree(tgttree);
	result.setSourceTree(srctree);
	TheResultList.AddTail(result);
	ResultNumber++;
}

void ResultRecord::Write(fstream &file)
{
	int length;
	length=strlen(Lead);
	file.write((const char *)&length,sizeof(length));
	file.write(Lead,length);						/* Lead */
	length=strlen(Trail);
	file.write((const char *)&length,sizeof(length));
	file.write(Trail,length);						/* Trail */
	length=strlen(SrcText);
	file.write((const char *)&length,sizeof(length));
	file.write(SrcText,length);						/* SrcText */
	file.write((const char *)&SrcPosition,sizeof(SrcPosition));	/* SrcPosition */
	file.write((const char *)&ResultSelected,sizeof(ResultSelected));	/*ResultSelected*/
	int number=ResultNumber;
	file.write((const char *)&number,sizeof(number));		/*number*/
	int i;
	ResultLink * link;
	for (i=0,link=GetResultHead();
		 link && i<number;i++,link=link->GetNext())
	{
		const char * text=link->GetData()->getTargetSentence();
		int length=strlen(text);
		file.write((const char *)&length,sizeof(length));
		file.write(text,length);							/*TargetSentence*/
		text=link->GetData()->getTargetTree();
		length=strlen(text);
		file.write((const char *)&length,sizeof(length));
		file.write(text,length);							/*TargetTree*/
		text=link->GetData()->getSourceTree();
		length=strlen(text);
		file.write((const char *)&length,sizeof(length));
		file.write(text,length);							/*SourceTree*/
	}
	file.flush();
	if (i<number || link!=NULL)
		error("ResultRecord::Write(): Number is not count of TargetSentenceList.");
}

int ResultRecord::Read(fstream &file)
{
	try
	{
		free();
		if (file.peek()==EOF) return FALSE;
		int length;
		file.read((char *)&length,sizeof(length));
		Lead=new char[length+1];
		file.read(Lead,length);
		Lead[length]=0;									/* Lead */
		file.read((char *)&length,sizeof(length));
		Trail=new char[length+1];
		file.read(Trail,length);
		Trail[length]=0;								/* Trail */
		file.read((char *)&length,sizeof(length));
		SrcText=new char[length+1];
		file.read(SrcText,length);
		SrcText[length]=0;								/* SrcText */
		file.read((char *)&SrcPosition,sizeof(SrcPosition));	/* SrcPosition */
		file.read((char *)&ResultSelected,sizeof(ResultSelected));	/*Selected*/
		int number;
		file.read((char *)&number,sizeof(number));				/*number*/
		int i;
		for (i=0;i<number;i++)
		{
			int length;
			char tgttext[TEXTLENGTH+1],tgttree[TREELENGTH+1],srctree[TREELENGTH+1];
			file.read((char *)&length,sizeof(length));
			file.read(tgttext,length);								/*TargetSentence*/
			tgttext[length]=0;
			file.read((char *)&length,sizeof(length));
			file.read(tgttree,length);								/*TargetTree*/
			tgttree[length]=0;
			file.read((char *)&length,sizeof(length));
			file.read(srctree,length);								/*SourceTree*/
			srctree[length]=0;
			AddResult(tgttext,tgttree,srctree);
		}
		return TRUE;
	}
	catch (...)
	{
		message("ResultRecord::Read(): File format error.");
		free();
		return FALSE;
	}
}
