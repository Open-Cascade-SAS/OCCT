// Created on: 1993-04-07
// Created by: Laurent BUCHARD
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

#ifndef _IntCurveSurface_ThePolygonOfHInter_HeaderFile
#define _IntCurveSurface_ThePolygonOfHInter_HeaderFile

#include <Adaptor3d_Curve.hxx>
#include <Bnd_Box.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <NCollection_Array1.hxx>
class Standard_OutOfRange;
class IntCurveSurface_TheHCurveTool;
class Bnd_Box;
class gp_Pnt;

class IntCurveSurface_ThePolygonOfHInter
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT IntCurveSurface_ThePolygonOfHInter(const occ::handle<Adaptor3d_Curve>& Curve,
                                                     const int         NbPnt);

  Standard_EXPORT IntCurveSurface_ThePolygonOfHInter(const occ::handle<Adaptor3d_Curve>& Curve,
                                                     const double            U1,
                                                     const double            U2,
                                                     const int         NbPnt);

  Standard_EXPORT IntCurveSurface_ThePolygonOfHInter(const occ::handle<Adaptor3d_Curve>& Curve,
                                                     const NCollection_Array1<double>&    Upars);

  //! Give the bounding box of the polygon.
  const Bnd_Box& Bounding() const { return TheBnd; }

  double DeflectionOverEstimation() const { return TheDeflection; }

  void SetDeflectionOverEstimation(const double x)
  {
    TheDeflection = x;
    TheBnd.Enlarge(TheDeflection);
  }

  void Closed(const bool flag) { ClosedPolygon = flag; }

  // clang-format off
  bool Closed() const { return false; } // -- Voir si le cas Closed est traitable

  // clang-format on

  //! Give the number of Segments in the polyline.
  int NbSegments() const { return NbPntIn - 1; }

  //! Give the point of range Index in the Polygon.
  const gp_Pnt& BeginOfSeg(const int theIndex) const { return ThePnts(theIndex); }

  //! Give the point of range Index in the Polygon.
  const gp_Pnt& EndOfSeg(const int theIndex) const { return ThePnts(theIndex + 1); }

  //! Returns the parameter (On the curve)
  //! of the first point of the Polygon
  double InfParameter() const { return Binf; }

  //! Returns the parameter (On the curve)
  //! of the last point of the Polygon
  double SupParameter() const { return Bsup; }

  //! Give an approximation of the parameter on the curve
  //! according to the discretization of the Curve.
  Standard_EXPORT double ApproxParamOnCurve(const int Index,
                                                   const double    ParamOnLine) const;

  Standard_EXPORT void Dump() const;

protected:
  Standard_EXPORT void Init(const occ::handle<Adaptor3d_Curve>& Curve);

  Standard_EXPORT void Init(const occ::handle<Adaptor3d_Curve>& Curve,
                            const NCollection_Array1<double>&    Upars);

private:
  Bnd_Box                       TheBnd;
  double                 TheDeflection;
  int              NbPntIn;
  NCollection_Array1<gp_Pnt>            ThePnts;
  bool              ClosedPolygon;
  double                 Binf;
  double                 Bsup;
  occ::handle<NCollection_HArray1<double>> myParams;
};

#endif // _IntCurveSurface_ThePolygonOfHInter_HeaderFile
