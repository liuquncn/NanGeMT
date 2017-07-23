#ifndef tag_h
#define tag_h

#include <string>

using namespace std;

class regionc {
public:
  size_t begin;
  size_t end;
  bool operator==(regionc const & other) const {
    return((begin==other.begin) && (end==other.end));
  }
  bool operator!=(regionc const & other) const {
    return((begin!=other.begin) || (end!=other.end));
  }
  bool operator<(regionc const & other) const {
    if (begin<other.begin)
      return(true);
    else if (begin>other.begin)
      return(false);
    else
      return(end<other.end);
  }
  bool operator>(regionc const & other) const {
    if (begin>other.begin)
      return(true);
    else if (begin<other.begin)
      return(false);
    else 
      return(end>other.end);
  }
  bool outside(regionc & reg);
  bool contain(regionc & reg);
  regionc(){
    begin=0;
    end=0;
  };
  regionc(size_t inbegin, size_t inend) {
    begin=inbegin;
    end=inend;
  }
};

class tagc {
public:
  //size_t loc;
  regionc sentreg;
  string word;
  string cat;
  string catid;
  string feat;
  vector<string> ruleset;
};

class segc {
public:
  string seq;
  char flag;  // 'w': word string; 't': word and its tag; 'p': phrase
};

//void postag(vector<string> & words, vector<tagc> & tags);
void postag(vector<string> & words, size_t initpos, vector<tagc> & tags);
void procunkword(string & seq, tagc & tag);
void iniedges(vector<segc> & segs);
void maketag(string & word, string & cat, size_t initpos, tagc & tag);

#endif
