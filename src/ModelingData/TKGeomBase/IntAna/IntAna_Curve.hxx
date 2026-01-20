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
#include <NCollection_List.hxx>

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
                                             const double    Qxx,
                                             const double    Qyy,
                                             const double    Qzz,
                                             const double    Qxy,
                                             const double    Qxz,
                                             const double    Qyz,
                                             const double    Qx,
                                             const double    Qy,
                                             const double    Qz,
                                             const double    Q1,
                                             const double    Tol,
                                             const double    DomInf,
                                             const double    DomSup,
                                             const bool TwoZForATheta,
                                             const bool ZIsPositive);

  //! Sets the parameters used to compute Points and
  //! Derivative on the curve.
  Standard_EXPORT void SetConeQuadValues(const gp_Cone&         Cone,
                                         const double    Qxx,
                                         const double    Qyy,
                                         const double    Qzz,
                                         const double    Qxy,
                                         const double    Qxz,
                                         const double    Qyz,
                                         const double    Qx,
                                         const double    Qy,
                                         const double    Qz,
                                         const double    Q1,
                                         const double    Tol,
                                         const double    DomInf,
                                         const double    DomSup,
                                         const bool TwoZForATheta,
                                         const bool ZIsPositive);

  //! Returns TRUE if the curve is not infinite at the
  //! last parameter or at the first parameter of the domain.
  Standard_EXPORT bool IsOpen() const;

  //! Returns the parametric domain of the curve.
  Standard_EXPORT void Domain(double& theFirst, double& theLast) const;

  //! Returns TRUE if the function is constant.
  Standard_EXPORT bool IsConstant() const;

  //! Returns TRUE if the domain is open at the beginning.
  Standard_EXPORT bool IsFirstOpen() const;

  //! Returns TRUE if the domain is open at the end.
  Standard_EXPORT bool IsLastOpen() const;

  //! Returns the point at parameter Theta on the curve.
  Standard_EXPORT gp_Pnt Value(const double Theta);

  //! Returns the point and the first derivative at parameter
  //! Theta on the curve.
  Standard_EXPORT bool D1u(const double Theta, gp_Pnt& P, gp_Vec& V);

  //! Tries to find the parameter of the point P on the curve.
  //! If the method returns False, the "projection" is
  //! impossible.
  //! If the method returns True at least one parameter has been found.
  //! theParams is always sorted in ascending order.
  Standard_EXPORT void FindParameter(const gp_Pnt& P, NCollection_List<double>& theParams) const;

  //! If flag is True, the Curve is not defined at the
  //! first parameter of its domain.
  Standard_EXPORT void SetIsFirstOpen(const bool Flag);

  //! If flag is True, the Curve is not defined at the
  //! first parameter of its domain.
  Standard_EXPORT void SetIsLastOpen(const bool Flag);

  //! Trims this curve
  Standard_EXPORT void SetDomain(const double theFirst, const double theLast);

protected:
  //! Protected function.
  Standard_EXPORT gp_Pnt InternalValue(const double Theta1,
                                       const double Theta2) const;

  //! Protected function.
  Standard_EXPORT void InternalUVValue(const double Param,
                                       double&      U,
                                       double&      V,
                                       double&      A,
                                       double&      B,
                                       double&      C,
                                       double&      Co,
                                       double&      Si,
                                       double&      Di) const;

private:
  double    Z0Cte;
  double    Z0Sin;
  double    Z0Cos;
  double    Z0SinSin;
  double    Z0CosCos;
  double    Z0CosSin;
  double    Z1Cte;
  double    Z1Sin;
  double    Z1Cos;
  double    Z1SinSin;
  double    Z1CosCos;
  double    Z1CosSin;
  double    Z2Cte;
  double    Z2Sin;
  double    Z2Cos;
  double    Z2SinSin;
  double    Z2CosCos;
  double    Z2CosSin;
  bool TwoCurves;
  bool TakeZPositive;
  double    Tolerance;

  //! Internal fields defining the default domain
  double       DomainInf, DomainSup;
  bool    RestrictedInf;
  bool    RestrictedSup;
  bool    firstbounded;
  bool    lastbounded;
  GeomAbs_SurfaceType typequadric;
  double       RCyl;
  double       Angle;
  gp_Ax3              Ax3;

  //! Trim boundaries
  double myFirstParameter, myLastParameter;
};

#endif // _IntAna_Curve_HeaderFile
