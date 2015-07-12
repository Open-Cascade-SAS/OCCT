// Created on: 1997-04-29
// Created by: Stagiaire Francois DUMONT
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

#ifndef _Geom2dConvert_CompCurveToBSplineCurve_HeaderFile
#define _Geom2dConvert_CompCurveToBSplineCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <Convert_ParameterisationType.hxx>
#include <Standard_Boolean.hxx>
class Geom2d_BSplineCurve;
class Geom2d_BoundedCurve;


//! This algorithm converts and concat several curve in an BSplineCurve
class Geom2dConvert_CompCurveToBSplineCurve 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Initialize the algorithme
  //! - Parameterisation is used to convert
  Standard_EXPORT Geom2dConvert_CompCurveToBSplineCurve(const Convert_ParameterisationType Parameterisation = Convert_TgtThetaOver2);
  
  //! Initialize the algorithme with one curve
  //! - Parameterisation is used to convert
  Standard_EXPORT Geom2dConvert_CompCurveToBSplineCurve(const Handle(Geom2d_BoundedCurve)& BasisCurve, const Convert_ParameterisationType Parameterisation = Convert_TgtThetaOver2);
  
  //! Append a curve in the BSpline
  //! Return False if the curve is not G0 with the BSplineCurve.
  //! Tolerance is used to check continuity and decrease
  //! Multiplicty at the common Knot
  //! After is usefull if BasisCurve is a closed curve .
  Standard_EXPORT Standard_Boolean Add (const Handle(Geom2d_BoundedCurve)& NewCurve, const Standard_Real Tolerance, const Standard_Boolean After = Standard_False);
  
  Standard_EXPORT Handle(Geom2d_BSplineCurve) BSplineCurve() const;
  
  //! Clear result curve
  Standard_EXPORT void Clear();




protected:





private:

  
  //! Concat two BSplineCurves.
  Standard_EXPORT void Add (Handle(Geom2d_BSplineCurve)& FirstCurve, Handle(Geom2d_BSplineCurve)& SecondCurve, const Standard_Boolean After);


  Handle(Geom2d_BSplineCurve) myCurve;
  Standard_Real myTol;
  Convert_ParameterisationType myType;


};







#endif // _Geom2dConvert_CompCurveToBSplineCurve_HeaderFile
