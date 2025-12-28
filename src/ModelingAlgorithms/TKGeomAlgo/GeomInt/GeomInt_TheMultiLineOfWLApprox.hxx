// Created on: 1995-01-27
// Created by: Jacques GOUSSARD
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _GeomInt_TheMultiLineOfWLApprox_HeaderFile
#define _GeomInt_TheMultiLineOfWLApprox_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <Approx_Status.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
class IntPatch_WLine;
class ApproxInt_SvSurfaces;

class GeomInt_TheMultiLineOfWLApprox
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT GeomInt_TheMultiLineOfWLApprox();

  //! The class SvSurfaces is used when the approximation algorithm
  //! needs some extra points on the line <line>.
  //! A New line is then created which shares the same surfaces and functions.
  //! SvSurfaces is a deferred class which allows several implementations of
  //! this algorithm with different surfaces (bi-parametric ones, or
  //! implicit and biparametric ones)
  Standard_EXPORT GeomInt_TheMultiLineOfWLApprox(const occ::handle<IntPatch_WLine>& line,
                                                 void* const                        PtrSvSurfaces,
                                                 const int                          NbP3d,
                                                 const int                          NbP2d,
                                                 const bool                         ApproxU1V1,
                                                 const bool                         ApproxU2V2,
                                                 const double                       xo,
                                                 const double                       yo,
                                                 const double                       zo,
                                                 const double                       u1o,
                                                 const double                       v1o,
                                                 const double                       u2o,
                                                 const double                       v2o,
                                                 const bool                         P2DOnFirst,
                                                 const int                          IndMin = 0,
                                                 const int                          IndMax = 0);

  //! No Extra points will be added on the current line
  Standard_EXPORT GeomInt_TheMultiLineOfWLApprox(const occ::handle<IntPatch_WLine>& line,
                                                 const int                          NbP3d,
                                                 const int                          NbP2d,
                                                 const bool                         ApproxU1V1,
                                                 const bool                         ApproxU2V2,
                                                 const double                       xo,
                                                 const double                       yo,
                                                 const double                       zo,
                                                 const double                       u1o,
                                                 const double                       v1o,
                                                 const double                       u2o,
                                                 const double                       v2o,
                                                 const bool                         P2DOnFirst,
                                                 const int                          IndMin = 0,
                                                 const int                          IndMax = 0);

  Standard_EXPORT int FirstPoint() const;

  Standard_EXPORT int LastPoint() const;

  //! Returns the number of 2d points of a TheLine.
  Standard_EXPORT int NbP2d() const;

  //! Returns the number of 3d points of a TheLine.
  Standard_EXPORT int NbP3d() const;

  Standard_EXPORT Approx_Status WhatStatus() const;

  //! Returns the 3d points of the multipoint <MPointIndex> when only 3d points exist.
  Standard_EXPORT void Value(const int MPointIndex, NCollection_Array1<gp_Pnt>& tabPt) const;

  //! Returns the 2d points of the multipoint <MPointIndex> when only 2d points exist.
  Standard_EXPORT void Value(const int MPointIndex, NCollection_Array1<gp_Pnt2d>& tabPt2d) const;

  //! returns the 3d and 2d points of the multipoint <MPointIndex>.
  Standard_EXPORT void Value(const int                     MPointIndex,
                             NCollection_Array1<gp_Pnt>&   tabPt,
                             NCollection_Array1<gp_Pnt2d>& tabPt2d) const;

  //! Returns the 3d tangency points of the multipoint <MPointIndex> only
  //! when 3d points exist.
  Standard_EXPORT bool Tangency(const int MPointIndex, NCollection_Array1<gp_Vec>& tabV) const;

  //! Returns the 2d tangency points of the multipoint <MPointIndex> only
  //! when 2d points exist.
  Standard_EXPORT bool Tangency(const int MPointIndex, NCollection_Array1<gp_Vec2d>& tabV2d) const;

  //! Returns the 3d and 2d points of the multipoint <MPointIndex>.
  Standard_EXPORT bool Tangency(const int                     MPointIndex,
                                NCollection_Array1<gp_Vec>&   tabV,
                                NCollection_Array1<gp_Vec2d>& tabV2d) const;

  //! Tries to make a sub-line between <Low> and <High> points of this line
  //! by adding <NbPointsToInsert> new points
  Standard_EXPORT GeomInt_TheMultiLineOfWLApprox MakeMLBetween(const int Low,
                                                               const int High,
                                                               const int NbPointsToInsert) const;

  //! Tries to make a sub-line between <Low> and <High> points of this line
  //! by adding one more point between (indbad-1)-th and indbad-th points
  Standard_EXPORT bool MakeMLOneMorePoint(const int                       Low,
                                          const int                       High,
                                          const int                       indbad,
                                          GeomInt_TheMultiLineOfWLApprox& OtherLine) const;

  //! Dump of the current multi-line.
  Standard_EXPORT void Dump() const;

private:
  void*                       PtrOnmySvSurfaces;
  occ::handle<IntPatch_WLine> myLine;
  int                         indicemin;
  int                         indicemax;
  int                         nbp3d;
  int                         nbp2d;
  bool                        myApproxU1V1;
  bool                        myApproxU2V2;
  bool                        p2donfirst;
  double                      Xo;
  double                      Yo;
  double                      Zo;
  double                      U1o;
  double                      V1o;
  double                      U2o;
  double                      V2o;
};

#endif // _GeomInt_TheMultiLineOfWLApprox_HeaderFile
