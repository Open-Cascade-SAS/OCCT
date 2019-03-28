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

#ifndef _IntPolyh_CoupleMapHasher_HeaderFile
#define _IntPolyh_CoupleMapHasher_HeaderFile

#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <IntPolyh_Couple.hxx>

class IntPolyh_Couple;

class IntPolyh_CoupleMapHasher 
{
public:
  
  //! Computes a hash code for the given couple, in the range [1, theUpperBound]
  //! @param theCouple the couple which hash code is to be computed
  //! @param theUpperBound the upper bound of the range a computing hash code must be within
  //! @return a computed hash code, in the range [1, theUpperBound]
  static Standard_Integer HashCode (const IntPolyh_Couple& theCouple, const Standard_Integer theUpperBound)
  {
    return theCouple.HashCode (theUpperBound);
  }

  static Standard_Boolean IsEqual(const IntPolyh_Couple& theCouple1,
                                  const IntPolyh_Couple& theCouple2)
  {
    return theCouple1.IsEqual(theCouple2);
  }

protected:

private:

};

#endif // _IntPolyh_CoupleMapHasher_HeaderFile
