// Created on: 1997-04-21
// Created by: Denis PASCAL
// Copyright (c) 1997-1999 Matra Datavision
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


#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <Draw_Display.hxx>
#include <DrawDim.hxx>
#include <DrawDim_Distance.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DrawDim_Distance,DrawDim_Dimension)

//=======================================================================
//function : DrawDim_Distance
//purpose  : 
//=======================================================================
DrawDim_Distance::DrawDim_Distance (const TopoDS_Face& plane1,
				    const TopoDS_Face& plane2)
{
  myPlane1 = plane1;
  myPlane2 = plane2;
}


//=======================================================================
//function : DrawDim_Distance
//purpose  : 
//=======================================================================

DrawDim_Distance::DrawDim_Distance (const TopoDS_Face& plane1)

{
  myPlane1 = plane1;
}

//=======================================================================
//function : Plane1
//purpose  : 
//=======================================================================

const TopoDS_Face& DrawDim_Distance::Plane1() const
{
  return myPlane1;
}

//=======================================================================
//function : Plane1
//purpose  : 
//=======================================================================

 void DrawDim_Distance::Plane1(const TopoDS_Face& face) 
{
  myPlane1 = face;
}

//=======================================================================
//function : Plane2
//purpose  : 
//=======================================================================

const TopoDS_Face& DrawDim_Distance::Plane2() const
{
  return myPlane2;
}

//=======================================================================
//function : Plane2
//purpose  : 
//=======================================================================

void DrawDim_Distance::Plane2(const TopoDS_Face& face) 
{ 
  myPlane2 = face;
}


//=======================================================================
//function : DrawOn
//purpose  : 
//=======================================================================

void DrawDim_Distance::DrawOn(Draw_Display& dis) const
{

  // compute the points and the direction
  BRepAdaptor_Surface surf1(myPlane1);

  // today we process only planar faces
  if (surf1.GetType() != GeomAbs_Plane)
    return;
  
  const gp_Ax1& anAx1 = surf1.Plane().Axis();
  gp_Vec V = anAx1.Direction();

  // output
  gp_Pnt FAttach;   // first attach point
  gp_Pnt SAttach;   // second attach point

  // first point, try a vertex
  TopExp_Explorer explo(myPlane1,TopAbs_VERTEX);
  if (explo.More()) {
    FAttach = BRep_Tool::Pnt(TopoDS::Vertex(explo.Current()));
  }
  else {
    // no vertex, use the origin
    FAttach = anAx1.Location();
  }
  

  if (!myPlane2.IsNull()) {
    // translate the point until the second face
    BRepAdaptor_Surface surf2(myPlane2);
    surf2.D0(0,0,SAttach);
    Standard_Real r = V.Dot(gp_Vec(FAttach,SAttach));
    V *= r;
  }
    
  SAttach = FAttach;
  SAttach.Translate(V);

  // DISPLAY
  dis.Draw (FAttach,SAttach);
  V *= 0.5;
  FAttach.Translate(V);
  dis.DrawMarker(FAttach, Draw_Losange);
  DrawText(FAttach,dis);
}
