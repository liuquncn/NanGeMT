
#ifndef CHNPRSRBASE_H
#define CHNPRSRBASE_H

#include "../kbase/prsrbase.h"

class ChnPrsRbase : public PrsRbase
{
  public:

	int hasSrcLang() { return CHINESE; }
};

#endif // CHNPRSRBASE_H
