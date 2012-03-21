// Created on: 1993-11-18
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


#include <TopOpeBRep_PointClassifier.ixx>

#include <BRepAdaptor_Surface.hxx>

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
