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


#include <Draft_FaceInfo.hxx>
#include <Geom_Curve.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <Standard_DomainError.hxx>
#include <TopoDS_Face.hxx>

//=======================================================================
//function : Draft_FaceInfo
//purpose  : 
//=======================================================================
Draft_FaceInfo::Draft_FaceInfo ()
{
}

//=======================================================================
//function : Draft_FaceInfo
//purpose  : 
//=======================================================================

Draft_FaceInfo::Draft_FaceInfo (const Handle(Geom_Surface)& S,\
				const Standard_Boolean HasNewGeometry):
       myNewGeom(HasNewGeometry)
{
  Handle(Geom_RectangularTrimmedSurface) T = Handle(Geom_RectangularTrimmedSurface)::DownCast(S);
  if (!T.IsNull()) myGeom = T->BasisSurface();
  else             myGeom = S;
}


//=======================================================================
//function : RootFace
//purpose  : 
//=======================================================================

void Draft_FaceInfo::RootFace(const TopoDS_Face& F)
{
  myRootFace = F;
}



//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void Draft_FaceInfo::Add(const TopoDS_Face& F)
{
  if (myF1.IsNull()) {
    myF1 = F;
  }
  else if (myF2.IsNull()) {
    myF2 = F;
  }
}


//=======================================================================
//function : FirstFace
//purpose  : 
//=======================================================================

const TopoDS_Face& Draft_FaceInfo::FirstFace () const
{
  return myF1;
}


//=======================================================================
//function : SecondFace
//purpose  : 
//=======================================================================

const TopoDS_Face& Draft_FaceInfo::SecondFace () const
{
  return myF2;
}


//=======================================================================
//function : NewGeometry
//purpose  : 
//=======================================================================

Standard_Boolean Draft_FaceInfo::NewGeometry() const
{
  return myNewGeom;
}

//=======================================================================
//function : Geometry
//purpose  : 
//=======================================================================

const Handle(Geom_Surface)& Draft_FaceInfo::Geometry() const
{
  return myGeom;
}

//=======================================================================
//function : ChangeGeometry
//purpose  : 
//=======================================================================

Handle(Geom_Surface)& Draft_FaceInfo::ChangeGeometry()
{
  return myGeom;
}

//=======================================================================
//function : Curve
//purpose  : 
//=======================================================================

const Handle(Geom_Curve)& Draft_FaceInfo::Curve() const
{
  return myCurv;
}

//=======================================================================
//function : ChangeCurve
//purpose  : 
//=======================================================================

Handle(Geom_Curve)& Draft_FaceInfo::ChangeCurve()
{
  return myCurv;
}

//=======================================================================
//function : RootFace
//purpose  : 
//=======================================================================

const TopoDS_Face & Draft_FaceInfo::RootFace() const
{
  return myRootFace;
}


