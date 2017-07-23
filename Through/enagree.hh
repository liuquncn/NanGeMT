#ifndef enagree_h
#define enagree_h

#include <string>
#include <vector>
#include "parse.hh"

using namespace std;

bool enagree(string & bind, edgec & edge);
bool enaagree(string & bind, edgec & edge);
void enprocfeatval(edgec & edge, string & featname, string & featval, string & valtype, vector< vector<string> > & valset);
bool envalagree(string & lval, string & rval, string & valtype, string & opera, vector< vector<string> > & valset);
bool isenpos(string & syncat);
void enmodifeatval(edgec & ce, string & featname, string & nval);
void enrecurprocyx(edgec & edge, string & yxval);
bool trnagree(string & bind, edgec & edge, string & nbind);
bool trnbasagree(string & bind, edgec & edge, string & nbind);
bool proctrnagreeset(vector<string> & bindset, edgec & edge, string & nbind);
bool trnaagree(string & bind, edgec & edge, string & nbind);
void trnprocfeatval(edgec & edge, string & featname, string & featval, string & valtype, vector< vector<string> > & valset);
//edgec & trnfindsonedge(edgec & edge, string & nota);
void trnfindsonedge(edgec & edge, string & nota, edgec & sone);
void trnfindfeatval(edgec & ce, string & featname, string & featval, string & valtype, vector< vector<string> > & valset);
bool trnbasfeatval(edgec & ce, string & featname, string & featval, string & valtype, vector< vector<string> > & valset);
void trnfindsonedge(edgec & edge, string & nota, edgec & sone);
void trnfindedge(int eeid, edgec & edge);
int trnfindsonid(edgec & edge, string & nota);
int recurpostfinddescid(edgec & root, int repeat, string & cat);
void makfindfeatval(edgec & ce, string & featname, string & featval, string & valtype, vector< vector<string> > & valset);
bool makbasfeatval(edgec & ce, string & featname, string & featval, string & valtype, vector< vector<string> > & valset);
void bldbasfeatval(edgec & edge, string & fname, string & featval, string & valtype, vector< vector<string> > & valset);
void findecat(edgec & edge, string & ecat);

#endif
