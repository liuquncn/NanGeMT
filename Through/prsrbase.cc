#include <assert.h>
#include <iostream>
#include "parse.hh"
#include "prsrbase.hh"
#include "unity.hh"
#include "transform.hh"
#include "extvar.hh"

using namespace std;

extern string prsrbase;

void parserule(size_t intropos, string & prsid, cfgc & cfg, size_t & head, string & bind)
{
  string entry;
  size_t nextpos=prsrbase.find("&&",intropos+2);
  if (nextpos!=string::npos)
    entry=prsrbase.substr(intropos,nextpos-intropos);
  else
    entry=prsrbase.substr(intropos);
  parserulentry(entry,prsid,cfg,head,bind);
}

void parserulentry(string & entry, string & prsid, cfgc & cfg, size_t & head, string & bind)
{
  size_t idbpos=entry.find("{");
  size_t idepos=entry.find("}");
  prsid=entry.substr(idbpos+1,idepos-idbpos-1);
  size_t bindpos=entry.find("::");
  size_t swipos=entry.find("||");
  size_t tranpos=entry.find("=>");
  string cfgent=entry.substr(idepos+1,min(bindpos,min(swipos,tranpos))-idepos-1);
  string yxbind;
  getcfg(cfgent,cfg,head,yxbind);
  if (bindpos!=string::npos) {
    string temp=entry.substr(bindpos+2,min(swipos,tranpos)-bindpos-2);
    removespace(temp,bind);
  }
  bind+=yxbind;
}

void getcfg(string & entry, cfgc & cfg, size_t & head, string & yxbind)
{
  size_t arrowpos=entry.find("->");
  assert(arrowpos!=string::npos);
  string temp=entry.substr(0,arrowpos);
  removespace(temp,cfg.reduct);
  temp=entry.substr(arrowpos+2);
  head=0;
  string remain;
  removespace(temp,remain);
  while (!remain.empty()) {
    temp=remain;
    string cat;
    string oneu;
    getoneunit(temp,oneu,remain);
    assert(!oneu.empty());
    if (oneu[0]=='!') {
      head=cfg.part.size();
      oneu=oneu.substr(1);
    }
    if (oneu.find("<")!=string::npos) {
      size_t lanpos=oneu.find("<");
      size_t ranpos=oneu.find(">");
      assert(ranpos!=string::npos);
      cat=oneu.substr(0,lanpos);
      string yx=oneu.substr(lanpos+1,ranpos-lanpos-1);
      removequota(yx);
      int repeat=1;
      for (vector<string>::iterator i=cfg.part.begin();i!=cfg.part.end();i++) {
	if ((*i)==cat)
	  repeat++;
      }
      string intro;
      for (int i=0;i<repeat;i++)
	intro+="%";
      intro+=cat;
      yxbind+=","+intro+".yx="+yx;
    }
    else
      cat=oneu;
    cfg.part.push_back(cat);
  }
}

void removequota(string & in)
{
  size_t quotapos=in.find("\"");
  while (quotapos!=string::npos) {
    in.replace(quotapos,1,"");
    quotapos=in.find("\"");
  }
}

void getdotreg(string & entry, tagc & tag, regionc & dotreg)
{
  size_t idepos=entry.find("}");
  size_t bindpos=entry.find("::");
  size_t swipos=entry.find("||");
  size_t tranpos=entry.find("=>");
  string cfgent=entry.substr(idepos+1,min(bindpos,min(swipos,tranpos))-idepos-1);
  size_t arrowpos=cfgent.find("->");
  string temp=cfgent.substr(arrowpos+2);
  string remain;
  removespace(temp,remain);
  size_t dotpos=0;
  while (!remain.empty()) {
    temp=remain;
    string oneu;
    getoneunit(temp,oneu,remain);
    //if (oneu[0]=='!') 
    if (oneu.find("!")==0)
      oneu=oneu.substr(1);
    if (oneu.find("<")!=string::npos) {
      size_t lanpos=oneu.find("<");
      size_t ranpos=oneu.find(">");
      string cat=oneu.substr(0,lanpos);
      string yx=oneu.substr(lanpos+1,ranpos-lanpos-1);
      removequota(yx);
      if ((cat==tag.cat) && (yx==tag.word)) {
	dotreg.begin=dotpos;
	dotreg.end=dotpos+1;
	break;
      }
    }
    dotpos++;
  }
}

void shallowprsrbase(vector<string> & prsrset, vector<prsrc> & prsrtable)
{
  for (size_t i=0;i<prsrset.size();i++) {
    string tempbind;
    prsrc prsr;
    parserulentry(prsrset[i],prsr.prsid,prsr.cfg,prsr.head,tempbind);
    advrmspace(tempbind,prsr.bind);
    prsrtable.push_back(prsr);
    addprsrmap(prsredmap,prsr.cfg.reduct,i);
    addprsrmap(prsfirstmap,prsr.cfg.part.front(),i);
    addprsrmap(prslastmap,prsr.cfg.part.back(),i);
  }
}

void addprsrmap(map<string, vector<int> > & prsrmap, string & str, size_t idx)
{
  map<string, vector<int> >::iterator i=prsrmap.find(str);
  if (i==prsrmap.end()) {
    vector<int> vi;
    vi.push_back(idx);
    prsrmap.insert(pair<string,vector<int> >(str,vi));
  }
  else {
    i->second.push_back(idx);
  }
}

