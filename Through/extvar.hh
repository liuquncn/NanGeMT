#ifndef extvar_h
#define extvar_h

#include <boost/regex.hpp>

extern string model;
extern string dictn;
extern string prsrbase;
extern string makrbase;
extern string bldrbase;
extern string locdictn;
extern vector<string> prsrset;
extern vector<string> makrset;
extern vector<string> bldrset;

extern string chnsent;
extern vector<string> chnwords;
extern vector<tagc> chntags;
extern vector<segc> chnsegs;
extern boost::regex numreg;
extern boost::regex spacereg;
extern boost::regex brackreg;
extern vector<edgec> chart;
extern vector<int> parseresult;
extern vector<prsrc> prsrtable;
extern map<string,vector<int> > prsredmap;
extern map<string,vector<int> > prsfirstmap;
extern map<string,vector<int> > prslastmap;
extern map<loccatc,vector<int> > compbeginmap;
extern map<loccatc,vector<int> > compendmap;
extern map<loccatc,vector<int> > incompbeginmap;
extern map<loccatc,vector<int> > incompendmap;
extern map<compedgekeyc,int> compedgemap;
extern map<incedgekeyc,int> incedgemap;
extern map<regionc,vector<int> > compsentregmap;
extern map<regionc,vector<int> > incompsentregmap;
extern vector<edgec> enchart;
extern map< int,vector<int> > chtoenidx;
extern map<int,int> entochidx;
extern vector<int> trnsresult;
extern vector< vector<edgec> > makresult;
extern vector< vector<edgec> > bldresult;

#endif
