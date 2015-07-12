// Created on: 1992-05-13
// Created by: NW,JPB,CAL
// Copyright (c) 1992-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef _Aspect_GenId_HeaderFile
#define _Aspect_GenId_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <Standard_Boolean.hxx>
class Aspect_IdentDefinitionError;


//! This class permits the creation and control of integer identifiers.
class Aspect_GenId 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates an available set of identifiers with the lower bound 0 and the upper bound INT_MAX / 2.
  Standard_EXPORT Aspect_GenId();
  
  //! Creates an available set of identifiers with specified range.
  //! Raises IdentDefinitionError if theUpper is less than theLow.
  Standard_EXPORT Aspect_GenId(const Standard_Integer theLow, const Standard_Integer theUpper);
  
  //! Free all identifiers - make the whole range available again.
  Standard_EXPORT void Free();
  
  //! Free specified identifier. Warning - method has no protection against double-freeing!
  Standard_EXPORT void Free (const Standard_Integer theId);
  
  //! Returns true if there are available identifiers in range.
  Standard_EXPORT Standard_Boolean HasFree() const;
  
  //! Returns the number of available identifiers.
  Standard_EXPORT Standard_Integer Available() const;
  
  //! Returns the lower identifier in range.
  Standard_EXPORT Standard_Integer Lower() const;
  
  //! Returns the next available identifier.
  //! Warning: Raises IdentDefinitionError if all identifiers are busy.
  Standard_EXPORT Standard_Integer Next();
  
  //! Returns the upper identifier in range.
  Standard_EXPORT Standard_Integer Upper() const;




protected:





private:



  Standard_Integer myFreeCount;
  Standard_Integer myLength;
  Standard_Integer myLowerBound;
  Standard_Integer myUpperBound;
  TColStd_ListOfInteger myFreeIds;


};







#endif // _Aspect_GenId_HeaderFile
