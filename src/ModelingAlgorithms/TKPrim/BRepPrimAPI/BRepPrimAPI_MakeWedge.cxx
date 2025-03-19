// Created on: 1993-07-23
// Created by: Remi LEQUETTE
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

#include <BRepPrimAPI_MakeWedge.hxx>
#include <gp_Ax2.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>

//=================================================================================================

BRepPrimAPI_MakeWedge::BRepPrimAPI_MakeWedge(const Standard_Real dx,
                                             const Standard_Real dy,
                                             const Standard_Real dz,
                                             const Standard_Real ltx)
    : myWedge(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1), gp_Dir(1, 0, 0)), dx, dy, dz, ltx)
{
}

//=================================================================================================

BRepPrimAPI_MakeWedge::BRepPrimAPI_MakeWedge(const gp_Ax2&       Axes,
                                             const Standard_Real dx,
                                             const Standard_Real dy,
                                             const Standard_Real dz,
                                             const Standard_Real ltx)
    : myWedge(Axes, dx, dy, dz, ltx)
{
}

//=================================================================================================

BRepPrimAPI_MakeWedge::BRepPrimAPI_MakeWedge(const Standard_Real dx,
                                             const Standard_Real dy,
                                             const Standard_Real dz,
                                             const Standard_Real xmin,
                                             const Standard_Real zmin,
                                             const Standard_Real xmax,
                                             const Standard_Real zmax)
    : myWedge(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1), gp_Dir(1, 0, 0)),
              0,
              0,
              0,
              zmin,
              xmin,
              dx,
              dy,
              dz,
              zmax,
              xmax)
{
}

//=================================================================================================

BRepPrimAPI_MakeWedge::BRepPrimAPI_MakeWedge(const gp_Ax2&       Axes,
                                             const Standard_Real dx,
                                             const Standard_Real dy,
                                             const Standard_Real dz,
                                             const Standard_Real xmin,
                                             const Standard_Real zmin,
                                             const Standard_Real xmax,
                                             const Standard_Real zmax)
    : myWedge(Axes, 0, 0, 0, zmin, xmin, dx, dy, dz, zmax, xmax)
{
}

//=================================================================================================

BRepPrim_Wedge& BRepPrimAPI_MakeWedge::Wedge()
{
  return myWedge;
}

//=================================================================================================

const TopoDS_Shell& BRepPrimAPI_MakeWedge::Shell()
{
  Build();
  return myWedge.Shell();
}

//=================================================================================================

void BRepPrimAPI_MakeWedge::Build(const Message_ProgressRange& /*theRange*/)
{
  BRep_Builder B;
  B.MakeSolid(TopoDS::Solid(myShape));
  B.Add(myShape, myWedge.Shell());
  Done();
}

//=================================================================================================

const TopoDS_Solid& BRepPrimAPI_MakeWedge::Solid()
{
  Build();
  return TopoDS::Solid(myShape);
}

//=================================================================================================

BRepPrimAPI_MakeWedge::operator TopoDS_Shell()
{
  return Shell();
}

//=================================================================================================

BRepPrimAPI_MakeWedge::operator TopoDS_Solid()
{
  return Solid();
}
