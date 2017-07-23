#ifndef globvar_h
#define globvar_h

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <streambuf>
#include <vector>
#include <set>
#include <map>
#include <boost/regex.hpp>
#include "postag.hh"
#include "parse.hh"

using namespace std;

string model;
string dictn;
string prsrbase;
string makrbase;
string bldrbase;
string locdictn;
vector<string> prsrset;
vector<string> makrset;
vector<string> bldrset;

string chnsent; 
vector<string> chnwords;
vector<segc> chnsegs;
vector<tagc> chntags;
edgec chntree;
edgec entree;

string shi;
string fou;
string hanzi;
string hanyucilei;
string hanyuduanyu;
string chongdie;
string liang;
string er;
string le;
string yi;
string guo;
string bu;
string yuyilei;
string yinjie;
string jufachengfen;
string yuyichengfen;
string zhongxinyu;
string yingyucilei;
string Rliang;
string Ner;
string NMer;
string VleV;
string VyiV;
string VleN;
string VguoN;
string VleyiN;
string VguoyiN;
string VbuleN;
string sczlxiaoshu;
string sczlfuhao;
string sczlshuliang;
string mczlng;
boost::regex numreg("[\\d]+[\\.\\d]*[%]*");
boost::regex idreg("\\{[A-Z|a-z|0-9|\\.]+\\}");
boost::regex spacereg("[\\s]+");
boost::regex brackreg("\\([A-Z|a-z|0-9|\\s|\\.]+\\)");

#endif
