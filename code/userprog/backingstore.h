#ifndef BACKINGSTORE_H
#define BAKINGSTORE_H

#include "openfile.h"
#include "translate.h"
#include "addrspace.h"

class BackingStore{
public:
  BackingStore(AddrSpace *as);
  ~BackingStore();
  void PageOut(TranslationEntry *pte);
  void PageIn(TranslationEntry *pte);
private:
  AddrSpace *space;
  OpenFile *backstore;
};

#endif
