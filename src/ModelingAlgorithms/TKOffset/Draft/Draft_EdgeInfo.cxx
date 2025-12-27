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
#include <TopoDS_Face.hxx>

//=================================================================================================

Draft_EdgeInfo::Draft_EdgeInfo()
    : myNewGeom(false),
      myTgt(false),
      myTol(0)
{
}

//=================================================================================================

Draft_EdgeInfo::Draft_EdgeInfo(const bool HasNewGeometry)
    : myNewGeom(HasNewGeometry),
      myTgt(false),
      myTol(0)
{
}

//=================================================================================================

void Draft_EdgeInfo::Add(const TopoDS_Face& F)
{
  if (myFirstF.IsNull())
  {
    myFirstF = F;
  }
  else if (!myFirstF.IsSame(F) && mySeconF.IsNull())
  {
    mySeconF = F;
  }
  myTol = std::max(myTol, BRep_Tool::Tolerance(F));
}

//=================================================================================================

void Draft_EdgeInfo::RootFace(const TopoDS_Face& F)
{
  myRootFace = F;
}

//=================================================================================================

void Draft_EdgeInfo::Tangent(const gp_Pnt& P)
{
  myTgt = true;
  myPt  = P;
}

//=================================================================================================

bool Draft_EdgeInfo::IsTangent(gp_Pnt& P) const
{
  P = myPt;
  return myTgt;
}

//=================================================================================================

bool Draft_EdgeInfo::NewGeometry() const
{
  return myNewGeom;
}

//=================================================================================================

void Draft_EdgeInfo::SetNewGeometry(const bool NewGeom)
{
  myNewGeom = NewGeom;
}

//=================================================================================================

const occ::handle<Geom_Curve>& Draft_EdgeInfo::Geometry() const
{
  return myGeom;
}

//=================================================================================================

const TopoDS_Face& Draft_EdgeInfo::FirstFace() const
{
  return myFirstF;
}

//=================================================================================================

const TopoDS_Face& Draft_EdgeInfo::SecondFace() const
{
  return mySeconF;
}

//=================================================================================================

occ::handle<Geom_Curve>& Draft_EdgeInfo::ChangeGeometry()
{
  return myGeom;
}

//=======================================================================
// function : occ::handle<Geom2d_Curve>&
// purpose  :
//=======================================================================

const occ::handle<Geom2d_Curve>& Draft_EdgeInfo::FirstPC() const
{
  return myFirstPC;
}

//=======================================================================
// function : occ::handle<Geom2d_Curve>&
// purpose  :
//=======================================================================

const occ::handle<Geom2d_Curve>& Draft_EdgeInfo::SecondPC() const
{
  return mySeconPC;
}

//=================================================================================================

occ::handle<Geom2d_Curve>& Draft_EdgeInfo::ChangeFirstPC()
{
  return myFirstPC;
}

//=================================================================================================

occ::handle<Geom2d_Curve>& Draft_EdgeInfo::ChangeSecondPC()
{
  return mySeconPC;
}

//=================================================================================================

const TopoDS_Face& Draft_EdgeInfo::RootFace() const
{
  return myRootFace;
}

//=================================================================================================

void Draft_EdgeInfo::Tolerance(const double tol)
{
  myTol = tol;
}

double Draft_EdgeInfo::Tolerance() const
{
  return myTol;
}
