// File:	Standard.cxx
// Created:	Tue Mar 15 17:33:31 2005
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <Standard.ixx>

#include <stdlib.h>

#include <Standard_MMgrOpt.hxx>
#include <Standard_MMgrRaw.hxx>
#include <Standard_MMgrTBBalloc.hxx>

// Global reentrant flag
static Standard_Boolean Standard_IsReentrant = Standard_False;

//=======================================================================
//class    : Standard_MMgrFactory 
//purpose  : Container for pointer to memory manager;
//           used to construct appropriate memory manager according
//           to environment settings, and to ensure destruction upon exit
//=======================================================================

class Standard_MMgrFactory {
 public:
  Standard_MMgrFactory();
  ~Standard_MMgrFactory();
 public:
  Standard_MMgrRoot* myFMMgr;
};

//=======================================================================
//function : Standard_MMgrFactory
//purpose  : Check environment variables and create appropriate memory manager
//=======================================================================

Standard_MMgrFactory::Standard_MMgrFactory() : myFMMgr(0)
{
  char *var;
  Standard_Boolean bClear, bMMap, bReentrant;
  Standard_Integer aCellSize, aNbPages, aThreshold, bOptAlloc;
  //
  bOptAlloc   = atoi((var = getenv("MMGT_OPT"      )) ? var : "1"    ); 
  bClear      = atoi((var = getenv("MMGT_CLEAR"    )) ? var : "1"    );
  bMMap       = atoi((var = getenv("MMGT_MMAP"     )) ? var : "1"    ); 
  aCellSize   = atoi((var = getenv("MMGT_CELLSIZE" )) ? var : "200"  ); 
  aNbPages    = atoi((var = getenv("MMGT_NBPAGES"  )) ? var : "1000" );
  aThreshold  = atoi((var = getenv("MMGT_THRESHOLD")) ? var : "40000");
  bReentrant  = atoi((var = getenv("MMGT_REENTRANT")) ? var : "0"    );
  
  if ( bOptAlloc == 1 ) { 
    myFMMgr = new Standard_MMgrOpt(bClear, bMMap, aCellSize, aNbPages,
                                   aThreshold, bReentrant);
  }
  else if ( bOptAlloc == 2 ) {
    myFMMgr = new Standard_MMgrTBBalloc(bClear);
  }
  else {
    myFMMgr = new Standard_MMgrRaw(bClear);
  }

  // Set grobal reentrant flag according to MMGT_REENTRANT environment variable
  if ( ! Standard_IsReentrant )
    Standard_IsReentrant = bReentrant;
}

//=======================================================================
//function : ~Standard_MMgrFactory
//purpose  : 
//=======================================================================

Standard_MMgrFactory::~Standard_MMgrFactory()
{
  if ( ! myFMMgr ) {
    myFMMgr->Purge(Standard_True);
//  delete myFMMgr;
//  myFMMgr = 0;  
  }
}

//=======================================================================
// function: GetMMgr
//
// This static function has a purpose to wrap static holder for memory 
// manager instance. 
//
// Wrapping holder inside a function is needed to ensure that it will
// be initialized not later than the first call to memory manager (that
// would be impossible to guarantee if holder was static variable on 
// global or file scope, because memory manager may be called from 
// constructors of other static objects).
//
// Note that at the same time we could not guarantee that the holder 
// object is destroyed after last call to memory manager, since that 
// last call may be from static Handle() object which has been initialized
// dynamically during program execution rather than in its constructor.
//
// Therefore holder currently does not call destructor of the memory manager 
// but only its method Purge() with Standard_True.
//
// To free the memory completely, we probably could use compiler-specific 
// pragmas (such as '#pragma fini' on SUN Solaris and '#pragma init_seg' on 
// WNT MSVC++) to put destructing function in code segment that is called
// after destructors of other (even static) objects. However, this is not 
// done by the moment since it is compiler-dependent and there is no guarantee 
// thatsome other object calling memory manager is not placed also in that segment...
//
// Note that C runtime function atexit() could not help in this problem 
// since its behaviour is the same as for destructors of static objects 
// (see ISO 14882:1998 "Programming languages -- C++" 3.6.3)
//
// The correct approach to deal with the problem would be to have memory manager 
// to properly control its memory allocation and caching free blocks so 
// as to release all memory as soon as it is returned to it, and probably
// even delete itself if all memory it manages has been released and 
// last call to method Purge() was with True.
//
// Note that one possible method to control memory allocations could
// be counting calls to Allocate() and Free()...
//
//=======================================================================

static Standard_MMgrRoot* GetMMgr()
{
  static Standard_MMgrFactory aFactory;
  return aFactory.myFMMgr;
}

//=======================================================================
//function : Allocate
//purpose  : 
//=======================================================================

Standard_Address Standard::Allocate(const Standard_Size size)
{
  return GetMMgr()->Allocate(size);
}

//=======================================================================
//function : Free
//purpose  : 
//=======================================================================

void Standard::Free(Standard_Address& aStorage)
{
  GetMMgr()->Free(aStorage);
}

//=======================================================================
//function : Reallocate
//purpose  : 
//=======================================================================

Standard_Address Standard::Reallocate(Standard_Address& aStorage,
				      const Standard_Size newSize)
{
  return GetMMgr()->Reallocate(aStorage, newSize);
}

//=======================================================================
//function : Purge
//purpose  : 
//=======================================================================

Standard_Integer Standard::Purge()
{
  return GetMMgr()->Purge();
}

//=======================================================================
//function : IsReentrant
//purpose  : 
//=======================================================================

Standard_Boolean Standard::IsReentrant()
{
  return Standard_IsReentrant;
}

//=======================================================================
//function : SetReentrant
//purpose  : 
//=======================================================================

void Standard::SetReentrant (const Standard_Boolean isReentrant)
{
  Standard_IsReentrant = isReentrant;
  GetMMgr()->SetReentrant (isReentrant);
}
