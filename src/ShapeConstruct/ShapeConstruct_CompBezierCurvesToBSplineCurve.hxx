// Created on: 1993-10-20
// Created by: Bruno DUMORTIER
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _ShapeConstruct_CompBezierCurvesToBSplineCurve_HeaderFile
#define _ShapeConstruct_CompBezierCurvesToBSplineCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Convert_SequenceOfArray1OfPoles.hxx>
#include <TColgp_SequenceOfPnt.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <TColStd_SequenceOfInteger.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
class Standard_ConstructionError;


//! Converts a list  of connecting Bezier Curves to  a
//! BSplineCurve.
//! if possible, the continuity of the BSpline will be
//! increased to more than C0.
class ShapeConstruct_CompBezierCurvesToBSplineCurve 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT ShapeConstruct_CompBezierCurvesToBSplineCurve(const Standard_Real AngularTolerance = 1.0e-4);
  
  Standard_EXPORT void AddCurve (const TColgp_Array1OfPnt& Poles);
  
  //! Computes the algorithm.
  Standard_EXPORT void Perform();
  
  Standard_EXPORT Standard_Integer Degree() const;
  
  Standard_EXPORT Standard_Integer NbPoles() const;
  
  Standard_EXPORT void Poles (TColgp_Array1OfPnt& Poles) const;
  
  Standard_EXPORT Standard_Integer NbKnots() const;
  
  Standard_EXPORT void KnotsAndMults (TColStd_Array1OfReal& Knots, TColStd_Array1OfInteger& Mults) const;




protected:





private:



  Convert_SequenceOfArray1OfPoles mySequence;
  TColgp_SequenceOfPnt CurvePoles;
  TColStd_SequenceOfReal CurveKnots;
  TColStd_SequenceOfInteger KnotsMultiplicities;
  Standard_Integer myDegree;
  Standard_Real myAngular;
  Standard_Boolean myDone;


};







#endif // _ShapeConstruct_CompBezierCurvesToBSplineCurve_HeaderFile
