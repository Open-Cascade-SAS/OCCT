// Created by: DAUTRY Philippe
// Copyright (c) 1997-1999 Matra Datavision
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

//      	----------------------

// Version:	0.0
//Version	Date		Purpose
//		0.0	Sep  8 1997	Creation



#include <TDF_AttributeDelta.ixx>

#include <Standard_GUID.hxx>

#include <TDF_Label.hxx>
#include <TDF_Tool.hxx>

#include <TCollection_AsciiString.hxx>

//=======================================================================
//function : TDF_AttributeDelta
//purpose  : 
//=======================================================================

TDF_AttributeDelta::TDF_AttributeDelta
(const Handle(TDF_Attribute)& anAttribute)
: myAttribute(anAttribute),
  myLabel(anAttribute->Label())
{}


//=======================================================================
//function : Label
//purpose  : 
//=======================================================================

TDF_Label TDF_AttributeDelta::Label() const
{ return myLabel; }


//=======================================================================
//function : Attribute
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) TDF_AttributeDelta::Attribute() const
{ return myAttribute; }


//=======================================================================
//function : ID
//purpose  : 
//=======================================================================

Standard_GUID TDF_AttributeDelta::ID() const
{ return myAttribute->ID(); }


//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

Standard_OStream& TDF_AttributeDelta::Dump(Standard_OStream& OS) const
{
  static TCollection_AsciiString entry;
  TDF_Tool::Entry(Label(),entry);
  OS<<this->DynamicType()->Name()<<" at "<<entry;
  OS<<" on "<<Attribute()->DynamicType()->Name();
  return OS;
}
