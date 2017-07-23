#include <vector>
#include <string>
#include <assert.h>
#include <iostream>
#include <algorithm>
#include <boost/regex.hpp>
#include "postag.hh"
#include "transform.hh"
#include "segment.hh"
#include "parse.hh"

using namespace std;

extern vector<edgec> chart;
extern string chnsent;
extern string locdictn;
extern string dictn;
extern string sczlxiaoshu;
extern string sczlfuhao;
extern string sczlshuliang;
extern string mczlng;
extern boost::regex numreg;

void postag(vector<string> & words, size_t initpos, vector<tagc> & tags)
{
  size_t bpos=initpos;
  for (vector<string>::iterator i=words.begin(); i!=words.end(); i++) {
    size_t epos=bpos+i->length();
    string keyword="$$ "+(*i)+"\n";
    size_t ebpos=dictn.find(keyword);
    if (ebpos!=string::npos) {
      size_t eepos=dictn.find("$$",ebpos+i->size()+3);
      string entry=dictn.substr(ebpos+i->size()+3,eepos-ebpos-i->size()-3);
      vector<string> catentset;
      string septor="**";
      getset(entry,septor,catentset);
      for (vector<string>::iterator j=catentset.begin();j!=catentset.end();j++) {
	size_t rulepos=j->find("&&");
	string catent,rulent;
	if (rulepos!=string::npos) {
	  catent=j->substr(0,rulepos);
	  rulent=j->substr(rulepos);
	}
	else {
	  catent=(*j);
	  rulent="";
	}
	
	size_t idbpos=catent.find("{");
	size_t idepos=catent.find("}",idbpos);
	size_t catbpos=idepos+1;
	while (catent[catbpos]==' ')
	  catbpos++;
	size_t catepos=catbpos;
	while (isalpha((unsigned char)catent[catepos]))
	  catepos++;
	size_t featpos=catent.find("$=[",catepos);
	size_t spapos=catent.find(" ",featpos);
	size_t swipos=catent.find("||",featpos);
	size_t tranpos=catent.find("=>",featpos);
	
	tagc tag;
	tag.sentreg.begin=bpos;
	tag.sentreg.end=epos;
	tag.word=*i;
	tag.cat=catent.substr(catbpos,catepos-catbpos);
	tag.catid=catent.substr(idbpos+1,idepos-idbpos-1);
	if (featpos!=string::npos) {
	  string temp=catent.substr(featpos+2,min(spapos,min(swipos,tranpos))-featpos-2);
	  removespace(temp,tag.feat);
	}
	string septor="&&";
	getset(rulent,septor,tag.ruleset);
	tags.push_back(tag);
      }
    }
    else {
      tagc tag;
      tag.sentreg.begin=bpos;
      tag.sentreg.end=epos;
      tag.word=*i;
      tag.catid="unk";
      procunkword(*i,tag); //="unknown";
      tags.push_back(tag);
    }
    bpos=epos;
  }
}

void procunkword(string & seq, tagc & tag)
{
  boost::match_results<string::const_iterator> m;
  if (boost::regex_match(seq,m,numreg)) {
    tag.cat="m";
    if (seq.find(".")!=string::npos) 
      tag.feat="["+sczlxiaoshu+"]";
    else if (seq.size()==4)
      tag.feat="["+sczlfuhao+"]";
    else
      tag.feat="["+sczlshuliang+"]";
  }
  else {
    tag.cat="n";
    tag.feat="["+mczlng+"]";
  }
}

void iniedges(vector<segc> & segs)
{
  chnsent="";
  size_t initpos=0;
  for (vector<segc>::iterator i=segs.begin();i!=segs.end();i++) {
    vector<edgec> edges;
    if (i->flag=='w') {
      wordstrtoedge(i->seq,initpos,'a',edges);
    }
    else if (i->flag=='t') {
      wordtagtoedge(i->seq,initpos,'a',edges);
    }
    else if (i->flag=='p') {
      phrasetoedge(i->seq,initpos,'a',edges);
    }
    else
      assert(0);
  }
}

void maketag(string & word, string & cat, size_t initpos, tagc & tag)
{
  size_t bpos=initpos;
  size_t epos=initpos+word.size();
  string keyword="$$ "+word+"\n";
  size_t ebpos=dictn.find(keyword);
  if (ebpos!=string::npos) {
    size_t eepos=dictn.find("$$",ebpos+word.size()+3);
    string entry=dictn.substr(ebpos+word.size()+3,eepos-ebpos-word.size()-3);
    vector<string> catentset;
    string septor="**";
    getset(entry,septor,catentset);
    char found=0;
    for (vector<string>::iterator j=catentset.begin();j!=catentset.end();j++) {
      size_t rulepos=j->find("&&");
      string catent,rulent;
      if (rulepos!=string::npos) {
	catent=j->substr(0,rulepos);
	rulent=j->substr(rulepos);
      }
      else {
	catent=(*j);
	rulent="";
      }
      
      size_t idbpos=catent.find("{");
      size_t idepos=catent.find("}",idbpos);
      size_t catbpos=idepos+1;
      while (catent[catbpos]==' ')
	catbpos++;
      size_t catepos=catbpos;
      while (isalpha((unsigned char)catent[catepos]))
	catepos++;
      size_t featpos=catent.find("$=[",catepos);
      size_t spapos=catent.find(" ",featpos);
      size_t swipos=catent.find("||",featpos);
      size_t tranpos=catent.find("=>",featpos);

      string tempcat=catent.substr(catbpos,catepos-catbpos);
      if (tempcat==cat) {
	tag.sentreg.begin=bpos;
	tag.sentreg.end=epos;
	tag.word=word;
	tag.cat=cat;
	tag.catid=catent.substr(idbpos+1,idepos-idbpos-1);
	if (featpos!=string::npos) {
	  string temp=catent.substr(featpos+2,min(spapos,min(swipos,tranpos))-featpos-2);
	  removespace(temp,tag.feat);
	}
	string septor="&&";
	getset(rulent,septor,tag.ruleset);
	found=1;
	break;
      }
    }
    if (found==0) {
      cout << "there is no " << word << "with cat " << cat << " in dictionary" << endl; 
      assert(0);
    }
  }
  else {
    cout << "there is no " << word << " in dictionary" << endl; 
    assert(0);
  }
}
