// Created on: 1998-03-23
// Created by: Jean Yves LEBEY
// Copyright (c) 1998-1999 Matra Datavision
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
#include <TopoDS_Face.hxx>
#include <TopOpeBRepTool_C2DF.hxx>
#include <TopOpeBRepTool_define.hxx>

//=======================================================================
//function : TopOpeBRepTool_C2DF
//purpose  : 
//=======================================================================
TopOpeBRepTool_C2DF::TopOpeBRepTool_C2DF() {}

//=======================================================================
//function : TopOpeBRepTool_C2DF
//purpose  : 
//=======================================================================

TopOpeBRepTool_C2DF::TopOpeBRepTool_C2DF
(const Handle(Geom2d_Curve)& PC,const Standard_Real f2d,const Standard_Real l2d,const Standard_Real tol,const TopoDS_Face& F)
{
  myPC = PC;myf2d = f2d;myl2d = l2d;mytol = tol;
  myFace = F;
}

//=======================================================================
//function : SetPC
//purpose  : 
//=======================================================================

void TopOpeBRepTool_C2DF::SetPC
(const Handle(Geom2d_Curve)& PC,const Standard_Real f2d,const Standard_Real l2d,const Standard_Real tol)
{
  myPC = PC;myf2d = f2d;myl2d = l2d;mytol = tol;
}

//=======================================================================
//function : SetFace
//purpose  : 
//=======================================================================

void TopOpeBRepTool_C2DF::SetFace(const TopoDS_Face& F)
{
  myFace = F;
}

//=======================================================================
//function : PC
//purpose  : 
//=======================================================================

const Handle(Geom2d_Curve)& TopOpeBRepTool_C2DF::PC(Standard_Real& f2d,Standard_Real& l2d,Standard_Real& tol) const
{
  f2d = myf2d;l2d = myl2d;tol = mytol;
  return myPC;
}

//=======================================================================
//function : Face
//purpose  : 
//=======================================================================

const TopoDS_Face& TopOpeBRepTool_C2DF::Face() const
{
  return myFace;
}

//=======================================================================
//function : IsPC
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_C2DF::IsPC(const Handle(Geom2d_Curve)& PC) const
{
  Standard_Boolean b = (PC == myPC);
  return b;
}

//=======================================================================
//function : IsFace
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_C2DF::IsFace(const TopoDS_Face& F) const
{
  Standard_Boolean b = (F.IsEqual(myFace));
  return b;
}

