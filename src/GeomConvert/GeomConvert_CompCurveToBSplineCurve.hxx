// Created on: 1996-09-23
// Created by: Philippe MANGIN
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _GeomConvert_CompCurveToBSplineCurve_HeaderFile
#define _GeomConvert_CompCurveToBSplineCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <Convert_ParameterisationType.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
class Geom_BSplineCurve;
class Geom_BoundedCurve;


//! Algorithm converts and concat several curve in an BSplineCurve
class GeomConvert_CompCurveToBSplineCurve 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Initialize the algorithme
  //! - Parameterisation is used to convert
  Standard_EXPORT GeomConvert_CompCurveToBSplineCurve(const Convert_ParameterisationType Parameterisation = Convert_TgtThetaOver2);
  
  //! Initialize the algorithme with one curve
  //! - Parameterisation is used to convert
  Standard_EXPORT GeomConvert_CompCurveToBSplineCurve(const Handle(Geom_BoundedCurve)& BasisCurve, const Convert_ParameterisationType Parameterisation = Convert_TgtThetaOver2);
  
  //! Append a curve in  the BSpline Return False if the
  //! curve is not G0  with the BSplineCurve.  Tolerance
  //! is   used to   check   continuity  and decrease
  //! Multiplicity at the common Knot until MinM
  //! if MinM = 0, the common Knot can be removed
  //!
  //! WithRatio defines whether the resulting curve should have a uniform
  //! parameterization. Setting WithRatio to Standard_False may greatly
  //! decrease the speed of algorithms like CPnts_AbscissaPoint::AdvPerform
  //! when applied to the resulting curve.
  Standard_EXPORT Standard_Boolean Add (const Handle(Geom_BoundedCurve)& NewCurve, const Standard_Real Tolerance, const Standard_Boolean After = Standard_False, const Standard_Boolean WithRatio = Standard_True, const Standard_Integer MinM = 0);
  
  Standard_EXPORT Handle(Geom_BSplineCurve) BSplineCurve() const;
  
  //! Clear a result curve
  Standard_EXPORT void Clear();




protected:





private:

  
  //! Concat two BSplineCurves.
  Standard_EXPORT void Add (Handle(Geom_BSplineCurve)& FirstCurve, Handle(Geom_BSplineCurve)& SecondCurve, const Standard_Boolean After, const Standard_Boolean WithRatio, const Standard_Integer MinM);


  Handle(Geom_BSplineCurve) myCurve;
  Standard_Real myTol;
  Convert_ParameterisationType myType;


};







#endif // _GeomConvert_CompCurveToBSplineCurve_HeaderFile
