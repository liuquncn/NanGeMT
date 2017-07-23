#include <stdlib.h>
#include <vector>
#include <assert.h>
#include "transform.hh"
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
extern string hanyucilei;
extern string hanyuduanyu;

bool agree(string & bind, edgec & edge, string & nbind)
{
  vector<string> bindset;
  getbindset(bind,bindset);
  return(procagreeset(bindset,edge,nbind));
}

bool procagreeset(vector<string> & bindset, edgec & edge, string & nbind)
{
  for (vector<string>::iterator i=bindset.begin(); i!=bindset.end(); i++) {
    size_t ifpos=i->find("IF");
    size_t truepos=i->find("TRUE");
    size_t falsepos=i->find("FALSE");
    size_t thenpos=i->find("THEN");
    size_t elsepos=i->find("ELSE");
    size_t endifpos=i->find("ENDIF");
    string anbind;
    if (ifpos==string::npos) {
      // basic case
      string temp=(*i);
      bool res=basagree(temp,edge,anbind);
      if (res==false)
	return(false);
      else
	nbind+=anbind;
    }
    else if (truepos!=string::npos) {
      // if <bind> TRUE
      string temp=i->substr(ifpos+3,truepos-ifpos-3);
      bool res=basagree(temp,edge,anbind);
      if (res==false)
	return(false);
      else
	nbind+=anbind;
    }
    else if (falsepos!=string::npos) {
      // if <bind> FALSE
      string temp=i->substr(ifpos+3,falsepos-ifpos-3);
      bool res=basagree(temp,edge,anbind);
      if (res==true)
	return(false);
    }
    else if ((thenpos!=string::npos) && (elsepos==string::npos)) {
      // if <bind> then <bind> endif
      string iftemp=i->substr(ifpos+3,thenpos-ifpos-3);
      string thentemp=i->substr(thenpos+5,endifpos-thenpos-5);
      string nifbind;
      string nthenbind;
      bool ifres=basagree(iftemp,edge,nifbind);
      bool thenres=basagree(thentemp,edge,nthenbind);
      if ((ifres==true) && (thenres==false))
	return(false);
      else if ((ifres==true) && (thenres==true))
	nbind+=nifbind+nthenbind;
    }
    else if ((thenpos==string::npos) && (elsepos!=string::npos)) {
      // if <bind> else <bind> endif
      string iftemp=i->substr(ifpos+3,elsepos-ifpos-3);
      string elsetemp=i->substr(elsepos+5,endifpos-elsepos-5);
      string nifbind;
      string nelsebind;
      bool ifres=basagree(iftemp,edge,nifbind);
      bool elseres=basagree(elsetemp,edge,nelsebind);
      if ((ifres==false) && (elseres==false))
	return(false);
      else if (ifres==true)
	nbind+=nifbind;
    }
    else if ((thenpos!=string::npos) && (elsepos!=string::npos)) {
      // if <bind> then <bind> else <bind> endif
      string iftemp=i->substr(ifpos+3,thenpos-ifpos-3);
      string thentemp=i->substr(thenpos+5,elsepos-thenpos-5);
      string elsetemp=i->substr(elsepos+5,endifpos-elsepos-5);
      string nifbind;
      string nthenbind;
      string nelsebind;
      bool ifres=basagree(iftemp,edge,nifbind);
      bool thenres=basagree(thentemp,edge,nthenbind);
      bool elseres=basagree(elsetemp,edge,nelsebind);
      if (((ifres==true) && (thenres==false)) || ((ifres==false) && (elseres==false)))
	return(false);
      else if ((ifres==true) && (thenres==true))
	nbind+=nifbind+nthenbind;
    }
  }
  return(true);
}

bool basagree(string & bind, edgec & edge, string & nbind)
{
  vector<string> bindset;
  getabindset(bind,bindset);
  for (vector<string>::iterator i=bindset.begin();i!=bindset.end();i++) {
    string anbind;
    bool res=aagree((*i),edge,anbind);
    if (res==false)
      return(false);
    else
      nbind += anbind;
  }
  return(true);
}

bool aagree(string & bind, edgec & edge, string & nbind)
{
  size_t atpos=bind.find("@");
  if (atpos!=string::npos) {
    vector<string> atset;
    getatset(bind,atset);
    string opera;
    string lfeat;
    string rfeat;
    string temp=bind.substr(0,atpos);
    getopeval(temp,lfeat,opera,rfeat);
    for (vector<string>::iterator i=atset.begin();i!=atset.end();i++) {
      string atbind=lfeat+"."+*i+opera+rfeat+"."+*i;
      string natbind;
      bool res=aagree(atbind,edge,natbind);
      if (res==false)
	return(false);
      else
	nbind+=natbind;
    }
    return(true);
  }
  else {
    string opera;
    string lfeat;
    string rfeat;
    getopeval(bind,lfeat,opera,rfeat);
    string rval;
    string rvaltype;
    vector< vector<string> > rvalset;
    procfeatval(edge,rfeat,rval,rvaltype,rvalset);
    if ((lfeat.find("$")==0) && edge.bind.empty()) {
      nbind=lfeat+opera+rval+",";
      return(true);
    }
    else {
      string lval;
      string lvaltype;
      vector< vector<string> > lvalset;
      procfeatval(edge,lfeat,lval,lvaltype,lvalset);
      string newval;
      if (!lvaltype.empty()) {
	bool res=valagree(lval,rval,lvaltype,opera,lvalset,newval);
	if (res==false)
	  return(false);
	else {
	  if (lfeat.find(".")!=string::npos)
	    nbind=lfeat+"="+newval+",";
	  return(true);
	}
      } 
      else if (!rvaltype.empty()) {
	bool res=valagree(lval,rval,rvaltype,opera,rvalset,newval);
	if (res==false)
	  return(false);
	else {
	  if (rfeat.find(".")!=string::npos)
	    nbind=rfeat+"="+newval+",";
	  return(true);
	}
      }
      assert(false);
    }
  }
}

bool valagree(string & lval, string & rval, string & valtype, string & opera, vector< vector<string> > & valset, string & nval)
{
  if (lval.find("[")==0) {
    if (rval.find("[")==0) {
      rval.replace(0,1,"");
      size_t rbrapos=rval.rfind("]");
      assert(rbrapos!=string::npos);
      rval.replace(rbrapos,1,"");
      size_t colonpos=rval.find(":");
      while (colonpos!=string::npos) {
	rval.replace(colonpos,1,"=");
	colonpos=rval.find(":");
      }
      vector<string> bindset;
      getabindset(rval,bindset);
      rval="";
      for (vector<string>::iterator i=bindset.begin();i!=bindset.end();i++)
	rval+="$."+(*i)+",";
      edgec edge;
      edge.bind=lval;
      string nval;
      bool res=basagree(rval,edge,nval);
      return(res);
    }
    else {
      if (rval.find("eid")==0) {
	int eid=atoi(rval.substr(3).c_str());
	edgec tempedge;
	findedge(eid,tempedge);
	bool res=valagree(lval,tempedge.bind,valtype,opera,valset,nval);
	if (res==true) {
	  nval=rval;
	  return(true);
	}
	else
	  return(false);
      }
      else {
	procshifou(rval);
	if (rval==fou)
	  return(false);
	else //if (rval==shi)
	  return(true);
      }
    }
  }
  else {
    string lv, rv;
    removespace(lval,lv);
    removespace(rval,rv);
    vector<string> lvalset;
    vector<string> rvalset;
    valsepa(lv,lvalset);
    valsepa(rv,rvalset);
    for (vector<string>::iterator i=lvalset.begin();i!=lvalset.end();i++) {
      for (vector<string>::iterator j=rvalset.begin();j!=rvalset.end();j++) {
	string subnval;
	bool res=opagree((*i),(*j),valtype,opera,valset,subnval);
	if (res==true)
	  nval=(subnval)+"|";
      }
    }
    if (nval.empty())
      return(false);
    else {
      nval=nval.substr(0,nval.size()-1);
      return(true);
    }
  }
}

bool opagree(string & lval, string & rval, string & valtype, string & opera, vector< vector<string> > & valset, string & nval)
{
  bool res=eqagree(lval,rval,valtype,valset,nval);
  if (opera=="!=") {
    res=!res;
    nval=lval;
  }
  return(res);
}

bool eqagree(string & lval, string & rval, string & valtype, vector< vector<string> > & valset, string & nval)
{
  vector<string> landset;
  vector<string> randset;
  getandset(lval,landset);
  getandset(rval,randset);
  for (vector<string>::iterator i=landset.begin();i!=landset.end();i++)
    for (vector<string>::iterator j=randset.begin();j!=randset.end();j++) {
      string anval;
      bool res=aeqagree(*i,*j,valtype,valset,anval);
      if (res==false)
	return(false);
      else 
	nval=anval; // think again
    }
  return(true);
}

bool aeqagree(string & lval, string & rval, string & valtype, vector< vector<string> > & valset, string & nval)
{
  if ((lval.find("~")!=0) && (rval.find("~")!=0)) {
    bool res=pureagree(lval,rval,valtype,valset,nval);
    return(res);
  }
  else if ((lval.find("~")!=0) && (rval.find("~")==0)) {
    string temp=rval.substr(1);
    bool res=pureagree(lval,temp,valtype,valset,nval);
    nval=lval; //+rval;
    return(!res);
  }
  else if ((lval.find("~")==0) && (rval.find("~")!=0)) {
    string temp=lval.substr(1);
    bool res=pureagree(temp,rval,valtype,valset,nval);
    nval=rval; //+lval;
    return(!res);
  }
  else {
    if (lval==rval)
      nval=lval;
    else
      nval=lval+rval;
    return(true);
  }
}

bool pureagree(string & lval, string & rval, string & valtype, vector< vector<string> > & valset, string & nval)
{ 
  if (valtype.find("Boolean")!=string::npos) {
    if (valset.empty()) {
      procshifou(lval);
      procshifou(rval);
      if (lval==rval) {
	nval=lval;
	return(true);
      }
      else
	return(false);
    }
    else if (!lval.empty() && !rval.empty()) {
      size_t lv=findval(lval,valset);
      size_t rv=findval(rval,valset);
      if (lv==rv) {
	nval=lval;
	return(true);
      }
      else
	return(false);
    }
    else if ((lval.empty() && !rval.empty()) ||
	     (!lval.empty() && rval.empty())) {
      nval=lval+rval;
      return(true);
    }
    else {
      //nval=?
      return(true);  
    }
  }
  else if (valtype.find("Number")!=string::npos) {
    if (lval==rval) {
      nval=lval;
      return(true);
    }
    else
      return(false);
  }
  else if (valtype.find("Symbol")!=string::npos) {
    if (valset.empty()) {
      if (lval==rval) {
	nval=lval;
	return(true);
      }
      else
	return(false);
    }
    else if (!lval.empty() && !rval.empty()) {
      size_t lv=findval(lval,valset);
      size_t rv=findval(rval,valset);
      if (lv==rv) {
	nval=lval;
	return(true);
      }
      else
	return(false);
    }
    else if ((lval.empty() && !rval.empty()) ||
	     (!lval.empty() && rval.empty())) {
      return(false);
    }
    else {
      return(true);
    }
  }
  else if (valtype.find("Hierar")!=string::npos) {
    string lcode;
    string rcode;
    gethierarcode(lval,lcode);
    gethierarcode(rval,rcode);
    if (lcode.find(rcode)==0) {
      nval=lval;
      return(true);
    }
    else
      return(false);
  }
  else if (valtype.find("Edge")!=string::npos) {
    procshifou(rval);
    if (lval==rval) {
      nval=lval;
      return(true);
    }
    if (!lval.empty() && (rval==shi)) {
      nval=lval;
      return(true);
    }
    if (lval.empty() && (rval==fou)) {
      nval=fou;
      return(true);
    }
    if (lval.empty() && (rval.find("eid")==0)) {
      nval=rval;
      return(true);
    }
    return(false);
  }
  assert(false);
}

void procfeatval(edgec & edge, string & featname, string & featval, string & valtype, vector< vector<string> > & valset)
{
  if (!featname.empty()) {
    if (featname[0]=='%') {
      size_t dotpos=featname.find(".");
      if (dotpos!=string::npos) {
	string nota=featname.substr(0,dotpos);
	edgec sone;
	findsonedge(edge,nota,sone);
	string fname=featname.substr(dotpos+1);
	findfeatval(sone,fname,featval,valtype,valset);
      }
      else {
	string nota=featname;
	int sonid=findsonid(edge,nota);
	ostringstream oss;
	oss << sonid;
	featval="eid"+oss.str();
	valtype="Edge";
      }
    }
    else if (featname[0]=='$') {
      size_t dotpos=featname.find(".");
      if (dotpos!=string::npos) {
	string fname=featname.substr(dotpos+1);
	findfeatval(edge,fname,featval,valtype,valset);
      }
      else {
	featval=edge.bind;
      }
    }
    else if (featname[0]=='[') {
      featval=featname;
      valtype="Edge";
    }
    else
      featval=featname;
  }
}

void findsonedge(edgec & edge, string & nota, edgec & sone)
{
  int sonid=findsonid(edge,nota);
  findedge(sonid,sone);
}

edgec & findsonedge(edgec & edge, string & nota)
{
  int sonid=findsonid(edge,nota);
  return(findedge(sonid));
}

int findsonid(edgec & edge, string & nota)
{
  int repeat=1;
  size_t mpos=1;
  while (nota[mpos]=='%') {
    repeat++;
    mpos++;
  }
  assert(mpos<nota.size());
  string cat=nota.substr(mpos);
  for (vector<string>::iterator i=edge.cfg.part.begin();i!=edge.cfg.part.end();i++) {
    if ((*i)==cat)
      repeat--;
    if (repeat==0) 
      return(edge.son[i-edge.cfg.part.begin()]);
  }
  return(-2);
}

void getnotbind(string & bind, string & notbind)
{
  vector<string> bindset;
  getabindset(bind,bindset);
  for (vector<string>::iterator i=bindset.begin();i!=bindset.end();i++) {
    string opera;
    string fname;
    string fval;
    getopeval(*i,fname,opera,fval);
    if ((opera=="=") || (opera=="==")) {
      if (fval.find("~")!=0)
	notbind+=fname+"=~"+fval+",";
      else 
	notbind+=fname+"="+fval.substr(1)+",";
    }
    else { // opera=="!="
      if (fval.find("~")!=0)
	notbind+=fname+"="+fval+",";
      else 
	notbind+=fname+"="+fval.substr(1)+",";
    }
  }
}

 void gethierarcode(string & name, string & code)
 {
   if (!name.empty() && !isalpha((unsigned char)name[0])) {
     size_t lepos=model.find("="+name+",");
     assert(lepos!=string::npos);
     size_t lbpos=lepos;
     while (!isspace((unsigned char)model[lbpos]))
       lbpos--;
     code=model.substr(lbpos+1,lepos-lbpos-1);
   }
   else
     code=name;
 }

