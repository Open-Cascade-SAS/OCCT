// Created on: 1993-07-23
// Created by: Remi LEQUETTE
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



#include <BRepBuilderAPI_MakeFace.ixx>


//=======================================================================
//function : BRepBuilderAPI_MakeFace
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeFace::BRepBuilderAPI_MakeFace()
{
}


//=======================================================================
//function : BRepBuilderAPI_MakeFace
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeFace::BRepBuilderAPI_MakeFace(const TopoDS_Face& F)
: myMakeFace(F)
{
  if ( myMakeFace.IsDone()) {
    Done();
    myShape = myMakeFace.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeFace
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeFace::BRepBuilderAPI_MakeFace(const gp_Pln& P)
: myMakeFace(P)
{
  if ( myMakeFace.IsDone()) {
    Done();
    myShape = myMakeFace.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeFace
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeFace::BRepBuilderAPI_MakeFace(const gp_Cylinder& C)
: myMakeFace(C)
{
  if ( myMakeFace.IsDone()) {
    Done();
    myShape = myMakeFace.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeFace
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeFace::BRepBuilderAPI_MakeFace(const gp_Cone& C)
: myMakeFace(C)
{
  if ( myMakeFace.IsDone()) {
    Done();
    myShape = myMakeFace.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeFace
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeFace::BRepBuilderAPI_MakeFace(const gp_Sphere& S)
: myMakeFace(S)
{
  if ( myMakeFace.IsDone()) {
    Done();
    myShape = myMakeFace.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeFace
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeFace::BRepBuilderAPI_MakeFace(const gp_Torus& T)
: myMakeFace(T)
{
  if ( myMakeFace.IsDone()) {
    Done();
    myShape = myMakeFace.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeFace
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeFace::BRepBuilderAPI_MakeFace(const Handle(Geom_Surface)& S,
                                                 const Standard_Real TolDegen)
: myMakeFace(S, TolDegen)
{
  if ( myMakeFace.IsDone()) {
    Done();
    myShape = myMakeFace.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeFace
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeFace::BRepBuilderAPI_MakeFace(const gp_Pln& P,
				   const Standard_Real UMin,
				   const Standard_Real UMax,
				   const Standard_Real VMin,
				   const Standard_Real VMax)
: myMakeFace(P,UMin,UMax,VMin,VMax)
{
  if ( myMakeFace.IsDone()) {
    Done();
    myShape = myMakeFace.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeFace
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeFace::BRepBuilderAPI_MakeFace(const gp_Cylinder& C,
				   const Standard_Real UMin,
				   const Standard_Real UMax,
				   const Standard_Real VMin,
				   const Standard_Real VMax)
: myMakeFace(C,UMin,UMax,VMin,VMax)
{
  if ( myMakeFace.IsDone()) {
    Done();
    myShape = myMakeFace.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeFace
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeFace::BRepBuilderAPI_MakeFace(const gp_Cone& C,
				   const Standard_Real UMin,
				   const Standard_Real UMax,
				   const Standard_Real VMin,
				   const Standard_Real VMax)
: myMakeFace(C,UMin,UMax,VMin,VMax)
{
  if ( myMakeFace.IsDone()) {
    Done();
    myShape = myMakeFace.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeFace
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeFace::BRepBuilderAPI_MakeFace(const gp_Sphere& S,
				   const Standard_Real UMin,
				   const Standard_Real UMax,
				   const Standard_Real VMin,
				   const Standard_Real VMax)
: myMakeFace(S,UMin,UMax,VMin,VMax)
{
  if ( myMakeFace.IsDone()) {
    Done();
    myShape = myMakeFace.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeFace
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeFace::BRepBuilderAPI_MakeFace(const gp_Torus& T,
				   const Standard_Real UMin,
				   const Standard_Real UMax,
				   const Standard_Real VMin,
				   const Standard_Real VMax)
: myMakeFace(T,UMin,UMax,VMin,VMax)
{
  if ( myMakeFace.IsDone()) {
    Done();
    myShape = myMakeFace.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeFace
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeFace::BRepBuilderAPI_MakeFace (const Handle(Geom_Surface)& S,
                                                  const Standard_Real UMin,
                                                  const Standard_Real UMax,
                                                  const Standard_Real VMin,
                                                  const Standard_Real VMax,
                                                  const Standard_Real TolDegen)
: myMakeFace (S, UMin, UMax, VMin, VMax, TolDegen)
{
  if ( myMakeFace.IsDone()) {
    Done();
    myShape = myMakeFace.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeFace
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeFace::BRepBuilderAPI_MakeFace(const TopoDS_Wire& W,
				   const Standard_Boolean OnlyPlane)
: myMakeFace(W,OnlyPlane)
{
  if ( myMakeFace.IsDone()) {
    Done();
    myShape = myMakeFace.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeFace
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeFace::BRepBuilderAPI_MakeFace(const gp_Pln& P,
				   const TopoDS_Wire& W,
				   const Standard_Boolean Inside)
: myMakeFace(P,W,Inside)
{
  if ( myMakeFace.IsDone()) {
    Done();
    myShape = myMakeFace.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeFace
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeFace::BRepBuilderAPI_MakeFace(const gp_Cylinder& C,
				   const TopoDS_Wire& W,
				   const Standard_Boolean Inside)
: myMakeFace(C,W,Inside)
{
  if ( myMakeFace.IsDone()) {
    Done();
    myShape = myMakeFace.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeFace
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeFace::BRepBuilderAPI_MakeFace(const gp_Cone& C,
				   const TopoDS_Wire& W,
				   const Standard_Boolean Inside)
: myMakeFace(C,W,Inside)
{
  if ( myMakeFace.IsDone()) {
    Done();
    myShape = myMakeFace.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeFace
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeFace::BRepBuilderAPI_MakeFace(const gp_Sphere& S,
				   const TopoDS_Wire& W,
				   const Standard_Boolean Inside)
: myMakeFace(S,W,Inside)
{
  if ( myMakeFace.IsDone()) {
    Done();
    myShape = myMakeFace.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeFace
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeFace::BRepBuilderAPI_MakeFace(const gp_Torus& T,
				   const TopoDS_Wire& W,
				   const Standard_Boolean Inside)
: myMakeFace(T,W,Inside)
{
  if ( myMakeFace.IsDone()) {
    Done();
    myShape = myMakeFace.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeFace
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeFace::BRepBuilderAPI_MakeFace(const Handle(Geom_Surface)& S,
				   const TopoDS_Wire& W,
				   const Standard_Boolean Inside)
: myMakeFace(S,W,Inside)
{
  if ( myMakeFace.IsDone()) {
    Done();
    myShape = myMakeFace.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeFace
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeFace::BRepBuilderAPI_MakeFace(const TopoDS_Face& F,
				   const TopoDS_Wire& W)
: myMakeFace(F,W)
{
  if ( myMakeFace.IsDone()) {
    Done();
    myShape = myMakeFace.Shape();
  }
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void BRepBuilderAPI_MakeFace::Init(const TopoDS_Face& F)
{
  myMakeFace.Init(F);
  if ( myMakeFace.IsDone()) {
    Done();
    myShape = myMakeFace.Shape();
  }
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void  BRepBuilderAPI_MakeFace::Init(const Handle(Geom_Surface)& S,
                                    const Standard_Boolean Bound,
                                    const Standard_Real TolDegen)
{
  myMakeFace.Init(S, Bound, TolDegen);
  if ( myMakeFace.IsDone()) {
    Done();
    myShape = myMakeFace.Shape();
  }
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void  BRepBuilderAPI_MakeFace::Init (const Handle(Geom_Surface)& SS,
                                     const Standard_Real Um,
                                     const Standard_Real UM,
                                     const Standard_Real Vm,
                                     const Standard_Real VM,
                                     const Standard_Real TolDegen)
{
  myMakeFace.Init (SS, Um, UM, Vm, VM, TolDegen);
  if ( myMakeFace.IsDone()) {
    Done();
    myShape = myMakeFace.Shape();
  }
}


//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void  BRepBuilderAPI_MakeFace::Add(const TopoDS_Wire& W)
{
  myMakeFace.Add(W);
  if ( myMakeFace.IsDone()) {
    Done();
    myShape = myMakeFace.Shape();
  }
}


//=======================================================================
//function : Face
//purpose  : 
//=======================================================================

const TopoDS_Face&  BRepBuilderAPI_MakeFace::Face()const 
{
  return myMakeFace.Face();
}



//=======================================================================
//function : operator
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeFace::operator TopoDS_Face() const
{
  return Face();
}

//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================

Standard_Boolean BRepBuilderAPI_MakeFace::IsDone() const
{
  return myMakeFace.IsDone();
}


//=======================================================================
//function : Error
//purpose  : 
//=======================================================================

BRepBuilderAPI_FaceError BRepBuilderAPI_MakeFace::Error() const
{
  switch ( myMakeFace.Error()) {

  case BRepLib_FaceDone:
    return BRepBuilderAPI_FaceDone;

  case BRepLib_NoFace:
    return BRepBuilderAPI_NoFace;

  case BRepLib_NotPlanar:
    return BRepBuilderAPI_NotPlanar;

  case BRepLib_CurveProjectionFailed:
    return BRepBuilderAPI_CurveProjectionFailed;

  case BRepLib_ParametersOutOfRange:
    return BRepBuilderAPI_ParametersOutOfRange;

  }

  // Portage WNT
  return BRepBuilderAPI_FaceDone;
}
