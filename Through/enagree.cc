#include <assert.h>
#include <stdlib.h>
#include "enagree.hh"
#include "unity.hh"
#include "transform.hh"
#include "generate.hh"
#include "agree.hh"

using namespace std;

extern string fou;
extern string model;
extern string yingyucilei;
extern string hanyucilei;
extern map<int,int> entochidx;
extern vector<edgec> enchart;
extern vector<edgec> makchart;

bool enagree(string & bind, edgec & edge)
{
  vector<string> bindset;
  getabindset(bind,bindset);
  for (vector<string>::iterator i=bindset.begin();i!=bindset.end();i++) {
    bool res=enaagree((*i),edge);
    if (res==false)
      return(false);
  }
  return(true);
}

bool enaagree(string & bind, edgec & edge)
{
  if (bind.find("@")!=string::npos) {
    return(true);
  }
  else {
    string opera;
    string lfeat;
    string rfeat;
    getopeval(bind,lfeat,opera,rfeat);
    string lval;
    string lvaltype;
    vector< vector<string> > lvalset;
    enprocfeatval(edge,lfeat,lval,lvaltype,lvalset);
    string rval;
    string rvaltype;
    vector< vector<string> > rvalset;
    enprocfeatval(edge,rfeat,rval,rvaltype,rvalset);
    if (!lvaltype.empty())
      return(envalagree(lval,rval,lvaltype,opera,lvalset));
    else if (!rvaltype.empty())
      return(envalagree(lval,rval,rvaltype,opera,rvalset));
    else
      return(lval==rval);
  }
}

void enprocfeatval(edgec & edge, string & featname, string & featval, string & valtype, vector< vector<string> > & valset)
{
  if (featname.find("$")==0) {
    assert(featname[1]=='.');
    string fname=featname.substr(2);
    bldbasfeatval(edge,fname,featval,valtype,valset);
  }
  else
    featval=featname;
}

void bldbasfeatval(edgec & edge, string & fname, string & featval, string & valtype, vector< vector<string> > & valset)
{
  string defval;
  checkmodel(fname,valtype,defval,valset);
  edgec curedge=edge;
  bool found=getfeatval(curedge.bind,fname,featval);
  edgec fedge;
  while (!found && (curedge.father!=-1)) {
    genefindedge(curedge.father,fedge);
    if ((fedge.head>=0) && (fedge.head<fedge.son.size()) && (fedge.son[fedge.head]==curedge.id)) {
      found=getfeatval(fedge.bind,fname,featval);
      curedge=fedge;
    }
    else
      break;
  }
  if (!found)
    featval=defval;
}

bool envalagree(string & lval, string & rval, string & valtype, string & opera, vector< vector<string> > & valset)
{
  string lv, rv;
  removespace(lval,lv);
  removespace(rval,rv);
  if ((lv.find("%")!=string::npos) &&
      (rv.find("%")!=string::npos))
    return(lv==rv);
  else if (lv.find("%")!=string::npos) {
    if (rv.empty())
      return(true);
    else
      return(false);
  }
  else if (rv.find("%")!=string::npos) {
    if (lv.empty())
      return(true);
    else
      return(false);
  }
  else if (lval.find("[")==0) {
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
      procshifou(rval);
      if (rval==fou)
	return(false);
      else // rval==shi
	return(true);
    }
  }
  else {
    vector<string> lvalset;
    vector<string> rvalset;
    valsepa(lv,lvalset);
    valsepa(rv,rvalset);
    for (vector<string>::iterator i=lvalset.begin();i!=lvalset.end();i++) {
      for (vector<string>::iterator j=rvalset.begin();j!=rvalset.end();j++) {
	string nval;
	bool res=opagree((*i),(*j),valtype,opera,valset,nval);
	if (res==true)
	  return(true);
      }
    }
    return(false);
  }
}

bool trnagree(string & bind, edgec & edge, string & nbind)
{
  vector<string> bindset;
  getbindset(bind,bindset);
  return(proctrnagreeset(bindset,edge,nbind));
}

bool proctrnagreeset(vector<string> & bindset, edgec & edge, string & nbind)
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
      bool res=trnbasagree(temp,edge,anbind);
      if (res==false)
	return(false);
      else
	nbind+=anbind;
    }
    else if (truepos!=string::npos) {
      // if <bind> TRUE
      string temp=i->substr(ifpos+3,truepos-ifpos-3);
      bool res=trnbasagree(temp,edge,anbind);
      if (res==false)
	return(false);
      else
	nbind+=anbind;
    }
    else if (falsepos!=string::npos) {
      // if <bind> FALSE
      string temp=i->substr(ifpos+3,falsepos-ifpos-3);
      bool res=trnbasagree(temp,edge,anbind);
      if (res==true)
	return(false);
      else {
	string notbind;
	getnotbind(anbind,notbind);
	nbind+=notbind;
      }
    }
    else if ((thenpos!=string::npos) && (elsepos==string::npos)) {
      // if <bind> then <bind> endif
      string iftemp=i->substr(ifpos+3,thenpos-ifpos-3);
      string thentemp=i->substr(thenpos+5,endifpos-thenpos-5);
      string nifbind;
      string nthenbind;
      bool ifres=trnbasagree(iftemp,edge,nifbind);
      bool thenres=trnbasagree(thentemp,edge,nthenbind);
      if ((ifres==true) && (thenres==false))
	return(false);
      else if ((ifres==true) && (thenres==true))
	nbind+=nifbind+nthenbind;
      else if (ifres==false) {
	string notbind;
	getnotbind(nifbind,notbind);
	nbind+=notbind;
      }
    }
    else if ((thenpos==string::npos) && (elsepos!=string::npos)) {
      // if <bind> else <bind> endif
      string iftemp=i->substr(ifpos+3,elsepos-ifpos-3);
      string elsetemp=i->substr(elsepos+5,endifpos-elsepos-5);
      string nifbind;
      string nelsebind;
      bool ifres=trnbasagree(iftemp,edge,nifbind);
      bool elseres=trnbasagree(elsetemp,edge,nelsebind);
      if ((ifres==false) && (elseres==false))
	return(false);
      else if ((ifres==false) && (elseres==true)) {
	string notbind;
	getnotbind(nifbind,notbind);
	nbind+=notbind+nelsebind;
      }
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
      bool ifres=trnbasagree(iftemp,edge,nifbind);
      bool thenres=trnbasagree(thentemp,edge,nthenbind);
      bool elseres=trnbasagree(elsetemp,edge,nelsebind);
      if (((ifres==true) && (thenres==false)) || ((ifres==false) && (elseres==false)))
	return(false);
      else if ((ifres==true) && (thenres==true))
	nbind+=nifbind+nthenbind;
      else if ((ifres==false) && (elseres==true)) {
	string notbind;
	getnotbind(nifbind,notbind);
	nbind+=notbind+nelsebind;
      }
    }
  }
  return(true);
}

bool trnbasagree(string & bind, edgec & edge, string & nbind)
{
  vector<string> bindset;
  getabindset(bind,bindset);
  for (vector<string>::iterator i=bindset.begin();i!=bindset.end();i++) {
    string anbind;
    bool res=trnaagree((*i),edge,anbind);
    if (res==false)
      return(false);
    else
      nbind += anbind;
  }
  return(true);
}

bool trnaagree(string & bind, edgec & edge, string & nbind)
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
      bool res=trnaagree(atbind,edge,natbind);
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
    trnprocfeatval(edge,rfeat,rval,rvaltype,rvalset);
    if ((lfeat.find("$")==0) && edge.bind.empty()) {
      nbind=lfeat+opera+rval+",";
      return(true);
    }
    else {
      string lval;
      string lvaltype;
      vector< vector<string> > lvalset;
      trnprocfeatval(edge,lfeat,lval,lvaltype,lvalset);

      if (lval.empty() || rval.empty()) {
	if (lval.empty() && rval.empty())
	  nbind=bind+",";
	else if (lval.empty())
	  nbind=lfeat+opera+rval+",";
	else // rval.empty()
	  nbind=rfeat+opera+lval+",";
	return(true);
      }

      string newval;
      if (!lvaltype.empty()) {
	bool res=valagree(lval,rval,lvaltype,opera,lvalset,newval);
	if (res==false)
	  return(false);
	else {
	  if (lfeat.find(".")!=string::npos)
	    nbind=lfeat+opera+newval+",";
	  return(true);
	}
      } 
      else if (!rvaltype.empty()) {
	bool res=valagree(lval,rval,rvaltype,opera,rvalset,newval);
	if (res==false)
	  return(false);
	else {
	  if (rfeat.find(".")!=string::npos)
	    nbind=rfeat+opera+newval+",";
	  return(true);
	}
      }
      else 
	assert(false);
    }
  }
}

void trnprocfeatval(edgec & edge, string & featname, string & featval, string & valtype, vector< vector<string> > & valset)
{
  if (!featname.empty()) {
    if (featname[0]=='%') {
      size_t dotpos=featname.find(".");
      if (dotpos!=string::npos) {
	string nota=featname.substr(0,dotpos);
	edgec sone;
	trnfindsonedge(edge,nota,sone);
	string fname=featname.substr(dotpos+1);
	trnfindfeatval(sone,fname,featval,valtype,valset);
      }
      else {
	string nota=featname;
	int sonid=trnfindsonid(edge,nota);
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
	trnfindfeatval(edge,fname,featval,valtype,valset);
      }
      else {
	featval=edge.bind;
	valtype="Edge";
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

void trnfindsonedge(edgec & edge, string & nota, edgec & sone)
{
  int sonid=trnfindsonid(edge,nota);
  trnfindedge(sonid,sone);
}

void trnfindedge(int eeid, edgec & edge)
{
  if (eeid>=0 && eeid<(int)enchart.size()) {
    edge=enchart[eeid];
  }
}

void trnfindfeatval(edgec & ce, string & featname, string & featval, string & valtype, vector< vector<string> > & valset)
{
  string temp="."+featname;
  string septor=".";
  vector<string> nameset;
  getset(temp,septor,nameset);
  edgec tempedge=ce;
  for (vector<string>::iterator i=nameset.begin();i!=nameset.end();i++) {
    featval="";
    trnbasfeatval(tempedge,(*i),featval,valtype,valset);
    if (valtype=="Edge") {
      procshifou(featval);
      if (featval.find("[")==0) {
	edgec nedge;
	nedge.bind=featval;
	tempedge=nedge;
      }
      else if (featval.empty() || (featval==fou)) {
	edgec empedge;
	tempedge=empedge;
      }
      else {
	if (featval.find("~")==0) { 
	  edgec empedge;
	  tempedge=empedge;
	}
	else if (featval.find("eid")==0) {
	  int eid=atoi(featval.substr(3).c_str());
	  trnfindedge(eid,tempedge);
	}
      }
    }
  }
}

bool trnbasfeatval(edgec & ce, string & featname, string & featval, string & valtype, vector< vector<string> > & valset)
{
  if (featname.find("eid")!=string::npos) {
    valtype="Edge";
    featval=featname;
  }
  else if (featname=="YX") {
    valtype="Symbol";
    enrecurprocyx(ce,featval);
  }
  else if (featname=="ECAT") {
    valtype="Symbol";
    findecat(ce,featval);
  }
  else {
    string defval;
    checkmodel(featname,valtype,defval,valset);
    edgec tempe=ce;
    bool found=getfeatval(tempe.bind,featname,featval);
    while (!found) {
      if ((tempe.head>=0) && (tempe.head<tempe.son.size())) {
	edgec hsone;
	trnfindedge(tempe.son[tempe.head],hsone);
	found=getfeatval(hsone.bind,featname,featval);
	tempe=hsone;
      }
      else
	break;
    }
  }
  return(true);
}

void enrecurprocyx(edgec & edge, string & yxval)
{
  if (edge.son.empty())
    yxval=edge.cfg.part.front();
  else {
    for (vector<int>::iterator i=edge.son.begin();i!=edge.son.end();i++) {
      edgec sone;
      trnfindedge(*i,sone);
      string sonyx;
      enrecurprocyx(sone,sonyx);
      yxval+=sonyx;
    }
  }
}

void findecat(edgec & edge, string & ecat)
{
  edgec headedge;
  if (!edge.cfg.reduct.empty()) {
    edgec curedge=edge;
    string syncat=edge.cfg.reduct;
    while (!isenpos(syncat)) {
      if (curedge.head<curedge.son.size()) {
	int headid=curedge.son[curedge.head];
	trnfindedge(headid,headedge);
	syncat=headedge.cfg.reduct;
	curedge=headedge;
      }
      else
	break;
    }
    if (isenpos(syncat))
      ecat=syncat;
    else
      ecat="";
  }
  else
    ecat="";
}

bool isenpos(string & syncat)
{
  size_t ecatpos=model.find(yingyucilei);
  assert(ecatpos!=string::npos);
  size_t lbrktpos=model.find("{",ecatpos);
  size_t rbrktpos=model.find("}",lbrktpos);
  if (model.substr(lbrktpos,rbrktpos-lbrktpos).find(syncat)!=string::npos)
    return(true);
  else
    return(false);
}

int trnfindsonid(edgec & edge, string & nota)
{
  int repeat=1;
  size_t mpos=1;
  while (nota[mpos]=='%') {
    repeat++;
    mpos++;
  }
  assert(mpos<nota.size());
  string cat=nota.substr(mpos);
  return(recurpostfinddescid(edge,repeat,cat));
}

int recurpostfinddescid(edgec & root, int repeat, string & cat)
{
  for (vector<int>::iterator i=root.son.begin();i!=root.son.end();i++) {
    edgec sone=enchart[*i];
    if (sone.cfg.reduct==cat) {
      repeat--;
      if (repeat==0) 
	return(sone.id);
    }
    map<int,int>::iterator j=entochidx.find(sone.id);
    if (j->second==-1) {
      int res=recurpostfinddescid(sone,repeat,cat);
      if (res!=-2)
	return(res);
    }
  }
  return(-2);
}

void makfindfeatval(edgec & ce, string & featname, string & featval, string & valtype, vector< vector<string> > & valset)
{
  string temp="."+featname;
  string septor=".";
  vector<string> nameset;
  getset(temp,septor,nameset);
  edgec tempedge=ce;
  for (vector<string>::iterator i=nameset.begin();i!=nameset.end();i++) {
    featval="";
    makbasfeatval(tempedge,(*i),featval,valtype,valset);
    if (valtype=="Edge") {
      procshifou(featval);
      if (featval.find("[")==0) {
	edgec nedge;
	nedge.bind=featval;
	tempedge=nedge;
      }
      else if (featval.empty() || (featval==fou)) {
	edgec empedge;
	tempedge=empedge;
      }
      else {
	if (featval.find("~")==0) { 
	  edgec empedge;
	  tempedge=empedge;
	}
	else if (featval.find("eid")==0) {
	  int eid=atoi(featval.substr(3).c_str());
	  tempedge=enchart[eid];
	}
      }
    }
  }
}

bool makbasfeatval(edgec & ce, string & featname, string & featval, string & valtype, vector< vector<string> > & valset)
{
  if (featname.find("eid")!=string::npos) {
    valtype="Edge";
    featval=featname;
  }
  else if (featname=="YX") {
    valtype="Symbol";
    enrecurprocyx(ce,featval);
  }
  else if (featname=="ECAT") {
    valtype="Symbol";
    findecat(ce,featval);
  }
  else {
    string defval;
    checkmodel(featname,valtype,defval,valset);
    edgec tempe=ce;
    bool found=getfeatval(tempe.bind,featname,featval);
    while (!found) {
      if ((tempe.head>=0) && (tempe.head<tempe.son.size())) {
	edgec & hsone=enchart[tempe.son[tempe.head]];
	found=getfeatval(hsone.bind,featname,featval);
	tempe=hsone;
      }
      else
	break;
    }
  }
  return(true);
}

void enmodifeatval(edgec & ce, string & featname, string & nval)
{
  string temp="."+featname;
  string septor=".";
  vector<string> nameset;
  getset(temp,septor,nameset);
  string nameval;
  string featval;
  string valtype;
  vector< vector<string> > valset;
  int eid=ce.id;
  for (vector<string>::iterator i=nameset.begin();i!=nameset.end();i++) {
    featval="";
    trnbasfeatval(enchart[eid],(*i),featval,valtype,valset);
    nameval=(*i)+":"+featval;
    if (valtype=="Edge") {
      if (!featval.empty()) {
	eid=atoi(featval.substr(3).c_str());
      }
      else { 
	return;
      }
    }
  }
  if (featval!=nval) {
    size_t nvpos=enchart[eid].bind.find(nameval);
    if (nvpos!=string::npos) {
      size_t valpos=enchart[eid].bind.find(featval,nvpos);
      enchart[eid].bind.replace(valpos,featval.length(),nval);
    }
    else {
      size_t colpos=nameval.find(":");
      assert(colpos!=string::npos);
      if (enchart[eid].bind.empty())
	enchart[eid].bind="["+nameval.substr(0,colpos+1)+nval+"]";
      else
	enchart[eid].bind.replace(1,0,nameval.substr(0,colpos+1)+nval+",");
    }
  }
}

