// Created on: 1998-03-23
// Created by: Jean Yves LEBEY
// Copyright (c) 1998-1999 Matra Datavision
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


#include <TopOpeBRepTool_C2DF.ixx>
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

