// Created on: 1993-10-14
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



// Modified by skv - Fri Mar  4 15:50:09 2005
// Add methods for supporting history.

#include <BRepPrimAPI_MakePrism.ixx>
#include <BRepLib.hxx>

// perform checks on the argument

static const TopoDS_Shape& check(const TopoDS_Shape& S)
{
 BRepLib::BuildCurves3d(S);
   return S;
}

//=======================================================================
//function : BRepPrimAPI_MakePrism
//purpose  : 
//=======================================================================

BRepPrimAPI_MakePrism::BRepPrimAPI_MakePrism(const TopoDS_Shape& S,
				     const gp_Vec& V, 
				     const Standard_Boolean Copy,
				     const Standard_Boolean Canonize) :
       myPrism(check(S),V,Copy,Canonize)
{
  Build();
}


//=======================================================================
//function : BRepPrimAPI_MakePrism
//purpose  : 
//=======================================================================

BRepPrimAPI_MakePrism::BRepPrimAPI_MakePrism(const TopoDS_Shape& S, 
				     const gp_Dir& D,
				     const Standard_Boolean Inf, 
				     const Standard_Boolean Copy,
				     const Standard_Boolean Canonize) :
       myPrism(check(S),D,Inf,Copy,Canonize)
{
  Build();
}


//=======================================================================
//function : Prism
//purpose  : 
//=======================================================================

const BRepSweep_Prism&  BRepPrimAPI_MakePrism::Prism()const 
{
  return myPrism;
}


//=======================================================================
//function : Build
//purpose  : 
//=======================================================================

void  BRepPrimAPI_MakePrism::Build()
{
  myShape = myPrism.Shape();
  Done();
}

//=======================================================================
//function : FirstShape
//purpose  : 
//=======================================================================

TopoDS_Shape BRepPrimAPI_MakePrism::FirstShape()
{
  return myPrism.FirstShape();
}


//=======================================================================
//function : LastShape
//purpose  : 
//=======================================================================

TopoDS_Shape BRepPrimAPI_MakePrism::LastShape()
{
  return myPrism.LastShape();
}


//=======================================================================
//function : Generated
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepPrimAPI_MakePrism::Generated (const TopoDS_Shape& S)
{
  myGenerated.Clear();
  myGenerated.Append(myPrism.Shape (S));
  return myGenerated;

}


// Modified by skv - Fri Mar  4 15:50:09 2005 Begin

//=======================================================================
//function : FirstShape
//purpose  : This method returns the bottom shape of the prism, generated
//           with theShape (subShape of the generating shape)
//=======================================================================

TopoDS_Shape BRepPrimAPI_MakePrism::FirstShape(const TopoDS_Shape &theShape)
{
  return myPrism.FirstShape(theShape);
}


//=======================================================================
//function : LastShape
//purpose  : This method returns the top shape of the prism, generated
//           with theShape (subShape of the generating shape)
//=======================================================================

TopoDS_Shape BRepPrimAPI_MakePrism::LastShape(const TopoDS_Shape &theShape)
{
  return myPrism.LastShape(theShape);
}

// Modified by skv - Fri Mar  4 15:50:09 2005 End
