#ifndef prsrbase_h
#define prsrbase_h

void parserule(size_t intropos, string & prsid, cfgc & cfg, size_t & head, string & bind);
void parserulentry(string & entry, string & prsid, cfgc & cfg, size_t & head, string & bind);
void getcfg(string & entry, cfgc & cfg, size_t & head, string & yxbind);
void removequota(string & in);
void getdotreg(string & entry, tagc & tag, regionc & dotreg);
void shallowprsrbase(vector<string> & prsrset, vector<prsrc> & prsrtable);
void addprsrmap(map<string, vector<int> > & prsrmap, string & str, size_t idx);

#endif
