#include <assert.h>
#include <stdlib.h>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <sstream>
#include "parse.hh"
#include "unity.hh"
#include "chongdie.hh"
#include "transform.hh"
#include "generate.hh"
#include "agree.hh"

using namespace std;

extern string model;
extern string hanzi;
extern string shi;
extern string fou;
extern string chongdie;
extern string yuyilei;
extern string yinjie;
extern string jufachengfen;
extern string yuyichengfen;
extern string hanyucilei;
extern string hanyuduanyu;
extern vector<edgec> chart;

void findfeatval(edgec & ce, string & featname, string & featval, string & valtype, vector< vector<string> > & valset)
{
  string temp="."+featname;
  string septor=".";
  vector<string> nameset;
  getset(temp,septor,nameset);
  edgec tempedge=ce;
  for (vector<string>::iterator i=nameset.begin();i!=nameset.end();i++) {
    featval="";
    basfeatval(tempedge,(*i),featval,valtype,valset);
    if (valtype=="Edge") {
      //procshifou(featval);
      //if (featval.empty() || (featval==fou) || (featval[0]=='~')) { 
      //	edgec empedge;
      //	tempedge=empedge;
      //}
      /*else*/ 
      if (featval[0]=='[') {
	edgec nedge;
	nedge.bind=featval;
	tempedge=nedge;
      }
      else if (featval.find("eid")==0) {
	int eid=atoi(featval.substr(3).c_str());
	findedge(eid,tempedge);
      }
      else {
      	edgec empedge;
      	tempedge=empedge;
      }
    }
  }
}

void basfeatval(edgec & ce, string & featname, string & featval, string & valtype, vector< vector<string> > & valset)
{
  string defval;
  if (featname.find("eid")!=string::npos) {
    valtype="Edge";
    featval=featname;
  }
  else if (featname==hanzi) {
    valtype="Symbol";
    featval=ce.cfg.part.front();
  }
  else if (featname=="yx") {
    valtype="Symbol";
    recurprocyx(ce,featval);
  }
  else if (featname==chongdie) {
    valtype="Symbol";
    procchongdie(ce,featval);
  }
  else if ((featname=="ccat") || (featname==hanyucilei)) {
    valtype="Symbol";
    findccat(ce,featval);
  }
  else if ((featname=="cpcat") || (featname==hanyuduanyu)) {
    valtype="Symbol";
    featval=ce.cfg.reduct;
  }
  else if (featname==yinjie) {
    valtype="Number";
    procyinjin(ce,featval);
  }
  else if ((featname==yuyilei) || (featname=="sem")) {
    valtype="Hierar";
    moregetfeatval(ce,featname,featval);
  }
  else if (jufachengfen.find(","+featname+",")!=string::npos) {
    valtype="Edge";
    getfeatval(ce.bind,featname,featval);
  }
  else if (yuyichengfen.find(","+featname+",")!=string::npos) {
    valtype="Edge";
    moregetfeatval(ce,featname,featval);
  }
  else {
    checkmodel(featname,valtype,defval,valset);
    bool found=moregetfeatval(ce,featname,featval);
    if (!found)
      featval=defval;
  }
}

void findedge(int eid, edgec & edge)
{
  assert((size_t)eid<chart.size());
  edge=chart[eid];
}

edgec & findedge(int eid)
{
  assert((size_t)eid<chart.size());
  return(chart[eid]);
}

void findccat(edgec & edge, string & ccat)
{
  edgec headedge;
  if (!edge.cfg.reduct.empty()) {
    edgec curedge=edge;
    string syncat=edge.cfg.reduct;
    while (!ispos(syncat)) {
      assert(curedge.head<curedge.son.size());
      int headid=curedge.son[curedge.head];
      findedge(headid,headedge);
      syncat=headedge.cfg.reduct;
      curedge=headedge;
    }
    assert(ispos(syncat));
    ccat=syncat;
  }
  else
    ccat="";
}

void checkmodel(string & featname, string & valtype, string & defval, vector< vector<string> > & valset)
{
  size_t featpos=model.find("("+featname+")");
  if (featpos==string::npos) {
    size_t aepos=model.find("="+featname);
    if (aepos==string::npos) {
      valtype="Unknown";
      return;
    }
    size_t abpos=aepos;
    while (!isspace((unsigned char)model[abpos]))
      abpos--;
    string alias=model.substr(abpos+1,aepos-abpos-1);
    featpos=model.find("("+alias+")");
    if (featpos==string::npos) {
      valtype="Unknown";
      return;
    }
  }
  size_t colonpos=model.find(":",featpos);
  assert(colonpos!=string::npos);
  size_t ncolpos=model.find(":",colonpos+1);
  string featbuf=model.substr(colonpos+1,ncolpos-colonpos-1);
  size_t defpos=featbuf.find("Default=");
  if (defpos!=string::npos) {
    size_t vspos=defpos+8;
    size_t vepos=vspos;
    while (!isspace((unsigned char)featbuf[vepos]))
      vepos++;
    defval=featbuf.substr(vspos,vepos-vspos);
  }
  size_t lbraktpos=featbuf.find("{");
  if (lbraktpos!=string::npos) {
    size_t rbraktpos=featbuf.find("}");
    string val=featbuf.substr(lbraktpos+1,rbraktpos-lbraktpos-1);
    commavalset(val,valset);
  }
  size_t tbpos=0;
  while (isspace((unsigned char)featbuf[tbpos]))
    tbpos++;
  size_t relpos=featbuf.find("RelMode");
  size_t ebpos=min(lbraktpos,min(defpos,relpos));
  valtype=featbuf.substr(tbpos,ebpos-tbpos);
}

void commavalset(string & val, vector< vector<string> > & valset)
{
  size_t spos=0;
  size_t commapos=val.find(",");
  while (commapos!=string::npos) {
    string oneval=val.substr(spos,commapos-spos);
    vector<string> oneset;
    eqvalset(oneval,oneset);
    valset.push_back(oneset);
    spos=commapos+1;
    commapos=val.find(",",spos);
  }
  string oneval=val.substr(spos);
  vector<string> oneset;
  eqvalset(oneval,oneset);
  valset.push_back(oneset);
}

void eqvalset(string & val, vector<string> & valset)
{
  size_t spos=0;
  size_t eqpos=val.find("=");
  while (eqpos!=string::npos) {
    string nsval;
    string valsub=val.substr(spos,eqpos-spos);
    removespace(valsub,nsval);
    procshifou(nsval);
    valset.push_back(nsval);
    spos=eqpos+1;
    eqpos=val.find("=",spos);
  }
  string nsval;
  string valsub=val.substr(spos);
  removespace(valsub,nsval);
  procshifou(nsval);
  valset.push_back(nsval);
}

void procshifou(string & val)
{
  if (isshi(val))
    val=shi;
  else if (isfou(val))
    val=fou;
}

bool isshi(string & val)
{
  if ((val=="YES") || (val=="Yes") || (val=="yes") || (val==shi))
    return(true);
  else
    return(false);
}

bool isfou(string & val)
{
  if ((val=="NO") || (val=="No") || (val=="no") || (val==fou))
    return(true);
  else
    return(false);
}

void getandset(string & val, vector<string> & andset)
{
  string septor="~";
  //if (val[0]=='~') {
  if (val.find("~")==0) { 
    getset(val,septor,andset);
    for (vector<string>::iterator i=andset.begin();i!=andset.end();i++)
      (*i)="~"+(*i);
  }
  else {
    string temp="~"+val;
    getset(temp,septor,andset);
    for (vector<string>::iterator i=andset.begin()+1;i!=andset.end();i++)
      (*i)="~"+(*i);
  }
}

size_t findval(string & val, vector< vector<string> > & valset)
{
  procshifou(val);
  for (vector< vector<string> >::iterator i=valset.begin();i!=valset.end();i++) {
    if (inset(val,*i))
      return(i-valset.begin());
  }
  for (vector< vector<string> >::iterator i=valset.begin();i!=valset.end();i++) {
    for (vector<string>::iterator j=i->begin();j!=i->end();j++) {
      cout << *j << " ";
    }
    cout << endl;
  }
  cout << val << endl;
  assert(false);
}

bool inset(string & val, vector<string> & valset)
{
  for (vector<string>::iterator i=valset.begin();i!=valset.end();i++) {
    if (val==*i)
      return(true);
  }
  return(false);
}

void valsepa(string & val, vector<string> & valset)
{
  //if ((val[0]=='[') || (val.find("~[")==0))
  if ((val.find("[")==0) || (val.find("~[")==0))
    valset.push_back(val);
  else {
    size_t stpos=0;
    size_t orpos=val.find("|");
    while (orpos!=string::npos) {
      valset.push_back(val.substr(stpos,orpos-stpos));
      stpos=orpos+1;
      orpos=val.find("|",stpos);
    }
    valset.push_back(val.substr(stpos));
  }
}

void removespace(string & val, string & nval)
{
  if (!val.empty()) {
    size_t spos=0;
    while ((spos<val.size()) && isspace((unsigned char)val[spos]))
      spos++;
    if (spos==val.size())
      nval="";
    else {
      size_t epos=val.size()-1;
      while (isspace((unsigned char)val[epos]))
	epos--;
      assert(epos>=0);
      nval=val.substr(spos,epos-spos+1);
    }
  }
  else
    nval=val;
}

void getbindset(string & bind, vector<string> & bindset)
{
  size_t ifpos=bind.find("IF");
  if (ifpos==string::npos)
    bindset.push_back(bind);
  else {
    bindset.push_back(bind.substr(0,ifpos));
    size_t eifpos;
    while (ifpos!=string::npos) {
      size_t endifpos=bind.find("ENDIF",ifpos);
      size_t truepos=bind.find("TRUE",ifpos);
      size_t falsepos=bind.find("FALSE",ifpos);
      eifpos=min(endifpos,min(truepos,falsepos));
      assert(eifpos!=string::npos);
      if (eifpos==truepos)
	eifpos += 4;
      else
	eifpos += 5;
      bindset.push_back(bind.substr(ifpos,eifpos-ifpos));
      ifpos=bind.find("IF",eifpos);
    }
    if (bind.substr(eifpos)!="")
      bindset.push_back(bind.substr(eifpos));
  }
}

void getabindset(string & bind, vector<string> & bindset)
{
  string temp;
  removespace(bind,temp);
  if (!temp.empty()) {
    size_t bpos=0;
    size_t mpos=0;
    int braknum=0;
    while (mpos<temp.size()) {
      if (temp[mpos]=='[')
	braknum++;
      if (temp[mpos]==']')
	braknum--;
      if ((temp[mpos]==',') && (braknum==0)) {
	string subtemp=temp.substr(bpos,mpos-bpos);
	string atemp;
	removespace(subtemp,atemp);
	if (!atemp.empty())
	  bindset.push_back(atemp);
	bpos=mpos+1;
      }
      mpos++;
    }
    if (bpos<mpos) {
      if (braknum!=0)
	cout << bind << endl;
      assert(braknum==0);
      string subtemp=temp.substr(bpos);
      string atemp;
      removespace(subtemp,atemp);
      if (!atemp.empty())
	bindset.push_back(atemp);
    }
  }
}

void getpercat(cfgc & cfg, size_t loc, string & percat)
{
  int repeat=1;
  for (size_t i=0;i<loc;i++) {
    if (cfg.part[i]==cfg.part[loc])
      repeat++;
  }
  string intro;
  for (int i=0;i<repeat;i++)
    intro += "%";
  intro += cfg.part[loc];
  percat=intro;
}

void getset(string & bigunit, string & septor, vector<string> & itemset)
{
  size_t bpos=bigunit.find(septor);
  if (bpos!=string::npos) {
    size_t epos=bigunit.find(septor,bpos+septor.size());
    while (bpos!=string::npos) {
      string aitem=bigunit.substr(bpos+septor.size(),epos-bpos-septor.size());
      itemset.push_back(aitem);
      bpos=epos;
      epos=bigunit.find(septor,bpos+septor.size());
    }
  }
}

void getoneunit(string & bigunit, string & subunit, string & rem)
{
  size_t mpos=0;
  int branum=0;
  int angnum=0;
  while ((mpos<bigunit.size()) && (bigunit[mpos]==' '))
    mpos++;
  bigunit=bigunit.substr(mpos);
  mpos=0;
  while (mpos<bigunit.size()) {
    if ((bigunit[mpos]==' ') &&  (branum==0) && (angnum==0))
      break;
    if (bigunit[mpos]=='(')
      branum++;
    else if (bigunit[mpos]==')')
      branum--;
    else if (bigunit[mpos]=='<')
      angnum++;
    else if (bigunit[mpos]=='>')
      angnum--;
    mpos++;
  }
  subunit=bigunit.substr(0,mpos);
  while (mpos<bigunit.size() && bigunit[mpos]==' ')
    mpos++;
  rem=bigunit.substr(mpos);
}

void getatset(string & bind, vector<string> & atset)
{
  size_t atpos=bind.find("@");
  while (atpos!=string::npos) {
    size_t atend=atpos+1;
    while ((atend<bind.size()) && !isspace((unsigned char)bind[atend]))
      atend++;
    atset.push_back(bind.substr(atpos+1,atend-atpos-1));
    atpos=bind.find("@",atend);
  }
}

void getopeval(string & bind, string & lval, string & opera, string & rval)
{
  string temp;
  removespace(bind,temp);
  size_t eqpos=temp.find("=");
  size_t seqpos;
  size_t eeqpos;
  assert(eqpos!=string::npos);
  /*
  if (eqpos==string::npos) {
    cout << "|" << temp << "|" << endl;
    exit(1);
  }
  */
  if (eqpos-1>=0 && eqpos-1<temp.size() && temp[eqpos-1]=='!') {
    seqpos=eqpos-1;
    eeqpos=eqpos;
  }
  else if (eqpos+1<temp.size() && temp[eqpos+1]=='=') {
    seqpos=eqpos;
    eeqpos=eqpos+1;
  }
  else {
    seqpos=eqpos;
    eeqpos=eqpos;
  }
  opera=temp.substr(seqpos,eeqpos-seqpos+1);
  lval=temp.substr(0,seqpos);
  rval=temp.substr(eeqpos+1);
}

bool getfeatval(string & feat, string & featname, string & featval)
{
  size_t featpos=feat.find(","+featname+":");
  if (featpos==string::npos)
    featpos=feat.find("["+featname+":");
  if (featpos==string::npos)
    featpos=feat.find("{"+featname+":");
  if (featpos!=string::npos) {
    size_t colonpos=feat.find(":",featpos);
    assert(colonpos!=string::npos);
    if ((feat[colonpos+1]=='[') || ((feat[colonpos+1]=='~') && (feat[colonpos+2]=='['))) { 
      size_t braktpos=feat.find("]",colonpos);
      assert(braktpos!=string::npos);
      featval=feat.substr(colonpos+1,braktpos-colonpos);
    }
    else {
      size_t commapos=feat.find(",",colonpos);
      size_t braktpos=feat.find("]",colonpos);
      featval=feat.substr(colonpos+1,min(commapos,braktpos)-colonpos-1);
    }
    return(true);
  }
  else
    return(false);
}

bool moregetfeatval(edgec & edge, string & featname, string & featval)
{
  edgec tempe=edge;
  bool found=getfeatval(tempe.bind,featname,featval);
  while (!found) {
    if ((tempe.head>=0) && (tempe.head<tempe.son.size())) {
      edgec hsone;
      findedge(tempe.son[tempe.head],hsone);
      found=getfeatval(hsone.bind,featname,featval);
      tempe=hsone;
    }
    else
      break;
  }
  return(found);
}
