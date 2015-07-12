// Created on: 1993-06-24
// Created by: Jean Yves LEBEY
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


#include <Geom2d_Curve.hxx>
#include <IntSurf_Situation.hxx>
#include <IntSurf_TypeTrans.hxx>
#include <TopAbs.hxx>
#include <TopOpeBRepDS_CurvePointInterference.hxx>
#include <TopOpeBRepDS_define.hxx>
#include <TopOpeBRepDS_EdgeVertexInterference.hxx>
#include <TopOpeBRepDS_FaceEdgeInterference.hxx>
#include <TopOpeBRepDS_Interference.hxx>
#include <TopOpeBRepDS_InterferenceTool.hxx>
#include <TopOpeBRepDS_SolidSurfaceInterference.hxx>
#include <TopOpeBRepDS_SurfaceCurveInterference.hxx>
#include <TopOpeBRepDS_Transition.hxx>

//=======================================================================
//function : MakeEdgeInterference
//purpose  : 
//=======================================================================
Handle(TopOpeBRepDS_Interference) TopOpeBRepDS_InterferenceTool::MakeEdgeInterference
(const TopOpeBRepDS_Transition& T,const TopOpeBRepDS_Kind SK,const Standard_Integer SI,const TopOpeBRepDS_Kind GK,const Standard_Integer GI,const Standard_Real P)
{
  return new TopOpeBRepDS_CurvePointInterference(T,SK,SI,GK,GI,P);
}

//=======================================================================
//function : MakeCurveInterference
//purpose  : 
//=======================================================================
Handle(TopOpeBRepDS_Interference) TopOpeBRepDS_InterferenceTool::MakeCurveInterference
(const TopOpeBRepDS_Transition& T,const TopOpeBRepDS_Kind SK,const Standard_Integer SI,const TopOpeBRepDS_Kind GK,const Standard_Integer GI,const Standard_Real P)
{
  return new TopOpeBRepDS_CurvePointInterference(T,SK,SI,GK,GI,P);
}

//=======================================================================
//function : DuplicateCurvePointInterference
//purpose  : 
//=======================================================================
Handle(TopOpeBRepDS_Interference) TopOpeBRepDS_InterferenceTool::DuplicateCurvePointInterference(const Handle(TopOpeBRepDS_Interference)& I)
{
  return new TopOpeBRepDS_CurvePointInterference
    (I->Transition().Complement(),
     I->SupportType(),I->Support(),
     I->GeometryType(),I->Geometry(),
     TopOpeBRepDS_InterferenceTool::Parameter(I));
}


//=======================================================================
//function : MakeFaceCurveInterference
//purpose  : 
//=======================================================================
Handle(TopOpeBRepDS_Interference) TopOpeBRepDS_InterferenceTool::MakeFaceCurveInterference
(const TopOpeBRepDS_Transition& Transition,const Standard_Integer FaceI,const Standard_Integer CurveI,const Handle(Geom2d_Curve)& PC)
{
  return new TopOpeBRepDS_SurfaceCurveInterference
    (Transition,
     TopOpeBRepDS_FACE,FaceI,
     TopOpeBRepDS_CURVE,CurveI,
     PC);
}


//=======================================================================
//function : MakeSolidSurfaceInterference
//purpose  : 
//=======================================================================
Handle(TopOpeBRepDS_Interference) TopOpeBRepDS_InterferenceTool::MakeSolidSurfaceInterference
(const TopOpeBRepDS_Transition& Transition,const Standard_Integer SolidI,const Standard_Integer SurfaceI)
{
  return new TopOpeBRepDS_SolidSurfaceInterference
    (Transition,
     TopOpeBRepDS_SOLID,SolidI,
     TopOpeBRepDS_SURFACE,SurfaceI);
}


//=======================================================================
//function : MakeEdgeVertexInterference
//purpose  : 
//=======================================================================
Handle(TopOpeBRepDS_Interference) TopOpeBRepDS_InterferenceTool::MakeEdgeVertexInterference
(const TopOpeBRepDS_Transition& Transition,
 const Standard_Integer EdgeI, 
 const Standard_Integer VertexI,
 const Standard_Boolean VertexIsBound,
 const TopOpeBRepDS_Config C,
 const Standard_Real param)
{
  // soit I l'EVI creee : 
  return new TopOpeBRepDS_EdgeVertexInterference
    (Transition,       // transition en cheminant sur l'arete accedante a I
     EdgeI,            // arete croisee par l'arete accedante, en V
     VertexI,          // vertex geometrie de I
     VertexIsBound,    // vertex est il un sommet de l'arete accedante de I
     C,                // orient. relative des aretes accedante et croisee.
     param);           // parametre de V sur arete accedante de I
}

//=======================================================================
//function : MakeFaceEdgeInterference
//purpose  : 
//=======================================================================
Handle(TopOpeBRepDS_Interference) TopOpeBRepDS_InterferenceTool::MakeFaceEdgeInterference
(const TopOpeBRepDS_Transition& Transition,
 const Standard_Integer FaceI, 
 const Standard_Integer EdgeI,
 const Standard_Boolean EdgeIsBound,
 const TopOpeBRepDS_Config C)
{
  return new TopOpeBRepDS_FaceEdgeInterference
    (Transition,
     FaceI,
     EdgeI,
     EdgeIsBound,
     C);
}

//=======================================================================
//function : Parameter
//purpose  : only on I = CurvePointInterference
//=======================================================================
Standard_Real TopOpeBRepDS_InterferenceTool::Parameter(const Handle(TopOpeBRepDS_Interference)& I)
{
  return Handle(TopOpeBRepDS_CurvePointInterference)::DownCast (I)->Parameter();
}

//=======================================================================
//function : Parameter
//purpose  : only on I = CurvePointInterference
//=======================================================================
void TopOpeBRepDS_InterferenceTool::Parameter(const Handle(TopOpeBRepDS_Interference)& I,const Standard_Real Par)
{
  Handle(TopOpeBRepDS_CurvePointInterference)::DownCast (I)->Parameter(Par);
}
