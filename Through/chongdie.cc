#include <assert.h>
#include "transform.hh"
#include "unity.hh"
#include "parse.hh"
#include "chongdie.hh"

using namespace std;

extern string shi;
extern string fou;
extern string hanzi;
extern string hanyucilei;
extern string chongdie;
extern string liang;
extern string er;
extern string le;
extern string yi;
extern string guo;
extern string bu;
extern string Rliang;
extern string Ner;
extern string NMer;
extern string VleV;
extern string VyiV;
extern string VleN;
extern string VguoN;
extern string VleyiN;
extern string VguoyiN;
extern string VbuleN;

// according to ovlrbase.utf8

void procchongdie(edgec & edge, string & featval)
{
  if (edge.son.size()==2) {
    edgec firword,secword;
    findedge(edge.son[0],firword);
    findedge(edge.son[1],secword);
    if (firword.cfg.part.front()==secword.cfg.part.front()) {
      if (firword.cfg.reduct=="a")
	featval="AA";
      else if (firword.cfg.part.front()=="d")
	featval="DD";
      else if (firword.cfg.part.front()=="v")
	featval="VV";
    }
    else if ((firword.cfg.reduct=="r") && (secword.cfg.part.front()==liang))
      featval=Rliang; 
    else if ((firword.cfg.reduct=="n") && (secword.cfg.part.front()==er))
      featval=Ner; 
  }
  else if (edge.son.size()==3) {
    edgec firword,secword,thiword;
    findedge(edge.son[0],firword);
    findedge(edge.son[1],secword);
    findedge(edge.son[2],thiword);
    if ((firword.cfg.reduct=="m") && (secword.cfg.reduct=="n") && (thiword.cfg.part.front()==er))
      featval=NMer; 
    else if ((firword.cfg.reduct=="v") && (thiword.cfg.reduct=="v") && (secword.cfg.part.front()==le))
      featval=VleV; 
    else if ((firword.cfg.reduct=="v") && (thiword.cfg.reduct=="v") && (secword.cfg.part.front()==yi))
      featval=VyiV; 
    else if ((firword.cfg.reduct=="v") && (thiword.cfg.reduct=="n") && (secword.cfg.part.front()==le))
      featval=VleN; 
    else if ((firword.cfg.reduct=="v") && (secword.cfg.reduct=="v") && (thiword.cfg.reduct=="n"))
      featval="VVN";
    else if ((firword.cfg.reduct=="v") && (thiword.cfg.reduct=="n") && (secword.cfg.part.front()==guo))
      featval=VguoN; 
  }
  else if (edge.son.size()==4) {
    edgec firword,secword,thiword,fouword;
    findedge(edge.son[0],firword);
    findedge(edge.son[1],secword);
    findedge(edge.son[2],thiword);
    findedge(edge.son[3],fouword);
    if ((firword.cfg.reduct=="a") && 
	(secword.cfg.reduct=="a") && 
	(thiword.cfg.reduct=="a") && 
	(fouword.cfg.reduct=="a") &&
	(firword.cfg.part.front()==secword.cfg.part.front()) &&
	(thiword.cfg.part.front()==fouword.cfg.part.front()))
      featval="AABB";
    else if ((firword.cfg.reduct=="a") && 
	     (secword.cfg.reduct=="a") && 
	     (thiword.cfg.reduct=="a") && 
	     (fouword.cfg.reduct=="a") &&
	     (firword.cfg.part.front()==thiword.cfg.part.front()) &&
	     (secword.cfg.part.front()==fouword.cfg.part.front())) 
      featval="ABAB";
    else if ((firword.cfg.reduct=="u") && 
	     (thiword.cfg.reduct=="u") &&
	     (secword.cfg.reduct=="v") && 
	     (fouword.cfg.reduct=="v") &&
	     (firword.cfg.part.front()==thiword.cfg.part.front()) &&
	     (secword.cfg.part.front()==fouword.cfg.part.front()))
      featval="UVUV";
    else if ((firword.cfg.reduct=="v") &&
	     (secword.cfg.part.front()==le) &&
	     (thiword.cfg.part.front()==yi) &&
	     (fouword.cfg.reduct=="n"))
      featval=VleyiN; 
    else if ((firword.cfg.reduct=="v") &&
	     (secword.cfg.part.front()==guo) &&
	     (thiword.cfg.part.front()==yi) &&
	     (fouword.cfg.reduct=="n"))
      featval=VguoyiN; 
    else if ((firword.cfg.reduct=="v") &&
	     (secword.cfg.part.front()==bu) &&
	     (thiword.cfg.part.front()==le) &&
	     (fouword.cfg.reduct=="n"))
      featval=VbuleN; 
  }
  else
    featval=fou; //"NULL";
}

void recurprocyx(edgec & edge, string & yxval)
{
  edgec desce;
  findfinaldesc(edge,desce);
  if (desce.son.empty() && !desce.cfg.part.empty())
    yxval=desce.cfg.part.front();
  else
    yxval="";
}

void procyinjin(edgec & edge, string & yinjieval)
{
  if (!edge.cfg.reduct.empty()) {
    string yxstr;
    recurprocyx(edge,yxstr);
    stringstream temp;
    temp << yxstr.size()/shi.size();  // shi.size() represent size of a chinese character
    yinjieval=temp.str();
  }
  else
    yinjieval="0";
}

void findfinaldesc(edgec & edge, edgec & desce)
{
  if (edge.son.empty())
    desce=edge;
  else {
    if ((edge.head>=0) && (edge.head<edge.son.size())) {
      edgec & hsone=findedge(edge.son[edge.head]);
      findfinaldesc(hsone,desce);
    }
  }
}
