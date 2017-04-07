// Created on: 1999-04-08
// Created by: Fabrice SERVANT
// Copyright (c) 1999 Matra Datavision
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

#ifndef _IntPolyh_Couple_HeaderFile
#define _IntPolyh_Couple_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>

//! The class represents the couple of indices with additional
//! characteristics such as analyzed flag and an angle.<br>
//! In IntPolyh_MaillageAffinage algorithm the class is used as a
//! couple of interfering triangles with the intersection angle.
class IntPolyh_Couple
{
public:

  DEFINE_STANDARD_ALLOC

  //! Constructor
  IntPolyh_Couple() :
    myIndex1(-1),myIndex2(-1),myAnalyzed(0),myAngle(-2.0)
  {}
  //! Constructor
  IntPolyh_Couple(const Standard_Integer theTriangle1,
                  const Standard_Integer theTriangle2,
                  const Standard_Real theAngle = -2.0)
  :
    myIndex1(theTriangle1),
    myIndex2(theTriangle2),
    myAnalyzed(Standard_False),
    myAngle(theAngle)
  {}

  //! Returns the first index
  Standard_Integer FirstValue() const
  {
    return myIndex1;
  }
  //! Returns the second index
  Standard_Integer SecondValue() const
  {
    return myIndex2;
  }
  //! Returns TRUE if the couple has been analyzed
  Standard_Boolean IsAnalyzed() const
  {
    return myAnalyzed;
  }
  //! Returns the angle
  Standard_Real Angle() const
  {
    return myAngle;
  }
  //! Sets the triangles
  void SetCoupleValue(const Standard_Integer theInd1,
                      const Standard_Integer theInd2)
  {
    myIndex1 = theInd1;
    myIndex2 = theInd2;
  }
  //! Sets the analyzed flag
  void SetAnalyzed(const Standard_Boolean theAnalyzed)
  {
    myAnalyzed = theAnalyzed;
  }
  //! Sets the angle
  void SetAngle(const Standard_Real theAngle)
  {
    myAngle = theAngle;
  }
  //! Returns true if the Couple is equal to <theOther>
  Standard_Boolean IsEqual (const IntPolyh_Couple& theOther) const
  {
    return (myIndex1 == theOther.myIndex1 && myIndex2 == theOther.myIndex2) ||
           (myIndex1 == theOther.myIndex2 && myIndex2 == theOther.myIndex1);
  }
  //
  //! Returns hash code
  Standard_Integer HashCode (const Standard_Integer theUpper) const
  {
    return ::HashCode(myIndex1 + myIndex2, theUpper);
  }
  // Dump
  Standard_EXPORT void Dump (const Standard_Integer v) const;

protected:

private:

  Standard_Integer myIndex1;
  Standard_Integer myIndex2;
  Standard_Boolean myAnalyzed;
  Standard_Real myAngle;

};

#endif // _IntPolyh_Couple_HeaderFile
