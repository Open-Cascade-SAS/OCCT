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

  static Standard_Integer HashCode(const IntPolyh_Couple& theCouple,
                                   const Standard_Integer Upper)
  {
    return theCouple.HashCode(Upper);
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
