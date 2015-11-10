// Created on: 1993-10-14
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

// Modified by skv - Fri Mar  4 15:50:09 2005
// Add methods for supporting history.

#include <BRep_TEdge.hxx>
#include <BRepLib.hxx>
#include <BRepPrimAPI_MakeRevol.hxx>
#include <BRepSweep_Revol.hxx>
#include <gp_Ax1.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Shape.hxx>

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
  BRepLib::UpdateInnerTolerances(myShape);
  
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
