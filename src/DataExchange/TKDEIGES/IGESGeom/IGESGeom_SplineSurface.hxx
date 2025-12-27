// Created on: 1993-01-09
// Created by: CKY / Contract Toubro-Larsen ( Kiran )
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

#ifndef _IGESGeom_SplineSurface_HeaderFile
#define _IGESGeom_SplineSurface_HeaderFile

#include <Standard.hxx>

#include <Standard_Integer.hxx>
#include <IGESBasic_HArray2OfHArray1OfReal.hxx>
#include <IGESData_IGESEntity.hxx>
#include <Standard_Real.hxx>

//! defines IGESSplineSurface, Type <114> Form <0>
//! in package IGESGeom
//! A parametric spline surface is a grid of polynomial
//! patches. Patch could be of the type Linear, Quadratic,
//! Cubic, Wilson-Fowler, Modified Wilson-Fowler, B-Spline
//! The M * N grid of patches is defined by the 'u' break
//! points TU(1), TU(2), ..., TU(M+1) and the 'v' break
//! points TV(1), TV(2), TV(3) ..., TV(N+1).
class IGESGeom_SplineSurface : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESGeom_SplineSurface();

  //! This method is used to set the fields of the class
  //! SplineSurface
  //! - aBoundaryType   : Type of Spline boundary
  //! 1 = Linear
  //! 2 = Quadratic
  //! 3 = Cubic
  //! 4 = Wilson-Fowler
  //! 5 = Modified Wilson-Fowler
  //! 6 = B-spline
  //! - aPatchType      : Type of patch contained in the grid
  //! 1 = Cartesian Product
  //! 0 = Unspecified
  //! - allUBreakpoints : u values of grid lines
  //! - allVBreakpoints : v values of grid lines
  //! - allXCoeffs      : X coefficients of M x N patches
  //! - allYCoeffs      : Y coefficients of M x N patches
  //! - allZCoeffs      : Z coefficients of M x N patches
  //! raises exception if allXCoeffs, allYCoeffs & allZCoeffs are not
  //! of the same size.
  //! or if the size of each element of the double array is not 16
  Standard_EXPORT void Init(const int                          aBoundaryType,
                            const int                          aPatchType,
                            const occ::handle<TColStd_HArray1OfReal>&            allUBreakpoints,
                            const occ::handle<TColStd_HArray1OfReal>&            allVBreakpoints,
                            const occ::handle<IGESBasic_HArray2OfHArray1OfReal>& allXCoeffs,
                            const occ::handle<IGESBasic_HArray2OfHArray1OfReal>& allYCoeffs,
                            const occ::handle<IGESBasic_HArray2OfHArray1OfReal>& allZCoeffs);

  //! returns the number of U segments
  Standard_EXPORT int NbUSegments() const;

  //! returns the number of V segments
  Standard_EXPORT int NbVSegments() const;

  //! returns boundary type
  Standard_EXPORT int BoundaryType() const;

  //! returns patch type
  Standard_EXPORT int PatchType() const;

  //! returns U break point of the grid line referred to by anIndex
  //! raises exception if anIndex <= 0 or anIndex > NbUSegments() + 1
  Standard_EXPORT double UBreakPoint(const int anIndex) const;

  //! returns V break point of the grid line referred to by anIndex
  //! raises exception if anIndex <= 0 or anIndex > NbVSegments() + 1
  Standard_EXPORT double VBreakPoint(const int anIndex) const;

  //! returns X polynomial of patch referred to by anIndex1, anIndex2
  //! raises exception if anIndex1 <= 0 or anIndex1 > NbUSegments()
  //! or anIndex2 <= 0 or anIndex2 > NbVSegments()
  Standard_EXPORT occ::handle<TColStd_HArray1OfReal> XPolynomial(const int anIndex1,
                                                            const int anIndex2) const;

  //! returns Y polynomial of patch referred to by anIndex1, anIndex2
  //! raises exception if anIndex1 <= 0 or anIndex1 > NbUSegments()
  //! or anIndex2 <= 0 or anIndex2 > NbVSegments()
  Standard_EXPORT occ::handle<TColStd_HArray1OfReal> YPolynomial(const int anIndex1,
                                                            const int anIndex2) const;

  //! returns Z polynomial of patch referred to by anIndex1, anIndex2
  //! raises exception if anIndex1 <= 0 or anIndex1 > NbUSegments()
  //! or anIndex2 <= 0 or anIndex2 > NbVSegments()
  Standard_EXPORT occ::handle<TColStd_HArray1OfReal> ZPolynomial(const int anIndex1,
                                                            const int anIndex2) const;

  //! returns in one all the polynomial values "in bulk"
  //! useful for massive treatments
  Standard_EXPORT void Polynomials(occ::handle<IGESBasic_HArray2OfHArray1OfReal>& XCoef,
                                   occ::handle<IGESBasic_HArray2OfHArray1OfReal>& YCoef,
                                   occ::handle<IGESBasic_HArray2OfHArray1OfReal>& ZCoef) const;

  DEFINE_STANDARD_RTTIEXT(IGESGeom_SplineSurface, IGESData_IGESEntity)

private:
  int                         theBoundaryType;
  int                         thePatchType;
  occ::handle<TColStd_HArray1OfReal>            theUBreakPoints;
  occ::handle<TColStd_HArray1OfReal>            theVBreakPoints;
  occ::handle<IGESBasic_HArray2OfHArray1OfReal> theXCoeffs;
  occ::handle<IGESBasic_HArray2OfHArray1OfReal> theYCoeffs;
  occ::handle<IGESBasic_HArray2OfHArray1OfReal> theZCoeffs;
};

#endif // _IGESGeom_SplineSurface_HeaderFile
