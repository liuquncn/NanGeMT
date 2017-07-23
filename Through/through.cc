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
#include "globvar.hh"
#include "segment.hh"
#include "postag.hh"
#include "parse.hh"
#include "transform.hh"
#include "generate.hh"

using namespace std;

extern vector<edgec> chart;
extern map<int,vector<edgec> > enchart; 
extern vector<int> parseresult;
extern vector<int> trnsresult;
extern vector< vector<edgec> > makresult;
extern vector< vector<edgec> > bldresult;
void inputsent(ifstream & chnfile, string & chnsent);
void loadmodelcore();

int main()
{
  loadmodelcore();
  loadkbase();
  ifstream chnfile;
  chnfile.open("chnsent.txt");
  while (!chnfile.eof()) {
    string compsent;
    inputsent(chnfile,compsent);
    if (compsent.empty()) 
      break;
    else {
      resetglobvar();
      compseg(compsent,chnsegs);
      parse();
      transform();
      maktree();
      bldword();
      printresult();
    }
  }
  chnfile.close();
}

void inputsent(ifstream & chnfile, string & chnsent)
{
  string temp;
  getline(chnfile,temp);
  cout << temp << endl;
  removespace(temp,chnsent);
}

void loadmodelcore()
{
  ifstream spewordf;
  spewordf.open("speword.utf8");
  getline(spewordf,shi);
  getline(spewordf,fou);
  getline(spewordf,hanzi);
  getline(spewordf,hanyucilei);
  getline(spewordf,hanyuduanyu);
  getline(spewordf,chongdie);
  getline(spewordf,liang);
  getline(spewordf,er);
  getline(spewordf,le);
  getline(spewordf,yi);
  getline(spewordf,guo);
  getline(spewordf,bu);
  getline(spewordf,yuyilei);
  getline(spewordf,yinjie);
  getline(spewordf,jufachengfen);
  getline(spewordf,yuyichengfen);
  getline(spewordf,zhongxinyu);
  getline(spewordf,yingyucilei);
  getline(spewordf,Rliang);
  getline(spewordf,Ner);
  getline(spewordf,NMer);
  getline(spewordf,VleV);
  getline(spewordf,VyiV);
  getline(spewordf,VleN);
  getline(spewordf,VguoN);
  getline(spewordf,VleyiN);
  getline(spewordf,VguoyiN);
  getline(spewordf,VbuleN);
  getline(spewordf,sczlxiaoshu);
  getline(spewordf,sczlfuhao);
  getline(spewordf,sczlshuliang);
  getline(spewordf,mczlng);
  spewordf.close();
}
