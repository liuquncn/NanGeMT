#ifndef generate_h
#define generate_h

#include <set>
#include "parse.hh"

void trnextracttree(int fid, edgec & edge, vector<edgec> & tranchart);
void maktree();
void generatetree(vector<edgec> & tranchart, vector<edgec> & bldchart);
bool maksent(vector<edgec> & tranchart);
void sepmakrule(string & makr, string & prsid, string & ltree, string & lbind, string & rtree, string & rbind);
void septreebind(string & seq, string & tree, string & bind);
bool makagree(map<string,edgec> & netable, string & bind, edgec & edge);
bool makaagree(map<string,edgec> & netable, string & bind, edgec & edge);
void makprocfeatval(map<string,edgec> & netable, edgec & edge, string & featname, string & featval, string & valtype, vector< vector<string> > & valset);
bool treematch(string & tree, edgec & edge, map<string,edgec> & netable);
void septree(string & tree, string & root, vector<string> & sons, int & head);
void getsons(string & sonseq, vector<string> & sons, int & head);
bool nodematch(string & cat, string & word, edgec & edge);
string addnetable(map<string,edgec> & netable, string & cat, edgec & edge);
void delematch(set<int> & matchids, vector<edgec> & ntree);
void getoriname(string & corrname, string & trnname, string & oriname);
bool recurtreematch(string & tree, edgec & edge, map<string,edgec> & netable, set<int> & matchids, vector<edgec> & makchart);
bool maktree(map<string,edgec> & netable, string & rtree, map<string,edgec> & rnetable, string & bind, vector<edgec> & ntree, vector<edgec> & remchart);
bool recurmaktree(map<string,edgec> & lnetable, string & root, string & oriroot, vector<string> & sons, int head, string & name, map<string,edgec> & rnetable, string & bind, vector<edgec> & ntree, int & rootid, vector<edgec> & remchart);
int findmatchroot(set<int> & matchids);
void printgenetree(vector<edgec> & genechart);
void recurprintgene(edgec & edge);
void genefindedge(int & eid, edgec & edge);
void printnetable(map<string,edgec> & netable);
bool getcorrbind(string & bind, string & name, string & ebind);
void recurshowgene(edgec & edge, int spanum);
bool unify(string & featset1, string & featset2, string & nfeatset);
void getfeatmap(string & feats, map<string,string> & featmap);
void maptostr(map<string,string> & featmap, string & feats);
//bool valunity(string & lval, string & rval, string & valtype, string & opera, vector< vector<string> > & valset, string & nval);
void bldword(vector<edgec> & genechart, vector<edgec> & bldchart);
void bldaword(edgec & edge);
bool bldmatch(string & word, string & cat, string & feat, edgec & edge);
void sepbldrule(string & bldrule, string & word, string & cat, string & feat, string & nword);
void printgenesent(vector<edgec> & genechart);
void recurprintgenesent(edgec & edge);
void findgeneroot(vector<edgec> & genechart, edgec & root);
void feattobind(string & feat, string & bind);
void modifather(edgec & edge, int nfid);
edgec & genefindedge(int & eid);
void geneassignfather();
edgec & genefindroot(vector<edgec> & genechart);
void recurgeneassign(edgec & edge);
void findcompprep(string & feat, string & compp);
void geneadj(edgec & edge);
void geneadv(edgec & edge);
void genenoun(edgec & edge);
void geneverb(edgec & edge);
void printunvs(string & word);
void printvnvd(string & word);
void printnouns(string & word);
void printvg(string & word);
bool verbbe(string & word);
void printmakbld();
bool maketree(edgec & root);
bool makeedge(edgec & edge);
bool makenewtree(map<string,edgec> & lnetable, string & cfg, edgec & edge, map<string,edgec> & rnetable);
void makenewatom(map<string,edgec> & lnetable, string & dualred, edgec & edge);
void printresult();
void bldword();
void sepnode(string & node, string & cat, string & word);

#endif
