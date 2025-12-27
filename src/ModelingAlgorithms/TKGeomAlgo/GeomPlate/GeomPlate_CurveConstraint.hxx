// Created on: 1997-05-05
// Created by: Joelle CHAUVET
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _GeomPlate_CurveConstraint_HeaderFile
#define _GeomPlate_CurveConstraint_HeaderFile

#include <Adaptor3d_CurveOnSurface.hxx>
#include <GeomLProp_SLProps.hxx>

class Geom2d_Curve;
class Law_Function;
class gp_Pnt;
class gp_Vec;

//! Defines curves as constraints to be used to deform a surface.
class GeomPlate_CurveConstraint : public Standard_Transient
{

public:
  //! Initializes an empty curve constraint object.
  Standard_EXPORT GeomPlate_CurveConstraint();

  //! Create a constraint
  //! Order is the order of the constraint. The possible values for order are -1,0,1,2.
  //! Order i means constraints Gi
  //! Npt is the number of points associated with the constraint.
  //! TolDist is the maximum error to satisfy for G0 constraints
  //! TolAng is the maximum error to satisfy for G1 constraints
  //! TolCurv is the maximum error to satisfy for G2 constraints
  //! These errors can be replaced by laws of criterion.
  //! Raises ConstructionError if Order is not -1 , 0, 1, 2
  Standard_EXPORT GeomPlate_CurveConstraint(const occ::handle<Adaptor3d_Curve>& Boundary,
                                            const int         Order,
                                            const int         NPt     = 10,
                                            const double            TolDist = 0.0001,
                                            const double            TolAng  = 0.01,
                                            const double            TolCurv = 0.1);

  //! Allows you to set the order of continuity required for
  //! the constraints: G0, G1, and G2, controlled
  //! respectively by G0Criterion G1Criterion and G2Criterion.
  Standard_EXPORT void SetOrder(const int Order);

  //! Returns the order of constraint, one of G0, G1 or G2.
  Standard_EXPORT int Order() const;

  //! Returns the number of points on the curve used as a
  //! constraint. The default setting is 10. This parameter
  //! affects computation time, which increases by the cube of
  //! the number of points.
  Standard_EXPORT int NbPoints() const;

  //! Allows you to set the number of points on the curve
  //! constraint. The default setting is 10. This parameter
  //! affects computation time, which increases by the cube of
  //! the number of points.
  Standard_EXPORT void SetNbPoints(const int NewNb);

  //! Allows you to set the G0 criterion. This is the law
  //! defining the greatest distance allowed between the
  //! constraint and the target surface for each point of the
  //! constraint. If this criterion is not set, TolDist, the
  //! distance tolerance from the constructor, is used.
  Standard_EXPORT void SetG0Criterion(const occ::handle<Law_Function>& G0Crit);

  //! Allows you to set the G1 criterion. This is the law
  //! defining the greatest angle allowed between the
  //! constraint and the target surface. If this criterion is not
  //! set, TolAng, the angular tolerance from the constructor, is used.
  //! Raises ConstructionError if the curve is not on a surface
  Standard_EXPORT void SetG1Criterion(const occ::handle<Law_Function>& G1Crit);

  Standard_EXPORT void SetG2Criterion(const occ::handle<Law_Function>& G2Crit);

  //! Returns the G0 criterion at the parametric point U on
  //! the curve. This is the greatest distance allowed between
  //! the constraint and the target surface at U.
  Standard_EXPORT double G0Criterion(const double U) const;

  //! Returns the G1 criterion at the parametric point U on
  //! the curve. This is the greatest angle allowed between
  //! the constraint and the target surface at U.
  //! Raises ConstructionError if the curve is not on a surface
  Standard_EXPORT double G1Criterion(const double U) const;

  //! Returns the G2 criterion at the parametric point U on
  //! the curve. This is the greatest difference in curvature
  //! allowed between the constraint and the target surface at U.
  //! Raises ConstructionError if the curve is not on a surface
  Standard_EXPORT double G2Criterion(const double U) const;

  Standard_EXPORT double FirstParameter() const;

  Standard_EXPORT double LastParameter() const;

  Standard_EXPORT double Length() const;

  Standard_EXPORT GeomLProp_SLProps& LPropSurf(const double U);

  Standard_EXPORT void D0(const double U, gp_Pnt& P) const;

  Standard_EXPORT void D1(const double U, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2) const;

  Standard_EXPORT void D2(const double U,
                          gp_Pnt&             P,
                          gp_Vec&             V1,
                          gp_Vec&             V2,
                          gp_Vec&             V3,
                          gp_Vec&             V4,
                          gp_Vec&             V5) const;

  Standard_EXPORT occ::handle<Adaptor3d_Curve> Curve3d() const;

  //! loads a 2d curve associated the surface resulting of the constraints
  Standard_EXPORT void SetCurve2dOnSurf(const occ::handle<Geom2d_Curve>& Curve2d);

  //! Returns a 2d curve associated the surface resulting of the constraints
  Standard_EXPORT occ::handle<Geom2d_Curve> Curve2dOnSurf() const;

  //! loads a 2d curve resulting from the normal projection of
  //! the curve on the initial surface
  Standard_EXPORT void SetProjectedCurve(const occ::handle<Adaptor2d_Curve2d>& Curve2d,
                                         const double              TolU,
                                         const double              TolV);

  //! Returns the projected curve resulting from the normal projection of the
  //! curve on the initial surface
  Standard_EXPORT occ::handle<Adaptor2d_Curve2d> ProjectedCurve() const;

  DEFINE_STANDARD_RTTIEXT(GeomPlate_CurveConstraint, Standard_Transient)

protected:
  occ::handle<Adaptor3d_CurveOnSurface> myFrontiere;
  int                 myNbPoints;
  int                 myOrder;
  occ::handle<Adaptor3d_Curve>          my3dCurve;
  int                 myTang;
  occ::handle<Geom2d_Curve>             my2dCurve;
  occ::handle<Adaptor2d_Curve2d>        myHCurve2d;
  occ::handle<Law_Function>             myG0Crit;
  occ::handle<Law_Function>             myG1Crit;
  occ::handle<Law_Function>             myG2Crit;
  bool                 myConstG0;
  bool                 myConstG1;
  bool                 myConstG2;
  GeomLProp_SLProps                myLProp;
  double                    myTolDist;
  double                    myTolAng;
  double                    myTolCurv;
  double                    myTolU;
  double                    myTolV;
};

#endif // _GeomPlate_CurveConstraint_HeaderFile
