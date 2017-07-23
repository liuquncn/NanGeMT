// Result.h : header file
//

#ifndef RESULT_H
#define RESULT_H

#include <iostream>
#include <fstream>
#include "struct.h"

class ResultRecord
{
	char * Lead;
	char * Trail;
	char * SrcText;
	long SrcPosition;
	int ResultSelected;
	int ResultNumber;
	ResultList TheResultList;
public:
	ResultRecord() { Lead=Trail=SrcText=NULL; ResultSelected=ResultNumber=0; }
	ResultRecord(const ResultRecord & sent) { operator=(sent); }
	~ResultRecord() { free(); }
	void free();
	static int isSpaceLine(const char * line);
	static int isEmptyLine(const char * line);
	char * GetLead() { return Lead; }
	char * GetTrail() { return Trail; }
	char * GetSrcText() { return SrcText; }
	long GetSrcPosition() { return SrcPosition; }
	int GetResultSelected() { return ResultSelected; }
	int GetResultNumber() { return ResultNumber; }
	ResultLink * GetResultHead() const { return TheResultList.GetHead(); }
	ResultLink * GetResult();
	BOOL MakSrcText(const char * & begin, const char * end);
	void SetSrcPosition(long p) { SrcPosition=p; }
	void SetResultSelected(int s) { ResultSelected=s; }
	void AddResult(const char * tgttext,const char * tgttree,const char * srctree);
	void Write(fstream &file);
	int Read(fstream &file);  // return TRUE or FALSE
	int operator == (const ResultRecord & record) const;
	ResultRecord & operator = (const ResultRecord & record);
};

typedef ObjLink<ResultRecord> ResultRecordLink;

class ResultRecordList: public ObjList<ResultRecord,ResultRecordLink>
{
};

#endif /* RESULT_H */
