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

#include <BRepPrimAPI_MakeRevol.ixx>
#include <BRepLib.hxx>
#include <TopExp_Explorer.hxx>
#include <BRep_TEdge.hxx>

// perform checks on the argument

static const TopoDS_Shape& check(const TopoDS_Shape& S)
{
 BRepLib::BuildCurves3d(S);
   return S;
}

//=======================================================================
//function : BRepPrimAPI_MakeRevol
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeRevol::BRepPrimAPI_MakeRevol(const TopoDS_Shape& S, 
				     const gp_Ax1& A, 
				     const Standard_Real D, 
				     const Standard_Boolean Copy) :
       myRevol(check(S),A,D,Copy)
{
  Build();
}


//=======================================================================
//function : BRepPrimAPI_MakeRevol
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeRevol::BRepPrimAPI_MakeRevol(const TopoDS_Shape& S, 
				     const gp_Ax1& A, 
				     const Standard_Boolean Copy) :
       myRevol(check(S),A,Copy)
{
  Build();
}


//=======================================================================
//function : Revol
//purpose  : 
//=======================================================================

const BRepSweep_Revol&  BRepPrimAPI_MakeRevol::Revol() const 
{
  return myRevol;
}


//=======================================================================
//function : Build
//purpose  : 
//=======================================================================

void  BRepPrimAPI_MakeRevol::Build()
{
  myShape = myRevol.Shape();
  Done();
// Modified by skv - Fri Mar  4 15:50:09 2005 Begin
  myDegenerated.Clear();

  TopExp_Explorer anExp(myShape, TopAbs_EDGE);

  for (; anExp.More(); anExp.Next()) {
    const TopoDS_Shape &anEdge = anExp.Current();
    Handle(BRep_TEdge)  aTEdge = Handle(BRep_TEdge)::DownCast(anEdge.TShape());

    if (aTEdge->Degenerated())
      myDegenerated.Append(anEdge);
  }
// Modified by skv - Fri Mar  4 15:50:09 2005 End
}


//=======================================================================
//function : FirstShape
//purpose  : 
//=======================================================================

TopoDS_Shape BRepPrimAPI_MakeRevol::FirstShape()
{
  return myRevol.FirstShape();
}


//=======================================================================
//function : LastShape
//purpose  : 
//=======================================================================

TopoDS_Shape BRepPrimAPI_MakeRevol::LastShape()
{
  return myRevol.LastShape();
}


//=======================================================================
//function : Generated
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepPrimAPI_MakeRevol::Generated (const TopoDS_Shape& S)
{
  myGenerated.Clear();
  if (!myRevol.Shape (S).IsNull())
    myGenerated.Append (myRevol.Shape (S));
  return myGenerated;
}


// Modified by skv - Fri Mar  4 15:50:09 2005 Begin

//=======================================================================
//function : FirstShape
//purpose  : This method returns the shape of the beginning of the revolution,
//           generated with theShape (subShape of the generating shape).
//=======================================================================

TopoDS_Shape BRepPrimAPI_MakeRevol::FirstShape(const TopoDS_Shape &theShape)
{
  return myRevol.FirstShape(theShape);
}


//=======================================================================
//function : LastShape
//purpose  : This method returns the shape of the end of the revolution,
//           generated with theShape (subShape of the generating shape).
//=======================================================================

TopoDS_Shape BRepPrimAPI_MakeRevol::LastShape(const TopoDS_Shape &theShape)
{
  return myRevol.LastShape(theShape);
}

//=======================================================================
//function : HasDegenerated
//purpose  : 
//=======================================================================

Standard_Boolean BRepPrimAPI_MakeRevol::HasDegenerated () const
{
  return (!myDegenerated.IsEmpty());
}


//=======================================================================
//function : Degenerated
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepPrimAPI_MakeRevol::Degenerated () const
{
  return myDegenerated;
}
// Modified by skv - Fri Mar  4 15:50:09 2005 End
