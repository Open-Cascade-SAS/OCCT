// Created on: 2004-11-23
// Created by: Andrey BETENEV
// Copyright (c) 2004-2012 OPEN CASCADE SAS
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

// The original implementation Copyright: (C) RINA S.p.A

#include <TObj_Persistence.hxx>
#include <TObj_Object.hxx>

//=======================================================================
//function : getMapOfTypes
//purpose  : Returns the map of types
//=======================================================================

TObj_DataMapOfStringPointer& TObj_Persistence::getMapOfTypes ()
{
  static TObj_DataMapOfStringPointer myMapOfTypes;
  return myMapOfTypes;
}

//=======================================================================
//function : Constructor
//purpose  : Register the type for persistence
//=======================================================================

TObj_Persistence::TObj_Persistence (const Standard_CString theType)
{
  myType = theType;
  getMapOfTypes().Bind ( theType, this );
}

//=======================================================================
//function : Destructor
//purpose  : Unregister the type
//=======================================================================

TObj_Persistence::~TObj_Persistence ()
{
  getMapOfTypes().UnBind ( myType );
}

//=======================================================================
//function : CreateNewObject
//purpose  :
//=======================================================================

Handle(TObj_Object) TObj_Persistence::CreateNewObject (const Standard_CString theType,
                                                               const TDF_Label& theLabel)
{
  if ( getMapOfTypes().IsBound ( theType ) )
  {
    TObj_Persistence *tool =
      (TObj_Persistence*) getMapOfTypes().Find ( theType );
    if ( tool ) return tool->New (theLabel);
  }
  return 0;
}

//=======================================================================
//function : DumpTypes
//purpose  :
//=======================================================================

void TObj_Persistence::DumpTypes (Standard_OStream &theOs)
{
  TObj_DataMapOfStringPointer::Iterator it ( getMapOfTypes() );
  for ( ; it.More(); it.Next() )
  {
    theOs << it.Key() << endl;
  }
}
