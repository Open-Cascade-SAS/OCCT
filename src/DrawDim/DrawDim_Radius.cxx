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



#include <DrawDim_Radius.ixx>
#include <BRepAdaptor_Surface.hxx>
#include <Geom_Surface.hxx>
#include <gp_Pnt.hxx>
#include <gp_Circ.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <GC_MakeCircle.hxx>
#include <TopoDS.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Circle.hxx>

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

  cout << "entree dans computeonefaceradius"<< endl;
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
