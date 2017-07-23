#include <stdlib.h>
#include <assert.h>
#include <map>
#include <iostream>
#include "segment.hh"
#include "parse.hh"
#include "unity.hh"
#include "prsrbase.hh"
#include "transform.hh"
#include "agree.hh"

using namespace std;

extern string model;
extern string dictn;
extern string prsrbase;
extern vector<string> prsrset;
extern string zhongxinyu;
extern string hanyucilei;
extern string jufachengfen;
extern string chnsent;
extern vector<segc> chnsegs;

vector<int> parseresult;
vector<prsrc> prsrtable;
map<string,vector<int> > prsredmap;
map<string,vector<int> > prsfirstmap;
map<string,vector<int> > prslastmap;
vector<edgec> chart;
map<loccatc,vector<int> > compbeginmap;
map<loccatc,vector<int> > compendmap;
map<loccatc,vector<int> > incompbeginmap;
map<loccatc,vector<int> > incompendmap;
map<compedgekeyc,int> compedgemap;
map<incedgekeyc,int> incedgemap;
map<regionc,vector<int> > compsentregmap;
map<regionc,vector<int> > incompsentregmap;

//int eid=0;
size_t agenda=0;

void parse(vector<tagc> & tags)
{
  size_t agenda=0;
  for (size_t i=0;i<tags.size();i++) {
    initialize(tags[i]);
    while (agenda<chart.size()) {
      edgec edge=chart[agenda];
      printedge(edge);
      size_t oldpos=chart.size();
      process(edge);
      printchart(chart,oldpos);
      agenda++;
    }
  }
  choosebestparse(parseresult);
  printbestparse(parseresult);
}

void parse()
{
  iniedges(chnsegs);
  printchart(chart,0);
  size_t agenda=0;
  while (agenda<chart.size()) {
    edgec edge=chart[agenda];
    printedge(edge);
    size_t oldpos=chart.size();
    process(edge);
    printchart(chart,oldpos);
    agenda++;
  }
  choosebestparse(parseresult);
  printbestparse(parseresult);
}

void initialize(vector<tagc> & tags)
{                                                                     
  agenda=0;
  for (vector<tagc>::iterator i=tags.begin(); i!=tags.end(); i++) {
    edgec newedge;
    newedge.id=chart.size(); //eid++;
    newedge.father=-2;
    newedge.state='a';
    newedge.source='d';
    newedge.cfg.reduct=i->cat;
    newedge.prsid=i->catid;
    newedge.cfg.part.push_back(i->word);
    newedge.head=0;
    newedge.dotreg.begin=0;
    newedge.dotreg.end=1;
    newedge.sentreg=i->sentreg;
    newedge.bind=i->feat;
    chart.push_back(newedge);
    compedgekeyc compedgekey(newedge.cfg.reduct,newedge.sentreg,newedge.bind);
    compedgemap.insert(pair<compedgekeyc,int>(compedgekey,newedge.id));
    for (vector<string>::iterator j=i->ruleset.begin();j!=i->ruleset.end();j++) {
      edgec locnewe;
      locnewe.source='l';
      locnewe.sentreg=i->sentreg;
      parserulentry((*j),locnewe.prsid,locnewe.cfg,locnewe.head,locnewe.bind);
      getdotreg((*j),(*i),locnewe.dotreg);
      locnewe.son.push_back(newedge.id);
      addtoagenda(chart,locnewe);
    }
  }
}

void initialize(tagc & tag)
{
  edgec newedge;
  newedge.id=chart.size(); //eid++;
  newedge.father=-2;
  newedge.state='a';
  newedge.source='d';
  newedge.cfg.reduct=tag.cat;
  newedge.prsid=tag.catid;
  newedge.cfg.part.push_back(tag.word);
  newedge.head=0;
  newedge.dotreg.begin=0;
  newedge.dotreg.end=1;
  newedge.sentreg=tag.sentreg;
  newedge.bind=tag.feat;
  chart.push_back(newedge);
  compedgekeyc compedgekey(newedge.cfg.reduct,newedge.sentreg,newedge.bind);
  compedgemap.insert(pair<compedgekeyc,int>(compedgekey,newedge.id));
  for (vector<string>::iterator i=tag.ruleset.begin();i!=tag.ruleset.end();i++) {
    edgec locnewe;
    locnewe.source='l';
    locnewe.sentreg=tag.sentreg;
    parserulentry((*i),locnewe.prsid,locnewe.cfg,locnewe.head,locnewe.bind);
    getdotreg((*i),tag,locnewe.dotreg);
    locnewe.son.push_back(newedge.id);
    addtoagenda(chart,locnewe);
  }
}

void process(edgec & edge)
{
  if (edge.state!='f') {
    addtochart(edge);
    if (!complete(edge)) {
      forward(edge);
      //tdpredict(edge);
    }
    else {
      backward(edge);
      bupredict(edge);
    }
  }
}

void forward(edgec & ice)
{
  if (ice.dotreg.end<ice.cfg.part.size()) {
    loccatc cebegin;
    cebegin.loc=ice.sentreg.end;
    cebegin.cat=ice.cfg.part[ice.dotreg.end];
    map<loccatc,vector<int> >::iterator i=compbeginmap.find(cebegin);
    if (i!=compbeginmap.end()) {
      for (vector<int>::iterator j=i->second.begin();j!=i->second.end();j++) {
	edgec ce=chart[*j];
	if ((ice.source=='l') || (ce.source=='l') || ((ice.state=='a') && (ce.state=='a'))) {
	  edgec newedge=ice;
	  newedge.dotreg.end++;
	  newedge.sentreg.end=ce.sentreg.end;
	  newedge.son.push_back(ce.id);
	  addtoagenda(chart,newedge);
	}
      }
    }
  }
  if (ice.dotreg.begin>0) {
    loccatc ceend;
    ceend.loc=ice.sentreg.begin;
    ceend.cat=ice.cfg.part[ice.dotreg.begin-1];
    map<loccatc,vector<int> >::iterator i=compendmap.find(ceend);
    if (i!=compendmap.end()) {
      for (vector<int>::iterator j=i->second.begin();j!=i->second.end();j++) {
	edgec ce=chart[*j];
	if ((ice.source=='l') || (ce.source=='l') || ((ice.state=='a') && (ce.state=='a'))) {
	  edgec newedge=ice;
	  newedge.dotreg.begin--;
	  newedge.sentreg.begin=ce.sentreg.begin;
	  newedge.son.insert(newedge.son.begin(),ce.id);
	  addtoagenda(chart,newedge);
	}
      }
    }
  }
}

void backward(edgec & ce)
{
  {
    loccatc iceend;
    iceend.loc=ce.sentreg.begin;
    iceend.cat=ce.cfg.reduct;
    map<loccatc,vector<int> >::iterator i=incompendmap.find(iceend);
    if (i!=incompendmap.end()) {
      for (vector<int>::iterator j=i->second.begin();j!=i->second.end();j++) {
	edgec ice=chart[*j];
	if ((ice.source=='l') || (ce.source=='l') || ((ice.state=='a') && (ce.state=='a'))) {
	  edgec newedge=ice;
	  newedge.dotreg.end++;
	  newedge.sentreg.end=ce.sentreg.end;
	  newedge.son.push_back(ce.id);
	  addtoagenda(chart,newedge);
	}
      }
    }
  }
  {
    loccatc icebegin;
    icebegin.loc=ce.sentreg.end;
    icebegin.cat=ce.cfg.reduct;
    map<loccatc,vector<int> >::iterator i=incompbeginmap.find(icebegin);
    if (i!=incompbeginmap.end()) {
      for (vector<int>::iterator j=i->second.begin();j!=i->second.end();j++) {
	edgec ice=chart[*j];
	if ((ice.source=='l') || (ce.source=='l') || ((ice.state=='a') && (ce.state=='a'))) {
	  edgec newedge=ice;
	  newedge.dotreg.begin--;
	  newedge.sentreg.begin=ce.sentreg.begin;
	  newedge.son.insert(newedge.son.begin(),ce.id);
	  addtoagenda(chart,newedge);
	}
      }
    }
  }
}

void bupredict(edgec & ce)
{
  map<string,vector<int> >::iterator i=prsfirstmap.find(ce.cfg.reduct);
  if (i!=prsfirstmap.end()) {
    for (vector<int>::iterator j=i->second.begin();j!=i->second.end();j++) {
      edgec newedge;
      newedge.source='p';
      newedge.prsid=prsrtable[*j].prsid;
      newedge.cfg=prsrtable[*j].cfg;
      newedge.head=prsrtable[*j].head;
      newedge.dotreg.begin=0;
      newedge.dotreg.end=1;
      newedge.sentreg=ce.sentreg;
      newedge.bind=prsrtable[*j].bind;
      newedge.son.push_back(ce.id);
      addtoagenda(chart,newedge);
    }
  }
}

void addtoagenda(vector<edgec> & chart, edgec & edge)
{
  if (complete(edge)) {
    string bind=edge.bind;
    edge.bind="";
    string nbind;
    if (agree(bind,edge,nbind)) {
      edge.bind=nbind;
      string feat;
      bindtofeat(edge,feat);
      edge.bind=feat;
      compedgekeyc compedgekey(edge.cfg.reduct,edge.sentreg,edge.bind);
      map<compedgekeyc,int>::iterator i=compedgemap.find(compedgekey);
      if (i==compedgemap.end()) {
	edge.id=chart.size(); //eid++;
	edge.father=-2;
	edge.state='a';
	chart.push_back(edge);
	compedgemap.insert(pair<compedgekeyc,int>(compedgekey,edge.id));
	if (edge.source=='l')
	  setsilence(edge);
      }
    }
  }
  else {
    incedgekeyc incedgekey(edge.prsid,edge.dotreg,edge.sentreg,edge.son);
    map<incedgekeyc,int>::iterator i=incedgemap.find(incedgekey);
    if (i==incedgemap.end()) {
      edge.id=chart.size(); //eid++;
      edge.father=-2;
      edge.state='a';
      chart.push_back(edge);
      incedgemap.insert(pair<incedgekeyc,int>(incedgekey,edge.id));
    }
  }
}

void addtochart(edgec & edge)
{
  if (complete(edge)) {
    addcompmap(compbeginmap,compendmap,edge);
    addsentregmap(compsentregmap,edge);
  }
  else {
    addincompmap(incompbeginmap,incompendmap,edge);
    addsentregmap(incompsentregmap,edge);
  }
}

void addcompmap(map<loccatc,vector<int> > & compbeginmap, map<loccatc,vector<int> > & compendmap, edgec & ce)
{
  {
    loccatc cebegin;
    cebegin.loc=ce.sentreg.begin;
    cebegin.cat=ce.cfg.reduct;
    map<loccatc,vector<int> >::iterator i=compbeginmap.find(cebegin);
    if (i==compbeginmap.end()) {
      vector<int> vi;
      vi.push_back(ce.id);
      compbeginmap.insert(pair<loccatc,vector<int> >(cebegin,vi));
    }
    else
      i->second.push_back(ce.id);
  }
  {
    loccatc ceend;
    ceend.loc=ce.sentreg.end;
    ceend.cat=ce.cfg.reduct;
    map<loccatc,vector<int> >::iterator i=compendmap.find(ceend);
    if (i==compendmap.end()) {
      vector<int> vi;
      vi.push_back(ce.id);
      compendmap.insert(pair<loccatc,vector<int> >(ceend,vi));
    }
    else
      i->second.push_back(ce.id);
  }
}

void addincompmap(map<loccatc,vector<int> > & incompbeginmap, map<loccatc,vector<int> > & incompendmap, edgec & ice)
{
  if (ice.dotreg.begin>0) {
    loccatc icebegin;
    icebegin.loc=ice.sentreg.begin;
    icebegin.cat=ice.cfg.part[ice.dotreg.begin-1];
    map<loccatc,vector<int> >::iterator i=incompbeginmap.find(icebegin);
    if (i==incompbeginmap.end()) {
      vector<int> vi;
      vi.push_back(ice.id);
      incompbeginmap.insert(pair<loccatc,vector<int> >(icebegin,vi));
    }
    else
      i->second.push_back(ice.id);
  }
  if (ice.dotreg.end<ice.cfg.part.size()) {
    loccatc iceend;
    iceend.loc=ice.sentreg.end;
    iceend.cat=ice.cfg.part[ice.dotreg.end];
    map<loccatc,vector<int> >::iterator i=incompendmap.find(iceend);
    if (i==incompendmap.end()) {
      vector<int> vi;
      vi.push_back(ice.id);
      incompendmap.insert(pair<loccatc,vector<int> >(iceend,vi));
    }
    else
      i->second.push_back(ice.id);
  }
}

void addsentregmap(map<regionc,vector<int> > & sentregmap, edgec & edge)
{
  regionc region;
  region=edge.sentreg;
  map<regionc,vector<int> >::iterator i=sentregmap.find(region);
  if (i==sentregmap.end()) {
    vector<int> vi;
    vi.push_back(edge.id);
    sentregmap.insert(pair<regionc,vector<int> >(region,vi));
  }
  else
    i->second.push_back(edge.id);
}

void choosebestparse(vector<int> & parseresult)
{
  int sentlength=chnsent.length();
  int currcount;
  vector<int> wordcount(sentlength+1);
  vector<int> forestset(sentlength+1);
  vector<int> traceback(sentlength+1);
  vector<int> tracefront(sentlength+1);
  
  for (int i=0; i<=sentlength; i++) {
    wordcount[i]=100000000;
    forestset[i]=-1;
    traceback[i]=i-1;
    tracefront[i]=i+1;
  }

  wordcount[0]=0;
  for (int i=0; i<=sentlength; i++) {
    for (int j=0; j<i; j++) {
      regionc reg(j,i);
      map<regionc,vector<int> >::iterator k=compsentregmap.find(reg);
      if (k!=compsentregmap.end())
	currcount=wordcount[j]-2*(i-j);
      else
	currcount=wordcount[j]+2*(i-j);
      
      if (currcount < wordcount[i] && k!=compsentregmap.end()) {
	traceback[i]=j;
	wordcount[i]=currcount;
	forestset[i]=choosebesttree(k->second);
      }
    }
  }

  for (int i=sentlength; i>0; i=traceback[i]) {
    parseresult.insert(parseresult.begin(),forestset[i]);
  }
}

void choosebestparse2(vector<int> & parseresult, vector<int> & parseresult2)
{
  assert(!parseresult.empty());
  int prev=parseresult[0];
  for (vector<int>::iterator i=parseresult.begin()+1;i!=parseresult.end();i++) {
    int minnum=100000;
    int minid=-1;
    regionc reg(prev,*i);
    map<regionc,vector<int> >::iterator j=compsentregmap.find(reg);
    assert(j!=compsentregmap.end());
    for (vector<int>::iterator k=j->second.begin();k!=j->second.end();k++) {
      int nn=countnode(chart[*k]);
      if (chart[*k].source=='d')
	nn=nn-2;
      if (nn<minnum) {
	minnum=nn;
	minid=*k;
      }
    }
    assert(minid!=-1);
    parseresult2.push_back(minid);
    prev=*i;
  }
}

int choosebesttree(vector<int> roots)
{
  int minnum=100000;
  int minid=-1;
  for (vector<int>::iterator i=roots.begin();i!=roots.end();i++) {
    int nn=countnode(chart[*i]);
    if (chart[*i].source=='d')
      nn=nn-2;
    if (nn<minnum) {
      minnum=nn;
      minid=*i;
    }
  }
  return(minid);
}

int countnode(edgec & root)
{
  int nn=1;
  for (vector<int>::iterator i=root.son.begin();i!=root.son.end();i++) {
    nn+=countnode(chart[*i]);
  }
  return(nn);
}

void setsilence(edgec & edge)
{
  for (size_t i=0;i<chart.size();i++)
    if ((chart[i].sentreg.end>edge.sentreg.begin) && (chart[i].sentreg.begin<edge.sentreg.end) && (chart[i].source!='l'))
      chart[i].state='s';
}

void setregsilence(map<regionc,vector<int> > & sentregmap, regionc & reg)
{
  map<regionc,vector<int> >::iterator i=sentregmap.find(reg);
  if (i!=sentregmap.end())
    for (vector<int>::iterator j=i->second.begin();j!=i->second.end();j++)
      if (chart[*j].source!='l') {
	chart[*j].state='s';
      }
}

bool complete(edgec & edge)
{
  if (edge.dotreg.end-edge.dotreg.begin==edge.cfg.part.size())
    return(true);
  else
    return(false);
}

bool match(cfgc & cfg, string & compon)
{
  if (compon.find("<")==string::npos)
    return(cfg.reduct==compon);
  else {
    string combine=cfg.reduct+"<"+cfg.part.front()+">";
    return(combine==compon);
  }
}


bool ispos(string & syncat)
{
  size_t ccatpos=model.find(hanyucilei);
  assert(ccatpos!=string::npos);
  size_t lbrktpos=model.find("{",ccatpos);
  size_t rbrktpos=model.find("}",lbrktpos);
  if (model.substr(lbrktpos,rbrktpos-lbrktpos).find(syncat)!=string::npos)
    return(true);
  else
    return(false);
}

void bindtofeat(edgec & edge, string & feat)
{
  string bind=edge.bind;
  vector<string> bindset;
  getabindset(bind,bindset);
  bind="";
  for (vector<string>::iterator i=bindset.begin();i!=bindset.end();i++) {
    string name;
    string opera;
    string val;
    getopeval(*i,name,opera,val);
    if (!val.empty()) {
      if (name.find("$.")==0) 
	name=name.substr(2);
      if (opera=="!=") {
	if (val[0]=='~')
	  val=val.substr(1);
	else
	  val="~"+val;
      }
      bind+=name+":"+val+",";
    }
  }
  feat="["+bind.substr(0,bind.size()-1)+"]";
}

void printedge(edgec & edge)
{
  cout << edge.id << "(" << edge.source << ":" << edge.state << ";" << edge.prsid << ":" << edge.swiid << ":" << edge.tranid << ";" 
       << edge.father << ":" << edge.head << ")" << " sentreg(" << edge.sentreg.begin << "," << edge.sentreg.end 
       << ") " << "dotreg(" << edge.dotreg.begin << "," << edge.dotreg.end << ") " << edge.cfg.reduct << "->";
  for (vector<string>::iterator i=edge.cfg.part.begin();i!=edge.cfg.part.end();i++)
    cout << *i << " ";
  cout << "-> ";
  for (vector<int>::iterator i=edge.son.begin();i!=edge.son.end();i++)
    cout << *i << " ";
  if (complete(edge))
    cout << edge.bind;
  cout << endl;
}

void printbestparse(vector<int> & parseresult2)
{
  cout << endl << "-------best parse-------------" << endl;
  for (vector<int>::iterator i=parseresult2.begin();i!=parseresult2.end();i++) {
    edgec & root=chart[*i];
    recurprinttree(root);
    cout << endl;
    recurshowtree(root,0,0);
    recurshowtree(root,0,1);
  }
}

void recurprinttree(edgec & edge)
{
  cout << "(" << edge.prsid << ") ";
  cout << edge.cfg.reduct << " [ ";
  if (!edge.son.empty()) {
    for (vector<int>::iterator i=edge.son.begin();i!=edge.son.end();i++) {
      edgec & sone=chart[*i];
      recurprinttree(sone);
    }
  }
  else
    cout << edge.cfg.part.front();
  cout << " ] ";
}

void recurshowtree(edgec & edge, int spanum, int flag)
{
  if (spanum>=4) {
    for (int i=0;i<spanum-4;i++)
      cout << " ";
    cout << "+";
    for (int i=0;i<3;i++)
      cout << "-";
  }
  cout << edge.cfg.reduct << " " << edge.id << ":" << edge.father << " ";
  cout << "(" << edge.prsid << ":" << edge.swiid << ":" << edge.tranid << ":" << ")";
  if (flag==1)
    cout << edge.bind;
  cout << endl;
  if (!edge.son.empty()) {
    for (vector<int>::iterator i=edge.son.begin();i!=edge.son.end();i++) {
      edgec & sone=chart[*i];
      recurshowtree(sone,spanum+4,flag);
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

void printchart(vector<edgec> & chart, size_t bpos)
{
  assert(bpos<=chart.size());
  cout << "------printchart new edge--------" << endl;
  for (size_t i=bpos;i!=chart.size();i++) {
    cout << "    ";
    printedge(chart[i]);
  }
  cout << "-----end of printchart--------" << endl;
}

void advrmspace(string & tempswibind, string & swibind)
{
  string temp;
  for (size_t i=0;i<tempswibind.size();i++) {
    if ((tempswibind[i]!='\t') && (tempswibind[i]!='\n'))
      temp+=tempswibind[i];
  }
  removespace(temp,swibind);
}

bool directreduct(edgec & edge)
{
  if ((edge.cfg.part.size()==1) && ispos(edge.cfg.part.front()))
    return(true);
  else
    return(false);
}

void makeedge(tagc & tag, char source, char state, edgec & edge)
{
  edge.id=chart.size(); //eid++;
  edge.father=-2;
  edge.state=state;
  edge.source=source;
  edge.cfg.reduct=tag.cat;
  edge.prsid=tag.catid;
  edge.cfg.part.push_back(tag.word);
  edge.head=0;
  edge.dotreg.begin=0;
  edge.dotreg.end=1;
  edge.sentreg=tag.sentreg;
  edge.bind=tag.feat;
  chart.push_back(edge);
  compedgekeyc compedgekey(edge.cfg.reduct,edge.sentreg,edge.bind);
  compedgemap.insert(pair<compedgekeyc,int>(compedgekey,edge.id));
  for (vector<string>::iterator i=tag.ruleset.begin();i!=tag.ruleset.end();i++) {
    edgec locnewe;
    locnewe.source='l';
    locnewe.sentreg=tag.sentreg;
    parserulentry((*i),locnewe.prsid,locnewe.cfg,locnewe.head,locnewe.bind);
    getdotreg((*i),tag,locnewe.dotreg);
    locnewe.son.push_back(edge.id);
    addtoagenda(chart,locnewe);
  }
}

void wordstrtoedge(string & wordstr, size_t & initpos, char state, vector<edgec> & edges)
{
  chnsent+=wordstr;
  vector<string> subwords;
  dictsegment(wordstr,subwords);
  vector<tagc> subtags;
  postag(subwords,initpos,subtags);
  initpos+=wordstr.size();
  for (vector<tagc>::iterator j=subtags.begin();j!=subtags.end();j++) {
    edgec edge;
    makeedge(*j,'d',state,edge);
    edges.push_back(edge);
  }
}

void wordtagtoedge(string & wordtag, size_t & initpos, char state, vector<edgec> & edges)
{ 
  size_t bspos=wordtag.find("/");
  assert(bspos!=string::npos);
  string word=wordtag.substr(0,bspos);
  string cat=wordtag.substr(bspos+1);
  chnsent+=word;
  tagc subtag;
  maketag(word,cat,initpos,subtag);
  initpos+=word.size();
  edgec edge;
  makeedge(subtag,'d',state,edge);
  edges.push_back(edge);
}

void createedge(string & red, vector< vector<edgec> > & edgem, size_t initpos, char state, vector<edgec> & edges)
{
  cfgc cfg;
  createcfg(red,edgem,cfg);
  vector<prsrc> prsrs;
  findprsr(cfg,prsrs);
  for (vector<prsrc>::iterator i=prsrs.begin();i!=prsrs.end();i++) {
    edgec edge;
    getedge(*i,initpos,edge);
    vector<edgec> locchart;
    makelocchart(edgem,locchart);
    vector<edgec> subedges;
    parseoneprsr(edge,locchart,state,subedges);
    edges.insert(edges.end(),subedges.begin(),subedges.end());
  }
  if (edges.empty()) {
    cout << "incorrect input structure " << cfg.reduct << "->";
    for (vector<string>::iterator i=cfg.part.begin();i!=cfg.part.end();i++)
      cout << *i << " ";
    cout << endl;
    exit(1);
  }
}

void createcfg(string & red, vector< vector<edgec> > & edgem, cfgc & cfg)
{
  cfg.reduct=red;
  for (vector< vector<edgec> >::iterator i=edgem.begin();i!=edgem.end();i++) {
    cfg.part.push_back(i->begin()->cfg.reduct);
  }
}

void findprsr(cfgc & cfg, vector<prsrc> & prsrs)
{
  map<string, vector<int> >::iterator i=prsredmap.find(cfg.reduct);
  assert(i!=prsredmap.end());
  for (vector<int>::iterator j=i->second.begin();j!=i->second.end();j++)
    if (cfg==prsrtable[*j].cfg) {
      prsrs.push_back(prsrtable[*j]);
    }
}

void getedge(prsrc & prsr, size_t initpos, edgec & edge)
{
  edge.source='p';
  edge.state='f';
  edge.father=-2;
  edge.head=prsr.head;
  edge.prsid=prsr.prsid;
  edge.cfg=prsr.cfg;
  edge.bind=prsr.bind;
  edge.dotreg.begin=0;
  edge.dotreg.end=0;
  edge.sentreg.begin=initpos;
  edge.sentreg.end=initpos;
}

void makelocchart(vector< vector<edgec> > & edgem, vector<edgec> & locchart)
{
  for (vector< vector<edgec> >::iterator i=edgem.begin();i!=edgem.end();i++)
    locchart.insert(locchart.end(),i->begin(),i->end());
}

void parseoneprsr(edgec & iniedge, vector<edgec> & locchart, char state, vector<edgec> & edges)
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
      if (agree(bind,edge,nbind)) {
	edge.id=chart.size(); //eid++;
	edge.state=state;
	edge.bind=nbind;
	string feat;
	bindtofeat(edge,feat);
	edge.bind=feat;
	edges.push_back(edge);
	chart.push_back(edge);
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

void phrasetoedge(string & phrase, size_t & initpos, char state, vector<edgec> & edges)
{
  string red;
  string body;
  sepredbody(phrase,red,body);
  vector< vector<edgec> > edgem;
  size_t temppos=initpos;
  bodytoedge(body,initpos,edgem);
  createedge(red,edgem,temppos,state,edges);
}

void sepredbody(string & phrase, string & red, string & body)
{
  size_t lbspos=phrase.find("(");
  assert(lbspos!=string::npos);
  red=phrase.substr(0,lbspos);
  size_t rbspos=phrase.rfind(")");
  assert(rbspos!=string::npos);
  body=phrase.substr(lbspos+1,rbspos-lbspos-1);
}

void bodytoedge(string & body, size_t & initpos, vector< vector<edgec> > & edgem)
{
  vector<segc> subsegs;
  compseg(body,subsegs);
  for (vector<segc>::iterator i=subsegs.begin();i!=subsegs.end();i++) {
    vector<edgec> edges;
    if (i->flag=='t') {
      wordtagtoedge(i->seq,initpos,'f',edges);
    }
    else if (i->flag=='p') {
      phrasetoedge(i->seq,initpos,'f',edges);
    }
    else {
      assert(0);
    }
    edgem.push_back(edges);
  }
}

