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

#ifndef _IGESGeom_SplineCurve_HeaderFile
#define _IGESGeom_SplineCurve_HeaderFile

#include <Standard.hxx>

#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_HArray2.hxx>
#include <IGESData_IGESEntity.hxx>
#include <Standard_Real.hxx>

//! Defines IGESSplineCurve, Type <112> Form <0>
//! in package IGESGeom
//! The parametric spline is a sequence of parametric
//! polynomial segments. The curve could be of the type
//! Linear, Quadratic, Cubic, Wilson-Fowler, Modified
//! Wilson-Fowler, B-Spline. The N polynomial segments
//! are delimited by the break points:
//! T(1), T(2), T(3), ..., T(N+1).
class IGESGeom_SplineCurve : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESGeom_SplineCurve();

  //! This method is used to set the fields of the class
  //! SplineCurve
  //! - aType           : Spline Type
  //! 1 = Linear
  //! 2 = Quadratic
  //! 3 = Cubic
  //! 4 = Wilson-Fowler
  //! 5 = Modified Wilson-Fowler
  //! 6 = B Spline
  //! - aDegree         : Degree of continuity w.r.t. arc length
  //! - nbDimensions    : Number of dimensions
  //! 2 = Planar
  //! 3 = Non-planar
  //! - allBreakPoints  : Array of break points
  //! - allXPolynomials : X coordinate polynomials of segments
  //! - allYPolynomials : Y coordinate polynomials of segments
  //! - allZPolynomials : Z coordinate polynomials of segments
  //! - allXValues      : Values of 1st, 2nd, 3rd derivatives of
  //! X polynomials at the terminate point
  //! and values of X at terminate point
  //! - allYValues      : Values of 1st, 2nd, 3rd derivatives of
  //! Y polynomials at the terminate point
  //! and values of Y at terminate point
  //! - allZvalues      : Values of 1st, 2nd, 3rd derivatives of
  //! Z polynomials at the terminate point
  //! and values of Z at terminate point
  //! raises exception if allXPolynomials, allYPolynomials
  //! & allZPolynomials are not of same size OR allXValues, allYValues
  //! & allZValues are not of size 4
  Standard_EXPORT void Init(const int                                       aType,
                            const int                                       aDegree,
                            const int                                       nbDimensions,
                            const occ::handle<NCollection_HArray1<double>>& allBreakPoints,
                            const occ::handle<NCollection_HArray2<double>>& allXPolynomials,
                            const occ::handle<NCollection_HArray2<double>>& allYPolynomials,
                            const occ::handle<NCollection_HArray2<double>>& allZPolynomials,
                            const occ::handle<NCollection_HArray1<double>>& allXvalues,
                            const occ::handle<NCollection_HArray1<double>>& allYvalues,
                            const occ::handle<NCollection_HArray1<double>>& allZvalues);

  //! returns the type of Spline curve
  Standard_EXPORT int SplineType() const;

  //! returns the degree of the curve
  Standard_EXPORT int Degree() const;

  //! returns the number of dimensions
  //! 2 = Planar
  //! 3 = Non-planar
  Standard_EXPORT int NbDimensions() const;

  //! returns the number of segments
  Standard_EXPORT int NbSegments() const;

  //! returns breakpoint of piecewise polynomial
  //! raises exception if Index <= 0 or Index > NbSegments() + 1
  Standard_EXPORT double BreakPoint(const int Index) const;

  //! returns X coordinate polynomial for segment referred to by Index
  //! raises exception if Index <= 0 or Index > NbSegments()
  Standard_EXPORT void XCoordPolynomial(const int Index,
                                        double&   AX,
                                        double&   BX,
                                        double&   CX,
                                        double&   DX) const;

  //! returns Y coordinate polynomial for segment referred to by Index
  //! raises exception if Index <= 0 or Index > NbSegments()
  Standard_EXPORT void YCoordPolynomial(const int Index,
                                        double&   AY,
                                        double&   BY,
                                        double&   CY,
                                        double&   DY) const;

  //! returns Z coordinate polynomial for segment referred to by Index
  //! raises exception if Index <= 0 or Index > NbSegments()
  Standard_EXPORT void ZCoordPolynomial(const int Index,
                                        double&   AZ,
                                        double&   BZ,
                                        double&   CZ,
                                        double&   DZ) const;

  //! returns the value of X polynomial, the values of 1st, 2nd and
  //! 3rd derivatives of the X polynomial at the terminate point
  Standard_EXPORT void XValues(double& TPX0, double& TPX1, double& TPX2, double& TPX3) const;

  //! returns the value of Y polynomial, the values of 1st, 2nd and
  //! 3rd derivatives of the Y polynomial at the termminate point
  Standard_EXPORT void YValues(double& TPY0, double& TPY1, double& TPY2, double& TPY3) const;

  //! returns the value of Z polynomial, the values of 1st, 2nd and
  //! 3rd derivatives of the Z polynomial at the termminate point
  Standard_EXPORT void ZValues(double& TPZ0, double& TPZ1, double& TPZ2, double& TPZ3) const;

  DEFINE_STANDARD_RTTIEXT(IGESGeom_SplineCurve, IGESData_IGESEntity)

private:
  int                                      theType;
  int                                      theDegree;
  int                                      theNbDimensions;
  occ::handle<NCollection_HArray1<double>> theBreakPoints;
  occ::handle<NCollection_HArray2<double>> theXCoordsPolynomial;
  occ::handle<NCollection_HArray2<double>> theYCoordsPolynomial;
  occ::handle<NCollection_HArray2<double>> theZCoordsPolynomial;
  occ::handle<NCollection_HArray1<double>> theXvalues;
  occ::handle<NCollection_HArray1<double>> theYvalues;
  occ::handle<NCollection_HArray1<double>> theZvalues;
};

#endif // _IGESGeom_SplineCurve_HeaderFile
