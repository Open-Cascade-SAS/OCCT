// Created by: Eugeny MALTCHIKOV
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

#ifndef _BOPDS_PaveMapHasher_HeaderFile
#define _BOPDS_PaveMapHasher_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class BOPDS_Pave;



class BOPDS_PaveMapHasher 
{
public:

  DEFINE_STANDARD_ALLOC

  //! Computes a hash code for the given pave, in the range [1, theUpperBound]
  //! @param thePave the pave which hash code is to be computed
  //! @param theUpperBound the upper bound of the range a computing hash code must be within
  //! @return a computed hash code, in the range [1, theUpperBound]
  static Standard_Integer HashCode (const BOPDS_Pave& thePave, Standard_Integer theUpperBound);

  static Standard_Boolean IsEqual (const BOPDS_Pave& aPave1, const BOPDS_Pave& aPave2);




protected:





private:





};


#include <BOPDS_PaveMapHasher.lxx>





#endif // _BOPDS_PaveMapHasher_HeaderFile
