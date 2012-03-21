// Created on: 1993-01-21
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


#define No_Standard_NoSuchObject

#include <TopoDS_Iterator.ixx>


//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================

void TopoDS_Iterator::Initialize(const TopoDS_Shape& S,
                                 const Standard_Boolean cumOri,
                                 const Standard_Boolean cumLoc)
{
  if (cumLoc)
    myLocation = S.Location();
  else
    myLocation.Identity();
  if (cumOri)
    myOrientation = S.Orientation();
  else
    myOrientation = TopAbs_FORWARD;
  myShapes.Initialize(S.TShape()->Shapes());
  if (More()) {
    myShape = myShapes.Value();
    myShape.Orientation(TopAbs::Compose(myOrientation,myShape.Orientation()));
    //modified by NIZNHY-PKV Fri Jan 16 07:42:30 2009f
    if (!myLocation.IsIdentity())
    //modified by NIZNHY-PKV Fri Jan 16 07:42:37 2009t
      myShape.Move(myLocation);
  }
}

//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

void TopoDS_Iterator::Next()
{
  myShapes.Next();
  if (More()) {
    myShape = myShapes.Value();
    myShape.Orientation(TopAbs::Compose(myOrientation,myShape.Orientation()));
    //modified by NIZNHY-PKV Fri Jan 16 07:42:30 2009f
    if (!myLocation.IsIdentity())
    //modified by NIZNHY-PKV Fri Jan 16 07:42:37 2009t
      myShape.Move(myLocation);
  }
}
