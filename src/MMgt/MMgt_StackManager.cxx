#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <MMgt_StackManager.ixx>
#include <Standard_ProgramError.hxx>

#ifdef HAVE_MALLOC_H
# include <malloc.h>
#endif

#ifdef TRACE
#define  SHALLOWDUMP ShallowDump(cout)
#else
#define  SHALLOWDUMP
#endif

//============================================================================
//==== There are two declaration of classes just for protections =============
//============================================================================

class _BaseElement {
 public:
  virtual void M1() =0;
  virtual void M2() =0;
  virtual void M3() =0;
  virtual void M4() =0;
  virtual void M5() =0;
  virtual void M6() =0;
  virtual void M7() =0;
  virtual void M8() =0;
  virtual void M9() =0;
  virtual void M10()=0;
}; 

static class _Element: public _BaseElement 
{
 public:
  _Element() {};
  void Msg() 
    {Standard_ProgramError::Raise("Attempt to access to a 'deleted' object");}
  void M1() {Msg();}
  void M2() {Msg();}
  void M3() {Msg();}
  void M4() {Msg();}
  void M5() {Msg();}
  void M6() {Msg();}
  void M7() {Msg();}
  void M8() {Msg();}
  void M9() {Msg();}
  void M10(){Msg();}
  
  Standard_Address myNext;
  
} *anElement = new _Element;

//============================================================================

//============================================================================
MMgt_StackManager::MMgt_StackManager()
{
  myFreeListSize = sizeof(_Element);
  myFreeList = (Standard_Address)calloc((int)(myFreeListSize+1),
					sizeof(myFreeList));
}

//============================================================================
void MMgt_StackManager::Destructor() 
{
  SHALLOWDUMP;

  Purge();
  free((char*) myFreeList);
  myFreeListSize = 0;

  SHALLOWDUMP;
}

//============================================================================
void MMgt_StackManager::ShallowDump(Standard_OStream& s) const 
{
  Standard_Address aFree;
  Standard_Integer i, NbFree;
  
  s << "Begin class MMgt_StackManager\n" << endl
    << "\t... Size:" << myFreeListSize << endl;

  //==== A loop for Dumping all the storage in the Free List ===========
  for(i=sizeof(Standard_Address); i<= myFreeListSize; i++){
    aFree = ((Standard_Address *)myFreeList)[i];
    
    NbFree = 0;
    //==== A loop for deallocating all the storage with the same size =======
    while(aFree){
      aFree = ((_Element *)aFree)->myNext;
      NbFree++;
    }
    if(NbFree) s<< "\t... ["<< i<< "]: ("<< NbFree<< ") Free Block "<< endl;
  }
  s << "End class MMgt_StackManager" << endl;
}

//============================================================================
MMgt_StackManager MMgt_StackManager::ShallowCopy() const
{
  Standard_ProgramError::Raise
    ("Attempt to make a ShallowCopy of a 'MMgt_StackManager'");
  return *this;
}

//============================================================================
Standard_Address MMgt_StackManager::Allocate(const Standard_Integer aSize)
{
  Standard_Address aStack;

  if(aSize <= myFreeListSize && ((void* *)myFreeList)[aSize] != NULL){

    //==== There is the free storage in the Free List, so we use it ==========
    aStack = ((Standard_Address *)myFreeList)[aSize];
    ((Standard_Address *)myFreeList)[aSize] = ((_Element *)aStack)->myNext;

    //==== The storage is set to 0 ===========================================
    memset(aStack,0,(int)aSize);

  } else {

    //==== There is no storage to be used, so we allocated it from "heap" ====
    aStack = (void *)calloc((int)aSize, sizeof(char));
  }
  
  return aStack;
}

//============================================================================
void MMgt_StackManager::Free(Standard_Address& aStack, 
			     const Standard_Integer aSize)
{
   //==== Only the Storage large than a 'Element' can be used ================
   if((unsigned int ) aSize > sizeof(_Element)){

     if(aSize > myFreeListSize) {

       //==== If there is no storage of this size in FreeList ================
       myFreeList=(Standard_Address)realloc((char *)myFreeList,
					    (int)(aSize+1)*sizeof(myFreeList));

       //==== Initialize to "NULL" the new case of FreeList =================
       for(Standard_Integer i=myFreeListSize+1; i<=aSize; i++){
	 ((Standard_Address *)myFreeList)[i] = NULL;
       }

       myFreeListSize = aSize;
     }

     //==== Recycling the storage in the Free List ===========================
     anElement->myNext = ((Standard_Address *)myFreeList)[aSize];

     memcpy((char *)aStack, (char *)anElement, sizeof(_Element));
     ((Standard_Address *)myFreeList)[aSize] = aStack;
   } else {
     //==== The littles storgas will be managed by system ====================
     free((char *) aStack);
   }

   //==== Nullify ============================================================
   aStack = NULL;
}

void MMgt_StackManager::Purge()
{
  Standard_Address aFree;
  Standard_Address aOther;
  
  Standard_Integer i;
  
  //==== A loop for deallocating all the storage in the Free List ===========
  for(i=sizeof(Standard_Address); i<= myFreeListSize; i++){
    aFree = ((Standard_Address *)myFreeList)[i];
    
    //==== A loop for deallocating all the storage with the same size =======
    while(aFree){
      aOther = aFree;
      aFree = ((_Element *)aFree)->myNext;
      free((char *)aOther); 
    }
    
    ((Standard_Address *)myFreeList)[i] = NULL;
  }
}

