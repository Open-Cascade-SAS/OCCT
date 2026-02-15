// Created on: 1993-04-14
// Created by: Isabelle GRIGNON
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

#ifndef _BRepGProp_Face_HeaderFile
#define _BRepGProp_Face_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepAdaptor_Surface.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Standard_Integer.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Array1.hxx>
#include <GeomAbs_IsoType.hxx>
#include <NCollection_HArray1.hxx>
#include <TopLoc_Location.hxx>

class TopoDS_Face;
class gp_Pnt;
class gp_Vec;
class TopoDS_Edge;
class gp_Pnt2d;
class gp_Vec2d;
class Geom_Surface;

class BRepGProp_Face
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor. Initializes the object with a flag IsUseSpan
  //! that says if it is necessary to define spans on a face.
  //! This option has an effect only for BSpline faces. Spans
  //! are returned by the methods GetUKnots and GetTKnots.
  BRepGProp_Face(const bool IsUseSpan = false);

  //! Constructor. Initializes the object with the face and the
  //! flag IsUseSpan that says if it is necessary to define
  //! spans on a face. This option has an effect only for
  //! BSpline faces. Spans are returned by the methods GetUKnots
  //! and GetTKnots.
  BRepGProp_Face(const TopoDS_Face& F, const bool IsUseSpan = false);

  Standard_EXPORT void Load(const TopoDS_Face& F);

  Standard_EXPORT int VIntegrationOrder() const;

  //! Returns true if the face is not trimmed.
  bool NaturalRestriction() const;

  //! Returns the TopoDS face.
  const TopoDS_Face& GetFace() const;

  //! Returns the value of the boundary curve of the face.
  gp_Pnt2d Value2d(const double U) const;

  Standard_EXPORT int SIntOrder(const double Eps) const;

  Standard_EXPORT int SVIntSubs() const;

  Standard_EXPORT int SUIntSubs() const;

  Standard_EXPORT void UKnots(NCollection_Array1<double>& Knots) const;

  Standard_EXPORT void VKnots(NCollection_Array1<double>& Knots) const;

  Standard_EXPORT int LIntOrder(const double Eps) const;

  Standard_EXPORT int LIntSubs() const;

  Standard_EXPORT void LKnots(NCollection_Array1<double>& Knots) const;

  //! Returns the number of points required to do the
  //! integration in the U parametric direction with
  //! a good accuracy.
  Standard_EXPORT int UIntegrationOrder() const;

  //! Returns the parametric bounds of the Face.
  Standard_EXPORT void Bounds(double& U1, double& U2, double& V1, double& V2) const;

  //! Computes the point of parameter U, V on the Face <S> and
  //! the normal to the face at this point.
  Standard_EXPORT void Normal(const double U, const double V, gp_Pnt& P, gp_Vec& VNor) const;

  //! Loading the boundary arc.
  //! Returns FALSE if edge has no P-Curve.
  Standard_EXPORT bool Load(const TopoDS_Edge& E);

  //! Returns the parametric value of the start point of
  //! the current arc of curve.
  double FirstParameter() const;

  //! Returns the parametric value of the end point of
  //! the current arc of curve.
  double LastParameter() const;

  //! Returns the number of points required to do the
  //! integration along the parameter of curve.
  Standard_EXPORT int IntegrationOrder() const;

  //! Returns the point of parameter U and the first derivative
  //! at this point of a boundary curve.
  void D12d(const double U, gp_Pnt2d& P, gp_Vec2d& V1) const;

  //! Loading the boundary arc. This arc is either a top, bottom,
  //! left or right bound of a UV rectangle in which the
  //! parameters of surface are defined.
  //! If IsFirstParam is equal to true, the face is
  //! initialized by either left of bottom bound. Otherwise it is
  //! initialized by the top or right one.
  //! If theIsoType is equal to GeomAbs_IsoU, the face is
  //! initialized with either left or right bound. Otherwise -
  //! with either top or bottom one.
  Standard_EXPORT void Load(const bool IsFirstParam, const GeomAbs_IsoType theIsoType);

  //! Returns an array of U knots of the face. The first and last
  //! elements of the array will be theUMin and theUMax. The
  //! middle elements will be the U Knots of the face greater
  //! then theUMin and lower then theUMax in increasing order.
  //! If the face is not a BSpline, the array initialized with
  //! theUMin and theUMax only.
  Standard_EXPORT void GetUKnots(const double                              theUMin,
                                 const double                              theUMax,
                                 occ::handle<NCollection_HArray1<double>>& theUKnots) const;

  //! Returns an array of combination of T knots of the arc and
  //! V knots of the face. The first and last elements of the
  //! array will be theTMin and theTMax. The middle elements will
  //! be the Knots of the arc and the values of parameters of
  //! arc on which the value points have V coordinates close to V
  //! knots of face. All the parameter will be greater then
  //! theTMin and lower then theTMax in increasing order.
  //! If the face is not a BSpline, the array initialized with
  //! theTMin and theTMax only.
  Standard_EXPORT void GetTKnots(const double                              theTMin,
                                 const double                              theTMax,
                                 occ::handle<NCollection_HArray1<double>>& theTKnots) const;

private:
  BRepAdaptor_Surface       mySurface;
  Geom2dAdaptor_Curve       myCurve;
  occ::handle<Geom_Surface> myFaceSurface;
  TopLoc_Location           myFaceLocation;
  bool                      myIsFaceContextReady;
  bool                      mySReverse;
  bool                      myIsUseSpan;
};

#include <BRepGProp_Face.lxx>

#endif // _BRepGProp_Face_HeaderFile
