// Created on: 2010-03-15
// Created by: Sergey KUUL
// Copyright (c) 2010-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


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
