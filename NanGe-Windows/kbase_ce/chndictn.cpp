
#include "chndictn.h"

/****************************************************************
*   parameters:    char *word                            	    *
*   return value:  int                                   	    *
*   Description:   get a distinguish value of the given word	*
*                  to discriminate different word when their	*
*                  Hash value or pattern is the same        	*
*    algorithm:                                             	*
*                  get the word's first and last char,      	*
*                  OR the two letters' every bit and return 	*
*                  the result.                               	*
*****************************************************************/

static int HASHLENGTH=8;

int ChnDictn::getDstgVal(const char *word)
{
	int val=0;
	const char * wordend=word+HASHLENGTH;
	const char * next=getNext(word);

	while (*word && next <= wordend)
	{
		val=val^getValue(word);
		word=next;
		next=getNext(word);
	}

	return val;

}

/****************************************************************
*   parameters:   char *word,int Len(hash table Length)     	*
*   return value: int:Hash Value                            	*
*   Description:   when a word is given,calculate its Hash  	*
*                Value to locate its position in Index file 	*
*                or  in Buffer.                             	*
*   algorithm:                                              	*
*                get the word's first two chars.            	*
*                get their HiByte() and LowByte() to        	*
*                a1,a2,a3,a4.                               	*
*                calculate: a1+a2*95+a3*95*95+a4*95*95*95   	*
*                           every step should mod Len       	*
*****************************************************************/

int ChnDictn::getHashVal(const char *word,int Len)
{
	int   Hash=0;
	const char * wordend=word+HASHLENGTH;
	//const char * next=getNext(word);

	while (*word && word < wordend /*next <= wordend*/)
	{
		/*
		if (isDblByte(word))
		{
			Hash = ( Hash*71 + getQu(word) ) % Len;
			Hash = ( Hash*71 + getWei(word) ) % Len;
		}
		else Hash = ( Hash*71 + *word ) % Len;
		word=next;
		next=getNext(word);
		*/
		Hash = ( Hash*71 + (unsigned char)*word ) % Len;
		word++;
	}

	return Hash;
}

/************************************************************
*   Description:  read entrys matching the specied text     *
*                 if text=NULL,find out the entry matching  *
*                    the old text                           *
*                 if text!=NULL,find out the first entry    *
*                    matching the text.                     *
*   About matching: matching means to find the words in the *
*                   dictionary who is the first part of the *
*                   text. that is to say,text is always     *
*                   longer,at least equal to the word.      *
*                   total number of matching the text is the*
*                   same  as the length of the text.        *
*************************************************************/

cRuleList & ChnDictn::rd_MeansWord(char *iword, const char *itext)
{
	if( getBuilt()==NO) 
		error("ChnDictn::rd_MeansWord(): rd_MeansWord while Built=NO");
	if (getStatus()!=ROPEN) 
		error("ChnDictn::rd_MeansWord(): rd_MeansWord while not ROPEN.");

	static char text[SENTLENGTH];
	static char word[WORDLENGTH];
	static char * textpntr;
	static char * wordpntr;
	static long idxoffs;
	static int dstgval;
	static int longer;

	if (itext) 
	{
		strcpy(text,itext);
		textpntr=text;
		wordpntr=word;
		longer=0;
	}

	if (!*textpntr) return *(cRuleList *)NULL;

	setCharNext(wordpntr,textpntr);
	while (textpntr-text<HASHLENGTH)
	{
		*wordpntr=0;
		cRuleList & rulelist=rd_Means(word);
		if (&rulelist) { strcpy(iword,word); return rulelist; }
		else if (!*textpntr) return *(cRuleList *)NULL;
		else setCharNext(wordpntr,textpntr);
	}
	
	if (longer==0)
	{
		longer=1;
		dstgval=getDstgVal(text);
		int hashval=getHashVal(text,IdxFileSize);
		idxoffs=hashval;
	}

	if (idxoffs==-1) return *(cRuleList *)NULL;

	BOOL Find=FALSE;
	char Word[TOKENLENGTH];
	beginScan(MAXBUFFER);
	cRuleList * rulelist=(cRuleList *)NULL;;
	while (idxoffs!=-1)
	{
		IndexBuffUnit & index = IndexBuff[idxoffs];
		idxoffs=index.getNextIndex();
		if (index.getDstgValue()==dstgval && index.getDataOffset()!=-1)
		{
			DatFile.seekg(index.getDataOffset(),ios::beg);
			int i;
			i=rdDatLength();
			rdDatBuff(i);
			getWord(DatBuff,Word);
			int agree=stragree(text,Word);
			if(agree==0 || agree==1)
			{
				EntryBuffUnit * unit=EntryBuff.addEntry(Word,DatBuff);
				strcpy(iword,unit->getKey());
				rulelist=&unit->getRuleList();
				break;
			}
		}
	}
	endScan();
	return *rulelist;
}
