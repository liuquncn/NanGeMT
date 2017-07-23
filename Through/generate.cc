#include <vector>
#include <string>
#include <assert.h>
#include <iostream>
#include "unity.hh"
#include "transform.hh"
#include "generate.hh"
#include "agree.hh"
#include "enagree.hh"

using namespace std;

int makeid=0;

extern vector<edgec> enchart;
extern vector<int> trnsresult;
extern string makrbase;
extern string bldrbase;
extern vector<string> makrset;
extern vector<string> bldrset;
extern string shi;
extern string fou;

void maktree()
{
  for (vector<int>::iterator i=trnsresult.begin();i!=trnsresult.end();i++) {
    while(1) {
      recurgeneassign(enchart[*i]);
      //geneassignfather();
      if (!maketree(enchart[*i]))
	break;
    }
  }
  cout << "------ after maktree -------" << endl;
  printtrnsresult(trnsresult);
  printtrnssent(trnsresult);
}

bool maketree(edgec & root)
{
  bool changed=false;
  for (vector<int>::iterator i=root.son.begin();i!=root.son.end();i++) {
    changed=maketree(enchart[*i]) || changed;
  }
  changed=makeedge(root) || changed;
  return(changed);
}

bool makeedge(edgec & edge)
{
  bool changed=false;
  for (vector<string>::iterator i=makrset.begin();i!=makrset.end();i++) {
    string prsid;
    string ltree;
    string lbind;
    string rtree;
    string rbind;
    map<string,edgec> lnetable;
    sepmakrule(*i,prsid,ltree,lbind,rtree,rbind);
    bool matched=treematch(ltree,edge,lnetable);
    if (matched) { 
      bool res=makagree(lnetable,lbind,edge);
      if (res) {
	map<string,edgec> rnetable;
	cout << *i << endl;
	bool achanged=makenewtree(lnetable,rtree,edge,rnetable);
	if (achanged)
	  makagree(rnetable,rbind,edge);
	changed = changed || achanged;
      }
    }
  }
  return(changed);
}

bool makenewtree(map<string,edgec> & lnetable, string & cfg, edgec & edge, map<string,edgec> & rnetable)
{
  string red;
  vector<string> sons;
  int head;
  string cat;
  string word;
  septree(cfg,red,sons,head);
  sepnode(red,cat,word);
  addnetable(rnetable,cat,edge);
  vector<int> newsonids;
  for (vector<string>::iterator i=sons.begin();i!=sons.end();i++) {
    edgec sone;
    if (!makenewtree(lnetable,*i,sone,rnetable))
      return(false);
    else
      newsonids.push_back(sone.id);
  }
  makenewatom(lnetable,red,edge);
  if (!newsonids.empty())
    enchart[edge.id].son=newsonids;
  return(true);
}

void makenewatom(map<string,edgec> & lnetable, string & dualred, edgec & edge)
{
  string oldred;
  string newred;
  getoriname(dualred,newred,oldred);
  if (!oldred.empty()) {
    map<string,edgec>::iterator i=lnetable.find(oldred);
    assert(i!=lnetable.end());
    edge=i->second;
  }
  else if (newred.find("<")==string::npos) {
    map<string,edgec>::iterator i=lnetable.find(newred);
    assert(i!=lnetable.end());
    edge=i->second;
  }
  else {
    edge.id=enchart.size();
    edge.source='m';
    edge.father=-2;
    size_t lanpos=newred.find("<");
    size_t ranpos=newred.find(">");
    assert((lanpos!=string::npos) && (ranpos!=string::npos));
    edge.cfg.reduct=newred.substr(0,lanpos);
    edge.cfg.part.push_back(newred.substr(lanpos+1,ranpos-lanpos-1));
    enchart.push_back(edge);
  }
}
 
void getoriname(string & corrname, string & trnname, string & oriname)
{
  size_t bslapos=corrname.find("/");
  if (bslapos==string::npos) {
    trnname=corrname;
    oriname="";
  }
  else { // bslapos!=string::npos
    trnname=corrname.substr(0,bslapos);
    oriname="%";
    size_t mpos=bslapos+1;
    while ((mpos<corrname.length()) && (corrname[mpos]=='/')) {
      oriname+="%";
      mpos++;
    }
    oriname+=corrname.substr(mpos);
  }
}

bool treematch(string & tree, edgec & edge, map<string,edgec> & netable)
{
  string red;
  vector<string> sons;
  int head;
  string cat;
  string word;
  septree(tree,red,sons,head);
  sepnode(red,cat,word);
  if (!nodematch(cat,word,edge) || (sons.size()!=0 && sons.size()!=edge.son.size())) 
    return(false);
  else {
    addnetable(netable,cat,edge);
    for (size_t i=0;i<sons.size();i++) {
      if (!treematch(sons[i],enchart[edge.son[i]],netable))
	return(false);
    }
    return(true);
  }
}

string addnetable(map<string,edgec> & netable, string & cat, edgec & edge)
{
  string phrid=cat;
  if (!netable.empty()) {
    phrid="%";
    for (map<string,edgec>::iterator i=netable.begin();i!=netable.end();i++) {
      if (i->first.find("%")==0) {
	size_t catbpos=i->first.find_first_not_of('%');
	if (cat==i->first.substr(catbpos))
	  phrid+="%";
      }
    }
    phrid+=cat;
  }
  netable.insert(pair<string,edgec>(phrid,edge));
  return(phrid);
}

bool nodematch(string & cat, string & word, edgec & edge)
{
  if (word=="")
    return(cat==edge.cfg.reduct);
  else 
    return(cat==edge.cfg.reduct && word==edge.cfg.part.front());
}

void septree(string & tree, string & root, vector<string> & sons, int & head)
{
  size_t lpapos=tree.find("(");
  if (lpapos!=string::npos) {
    string temp=tree.substr(0,lpapos);
    removespace(temp,root);
    size_t rpapos=tree.rfind(")");
    string sonseq=tree.substr(lpapos+1,rpapos-lpapos-1);
    getsons(sonseq,sons,head);
  }
  else
    root=tree;
}

void sepnode(string & node, string & cat, string & word)
{
  size_t lanpos=node.find("<");
  if (lanpos==string::npos) {
    cat=node;
    word="";
  }
  else {
    cat=node.substr(0,lanpos);
    size_t ranpos=node.find(">");
    word=node.substr(lanpos+1,ranpos-lanpos-1);
  }
}

void getsons(string & sonseq, vector<string> & sons, int & head)
{
  head=-1;
  int n=0;
  string temp=sonseq;
  while (!temp.empty()) {
    string son;
    string rem;
    getoneunit(temp,son,rem);
    if (son.find("!")==0) {
      son=son.substr(1);
      head=n;
    }
    sons.push_back(son);
    temp=rem;
    n++;
  }
}

bool makagree(map<string,edgec> & netable, string & bind, edgec & edge)
{
  assert(bind.find("IF")==string::npos);
  vector<string> bindset;
  getabindset(bind,bindset);
  for (vector<string>::iterator i=bindset.begin();i!=bindset.end();i++) {
    if (!makaagree(netable,*i,edge))
      return(false);
  }
  return(true);
}

bool makaagree(map<string,edgec> & netable, string & bind, edgec & edge)
{
  string lfeat;
  string opera;
  string rfeat;
  getopeval(bind,lfeat,opera,rfeat);
  string lval;
  string lvaltype;
  vector< vector<string> > lvalset;
  makprocfeatval(netable,edge,lfeat,lval,lvaltype,lvalset);
  string rval;
  string rvaltype;
  vector< vector<string> > rvalset;
  makprocfeatval(netable,edge,rfeat,rval,rvaltype,rvalset);
  if (!lvaltype.empty())
    return(envalagree(lval,rval,lvaltype,opera,lvalset));
  else if (!rvaltype.empty())
    return(envalagree(lval,rval,rvaltype,opera,rvalset));
  else
    return(lval==rval);
}

void makprocfeatval(map<string,edgec> & netable, edgec & edge, string & featname, string & featval, string & valtype, vector< vector<string> > & valset)
{
  if (!featname.empty()) {
    if (featname[0]=='%') {
      size_t dotpos=featname.find(".");
      if (dotpos!=string::npos) {
	string nota=featname.substr(0,dotpos);
	map<string,edgec>::iterator i=netable.find(nota);
	string fname=featname.substr(dotpos+1);
	makfindfeatval((i->second),fname,featval,valtype,valset);
      }
      else
	featval=featname;
    }
    else if (featname[0]=='$') {
      size_t dotpos=featname.find(".");
      if (dotpos!=string::npos) {
	string fname=featname.substr(2);
	makfindfeatval(edge,fname,featval,valtype,valset);
      }
      else {
	valtype="Edge";
	if (!edge.bind.empty())
	  featval=edge.bind;
	else
	  featval="[]";
      }
    }
    else
      featval=featname;
  }
}

void sepmakrule(string & makr, string & prsid, string & ltree, string & lbind, string & rtree, string & rbind)
{
  size_t lbrapos=makr.find("{");
  assert(lbrapos!=string::npos);
  size_t idbpos=lbrapos+1;
  while (makr[idbpos]==' ')
    idbpos++;
  size_t rbrapos=makr.find("}",lbrapos);
  assert(rbrapos!=string::npos);
  size_t idepos=rbrapos-1;
  while (makr[idepos]==' ')
    idepos--;
  prsid=makr.substr(idbpos,idepos-idbpos+1);
  size_t tranpos=makr.find("=>");
  assert(tranpos!=string::npos);
  string left=makr.substr(rbrapos+1,tranpos-rbrapos-1);
  string right=makr.substr(tranpos+2);
  septreebind(left,ltree,lbind);
  septreebind(right,rtree,rbind);
}

void septreebind(string & seq, string & tree, string & bind)
{
  size_t perpos=seq.find("%");
  size_t dolpos=seq.find("$=");
  size_t segpos=min(perpos,dolpos);
  if (segpos!=string::npos) {
    tree=seq.substr(0,segpos);
    bind=seq.substr(segpos);
  }
  else {
    tree=seq;
    bind="";
  }
}

void recurprintgenesent(edgec & edge)
{
  if (!edge.son.empty()) {
    for (vector<int>::iterator i=edge.son.begin();i!=edge.son.end();i++) {
      recurprintgenesent(enchart[*i]);
    }
  }
  else {
    string word=edge.cfg.part.front();
    if (!word.empty()) {
      if (word[0]=='\"') {
	size_t rqopos=word.rfind("\"");
	cout << word.substr(1,rqopos-1) << " ";
      }
      else {
	if (edge.cfg.reduct=="A")
	  geneadj(edge);
	else if (edge.cfg.reduct=="D")
	  geneadv(edge);
	else if (edge.cfg.reduct=="N")
	  genenoun(edge);
	else if (edge.cfg.reduct=="V")
	  geneverb(edge);
	else
	  cout << edge.cfg.part.front() << " ";
      }
    }
  }
}

void recurshowgene(edgec & edge, int spanum)
{
  if (spanum>=4) {
    for (int i=0;i<spanum-4;i++)
      cout << " ";
    cout << "+";
    for (int i=0;i<3;i++)
      cout << "-";
  }
  cout << edge.cfg.reduct << " " << edge.id << ":" << edge.father << " ";
  cout << "(" << edge.prsid << "," << edge.swiid << "," << edge.tranid << ")";
  cout << edge.bind;
  cout << endl;
  if (!edge.son.empty()) {
    for (vector<int>::iterator i=edge.son.begin();i!=edge.son.end();i++) {
      edgec sone;
      genefindedge(*i,sone);
      recurshowgene(sone,spanum+4);
    }
  }
  else {
    for (int i=0;i<spanum;i++)
      cout << " ";
    cout << "+";
    for (int i=0;i<3;i++)
      cout << "-";
    cout << edge.cfg.part.front() << endl;
  }
}

void recurprintgene(edgec & edge)
{
  if (!edge.son.empty())
    cout << "(" << edge.prsid << ") ";
  cout << edge.cfg.reduct << " [ ";
  if (!edge.son.empty()) {
    for (vector<int>::iterator i=edge.son.begin();i!=edge.son.end();i++) {
      edgec sone;
      genefindedge(*i,sone);
      recurprintgene(sone);
    }
  }
  else
    cout << edge.cfg.part.front();
  cout << " ] ";
}

void genefindedge(int & eid, edgec & edge)
{
  if (eid>=0 && (size_t)eid<enchart.size())
    edge=enchart[eid];
}

void printnetable(map<string,edgec> & netable)
{
  cout << "-------- print netable ----------" << endl;
  for (map<string,edgec>::iterator i=netable.begin();i!=netable.end();i++) {
    cout << i->first << ":" << i->second.id << endl;
  }
}

bool getcorrbind(string & bind, string & name, string & ebind)
{
  size_t namepos=bind.find(name+"=[");
  if (namepos!=string::npos) {
    if ((namepos==0) || (bind[namepos-1]!='%')) {
      size_t lbrapos=bind.find("[",namepos);
      size_t rbrapos=bind.find("]",namepos);
      string feats=bind.substr(lbrapos,rbrapos-lbrapos+1);
      string resfeat;
      bool res=unify(feats,ebind,resfeat);
      if (res==false)
	return(false);
      else
	ebind=resfeat;
    }
  }
  return(true);
}

bool unify(string & feats1, string & feats2, string & nfeats)
{
  map<string,string> featmap1;
  getfeatmap(feats1,featmap1);
  map<string,string> featmap2;
  getfeatmap(feats2,featmap2);
  map<string,string> smap,bmap;
  if (featmap1.size()<featmap2.size()) {
    smap=featmap1;
    bmap=featmap2;
  }
  else {
    smap=featmap2;
    bmap=featmap1;
  }
  for (map<string,string>::iterator i=smap.begin();i!=smap.end();i++) {
    map<string,string>::iterator j=bmap.find(i->first);
    if (j!=bmap.end()) {
      string valtype;
      string defval;
      vector< vector<string> > valset;
      string fname=i->first;
      checkmodel(fname,valtype,defval,valset);
      string newval;
      string opera="=";
      bool res=valagree(i->second,j->second,valtype,opera,valset,newval);
      if (res==true)
	j->second=newval;
      else
	return(false);
    }
    else
      bmap.insert(pair<string,string>(i->first,i->second));
  }
  maptostr(bmap,nfeats);
  return(true);
}

void getfeatmap(string & feats, map<string,string> & featmap)
{
  size_t lbrapos=feats.find("[");
  size_t rbrapos=feats.rfind("]");
  string temp=feats.substr(lbrapos+1,rbrapos-lbrapos-1);
  vector<string> featset;
  getabindset(temp,featset);
  for (vector<string>::iterator i=featset.begin();i!=featset.end();i++) {
    size_t colpos=i->find(":");
    assert(colpos!=string::npos);
    featmap.insert(pair<string,string>(i->substr(0,colpos),i->substr(colpos+1)));
  }
}

void maptostr(map<string,string> & featmap, string & feats)
{
  feats="[";
  for (map<string,string>::iterator i=featmap.begin();i!=featmap.end();i++) {
    feats+=i->first+":"+i->second+",";
  }
  feats[feats.size()-1]=']';
}

void bldword()
{
  geneassignfather();
  for (size_t i=0;i<enchart.size();i++) {
    if (enchart[i].son.empty())
      bldaword(enchart[i]);
  }
  cout << "------- after bldword -------" << endl;
  printtrnssent(trnsresult);
}

void bldaword(edgec & edge)
{
  for (vector<string>::iterator i=bldrset.begin();i!=bldrset.end();i++) {
    string word;
    string cat;
    string feat;
    string nword;
    sepbldrule(*i,word,cat,feat,nword);
    if (bldmatch(word,cat,feat,edge)) {
      enchart[edge.id].cfg.part.front()=nword;
      break;
    }
  }
}

bool bldmatch(string & word, string & cat, string & feat, edgec & edge)
{
  if ((word==edge.cfg.part.front()) &&
      (cat==edge.cfg.reduct)) {
    string bind;
    feattobind(feat,bind);
    string nbind;
    if (enagree(bind,edge)) {
      edge.bind=nbind;
      return(true);
    }
    else
      return(false);
  }
  else
    return(false);
}

void feattobind(string & feat, string & bind)
{
  bind=feat.substr(1,feat.size()-2);
  vector<string> bindset;
  getabindset(bind,bindset);
  bind="";
  for (vector<string>::iterator i=bindset.begin();i!=bindset.end();i++) {
    size_t colpos=i->find(":");
    i->replace(colpos,1,"=");
    bind+="$."+(*i)+",";
  }
  bind=bind.substr(0,bind.size()-1);
}

void sepbldrule(string & bldrule, string & word, string & cat, string & feat, string & nword)
{
  size_t wbpos=bldrule.find_first_not_of(' ');
  size_t wepos=bldrule.find_first_of(' ',wbpos);
  word=bldrule.substr(wbpos,wepos-wbpos);
  size_t dashpos=bldrule.find("--", wepos);
  size_t spapos=bldrule.find(" ",dashpos);
  size_t cbpos=bldrule.find_first_not_of(' ',spapos);
  size_t cepos=bldrule.find_first_of(' ',cbpos);
  cat=bldrule.substr(cbpos,cepos-cbpos);
  size_t lbrapos=bldrule.find("[",cepos);
  size_t rbrapos=bldrule.find("]",lbrapos);
  feat=bldrule.substr(lbrapos,rbrapos-lbrapos+1);
  size_t arrowpos=bldrule.find(">>",rbrapos);
  spapos=bldrule.find(" ",arrowpos);
  size_t nwbpos=bldrule.find_first_not_of(' ',spapos);
  size_t nwepos=bldrule.find_last_of("abcdefghijklmnopqrstuvwxyz");
  nword=bldrule.substr(nwbpos,nwepos-nwbpos+1);
}

void geneadj(edgec & edge)
{
  string valtype;
  vector< vector<string> > valset;
  string degrname="DEGR";
  string degrval;
  bldbasfeatval(edge,degrname,degrval,valtype,valset);
  string amorfname="AMORF";
  string amorfval;
  bldbasfeatval(edge,amorfname,amorfval,valtype,valset);
  if (degrval=="SUPE") {
    if (amorfval=="IREG") {
      string supeval;
      makbasfeatval(edge,degrval,supeval,valtype,valset);
      cout << supeval << " ";
    }
    else if (amorfval=="SUFF")
      cout << edge.cfg.part.front() << "est ";
    else
      cout << edge.cfg.part.front() << " ";
  }
  else if (degrval=="COMP") {
    if (amorfval=="IREG") {
      string compval;
      makbasfeatval(edge,degrval,compval,valtype,valset);
      cout << compval << " ";
    }
    else if (amorfval=="SUFF")
      cout << edge.cfg.part.front() << "er ";
    else
      cout << edge.cfg.part.front() << " ";
  }
  else
    cout << edge.cfg.part.front() << " ";
}

void geneadv(edgec & edge)
{
  string valtype;
  vector< vector<string> > valset;
  string degrname="DEGR";
  string degrval;
  bldbasfeatval(edge,degrname,degrval,valtype,valset);
  string dmorfname="DMORF";
  string dmorfval;
  bldbasfeatval(edge,dmorfname,dmorfval,valtype,valset);
  if (degrval=="SUPE") {
    if (dmorfval=="IREG") {
      string supeval;
      makbasfeatval(edge,degrval,supeval,valtype,valset);
      cout << supeval << " ";
    }
    else if (dmorfval=="SUFF")
      cout << edge.cfg.part.front() << "est ";
    else
      cout << edge.cfg.part.front() << " ";
  }
  else if (degrval=="COMP") {
    if (dmorfval=="IREG") {
      string compval;
      makbasfeatval(edge,degrval,compval,valtype,valset);
      cout << compval << " ";
    }
    else if (dmorfval=="SUFF")
      cout << edge.cfg.part.front() << "er ";
    else
      cout << edge.cfg.part.front() << " ";
  }
  else
    cout << edge.cfg.part.front() << " ";
}

void genenoun(edgec & edge)
{
  string valtype;
  vector< vector<string> > valset;
  string nnumname="NNUM";
  string nnumval;
  bldbasfeatval(edge,nnumname,nnumval,valtype,valset);
  if (nnumval=="PLUR") {
    string nmorfname="NMORF";
    string nmorfval;
    bldbasfeatval(edge,nmorfname,nmorfval,valtype,valset);
    if (nmorfval=="IREG") {
      string plurval;
      makbasfeatval(edge,nnumval,plurval,valtype,valset);
      cout << plurval << " ";
    }
    else
      printnouns(edge.cfg.part.front());
  }
  else
    cout << edge.cfg.part.front() << " ";
}

void geneverb(edgec & edge)
{
  if (!verbbe(edge.cfg.part.front())) {
    string valtype;
    vector< vector<string> > valset;
    string vmorfname="VMORF";
    string vmorfval;
    bldbasfeatval(edge,vmorfname,vmorfval,valtype,valset);
    string formname="FORM";
    string formval;
    bldbasfeatval(edge,formname,formval,valtype,valset);
    if (formval=="VG")
      printvg(edge.cfg.part.front());
    else if (formval=="VN") {
      if (vmorfval=="IREG") {
	string vnname="VN";
	string vnval;
	bldbasfeatval(edge,vnname,vnval,valtype,valset);
	cout << vnval << " ";
      }
      else
	cout << edge.cfg.part.front() << "ed ";
    }
    else if (formval=="RT")
      cout << edge.cfg.part.front() << " ";
    else {
      string voicename="VOICE";
      string voiceval;
      bldbasfeatval(edge,voicename,voiceval,valtype,valset);
      if (voiceval.empty())
	voiceval="ACTI";
    
      string futrname="FUTR";
      string futrval;
      bldbasfeatval(edge,futrname,futrval,valtype,valset);
      procshifou(futrval);
      if (futrval.empty())
	futrval=fou;
    
      string pastname="PAST";
      string pastval;
      bldbasfeatval(edge,pastname,pastval,valtype,valset);
      procshifou(pastval);
      if (pastval.empty())
	pastval=fou;
    
      string npername="NPER";
      string nperval;
      bldbasfeatval(edge,npername,nperval,valtype,valset);
      if (nperval.empty())
	nperval="3";
    
      string nnumname="NNUM";
      string nnumval;
      bldbasfeatval(edge,nnumname,nnumval,valtype,valset);
      if (nnumval.empty())
	nnumval="SNGL";
      if (voiceval=="ACTI" && futrval==shi && pastval==shi ) {
	cout << "would " << edge.cfg.part.front() << " ";
      }
      else if (voiceval=="PASS" && futrval==shi && pastval==shi) {
	cout << "would be ";
	if (vmorfval=="IREG") {
	  string vnname="VN";
	  string vnval;
	  bldbasfeatval(edge,vnname,vnval,valtype,valset);
	  cout << vnval << " ";
	}
	else 
	  printvnvd(edge.cfg.part.front());
      }
      else if (voiceval=="ACTI" && futrval==shi && pastval==fou) {
	cout << "will " << edge.cfg.part.front() << " ";
      }
      else if (voiceval=="PASS" && futrval==shi && pastval==fou) {
	cout << "will be ";
	if (vmorfval=="IREG") {
	  string vnname="VN";
	  string vnval;
	  bldbasfeatval(edge,vnname,vnval,valtype,valset);
	  cout << vnval << " ";
	}
	else 
	  printvnvd(edge.cfg.part.front());
      }
      else if (voiceval=="ACTI" && futrval==fou && pastval==shi) {
	if (vmorfval=="IREG") {
	  string vdname="VD";
	  string vdval;
	  bldbasfeatval(edge,vdname,vdval,valtype,valset);
	  cout << vdval << " ";
	}
	else
	  printvnvd(edge.cfg.part.front());
      }
      else if (voiceval=="PASS" && futrval==fou && pastval==shi) {
	if ((nperval=="1" && nnumval=="SNGL") || (nperval=="3" && nnumval=="SNGL"))
	  cout << "was ";
	else
	  cout << "were ";
	if (vmorfval=="IREG") {
	  string vnname="VN";
	  string vnval;
	  bldbasfeatval(edge,vnname,vnval,valtype,valset);
	  cout << vnval << " ";
	}
	else 
	  printvnvd(edge.cfg.part.front());
      }
      else if (voiceval=="ACTI" && futrval==fou && pastval==fou) {
	if (nperval=="3" && nnumval=="SNGL") 
	  printunvs(edge.cfg.part.front());
	else
	  cout << edge.cfg.part.front() << " ";
      }
      else if (voiceval=="PASS" && futrval==fou && pastval==fou) {
	if ((nperval=="1" && nnumval=="SNGL") || (nperval=="3" && nnumval=="SNGL")) 
	  cout << "is ";
	if (vmorfval=="IREG") {
	  string vdname="VD";
	  string vdval;
	  bldbasfeatval(edge,vdname,vdval,valtype,valset);
	  cout << vdval << " ";
	}
	else 
	  printvnvd(edge.cfg.part.front());
      }
      else {
	cout << voiceval << " " << futrval << " " << pastval << endl;
	assert(false);
      }
    }
    string comppname="COMPPREP";
    string comppval;
    makbasfeatval(edge,comppname,comppval,valtype,valset);
    if (!comppval.empty())
      cout << comppval << " ";
  }
  else
    cout << edge.cfg.part.front() << " ";
}

bool verbbe(string & word)
{
  if (word=="be" || word=="am" || word=="is" || word=="are" || word=="was" || word=="were")
    return(true);
  else
    return(false);
}

void printvnvd(string & word)
{
  if (!word.empty()) {
    if (word[word.size()-1]=='e')
      cout << word << "d ";
    else
      cout << word << "ed ";
  }
}

void printunvs(string & word)
{
  if (!word.empty()) {
    if (word[word.size()-1]=='y')
      cout << word.substr(0,word.size()-1) << "ies ";
    else if (word[word.size()-1]=='s' || word[word.size()-1]=='x' || word[word.size()-1]=='o')
      cout << word << "es ";
    else if (word.size()>2 && (word.substr(word.size()-2)=="ch" || word.substr(word.size()-2)=="sh"))
      cout << word << "es ";
    else
      cout << word << "s ";
  }
}

void printvg(string & word)
{
  if (!word.empty()) {
    if (word.size()>2 && (word.substr(word.size()-2)=="un" || word.substr(word.size()-2)=="im" || 
			  word.substr(word.size()-2)=="et" || word.substr(word.size()-2)=="it" ||
			  word.substr(word.size()-2)=="ut" || word.substr(word.size()-2)=="in" ||
			  word.substr(word.size()-2)=="op" ))
      cout << word << word[word.size()-1] << "ing ";
    else if (word.size()>2 && word.substr(word.size()-2)=="ee")
      cout << word << "ing ";
    else if (word[word.size()-1]=='e')
      cout << word.substr(0,word.size()-1) << "ing ";
    else
      cout << word << "ing ";
  }
}

void printnouns(string & word)
{
  if (!word.empty()) {
    if (word[word.size()-1]=='y')
      cout << word.substr(0,word.size()-1) << "ies ";
    else if (word[word.size()-1]=='s' || word[word.size()-1]=='x')
      cout << word << "es ";
    else if (word.size()>2 && (word.substr(word.size()-2)=="sh" || word.substr(word.size()-2)=="ch"))
      cout << word << "es ";
    else
      cout << word << "s ";
  }
}

void printresult()
{
  cout << "------- final result ------" << endl;
  for (vector<int>::iterator i=trnsresult.begin();i!=trnsresult.end();i++)
    recurshowgene(enchart[*i],0);
  for (vector<int>::iterator i=trnsresult.begin();i!=trnsresult.end();i++)
    recurprintgenesent(enchart[*i]);
  cout << endl;
}

void geneassignfather()
{
  for (vector<int>::iterator i=trnsresult.begin();i!=trnsresult.end();i++) 
    recurgeneassign(enchart[*i]);
}

void recurgeneassign(edgec & edge)
{
  for (vector<int>::iterator i=edge.son.begin();i!=edge.son.end();i++) {
    enchart[*i].father=edge.id;
    recurgeneassign(enchart[*i]);
  }
}
