// Created on: 1991-04-09
// Created by: Remi LEQUETTE
// Copyright (c) 1991-1999 Matra Datavision
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


#include <Standard_NullObject.hxx>
#include <TopoDS_Builder.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_CompSolid.hxx>
#include <TopoDS_FrozenShape.hxx>
#include <TopoDS_ListIteratorOfListOfShape.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_TCompound.hxx>
#include <TopoDS_TShape.hxx>
#include <TopoDS_TWire.hxx>
#include <TopoDS_UnCompatibleShapes.hxx>
#include <TopoDS_Wire.hxx>

//=======================================================================
//function : MakeShape
//purpose  : Make a Shape from a TShape
//=======================================================================
void TopoDS_Builder::MakeShape (TopoDS_Shape& S, 
                                const Handle(TopoDS_TShape)& T) const
{
  S.TShape(T);
  S.Location(TopLoc_Location());
  S.Orientation(TopAbs_FORWARD);
}


//=======================================================================
//function : Add
//purpose  : insert aComponent in aShape
//=======================================================================

void TopoDS_Builder::Add (TopoDS_Shape& aShape, 
                          const TopoDS_Shape& aComponent) const
{
  // From now the Component cannot be edited
  aComponent.TShape()->Free(Standard_False);

  // Note that freezing aComponent before testing if aShape is free
  // prevents from self-insertion
  // but aShape will be frozen when the Exception is raised
  if (aShape.Free())
  {
    static const unsigned int aTb[9]=
    {
      //COMPOUND to:
      (1<<((unsigned int)TopAbs_COMPOUND)),
      //COMPSOLID to:
      (1<<((unsigned int)TopAbs_COMPOUND)),
      //SOLID to:
      (1<<((unsigned int)TopAbs_COMPOUND)) |
      (1<<((unsigned int)TopAbs_COMPSOLID)),
      //SHELL to:
      (1<<((unsigned int)TopAbs_COMPOUND)) |
      (1<<((unsigned int)TopAbs_SOLID)),
      //FACE to:
      (1<<((unsigned int)TopAbs_COMPOUND)) |
      (1<<((unsigned int)TopAbs_SHELL)),
      //WIRE to:
      (1<<((unsigned int)TopAbs_COMPOUND)) |
      (1<<((unsigned int)TopAbs_FACE)),
      //EDGE to:
      (1<<((unsigned int)TopAbs_COMPOUND)) |
      (1<<((unsigned int)TopAbs_SOLID)) |
      (1<<((unsigned int)TopAbs_WIRE)),
      //VERTEX to:
      (1<<((unsigned int)TopAbs_COMPOUND)) |
      (1<<((unsigned int)TopAbs_SOLID)) |
      (1<<((unsigned int)TopAbs_FACE)) |
      (1<<((unsigned int)TopAbs_EDGE)),
      //SHAPE to:
      0
    };
    //
    const unsigned int iC=(unsigned int)aComponent.ShapeType();
    const unsigned int iS=(unsigned int)aShape.ShapeType();
    //
    if ((aTb[iC] & (1<<iS)) != 0) {
      TopoDS_ListOfShape& L = aShape.TShape()->ChangeShapes();
      L.Append(aComponent);
      TopoDS_Shape& S = L.Last();
      //
      // compute the relative Orientation
      if (aShape.Orientation() == TopAbs_REVERSED)
        S.Reverse();
      //
      // and the Relative Location
      const TopLoc_Location& aLoc=aShape.Location();
      if (!aLoc.IsIdentity())
        S.Move(aLoc.Inverted());
      //
      // Set the TShape as modified.
      aShape.TShape()->Modified(Standard_True);
    }
    else {
      throw TopoDS_UnCompatibleShapes("TopoDS_Builder::Add");
    }
  }
  else {
    throw TopoDS_FrozenShape("TopoDS_Buider::Add");
  }
}


//=======================================================================
//function : Remove
//purpose  : Remove a Shape from an other one
//=======================================================================

void TopoDS_Builder::Remove (TopoDS_Shape& aShape, 
                             const TopoDS_Shape& aComponent) const
{
  // check  if aShape  is  not Frozen
  TopoDS_FrozenShape_Raise_if (!aShape.Free(),"TopoDS_Builder::Remove");
  
  // compute the relative Orientation and Location of aComponent
  TopoDS_Shape S = aComponent;
  if (aShape.Orientation() == TopAbs_REVERSED)
    S.Reverse();
  S.Location(S.Location().Predivided(aShape.Location()));

  TopoDS_ListOfShape& L = aShape.TShape()->ChangeShapes();
  TopoDS_ListIteratorOfListOfShape It(L);
  while (It.More()) {
    if (It.Value() == S) {
      L.Remove(It);
      aShape.TShape()->Modified(Standard_True);
      break;
    }
    It.Next();
  }
}
