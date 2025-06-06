// Created on: 2002-03-04
// Created by: Michael KLOKOV
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#ifndef _BOPAlgo_SectionAttribute_HeaderFile
#define _BOPAlgo_SectionAttribute_HeaderFile

//! Class is a container of the flags used
//! by intersection algorithm
class BOPAlgo_SectionAttribute
{
public:
  //! Default constructor
  BOPAlgo_SectionAttribute()
      : myApproximation(Standard_True),
        myPCurve1(Standard_True),
        myPCurve2(Standard_True)
  {
  }

  //! Constructor
  BOPAlgo_SectionAttribute(const Standard_Boolean theAproximation,
                           const Standard_Boolean thePCurveOnS1,
                           const Standard_Boolean thePCurveOnS2)
      : myApproximation(theAproximation),
        myPCurve1(thePCurveOnS1),
        myPCurve2(thePCurveOnS2)
  {
  }

  //! Sets the Approximation flag
  void Approximation(const Standard_Boolean theApprox) { myApproximation = theApprox; }

  //! Sets the PCurveOnS1 flag
  void PCurveOnS1(const Standard_Boolean thePCurveOnS1) { myPCurve1 = thePCurveOnS1; }

  //! Sets the PCurveOnS2 flag
  void PCurveOnS2(const Standard_Boolean thePCurveOnS2) { myPCurve2 = thePCurveOnS2; }

  //! Returns the Approximation flag
  Standard_Boolean Approximation() const { return myApproximation; }

  //! Returns the PCurveOnS1 flag
  Standard_Boolean PCurveOnS1() const { return myPCurve1; }

  //! Returns the PCurveOnS2 flag
  Standard_Boolean PCurveOnS2() const { return myPCurve2; }

protected:
private:
  Standard_Boolean myApproximation;
  Standard_Boolean myPCurve1;
  Standard_Boolean myPCurve2;
};

#endif // _BOPAlgo_SectionAttribute_HeaderFile
