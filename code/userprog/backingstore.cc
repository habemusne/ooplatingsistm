#include <sstream>
#include <string>
#include "machine.h"
#include "system.h"
#include "backingstore.h"

BackingStore::BackingStore(AddrSpace *as){
  this->space = as;
  std::ostringstream os;
  os << "~swap";
  os << this->space->spaceId;
  std::string bsFilename = os.str();
  const char* bsFilenameConstChar = bsFilename.c_str();
  char* bsFilenameChar = const_cast<char*> (bsFilenameConstChar);
printf("swap file name to be created is %s\n", bsFilenameChar);
  bool result = fileSystem->Create(bsFilenameChar, space->getNumPages() * PageSize);
  if (result == FALSE){
    fileSystem->Remove(bsFilenameChar);
    result = fileSystem->Create(bsFilenameChar, space->getNumPages() * PageSize);
  }
  ASSERT(result == TRUE);
  this->backstore = fileSystem->Open(bsFilenameChar);
  ASSERT(this->backstore != NULL);
}

BackingStore::~BackingStore(){
  //Do not delete the address space. Leave Thread::Finish() for that

  std::ostringstream os;
  os << "~swap";
  os << this->space->spaceId;
  std::string bsFilename = os.str();
  const char* bsFilenameConstChar = bsFilename.c_str();
  char* bsFilenameChar = const_cast<char*> (bsFilenameConstChar);
printf("swap file name to be deleted is %s\n", bsFilenameChar);
  fileSystem->Remove(bsFilenameChar);
}

void BackingStore::PageOut(TranslationEntry *pte){
  int virt_addr_start = pte->virtualPage * PageSize;
printf("%d\n", virt_addr_start);
  this->backstore->WriteAt(space->vir_to_phys(virt_addr_start), PageSize, 
    0 + virt_addr_start);
}

void BackingStore::PageIn(TranslationEntry *pte){
  int virt_addr_start = pte->virtualPage * PageSize;
  this->backstore->ReadAt(space->vir_to_phys(virt_addr_start), PageSize, 0 + virt_addr_start);
}

