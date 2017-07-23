
#ifndef UNIFY_H

#define UNIFY_H

#include "../kbase/model.h"

class AtomNew;
class AtomMap;
class AtomCut;
class FestNew;
class FestMap;
class FestCut;
class UnityMark;
class UnityLink;
class UnityList;
class Unity;
class UnityElem;
class UnityStack;
class Atom;
class Hierar;
class Symbol;
class Number;
class Boolean;
class Fest;
class cIntFest;
class IntFeat;
class cExtFest;
class ExtFeat;

extern int GlobalSrcLang;
extern int GlobalTgtLang;
extern Model * GlobalModel;

static void * NONE= (void *)(-1);

class AtomMap
{
	Atom * MapFrom;
	Atom * MapTo;
	AtomMap * Next;

  public:

	AtomMap() { MapFrom=MapTo=NULL; Next=NULL; }

	friend class Unity;
};

class AtomNew
{
	Atom * New;
	AtomNew * Next;

  public:

	AtomNew() { New=NULL; Next=NULL; }

	friend class Unity;
};

class AtomCut
{
	IntFeat * Cut;
	AtomCut * Next;

  public:

	AtomCut() { Cut=NULL; Next=NULL; }

	friend class Unity;
};

class FestMap
{
	Fest * MapFrom;
	Fest * MapTo;
	FestMap * Next;

  public:

	FestMap() { MapFrom=MapTo=NULL; Next=NULL; }

	friend class Unity;
};

class FestNew
{
	Fest * New;
	FestNew * Next;

  public:

	FestNew() { New=NULL; Next=NULL; }

	friend class Unity;
};

class FestCut
{
	ExtFeat * Cut;
	FestCut * Next;

  public:

	FestCut() { Cut=NULL; Next=NULL; }

	friend class Unity;
};

class UnityLink
{
	Unity * TheUnity;
	UnityLink * Next;

  public:

	UnityLink() { TheUnity=NULL; Next=NULL; }

	Unity * getUnity() { return TheUnity; }
	UnityLink * getNext() { return Next; }

	friend class UnityList;
};

class UnityList
{
	UnityLink * FstLink;

  public:

	UnityList() { FstLink=NULL; }
	~UnityList() { free(); }

	void free();

	UnityLink * getFstLink() { return FstLink; }
	void add(Unity * unity)
	{
		UnityLink * link=new UnityLink;
		link->TheUnity=unity;
		link->Next=FstLink;
		FstLink=link;
	}
	Unity * del()
	{
		if (FstLink) error("UnityList::del(): Nothing to delete.");
		UnityLink * link=FstLink;
		FstLink=link->Next;
		Unity * unity=link->TheUnity;
		delete link;
		return unity;
	}
};

enum
{
	SRCLEXSTA=0x01, SRCLEXDYN=0x02, SRCLEX=0x03,
	SRCPHRSTA=0x04, SRCPHRDYN=0x08, SRCPHR=0x0C,
	SRCSTA=0x05,	SRCDYN=0x0A,	SRC=0x0F,
	TGTLEXSTA=0x10, TGTLEXDYN=0x20, TGTLEX=0x30,
	TGTPHRSTA=0x40, TGTPHRDYN=0x80, TGTPHR=0xC0,
	TGTSTA=0x50,	TGTDYN=0xA0,	TGT=0xF0,
	SEMSTA=0x100,	SEMDYN=0x200,	SEM=0x300
};

class UnityMark
{
	AtomMap * FstAtomMap;
	AtomNew	* FstAtomNew;
	AtomCut	* FstAtomCut;
	FestMap * FstFestMap;
	FestNew	* FstFestNew;
	FestCut	* FstFestCut;

	friend class Unity;
};

enum { UNIFY, SHARE, SEVER };

class Unity
{

	static int DefaultSwitch;

	AtomMap * FstAtomMap;
	AtomNew	* FstAtomNew;
	AtomCut	* FstAtomCut;
	FestMap * FstFestMap;
	FestNew	* FstFestNew;
	FestCut	* FstFestCut;

	UnityList Children;

	Unity * Parent;

	int Valid;

  public:

	static int getDefaultSwitch() { return DefaultSwitch; }
	static void setDefaultSwitch(int s) { DefaultSwitch=s; }
	static int testDefaultSwitch(Attribute * attrb);

	Unity()
	{
		FstAtomMap=NULL;
		FstAtomNew=NULL;
		FstAtomCut=NULL;
		FstFestMap=NULL;
		FstFestNew=NULL;
		FstFestCut=NULL;
		Parent=NULL;
		Valid=NO;
	}
	~Unity() { free(); }

	void free();

	UnityList & getChildren() { return Children; }
	Unity * getParent() { return Parent; }
	int getValid() { return Valid; }

	void addChild(Unity * unity)
	{
		if (FstAtomMap||FstAtomNew||FstAtomCut||FstFestMap||FstFestNew||FstFestCut)
			error("Unity::addChild(): cannot addChild while not empty.");
		if (Valid) { unity->use(); unity->Parent=this; }
		Children.add(unity);
	}

	void use();
	void unuse();

	void addAtomMap(Atom * mapfrom, Atom * mapto);
	void addFestMap(Fest * mapfrom, Fest * mapto);

	void addAtomNew(Atom * atom);
	void addFestNew(Fest * fest);
	int hasAtomNew(Atom * atom);
	int hasFestNew(Fest * fest);

	void addAtomCut(IntFeat * cut);
	void addFestCut(ExtFeat * cut);

	int setSrcHerit(Fest * fest, Fest * herit);
	static void clrSrcHerit(Fest * fest);
	int setTgtHerit(Fest * fest, Fest * herit);
	static void clrTgtHerit(Fest * fest);

	Atom * getIntValue(Fest * fest, Attribute * attrb, BOOL get_herit_default=TRUE);
	Fest * getExtValue(Fest * fest, Attribute * attrb, BOOL get_herit_default=TRUE);

	int unify(Fest *fest1,IToken &token1,Fest *fest2,IToken &token2);
	int unify(Fest *fest1,Fest *fest2,IToken &token2);
	int unify(Fest *fest1,Fest *fest2);
	int unify(Atom *atom1,Fest *fest2,IToken &token2);
	int unify(Atom *atom1,Atom *atom2);
	int unify(Fest *fest1,IToken &token1,IToken &token2);
	int unify(Fest *fest1,IToken &token2);
	int unify(Atom *atom1,IToken &token2);
	int share(Fest *fest1,Fest *fest2,IToken &token2);
	int sever(Fest *fest1,Fest *fest2,IToken &token2);

	int _unify(Atom *atom1,Atom *atom2);
	int _unify(Atom *atom1,IToken &token2);

	int _unify(Fest *fest1,Fest *fest2);
	int _unify(Fest *fest1,IToken &token2);

	void record(UnityMark & mark)
	{
		mark.FstAtomMap=FstAtomMap;
		mark.FstAtomNew=FstAtomNew;
		mark.FstAtomCut=FstAtomCut;
		mark.FstFestMap=FstFestMap;
		mark.FstFestNew=FstFestNew;
		mark.FstFestCut=FstFestCut;
	}
	void recover(UnityMark & mark);

#ifdef KBMS
	void output_atom(ostream &o,Atom * atom,Attribute * attr);
	void output_fest(ostream &o,Fest * fest,int language);
#endif

};

class UnityElem : public Unity
{
	UnityElem * Next;

  public:

	UnityElem() { Next=NULL; }

	friend class UnityStack;
};

class UnityStack
{
	UnityElem * TopElem;

  public:

	UnityStack() { TopElem=NULL; }
	~UnityStack() { free(); }

	void free();

	void push()
	{
		UnityElem * elem=new UnityElem;
		elem->Next=TopElem;
		TopElem=elem;
	}
	void pop()
	{
		if (!TopElem) error("UnityStack::pop(): underflow.");
		UnityElem * elem=TopElem;
		TopElem=elem->Next;
		delete elem;
	}
	UnityElem * top() { return TopElem; }

	void moveFrom(UnityStack & stack)
	{
		if (TopElem) error("UnityStack::moveFrom(): move to a unempty UnityStack.");
		TopElem=stack.TopElem;
		stack.TopElem=NULL;
	}
};

class Atom
{

  protected:

	char * Nucleus;
	Unity * MapBy;
	Atom * MapTo;
	Unity * BelongTo;

	virtual void unify_Nucleus(OToken &token0, IToken &token1, IToken &token2)=0;
	virtual Atom * newAtom()=0;

  public:

	Atom() { Nucleus=NULL; MapBy=NULL; MapTo=NULL; BelongTo=NULL; }
	~Atom() { free(); }

	void free() { if (Nucleus) delete Nucleus; Nucleus=NULL; }

	virtual int hasType()=0;

	const char * getNucleus() { return Nucleus; }
	Unity * getMapBy() { return MapBy; }
	Atom * getMapTo() { return MapTo; }
	Unity * getBelongTo() { return BelongTo; }

	static void map( Atom * &atom )
	{
		while (atom->MapBy && atom->MapBy->getValid()==YES &&
			   atom->MapTo!=atom && atom->MapTo!=NONE) atom=atom->MapTo;
	}

	int isEmpty() { return Nucleus==NULL; }

	int isNewBy(Unity * unity) { return BelongTo==unity; }

	static int isTrue(const char *s)
		{ return !strcmp(s,"YES")||!strcmp(s,"Yes")||!strcmp(s,"ÊÇ"); }

	static int isFalse(const char *s)
		{ return !strcmp(s,"NO")||!strcmp(s,"No")||!strcmp(s,"·ñ"); }

	int isTrue() { return hasType()==MTBOOL && isTrue(Nucleus); }

	int isFalse() { return hasType()==MTBOOL && isFalse(Nucleus); }

	static int leadBy(IToken &token)
		{ return token.isDelim("~") || token.isIdent() || token.isInteg(); }
	virtual void inputFrom(IToken &token)=0;
	static void skipOver(IToken &token);

	friend class Fest;
	friend class Unity;
};

#ifdef KBMS

typedef Attribute * AttrPtr;
//IOMANIPdeclare(AttrPtr);
//OMANIP(AttrPtr) setAttr(AttrPtr attr);

#endif

class Hierar : public Atom
{

  protected:

	void unify_Nucleus(OToken & token0, IToken & token1, IToken & token2)
		{ unify_Hierar(token0,token1,token2); }
	Atom * newAtom() { return new Hierar; }

  public:

	int hasType() { return HIERAR; }
	void inputFrom(IToken &token);
};

class Symbol : public Atom
{

  protected:

	void unify_Nucleus(OToken & token0, IToken & token1, IToken & token2)
		{ unify_Symbol(token0,token1,token2); }
	Atom * newAtom() { return new Symbol; }

  public:

	int hasType() { return SYMBOL; }
	void inputFrom(IToken &token);
};

class Number : public Atom
{

  protected:

	void unify_Nucleus(OToken & token0, IToken & token1, IToken & token2)
		{ unify_Number(token0,token1,token2); }
	Atom * newAtom() { return new Number; }

  public:

	int hasType() { return NUMBER; }
	void inputFrom(IToken &token);
};

class Boolean : public Atom
{

  protected:

	void unify_Nucleus(OToken & token0, IToken & token1, IToken & token2)
		{ unify_Boolean(token0,token1,token2); }
	Atom * newAtom() { return new Boolean; }

  public:

	int hasType() { return MTBOOL; }
	void inputFrom(IToken &token);
};

class cIntFest
{
  IntFeat * FstFeat;
  IntFeat * LstFeat;
  
 public:
  
  cIntFest() { FstFeat=LstFeat=NULL; }
  
  friend class Fest;
  friend class Unity;
};

class IntFeat
{
	Attribute * Attrb;
	Atom * Value;
	Unity * CutBy;
	IntFeat * Next;

  public:

	IntFeat() { Attrb=NULL; Value=NULL; CutBy=NULL; Next=NULL; }

	friend class IntFest;
	friend class Fest;
	friend class Unity;
};

class cExtFest
{
  ExtFeat * FstFeat;
  ExtFeat * LstFeat;
  
 public:
  
  cExtFest() { FstFeat=LstFeat=NULL; }
  
  friend class Fest;
  friend class Unity;
};

class ExtFeat
{
	Attribute * Attrb;
	Fest * Value;
	Unity * CutBy;
	ExtFeat * Next;

  public:

	ExtFeat() { Attrb=NULL; Value=NULL; CutBy=NULL; Next=NULL; }

	friend class cExtFest;
	friend class Fest;
	friend class Unity;
};

class Fest
{
	Fest * SrcHerit;
	Fest * TgtHerit;
	Fest * MapTo;
	Unity * MapBy;
	Unity * BelongTo;

	cIntFest IntFest;
	cExtFest ExtFest;

  public:

	Fest() { SrcHerit=TgtHerit=MapTo=NULL; MapBy=BelongTo=NULL; }
	~Fest() { free(); }

	void free();

	Fest * getSrcHerit() { return SrcHerit; }
	Fest * getTgtHerit() { return TgtHerit; }
	Unity * getMapBy() { return MapBy; }
	Fest * getMapTo() { return MapTo; }
	Unity * getBelongTo() { return BelongTo; }

	static void map( Fest * &fest )
	{
		while (fest->MapBy && fest->MapBy->getValid()==YES &&
			   fest->MapTo!=fest && fest->MapTo!=NONE) fest=fest->MapTo;
	}

	IntFeat * getIntFeat(Attribute * attrb);
	IntFeat * addIntFeat(Attribute * attrb, Atom * value);

	ExtFeat * getExtFeat(Attribute * attrb);
	ExtFeat * addExtFeat(Attribute * attrb, Fest * value);

	int isEmpty() { return !IntFest.FstFeat && !ExtFest.FstFeat; }
	int isNewBy(Unity * unity) { return BelongTo==unity; }
	int isWhole() { return (SrcHerit==NULL || SrcHerit==NONE)
						&& (TgtHerit==NULL || TgtHerit==NONE); }

	int isNone() { return IntFest.FstFeat==NONE; }
	static int isNone(const char * p) 
		{ return !strcmp(p,"ÎÞ") || !strcmp(p,"one") || !strcmp(p,"NONE"); }

	static int leadBy(IToken &token)
		{ return token.isDelim("[") || token.isDelim("{") || isNone(token); }
	void inputFrom(IToken &token);
	static void skipOver(IToken &token);

	friend class Unity;
};

inline void Unity::clrSrcHerit(Fest * fest) { fest->SrcHerit=NULL; }
inline void Unity::clrTgtHerit(Fest * fest) { fest->TgtHerit=NULL; }

//#ifdef KBMS
//OMANIP (int) setLang(int lang);
//#endif


#endif

