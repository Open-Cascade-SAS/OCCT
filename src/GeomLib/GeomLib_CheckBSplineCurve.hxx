// Created on: 1997-05-28
// Created by: Xavier BENVENISTE
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _GeomLib_CheckBSplineCurve_HeaderFile
#define _GeomLib_CheckBSplineCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

class Geom_BSplineCurve;


//! Checks for the end  tangents : whether or not those
//! are reversed regarding the third or n-3rd control
class GeomLib_CheckBSplineCurve 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT GeomLib_CheckBSplineCurve(const Handle(Geom_BSplineCurve)& Curve,
                                            const Standard_Real Tolerance,
                                            const Standard_Real AngularTolerance);
  
    Standard_Boolean IsDone() const;
  
  Standard_EXPORT void NeedTangentFix (Standard_Boolean& FirstFlag, Standard_Boolean& SecondFlag) const;
  
  Standard_EXPORT void FixTangent (const Standard_Boolean FirstFlag, const Standard_Boolean LastFlag);
  
  //! modifies the curve
  //! by fixing the first or the last tangencies
  //!
  //! if Index3D not in the Range [1,Nb3dSpaces]
  //! if the Approx is not Done
  Standard_EXPORT Handle(Geom_BSplineCurve) FixedTangent (const Standard_Boolean FirstFlag, const Standard_Boolean LastFlag);




protected:





private:

  void FixTangentOnCurve(Handle(Geom_BSplineCurve)& theCurve,
                         const Standard_Boolean FirstFlag,
                         const Standard_Boolean LastFlag);


  Handle(Geom_BSplineCurve) myCurve;
  Standard_Boolean myDone;
  Standard_Boolean myFixFirstTangent;
  Standard_Boolean myFixLastTangent;
  Standard_Real myAngularTolerance;
  Standard_Real myTolerance;

  Standard_Integer myIndSecondPole;
  Standard_Integer myIndPrelastPole;
};


#include <GeomLib_CheckBSplineCurve.lxx>





#endif // _GeomLib_CheckBSplineCurve_HeaderFile
