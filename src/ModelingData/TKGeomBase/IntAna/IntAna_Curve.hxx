// Created on: 1992-06-30
// Created by: Laurent BUCHARD
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _IntAna_Curve_HeaderFile
#define _IntAna_Curve_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <GeomAbs_SurfaceType.hxx>
#include <gp_Ax3.hxx>
#include <TColStd_ListOfReal.hxx>

class gp_Cone;
class gp_Cylinder;

//! Definition of a parametric Curve which is the result
//! of the intersection between two quadrics.
class IntAna_Curve
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty Constructor
  Standard_EXPORT IntAna_Curve();

  //! Sets the parameters used to compute Points and Derivative
  //! on the curve.
  Standard_EXPORT void SetCylinderQuadValues(const gp_Cylinder&     Cylinder,
                                             const Standard_Real    Qxx,
                                             const Standard_Real    Qyy,
                                             const Standard_Real    Qzz,
                                             const Standard_Real    Qxy,
                                             const Standard_Real    Qxz,
                                             const Standard_Real    Qyz,
                                             const Standard_Real    Qx,
                                             const Standard_Real    Qy,
                                             const Standard_Real    Qz,
                                             const Standard_Real    Q1,
                                             const Standard_Real    Tol,
                                             const Standard_Real    DomInf,
                                             const Standard_Real    DomSup,
                                             const Standard_Boolean TwoZForATheta,
                                             const Standard_Boolean ZIsPositive);

  //! Sets  the parameters used    to compute Points  and
  //! Derivative on the curve.
  Standard_EXPORT void SetConeQuadValues(const gp_Cone&         Cone,
                                         const Standard_Real    Qxx,
                                         const Standard_Real    Qyy,
                                         const Standard_Real    Qzz,
                                         const Standard_Real    Qxy,
                                         const Standard_Real    Qxz,
                                         const Standard_Real    Qyz,
                                         const Standard_Real    Qx,
                                         const Standard_Real    Qy,
                                         const Standard_Real    Qz,
                                         const Standard_Real    Q1,
                                         const Standard_Real    Tol,
                                         const Standard_Real    DomInf,
                                         const Standard_Real    DomSup,
                                         const Standard_Boolean TwoZForATheta,
                                         const Standard_Boolean ZIsPositive);

  //! Returns TRUE if the curve is not  infinite  at the
  //! last parameter or at the first parameter of the domain.
  Standard_EXPORT Standard_Boolean IsOpen() const;

  //! Returns the parametric domain of the curve.
  Standard_EXPORT void Domain(Standard_Real& theFirst, Standard_Real& theLast) const;

  //! Returns TRUE if the function is constant.
  Standard_EXPORT Standard_Boolean IsConstant() const;

  //! Returns TRUE if the domain is open at the beginning.
  Standard_EXPORT Standard_Boolean IsFirstOpen() const;

  //! Returns TRUE if the domain is open at the end.
  Standard_EXPORT Standard_Boolean IsLastOpen() const;

  //! Returns the point at parameter Theta on the curve.
  Standard_EXPORT gp_Pnt Value(const Standard_Real Theta);

  //! Returns the point and the first derivative at parameter
  //! Theta on the curve.
  Standard_EXPORT Standard_Boolean D1u(const Standard_Real Theta, gp_Pnt& P, gp_Vec& V);

  //! Tries to find the parameter of the point P on the curve.
  //! If the method returns False, the "projection" is
  //! impossible.
  //! If the method returns True at least one parameter has been found.
  //! theParams is always sorted in ascending order.
  Standard_EXPORT void FindParameter(const gp_Pnt& P, TColStd_ListOfReal& theParams) const;

  //! If flag is True, the Curve is not defined at the
  //! first parameter of its domain.
  Standard_EXPORT void SetIsFirstOpen(const Standard_Boolean Flag);

  //! If flag is True, the Curve is not defined at the
  //! first parameter of its domain.
  Standard_EXPORT void SetIsLastOpen(const Standard_Boolean Flag);

  //! Trims this curve
  Standard_EXPORT void SetDomain(const Standard_Real theFirst, const Standard_Real theLast);

protected:
  //! Protected function.
  Standard_EXPORT gp_Pnt InternalValue(const Standard_Real Theta1,
                                       const Standard_Real Theta2) const;

  //! Protected function.
  Standard_EXPORT void InternalUVValue(const Standard_Real Param,
                                       Standard_Real&      U,
                                       Standard_Real&      V,
                                       Standard_Real&      A,
                                       Standard_Real&      B,
                                       Standard_Real&      C,
                                       Standard_Real&      Co,
                                       Standard_Real&      Si,
                                       Standard_Real&      Di) const;

private:
  Standard_Real    Z0Cte;
  Standard_Real    Z0Sin;
  Standard_Real    Z0Cos;
  Standard_Real    Z0SinSin;
  Standard_Real    Z0CosCos;
  Standard_Real    Z0CosSin;
  Standard_Real    Z1Cte;
  Standard_Real    Z1Sin;
  Standard_Real    Z1Cos;
  Standard_Real    Z1SinSin;
  Standard_Real    Z1CosCos;
  Standard_Real    Z1CosSin;
  Standard_Real    Z2Cte;
  Standard_Real    Z2Sin;
  Standard_Real    Z2Cos;
  Standard_Real    Z2SinSin;
  Standard_Real    Z2CosCos;
  Standard_Real    Z2CosSin;
  Standard_Boolean TwoCurves;
  Standard_Boolean TakeZPositive;
  Standard_Real    Tolerance;

  //! Internal fields defining the default domain
  Standard_Real       DomainInf, DomainSup;
  Standard_Boolean    RestrictedInf;
  Standard_Boolean    RestrictedSup;
  Standard_Boolean    firstbounded;
  Standard_Boolean    lastbounded;
  GeomAbs_SurfaceType typequadric;
  Standard_Real       RCyl;
  Standard_Real       Angle;
  gp_Ax3              Ax3;

  //! Trim boundaries
  Standard_Real myFirstParameter, myLastParameter;
};

#endif // _IntAna_Curve_HeaderFile
