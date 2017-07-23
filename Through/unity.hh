#ifndef unity_h
#define unity_h

#include <string>
#include "parse.hh"

using namespace std;

enum uresultc { SUCC, FAIL, WAIT };

uresultc unity(edgec & ice, size_t loc, edgec & ce, string & nbind);
uresultc basunity(cfgc & cfg, string & bind, size_t loc, edgec & ce, string & nbind);
uresultc aunity(cfgc & cfg, string & bind, size_t loc, edgec & ce, string & nbind);
//uresultc valunity(string & lval, string & rval, string & valtype, string & opera, vector< vector<string> > & valset);
uresultc opunity(string & lval, string & rval, string & valtype, string & opera, vector< vector<string> > & valset, string & nval);
uresultc equnity(string & lval, string & rval, string & valtype, vector< vector<string> > & valset, string & nval);
uresultc pureunity(string & lval, string & rval, string & valtype, vector< vector<string> > & valset, string & nval);
void valsepa(string & val, vector<string> & valset);
void checkmodel(string & featname, string & valtype, string & defval, vector< vector<string> > & valset);
uresultc notunity(uresultc ures);
void removespace(string & val, string & nval);
void commavalset(string & val, vector< vector<string> > & valset);
void eqvalset(string & val, vector<string> & valset);
size_t findval(string & val, vector< vector<string> > & valset);
bool inset(string & val, vector<string> & valset);
bool isshi(string & val);
bool isfou(string & val);
void procshifou(string & val);
void findccat(edgec & edge, string & ccat);
void findfeatval(edgec & ce, string & featname, string & featval, string & valtype, vector< vector<string> > & valset);
void findedge(int eid, edgec & edge);
void basfeatval(edgec & ce, string & featname, string & featval, string & valtype, vector< vector<string> > & valset);
void getbindset(string & bind, vector<string> & bindset);
uresultc procbindset(vector<string> & bindset, cfgc & cfg, size_t loc, edgec & ce, string & nbind);
void getabindset(string & bind, vector<string> & bindset);
void getpercat(cfgc & cfg, size_t loc, string & percat);
void getandset(string & val, vector<string> & andset);
uresultc notequnity(string & lval, string & rval, string & valtype, vector< vector<string> > & valset, string & nval);
uresultc valunity(string & lval, string & rval, string & valtype, string & opera, vector< vector<string> > & valset, string & newval);
edgec & findedge(int eid);
edgec & findicedge(int eid);
void getset(string & bigunit, string & septor, vector<string> & itemset);
void getoneunit(string & bigunit, string & subunit, string & rem);
void getatset(string & bind, vector<string> & atset);
void getopeval(string & bind, string & lval, string & opera, string & rval);
bool getfeatval(string & feat, string & featname, string & featval);
bool moregetfeatval(edgec & edge, string & featname, string & featval);

#endif
