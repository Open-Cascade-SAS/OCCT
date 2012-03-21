// Created on: 1994-11-14
// Created by: Jean Yves LEBEY
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


#include <TopOpeBRepBuild_Pave.ixx>
#include <TopAbs.hxx>

//=======================================================================
//function : TopOpeBRepBuild_Pave
//purpose  : 
//=======================================================================

TopOpeBRepBuild_Pave::TopOpeBRepBuild_Pave
(const TopoDS_Shape& V, const Standard_Real P, const Standard_Boolean B) :
TopOpeBRepBuild_Loop(V),
myVertex(V),myParam(P),myIsShape(B),myHasSameDomain(Standard_False),myIntType(TopOpeBRepDS_FACE)
{
}

//=======================================================================
//function : HasSameDomain
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_Pave::HasSameDomain(const Standard_Boolean B) 
{
  myHasSameDomain = B;
}

//=======================================================================
//function : SameDomain
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_Pave::SameDomain(const TopoDS_Shape& VSD) 
{
  mySameDomain = VSD;
}

//=======================================================================
//function : HasSameDomain
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepBuild_Pave::HasSameDomain() const
{
  return myHasSameDomain;
}

//=======================================================================
//function : SameDomain
//purpose  : 
//=======================================================================

const TopoDS_Shape& TopOpeBRepBuild_Pave::SameDomain() const
{
  return mySameDomain;
}

//=======================================================================
//function : Vertex
//purpose  : 
//=======================================================================

const TopoDS_Shape& TopOpeBRepBuild_Pave::Vertex() const 
{
  return myVertex;
}

//=======================================================================
//function : ChangeVertex
//purpose  : 
//=======================================================================

TopoDS_Shape& TopOpeBRepBuild_Pave::ChangeVertex()
{
  return myVertex;
}

//=======================================================================
//function : Parameter
//purpose  : 
//=======================================================================

Standard_Real TopOpeBRepBuild_Pave::Parameter() const 
{
  return myParam;
}

//modified by NIZHNY-MZV  Mon Feb 21 14:11:40 2000
//=======================================================================
//function : Parameter
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Pave::Parameter(const Standard_Real Par)  
{
  myParam = Par;
}

//=======================================================================
//function : IsShape
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepBuild_Pave::IsShape() const 
{
  return myIsShape;
}

//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================

const TopoDS_Shape& TopOpeBRepBuild_Pave::Shape() const 
{
  return myVertex;
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_Pave::Dump() const 
{
#ifdef DEB
  cout<<Parameter()<<" ";TopAbs::Print(Vertex().Orientation(),cout);
#endif
}

//modified by NIZHNY-MZV  Mon Feb 21 14:27:48 2000
//=======================================================================
//function : ChangeVertex
//purpose  : 
//=======================================================================

TopOpeBRepDS_Kind& TopOpeBRepBuild_Pave::InterferenceType()
{
  return myIntType;
}
