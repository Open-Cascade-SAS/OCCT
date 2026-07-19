// Created on: 1993-01-20
// Created by: Laurent PAINNOT
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

#ifndef _AppDef_MyLineTool_HeaderFile
#define _AppDef_MyLineTool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <Standard_Integer.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <Approx_Status.hxx>
class AppDef_MultiLine;

//! Example of MultiLine tool corresponding to the tools of the packages AppParCurves and Approx.
//! For Approx, the tool will not add points if the algorithms want some.
class AppDef_MyLineTool
{
public:
  DEFINE_STANDARD_ALLOC

  //! Returns the first index of multipoints of the MultiLine.
  Standard_EXPORT static int FirstPoint(const AppDef_MultiLine& ML);

  //! Returns the last index of multipoints of the MultiLine.
  Standard_EXPORT static int LastPoint(const AppDef_MultiLine& ML);

  //! Returns the number of 2d points of a MultiLine.
  Standard_EXPORT static int NbP2d(const AppDef_MultiLine& ML);

  //! Returns the number of 3d points of a MultiLine.
  Standard_EXPORT static int NbP3d(const AppDef_MultiLine& ML);

  //! returns the 3d points of the multipoint <MPointIndex>
  //! when only 3d points exist.
  Standard_EXPORT static void Value(const AppDef_MultiLine&     ML,
                                    const int                   MPointIndex,
                                    NCollection_Array1<gp_Pnt>& tabPt);

  //! returns the 2d points of the multipoint <MPointIndex>
  //! when only 2d points exist.
  Standard_EXPORT static void Value(const AppDef_MultiLine&       ML,
                                    const int                     MPointIndex,
                                    NCollection_Array1<gp_Pnt2d>& tabPt2d);

  //! returns the 3d and 2d points of the multipoint
  //! <MPointIndex>.
  Standard_EXPORT static void Value(const AppDef_MultiLine&       ML,
                                    const int                     MPointIndex,
                                    NCollection_Array1<gp_Pnt>&   tabPt,
                                    NCollection_Array1<gp_Pnt2d>& tabPt2d);

  //! returns the 3d points of the multipoint <MPointIndex>
  //! when only 3d points exist.
  Standard_EXPORT static bool Tangency(const AppDef_MultiLine&     ML,
                                       const int                   MPointIndex,
                                       NCollection_Array1<gp_Vec>& tabV);

  //! returns the 2d tangency points of the multipoint
  //! <MPointIndex> only when 2d points exist.
  Standard_EXPORT static bool Tangency(const AppDef_MultiLine&       ML,
                                       const int                     MPointIndex,
                                       NCollection_Array1<gp_Vec2d>& tabV2d);

  //! returns the 3d and 2d points of the multipoint
  //! <MPointIndex>.
  Standard_EXPORT static bool Tangency(const AppDef_MultiLine&       ML,
                                       const int                     MPointIndex,
                                       NCollection_Array1<gp_Vec>&   tabV,
                                       NCollection_Array1<gp_Vec2d>& tabV2d);

  //! returns the 3d curvatures of the multipoint <MPointIndex>
  //! when only 3d points exist.
  Standard_EXPORT static bool Curvature(const AppDef_MultiLine&     ML,
                                        const int                   MPointIndex,
                                        NCollection_Array1<gp_Vec>& tabV);

  //! returns the 2d curvatures of the multipoint
  //! <MPointIndex> only when 2d points exist.
  Standard_EXPORT static bool Curvature(const AppDef_MultiLine&       ML,
                                        const int                     MPointIndex,
                                        NCollection_Array1<gp_Vec2d>& tabV2d);

  //! returns the 3d and 2d curvatures of the multipoint
  //! <MPointIndex>.
  Standard_EXPORT static bool Curvature(const AppDef_MultiLine&       ML,
                                        const int                     MPointIndex,
                                        NCollection_Array1<gp_Vec>&   tabV,
                                        NCollection_Array1<gp_Vec2d>& tabV2d);

  //! returns NoPointsAdded
  Standard_EXPORT static Approx_Status WhatStatus(const AppDef_MultiLine& ML,
                                                  const int               I1,
                                                  const int               I2);

  //! Is never called in the algorithms.
  //! Nothing is done.
  Standard_EXPORT static AppDef_MultiLine MakeMLBetween(const AppDef_MultiLine& ML,
                                                        const int               I1,
                                                        const int               I2,
                                                        const int               NbPMin);

  //! Is never called in the algorithms.
  //! Nothing is done.
  Standard_EXPORT static bool MakeMLOneMorePoint(const AppDef_MultiLine& ML,
                                                 const int               I1,
                                                 const int               I2,
                                                 const int               indbad,
                                                 AppDef_MultiLine&       OtherLine);
};

#endif // _AppDef_MyLineTool_HeaderFile
