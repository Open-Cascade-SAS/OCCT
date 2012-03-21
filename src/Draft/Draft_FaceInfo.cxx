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



#include <Draft_FaceInfo.ixx>
#include <Geom_RectangularTrimmedSurface.hxx>


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


