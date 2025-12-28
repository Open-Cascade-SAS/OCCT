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

#ifndef _IGESGeom_BSplineSurface_HeaderFile
#define _IGESGeom_BSplineSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_HArray2.hxx>
#include <gp_XYZ.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_HArray2.hxx>
#include <IGESData_IGESEntity.hxx>
class gp_Pnt;

//! defines IGESBSplineSurface, Type <128> Form <0-9>
//! in package IGESGeom
//! A parametric equation obtained by dividing two summations
//! involving weights (which are real numbers), the control
//! points, and B-Spline basis functions
class IGESGeom_BSplineSurface : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESGeom_BSplineSurface();

  //! This method is used to set the fields of the class
  //! BSplineSurface
  //! - anIndexU             : Upper index of first sum
  //! - anIndexV             : Upper index of second sum
  //! - aDegU, aDegV         : Degrees of first and second sets
  //! of basis functions
  //! - aCloseU, aCloseV     : 1 = Closed in U, V directions
  //! 0 = open in U, V directions
  //! - aPolynom             : 0 = Rational, 1 = polynomial
  //! - aPeriodU, aPeriodV   : 0 = Non periodic in U or V direction
  //! 1 = Periodic in U or V direction
  //! - allKnotsU, allKnotsV : Knots in U and V directions
  //! - allWeights           : Array of weights
  //! - allPoles             : XYZ coordinates of all control points
  //! - aUmin                : Starting value of U direction
  //! - aUmax                : Ending value of U direction
  //! - aVmin                : Starting value of V direction
  //! - aVmax                : Ending value of V direction
  //! raises exception if allWeights & allPoles are not of same size.
  Standard_EXPORT void Init(const int               anIndexU,
                            const int               anIndexV,
                            const int               aDegU,
                            const int               aDegV,
                            const bool               aCloseU,
                            const bool               aCloseV,
                            const bool               aPolynom,
                            const bool               aPeriodU,
                            const bool               aPeriodV,
                            const occ::handle<NCollection_HArray1<double>>& allKnotsU,
                            const occ::handle<NCollection_HArray1<double>>& allKnotsV,
                            const occ::handle<NCollection_HArray2<double>>& allWeights,
                            const occ::handle<NCollection_HArray2<gp_XYZ>>&   allPoles,
                            const double                  aUmin,
                            const double                  aUmax,
                            const double                  aVmin,
                            const double                  aVmax);

  //! Changes FormNumber (indicates the Shape of the Surface)
  //! Error if not in range [0-9]
  Standard_EXPORT void SetFormNumber(const int form);

  //! returns the upper index of the first sum (U)
  Standard_EXPORT int UpperIndexU() const;

  //! returns the upper index of the second sum (V)
  Standard_EXPORT int UpperIndexV() const;

  //! returns degree of first set of basis functions
  Standard_EXPORT int DegreeU() const;

  //! returns degree of second set of basis functions
  Standard_EXPORT int DegreeV() const;

  //! True if closed in U direction else False
  Standard_EXPORT bool IsClosedU() const;

  //! True if closed in V direction else False
  Standard_EXPORT bool IsClosedV() const;

  //! True if polynomial, False if rational
  //! <flag> False (D) : computed from Weights
  //! <flag> True : recorded
  Standard_EXPORT bool IsPolynomial(const bool flag = false) const;

  //! True if periodic in U direction else False
  Standard_EXPORT bool IsPeriodicU() const;

  //! True if periodic in V direction else False
  Standard_EXPORT bool IsPeriodicV() const;

  //! returns number of knots in U direction
  //! KnotsU are numbered from -DegreeU
  Standard_EXPORT int NbKnotsU() const;

  //! returns number of knots in V direction
  //! KnotsV are numbered from -DegreeV
  Standard_EXPORT int NbKnotsV() const;

  //! returns the value of knot referred to by anIndex in U direction
  //! raises exception if
  //! anIndex < -DegreeU() or anIndex > (NbKnotsU() - DegreeU())
  Standard_EXPORT double KnotU(const int anIndex) const;

  //! returns the value of knot referred to by anIndex in V direction
  //! raises exception if
  //! anIndex < -DegreeV() or anIndex > (NbKnotsV() - DegreeV())
  Standard_EXPORT double KnotV(const int anIndex) const;

  //! returns number of poles in U direction
  Standard_EXPORT int NbPolesU() const;

  //! returns number of poles in V direction
  Standard_EXPORT int NbPolesV() const;

  //! returns the weight referred to by anIndex1, anIndex2
  //! raises exception if anIndex1 <= 0 or anIndex1 > NbPolesU()
  //! or if anIndex2 <= 0 or anIndex2 > NbPolesV()
  Standard_EXPORT double Weight(const int anIndex1,
                                       const int anIndex2) const;

  //! returns the control point referenced by anIndex1, anIndex2
  //! raises exception if anIndex1 <= 0 or anIndex1 > NbPolesU()
  //! or if anIndex2 <= 0 or anIndex2 > NbPolesV()
  Standard_EXPORT gp_Pnt Pole(const int anIndex1,
                              const int anIndex2) const;

  //! returns the control point referenced by anIndex1, anIndex2
  //! after applying the Transf.Matrix
  //! raises exception if anIndex1 <= 0 or anIndex1 > NbPolesU()
  //! or if anIndex2 <= 0 or anIndex2 > NbPolesV()
  Standard_EXPORT gp_Pnt TransformedPole(const int anIndex1,
                                         const int anIndex2) const;

  //! returns starting value in the U direction
  Standard_EXPORT double UMin() const;

  //! returns ending value in the U direction
  Standard_EXPORT double UMax() const;

  //! returns starting value in the V direction
  Standard_EXPORT double VMin() const;

  //! returns ending value in the V direction
  Standard_EXPORT double VMax() const;

  DEFINE_STANDARD_RTTIEXT(IGESGeom_BSplineSurface, IGESData_IGESEntity)

private:
  int              theIndexU;
  int              theIndexV;
  int              theDegreeU;
  int              theDegreeV;
  bool              isClosedU;
  bool              isClosedV;
  bool              isPolynomial;
  bool              isPeriodicU;
  bool              isPeriodicV;
  occ::handle<NCollection_HArray1<double>> theKnotsU;
  occ::handle<NCollection_HArray1<double>> theKnotsV;
  occ::handle<NCollection_HArray2<double>> theWeights;
  occ::handle<NCollection_HArray2<gp_XYZ>>   thePoles;
  double                 theUmin;
  double                 theUmax;
  double                 theVmin;
  double                 theVmax;
};

#endif // _IGESGeom_BSplineSurface_HeaderFile
