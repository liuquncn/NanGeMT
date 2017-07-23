#ifndef segment_h
#define segment_h

#include <vector>
#include <string>
#include "parse.hh"

using namespace std;

void segment(string & sent, vector<string> & words);
void dictsegment(string & sent, vector<string> & words);
void loadkbase();
size_t dictnfind(string & word);
void resetglobvar();
void compseg(string & sent, vector<segc> & segs);

#endif
