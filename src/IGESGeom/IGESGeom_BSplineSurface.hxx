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
#include <Standard_Boolean.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray2OfReal.hxx>
#include <TColgp_HArray2OfXYZ.hxx>
#include <Standard_Real.hxx>
#include <IGESData_IGESEntity.hxx>
class gp_Pnt;


class IGESGeom_BSplineSurface;
DEFINE_STANDARD_HANDLE(IGESGeom_BSplineSurface, IGESData_IGESEntity)

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
  Standard_EXPORT void Init (const Standard_Integer anIndexU, const Standard_Integer anIndexV, const Standard_Integer aDegU, const Standard_Integer aDegV, const Standard_Boolean aCloseU, const Standard_Boolean aCloseV, const Standard_Boolean aPolynom, const Standard_Boolean aPeriodU, const Standard_Boolean aPeriodV, const Handle(TColStd_HArray1OfReal)& allKnotsU, const Handle(TColStd_HArray1OfReal)& allKnotsV, const Handle(TColStd_HArray2OfReal)& allWeights, const Handle(TColgp_HArray2OfXYZ)& allPoles, const Standard_Real aUmin, const Standard_Real aUmax, const Standard_Real aVmin, const Standard_Real aVmax);
  
  //! Changes FormNumber (indicates the Shape of the Surface)
  //! Error if not in range [0-9]
  Standard_EXPORT void SetFormNumber (const Standard_Integer form);
  
  //! returns the upper index of the first sum (U)
  Standard_EXPORT Standard_Integer UpperIndexU() const;
  
  //! returns the upper index of the second sum (V)
  Standard_EXPORT Standard_Integer UpperIndexV() const;
  
  //! returns degree of first set of basis functions
  Standard_EXPORT Standard_Integer DegreeU() const;
  
  //! returns degree of second set of basis functions
  Standard_EXPORT Standard_Integer DegreeV() const;
  
  //! True if closed in U direction else False
  Standard_EXPORT Standard_Boolean IsClosedU() const;
  
  //! True if closed in V direction else False
  Standard_EXPORT Standard_Boolean IsClosedV() const;
  
  //! True if polynomial, False if rational
  //! <flag> False (D) : computed from Weights
  //! <flag> True : recorded
  Standard_EXPORT Standard_Boolean IsPolynomial (const Standard_Boolean flag = Standard_False) const;
  
  //! True if periodic in U direction else False
  Standard_EXPORT Standard_Boolean IsPeriodicU() const;
  
  //! True if periodic in V direction else False
  Standard_EXPORT Standard_Boolean IsPeriodicV() const;
  
  //! returns number of knots in U direction
  //! KnotsU are numbered from -DegreeU
  Standard_EXPORT Standard_Integer NbKnotsU() const;
  
  //! returns number of knots in V direction
  //! KnotsV are numbered from -DegreeV
  Standard_EXPORT Standard_Integer NbKnotsV() const;
  
  //! returns the value of knot referred to by anIndex in U direction
  //! raises exception if
  //! anIndex < -DegreeU() or anIndex > (NbKnotsU() - DegreeU())
  Standard_EXPORT Standard_Real KnotU (const Standard_Integer anIndex) const;
  
  //! returns the value of knot referred to by anIndex in V direction
  //! raises exception if
  //! anIndex < -DegreeV() or anIndex > (NbKnotsV() - DegreeV())
  Standard_EXPORT Standard_Real KnotV (const Standard_Integer anIndex) const;
  
  //! returns number of poles in U direction
  Standard_EXPORT Standard_Integer NbPolesU() const;
  
  //! returns number of poles in V direction
  Standard_EXPORT Standard_Integer NbPolesV() const;
  
  //! returns the weight referred to by anIndex1, anIndex2
  //! raises exception if anIndex1 <= 0 or anIndex1 > NbPolesU()
  //! or if anIndex2 <= 0 or anIndex2 > NbPolesV()
  Standard_EXPORT Standard_Real Weight (const Standard_Integer anIndex1, const Standard_Integer anIndex2) const;
  
  //! returns the control point referenced by anIndex1, anIndex2
  //! raises exception if anIndex1 <= 0 or anIndex1 > NbPolesU()
  //! or if anIndex2 <= 0 or anIndex2 > NbPolesV()
  Standard_EXPORT gp_Pnt Pole (const Standard_Integer anIndex1, const Standard_Integer anIndex2) const;
  
  //! returns the control point referenced by anIndex1, anIndex2
  //! after applying the Transf.Matrix
  //! raises exception if anIndex1 <= 0 or anIndex1 > NbPolesU()
  //! or if anIndex2 <= 0 or anIndex2 > NbPolesV()
  Standard_EXPORT gp_Pnt TransformedPole (const Standard_Integer anIndex1, const Standard_Integer anIndex2) const;
  
  //! returns starting value in the U direction
  Standard_EXPORT Standard_Real UMin() const;
  
  //! returns ending value in the U direction
  Standard_EXPORT Standard_Real UMax() const;
  
  //! returns starting value in the V direction
  Standard_EXPORT Standard_Real VMin() const;
  
  //! returns ending value in the V direction
  Standard_EXPORT Standard_Real VMax() const;




  DEFINE_STANDARD_RTTIEXT(IGESGeom_BSplineSurface,IGESData_IGESEntity)

protected:




private:


  Standard_Integer theIndexU;
  Standard_Integer theIndexV;
  Standard_Integer theDegreeU;
  Standard_Integer theDegreeV;
  Standard_Boolean isClosedU;
  Standard_Boolean isClosedV;
  Standard_Boolean isPolynomial;
  Standard_Boolean isPeriodicU;
  Standard_Boolean isPeriodicV;
  Handle(TColStd_HArray1OfReal) theKnotsU;
  Handle(TColStd_HArray1OfReal) theKnotsV;
  Handle(TColStd_HArray2OfReal) theWeights;
  Handle(TColgp_HArray2OfXYZ) thePoles;
  Standard_Real theUmin;
  Standard_Real theUmax;
  Standard_Real theVmin;
  Standard_Real theVmax;


};







#endif // _IGESGeom_BSplineSurface_HeaderFile
