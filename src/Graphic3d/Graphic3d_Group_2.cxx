// Created by: NW,JPB,CAL
// Copyright (c) 1991-1999 Matra Datavision
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




//-Version  

//-Design Declaration of variables specific to groups
//    of primitives

//-Warning  A group is defined in a structure
// This is the smallest editable entity

//-References 

//-Language C++ 2.0

//-Declarations

// for the class
#include <Graphic3d_Group.jxx>
#include <Graphic3d_Group.pxx>

#include <Graphic3d_StructureManager.hxx>

// Structures are necessairy for interfacing with routines c
// Routines C should be declared externally
//-Methods, in order

Standard_Boolean Graphic3d_Group::IsDeleted () const {

  return ( (MyCGroup.ptrGroup == NULL) || (MyStructure->IsDeleted ()) );

}

Standard_Boolean Graphic3d_Group::ContainsFacet () const {

  return (MyContainsFacet);

}

Standard_Boolean Graphic3d_Group::IsEmpty () const {

  if (IsDeleted ()) return (Standard_True);

  Standard_ShortReal RL = ShortRealLast ();
  Standard_ShortReal RF = ShortRealFirst ();
  Standard_Boolean Result = ((MyBounds.XMin == RL) && (MyBounds.YMin == RL) &&
    (MyBounds.ZMin == RL) && (MyBounds.XMax == RF) &&
    (MyBounds.YMax == RF) && (MyBounds.ZMax == RF));

  if (Result != MyIsEmpty)
    cout << "MyIsEmpty != IsEmpty ()\n" << flush;

  return (Result);

}

void Graphic3d_Group::SetMinMaxValues (const Standard_Real XMin, const Standard_Real YMin, const Standard_Real ZMin, const Standard_Real XMax, const Standard_Real YMax, const Standard_Real ZMax) {

  MyBounds.XMin = Standard_ShortReal (XMin);
  MyBounds.YMin = Standard_ShortReal (YMin);
  MyBounds.ZMin = Standard_ShortReal (ZMin);
  MyBounds.XMax = Standard_ShortReal (XMax);
  MyBounds.YMax = Standard_ShortReal (YMax);
  MyBounds.ZMax = Standard_ShortReal (ZMax);

}

void Graphic3d_Group::MinMaxValues (Standard_Real& XMin, Standard_Real& YMin, Standard_Real& ZMin, Standard_Real& XMax, Standard_Real& YMax, Standard_Real& ZMax) const {

  MinMaxCoord (XMin, YMin, ZMin, XMax, YMax, ZMax);

}

Handle(Graphic3d_Structure) Graphic3d_Group::Structure () const {

  return MyStructure;

}

void Graphic3d_Group::MinMaxCoord (Standard_Real& XMin, Standard_Real& YMin, Standard_Real& ZMin, Standard_Real& XMax, Standard_Real& YMax, Standard_Real& ZMax) const {

  if (IsEmpty ()) {
    // Empty Group
    XMin  = YMin  = ZMin  = ShortRealFirst ();
    XMax  = YMax  = ZMax  = ShortRealLast ();
  }
  else {
    XMin  = Standard_Real (MyBounds.XMin);
    YMin  = Standard_Real (MyBounds.YMin);
    ZMin  = Standard_Real (MyBounds.ZMin);
    XMax  = Standard_Real (MyBounds.XMax);
    YMax  = Standard_Real (MyBounds.YMax);
    ZMax  = Standard_Real (MyBounds.ZMax);
  }

}

void Graphic3d_Group::Update () const {

  if (IsDeleted ()) return;

  if ( (MyStructure->StructureManager ())->UpdateMode () == Aspect_TOU_ASAP )
    (MyStructure->StructureManager ())->Update ();

}
