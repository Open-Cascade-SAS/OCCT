// Created on: 1994-02-24
// Created by: Laurent BOURESCHE
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

#ifndef _BRepLProp_SLProps_HeaderFile
#define _BRepLProp_SLProps_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <BRepAdaptor_Surface.hxx>
#include <Standard_Integer.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Dir.hxx>
#include <LProp_Status.hxx>
class LProp_BadContinuity;
class Standard_DomainError;
class Standard_OutOfRange;
class LProp_NotDefined;
class BRepAdaptor_Surface;
class BRepLProp_SurfaceTool;
class gp_Pnt;
class gp_Vec;
class gp_Dir;

class BRepLProp_SLProps
{
public:
  DEFINE_STANDARD_ALLOC

  //! Initializes the local properties of the surface <S>
  //! for the parameter values (<U>, <V>).
  //! The current point and the derivatives are
  //! computed at the same time, which allows an
  //! optimization of the computation time.
  //! <N> indicates the maximum number of derivations to
  //! be done (0, 1, or 2). For example, to compute
  //! only the tangent, N should be equal to 1.
  //! <Resolution> is the linear tolerance (it is used to test
  //! if a vector is null).
  Standard_EXPORT BRepLProp_SLProps(const BRepAdaptor_Surface& S,
                                    const double        U,
                                    const double        V,
                                    const int     N,
                                    const double        Resolution);

  //! idem as previous constructor but without setting the value
  //! of parameters <U> and <V>.
  Standard_EXPORT BRepLProp_SLProps(const BRepAdaptor_Surface& S,
                                    const int     N,
                                    const double        Resolution);

  //! idem as previous constructor but without setting the value
  //! of parameters <U> and <V> and the surface.
  //! the surface can have an empty constructor.
  Standard_EXPORT BRepLProp_SLProps(const int N, const double Resolution);

  //! Initializes the local properties of the surface S
  //! for the new surface.
  Standard_EXPORT void SetSurface(const BRepAdaptor_Surface& S);

  //! Initializes the local properties of the surface S
  //! for the new parameter values (<U>, <V>).
  Standard_EXPORT void SetParameters(const double U, const double V);

  //! Returns the point.
  Standard_EXPORT const gp_Pnt& Value() const;

  //! Returns the first U derivative.
  //! The derivative is computed if it has not been yet.
  Standard_EXPORT const gp_Vec& D1U();

  //! Returns the first V derivative.
  //! The derivative is computed if it has not been yet.
  Standard_EXPORT const gp_Vec& D1V();

  //! Returns the second U derivatives
  //! The derivative is computed if it has not been yet.
  Standard_EXPORT const gp_Vec& D2U();

  //! Returns the second V derivative.
  //! The derivative is computed if it has not been yet.
  Standard_EXPORT const gp_Vec& D2V();

  //! Returns the second UV cross-derivative.
  //! The derivative is computed if it has not been yet.
  Standard_EXPORT const gp_Vec& DUV();

  //! returns True if the U tangent is defined.
  //! For example, the tangent is not defined if the
  //! two first U derivatives are null.
  Standard_EXPORT bool IsTangentUDefined();

  //! Returns the tangent direction <D> on the iso-V.
  Standard_EXPORT void TangentU(gp_Dir& D);

  //! returns if the V tangent is defined.
  //! For example, the tangent is not defined if the
  //! two first V derivatives are null.
  Standard_EXPORT bool IsTangentVDefined();

  //! Returns the tangent direction <D> on the iso-V.
  Standard_EXPORT void TangentV(gp_Dir& D);

  //! Tells if the normal is defined.
  Standard_EXPORT bool IsNormalDefined();

  //! Returns the normal direction.
  Standard_EXPORT const gp_Dir& Normal();

  //! returns True if the curvature is defined.
  Standard_EXPORT bool IsCurvatureDefined();

  //! returns True if the point is umbilic (i.e. if the
  //! curvature is constant).
  Standard_EXPORT bool IsUmbilic();

  //! Returns the maximum curvature
  Standard_EXPORT double MaxCurvature();

  //! Returns the minimum curvature
  Standard_EXPORT double MinCurvature();

  //! Returns the direction of the maximum and minimum curvature
  //! <MaxD> and <MinD>
  Standard_EXPORT void CurvatureDirections(gp_Dir& MaxD, gp_Dir& MinD);

  //! Returns the mean curvature.
  Standard_EXPORT double MeanCurvature();

  //! Returns the Gaussian curvature
  Standard_EXPORT double GaussianCurvature();

private:
  BRepAdaptor_Surface mySurf;
  double       myU;
  double       myV;
  int    myDerOrder;
  int    myCN;
  double       myLinTol;
  gp_Pnt              myPnt;
  gp_Vec              myD1u;
  gp_Vec              myD1v;
  gp_Vec              myD2u;
  gp_Vec              myD2v;
  gp_Vec              myDuv;
  gp_Dir              myNormal;
  double       myMinCurv;
  double       myMaxCurv;
  gp_Dir              myDirMinCurv;
  gp_Dir              myDirMaxCurv;
  double       myMeanCurv;
  double       myGausCurv;
  int    mySignificantFirstDerivativeOrderU;
  int    mySignificantFirstDerivativeOrderV;
  LProp_Status        myUTangentStatus;
  LProp_Status        myVTangentStatus;
  LProp_Status        myNormalStatus;
  LProp_Status        myCurvatureStatus;
};

#endif // _BRepLProp_SLProps_HeaderFile
