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



#include <BRepPrimAPI_MakeBox.ixx>
#include <BRepBuilderAPI.hxx>
#include <TopoDS.hxx>
#include <Primitives_Direction.hxx>


inline gp_Pnt pmin(const gp_Pnt& p, 
		   const Standard_Real dx,
		   const Standard_Real dy, 
		   const Standard_Real dz)
{
  gp_Pnt P = p;
  if (dx < 0) P.SetX(P.X()+dx);
  if (dy < 0) P.SetY(P.Y()+dy);
  if (dz < 0) P.SetZ(P.Z()+dz);
  return P;
}

//=======================================================================
//function : BRepPrimAPI_MakeBox
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeBox::BRepPrimAPI_MakeBox(const Standard_Real dx,
				 const Standard_Real dy,
				 const Standard_Real dz) :
       myWedge(gp_Ax2(pmin(gp_Pnt(0,0,0),dx,dy,dz),gp_Dir(0,0,1),gp_Dir(1,0,0)),
	       Abs(dx),Abs(dy),Abs(dz))
{
}


//=======================================================================
//function : BRepPrimAPI_MakeBox
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeBox::BRepPrimAPI_MakeBox(const gp_Pnt& P, 
				 const Standard_Real dx,
				 const Standard_Real dy, 
				 const Standard_Real dz) :
       myWedge(gp_Ax2(pmin(P,dx,dy,dz),gp_Dir(0,0,1),gp_Dir(1,0,0)),
	       Abs(dx),Abs(dy),Abs(dz))
{
}


//=======================================================================
//function : BRepPrimAPI_MakeBox
//purpose  : 
//=======================================================================

inline gp_Pnt pmin(const gp_Pnt& p1, const gp_Pnt& p2)
{
  return gp_Pnt(Min(p1.X(),p2.X()),Min(p1.Y(),p2.Y()),Min(p1.Z(),p2.Z()));
}

BRepPrimAPI_MakeBox::BRepPrimAPI_MakeBox(const gp_Pnt& P1, const gp_Pnt& P2) :
       myWedge(gp_Ax2(pmin(P1,P2),gp_Dir(0,0,1),gp_Dir(1,0,0)),
	       Abs(P2.X()-P1.X()),
	       Abs(P2.Y()-P1.Y()),
	       Abs(P2.Z()-P1.Z()))
{
}


//=======================================================================
//function : BRepPrimAPI_MakeBox
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeBox::BRepPrimAPI_MakeBox(const gp_Ax2& Axes, 
				 const Standard_Real dx, 
				 const Standard_Real dy, 
				 const Standard_Real dz) :
       myWedge(Axes,dx,dy,dz)
{
}


//=======================================================================
//function : Wedge
//purpose  : 
//=======================================================================

BRepPrim_Wedge&  BRepPrimAPI_MakeBox::Wedge()
{
  return myWedge;
}


//=======================================================================
//function : Shell
//purpose  : 
//=======================================================================

const TopoDS_Shell&  BRepPrimAPI_MakeBox::Shell()
{
  myShape = myWedge.Shell();
  Done();
  return TopoDS::Shell(myShape);
}

//=======================================================================
//function : Build
//purpose  : 
//=======================================================================

void BRepPrimAPI_MakeBox::Build()
{
  Solid();
}

//=======================================================================
//function : Solid
//purpose  : 
//=======================================================================

const TopoDS_Solid&  BRepPrimAPI_MakeBox::Solid()
{
  BRep_Builder B;
  B.MakeSolid(TopoDS::Solid(myShape));
  B.Add(myShape,myWedge.Shell());
  Done();
  return TopoDS::Solid(myShape);
}



//=======================================================================
//function : operator
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeBox::operator TopoDS_Shell()
{
  return Shell();
}

//=======================================================================
//function : operator
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeBox::operator TopoDS_Solid()
{
  return Solid();
}


//=======================================================================
//function : BottomFace
//purpose  : 
//=======================================================================

const TopoDS_Face& BRepPrimAPI_MakeBox::BottomFace () {

 return myWedge.Face (Primitives_ZMin);
}



//=======================================================================
//function : BackFace
//purpose  : 
//=======================================================================

const TopoDS_Face& BRepPrimAPI_MakeBox::BackFace () {

 return myWedge.Face (Primitives_XMin);
}


//=======================================================================
//function : FrontFace
//purpose  : 
//=======================================================================

const TopoDS_Face& BRepPrimAPI_MakeBox::FrontFace () {

 return myWedge.Face (Primitives_XMax);
}


//=======================================================================
//function : LeftFace
//purpose  : 
//=======================================================================

const TopoDS_Face& BRepPrimAPI_MakeBox::LeftFace () {

 return myWedge.Face (Primitives_YMin);
}


//=======================================================================
//function : RightFace
//purpose  : 
//=======================================================================

const TopoDS_Face& BRepPrimAPI_MakeBox::RightFace () {

 return myWedge.Face (Primitives_YMax);
}


//=======================================================================
//function : TopFace
//purpose  : 
//=======================================================================

const TopoDS_Face& BRepPrimAPI_MakeBox::TopFace () {

 return myWedge.Face (Primitives_ZMax);
}






