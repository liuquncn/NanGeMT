#include "unify.h"

enum { EQUALAGREE, SUPERAGREE, SUFERAGREE, DISAGREE };

void Atom::skipOver(IToken &token)
{
	if (token.isIdent()||token.isInteg()) token.input();
	while (token.isDelim("~")) { token.input(); token.input(); }
	while (token.isDelim("|")) { token.input(); token.input(); }
}

void Hierar::inputFrom(IToken &itoken)
{
	if (!isEmpty()) error("Hierar::inputFrom(): input to a non-empty atom.");
	char result[ATOMLENGTH];
	OToken otoken(result);
	while (TRUE)
	{
		if (itoken.isIdent()) { otoken.output(itoken); itoken.input(); }
		while (itoken.isDelim("~"))
		{
			itoken.input();
			otoken.output("~");
			otoken.output(itoken);
			itoken.input();
		}
		if (itoken.isDelim("|")) { otoken.output("|"); itoken.input(); }
		else break;
	}
	if (otoken.getOutPntr()==result)
		error("Hierar::inputFrom(): nothing to input.",itoken.getInPntr());
	otoken.outputNull();
	int length=otoken.getOutPntr()-result;
	if (length>=ATOMLENGTH) error("Hierar::inputFrom(): Atom too long.",result);
	Nucleus=new char[length];
	strcpy(Nucleus,result);
}

void Symbol::inputFrom(IToken &itoken)
{
	if (!isEmpty()) error("Symbol::inputFrom(): input to a non-empty atom.");
	char result[ATOMLENGTH];
	OToken otoken(result);
	if (itoken.isDelim("~"))
		while (itoken.isDelim("~"))
		{
			itoken.input();
			otoken.output("~");
			otoken.output(itoken);
			itoken.input();
		}
	else
	{
		otoken.output(itoken);
		itoken.input();
		while(itoken.isDelim("|"))
		{
			itoken.input();
			otoken.output("|");
			otoken.output(itoken);
			itoken.input();
		}
	}
	if (otoken.getOutPntr()==result)
		error("Symbol::inputFrom(): nothing to input.",itoken.getInPntr());
	otoken.outputNull();
	int length=otoken.getOutPntr()-result;
	if (length>=ATOMLENGTH) error("Symbol::inputFrom(): Atom too long.",result);
	Nucleus=new char[length];
	strcpy(Nucleus,result);
}

void Number::inputFrom(IToken &itoken)
{
	if (!isEmpty()) error("Number::inputFrom(): input to a non-empty atom.");
	char result[ATOMLENGTH];
	OToken otoken(result);
	if (itoken.isDelim("~"))
		while (itoken.isDelim("~"))
		{
			itoken.input();
			otoken.output("~");
			otoken.output(itoken);
			itoken.input();
		}
	else
	{
		otoken.output(itoken);
		itoken.input();
		while(itoken.isDelim("|"))
		{
			itoken.input();
			otoken.output("|");
			otoken.output(itoken);
			itoken.input();
		}
	}
	if (otoken.getOutPntr()==result)
		error("Number::inputFrom(): nothing to input.",itoken.getInPntr());
	otoken.outputNull();
	int length=otoken.getOutPntr()-result;
	if (length>=ATOMLENGTH) error("Number::inputFrom(): Atom too long.",result);
	Nucleus=new char[length];
	strcpy(Nucleus,result);
}

void Boolean::inputFrom(IToken &itoken)
{
	if (!isEmpty()) error("Boolean::inputFrom(): input to a non-empty atom.");
	char result[ATOMLENGTH];
	OToken otoken(result);
	otoken.output(itoken);
	itoken.input();
	if (otoken.getOutPntr()==result)
		error("Number::inputFrom(): nothing to input.",itoken.getInPntr());
	otoken.outputNull();
	int length=otoken.getOutPntr()-result;
	if (length>=ATOMLENGTH) error("Boolean::inputFrom(): Atom too long.",result);
	Nucleus=new char[length];
	strcpy(Nucleus,result);
}

void Fest::free()
{
	if (isNone()) return;
	IntFeat * intfeat=IntFest.FstFeat;
	if (intfeat!=NONE)
	{
		while (intfeat)
		{
			Atom * intvalue=intfeat->Value;
			IntFeat * intnext=intfeat->Next;
			delete intvalue;
			delete intfeat;
			intfeat=intnext;
		}
	}
	IntFest.FstFeat=IntFest.LstFeat=NULL;
	ExtFeat * extfeat=ExtFest.FstFeat;
	if (extfeat!=NONE)
	{
		while (extfeat)
		{
			Fest * extvalue=extfeat->Value;
			ExtFeat * extnext=extfeat->Next;
			delete extvalue;
			delete extfeat;
			extfeat=extnext;
		}
	}
	ExtFest.FstFeat=ExtFest.LstFeat=NULL;
}

IntFeat * Fest::addIntFeat(Attribute * attrb, Atom * value)
{
	IntFeat * intfeat=new IntFeat;
	intfeat->Attrb=attrb;
	intfeat->Value=value;
	value->BelongTo=BelongTo;
	if (!IntFest.LstFeat) IntFest.FstFeat=IntFest.LstFeat=intfeat;
	else { IntFest.LstFeat->Next=intfeat; IntFest.LstFeat=intfeat; }
	return intfeat;
}

ExtFeat * Fest::addExtFeat(Attribute * attrb, Fest * value)
{
	ExtFeat * extfeat=new ExtFeat;
	extfeat->Attrb=attrb;
	extfeat->Value=value;
	value->BelongTo=BelongTo;
	if (!ExtFest.LstFeat) ExtFest.FstFeat=ExtFest.LstFeat=extfeat;
	else { ExtFest.LstFeat->Next=extfeat; ExtFest.LstFeat=extfeat; }
	return extfeat;
}

IntFeat * Fest::getIntFeat(Attribute * attrb)
{
	IntFeat * intfeat=IntFest.FstFeat;
	while ( intfeat && intfeat!=NONE && intfeat->Attrb!=attrb )
		intfeat=intfeat->Next;
	return intfeat;
}

ExtFeat * Fest::getExtFeat(Attribute * attrb)
{
	ExtFeat * extfeat=ExtFest.FstFeat;
	while ( extfeat && extfeat!=NONE && extfeat->Attrb!=attrb )
		extfeat=extfeat->Next;
	return extfeat;
}

void Fest::inputFrom(IToken &token)
{
	if (!isEmpty() || !isWhole())
		error("Fest::inputFrom(): Input to a non-empty Fest.",token.getInPntr());
	if (Fest::isNone(token))
	{
		token.input();
		IntFest.FstFeat=(IntFeat *)NONE;
		ExtFest.FstFeat=(ExtFeat *)NONE;
		return;
	}
	if (token.isDelim("["))
	{
		token.input();
		if (!token.isDelim("]"))
		{
			token.redrawin();
			while (!token.isDelim("]"))
			{
				token.input();
				Attribute * attrb=GlobalModel->getAttribute(token);
				if (!attrb || attrb->getIntExt()!=INTERNAL)
					error("Fest::inputFrom(): need internal attribute.",token);
				if (getIntFeat(attrb))
					error("Fest::inputFrom(): duplicate internal attribute in fest.",token);
				Atom * atom;
				switch (attrb->getAttType())
				{
					case HIERAR: atom=new Hierar(); break;
					case SYMBOL: atom=new Symbol(); break;
					case NUMBER: atom=new Number(); break;
					case MTBOOL: atom=new Boolean(); break;
				}
				addIntFeat(attrb,atom);
				token.input();
				token.input();
				atom->inputFrom(token);
			}
		}
		token.input();
	}
	if (token.isDelim("{"))
	{
		token.input();
		if (!token.isDelim("}"))
		{
			token.redrawin();
			while (!token.isDelim("}"))
			{
				token.input();
				Attribute * attrb=GlobalModel->getAttribute(token);
				if (!attrb || attrb->getIntExt()!=EXTERNAL)
					error("Fest::inputFrom(): need external attribute.",token);
				if (getExtFeat(attrb))
					error("Fest::inputFrom(): duplicate external attribute in fest.",token);
				Fest * fest=new Fest;
				addExtFeat(attrb,fest);
				token.input();
				token.input();
				fest->inputFrom(token);
			}
		}
		token.input();
	}
}

void Fest::skipOver(IToken &token)
{
	if (isNone(token)) { token.input(); return; }
	if (token.isDelim("[")) { token.gouptomatch(); token.input(); }
	if (token.isDelim("{")) { token.gouptomatch(); token.input(); }
}

int Unity::DefaultSwitch;

int Unity::testDefaultSwitch(Attribute * attrb)
{
	if (attrb->getLang()==GlobalSrcLang)
	{
		if (attrb->getLexPhr()==LEXICAL)
		{
			if (attrb->getStaDyn()==STATIC)
				return DefaultSwitch & SRCLEXSTA;
			else // (attrb->getStaDyn()==DYNAMIC)
				return DefaultSwitch & SRCLEXDYN;
		}
		else // (attrb->getLexPhr()==PHRASAL)
		{
			if (attrb->getStaDyn()==STATIC)
				return DefaultSwitch & SRCPHRSTA;
			else // (attrb->getStaDyn()==DYNAMIC)
				return DefaultSwitch & SRCPHRDYN;
		}
	}
	else if (attrb->getLang()==GlobalTgtLang)
	{
		if (attrb->getLexPhr()==LEXICAL)
		{
			if (attrb->getStaDyn()==STATIC)
				return DefaultSwitch & TGTLEXSTA;
			else // (attrb->getStaDyn()==DYNAMIC)
				return DefaultSwitch & TGTLEXDYN;
		}
		else // (attrb->getLexPhr()==LEXICAL)
		{
			if (attrb->getStaDyn()==STATIC)
				return DefaultSwitch & TGTPHRSTA;
			else // (attrb->getStaDyn()==DYNAMIC)
				return DefaultSwitch & TGTPHRDYN;
		}
	}
	else // (attrb->getLang()==SEMANTIC)
	{
		if (attrb->getStaDyn()==STATIC)
			return DefaultSwitch & SEMSTA;
		else // (attrb->getStaDyn()==DYNAMIC)
			return DefaultSwitch & SEMDYN;
	}
}

void Unity::free()
{
	if (Parent) error("Unity::free(): free with valid parent.");
	if (Valid) { unuse(); }

	AtomCut * atomcut;
	for (atomcut=FstAtomCut;atomcut;)
		{ AtomCut * next=atomcut->Next; delete atomcut; atomcut=next; }
	FstAtomCut=NULL;

	AtomMap * atommap;
	for (atommap=FstAtomMap;atommap;)
		{ AtomMap * next=atommap->Next; delete atommap; atommap=next; }
	FstAtomMap=NULL;

	AtomNew * atomnew;
	for (atomnew=FstAtomNew;atomnew;)
	{
		AtomNew * next=atomnew->Next;
		delete atomnew->New;
		delete atomnew;
		atomnew=next;
	}
	FstAtomNew=NULL;

	FestCut * festcut;
	for (festcut=FstFestCut;festcut;)
		{ FestCut * next=festcut->Next; delete festcut; festcut=next; }
	FstFestCut=NULL;

	FestMap * festmap;
	for (festmap=FstFestMap;festmap;)
		{ FestMap * next=festmap->Next; delete festmap; festmap=next; }
	FstFestMap=NULL;

	FestNew * festnew;
	for (festnew=FstFestNew;festnew;)
	{
		FestNew * next=festnew->Next;
		delete festnew->New;
		delete festnew;
		festnew=next;
	}
	FstFestNew=NULL;
}

void Unity::addAtomMap(Atom *mapfrom, Atom *mapto)
{
	AtomMap * atommap=new AtomMap;
	atommap->MapFrom=mapfrom;
	atommap->MapTo=mapto;
	atommap->Next=FstAtomMap;
	FstAtomMap=atommap;
	mapfrom->MapBy=this;
	mapfrom->MapTo=mapto;
}

void Unity::addAtomNew(Atom *atom)
{
	AtomNew * atomnew = new AtomNew;
	atomnew->New=atom;
	atomnew->Next=FstAtomNew;
	atom->BelongTo=this;
	FstAtomNew=atomnew;
}

int  Unity::hasAtomNew(Atom *atom)
{
	AtomNew * atomnew = FstAtomNew;
	while (atomnew)
		{ if (atomnew->New==atom) return TRUE; else atomnew=atomnew->Next; }
	return FALSE;
}

void Unity::addAtomCut(IntFeat * cut)
{
	AtomCut * atomcut = new AtomCut;
	atomcut->Cut=cut;
	atomcut->Next=FstAtomCut;
	if (cut->CutBy==NULL || cut->CutBy->Valid==NO) cut->CutBy=this;
	FstAtomCut=atomcut;
}

void Unity::addFestMap(Fest *mapfrom, Fest *mapto)
{
	if (mapfrom->SrcHerit && mapfrom->SrcHerit!=mapto->SrcHerit)
		error("Unity::addFestMap(): map a fest to another with a different SrcHerit.");
	if (mapfrom->TgtHerit && mapfrom->TgtHerit!=mapto->TgtHerit)
		error("Unity::addFestMap(): map a fest to another with a different TgtHerit.");
	FestMap * festmap=new FestMap;
	festmap->MapFrom=mapfrom;
	festmap->MapTo=mapto;
	festmap->Next=FstFestMap;
	FstFestMap=festmap;
	mapfrom->MapBy=this;
	mapfrom->MapTo=mapto;
}

void Unity::addFestNew(Fest *fest)
{
	FestNew * festnew = new FestNew;
	festnew->New=fest;
	festnew->Next=FstFestNew;
	fest->BelongTo=this;
	FstFestNew=festnew;
}

int  Unity::hasFestNew(Fest *fest)
{
	FestNew * festnew = FstFestNew;
	while (festnew)
		{ if (festnew->New==fest) return TRUE; else festnew=festnew->Next; }
	return FALSE;
}

void Unity::addFestCut(ExtFeat * cut)
{
	FestCut * festcut = new FestCut;
	festcut->Cut=cut;
	festcut->Next=FstFestCut;
	if (cut->CutBy==NULL || cut->CutBy->Valid==NO) cut->CutBy=this;
	FstFestCut=festcut;
}

void Unity::use()
{
	if (Valid) { if (Parent) { Parent->unuse(); Parent=NULL; } }
	else
	{
		Valid=YES;
		UnityLink * link;
		for (link=Children.getFstLink();link;link=link->getNext())
			{ link->getUnity()->use(); link->getUnity()->Parent=this; }
		AtomMap * atommap;
		for (atommap=FstAtomMap;atommap;atommap=atommap->Next)
		{
			if (atommap->MapFrom->MapBy==this) continue;
			atommap->MapFrom->MapTo=atommap->MapTo;
			atommap->MapFrom->MapBy=this;
			AtomMap * nextmap=atommap->Next;
		}
		AtomCut * atomcut;
		for (atomcut=FstAtomCut;atomcut;atomcut=atomcut->Next)
		{
			if (atomcut->Cut->CutBy==NULL || atomcut->Cut->CutBy->Valid==NO)
				atomcut->Cut->CutBy=this;
		}
		FestMap * festmap;
		for (festmap=FstFestMap;festmap;festmap=festmap->Next)
		{
			if (festmap->MapFrom->MapBy==this) continue;
			festmap->MapFrom->MapTo=festmap->MapTo;
			festmap->MapFrom->MapBy=this;
			FestMap * nextmap=festmap->Next;
		}
		FestCut * festcut;
		for (festcut=FstFestCut;festcut;festcut=festcut->Next)
		{
			if (festcut->Cut->CutBy==NULL || festcut->Cut->CutBy->Valid==NO)
				festcut->Cut->CutBy=this;
		}
	}
}

void Unity::unuse()
{
	if (!Valid) return;
	if (Parent) { Parent->unuse(); Parent=NULL; }
	AtomMap * atommap;
	for (atommap=FstAtomMap;atommap;atommap=atommap->Next)
		{ if (atommap->MapFrom->MapBy==this) atommap->MapFrom->MapBy=NULL; }
	AtomCut * atomcut;
	for (atomcut=FstAtomCut;atomcut;atomcut=atomcut->Next)
		{ if (atomcut->Cut->CutBy==this) atomcut->Cut->CutBy=NULL; }
	FestMap * festmap;
	for (festmap=FstFestMap;festmap;festmap=festmap->Next)
		{ if (festmap->MapFrom->MapBy==this) festmap->MapFrom->MapBy=NULL; }
	FestCut * festcut;
	for (festcut=FstFestCut;festcut;festcut=festcut->Next)
		{ if (festcut->Cut->CutBy==this) festcut->Cut->CutBy=NULL; }
	Valid=NO;
	UnityLink * childlink=Children.getFstLink();
	for (;childlink;childlink=childlink->getNext())
		childlink->getUnity()->Parent=NULL;
}

int Unity::setSrcHerit(Fest * fest, Fest * herit)
{
	if (fest->getSrcHerit()==herit) return SUCC;
	if (fest->getSrcHerit()) error("Unity::setSrcHerit(): set Duplicate SrcHerit.");
	if (herit==NULL || herit!=NONE && herit->SrcHerit==NULL)
		error("Unity::setSrcHerit(): set NULL SrcHerit.");
	if (herit!=NONE)
	{
		IntFeat * intfeat;
		for (intfeat=fest->IntFest.FstFeat;intfeat;intfeat=intfeat->Next)
		{
			Attribute * attrb=intfeat->Attrb;
			Atom * value=intfeat->Value; Atom::map(value);
			if (attrb->getLang()!=SEMANTIC &&
				attrb->getLang()!=GlobalSrcLang) continue;
			if (attrb->getRelative()==IRRELATIVE) continue;
			if (value->isEmpty()) continue;
			if (unify(value,getIntValue(herit,attrb))==FAIL) return FAIL;
		}
		ExtFeat * extfeat;
		for (extfeat=fest->ExtFest.FstFeat;extfeat;extfeat=extfeat->Next)
		{
			Attribute * attrb=extfeat->Attrb;
			Fest * value=extfeat->Value; Fest::map(value);
			if (attrb->getLang()!=SEMANTIC &&
				attrb->getLang()!=GlobalSrcLang) continue;
			if (attrb->getRelative()==IRRELATIVE) continue;
			if (value->isEmpty()) continue;
			if (unify(value,getExtValue(herit,attrb))==FAIL) return FAIL;
		}
	}
	fest->SrcHerit=herit;
	return SUCC;
}

int Unity::setTgtHerit(Fest * fest, Fest * herit)
{
	if (fest->getTgtHerit()==herit) return SUCC;
	if (fest->getTgtHerit()) error("Unity::setTgtHerit(): set Duplicate TgtHerit.");
	if (herit==NULL || herit!=NONE && herit->TgtHerit==NULL)
		error("Unity::setTgtHerit(): set NULL TgtHerit.");
	if (herit!=NONE)
	{
		IntFeat * intfeat;
		for (intfeat=fest->IntFest.FstFeat;intfeat;intfeat=intfeat->Next)
		{
			Attribute * attrb=intfeat->Attrb;
			Atom * value=intfeat->Value; Atom::map(value);
			if (attrb->getLang()!=GlobalTgtLang) continue;
			if (attrb->getRelative()==IRRELATIVE) continue;
			if (value->isEmpty()) continue;
			if (unify(value,getIntValue(herit,attrb))==FAIL) return FAIL;
		}
		ExtFeat * extfeat;
		for (extfeat=fest->ExtFest.FstFeat;extfeat;extfeat=extfeat->Next)
		{
			Attribute * attrb=extfeat->Attrb;
			Fest * value=extfeat->Value; Fest::map(value);
			if (attrb->getLang()!=GlobalTgtLang) continue;
			if (attrb->getRelative()==IRRELATIVE) continue;
			if (value->isEmpty()) continue;
			if (unify(value,getExtValue(herit,attrb))==FAIL) return FAIL;
		}
	}
	fest->TgtHerit=herit;
	return SUCC;
}

Atom * Unity::getIntValue(Fest * fest, Attribute * attrb, BOOL get_herit_default)
{
	if (fest->isNone()) error("Unity::getIntValue(): getIntValue from NONE fest.");
	Fest::map(fest);
	IntFeat * intfeat=fest->getIntFeat(attrb);
	Atom * value=intfeat?intfeat->Value:NULL;
	if (!value)
	{
		switch (attrb->getAttType())
		{
			case HIERAR: value=new Hierar(); break;
			case SYMBOL: value=new Symbol(); break;
			case NUMBER: value=new Number(); break;
			case MTBOOL: value=new Boolean(); break;
		}
		intfeat=fest->addIntFeat(attrb,value);
	}
	Atom::map(value);

	if (get_herit_default==FALSE)
		{ addAtomCut(intfeat); return value; }

	int lang=attrb->getLang();
	Fest * Fest::*Herit;
	if (lang==SEMANTIC||lang==GlobalSrcLang) Herit=&Fest::SrcHerit;
	else if (lang==GlobalTgtLang) Herit=&Fest::TgtHerit;
	else error("Unity::getIntValue(): illegal attribute.",attrb->getString());
	if (attrb->getRelative()==MTRELATIVE && value->isEmpty() &&
		fest->*Herit && fest->*Herit!=NONE &&
		(!intfeat->CutBy || intfeat->CutBy->Valid==NO))
	{
		Fest * herit=fest;
		IntFeat * heritintfeat;
		Atom * heritvalue;
		do
		{
			herit=herit->*Herit;
			Fest::map(herit);
			heritintfeat=herit->getIntFeat(attrb);
			if (!heritintfeat) continue;
			heritvalue=heritintfeat->Value;
			Atom::map(heritvalue);
			if (heritintfeat->CutBy && heritintfeat->CutBy->Valid==YES) break;
			if (!heritvalue->isEmpty()) break;
		}
		while (herit->*Herit && herit->*Herit!=NONE);
		if (!heritintfeat)
		{
			heritvalue=value->newAtom();
			herit->addIntFeat(attrb,heritvalue);
		}
		addAtomMap(value,heritvalue);
		value=heritvalue;
	}

	if (value->isEmpty() && !value->isNewBy(this) &&
		attrb->getDefault() && testDefaultSwitch(attrb) )
	{
		IToken tokendflt(attrb->getDefault()); tokendflt.input();
		unify(value,tokendflt);
		Atom::map(value);
	}
	return value;
}

Fest * Unity::getExtValue(Fest * fest, Attribute * attrb, BOOL get_herit_default)
{
	if (fest->isNone()) error("Unity::getExtValue(): getExtValue from NONE fest.");
	Fest::map(fest);
	ExtFeat * extfeat=fest->getExtFeat(attrb);
	Fest * value=extfeat?extfeat->Value:NULL;
	if (!value)
	{
		value=new Fest;
		extfeat=fest->addExtFeat(attrb,value);
	}
	Fest::map(value);

	if (get_herit_default==FALSE)
		{ addFestCut(extfeat); return value; }

	int lang=attrb->getLang();
	Fest * Fest::*Herit;
	if (lang==SEMANTIC||lang==GlobalSrcLang) Herit=&Fest::SrcHerit;
	else if (lang==GlobalTgtLang) Herit=&Fest::TgtHerit;
	else error("Unity::getExtValue(): illegal attribute.",attrb->getString());
	if (attrb->getRelative()==MTRELATIVE && value->isEmpty() &&
		!value->getSrcHerit() && !value->getTgtHerit() &&
		fest->*Herit && fest->*Herit!=NONE &&
		(!extfeat->CutBy || extfeat->CutBy->Valid==NO))
	{
		Fest * herit=fest;
		ExtFeat * heritextfeat;
		Fest * heritvalue;
		do
		{
			herit=herit->*Herit;
			Fest::map(herit);
			heritextfeat=herit->getExtFeat(attrb);
			if (!heritextfeat) continue;
			heritvalue=heritextfeat->Value;
			Fest::map(heritvalue);
			if (heritextfeat->CutBy && heritextfeat->CutBy->Valid==YES) break;
			if (heritvalue->getSrcHerit() || heritvalue->getTgtHerit() ||
				!heritvalue->isEmpty()) break;
		}
		while (herit->*Herit && herit->*Herit!=NONE);
		if (!heritextfeat)
		{
			heritvalue=new Fest;
			herit->addExtFeat(attrb,heritvalue);
		}
		addFestMap(value,heritvalue);
		value=heritvalue;
	}

	if (value->isEmpty() && value->isWhole() && !value->isNewBy(this) &&
		attrb->getDefault() && testDefaultSwitch(attrb) )
	{
		IToken tokendflt(attrb->getDefault()); tokendflt.input();
		unify(value,tokendflt);
		Fest::map(value);
	}
	return value;
}

int Unity::unify(Fest *fest1,IToken &token1,Fest *fest2,IToken &token2)
{
	Fest::map(fest1);
	if (token1.isDelim("."))
	{
		if (fest1->isNone()) return FAIL;
		token1.input();
		Attribute * attrb1=GlobalModel->getAttribute(token1);
		token1.input();
		if (attrb1->getIntExt()==INTERNAL)
		{
			Atom * a1=getIntValue(fest1,attrb1);
			return unify(a1,fest2,token2);
		}
		else
		{
			Fest * f1=getExtValue(fest1,attrb1);
			return unify(f1,token1,fest2,token2);
		}
	}
	else if (token1.isDelim("==")) return share(fest1,fest2,token2);
	else if (token1.isDelim("!=")) return sever(fest1,fest2,token2);
	else /* token1.isDelim("=") */ return unify(fest1,fest2,token2);
}

int Unity::share(Fest *fest1,Fest *fest2,IToken &token2)
{
	Fest::map(fest2);
	if (token2.isDelim("."))
	{
		if (fest2->isNone()) return FAIL;
		token2.input();
		Attribute * attrb2=GlobalModel->getAttribute(token2);
		token2.input();
		if (attrb2->getIntExt()==INTERNAL)
			error("Unity::share(): Unify a Fest with an Atom.");
		Fest * f2=getExtValue(fest2,attrb2);
		return share(fest1,f2,token2);
	}
	else /* if (token2.isDelim("@")) */
	{
		if (fest1->isNone() || fest2->isNone()) return FAIL;
		do
		{
			token2.input();
			Attribute * attrb=GlobalModel->getAttribute(token2);
			token2.input();
			if (attrb->getIntExt()==INTERNAL)
			{
				Atom * a1=getIntValue(fest1,attrb);
				Atom * a2=getIntValue(fest2,attrb);
				if (unify(a1,a2)==FAIL)
				{
					while (token2.isDelim("@"))
						{ token2.input(); token2.input(); }
					return FAIL;
				}
			}
			else
			{
				Fest * f1=getExtValue(fest1,attrb);
				Fest * f2=getExtValue(fest2,attrb);
				if (unify(f1,f2)==FAIL)
				{
					while (token2.isDelim("@"))
						{ token2.input(); token2.input(); }
					return FAIL;
				}
			}
		} while (token2.isDelim("@"));
		return SUCC;
	}
}

int Unity::sever(Fest *fest1,Fest *fest2,IToken &token2)
{
	Fest::map(fest2);
	if (token2.isDelim("."))
	{
		if (fest2->isNone()) return FAIL;
		token2.input();
		Attribute * attrb2=GlobalModel->getAttribute(token2);
		token2.input();
		if (attrb2->getIntExt()==INTERNAL)
			error("Unity::sever(): Unify a Fest with an Atom.");
		Fest * f2=getExtValue(fest2,attrb2);
		return sever(fest1,f2,token2);
	}
	else /* if (token2.isDelim("@")) */
	{
		if (fest1->isNone() || fest2->isNone()) return FAIL;
		Fest * srcfest, * tgtfest;
		if (fest1->getSrcHerit()==fest2) srcfest=fest1;
		else if (fest2->getSrcHerit()==fest1) srcfest=fest2;
		else srcfest=NULL;
		if (fest1->getTgtHerit()==fest2) tgtfest=fest1;
		else if (fest2->getTgtHerit()==fest1) tgtfest=fest2;
		else tgtfest=NULL;
		if (srcfest==NULL && tgtfest==NULL)
			error("Unity::sever(): Sever Fests without herit relations.",
				  token2.getInPntr());
		do
		{
			token2.input();
			Attribute * attrb=GlobalModel->getAttribute(token2);
			token2.input();
			int lang=attrb->getLang();
			int intext=attrb->getIntExt();
			if ((lang==SEMANTIC || lang==GlobalSrcLang) && srcfest)
			{
				if (intext==INTERNAL) getIntValue(srcfest,attrb,FALSE);
				else /* intext==EXTERNAL */ getExtValue(srcfest,attrb,FALSE);
			}
			if (lang==GlobalTgtLang && tgtfest)
			{
				if (intext==INTERNAL) getIntValue(tgtfest,attrb,FALSE);
				else /* intext==EXTERNAL */ getExtValue(tgtfest,attrb,FALSE);
			}
		} while (token2.isDelim("@"));
		return SUCC;
	}
}

int Unity::unify(Fest *fest1,Fest *fest2,IToken &token2)
{
	Fest::map(fest2);
	if (token2.isDelim("."))
	{
		if (fest2->isNone()) return FAIL;
		token2.input();
		Attribute * attrb2=GlobalModel->getAttribute(token2);
		token2.input();
		if (attrb2->getIntExt()==INTERNAL)
			error("Unity::unify(): Unify a Fest with an Atom.");
		Fest * f2=getExtValue(fest2,attrb2);
		return unify(fest1,f2,token2);
	}
	else return unify(fest1,fest2);
}

int Unity::unify(Fest *fest1,Fest *fest2)
{
	Fest::map(fest1);
	Fest::map(fest2);
	return _unify(fest1,fest2);
}

int Unity::unify(Atom *atom1,Fest *fest2,IToken &token2)
{
	Fest::map(fest2);
	if (!token2.isDelim("."))
		error("Unity::unify(): Unify an Atom with a Fest.");
	if (fest2->isNone()) return FAIL;
	token2.input();
	Attribute * attrb2=GlobalModel->getAttribute(token2);
	token2.input();
	if (attrb2->getIntExt()==INTERNAL)
	{
		Atom * a2=getIntValue(fest2,attrb2);
		return unify(atom1,a2);
	}
	else
	{
		Fest * f2=getExtValue(fest2,attrb2);
		return unify(atom1,f2,token2);
	}
}

int Unity::unify(Atom *atom1,Atom *atom2)
{
	Atom::map(atom1);
	Atom::map(atom2);
	return _unify(atom1,atom2);
}

int Unity::unify(Fest *fest1,IToken &token1,IToken &token2)
{
	Fest::map(fest1);
	if (token1.isDelim("."))
	{
		if (fest1->isNone()) return FAIL;
		token1.input();
		Attribute * attrb1=GlobalModel->getAttribute(token1);
		token1.input();
		if (attrb1->getIntExt()==INTERNAL)
		{
			Atom * a1=getIntValue(fest1,attrb1);
			return unify(a1,token2);
		}
		else
		{
			Fest * f1=getExtValue(fest1,attrb1);
			return unify(f1,token1,token2);
		}
	}
	else return unify(fest1,token2);
}

int Unity::unify(Fest *fest1,IToken &token2)
{
	Fest::map(fest1);
	if (Atom::isTrue(token2))
	{
		token2.input();
		return (!fest1->isNone()&&(fest1->getSrcHerit()||fest1->getTgtHerit()));
	}
	else if (Atom::isFalse(token2))
	{
		token2.input();
		return (fest1->isNone()||(!fest1->getSrcHerit()&&!fest1->getTgtHerit()));
	}
	else return _unify(fest1,token2);
}

int Unity::unify(Atom *atom1,IToken &token2)
{
	Atom::map(atom1);
	return _unify(atom1,token2);
}

int Unity::_unify(Atom * atom1, Atom * atom2)
{
	if (Valid==NO) error("Unity::_unify(): unify on a invalid Unity.");
	if (atom1==atom2) return SUCC;
	if (atom1->hasType()!=atom2->hasType()) return FAIL;
	if (atom1->isEmpty())
		{ addAtomMap(atom1,atom2); return SUCC; }
	if (atom2->isEmpty())
		{ addAtomMap(atom2,atom1); return SUCC; }
	char result[ATOMLENGTH];
	OToken token0(result);
	IToken token1(atom1->Nucleus), token2(atom2->Nucleus);
	token1.input();
	token2.input();
	atom1->unify_Nucleus(token0,token1,token2);
	if (token0.getOutPntr()==result) return FAIL;
	token0.outputNull();
	if (!strcmp(result,atom1->Nucleus)) addAtomMap(atom2,atom1);
	else if (!strcmp(result,atom2->Nucleus)) addAtomMap(atom1,atom2);
	else
	{
		Atom * atom=atom1->newAtom();
		addAtomNew(atom);
		addAtomMap(atom1,atom);
		addAtomMap(atom2,atom);
		int length=token0.getOutPntr()-result;
		if (length>=ATOMLENGTH) error("Unity::_unify(): Atom too long.",result);
		atom->Nucleus=new char[length];
		strcpy(atom->Nucleus,result);
	}
	return SUCC;
};

int Unity::_unify(Atom * atom1, IToken & token2)
{
	if (Valid==NO) error("Unity::_unify(): unify on a invalid Unity.");
	if (atom1->isEmpty())
	{
		if (atom1->isNewBy(this)) atom1->inputFrom(token2);
		else
		{
			Atom * atom=atom1->newAtom();
			addAtomNew(atom);
			addAtomMap(atom1,atom);
			atom->inputFrom(token2);
		}
		return SUCC;
	}
	char result[ATOMLENGTH];
	OToken token0(result);
	IToken token1(atom1->Nucleus);
	token1.input();
	atom1->unify_Nucleus(token0,token1,token2);
	if (token0.getOutPntr()==result) return FAIL;
	token0.outputNull();
	if (strcmp(atom1->Nucleus,result))
	{
		Atom * atom=atom1->newAtom();
		addAtomNew(atom);
		addAtomMap(atom1,atom);
		int length=token0.getOutPntr()-result;
		if (length>=ATOMLENGTH) error("Unity::_unify(): Atom too long.",result);
		atom->Nucleus=new char[length];
		strcpy(atom->Nucleus,result);
	}
	return SUCC;
}

int Unity::_unify(Fest * fest1, Fest * fest2)
{
	if (Valid==NO) error("Unity::_unify(): unify on a invalid Unity.");
	if (fest1==fest2) return SUCC;
	if (fest1->isEmpty() && !fest1->getSrcHerit() && !fest1->getTgtHerit())
	{
		addFestMap(fest1,fest2);
		return SUCC;
	}
	if (fest2->isEmpty() && !fest2->getSrcHerit() && !fest2->getTgtHerit())
	{
		addFestMap(fest2,fest1);
		return SUCC;
	}
	if (fest1->isNone() || fest2->isNone()) return FAIL;
	Fest * mapfrom, * mapto;
	if (!fest1->getSrcHerit() && !fest1->getTgtHerit())
		{ mapfrom=fest1; mapto=fest2; }
	else if (!fest2->getSrcHerit() && !fest2->getTgtHerit())
		{ mapfrom=fest2; mapto=fest1; }
	else if (fest1->getSrcHerit()==fest2->getSrcHerit() &&
			 fest1->getTgtHerit()==fest2->getTgtHerit())
		{ mapfrom=fest1; mapto=fest2; }
	else
		return FAIL;
	addFestMap(mapfrom,mapto);
	IntFeat * intfeat=mapfrom->IntFest.FstFeat;
	while (intfeat)
	{
		Attribute * attrb=intfeat->Attrb;
		Atom * value=getIntValue(mapto,attrb);
		if (unify(value,intfeat->Value)==FAIL) return FAIL;
		intfeat=intfeat->Next;
	}
	ExtFeat * extfeat=mapfrom->ExtFest.FstFeat;
	while (extfeat)
	{
		Attribute * attrb=extfeat->Attrb;
		Fest * value=getExtValue(mapto,attrb);
		if (unify(value,extfeat->Value)==FAIL) return FAIL;
		extfeat=extfeat->Next;
	}
	return SUCC;
}

int Unity::_unify(Fest * fest1, IToken & token2)
{
	if (Valid==NO) error("Unity::_unify(): unify on a invalid Unity.");
	if (fest1->isNone())
	{
		if (Fest::isNone(token2)) return SUCC;
		else return FAIL;
	}
	if (fest1->isEmpty() && fest1->isWhole())
	{
		if (fest1->isNewBy(this)) fest1->inputFrom(token2);
		else
		{
			Fest * fest=new Fest;
			if (fest1->getSrcHerit()==NONE) setSrcHerit(fest,(Fest *)NONE);
			if (fest1->getTgtHerit()==NONE) setTgtHerit(fest,(Fest *)NONE);
			addFestNew(fest);
			addFestMap(fest1,fest);
			fest->inputFrom(token2);
		}
		return SUCC;
	}
	if (Fest::isNone(token2)) return FAIL;
	if (token2.isDelim("["))
	{
		if (fest1->isNone()) return FAIL;
		token2.input();
		if (!token2.isDelim("]"))
		{
			token2.redrawin();
			while (!token2.isDelim("]"))
			{
				token2.input();
				Attribute * attrb=GlobalModel->getAttribute(token2);
				if (!attrb || attrb->getIntExt()!=INTERNAL)
					error("Unity::_unify(): need Internal attribute.",token2);
				Atom * atom=getIntValue(fest1,attrb);
				token2.input();
				token2.input();
				if (unify(atom,token2)==FAIL)
				{
					while (!token2.isDelim("]")) token2.input(); token2.input();
					if (token2.isDelim("{")){ token2.gouptomatch(); token2.input(); }
					return FAIL;
				}
			}
		}
		token2.input();
	}
	if (token2.isDelim("{"))
	{
		if (fest1->isNone()) return FAIL;
		token2.input();
		if (!token2.isDelim("}"))
		{
			token2.redrawin();
			while (!token2.isDelim("}"))
			{
				token2.input();
				Attribute * attrb=GlobalModel->getAttribute(token2);
				if (!attrb || attrb->getIntExt()!=EXTERNAL)
					error("Unity::_unify(): need External attribute",token2);
				Fest * fest=getExtValue(fest1,attrb);
				token2.input();
				token2.input();
				if (unify(fest,token2)==FAIL)
				{
					token2.setBuffer("{");
					token2.gouptomatch();
					token2.input();
					return FAIL;
				}
			}
		}
		token2.input();
	}
	return SUCC;
}

void Unity::recover(UnityMark & mark)
{
	if (Parent) error("Unity::recover(): recover with valid parent.");

	AtomCut * atomcut=FstAtomCut;
	while (atomcut!=mark.FstAtomCut)
	{
		AtomCut * next=atomcut->Next;
		if (atomcut->Cut->CutBy==this) atomcut->Cut->CutBy=NULL;
		delete atomcut;
		atomcut=next;
	}
	FstAtomCut=mark.FstAtomCut;

	AtomMap * atommap=FstAtomMap;
	while (atommap!=mark.FstAtomMap)
	{
		AtomMap * next=atommap->Next;
		if (atommap->MapFrom->MapBy==this) atommap->MapFrom->MapBy=NULL;
		delete atommap;
		atommap=next;
	}
	FstAtomMap=mark.FstAtomMap;

	AtomNew * atomnew=FstAtomNew;
	while (atomnew!=mark.FstAtomNew)
	{
		AtomNew * next=atomnew->Next;
		delete atomnew->New;
		delete atomnew;
		atomnew=next;
	}
	FstAtomNew=mark.FstAtomNew;

	FestCut * festcut=FstFestCut;
	while (festcut!=mark.FstFestCut)
	{
		FestCut * next=festcut->Next;
		if (festcut->Cut->CutBy==this) festcut->Cut->CutBy=NULL;
		delete festcut;
		festcut=next;
	}
	FstFestCut=mark.FstFestCut;

	FestMap * festmap=FstFestMap;
	while (festmap!=mark.FstFestMap)
	{
		FestMap * next=festmap->Next;
		if (festmap->MapFrom->MapBy==this) festmap->MapFrom->MapBy=NULL;
		delete festmap;
		festmap=next;
	}
	FstFestMap=mark.FstFestMap;

	FestNew * festnew=FstFestNew;
	while (festnew!=mark.FstFestNew)
	{
		FestNew * next=festnew->Next;
		delete festnew->New;
		delete festnew;
		festnew=next;
	}
	FstFestNew=mark.FstFestNew;
}

void UnityList::free()
{
	UnityLink * link=FstLink;
	while (link) { UnityLink * next=link->Next; delete link; link=next; }
	FstLink=NULL;
}

void UnityStack::free()
{
	UnityElem * elem=TopElem;
	while (elem) { UnityElem * next=elem->Next; delete elem; elem=next; }
}


#ifdef KBMS

void Unity::output_atom(ostream & o,Atom * atom,Attribute * attr)
{
	if (atom->isEmpty()) { o << "(©у)"; return; }
	cValList * vallist=&attr->getValList();
	char buffer[ATOMLENGTH];
	IOToken token(atom->Nucleus,buffer);
	token.input();
	while (*token)
	{
		if (vallist)
		{
			Value * value=vallist->find(token);
			if (value)
			{
				if (value->getRefer()) value=value->getRefer();
				token.setBuffer(value->getString());
			}
		}
		token.output();
		token.input();
	}
	token.outputNull();
	o << buffer;
}

void Unity::output_fest(ostream & o,Fest * fest,int language)
{
	if (this==NULL)
		error("Unity::output_fest(): unity is NULL.");
	if (Valid==FALSE)
		error("Unity::output_fest(): unity is invalid.");
	if (fest->isNone()) { o << "(нч)"; return; }
	if (fest->isEmpty()) { o << "(©у)"; return; }
	o << '[';
	int comma;
	IntFeat * intfeat;
	for (comma=0,intfeat=fest->IntFest.FstFeat;intfeat;intfeat=intfeat->Next)
	{
		Attribute * attrb=intfeat->Attrb;
		Attribute * attrbref=attrb->getRefer();
		if (attrbref==NULL) attrbref=attrb;
		int lang=attrb->getLang();
		if (language==GlobalSrcLang)
			{ if (lang!=language && lang!=SEMANTIC) continue; }
		else // (language==GlobalTgtLang)
			{ if (lang!=language) continue; }
		Atom * value=intfeat->Value;
		Atom::map(value);
		if (value->isEmpty()) continue;
		if (comma) o<< ',';
		o << attrbref->getString();
		o << ':';
		output_atom(o,value,attrb);
		comma=1;
	}
	o << ']';
	if (!fest->ExtFest.FstFeat) return;
	o << '{';
	ExtFeat * extfeat;
	for (comma=0,extfeat=fest->ExtFest.FstFeat;extfeat;extfeat=extfeat->Next)
	{
		Attribute * attrb=extfeat->Attrb;
		Attribute * attrbref=attrb->getRefer();
		if (attrbref==NULL) attrbref=attrb;
		int lang=attrb->getLang();
		if (language==GlobalSrcLang)
			{ if (lang!=language && lang!=SEMANTIC) continue; }
		else // (language==GlobalTgtLang)
			{ if (lang!=language) continue; }
		Fest * value=extfeat->Value;
		Fest::map(value);
		if (value->isEmpty()) continue;
		if (comma) o<< ',';
		o << attrbref->getString() << ':';
		Fest * Fest:: * Herit;
		if (language==GlobalSrcLang) Herit=&Fest::SrcHerit;
		else if (language==GlobalTgtLang) Herit=&Fest::TgtHerit;
		else Herit=NULL;
		if (Herit!=NULL && value->*Herit)
		{
			Fest * herit;
			Atom * baseform=NULL, * lexcat=NULL, * phrcat=NULL;
			Attribute * baseformatt=GlobalModel->getBaseForm(language);
			Attribute * lexcatatt=GlobalModel->getLexCat(language);
			Attribute * phrcatatt=GlobalModel->getPhrCat(language);
			for (herit=value;herit && herit!=NONE;herit=herit->*Herit)
			{
				IntFeat * intfeat;
				for (intfeat=herit->IntFest.FstFeat;intfeat && intfeat!=NONE;
					 intfeat=intfeat->Next)
				{
					if (intfeat->Value->isEmpty()) continue;
					if (intfeat->Attrb==baseformatt)
						baseform=intfeat->Value;
					else if (intfeat->Attrb==lexcatatt)
						lexcat=intfeat->Value;
					else if (!phrcat && intfeat->Attrb==phrcatatt)
						phrcat=intfeat->Value;
					if (phrcat && baseform) break;
				}
				if (phrcat && baseform) break;
			}
			if (phrcat && phrcat->Nucleus) output_atom(o,phrcat,phrcatatt);
			else if (lexcat && lexcat->Nucleus) output_atom(o,lexcat,lexcatatt);
			if (baseform && baseform->Nucleus) { o << '<'; output_atom(o,baseform,baseformatt); o << '>'; }
			else o << "(©у)";
		}
		else output_fest(o,value,language);
		comma=1;
	}
	o << '}';
}

#endif
