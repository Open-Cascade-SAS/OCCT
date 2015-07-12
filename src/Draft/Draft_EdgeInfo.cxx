// Created on: 1994-08-31
// Created by: Jacques GOUSSARD
// Copyright (c) 1994-1999 Matra Datavision
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
#include <Draft_EdgeInfo.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <gp_Pnt.hxx>
#include <Standard_DomainError.hxx>
#include <TopoDS_Face.hxx>

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
//function : Handle(Geom2d_Curve)&
//purpose  : 
//=======================================================================

const Handle(Geom2d_Curve)& Draft_EdgeInfo::FirstPC() const 
{
  return myFirstPC;
}


//=======================================================================
//function : Handle(Geom2d_Curve)&
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


