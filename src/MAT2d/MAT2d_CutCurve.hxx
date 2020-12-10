// Created on: 1994-09-23
// Created by: Yves FRICAUD
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _MAT2d_CutCurve_HeaderFile
#define _MAT2d_CutCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TColGeom2d_SequenceOfCurve.hxx>
#include <MAT_Side.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
class Standard_OutOfRange;
class Geom2d_Curve;
class Geom2d_TrimmedCurve;


//! Cuts a curve at the extremas of curvature
//! and at the inflections. Constructs a trimmed
//! Curve for each interval.
class MAT2d_CutCurve 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT MAT2d_CutCurve();
  
  Standard_EXPORT MAT2d_CutCurve(const Handle(Geom2d_Curve)& C);
  
  //! Cuts a curve at the extremas of curvature
  //! and at the inflections.
  Standard_EXPORT void Perform (const Handle(Geom2d_Curve)& C);
  
  //! Cuts a curve at the inflections, and at the extremas
  //! of curvature where the concavity is on <aSide>.
  Standard_EXPORT void Perform (const Handle(Geom2d_Curve)& C, const MAT_Side aSide);
  
  //! Cuts a curve at the inflections.
  Standard_EXPORT void PerformInf (const Handle(Geom2d_Curve)& C);
  
  //! Returns True if the curve is not cut.
  Standard_EXPORT Standard_Boolean UnModified() const;
  
  //! Returns the number of curves.
  //! it's always greatest than 2.
  //!
  //! raises if the Curve is UnModified;
  Standard_EXPORT Standard_Integer NbCurves() const;
  
  //! Returns the Indexth curve.
  //! raises if Index not in the range [1,NbCurves()]
  Standard_EXPORT Handle(Geom2d_TrimmedCurve) Value (const Standard_Integer Index) const;




protected:





private:



  TColGeom2d_SequenceOfCurve theCurves;


};







#endif // _MAT2d_CutCurve_HeaderFile
