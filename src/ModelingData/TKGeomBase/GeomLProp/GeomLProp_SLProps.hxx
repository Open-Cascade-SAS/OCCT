// Created on: 1992-03-26
// Created by: Herve LEGRAND
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

#ifndef _GeomLProp_SLProps_HeaderFile
#define _GeomLProp_SLProps_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <Standard_OutOfRange.hxx>

#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Dir.hxx>
#include <LProp_Status.hxx>

#include <GeomLProp_SurfaceUtils.hxx>

//! Template class for computing local properties of a 3D surface:
//! point, first and second derivatives, tangent directions, normal,
//! and curvature analysis (max, min, mean, Gaussian).
//! @tparam SurfaceType the surface storage type (e.g. occ::handle<Geom_Surface>,
//!         BRepAdaptor_Surface, occ::handle<Adaptor3d_Surface>, HLRBRep_SurfacePtr)
//! @tparam Access the access policy for evaluating surface derivatives
template <typename SurfaceType, typename Access = LProp_SurfaceUtils::DirectAccess>
class GeomLProp_SLPropsBase
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
  GeomLProp_SLPropsBase(const SurfaceType& S,
                        const double       U,
                        const double       V,
                        const int          N,
                        const double       Resolution)
      : mySurf(S),
        myU(RealLast()),
        myV(RealLast()),
        myDerOrder(N),
        myCN(4),
        myLinTol(Resolution),
        myUTangentStatus(LProp_Undecided),
        myVTangentStatus(LProp_Undecided),
        myNormalStatus(LProp_Undecided),
        myCurvatureStatus(LProp_Undecided)
  {
    Standard_OutOfRange_Raise_if(N < 0 || N > 2, "GeomLProp_SLPropsBase::GeomLProp_SLPropsBase()");
    SetParameters(U, V);
  }

  //! idem as previous constructor but without setting the value
  //! of parameters <U> and <V>.
  GeomLProp_SLPropsBase(const SurfaceType& S, const int N, const double Resolution)
      : mySurf(S),
        myU(RealLast()),
        myV(RealLast()),
        myDerOrder(N),
        myCN(4),
        myLinTol(Resolution),
        myUTangentStatus(LProp_Undecided),
        myVTangentStatus(LProp_Undecided),
        myNormalStatus(LProp_Undecided),
        myCurvatureStatus(LProp_Undecided)
  {
    Standard_OutOfRange_Raise_if(N < 0 || N > 2, "GeomLProp_SLPropsBase::GeomLProp_SLPropsBase()");
  }

  //! idem as previous constructor but without setting the value
  //! of parameters <U> and <V> and the surface.
  //! the surface can have an empty constructor.
  GeomLProp_SLPropsBase(const int N, const double Resolution)
      : mySurf{},
        myU(RealLast()),
        myV(RealLast()),
        myDerOrder(N),
        myCN(0),
        myLinTol(Resolution),
        myUTangentStatus(LProp_Undecided),
        myVTangentStatus(LProp_Undecided),
        myNormalStatus(LProp_Undecided),
        myCurvatureStatus(LProp_Undecided)
  {
    Standard_OutOfRange_Raise_if(N < 0 || N > 2,
                                 "GeomLProp_SLPropsBase::GeomLProp_SLPropsBase() bad level");
  }

  //! Initializes the local properties of the surface S
  //! for the new surface.
  void SetSurface(const SurfaceType& S)
  {
    mySurf = S;
    myCN   = 4;
  }

  //! Initializes the local properties of the surface S
  //! for the new parameter values (<U>, <V>).
  void SetParameters(const double U, const double V)
  {
    LProp_SurfaceUtils::SetParameters<Access>(mySurf,
                                              U,
                                              V,
                                              myU,
                                              myV,
                                              myDerOrder,
                                              myPnt,
                                              myD1u,
                                              myD1v,
                                              myD2u,
                                              myD2v,
                                              myDuv,
                                              myUTangentStatus,
                                              myVTangentStatus,
                                              myNormalStatus,
                                              myCurvatureStatus);
  }

  //! Returns the point.
  const gp_Pnt& Value() const { return myPnt; }

  //! Returns the first U derivative.
  //! The derivative is computed if it has not been yet.
  const gp_Vec& D1U()
  {
    return LProp_SurfaceUtils::EnsureSurfDeriv<
      Access>(mySurf, myU, myV, myDerOrder, 1, myPnt, myD1u, myD1v, myD2u, myD2v, myDuv, myD1u);
  }

  //! Returns the first V derivative.
  //! The derivative is computed if it has not been yet.
  const gp_Vec& D1V()
  {
    return LProp_SurfaceUtils::EnsureSurfDeriv<
      Access>(mySurf, myU, myV, myDerOrder, 1, myPnt, myD1u, myD1v, myD2u, myD2v, myDuv, myD1v);
  }

  //! Returns the second U derivatives
  //! The derivative is computed if it has not been yet.
  const gp_Vec& D2U()
  {
    return LProp_SurfaceUtils::EnsureSurfDeriv<
      Access>(mySurf, myU, myV, myDerOrder, 2, myPnt, myD1u, myD1v, myD2u, myD2v, myDuv, myD2u);
  }

  //! Returns the second V derivative.
  //! The derivative is computed if it has not been yet.
  const gp_Vec& D2V()
  {
    return LProp_SurfaceUtils::EnsureSurfDeriv<
      Access>(mySurf, myU, myV, myDerOrder, 2, myPnt, myD1u, myD1v, myD2u, myD2v, myDuv, myD2v);
  }

  //! Returns the second UV cross-derivative.
  //! The derivative is computed if it has not been yet.
  const gp_Vec& DUV()
  {
    return LProp_SurfaceUtils::EnsureSurfDeriv<
      Access>(mySurf, myU, myV, myDerOrder, 2, myPnt, myD1u, myD1v, myD2u, myD2v, myDuv, myDuv);
  }

  //! returns True if the U tangent is defined.
  //! For example, the tangent is not defined if the
  //! two first U derivatives are null.
  bool IsTangentUDefined()
  {
    return LProp_SurfaceUtils::IsTangentUDefined(*this,
                                                 myCN,
                                                 myLinTol,
                                                 mySignificantFirstDerivativeOrderU,
                                                 myUTangentStatus);
  }

  //! Returns the tangent direction <D> on the iso-V.
  void TangentU(gp_Dir& D)
  {
    LProp_SurfaceUtils::TangentU<Access>(*this,
                                         mySurf,
                                         myU,
                                         myV,
                                         myD1u,
                                         myD2u,
                                         mySignificantFirstDerivativeOrderU,
                                         D);
  }

  //! returns if the V tangent is defined.
  //! For example, the tangent is not defined if the
  //! two first V derivatives are null.
  bool IsTangentVDefined()
  {
    return LProp_SurfaceUtils::IsTangentVDefined(*this,
                                                 myCN,
                                                 myLinTol,
                                                 mySignificantFirstDerivativeOrderV,
                                                 myVTangentStatus);
  }

  //! Returns the tangent direction <D> on the iso-V.
  void TangentV(gp_Dir& D)
  {
    LProp_SurfaceUtils::TangentV<Access>(*this,
                                         mySurf,
                                         myU,
                                         myV,
                                         myD1v,
                                         myD2v,
                                         mySignificantFirstDerivativeOrderV,
                                         D);
  }

  //! Tells if the normal is defined.
  bool IsNormalDefined()
  {
    return LProp_SurfaceUtils::IsNormalDefined(myD1u, myD1v, myLinTol, myNormal, myNormalStatus);
  }

  //! Returns the normal direction.
  const gp_Dir& Normal() { return LProp_SurfaceUtils::Normal(*this, myNormal); }

  //! returns True if the curvature is defined.
  bool IsCurvatureDefined()
  {
    return LProp_SurfaceUtils::IsCurvatureDefined(*this,
                                                  myCN,
                                                  myDerOrder,
                                                  myD1u,
                                                  myD1v,
                                                  myD2u,
                                                  myD2v,
                                                  myDuv,
                                                  myNormal,
                                                  myMinCurv,
                                                  myMaxCurv,
                                                  myDirMinCurv,
                                                  myDirMaxCurv,
                                                  myMeanCurv,
                                                  myGausCurv,
                                                  myCurvatureStatus);
  }

  //! returns True if the point is umbilic (i.e. if the
  //! curvature is constant).
  bool IsUmbilic() { return LProp_SurfaceUtils::IsUmbilic(*this, myMaxCurv, myMinCurv); }

  //! Returns the maximum curvature
  double MaxCurvature() { return LProp_SurfaceUtils::RequireCurvature(*this, myMaxCurv); }

  //! Returns the minimum curvature
  double MinCurvature() { return LProp_SurfaceUtils::RequireCurvature(*this, myMinCurv); }

  //! Returns the direction of the maximum and minimum curvature
  //! <MaxD> and <MinD>
  void CurvatureDirections(gp_Dir& MaxD, gp_Dir& MinD)
  {
    LProp_SurfaceUtils::CurvatureDirections(*this, myDirMaxCurv, myDirMinCurv, MaxD, MinD);
  }

  //! Returns the mean curvature.
  double MeanCurvature() { return LProp_SurfaceUtils::RequireCurvature(*this, myMeanCurv); }

  //! Returns the Gaussian curvature
  double GaussianCurvature() { return LProp_SurfaceUtils::RequireCurvature(*this, myGausCurv); }

private:
  SurfaceType  mySurf;
  double       myU;
  double       myV;
  int          myDerOrder;
  int          myCN;
  double       myLinTol;
  gp_Pnt       myPnt;
  gp_Vec       myD1u;
  gp_Vec       myD1v;
  gp_Vec       myD2u;
  gp_Vec       myD2v;
  gp_Vec       myDuv;
  gp_Dir       myNormal;
  double       myMinCurv = 0.0;
  double       myMaxCurv = 0.0;
  gp_Dir       myDirMinCurv;
  gp_Dir       myDirMaxCurv;
  double       myMeanCurv                         = 0.0;
  double       myGausCurv                         = 0.0;
  int          mySignificantFirstDerivativeOrderU = 0;
  int          mySignificantFirstDerivativeOrderV = 0;
  LProp_Status myUTangentStatus;
  LProp_Status myVTangentStatus;
  LProp_Status myNormalStatus;
  LProp_Status myCurvatureStatus;
};

//! Default surface local properties class using occ::handle<Geom_Surface>.
using GeomLProp_SLProps = GeomLProp_SLPropsBase<occ::handle<Geom_Surface>>;

#endif // _GeomLProp_SLProps_HeaderFile
