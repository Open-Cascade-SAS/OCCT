// Created on: 1993-07-23
// Created by: Remi LEQUETTE
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


#include <BRepBuilderAPI_MakeFace.hxx>
#include <Geom_Surface.hxx>
#include <gp_Cone.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Pln.hxx>
#include <gp_Sphere.hxx>
#include <gp_Torus.hxx>
#include <StdFail_NotDone.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>

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
