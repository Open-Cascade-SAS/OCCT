// Created on: 1995-06-06
// Created by: Jean Yves LEBEY
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

#ifndef _BRepApprox_TheMultiLineToolOfApprox_HeaderFile
#define _BRepApprox_TheMultiLineToolOfApprox_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Boolean.hxx>
class BRepApprox_TheMultiLineOfApprox;
class ApproxInt_SvSurfaces;

class BRepApprox_TheMultiLineToolOfApprox
{
public:
  DEFINE_STANDARD_ALLOC

  //! Returns the number of multipoints of the TheMultiLine.
  static int FirstPoint(const BRepApprox_TheMultiLineOfApprox& ML);

  //! Returns the number of multipoints of the TheMultiLine.
  static int LastPoint(const BRepApprox_TheMultiLineOfApprox& ML);

  //! Returns the number of 2d points of a TheMultiLine.
  static int NbP2d(const BRepApprox_TheMultiLineOfApprox& ML);

  //! Returns the number of 3d points of a TheMultiLine.
  static int NbP3d(const BRepApprox_TheMultiLineOfApprox& ML);

  //! returns the 3d points of the multipoint <MPointIndex>
  //! when only 3d points exist.
  static void Value(const BRepApprox_TheMultiLineOfApprox& ML,
                    const int                              MPointIndex,
                    NCollection_Array1<gp_Pnt>&            tabPt);

  //! returns the 2d points of the multipoint <MPointIndex>
  //! when only 2d points exist.
  static void Value(const BRepApprox_TheMultiLineOfApprox& ML,
                    const int                              MPointIndex,
                    NCollection_Array1<gp_Pnt2d>&          tabPt2d);

  //! returns the 3d and 2d points of the multipoint
  //! <MPointIndex>.
  static void Value(const BRepApprox_TheMultiLineOfApprox& ML,
                    const int                              MPointIndex,
                    NCollection_Array1<gp_Pnt>&            tabPt,
                    NCollection_Array1<gp_Pnt2d>&          tabPt2d);

  //! returns the 3d points of the multipoint <MPointIndex>
  //! when only 3d points exist.
  static bool Tangency(const BRepApprox_TheMultiLineOfApprox& ML,
                       const int                              MPointIndex,
                       NCollection_Array1<gp_Vec>&            tabV);

  //! returns the 2d tangency points of the multipoint
  //! <MPointIndex> only when 2d points exist.
  static bool Tangency(const BRepApprox_TheMultiLineOfApprox& ML,
                       const int                              MPointIndex,
                       NCollection_Array1<gp_Vec2d>&          tabV2d);

  //! returns the 3d and 2d points of the multipoint
  //! <MPointIndex>.
  static bool Tangency(const BRepApprox_TheMultiLineOfApprox& ML,
                       const int                              MPointIndex,
                       NCollection_Array1<gp_Vec>&            tabV,
                       NCollection_Array1<gp_Vec2d>&          tabV2d);

  //! returns the 3d curvature of the multipoint <MPointIndex>
  //! when only 3d points exist.
  static bool Curvature(const BRepApprox_TheMultiLineOfApprox& ML,
                        const int                              MPointIndex,
                        NCollection_Array1<gp_Vec>&            tabV);

  //! returns the 2d curvature points of the multipoint
  //! <MPointIndex> only when 2d points exist.
  static bool Curvature(const BRepApprox_TheMultiLineOfApprox& ML,
                        const int                              MPointIndex,
                        NCollection_Array1<gp_Vec2d>&          tabV2d);

  //! returns the 3d and 2d curvature of the multipoint
  //! <MPointIndex>.
  static bool Curvature(const BRepApprox_TheMultiLineOfApprox& ML,
                        const int                              MPointIndex,
                        NCollection_Array1<gp_Vec>&            tabV,
                        NCollection_Array1<gp_Vec2d>&          tabV2d);

  //! Is called if WhatStatus returned "PointsAdded".
  static BRepApprox_TheMultiLineOfApprox MakeMLBetween(const BRepApprox_TheMultiLineOfApprox& ML,
                                                       const int                              I1,
                                                       const int                              I2,
                                                       const int NbPMin);

  //! Is called when the Bezier curve contains a loop
  static bool MakeMLOneMorePoint(const BRepApprox_TheMultiLineOfApprox& ML,
                                 const int                              I1,
                                 const int                              I2,
                                 const int                              indbad,
                                 BRepApprox_TheMultiLineOfApprox&       OtherLine);

  static Approx_Status WhatStatus(const BRepApprox_TheMultiLineOfApprox& ML,
                                  const int                              I1,
                                  const int                              I2);

  //! Dump of the current multi-line.
  static void Dump(const BRepApprox_TheMultiLineOfApprox& ML);
};

#define TheMultiLine BRepApprox_TheMultiLineOfApprox
#define TheMultiLine_hxx <BRepApprox_TheMultiLineOfApprox.hxx>
#define TheMultiMPoint ApproxInt_SvSurfaces
#define TheMultiMPoint_hxx <ApproxInt_SvSurfaces.hxx>
#define ApproxInt_MultiLineTool BRepApprox_TheMultiLineToolOfApprox
#define ApproxInt_MultiLineTool_hxx <BRepApprox_TheMultiLineToolOfApprox.hxx>

#include <ApproxInt_MultiLineTool.lxx>

#undef TheMultiLine
#undef TheMultiLine_hxx
#undef TheMultiMPoint
#undef TheMultiMPoint_hxx
#undef ApproxInt_MultiLineTool
#undef ApproxInt_MultiLineTool_hxx

#endif // _BRepApprox_TheMultiLineToolOfApprox_HeaderFile
