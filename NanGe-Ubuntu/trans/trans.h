
#ifndef TRANS_H

#define TRANS_H

#include "../kbase/dictn.h"
#include "../kbase/prsrbase.h"
#include "../kbase/makrbase.h"

#include "struct.h"

class Translator;

enum
  {
    BEGIN,
    CUTWORD0,
    CUTWORD1,
    PRSSENT,
    TRANSFORM0,
    TRANSFORM1,
    MAKSENT,
    BLDWORD0,
    BLDWORD1,
    BLDWORD2,
    END
  };

enum
  {
    PROGRESS,
    RETRIEVE,
    NOACTION
  };

class Translator
{
 protected:
  
  static int Opened;
  
 public:
  
  static int UseLexdbase;
  static int UseDictnref;
  static int UsePreEdit;
  static void (*UpdateStatus)();
  
  //protected:
  
  int TheSrcLang;
  int TheTgtLang;
  Model * TheModel;
  Dictn * TheDictn;
  Dictn * TheDictnref;
  PrsRbase * ThePrsRbase;
  MakRbase * TheMakRbase;
  
  
  unsigned char Status;
  
  unsigned char Stage;
  unsigned char Direction;
  
  unsigned char Stop;
  unsigned char Pause;
  
  char SourceSentence[SENTLENGTH+1];
  char TargetSentence[SENTLENGTH+1];
  char SourceTree[TREELENGTH+1];
  char TargetTree[TREELENGTH+1];
  char SourceSegment[TREELENGTH+1];
  
  ResultList TheResultList;
  int TheResultNumber;
  
  SrcStruct TheSrcStruct;
  TgtStruct TheTgtStruct;
  
  SrcSgmtPairList TheMatchList;
  
  Unity * TheUnity;
  
  RootList TheRootList;
  SrcRootLink * TheRootLink;
  CplNode  * TheRootNode;
  // int TheRootListProcessedNumber;   // the number of TheRootList ever processed
  
  PrsList ThePrsList;
  NodeUnityStack TheNodeUnityStack;
  SftList TheSftList;
  TrnStack TheTrnStack;
  MakStack TheMakStack;
  
  void setDictn(Dictn * dictn)
  {
    if (Status!=CLOSED)
      error("Translator::setDictn(): the Translator is opened.");
    TheDictn=dictn;
  }
  void setDictnref(Dictn * dictnref)
  {
    if (Status!=CLOSED)
      error("Translator::setDictnref(): the Translator is opened.");
    TheDictnref=dictnref;
  }
  void setPrsRbase(PrsRbase * prsrbase)
  {
    if (Status!=CLOSED)
      error("Translator::setPrsRbase(): the Translator is opened.");
    ThePrsRbase=prsrbase;
  }
  void setMakRbase(MakRbase * makrbase)
  {
    if (Status!=CLOSED)
      error("Translator::setMakRbase(): the Translator is opened.");
    TheMakRbase=makrbase;
  }
  
  virtual void preProcess()=0;
  
  void cutWord0();
  void cutWordBack0();
  virtual void cutWord1()=0;
  virtual void cutWordBack1()=0;
  
  void prsSent();
  void prsSentBack();
  
  void transform0();
  void transformBack0();
  
  void transform1();
  void transformBack1();
  
  void makSent();
  void makSentBack();
  
  void bldWord0();
  void bldWordBack0();
  virtual void bldWord1()=0;
  virtual void bldWordBack1()=0;
  void bldWord2();
  void bldWordBack2();
  
  virtual void postProcess(char * text)=0;
  
  virtual void submit(PtlNode * node);
  virtual CplNode * compare(CplNode * newnode,CplNode * oldnode);
  virtual void killnode(PtlNode * node);
  
  CplNode * createNode1(IToken &token,
			const char *baseform, const char *varyform);
  CplNode * createNode2(SrcSgmt * bgnsgmt,SrcSgmt * endsgmt,
			const char * srclable, const char * tgtlabel);
  
  void prsNode1(CplNode * node);
  void prsNode2(PtlNode * node);
  
  int  findRootList();  /* Find a shortest RootList */
  int  alterRootList(SrcRootLink * rootlink,int failurenumber,int maxfailurenumber);  /* change the node in RootLink */
  SrcRootLink * splitRootList(SrcRootLink * rootlink);  /* Split a Node in RootList */
  
  virtual int isSrcSentence(CplNode * node) { return FALSE; }
  virtual int isTgtSentence(CplNode * node) { return FALSE; }
  virtual int isLegalSrcSect(SrcSect * sect) { return TRUE; }
  
  int trnNode(CplNode * node, const char * &trnpart, const SftLink * sftbott);
  void trnNodeBack(CplNode * node, const char * trnpart, const SftLink * sftbott);
  
  int makNode(CplNode * node, RuleLink * &rulelink, const char * &rightpart, const SftLink * sftbott);
  int makNode0(CplNode * node, RuleLink * &rulelink, const char * &rightpart);
  int makNode1(CplNode * node, RuleLink * &rulelink, const char * &rightpart, const SftLink * sftbott);
  void makNodeBack(CplNode * node, RuleLink * &rulelink, const char * rightpart, const SftLink * sftbott);
  
  int prsReductn(CplNode *rootnode, IToken & token, const char * prsbind);
  PtlNode * prsTree1(CplNode *curnode, const char * prsrule, int ruletype);
  PtlNode * prsTree2(CplNode *curnode, PtlNode * auxnode);
  int prsTree0(PtlNode *rootnode, IToken &token, const char * prsbind);
  int prsForest(PtlNode *rootnode, CplNode *justnode,
		IToken & token, const char * prsbind);
  
  int trnTree(CplNode * rootnode, const char * &trnpart, const SftLink * sftbott);
  int trnSwitch(CplNode * rootnode, const char * &trnpart);
  int trnForest(CplNode * rootnode, CplNode * justnode, IToken & token);
  void trnTreeBack(CplNode * rootnode, const SftLink * sftbott);
  void trnSwitchBack(CplNode * rootnode);
  static int trnUnneedful(CplNode * node);
  
  int makLTree(CplNode * rootnode, const char * rule, const char * &rightpart);
  int makLForest(CplNode * rootnode, CplNode * justnode,
		 IToken & token, const char * bind);
  void makLTreeBack(CplNode * rootnode);
  
  int makRTree(CplNode * rootnode, const char * &rightpart, const SftLink * sftbott);
  int makRForest(CplNode * rootnode, CplNode * justnode, IToken & token);
  void makRTreeBack(CplNode * rootnode, const SftLink * sftbott);
  
  void useNode(PtlNode * node)
  { node->useUnity(); node->setFamily(); TheUnity=node->topUnity(); }
  void useNode(CplNode * node)
  { node->useUnity(); node->setFamily(); TheUnity=node->topUnity(); }
  void initNodeUnity(CplNode * node)
  { TheNodeUnityStack.init(node); TheUnity=node->topUnity(); }
  void pushNodeUnity(CplNode * node)
  { TheNodeUnityStack.push(node); TheUnity=node->topUnity(); }
  void popNodeUnity()
  { TheNodeUnityStack.pop();
    CplLink * link=TheNodeUnityStack.top();
    if (link) TheUnity=link->getNode()->topUnity();
    else TheUnity=TheNodeUnityStack.base()->topUnity(); }
  
  void recordUnity(UnityMark & mark) { TheUnity->record(mark); }
  void recoverUnity(UnityMark & mark) { TheUnity->recover(mark); }
  
  int setSrcHerit(CplNode * node, CplNode * heritnode)
  {
    return (heritnode==NONE)?
      TheUnity->setSrcHerit(node->getFest(),(Fest *)NONE):
      TheUnity->setSrcHerit(node->getFest(),heritnode->getFest());
  }
  static void clrSrcHerit(CplNode * node)
  { Unity::clrSrcHerit(node->getFest()); }
  int setTgtHerit(CplNode * node, CplNode * heritnode)
  {
    return (heritnode==NONE)?
      TheUnity->setTgtHerit(node->getFest(),(Fest *)NONE):
      TheUnity->setTgtHerit(node->getFest(),heritnode->getFest());
  }
  static void clrTgtHerit(CplNode * node)
  { Unity::clrTgtHerit(node->getFest()); }
  
  Atom * getIntValue(CplNode * node, Attribute * attrb)
  { return TheUnity->getIntValue(node->getFest(),attrb); }
  Fest * getExtValue(CplNode * node, Attribute * attrb)
  { return TheUnity->getExtValue(node->getFest(),attrb); }
  
  int unifyNode(CplNode *node1,IToken &token1,CplNode *node2,IToken &token2)
  { return TheUnity->unify(node1->getFest(),token1,node2->getFest(),token2);}
  int unifyNode(CplNode *node1,IToken &token1,IToken &token2)
  { return TheUnity->unify(node1->getFest(),token1,token2); }
  int unifyNode(CplNode *node1,IToken &token2)
  { return TheUnity->unify(node1->getFest(),token2); }
  int unifyNode(CplNode *node1,CplNode *node2)
  { return TheUnity->unify(node1->getFest(),node2->getFest()); }
  
  int unifySrcLabel(CplNode * node, IToken & token);
  int unifyTgtLabel(CplNode * node, IToken & token);
  int unifySrcBaseForm(CplNode * node, IToken & token);
  int unifySrcVaryForm(CplNode * node, IToken & token);
  int unifyTgtBaseForm(CplNode * node, IToken & token);
  int unifyTgtVaryForm(CplNode * node, IToken & token);
  
  int unifyPtlBind(PtlNode * rootnode, IToken &token);
  int unifyCplBind(CplNode * rootnode, IToken &token);
  int unifyBind(CplNode * rootnode, IToken & token);
  int doBind(PtlNode * rootnode,IToken & token);

  static int ignorePtlBind(PtlNode * rootnode, IToken &token);
  static int ignoreCplBind(CplNode * rootnode, IToken &token);
  static int isBind(IToken & token);
  static void skipBind(IToken & token);
  
#ifdef KBMS
  
  SpyStruct TheSpyStruct;
  //char * SpyFileName;
  //fstream SpyFile;
  
  int WchTrnFail;
  //char * WchFileName;
  fstream WchFile;
  
  void output_spyBind(PtlNode * node,IToken & token);
  void output_killnode(CplNode * node);
  void output_RootList();
  void output_SrcRootNode(CplNode * node,int depth=1);
  void output_TgtRootNode(CplNode * node,int depth=1);
  void output_prsCplTree(CplNode * node);
  void output_prsPtlTree(PtlNode * node);
  void output_limitsametypenode(PtlNode * node);
  void output_trnTree(CplNode * node);
  void output_makTree(CplNode * node);
  void output_cutNode(CplNode * node);
  void output_prsPtlNode(PtlNode * node);
  void output_prsCplNode(CplNode * node);
  void output_trnNode(CplNode * node);
  void output_trnNodeSucc(CplNode * node);
  void output_trnNodeFail(CplNode * node,IToken & token=*(IToken *)NULL);
  void output_makNode(CplNode * node, const char * makrule);
  void output_bldWordFail(CplNode * node,const char * form);
  void output_sftWordFail(SftNode * sftnode);
  void output_sftWord(SftNode * sftnode,CplNode * sftfrom);
  void output_bldNode(TgtSgmt * sgmt, const char * bldrule);
  
#endif
  
 public:
  
  Translator() { init(); }
  ~Translator() { free(); }
  
  void init();
  void free();
  
  void reset();
  
  virtual int hasSrcLang()=0;
  virtual int hasTgtLang()=0;
  
  virtual void open()
  {
    if (Status!=CLOSED) return;
    if (Opened==1) error("Another Translator is opened.");
    
    Status=ROPEN;
    Opened=1;
    UseDictnref=0;
    UseLexdbase=0;
    
    Stage=BEGIN;
    Stop=FALSE;
    Pause=FALSE;
    Direction=NOACTION;
    
    TheSrcLang=hasSrcLang();
    TheTgtLang=hasTgtLang();
    
    if (TheSrcLang<0)
      error("Translator::open(): Illegal SrcLang.");
    if (TheTgtLang<0)
      error("Translator::open(): Illegal TgtLang.");
    if (TheSrcLang==TheTgtLang)
      error("Translator::open(): SrcLang is equal to TgtLang.");
    
    TheModel->use();
    TheDictn->use();
    TheDictnref->use();
    ThePrsRbase->use();
    TheMakRbase->use();
    
    if (!TheModel || TheModel->getBuilt()==NO)
      error("Translator::open(): Model not open.");
    if (!TheModel->hasLang(TheSrcLang))
      error("Translator::open(): SrcLang is not in Model.");
    if (!TheModel->hasLang(TheTgtLang))
      error("Translator::open(): TgtLang is not in Model.");
  }
  
  virtual void close()
  {
    if (Status==CLOSED) return;
    
    Status=CLOSED;
    Opened=0;
    
    TheMakRbase->unuse();
    ThePrsRbase->unuse();
    TheDictnref->unuse();
    TheDictn->unuse();
    TheModel->unuse();
    free();
    Stage=BEGIN;
    Stop=FALSE;
    Pause=FALSE;
    Direction=NOACTION;
  }
  
  Model * getModel() { return TheModel; }
  Dictn * getDictn() { return TheDictn; }
  Dictn * getDictnref() { return TheDictnref; }
  PrsRbase * getPrsRbase() { return ThePrsRbase; }
  MakRbase * getMakRbase() { return TheMakRbase; }
  
  int getStatus() { return Status; }
  int getStage() { return Stage; }
  int getDirection() { return Direction; }
  int getStop() { return Stop; }
  int getPause() { return Pause; }
  const char * getSourceSentence() { return SourceSentence; }
  const char * getTargetSentence() { return TargetSentence; }
  const char * getSourceTree() { return SourceTree; }
  const char * getTargetTree() { return TargetTree; }
  const char * getSourceSegment() { return SourceSegment; }
  ResultList & getResultList() { return TheResultList; }
  const SrcStruct & getSrcStruct() { return TheSrcStruct; }
  const TgtStruct & getTgtStruct() { return TheTgtStruct; }
  const CplNode * getRootNode() { return TheRootNode; }
  const TrnStack & getTrnStack() { return TheTrnStack; }
  const MakStack & getMakStack() { return TheMakStack; }
  
  void setModel(Model * model)
  {
    if (Status!=CLOSED)
      error("Translator::setModel(): the Translator is opened.");
    TheModel=model;
  }
  
  void setSourceSentence(char * text);
  void clrSourceSentence();
  int  makTargetSentence();
  void makSourceSegment();
  int  makSourceParsing();
  void makSourceParsingTree(CplNode * node,
			    OToken &ttext,OToken &ttree,OToken &stree);
  
  void stop() { Stop=TRUE; }
  void palse() { Pause=TRUE; }
  void resume() { Pause=FALSE; }
  virtual int  translateSentence( int endstage=END );
  virtual void translateSentenceBack();
  
#ifdef KBMS
  
  void openSpy(char * spyfilename, char * wchfilename)
  {
    if (spyfilename && *spyfilename)
      {
	TheSpyStruct.read(spyfilename);
      }
    if (wchfilename && *wchfilename)
      {
	WchFile.open(wchfilename,ios::out|ios::trunc);
	if (!WchFile.is_open()) error("Translator::open(): WchFile open error.");
      }
  }
  void closeSpy()
  {
    TheSpyStruct.free();
    if (WchFile.is_open()) WchFile.close();
  }
  //void setSpyFileName(char * spyfilename) { SpyFileName=spyfilename; }
  //void setWchFileName(char * wchfilename) { WchFileName=wchfilename; }
  
#endif
  
};

#endif /* TRANS_H */

