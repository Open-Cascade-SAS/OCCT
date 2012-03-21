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

#include <BRepOffsetAPI_MakeDraft.ixx>


BRepOffsetAPI_MakeDraft::BRepOffsetAPI_MakeDraft(const TopoDS_Shape& Shape,
				     const gp_Dir& Dir,
				     const Standard_Real Angle)
                               : myDraft( Shape, Dir, Angle)
{
  NotDone();
}

 void BRepOffsetAPI_MakeDraft::SetOptions(const BRepBuilderAPI_TransitionMode Style,
				    const Standard_Real AngleMin, 
				    const Standard_Real AngleMax)
{
  BRepFill_TransitionStyle style =  BRepFill_Right;
  if (Style == BRepBuilderAPI_RoundCorner) style =  BRepFill_Round;
  myDraft.SetOptions( style, AngleMin,  AngleMax);
}

 void BRepOffsetAPI_MakeDraft::SetDraft(const Standard_Boolean IsInternal)
{
   myDraft.SetDraft(IsInternal);
}

 void BRepOffsetAPI_MakeDraft::Perform(const Standard_Real LengthMax) 
{
  myDraft.Perform( LengthMax);
  if (myDraft.IsDone()) {
    Done();
    myShape = myDraft.Shape();
  }
}

 void BRepOffsetAPI_MakeDraft::Perform(const Handle(Geom_Surface)& Surface,
				 const Standard_Boolean KeepInsideSurface) 
{
  myDraft.Perform(Surface, KeepInsideSurface);
  if (myDraft.IsDone()) {
    Done();
    myShape = myDraft.Shape();
  }  
}

 void BRepOffsetAPI_MakeDraft::Perform(const TopoDS_Shape& StopShape,
				 const Standard_Boolean KeepOutSide) 
{
  myDraft.Perform( StopShape, KeepOutSide);
  if (myDraft.IsDone()) {
    Done();
    myShape = myDraft.Shape();
  } 
}

 TopoDS_Shell BRepOffsetAPI_MakeDraft::Shell() const
{
  return myDraft.Shell();
}

const TopTools_ListOfShape& BRepOffsetAPI_MakeDraft::Generated(const TopoDS_Shape& S) 
{
  return myDraft.Generated( S );
}

