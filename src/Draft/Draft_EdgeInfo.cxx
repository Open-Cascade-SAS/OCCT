// Created on: 1994-08-31
// Created by: Jacques GOUSSARD
// Copyright (c) 1994-1999 Matra Datavision
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



#include <Draft_EdgeInfo.ixx>

#include <Standard_DomainError.hxx>
#include <BRep_Tool.hxx>


//=======================================================================
//function : Draft_EdgeInfo
//purpose  : 
//=======================================================================

Draft_EdgeInfo::Draft_EdgeInfo():
       myNewGeom(Standard_False),myTgt(Standard_False),myTol(0)
{}


//=======================================================================
//function : Draft_EdgeInfo
//purpose  : 
//=======================================================================

Draft_EdgeInfo::Draft_EdgeInfo(const Standard_Boolean HasNewGeometry):
       myNewGeom(HasNewGeometry),myTgt(Standard_False),myTol(0)
{}


//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void Draft_EdgeInfo::Add(const TopoDS_Face& F)
{
  if (myFirstF.IsNull()) {
    myFirstF = F;
  }
  else if (!myFirstF.IsSame(F) && mySeconF.IsNull()) {
    mySeconF = F;
  }
  myTol=Max(myTol, BRep_Tool::Tolerance(F));
}

//=======================================================================
//function : RootFace
//purpose  : 
//=======================================================================

void Draft_EdgeInfo::RootFace(const TopoDS_Face& F)
{
  myRootFace = F;
}



//=======================================================================
//function : Tangent
//purpose  : 
//=======================================================================

void Draft_EdgeInfo::Tangent(const gp_Pnt& P)
{
  myTgt = Standard_True;
  myPt  = P;
}

//=======================================================================
//function : IsTangent
//purpose  : 
//=======================================================================

Standard_Boolean Draft_EdgeInfo::IsTangent(gp_Pnt& P) const
{
  P = myPt;
  return myTgt;
}

//=======================================================================
//function : NewGeometry
//purpose  : 
//=======================================================================

Standard_Boolean Draft_EdgeInfo::NewGeometry() const
{
  return myNewGeom;
}

//=======================================================================
//function : SetNewGeometry
//purpose  : 
//=======================================================================

void Draft_EdgeInfo::SetNewGeometry( const Standard_Boolean NewGeom )
{
  myNewGeom = NewGeom;
}

//=======================================================================
//function : Geometry
//purpose  : 
//=======================================================================

const Handle(Geom_Curve)& Draft_EdgeInfo::Geometry() const
{
  return myGeom;
}


//=======================================================================
//function : FirstFace
//purpose  : 
//=======================================================================

const TopoDS_Face& Draft_EdgeInfo::FirstFace () const
{
  return myFirstF;
}


//=======================================================================
//function : SecondFace
//purpose  : 
//=======================================================================

const TopoDS_Face& Draft_EdgeInfo::SecondFace () const
{
  return mySeconF;
}


//=======================================================================
//function : ChangeGeometry
//purpose  : 
//=======================================================================

Handle(Geom_Curve)& Draft_EdgeInfo::ChangeGeometry()
{
  return myGeom;
}

//=======================================================================
//function : Handle_Geom2d_Curve&
//purpose  : 
//=======================================================================

const Handle(Geom2d_Curve)& Draft_EdgeInfo::FirstPC() const 
{
  return myFirstPC;
}


//=======================================================================
//function : Handle_Geom2d_Curve&
//purpose  : 
//=======================================================================

const Handle(Geom2d_Curve)& Draft_EdgeInfo::SecondPC() const 
{
  return mySeconPC;
}

//=======================================================================
//function : ChangeFirstPC
//purpose  : 
//=======================================================================

Handle(Geom2d_Curve)& Draft_EdgeInfo::ChangeFirstPC()
{
  return myFirstPC;
}


//=======================================================================
//function : ChangeSecondPC
//purpose  : 
//=======================================================================

Handle(Geom2d_Curve)& Draft_EdgeInfo::ChangeSecondPC()
{
  return mySeconPC;
}


//=======================================================================
//function : RootFace
//purpose  : 
//=======================================================================

const TopoDS_Face & Draft_EdgeInfo::RootFace() const
{
  return myRootFace;
}

//=======================================================================
//function : Tolerance
//purpose  : 
//=======================================================================

void  Draft_EdgeInfo::Tolerance(const Standard_Real tol)
{
  myTol=tol;
}
Standard_Real  Draft_EdgeInfo::Tolerance() const
{
  return myTol;
}


