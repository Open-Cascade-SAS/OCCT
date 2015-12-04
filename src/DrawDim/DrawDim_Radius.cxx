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


#include <BRepAdaptor_Surface.hxx>
#include <Draw_Display.hxx>
#include <DrawDim_Radius.hxx>
#include <GC_MakeCircle.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <gp_Circ.hxx>
#include <gp_Pnt.hxx>
#include <Standard_Type.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DrawDim_Radius,DrawDim_Dimension)

//=======================================================================
//function : DrawDim_Radius
//purpose  : 
//=======================================================================
DrawDim_Radius::DrawDim_Radius(const TopoDS_Face& cylinder)
{
  myCylinder = cylinder;
}

//=======================================================================
//function : Cylinder
//purpose  : 
//=======================================================================

const TopoDS_Face& DrawDim_Radius::Cylinder() const
{
  return myCylinder;
}

//=======================================================================
//function : Cylinder
//purpose  : 
//=======================================================================

void DrawDim_Radius::Cylinder(const TopoDS_Face& face) 
{
  myCylinder = face;
}


//=======================================================================
//function : DrawOn
//purpose  : 
//=======================================================================

void DrawDim_Radius::DrawOn(Draw_Display& dis) const
{
  // input  
  TopoDS_Shape myFShape = myCylinder;

  // output
  gp_Pnt myPosition;
  gp_Circ myCircle;

//=======================================================================
//function : ComputeOneFaceRadius
//purpose  : 
//=======================================================================

//void AIS_RadiusDimension::ComputeOneFaceRadius(const Handle(Prs3d_Presentation)& aPresentation)
//{
#ifdef OCCT_DEBUG
  cout << "entree dans computeonefaceradius"<< endl;
#endif
  BRepAdaptor_Surface surfAlgo (TopoDS::Face(myFShape));
  Standard_Real uFirst, uLast, vFirst, vLast;
  uFirst = surfAlgo.FirstUParameter();
  uLast = surfAlgo.LastUParameter();
  vFirst = surfAlgo.FirstVParameter();
  vLast = surfAlgo.LastVParameter();
  Standard_Real uMoy = (uFirst + uLast)/2;
  Standard_Real vMoy = (vFirst + vLast)/2;
  gp_Pnt curpos ;
  surfAlgo.D0(uMoy, vMoy, curpos);
  const Handle(Geom_Surface)& surf = surfAlgo.Surface().Surface();
  Handle(Geom_Curve) aCurve;
  if (surf->DynamicType() == STANDARD_TYPE(Geom_ToroidalSurface)) {
    aCurve = surf->UIso(uMoy);
    uFirst = vFirst;
    uLast = vLast;
  }
  else {
    aCurve = surf->VIso(vMoy);
  }

  if (aCurve->DynamicType() == STANDARD_TYPE(Geom_Circle)) {
    myCircle = Handle(Geom_Circle)::DownCast(aCurve)->Circ();
  } // if (aCurve->DynamicType() ...

  else {
    // compute a circle from 3 points on "aCurve"
    gp_Pnt P1, P2;
    surfAlgo.D0(uFirst, vMoy, P1);
    surfAlgo.D0(uLast, vMoy, P2);
    GC_MakeCircle mkCirc(P1, curpos, P2);
    myCircle = mkCirc.Value()->Circ();
  } // else ...

  myPosition = curpos;

  // DISPLAY
  // Add(myText, curpos, mCircle, uFirst, uLast)    

  dis.Draw(myCircle,uFirst,uLast);  
  dis.DrawMarker(myPosition, Draw_Losange);
}
