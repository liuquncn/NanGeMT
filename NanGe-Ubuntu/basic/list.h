#include "error.h"
#ifndef LIST_H
#define LIST_H

template <class T> class ObjLink;
template <class T, class L> class ObjList;

template <class T>
class ObjLink
{
	T * Data;
	ObjLink * Prev;
	ObjLink * Next;
protected:
	ObjLink() { Data=NULL; Prev=Next=NULL; }
public:
	T * GetData() { return Data; }
	ObjLink * GetPrev() { return Prev; }
	ObjLink * GetNext() { return Next; }
	typedef ObjLink<T> ObjLinkT;
	friend class ObjList<T, ObjLinkT >;
};

template <class T,class L>
// L must be ObjLink<T> or its derived class
class ObjList
{
	L * Head;
	L * Tail;
	int Number;
public:
	ObjList() { Head=Tail=NULL; Number=0; }
	~ObjList() { free(); }
	void free();
	L * GetHead() const { return Head; }
	L * GetTail() const { return Tail; }
	int GetNumber() const { return Number; }
	L * AddHead() { return AddHead((const T *)NULL); }
	L * AddHead(const T * data) { return AddHead(*data); }
	L * AddHead(const T & data);
	L * AddTail() { return AddTail((const T *)NULL); }
	L * AddTail(const T * data) { return AddTail(*data); }
	L * AddTail(const T & data);
	L * AddBefore(L * link) { return AddBefore((const T *)NULL,link); }
	L * AddBefore(const T * data,L * link) { return AddBefore(*data,link); }
	L * AddBefore(const T & data,L * link);
	L * AddBehind(L * link) { return AddBefore((const T *)NULL,link); }
	L * AddBehind(const T * data,L * link) { return AddBehind(*data,link); }
	L * AddBehind(const T & data,L * link);
	L * Find(const T * data) const { return Find(*data); }
	L * Find(const T & data) const;
	void Delete(L  * link);
	void Delete(L  & link) { Delete(&link); }
protected:
	virtual T * NewData(const T * data) { return data?new T(*data):new T; }
	virtual void DeleteData(T * data) { delete data; }
};

template <class T,class L>
void ObjList<T,L>::free()
{
	L * link, * nextlink;
	for (link=Head;link;link=nextlink)
	{
		nextlink=link->GetNext();
		DeleteData(link->GetData());
		delete link;
	}
	Head=Tail=NULL;
	Number=0;
}

template <class T,class L>
L * ObjList<T,L>::AddHead(const T & data)
{
	Number++;
	T * newdata=NewData(&data);
	L * link=new L;
	link->Data=newdata;
	if (Head) {	link->Next=Head; Head->Prev=link; Head=link; }
	else { Head=Tail=link; }
	return link;
}

template <class T,class L>
L * ObjList<T,L>::AddTail(const T & data)
{
	Number++;
	T * newdata=NewData(&data);
	L * link=new L;
	link->Data=newdata;
	if (Tail) { link->Prev=Tail; Tail->Next=link; Tail=link; }
	else { Head=Tail=link; }
	return link;
}

template <class T,class L>
L * ObjList<T,L>::Find(const T & data) const
{
	L * link;
	for (link=Head;link;link=link->Next)
		if (*link->Data==data) return link;
	return NULL;
}

template <class T,class L>
L * ObjList<T,L>::AddBefore(const T & data,L * link)
{
	Number++;
	L * l;
	for (l=Head;l;l=l->Next) if (l==link) break;
	if (l==NULL) error("ObjList::AddBefore(): link is not in the list.");
	T * newdata=NewData(&data);
	L * newlink=new L;
	newlink->Data=newdata;
	if (Head==link) { Head=newlink; }
	else { newlink->Prev=link->Prev; link->Prev->Next=newlink; }
	newlink->Next=link;
	link->Prev=newlink;
	return newlink;
}

template <class T,class L>
L * ObjList<T,L>::AddBehind(const T & data,L * link)
{
	Number++;
	L * l;
	for (l=Head;l;l=l->Next) if (l==link) break;
	if (l==NULL) error("ObjList::AddBehind(): link is not in the list.");
	T * newdata=NewData(&data);
	L * newlink=new L;
	newlink->Data=newdata;
	if (Tail==link) { Tail=newlink; }
	else { newlink->Next=link->Next; link->Next->Prev=newlink; }
	newlink->Prev=link;
	link->Next=newlink;
	return newlink;
}

template <class T,class L>
void ObjList<T,L>::Delete(L * link)
{
	Number--;
	L * l;
	for (l=Head;l;l=l->Next) if (l==link) break;
	if (l==NULL) error("ObjList::Delete(): link is not in the list.");
	if (link==Head && link==Tail) { Head=Tail=NULL; }
	else if (link==Head) { link->Next->Prev=NULL; Head=link->Next; }
	else if (link==Tail) { link->Prev->Next=NULL; Tail=link->Prev; }
	else { link->Prev->Next=link->Next; link->Next->Prev=link->Prev; }
	DeleteData(link->Data);
	delete link;
}

typedef ObjLink<char> StrLink;

/*
class StrLink: public ObjLink<char>
{
public:
	StrLink() : ObjLink<char>() {};
	char * GetStr() { return GetData(); }
	StrLink * GetNext() { return (StrLink *)ObjLink<char>::GetNext(); }
	friend class StrList;
	friend class ObjList<char,StrLink>;
};
*/

class StrList : public ObjList<char,StrLink>
{
public:
	StrList() : ObjList<char,StrLink>() {}
	StrList(const StrList & list) : ObjList<char,StrLink>() { operator=(list); }
protected:
	virtual char * NewData(const char * data)
	{
		char * newdata=NULL;
		if (data)
		{
			newdata=new char[strlen(data)+1];
			strcpy(newdata,data);
		}
		return newdata;
	}
	virtual void DeleteData(char * data) { delete[] data; }
public:
	StrList & operator = (const StrList & list)
	{
		StrLink * link;
		for (link=list.GetHead();link;link=link->GetNext())
			AddTail(link->GetData());
		return *this;
	}
};


template <class T> class PtrLink;
template <class T, class L> class PtrList;

template <class T>
class PtrLink
{
	T * Data;
	PtrLink * Prev;
	PtrLink * Next;
protected:
	PtrLink() { Data=NULL; Prev=Next=NULL; }
public:
	T * GetData() { return Data; }
	PtrLink * GetPrev() { return Prev; }
	PtrLink * GetNext() { return Next; }
	typedef PtrLink<T> PtrLinkT;
	friend class PtrList<T, PtrLinkT >;
};

template <class T,class L>
// L must be PtrLink<T> or its derived class
class PtrList
{
	L * Head;
	L * Tail;
	int Number;
public:
	PtrList() { Head=Tail=NULL; Number=0; }
	~PtrList() { free(); }
	void free();
	void freeData();
	L * GetHead() const { return Head; }
	L * GetTail() const { return Tail; }
	int GetNumber() const { return Number; }
	L * AddHead() { return AddHead((T *)NULL); }
	L * AddHead(T * data) { return AddHead(*data); }
	L * AddHead(T & data);
	L * AddTail() { return AddTail((T *)NULL); }
	L * AddTail(T * data) { return AddTail(*data); }
	L * AddTail(T & data);
	L * AddBefore(L * link) { return AddBefore((T *)NULL,link); }
	L * AddBefore(T * data,L * link) { return AddBefore(*data,link); }
	L * AddBefore(T & data,L * link);
	L * AddBehind(L * link) { return AddBefore((T *)NULL,link); }
	L * AddBehind(T * data,L * link) { return AddBehind(*data,link); }
	L * AddBehind(T & data,L * link);
	L * Find(const T * data) const { return Find(*data); }
	L * Find(const T & data) const;
	void Delete(L  * link);
	void Delete(L  & link) { Delete(&link); }
};

template <class T,class L>
void PtrList<T,L>::free()
{
	L * link, * nextlink;
	for (link=Head;link;link=nextlink)
	{
		nextlink=link->GetNext();
		delete link;
	}
	Head=Tail=NULL;
	Number=0;
}

template <class T,class L>
void PtrList<T,L>::freeData()
{
	L * link;
	for (link=Head;link;link=link->GetNext())
	{
		delete link->Data;
		link->Data=NULL;
	}
	}

template <class T,class L>
L * PtrList<T,L>::AddHead(T & data)
{
	Number++;
	L * link=new L;
	link->Data=&data;
	if (Head) {	link->Next=Head; Head->Prev=link; Head=link; }
	else { Head=Tail=link; }
	return link;
}

template <class T,class L>
L * PtrList<T,L>::AddTail(T & data)
{
	Number++;
	L * link=new L;
	link->Data=&data;
	if (Tail) { link->Prev=Tail; Tail->Next=link; Tail=link; }
	else { Head=Tail=link; }
	return link;
}

template <class T,class L>
L * PtrList<T,L>::Find(const T & data) const
{
	L * link;
	for (link=Head;link;link=link->Next)
		if (link->Data==&data) return link;
	return NULL;
}

template <class T,class L>
L * PtrList<T,L>::AddBefore(T & data,L * link)
{
	Number++;
	L * l;
	for (l=Head;l;l=l->Next) if (l==link) break;
	if (l==NULL) error("PtrList::AddBefore(): link is not in the list.");
	L * newlink=new L;
	newlink->Data=&data;
	if (Head==link) { Head=newlink; }
	else { newlink->Prev=link->Prev; link->Prev->Next=newlink; }
	newlink->Next=link;
	link->Prev=newlink;
	return newlink;
}

template <class T,class L>
L * PtrList<T,L>::AddBehind(T & data,L * link)
{
	Number++;
	L * l;
	for (l=Head;l;l=l->Next) if (l==link) break;
	if (l==NULL) error("PtrList::AddBehind(): link is not in the list.");
	L * newlink=new L;
	newlink->Data=&data;
	if (Tail==link) { Tail=newlink; }
	else { newlink->Next=link->Next; link->Next->Prev=newlink; }
	newlink->Prev=link;
	link->Next=newlink;
	return newlink;
}

template <class T,class L>
void PtrList<T,L>::Delete(L * link)
{
	Number--;
	L * l;
	for (l=Head;l;l=l->Next) if (l==link) break;
	if (l==NULL) error("PtrList::Delete(): link is not in the list.");
	if (link==Head && link==Tail) { Head=Tail=NULL; }
	else if (link==Head) { link->Next->Prev=NULL; Head=link->Next; }
	else if (link==Tail) { link->Prev->Next=NULL; Tail=link->Prev; }
	else { link->Prev->Next=link->Next; link->Next->Prev=link->Prev; }
	delete link;
}


#endif /* LIST_H */