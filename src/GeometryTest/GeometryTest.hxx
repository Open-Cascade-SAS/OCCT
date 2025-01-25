// Created on: 1991-06-24
// Created by: Christophe MARION
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _GeometryTest_HeaderFile
#define _GeometryTest_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Draw_Interpretor.hxx>

//! this  package  provides  commands for  curves  and
//! surface.
class GeometryTest
{
public:
  DEFINE_STANDARD_ALLOC

  //! defines all geometric commands.
  Standard_EXPORT static void AllCommands(Draw_Interpretor& I);

  //! defines curve commands.
  Standard_EXPORT static void CurveCommands(Draw_Interpretor& I);

  //! defines tangent curve commands.
  Standard_EXPORT static void CurveTanCommands(Draw_Interpretor& I);

  //! defines fair curve commands.
  Standard_EXPORT static void FairCurveCommands(Draw_Interpretor& I);

  //! defines surface commands.
  Standard_EXPORT static void SurfaceCommands(Draw_Interpretor& I);

  //! defines constrained curves commands.
  Standard_EXPORT static void ConstraintCommands(Draw_Interpretor& I);

  //! defines commands to test the GeomAPI
  //! - Intersection
  //! - Extrema
  //! - Projection
  //! - Approximation, interpolation
  Standard_EXPORT static void APICommands(Draw_Interpretor& I);

  //! defines commands to check local
  //! continuity between curves or surfaces
  Standard_EXPORT static void ContinuityCommands(Draw_Interpretor& I);

  //! defines     command  to    test  the    polyhedral
  //! triangulations and the polygons from the Poly package.
  Standard_EXPORT static void PolyCommands(Draw_Interpretor& I);

  //! defines commands to test projection of geometric objects
  Standard_EXPORT static void TestProjCommands(Draw_Interpretor& I);
};

#endif // _GeometryTest_HeaderFile
