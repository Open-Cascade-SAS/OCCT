// File:        TObj_Persistence.cxx
// Created:     Tue Nov 23 11:36:45 2004
// Author:      Andrey BETENEV
// Copyright:   Open CASCADE  2007
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
