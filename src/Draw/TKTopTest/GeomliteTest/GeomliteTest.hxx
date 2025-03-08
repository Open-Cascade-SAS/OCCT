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

#ifndef _GeomliteTest_HeaderFile
#define _GeomliteTest_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Draw_Interpretor.hxx>

//! this  package  provides  elementary commands for  curves  and
//! surface.
class GeomliteTest
{
public:
  DEFINE_STANDARD_ALLOC

  //! defines all geometric commands.
  Standard_EXPORT static void AllCommands(Draw_Interpretor& I);

  //! defines curve commands.
  Standard_EXPORT static void CurveCommands(Draw_Interpretor& I);

  //! defines surface commands.
  Standard_EXPORT static void SurfaceCommands(Draw_Interpretor& I);

  //! defines commands to test the Geom2dAPI
  //! - Intersection
  //! - Extrema
  //! - Projection
  //! - Approximation, interpolation
  Standard_EXPORT static void API2dCommands(Draw_Interpretor& I);

  //! defines constrained curves commands.
  Standard_EXPORT static void ApproxCommands(Draw_Interpretor& I);

  //! defines curves and surfaces modification commands.
  //! - Curve extension to point
  //! - Surface extension by length
  Standard_EXPORT static void ModificationCommands(Draw_Interpretor& I);

protected:
private:
};

#endif // _GeomliteTest_HeaderFile
