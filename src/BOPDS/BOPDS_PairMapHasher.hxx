// Created by: Eugeny MALTCHIKOV
// Copyright (c) 2017 OPEN CASCADE SAS
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

#ifndef _BOPDS_PairMapHasher_HeaderFile
#define _BOPDS_PairMapHasher_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <BOPDS_Pair.hxx>

class BOPDS_Pair;

class BOPDS_PairMapHasher 
{
public:

  DEFINE_STANDARD_ALLOC
 
  static Standard_Integer HashCode(const BOPDS_Pair& thePair,
                                   const Standard_Integer Upper)
  {
    return thePair.HashCode(Upper);
  }
 
  static Standard_Boolean IsEqual(const BOPDS_Pair& thePair1,
                                  const BOPDS_Pair& thePair2)
  {
    return thePair1.IsEqual(thePair2);
  }

protected:

private:

};

#endif // _BOPDS_PairMapHasher_HeaderFile
