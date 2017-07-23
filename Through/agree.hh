#ifndef agree_h
#define agree_h

#include <string>
#include <vector>
#include "unity.hh"

bool agree(string & bind, edgec & edge, string & nbind);
bool agree(string & bind, edgec & edge);
bool procagreeset(vector<string> & bindset, edgec & edge, string & nbind);
bool procagreeset(vector<string> & bindset, edgec & edge);
bool basagree(string & bind, edgec & edge, string & nbind);
//bool basagree(string & bind, edgec & edge);
bool aagree(string & bind, edgec & edge, string & nbind);
//bool aagree(string & bind, edgec & edge);
void findsonedge(edgec & edge, string & nota, edgec & sone);
int findsonid(edgec & edge, string & nota);
bool valagree(string & lval, string & rval, string & valtype, string & opera, vector< vector<string> > & valset, string & nval);
bool opagree(string & lval, string & rval, string & valtype, string & opera, vector< vector<string> > & valset, string & nval);
bool eqagree(string & lval, string & rval, string & valtype, vector< vector<string> > & valset, string & nval);
bool aeqagree(string & lval, string & rval, string & valtype, vector< vector<string> > & valset, string & nval);
bool pureagree(string & lval, string & rval, string & valtype, vector< vector<string> > & valset, string & nval);
void getnotbind(string & bind, string & notbind);
void gethierarcode(string & name, string & code);
edgec & findsonedge(edgec & edge, string & nota);

#endif
