// Created on: 1996-07-02
// Created by: Joelle CHAUVET
// Copyright (c) 1996-1999 Matra Datavision
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



#include <AdvApp2Var_Node.ixx>
#include <TColgp_HArray2OfPnt.hxx>
#include <TColStd_HArray2OfReal.hxx>


//=======================================================================
//function : AdvApp2Var_Node
//purpose  : 
//=======================================================================

AdvApp2Var_Node::AdvApp2Var_Node()  :
myOrdInU(2),
myOrdInV(2)
{
  myTruePoints = new TColgp_HArray2OfPnt  ( 0, 2, 0, 2);
  gp_Pnt P0(0.,0.,0.);
  myTruePoints->Init(P0);
  myErrors     = new TColStd_HArray2OfReal( 0, 2, 0, 2);
  myErrors->Init(0.);
}

//=======================================================================
//function : AdvApp2Var_Node
//purpose  : 
//=======================================================================

AdvApp2Var_Node::AdvApp2Var_Node(const Standard_Integer iu,
				 const Standard_Integer iv) :
myOrdInU(iu),
myOrdInV(iv)
{
  myTruePoints = new TColgp_HArray2OfPnt  ( 0, Max(0,iu), 0, Max(0,iv));
  gp_Pnt P0(0.,0.,0.);
  myTruePoints->Init(P0);
  myErrors     = new TColStd_HArray2OfReal( 0, Max(0,iu), 0, Max(0,iv));
  myErrors->Init(0.);
}

//=======================================================================
//function : AdvApp2Var_Node
//purpose  : 
//=======================================================================

AdvApp2Var_Node::AdvApp2Var_Node(const gp_XY& UV,
				 const Standard_Integer iu,
				 const Standard_Integer iv) :
myCoord(UV),
myOrdInU(iu),
myOrdInV(iv)
{
  myTruePoints = new TColgp_HArray2OfPnt  ( 0, iu, 0, iv);
  gp_Pnt P0(0.,0.,0.);
  myTruePoints->Init(P0);
  myErrors     = new TColStd_HArray2OfReal( 0, iu, 0, iv);
  myErrors->Init(0.);
}


//=======================================================================
//function : Coord
//purpose  : returns the coordinates (U,V) of the node
//=======================================================================

gp_XY AdvApp2Var_Node::Coord() const
{
  return myCoord;
}

//=======================================================================
//function : SetCoord
//purpose  : changes the coordinates (U,V) to (x1,x2)
//=======================================================================

void AdvApp2Var_Node::SetCoord(const Standard_Real x1,
			       const Standard_Real x2)
{
  myCoord.SetX(x1);
  myCoord.SetY(x2);
}

//=======================================================================
//function : UOrder
//purpose  : returns the continuity order in U of the node
//=======================================================================

Standard_Integer AdvApp2Var_Node::UOrder() const
{
  return myOrdInU;
}

//=======================================================================
//function : VOrder
//purpose  : returns the continuity order in V of the node
//=======================================================================

Standard_Integer AdvApp2Var_Node::VOrder() const
{
  return myOrdInV;
}


//=======================================================================
//function : SetPoint
//purpose  : affects the value F(U,V) or its derivates on the node (U,V)
//=======================================================================

void AdvApp2Var_Node::SetPoint(const Standard_Integer iu,
			       const Standard_Integer iv,
			       const gp_Pnt& Pt)
{
  myTruePoints->SetValue(iu, iv, Pt);
}


//=======================================================================
//function : Point
//purpose  : returns the value F(U,V) or its derivates on the node (U,V)
//=======================================================================

gp_Pnt AdvApp2Var_Node::Point(const Standard_Integer iu,
			      const Standard_Integer iv) const
{
  return myTruePoints->Value(iu, iv);
}


//=======================================================================
//function : SetError
//purpose  : affects the error between F(U,V) and its approximation
//=======================================================================

void AdvApp2Var_Node::SetError(const Standard_Integer iu,
			       const Standard_Integer iv,
			       const Standard_Real error)
{
  myErrors->SetValue(iu, iv, error);
}


//=======================================================================
//function : Error
//purpose  : returns the error between F(U,V) and its approximation
//=======================================================================

Standard_Real AdvApp2Var_Node::Error(const Standard_Integer iu,
				     const Standard_Integer iv) const
{
  return myErrors->Value(iu, iv);
}











