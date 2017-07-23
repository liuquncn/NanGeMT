#ifndef parse_h
#define parse_h

#include <vector>
#include <queue>
#include <map>
#include <sstream>
#include <string>
#include <iostream>
#include "postag.hh"

using namespace std;

class cfgc {
public:
  string reduct;
  vector<string> part;
  bool operator==(cfgc const & other) const {
    return((reduct==other.reduct) && (part==other.part));
  }
};

class prsrc {
public:
  string prsid;
  cfgc cfg;
  size_t head;
  string bind;
};

class edgec {
public:
  int id;
  string prsid;
  string swiid;
  string tranid;
  cfgc cfg;
  size_t head;
  string bind;
  regionc dotreg;
  regionc sentreg;
  vector<int> son;
  int father;
  char source;  // dict or prsrbase or input, d or p or i, input 表示输入时给出的情形
  char state;  // active or silent or fix, a or s or f, fix 表示输入时固定的情形
};

class loccatc {
public:
  size_t loc;
  string cat;
  bool operator<(loccatc const & other) const {
    if (loc<other.loc)
      return(true);
    else if (loc>other.loc)
      return(false);
    else
      return(cat<other.cat);
  }
};

class compedgekeyc {
public:
  string reduct;
  regionc sentreg;
  string bind;
  bool operator<(compedgekeyc const & other) const {
    ostringstream key;
    key << sentreg.begin << ";" << sentreg.end << ";"
	<< reduct << ";" 
	<< bind;
    ostringstream otherkey;
    otherkey << other.sentreg.begin << ";" << other.sentreg.end << ";"
	     << other.reduct << ";" 
	     << other.bind;
    return(key.str()<otherkey.str());
  }
  compedgekeyc(string & inreduct, regionc & insentreg, string & inbind) {
    reduct=inreduct;
    sentreg=insentreg;
    bind=inbind;
  }
};

class incedgekeyc {
public:
  string prsid;
  regionc dotreg;
  regionc sentreg;
  vector<int> son;
  bool operator<(incedgekeyc const & other) const {
    ostringstream key;
    key << sentreg.begin << ";" << sentreg.end << ";"
	<< prsid << ";" 
	<< dotreg.begin << ";" << dotreg.end << ";";
    //for (vector<int>::const_iterator i=son.begin();i!=son.end();i++)
    //key << *i << ";";
    ostringstream otherkey;
    otherkey << other.sentreg.begin << ";" << other.sentreg.end << ";"
	     << other.prsid << ";" 
	     << other.dotreg.begin << ";" << other.dotreg.end << ";";
    //for (vector<int>::const_iterator i=other.son.begin();i!=other.son.end();i++)
    //otherkey << *i << ";";
    return(key.str()<otherkey.str());
  }
  incedgekeyc(string & inprsid, regionc & indotreg, regionc & insentreg, vector<int> & inson) {
    prsid=inprsid;
    dotreg=indotreg;
    sentreg=insentreg;
    son=inson;
  }
};

void parse(vector<tagc> & tags);
void parse();
void initialize(vector<tagc> & tags);
void addincompchart(edgec & edge);
void addcompchart(edgec & edge);
void process(edgec & edge);
bool complete(edgec & edge);
void forward(edgec & edge);
void backward(edgec & edge);
bool match(cfgc & cfg, string & compon);
void tdpredict(edgec & edge);
void bupredict(edgec & edge);
bool ispos(string & syncat);
void transmit(string & hfeat, string & feat);
void getintextfeat(string & feat, string & infeat, string & exfeat);
void combinefeat(string & hfeat, string & feat, string & nfeat);
bool foundhname(string & hname, string & feat);
void getnameval(string & onefeat, string & fname, string & val);
void addtoagenda(vector<edgec> & agenda, edgec & edge);
void printedge(edgec & edge);
void bindtofeat(edgec & edge, string & feat);
void recurprinttree(edgec & edge);
void recurshowtree(edgec & edge, int spanum, int flag);
void initialchart(size_t sentlength, vector< vector < vector<edgec> > > & chart);
void choosebestparse(vector<int> & parseresult);
void printbestparse(vector<int> & parseresult);
void advrmspace(string & tempswibind, string & swibind);
void choosebestparse2(vector<int> & parseresult, vector<int> & parseresult2);
int countnode(edgec & root);
void setsilence(edgec & edge);
bool compatible(vector<regionc> & freezereg, regionc & reg);
void addfreeze(vector<regionc> & freezereg, regionc & reg);
void addcompmap(map<loccatc,vector<int> > & compbeginmap, map<loccatc,vector<int> > & compendmap, edgec & ce);
void addincompmap(map<loccatc,vector<int> > & incompbeginmap, map<loccatc,vector<int> > & incompendmap, edgec & ice);
void addsentregmap(map<regionc,vector<int> > & sentregmap, edgec & edge);
void setregsilence(map<regionc,vector<int> > & sentregmap, regionc & reg);
void addtochart(edgec & edge);
void printchart(vector<edgec> & chart, size_t bpos);
bool directreduct(edgec & edge);
void initialize(tagc & tag);
void makeedge(tagc & tag, char source, char state, edgec & edge);
void wordstrtoedge(string & wordstr, size_t & initpos, char state, vector<edgec> & edges);
void wordtagtoedge(string & wordtag, size_t & initpos, char state, vector<edgec> & edges);
void phrasetoedge(string & phrase, size_t & initpos, char state, vector<edgec> & edges);
void bodytoedge(string & body, size_t & initpos, vector< vector<edgec> > & edgem);
//void createedge(string & red, vector< vector<edgec> > & edgem, char state, vector<edgec> & edges);
void createedge(string & red, vector< vector<edgec> > & edgem, size_t initpos, char state, vector<edgec> & edges);
void createcfg(string & red, vector< vector<edgec> > & edgem, cfgc & cfg);
//bool findprsr(cfgc & cfg, prsrc & prsr);
void findprsr(cfgc & cfg, vector<prsrc> & prsrs);
void getedge(prsrc & prsr, size_t initpos, edgec & edge);
void makelocchart(vector< vector<edgec> > & edgem, vector<edgec> & locchart);
void parseoneprsr(edgec & iniedge, vector<edgec> & chart, char state, vector<edgec> & edges);
void sepredbody(string & phrase, string & red, string & body);
int choosebesttree(vector<int> roots);


#endif
