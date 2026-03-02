// Created on: 1992-09-04
// Created by: Remi GILET
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

#include <gce_MakeMirror.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>

//=================================================================================================

gce_MakeMirror::gce_MakeMirror(const gp_Pnt& Point)
{
  TheMirror.SetMirror(Point);
}

//=================================================================================================

gce_MakeMirror::gce_MakeMirror(const gp_Ax1& Axis)
{
  TheMirror.SetMirror(Axis);
}

//=================================================================================================

gce_MakeMirror::gce_MakeMirror(const gp_Lin& Line)
{
  TheMirror.SetMirror(gp_Ax1(Line.Location(), Line.Direction()));
}

//=================================================================================================

gce_MakeMirror::gce_MakeMirror(const gp_Pnt& Point, const gp_Dir& Direc)
{
  TheMirror.SetMirror(gp_Ax1(Point, Direc));
}

//=================================================================================================

gce_MakeMirror::gce_MakeMirror(const gp_Ax2& Plane)
{
  TheMirror.SetMirror(Plane);
}

//=================================================================================================

gce_MakeMirror::gce_MakeMirror(const gp_Pln& Plane)
{
  TheMirror.SetMirror(Plane.Position().Ax2());
}

//=================================================================================================

const gp_Trsf& gce_MakeMirror::Value() const
{
  return TheMirror;
}

