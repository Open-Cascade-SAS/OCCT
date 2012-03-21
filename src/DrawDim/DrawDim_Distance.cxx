// Created on: 1997-04-21
// Created by: Denis PASCAL
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <DrawDim_Distance.ixx>
#include <DrawDim.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <gp_Pln.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <gp_Vec.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Vertex.hxx>
#include <BRep_Tool.hxx>
#include <Precision.hxx>
#include <TCollection_AsciiString.hxx>


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
