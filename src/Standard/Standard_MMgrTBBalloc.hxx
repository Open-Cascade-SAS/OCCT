// File:	Standard_MMgrTBBalloc.hxx
// Created:	Tue Mar 15 12:05:43 2010
// Author:	Sergey KUUL

#ifndef _Standard_MMgrTBBalloc_HeaderFile
#define _Standard_MMgrTBBalloc_HeaderFile

#ifndef _Standard_Address_HeaderFile
#include <Standard_Address.hxx>
#endif
#ifndef _Standard_Integer_HeaderFile
#include <Standard_Integer.hxx>
#endif
#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif
#ifndef _Standard_MMgrRoot_HeaderFile
#include <Standard_MMgrRoot.hxx>
#endif

//!
//! Implementation of OCC memory manager which uses Intel TBB
//! scalable allocator.
//!
//! On configurations where TBB is not available standard RTL functions 
//! malloc() / free() are used.

class Standard_MMgrTBBalloc : public Standard_MMgrRoot
{
 public:
  //! Constructor; if aClear is True, the memory will be nullified
  //! upon allocation.
  Standard_EXPORT Standard_MMgrTBBalloc(const Standard_Boolean aClear=Standard_False);

  //! Allocate aSize bytes 
  Standard_EXPORT virtual Standard_Address Allocate(const Standard_Size aSize);
  
  //! Reallocate aPtr to the size aSize. 
  //! The pointer aPtr is nullified; new pointer is returned.
  Standard_EXPORT virtual Standard_Address Reallocate(Standard_Address& aPtr, 
						      const Standard_Size aSize);
  
  //! Free allocated memory. The pointer is nullified.
  Standard_EXPORT virtual void Free(Standard_Address&);

 protected:
  Standard_Boolean myClear; //! Option to nullify allocated memory
};

#endif
