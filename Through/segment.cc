#include <stdlib.h>
#include <vector>
#include <queue>
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <boost/regex.hpp>
#include "segment.hh"
#include "postag.hh"
#include "parse.hh"
#include "transform.hh"
#include "prsrbase.hh"
#include "extvar.hh"

using namespace std;

void segment(string & sent, vector<string> & words)
{
  cout << sent << endl;

  string tempsent="";
  string::const_iterator it=sent.begin();
  string::const_iterator end=sent.end();
  boost::smatch m;
  vector<string> subsents;
  while (boost::regex_search(it,end,m,spacereg)) {
    if (it!=m[0].first) {
      string subsent=string(it,m[0].first);
      tempsent+=subsent;
      vector<string> subwords;
      dictsegment(subsent,subwords);
      words.insert(words.end(),subwords.begin(),subwords.end());
    }
    it=m[0].second;
  }
  if (it!=end) {
    string subsent=string(it,end);
    tempsent+=subsent;
    vector<string> subwords;
    dictsegment(subsent,subwords);
    words.insert(words.end(),subwords.begin(),subwords.end());
  }
  sent=tempsent;
}

void dictsegment(string & sent, vector<string> & words)
{
  int currcount;
  vector<int> wordcount(sent.size()+1);
  vector<int> traceback(sent.size()+1);
  vector<int> tracefront(sent.size()+1);
  
  for (size_t i=0; i<=sent.size(); i++) {
    wordcount[i]=10000000;
    traceback[i]=i-1;
    tracefront[i]=i+1;
  }

  wordcount[0]=0;
  for (size_t i=0; i<=sent.size(); i++) {
    for (size_t j=0; j<i; j++) {
      string keyword="$$ "+sent.substr(j,i-j)+"\n";
      size_t ebpos=dictn.find(keyword);
      if (ebpos!=string::npos) {
	currcount=wordcount[j]-2*(i-j);
      }
      else {
	currcount=wordcount[j]+2*(i-j);
      }
      
      if (currcount < wordcount[i]) {
	traceback[i]=j;
	wordcount[i]=currcount;
      }
    }
  }
    
  for (int i=sent.size(); i>0; i=traceback[i])
    tracefront[traceback[i]]=i;
  
  for (size_t i=0; i<sent.size(); i=tracefront[i]) {
    string word=sent.substr(i, tracefront[i]-i);
    words.push_back(word);
  }
}

void loadkbase()
{
  ifstream modelf("model.utf8");
  ifstream dictnf("dictn.utf8");
  ifstream prsrbasef("prsrbase.utf8");
  ifstream makrbasef("makrbase.utf8");
  ifstream bldrbasef("bldrbase.utf8");

  model=string((std::istreambuf_iterator<char>(modelf)), std::istreambuf_iterator<char>());
  dictn=string((std::istreambuf_iterator<char>(dictnf)), std::istreambuf_iterator<char>());
  prsrbase=string((std::istreambuf_iterator<char>(prsrbasef)), std::istreambuf_iterator<char>());
  makrbase=string((std::istreambuf_iterator<char>(makrbasef)), std::istreambuf_iterator<char>());
  bldrbase=string((std::istreambuf_iterator<char>(bldrbasef)), std::istreambuf_iterator<char>());

  const boost::regex commreg("/\\*.*?\\*/");
  model=boost::regex_replace(model,commreg,"");
  dictn=boost::regex_replace(dictn,commreg,"");
  prsrbase=boost::regex_replace(prsrbase,commreg,"");
  makrbase=boost::regex_replace(makrbase,commreg,"");
  bldrbase=boost::regex_replace(bldrbase,commreg,"");

  string prsintro="&&";
  getset(prsrbase,prsintro,prsrset);
  string makintro="##";
  getset(makrbase,makintro,makrset);
  string bldintro="@@";
  getset(bldrbase,bldintro,bldrset);

  shallowprsrbase(prsrset,prsrtable);
}

void resetglobvar()
{
  chnwords.clear();
  chntags.clear();
  chnsegs.clear();
  chart.clear();
  parseresult.clear();
  compbeginmap.clear();
  compendmap.clear();
  incompbeginmap.clear();
  incompendmap.clear();
  compedgemap.clear();
  incedgemap.clear();
  compsentregmap.clear();
  incompsentregmap.clear();

  enchart.clear();
  chtoenidx.clear();
  entochidx.clear();
  trnsresult.clear();
  vector<int> eids;
  chtoenidx.insert(pair< int, vector<int> >(-1,eids));
}

void compseg(string & sent, vector<segc> & segs)
{
  //cout << sent << endl;
  int lbnum=0;
  string seq="";
  for (size_t i=0; i<sent.size(); i++) {
    if ((lbnum==0) && (sent[i]==' ')) {
      if (!seq.empty()) {
	segc seg;
	seg.seq=seq;
	seq="";
	segs.push_back(seg);
      }
    }
    else {
      seq+=sent[i];
      if (sent[i]=='(')
	lbnum++;
      else if (sent[i]==')')
	lbnum--;
    }
  }
  assert(lbnum==0);
  if (!seq.empty()) {
    segc seg;
    seg.seq=seq;
    seq="";
    segs.push_back(seg);
  }
  
  for (vector<segc>::iterator i=segs.begin();i!=segs.end();i++) {
    if (i->seq.find("(")!=string::npos)
      i->flag='p';
    else if (i->seq.find("/")!=string::npos)
      i->flag='t';
    else
      i->flag='w';
  }
}
