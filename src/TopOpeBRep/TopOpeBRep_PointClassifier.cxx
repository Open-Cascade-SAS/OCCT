// Created on: 1993-11-18
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


#include <BRepAdaptor_HSurface.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepTopAdaptor_TopolTool.hxx>
#include <gp_Pnt2d.hxx>
#include <TopoDS_Face.hxx>
#include <TopOpeBRep_PointClassifier.hxx>

//=======================================================================
//function : TopOpeBRep_PointClassifier
//purpose  : 
//=======================================================================
TopOpeBRep_PointClassifier::TopOpeBRep_PointClassifier()
{
  myHSurface = new BRepAdaptor_HSurface();
  Init();
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void TopOpeBRep_PointClassifier::Init() 
{
  myTopolToolMap.Clear();
  myState = TopAbs_UNKNOWN;
}

//=======================================================================
//function : Load
//purpose  : 
//=======================================================================

void TopOpeBRep_PointClassifier::Load(const TopoDS_Face& F) 
{
  Standard_Boolean found = myTopolToolMap.IsBound(F);
  if ( ! found ) {
    myHSurface->ChangeSurface().Initialize(F);
    myTopolTool = new BRepTopAdaptor_TopolTool(myHSurface);
    myTopolToolMap.Bind(F,myTopolTool);
  }
  else {
    myTopolTool = myTopolToolMap.Find(F);
  }
}

//=======================================================================
//function : Classify
//purpose  : 
//=======================================================================

TopAbs_State TopOpeBRep_PointClassifier::Classify
  (const TopoDS_Face& F, 
   const gp_Pnt2d& P2d, 
   const Standard_Real Tol)
{
  myState = TopAbs_UNKNOWN;
  Load(F);
  myState = myTopolTool->Classify(P2d,Tol);

  return myState;
}


//=======================================================================
//function : State
//purpose  : 
//=======================================================================

TopAbs_State TopOpeBRep_PointClassifier::State() const
{
  return myState;
}
