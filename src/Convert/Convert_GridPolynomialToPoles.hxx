// Created on: 1996-07-08
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

#ifndef _Convert_GridPolynomialToPoles_HeaderFile
#define _Convert_GridPolynomialToPoles_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColgp_HArray2OfPnt.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <TColStd_HArray2OfInteger.hxx>
class Standard_DomainError;
class StdFail_NotDone;


//! Convert a grid of Polynomial Surfaces
//! that are have continuity CM to an
//! Bspline Surface that has continuity
//! CM
class Convert_GridPolynomialToPoles 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! To    only  one   polynomial  Surface.
  //! The  Length  of  <PolynomialUIntervals> and <PolynomialVIntervals>
  //! have to be 2.
  //! This values defined the parametric domain of the Polynomial Equation.
  //!
  //! Coefficients :
  //! The <Coefficients> have to be formated than an "C array"
  //! [MaxUDegree+1] [MaxVDegree+1] [3]
  Standard_EXPORT Convert_GridPolynomialToPoles(const Standard_Integer MaxUDegree, const Standard_Integer MaxVDegree, const Handle(TColStd_HArray1OfInteger)& NumCoeff, const Handle(TColStd_HArray1OfReal)& Coefficients, const Handle(TColStd_HArray1OfReal)& PolynomialUIntervals, const Handle(TColStd_HArray1OfReal)& PolynomialVIntervals);
  
  //! To one grid of polynomial Surface.
  //! Warning!
  //! Continuity in each parametric direction can be at MOST the
  //! maximum degree of the polynomial functions.
  //!
  //! <TrueUIntervals>, <TrueVIntervals> :
  //! this is the true parameterisation for the composite surface
  //!
  //! Coefficients :
  //! The Coefficients have to be formated than an "C array"
  //! [NbVSurfaces] [NBUSurfaces] [MaxUDegree+1] [MaxVDegree+1] [3]
  //! raises DomainError    if <NumCoeffPerSurface> is not a
  //! [1, NbVSurfaces*NbUSurfaces, 1,2] array.
  //! if <Coefficients> is not a
  Standard_EXPORT Convert_GridPolynomialToPoles(const Standard_Integer NbUSurfaces, const Standard_Integer NBVSurfaces, const Standard_Integer UContinuity, const Standard_Integer VContinuity, const Standard_Integer MaxUDegree, const Standard_Integer MaxVDegree, const Handle(TColStd_HArray2OfInteger)& NumCoeffPerSurface, const Handle(TColStd_HArray1OfReal)& Coefficients, const Handle(TColStd_HArray1OfReal)& PolynomialUIntervals, const Handle(TColStd_HArray1OfReal)& PolynomialVIntervals, const Handle(TColStd_HArray1OfReal)& TrueUIntervals, const Handle(TColStd_HArray1OfReal)& TrueVIntervals);
  
  Standard_EXPORT void Perform (const Standard_Integer UContinuity, const Standard_Integer VContinuity, const Standard_Integer MaxUDegree, const Standard_Integer MaxVDegree, const Handle(TColStd_HArray2OfInteger)& NumCoeffPerSurface, const Handle(TColStd_HArray1OfReal)& Coefficients, const Handle(TColStd_HArray1OfReal)& PolynomialUIntervals, const Handle(TColStd_HArray1OfReal)& PolynomialVIntervals, const Handle(TColStd_HArray1OfReal)& TrueUIntervals, const Handle(TColStd_HArray1OfReal)& TrueVIntervals);
  
  Standard_EXPORT Standard_Integer NbUPoles() const;
  
  Standard_EXPORT Standard_Integer NbVPoles() const;
  
  //! returns the poles of the BSpline Surface
  Standard_EXPORT const Handle(TColgp_HArray2OfPnt)& Poles() const;
  
  Standard_EXPORT Standard_Integer UDegree() const;
  
  Standard_EXPORT Standard_Integer VDegree() const;
  
  Standard_EXPORT Standard_Integer NbUKnots() const;
  
  Standard_EXPORT Standard_Integer NbVKnots() const;
  
  //! Knots in the U direction
  Standard_EXPORT const Handle(TColStd_HArray1OfReal)& UKnots() const;
  
  //! Knots in the V direction
  Standard_EXPORT const Handle(TColStd_HArray1OfReal)& VKnots() const;
  
  //! Multiplicities of the knots in the U direction
  Standard_EXPORT const Handle(TColStd_HArray1OfInteger)& UMultiplicities() const;
  
  //! Multiplicities of the knots in the V direction
  Standard_EXPORT const Handle(TColStd_HArray1OfInteger)& VMultiplicities() const;
  
  Standard_EXPORT Standard_Boolean IsDone() const;




protected:





private:

  
  Standard_EXPORT void BuildArray (const Standard_Integer Degree, const Handle(TColStd_HArray1OfReal)& Knots, const Standard_Integer Continuty, Handle(TColStd_HArray1OfReal)& FlatKnots, Handle(TColStd_HArray1OfInteger)& Mults, Handle(TColStd_HArray1OfReal)& Parameters) const;


  Handle(TColStd_HArray1OfReal) myUFlatKnots;
  Handle(TColStd_HArray1OfReal) myVFlatKnots;
  Handle(TColStd_HArray1OfReal) myUKnots;
  Handle(TColStd_HArray1OfReal) myVKnots;
  Handle(TColStd_HArray1OfInteger) myUMults;
  Handle(TColStd_HArray1OfInteger) myVMults;
  Handle(TColgp_HArray2OfPnt) myPoles;
  Standard_Integer myUDegree;
  Standard_Integer myVDegree;
  Standard_Boolean myDone;


};







#endif // _Convert_GridPolynomialToPoles_HeaderFile
