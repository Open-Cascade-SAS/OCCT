// Created on: 1993-01-09
// Created by: Kiran
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

#ifndef _IGESGeom_BSplineCurve_HeaderFile
#define _IGESGeom_BSplineCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColgp_HArray1OfXYZ.hxx>
#include <Standard_Real.hxx>
#include <gp_XYZ.hxx>
#include <IGESData_IGESEntity.hxx>
class gp_Pnt;


class IGESGeom_BSplineCurve;
DEFINE_STANDARD_HANDLE(IGESGeom_BSplineCurve, IGESData_IGESEntity)

//! defines IGESBSplineCurve, Type <126> Form <0-5>
//! in package IGESGeom
//! A parametric equation obtained by dividing two summations
//! involving weights (which are real numbers), the control
//! points, and B-Spline basis functions
class IGESGeom_BSplineCurve : public IGESData_IGESEntity
{

public:

  
  Standard_EXPORT IGESGeom_BSplineCurve();
  
  //! This method is used to set the fields of the class
  //! BSplineCurve. Beware about indexation of arrays
  //! - anIndex      : Upper index of the sum
  //! - aDegree      : Degree of basis functions
  //! - aPlanar      : 0 = nonplanar curve, 1 = planar curve
  //! - aClosed      : 0 = open curve, 1 = closed curve
  //! - aPolynom     : 0 = rational, 1 = polynomial
  //! - aPeriodic    : 0 = nonperiodic, 1 = periodic
  //! - allKnots     : Knot sequence values [-Degree,Index+1]
  //! - allWeights   : Array of weights     [0,Index]
  //! - allPoles     : X, Y, Z coordinates of all control points
  //! [0,Index]
  //! - aUmin, aUmax : Starting and ending parameter values
  //! - aNorm        : Unit normal (if the curve is planar)
  //! raises exception if allWeights & allPoles are not of same size.
  Standard_EXPORT void Init (const Standard_Integer anIndex, const Standard_Integer aDegree, const Standard_Boolean aPlanar, const Standard_Boolean aClosed, const Standard_Boolean aPolynom, const Standard_Boolean aPeriodic, const Handle(TColStd_HArray1OfReal)& allKnots, const Handle(TColStd_HArray1OfReal)& allWeights, const Handle(TColgp_HArray1OfXYZ)& allPoles, const Standard_Real aUmin, const Standard_Real aUmax, const gp_XYZ& aNorm);
  
  //! Changes FormNumber (indicates the Shape of the Curve)
  //! Error if not in range [0-5]
  Standard_EXPORT void SetFormNumber (const Standard_Integer form);
  
  //! returns the upper index of the sum (see Knots,Poles)
  Standard_EXPORT Standard_Integer UpperIndex() const;
  
  //! returns the degree of basis functions
  Standard_EXPORT Standard_Integer Degree() const;
  
  //! returns True if the curve is Planar, False if non-planar
  Standard_EXPORT Standard_Boolean IsPlanar() const;
  
  //! returns True if the curve is closed, False if open
  Standard_EXPORT Standard_Boolean IsClosed() const;
  
  //! returns True if the curve is polynomial, False if rational
  //! <flag> False (D) : computed from the list of weights
  //! (all must be equal)
  //! <flag> True : as recorded
  Standard_EXPORT Standard_Boolean IsPolynomial (const Standard_Boolean flag = Standard_False) const;
  
  //! returns True if the curve is periodic, False otherwise
  Standard_EXPORT Standard_Boolean IsPeriodic() const;
  
  //! returns the number of knots (i.e. Degree + UpperIndex + 2)
  Standard_EXPORT Standard_Integer NbKnots() const;
  
  //! returns the knot referred to by anIndex,
  //! inside the range [-Degree,UpperIndex+1]
  //! raises exception if
  //! anIndex < -Degree() or anIndex > (NbKnots() - Degree())
  //! Note : Knots are numbered from -Degree (not from 1)
  Standard_EXPORT Standard_Real Knot (const Standard_Integer anIndex) const;
  
  //! returns number of poles (i.e. UpperIndex + 1)
  Standard_EXPORT Standard_Integer NbPoles() const;
  
  //! returns the weight referred to by anIndex, in [0,UpperIndex]
  //! raises exception if anIndex < 0 or anIndex > UpperIndex()
  Standard_EXPORT Standard_Real Weight (const Standard_Integer anIndex) const;
  
  //! returns the pole referred to by anIndex, in [0,UpperIndex]
  //! raises exception if anIndex < 0 or anIndex > UpperIndex()
  Standard_EXPORT gp_Pnt Pole (const Standard_Integer anIndex) const;
  
  //! returns the anIndex'th pole after applying Transf. Matrix
  //! raises exception if an Index < 0 or an Index > UpperIndex()
  Standard_EXPORT gp_Pnt TransformedPole (const Standard_Integer anIndex) const;
  
  //! returns starting parameter value
  Standard_EXPORT Standard_Real UMin() const;
  
  //! returns ending parameter value
  Standard_EXPORT Standard_Real UMax() const;
  
  //! if the curve is nonplanar then (0, 0, 0) is returned
  Standard_EXPORT gp_XYZ Normal() const;




  DEFINE_STANDARD_RTTIEXT(IGESGeom_BSplineCurve,IGESData_IGESEntity)

protected:




private:


  Standard_Integer theIndex;
  Standard_Integer theDegree;
  Standard_Boolean isPlanar;
  Standard_Boolean isClosed;
  Standard_Boolean isPolynomial;
  Standard_Boolean isPeriodic;
  Handle(TColStd_HArray1OfReal) theKnots;
  Handle(TColStd_HArray1OfReal) theWeights;
  Handle(TColgp_HArray1OfXYZ) thePoles;
  Standard_Real theUmin;
  Standard_Real theUmax;
  gp_XYZ theNorm;


};







#endif // _IGESGeom_BSplineCurve_HeaderFile
