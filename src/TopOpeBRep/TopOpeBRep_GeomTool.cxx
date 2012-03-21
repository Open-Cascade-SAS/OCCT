// Created on: 1993-06-24
// Created by: Jean Yves LEBEY
// Copyright (c) 1993-1999 Matra Datavision
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


#include <TopOpeBRep_GeomTool.ixx>

#include <BRep_Tool.hxx>
#include <TopoDS.hxx>
#include <TopOpeBRep_TypeLineCurve.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <Standard_ProgramError.hxx>
#include <TopOpeBRep_WPointInter.hxx>
#include <TopOpeBRep_WPointInterIterator.hxx>
#include <TopOpeBRepTool_CurveTool.hxx>
#include <TopOpeBRepTool_ShapeTool.hxx>

//=======================================================================
//function : TopOpeBRep_GeomTool
//purpose  : 
//=======================================================================
TopOpeBRep_GeomTool::TopOpeBRep_GeomTool()
{
}

//=======================================================================
//function : MakeCurves
//purpose  : 
//=======================================================================
void  TopOpeBRep_GeomTool::MakeCurves
(const Standard_Real min,const Standard_Real max,
 const TopOpeBRep_LineInter& L,
 const TopoDS_Shape& S1,const TopoDS_Shape& S2,
 TopOpeBRepDS_Curve& C,
 Handle(Geom2d_Curve)& PC1,Handle(Geom2d_Curve)& PC2)
{
  Standard_Boolean IsWalk = Standard_False;
  Handle(Geom_Curve) C3D;
  TopOpeBRep_TypeLineCurve typeline = L.TypeLineCurve();

  switch (typeline) {
    
  case TopOpeBRep_WALKING : {
    // make BSplines of degree 1
    C3D = MakeBSpline1fromWALKING3d(L);
    PC1 = MakeBSpline1fromWALKING2d(L,1);
    PC2 = MakeBSpline1fromWALKING2d(L,2);
    if (!PC1.IsNull())
      C.Curve1(PC1);
    if (!PC2.IsNull())
      C.Curve2(PC2);
    IsWalk = Standard_True;
    break;
  }
  case TopOpeBRep_LINE : 
  case TopOpeBRep_CIRCLE :
  case TopOpeBRep_ELLIPSE : {
    C3D = L.Curve();
    break;
  }
  case TopOpeBRep_PARABOLA :
  case TopOpeBRep_HYPERBOLA : {
    C3D = L.Curve(min,max); // Trimmed
    break;
  }
  case TopOpeBRep_ANALYTIC : 
  case TopOpeBRep_RESTRICTION : 
  case TopOpeBRep_OTHERTYPE :
    default : 
      Standard_ProgramError::Raise("TopOpeBRep_GeomTool::MakePrivateCurves");
    break;
  }

  Standard_Real tol = C.Tolerance();
  C.DefineCurve(C3D,tol,IsWalk);
  C.SetRange(min,max);
}


//=======================================================================
//function : MakeCurve
//purpose  : 
//=======================================================================
void  TopOpeBRep_GeomTool::MakeCurve
(const Standard_Real min,const Standard_Real max,
 const TopOpeBRep_LineInter& L,
 Handle(Geom_Curve)& C3D)
{
  TopOpeBRep_TypeLineCurve typeline = L.TypeLineCurve();
  
  switch (typeline) {
    
  case TopOpeBRep_WALKING : 
    C3D = MakeBSpline1fromWALKING3d(L);
    break;
    
  case TopOpeBRep_LINE : 
    C3D = L.Curve();
    break;

  case TopOpeBRep_CIRCLE :
  case TopOpeBRep_ELLIPSE :
  case TopOpeBRep_PARABOLA :
  case TopOpeBRep_HYPERBOLA :

    if      (typeline == TopOpeBRep_CIRCLE) 
      C3D = L.Curve();
    else if (typeline == TopOpeBRep_ELLIPSE) 
      C3D = L.Curve();
    else if (typeline == TopOpeBRep_PARABOLA) 
      C3D = L.Curve(min,max); // Trimmed
    else if (typeline == TopOpeBRep_HYPERBOLA) 
      C3D = L.Curve(min,max); //Trimmed

    break;
    
  case TopOpeBRep_ANALYTIC : 
  case TopOpeBRep_RESTRICTION : 
  case TopOpeBRep_OTHERTYPE :
  default : 
    Standard_ProgramError::Raise("TopOpeBRep_GeomTool::MakePrivateCurves");
    break;
  }
}

//=======================================================================
//function : MakeBSpline1fromWALKING3d
//purpose  : 
//=======================================================================
Handle(Geom_Curve) TopOpeBRep_GeomTool::MakeBSpline1fromWALKING3d
(const TopOpeBRep_LineInter& L)
{
  Standard_Integer ip;
  TopOpeBRep_WPointInterIterator itW(L);
  Standard_Integer nbpoints = L.NbWPoint();
  // Define points3d with the walking 3d points of <L>
  TColgp_Array1OfPnt points3d(1,nbpoints); 
  for (ip = 1, itW.Init(); itW.More(); ip++, itW.Next()) {
    points3d.SetValue(ip,itW.CurrentWP().Value());
  }
  Handle(Geom_Curve) C=TopOpeBRepTool_CurveTool::MakeBSpline1fromPnt(points3d);
  return C;
}

//=======================================================================
//function : MakeBSpline1fromWALKING2d
//purpose  : 
//=======================================================================
Handle(Geom2d_Curve) TopOpeBRep_GeomTool::MakeBSpline1fromWALKING2d
(const TopOpeBRep_LineInter& L, const Standard_Integer SI)
{
  Standard_Integer ip;
  TopOpeBRep_WPointInterIterator itW(L);
  Standard_Integer nbpoints = L.NbWPoint();
  // Define points2d with the walking 2d points of <L>
  TColgp_Array1OfPnt2d points2d(1,nbpoints);
  for (ip = 1, itW.Init(); itW.More(); ip++, itW.Next()) {
    if      (SI == 1) points2d.SetValue(ip,itW.CurrentWP().ValueOnS1());
    else if (SI == 2) points2d.SetValue(ip,itW.CurrentWP().ValueOnS2());
  }
  Handle(Geom2d_Curve) C=TopOpeBRepTool_CurveTool::MakeBSpline1fromPnt2d(points2d);
  return C;
}
