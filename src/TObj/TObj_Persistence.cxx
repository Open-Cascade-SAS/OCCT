// Created on: 2004-11-23
// Created by: Andrey BETENEV
// Copyright (c) 2004-2014 OPEN CASCADE SAS
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
