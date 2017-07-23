#include <assert.h>
#include <iostream>
#include <boost/regex.hpp>
#include "parse.hh"
#include "unity.hh"
#include "agree.hh"
#include "transform.hh"
#include "generate.hh"
#include "enagree.hh"

using namespace std;

extern vector<edgec> chart;
extern vector<int> parseresult;
vector<edgec> enchart;
map< int,vector<int> > chtoenidx;
map<int,int> entochidx;
vector<int> trnsresult;

extern string locdictn;
extern string dictn;
extern string prsrbase;
extern string zhongxinyu;
extern string chnsent;
extern vector<string> chnwords;
extern boost::regex numreg;
extern boost::regex idreg;

void transform()
{
  for (vector<int>::iterator i=parseresult.begin();i!=parseresult.end();i++) {
    transformtree(chart[*i]);
    addtrnsresult(chart[*i],trnsresult);
  }
  printtrnsresult(trnsresult);
  printtrnssent(trnsresult);
}

void addtrnsresult(edgec & root, vector<int> & trnsresult)
{
  map<int, vector<int> >::iterator i=chtoenidx.find(root.id);
  if (i!=chtoenidx.end() && !(i->second.empty())) {
    int minid=choosebesttree(i->second);
    trnsresult.push_back(minid);
  }
  else {
    for (vector<int>::iterator j=root.son.begin();j!=root.son.end();j++)
      addtrnsresult(chart[*j],trnsresult);
  }
}

bool transformtree(edgec & root)
{
  bool succ=true;
  if (!root.son.empty()) {
    transition(root);
    for (vector<int>::iterator i=root.son.begin();i!=root.son.end();i++) {
      succ=transformtree(chart[*i]) && succ;
    }
  }
  printedge(root);
  vector<edgec> enedges;
  if (succ)
    transformedge(root,enedges);
  if (enedges.empty()) {
    cout << "--- transform fail ---" << endl;
    return(false);
  }
  else {
    printtranchart(enedges);
    return(true);
  }
}

void transition(edgec & edge)
{
  assert(!edge.bind.empty());
  string feat=edge.bind.substr(1,edge.bind.size()-2);
  vector<string> featset;
  getabindset(feat,featset);
  feat="";
  for (vector<string>::iterator i=featset.begin();i!=featset.end();i++) {
    if (i->find("%")!=0)
      feat+=(*i)+",";
    else {
      string lfeat;
      string conn;
      string rfeat;
      size_t colpos=i->find(":");
      assert(colpos!=string::npos);
      lfeat=i->substr(0,colpos);
      rfeat=i->substr(colpos+1);
      size_t dotpos=lfeat.find(".");
      string nota=lfeat.substr(0,dotpos);
      int sonid=findsonid(edge,nota);
      edgec & sone=chart[sonid]; //genefindedge(sonid,edges);
      string featname=lfeat.substr(dotpos+1);
      modifeatval(sone,featname,rfeat);
    }
  }
  edge.bind="["+feat.substr(0,feat.size()-1)+"]";
}

void modifeatval(edgec & ce, string & featname, string & nval)
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
    basfeatval(chart[eid],(*i),featval,valtype,valset);
    nameval=(*i)+":"+featval;
    if (featval.find("eid")==0) {
      eid=atoi(featval.substr(3).c_str());
    }
  }
  if (featval!=nval) {
    modifyone(chart[eid],nameval,featval,nval);
  }
}

void modifyone(edgec & edge, string & nameval, string & featval, string & nval)
{
  size_t nvpos=edge.bind.find(nameval);
  if (nvpos!=string::npos) {
    size_t valpos=edge.bind.find(featval,nvpos);
    edge.bind.replace(valpos,featval.length(),nval);
  }
  else {
    size_t colpos=nameval.find(":");
    assert(colpos!=string::npos);
    if (edge.bind.empty())
      edge.bind="["+nameval.substr(0,colpos+1)+nval+"]";
    else
      edge.bind.replace(1,0,nameval.substr(0,colpos+1)+nval+",");
  }
}

void transformedge(edgec & edge, vector<edgec> & enedges)
{
  if (edge.son.empty()) {
    if (edge.prsid=="unk") {
      string entran;
      if (edge.cfg.reduct=="m") 
	entran="M<"+edge.cfg.part.front()+">";
      else
	entran="N<"+edge.cfg.part.front()+">";
      edgec aedge;
      createatomedge(entran,aedge);
      aedge.sentreg=edge.sentreg;
      enedges.push_back(aedge);
      entochidx.insert(pair<int,int>(aedge.id,edge.id));
    }
    else {
      string entry;
      getentry(edge.cfg.part.front(),entry);
      string catentry;
      getcatentry(edge.prsid,entry,catentry);
      string tranentry;
      gettranentry(edge,catentry,tranentry);
      vector<string> transet;
      string septor="=>";
      getset(tranentry,septor,transet);
      prsrtranedge(edge,transet,enedges);
    }
  }
  else {
    string rule;
    getrule(edge.source,edge.prsid,rule);
    string tranentry;
    gettranentry(edge,rule,tranentry);
    vector<string> transet;
    string septor="=>";
    getset(tranentry,septor,transet);
    prsrtranedge(edge,transet,enedges);
  }
  
  if (!enedges.empty()) {
    vector<int> edgeids;
    for (vector<edgec>::iterator i=enedges.begin();i!=enedges.end();i++)
      edgeids.push_back(i->id);
    chtoenidx.insert(pair<int,vector<int> >(edge.id,edgeids));
  }
}

void prsrtranedge(edgec & edge, vector<string> & transet, vector<edgec> & enedges)
{
  for (vector<string>::iterator i=transet.begin();i!=transet.end();i++) {
    string tranid;
    string trancfg;
    string tranbind;
    sepatranentry(*i,tranid,trancfg,tranbind);
    vector<edgec> subenegs;
    prsronetran(edge,trancfg,tranbind,subenegs);

    for (vector<edgec>::iterator k=subenegs.begin();k!=subenegs.end();k++) {
      k->sentreg=edge.sentreg;
      k->tranid=tranid;
      entochidx.insert(pair<int,int>(k->id,edge.id));
    }

    enedges.insert(enedges.end(),subenegs.begin(),subenegs.end());
  }
}

void flattranedge(edgec & edge, string & trancfg, string & tranbind, vector<edgec> & locchart, vector<edgec> & enedges)
{
  string enred;
  vector<string> itemset;
  getitemset(trancfg,enred,itemset);
  if (!itemset.empty()) {
    edgec enedge;
    getenedge(edge,enred,itemset,tranbind,enedge);
    vector<edgec> enegs;
    parseonerule(enedge,locchart,enedges);
  }
  else {
    edgec aedge;
    aedge.prsid=edge.prsid;
    aedge.bind=tranbind;
    string feat;
    entransition(aedge,feat);
    aedge.bind=feat;
    createatomedge(trancfg,aedge);
    enedges.push_back(aedge);
  }
}

void parseonerule(edgec & iniedge, vector<edgec> & locchart, vector<edgec> & edges)
{
  queue<edgec> agenda;
  agenda.push(iniedge);
  while (!agenda.empty()) {
    edgec edge=agenda.front();
    agenda.pop();
    if (complete(edge)) {
      string bind=edge.bind;
      edge.bind="";
      string nbind;
      if (trnagree(bind,edge,nbind)) {
	edge.bind=nbind;
	string feat;
	entransition(edge,feat);
	edge.id=enchart.size();
	edge.bind=feat;
	edges.push_back(edge);
	enchart.push_back(edge);
      }
    }
    else {
      for (vector<edgec>::iterator i=locchart.begin();i!=locchart.end();i++) {
	if (edge.sentreg.end==i->sentreg.begin) {
	  edgec newedge=edge;
	  newedge.father=-2;
	  newedge.dotreg.end++;
	  newedge.sentreg.end=i->sentreg.end;
	  newedge.son.push_back(i->id);
	  agenda.push(newedge);
	}
      }
    }
  }
}

void getentry(string & word, string & entry)
{
  string intro="$$ "+word;
  size_t spos=dictn.find(intro);
  assert(spos!=string::npos);
  size_t epos=dictn.find("$$",spos+intro.size());
  entry=dictn.substr(spos,epos-spos);
}

void getcatentry(string & catid, string & entry, string & catentry)
{
  string intro="**{"+catid+"}";
  size_t bpos=entry.find(intro);
  assert(bpos!=string::npos);
  size_t rpos=entry.find("&&",bpos+intro.size());
  size_t nipos=entry.find("**",bpos+intro.size());
  size_t epos=min(rpos,nipos);
  catentry=entry.substr(bpos+intro.size(),epos-bpos-intro.size());
}

void gettranentry(edgec & edge, string & catentry, string & tranentry)
{
  vector<string> swiset;
  getswiset(catentry,swiset);
  for (vector<string>::iterator i=swiset.begin();i!=swiset.end();i++) {
    string temp=*i;
    string swiid;
    removeruleid(temp,swiid);
    size_t tranpos=temp.find("=>");
    assert(tranpos!=string::npos);
    string tempswibind=temp.substr(0,tranpos);
    string swibind;
    advrmspace(tempswibind,swibind);
    string nbind;
    if (agree(swibind,edge,nbind)) {
      edge.swiid=swiid;
      tranentry=temp.substr(tranpos);
      break;
    }
  }
}

void getrule(char & source, string & prsid, string & rule)
{
  if (source=='p') {
    size_t bpos=prsrbase.find("{"+prsid+"}");
    assert(bpos!=string::npos);
    size_t epos=prsrbase.find("&&",bpos);
    rule=prsrbase.substr(bpos,epos-bpos);
  }
  else if (source=='l') {
    size_t bpos=dictn.find("{"+prsid+"}");
    size_t nrpos=dictn.find("&&",bpos);
    size_t ncatpos=dictn.find("**",bpos);
    size_t nenpos=dictn.find("$$",bpos);
    size_t epos=min(nrpos,min(ncatpos,nenpos));
    rule=dictn.substr(bpos,epos-bpos);
  }
  else
    assert(false);
}

void separate(string & switran, string & swi, string & tran)
{
  size_t tranpos=switran.find("=>");
  swi=switran.substr(0,tranpos);
  tran=switran.substr(tranpos+2);
}

void sepatranentry(string & tranentry, string & tranid, string & trancfg, string & tranbind)
{
  removeruleid(tranentry,tranid);
  string temp;
  removespace(tranentry,temp);
  string tempbind;
  getoneunit(temp,trancfg,tempbind);
  advrmspace(tempbind,tranbind);
}

void getitemset(string & trancfg, string & enred, vector<string> & itemset)
{
  size_t lbakpos=trancfg.find("(");
  if (lbakpos!=string::npos) {
    enred=trancfg.substr(0,lbakpos);
    size_t rbakpos=trancfg.rfind(")");
    string temp=trancfg.substr(lbakpos+1,rbakpos-lbakpos-1);
    while (!temp.empty()) {
      string item;
      string rem;
      getoneunit(temp,item,rem);
      itemset.push_back(item);
      temp=rem;
    }
  }
  else {
    size_t lanpos=trancfg.find("<");
    if (lanpos!=string::npos)
      enred=trancfg.substr(0,lanpos);
    else
      enred=trancfg;
  }
}

bool getsonidcat(edgec & edge, string & item, int & sonid, string & encat)
{
  size_t bslapos=item.find("/");
  if (bslapos!=string::npos) {
    int repeat=1;
    size_t eslapos=bslapos+1;
    while (item[eslapos]=='/') {
      eslapos++;
      repeat++;
    }
    if (item[0]!='!')
      encat=item.substr(0,bslapos);
    else
      encat=item.substr(1,bslapos-1);
    string chcat=item.substr(eslapos);
    int sonno;
    for (vector<string>::iterator i=edge.cfg.part.begin();i!=edge.cfg.part.end();i++) {
      if (*i==chcat)
	repeat--;
      if (repeat==0) {
	sonno=i-edge.cfg.part.begin();
	break;
      }
    }
    assert(repeat==0);
    sonid=edge.son[sonno];
    return(true);
  }
  else {
    return(false);
  }
}

void parseatomtran(edgec & edge, string & item, edgec & sone, string & encat)
{
  size_t bslapos=item.find("/");
  assert(bslapos!=string::npos);
  int repeat=1;
  size_t eslapos=bslapos+1;
  while (item[eslapos]=='/') {
    eslapos++;
    repeat++;
  }
  if (item[0]!='!')
    encat=item.substr(0,bslapos);
  else
    encat=item.substr(1,bslapos-1);
  string chcat=item.substr(eslapos);
  int sonno;
  for (vector<string>::iterator i=edge.cfg.part.begin();i!=edge.cfg.part.end();i++) {
    if (i->find("<")==string::npos) {
      if (*i==chcat)
	repeat--;
    }
    else {
      size_t lanpos=i->find("<");
      if (i->substr(0,lanpos)==chcat)
	repeat--;
    }
    if (repeat==0) {
      sonno=i-edge.cfg.part.begin();
      break;
    }
  }
  assert(repeat==0);
  sone=chart[edge.son[sonno]];
}

void getenedge(edgec & edge,string & enred, vector<string> & itemset, string & tranbind, edgec & enedge)
{
  enedge.source='t';
  enedge.father=-2;
  enedge.head=-1;
  enedge.prsid=edge.prsid;
  enedge.swiid=edge.swiid;
  enedge.tranid=edge.tranid;
  enedge.cfg.reduct=enred;
  enedge.bind=tranbind;
  enedge.dotreg.begin=0;
  enedge.dotreg.end=0;
  enedge.sentreg.begin=0;
  enedge.sentreg.end=0;
  getheadpart(itemset,enedge.head,enedge.cfg.part);
}

void getheadpart(vector<string> & itemset, size_t & head, vector<string> & part)
{
  for (vector<string>::iterator i=itemset.begin();i!=itemset.end();i++) {
    size_t tranpos=i->find("/");
    if (tranpos!=string::npos) {
      if ((*i)[0]=='!') {
	head=i-itemset.begin();
	part.push_back(i->substr(1,tranpos-1));
      }
      else
	part.push_back(i->substr(0,tranpos));
    }
    else {
      size_t lanpos=i->find("<");
      if (lanpos!=string::npos) {
	if ((*i)[0]=='!') {
	  head=i-itemset.begin();
	  part.push_back(i->substr(1,lanpos-1));
	}
	else
	  part.push_back(i->substr(0,lanpos));
      }
      else {
	if (i->find("!")==0) {
	  head=i-itemset.begin();
	  part.push_back(i->substr(1));
	}
	else
	  part.push_back(*i);
      }
    }
  }
}

void findenedges(int & chsonid, string & encat, vector<edgec> & enedges)
{
  map< int,vector<int> >::iterator i=chtoenidx.find(chsonid);
  if (i!=chtoenidx.end()) {
    for (vector<int>::iterator j=i->second.begin();j!=i->second.end();j++)
      if (enchart[*j].cfg.reduct==encat)
	enedges.push_back(enchart[*j]);
  }
}

void gettranset(string & entry, string & redcat, vector<string> & transet)
{
  size_t bpos=entry.find("=>");
  if (bpos!=string::npos) {
    size_t epos=entry.find("=>",bpos+2);
    while (bpos!=string::npos) {
      string tran=entry.substr(bpos+2,epos-bpos-2);
      string tranred;
      gettranred(tran,tranred);
      if ((redcat.empty()) || (tranred==redcat))
	transet.push_back(tran);
      bpos=epos;
      epos=entry.find("=>",bpos+2);
    }
  }
}

void gettranred(string & tran, string & tranred)
{
  size_t bpos=0;
  while (tran[bpos]==' ')
    bpos++;
  size_t epos=bpos;
  while ((epos<tran.size()) && (tran[epos]!=' ') && (tran[epos]!='(') && (tran[epos]!='<'))
    epos++;
  tranred=tran.substr(bpos,epos-bpos);
}

void getswiset(string & entry, vector<string> & swiset)
{
  size_t bpos=entry.find("||");
  if (bpos!=string::npos) {
    size_t epos=entry.find("||",bpos+2);
    while (bpos!=string::npos) {
      string swi=entry.substr(bpos+2,epos-bpos-2);
      swiset.push_back(swi);
      bpos=epos;
      epos=entry.find("||",bpos+2);
    }
  }
  else {
    size_t tranpos=entry.find("=>");
    if (tranpos!=string::npos)
      swiset.push_back(entry.substr(tranpos));
  }
}

void trnprinttree(edgec & edge)
{
  if (!edge.son.empty())
    cout << "(" << edge.prsid << ") ";
  cout << edge.cfg.reduct << " [ ";
  if (!edge.son.empty()) {
    for (vector<int>::iterator i=edge.son.begin();i!=edge.son.end();i++) {
      trnprinttree(enchart[*i]);
    }
  }
  else
    cout << edge.cfg.part.front();
  cout << " ] ";
}

void trnshowtree(edgec & edge, int spanum)
{
  for (int i=0;i<spanum;i++)
    cout << " ";
  cout << edge.cfg.reduct << " " << edge.id << ":" << edge.father << " ";
  cout << "(" << edge.prsid << "," << edge.swiid << "," << edge.tranid << ")";
  cout << edge.bind;
  cout << endl;
  if (!edge.son.empty()) {
    for (vector<int>::iterator i=edge.son.begin();i!=edge.son.end();i++) {
      trnshowtree(enchart[*i],spanum+4);
    }
  }
  else {
    for (int i=0;i<spanum+4;i++)
      cout << " ";
    cout << edge.cfg.part.front() << endl;
  }
}

void trnprintsent(edgec & edge)
{
  if (!edge.son.empty()) {
    for (vector<int>::iterator i=edge.son.begin();i!=edge.son.end();i++) {
      trnprintsent(enchart[*i]);
    }
  }
  else
    cout << edge.cfg.part.front() << " ";
}

void createatomedge(string & tranitem, edgec & aedge)
{
  string temp;
  removespace(tranitem,temp);
  size_t lanpos=temp.find("<");
  size_t ranpos=temp.rfind(">");
  if (lanpos!=string::npos) {
    if (temp[0]!='!')
      aedge.cfg.reduct=temp.substr(0,lanpos);
    else
      aedge.cfg.reduct=temp.substr(1,lanpos-1);
    aedge.cfg.part.push_back(temp.substr(lanpos+1,ranpos-lanpos-1));
  }
  else {
    if (temp.find("!")!=0)
      aedge.cfg.reduct=temp;
    else
      aedge.cfg.reduct=temp.substr(1);
    aedge.cfg.part.push_back("");
  }
  aedge.id=enchart.size();
  aedge.source='t';
  aedge.father=-2;
  aedge.dotreg.begin=0;
  aedge.dotreg.end=1;
  enchart.push_back(aedge);
}

bool entransition(edgec & edge, string & feat)
{
  feat=edge.bind;
  vector<string> featset;
  getabindset(feat,featset);
  feat="";
  for (vector<string>::iterator i=featset.begin();i!=featset.end();i++) {
    string lfeat;
    string opera;
    string rfeat;
    getopeval(*i,lfeat,opera,rfeat);
    if ((lfeat.find("$.")==0) && !rfeat.empty()) {
      lfeat=lfeat.substr(2);
      if ((opera=="=") || (opera=="=="))
	feat+=lfeat+":"+rfeat+",";
      else if (opera=="!=")
	feat+=lfeat+":~"+rfeat+",";
    }
    else if (lfeat=="$") {
      assert(rfeat[0]=='[');
      rfeat.replace(0,1,"");
      size_t rbrapos=rfeat.rfind("]");
      assert(rbrapos!=string::npos);
      rfeat.replace(rbrapos,1,"");
      feat=rfeat+",";
    }
    else {
      if ((lfeat.find("%")!=string::npos) && 
	  (rfeat.find("%")!=string::npos)) {
	;
      }
      else if (lfeat.find("%")!=string::npos) {
	size_t dotpos=lfeat.find(".");
	string nota=lfeat.substr(0,dotpos);
	int sonid=trnfindsonid(edge,nota);
	assert(sonid!=-2);
	string featname=lfeat.substr(dotpos+1);
	string newval;
	if ((opera=="=") || (opera=="=="))
	  newval=rfeat;
	else if (opera=="!=")
	  newval="~"+rfeat;
	enmodifeatval(enchart[sonid],featname,newval);
      }
      else if (rfeat.find("%")!=string::npos) {
	size_t dotpos=rfeat.find(".");
	string nota=rfeat.substr(0,dotpos);
	int sonid=trnfindsonid(edge,nota);
	assert(sonid!=-2);
	string featname=rfeat.substr(dotpos+1);
	string newval;
	if ((opera=="=") || (opera=="=="))
	  newval=lfeat;
	else if (opera=="!=")
	  newval="~"+lfeat;
	enmodifeatval(enchart[sonid],featname,newval);
      }
      else
	;  // pause here, think again
    }
  }
  feat="["+feat.substr(0,feat.size()-1)+"]";
  return(true);
}

void removeruleid(string & entry, string & ruleid)
{
  boost::smatch m;
  if (regex_search(entry,m,idreg)) {
    ruleid=string(m[0].first+1,m[0].second-1);
    entry=boost::regex_replace(entry,idreg,"",boost::match_default | boost::format_first_only);
  }
}

void prsronetran(edgec & edge, string & trancfg, string & tranbind, vector<edgec> & enedges)
{
  vector<edgec> locchart;
  string enred;
  vector<string> itemset;
  vector<string> parts;
  map<int,vector<int> >::iterator nulit=chtoenidx.find(-1);
  getitemset(trancfg,enred,itemset);
  for (vector<string>::iterator i=itemset.begin();i!=itemset.end();i++) {
    size_t lparpos=i->find("(");
    if (lparpos!=string::npos) {
      parts.push_back(i->substr(0,lparpos));
      string emptybind;
      vector<edgec> subenegs;
      prsronetran(edge,*i,emptybind,subenegs);

      for (vector<edgec>::iterator k=subenegs.begin();k!=subenegs.end();k++) {
	k->sentreg.begin=i-itemset.begin();
	k->sentreg.end=i-itemset.begin()+1;
	nulit->second.push_back(k->id);
	entochidx.insert(pair<int,int>(k->id,-1));
      }

      locchart.insert(locchart.end(),subenegs.begin(),subenegs.end());
    }
    else {
      parts.push_back(*i);
      getatomlocchart(edge,*i,i-itemset.begin(),locchart);
    }
  }

  string flatcfg;
  getflatcfg(trancfg,enred,parts,flatcfg);
  flattranedge(edge,flatcfg,tranbind,locchart,enedges);
}

void getflatcfg(string & trancfg, string & enred, vector<string> & parts, string & flatcfg)
{
  if (parts.empty())
    flatcfg=trancfg;
  else {
    flatcfg=enred+"(";
    for (vector<string>::iterator i=parts.begin();i!=parts.end();i++) {
      flatcfg+=(*i)+" ";
    }
    flatcfg+=")";
  }
}

void getatomlocchart(edgec & edge, string & atomtran, size_t loc, vector<edgec> & locchart)
{
  if (atomtran.find("/")!=string::npos) {
    string encat;
    edgec chedge;
    parseatomtran(edge,atomtran,chedge,encat);
    vector<edgec> enedges;
    findenedges(chedge.id,encat,enedges);
    for (vector<edgec>::iterator i=enedges.begin();i!=enedges.end();i++) {
      i->sentreg.begin=loc;
      i->sentreg.end=loc+1;
    }
    locchart.insert(locchart.end(),enedges.begin(),enedges.end());
  }
  else {
    edgec aedge;
    createatomedge(atomtran,aedge);
    aedge.sentreg.begin=loc;
    aedge.sentreg.end=loc+1;
    locchart.push_back(aedge);
    map< int,vector<int> >::iterator idxi=chtoenidx.find(-1);
    assert(idxi!=chtoenidx.end());
    idxi->second.push_back(aedge.id);
    entochidx.insert(pair<int,int>(aedge.id,-1));
  }
}

void printtrnsresult(vector<int> & trnsresult)
{
  for (vector<int>::iterator i=trnsresult.begin();i!=trnsresult.end();i++) {
    trnprinttree(enchart[*i]);
    cout << endl;
    trnshowtree(enchart[*i],0);
  }
  cout << endl;
}

void printtrnssent(vector<int> & trnsresult)
{
  for (vector<int>::iterator i=trnsresult.begin();i!=trnsresult.end();i++) {
    trnprintsent(enchart[*i]);
    cout << " ";
  }
  cout << endl;
}

void printtranchart(vector<edgec> & tranchart)
{
  for (vector<edgec>::iterator i=tranchart.begin();i!=tranchart.end();i++) {
    printedge(*i);
  }
}

