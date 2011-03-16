// File      : BinTObjDrivers_ObjectDriver.cxx
// Created   : Wed Nov 24 11:28:22 2004
// Author    : Edward AGAPOV
// Copyright:   Open CASCADE  2007
// The original implementation Copyright: (C) RINA S.p.A


#include <CDM_MessageDriver.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <TDF_Tool.hxx>

#include <BinTObjDrivers_ObjectDriver.hxx>
#include <TObj_Assistant.hxx>
#include <TObj_TObject.hxx>
#include <TObj_Persistence.hxx>

IMPLEMENT_STANDARD_HANDLE(BinTObjDrivers_ObjectDriver,BinMDF_ADriver)
IMPLEMENT_STANDARD_RTTIEXT(BinTObjDrivers_ObjectDriver,BinMDF_ADriver);

//=======================================================================
//function : BinTObjDrivers_ObjectDriver
//purpose  : constructor
//=======================================================================

BinTObjDrivers_ObjectDriver::BinTObjDrivers_ObjectDriver
                         (const Handle(CDM_MessageDriver)& theMessageDriver)
: BinMDF_ADriver( theMessageDriver, NULL)
{
}

//=======================================================================
//function : NewEmpty
//purpose  : Creates a new attribute
//=======================================================================

Handle(TDF_Attribute) BinTObjDrivers_ObjectDriver::NewEmpty() const
{
  return new TObj_TObject;
}

//=======================================================================
//function : Paste
//purpose  : Translate the contents of <theSource> and put it
//           into <theTarget>.
//=======================================================================

Standard_Boolean BinTObjDrivers_ObjectDriver::Paste
                         (const BinObjMgt_Persistent&  theSource,
                          const Handle(TDF_Attribute)& theTarget,
                          BinObjMgt_RRelocationTable&) const
{
  Standard_Integer aSavedPos = theSource.Position();

  // first try to get the type as an integer ID
  Standard_Integer anID;
  if (! (theSource >> anID)) return Standard_False;
  Handle(TObj_Object) anObject;
  if ( (unsigned)anID > 0xffff)
  {
    // if we are here it means that the type was stored as an ascii string,
    // so rewind theSource and reget
    theSource.SetPosition(aSavedPos);
    TCollection_AsciiString aName;
    if (! (theSource >> aName)) return Standard_False;
    anObject =
      TObj_Persistence::CreateNewObject(aName.ToCString(),theTarget->Label());
    if (anObject.IsNull())
    {
      TCollection_AsciiString anEntry;
      TDF_Tool::Entry (theTarget->Label(), anEntry);
      WriteMessage (TCollection_ExtendedString
                    ("TObj_TObject retrieval: wrong object type name ") +
                    aName + ", entry " + anEntry);
      TObj_Assistant::BindType(0);
      return Standard_False;
    }
    // register the type
    TObj_Assistant::BindType(anObject->DynamicType());
  }
  else 
  {
    // use anID to get the type from earlier registered ones
    Handle(Standard_Type) aType = TObj_Assistant::FindType(anID);
    if(!aType.IsNull())
      anObject =
        TObj_Persistence::CreateNewObject(aType->Name(), theTarget->Label());
    else 
    {
      return Standard_False;
    }
  }
  Handle(TObj_TObject)::DownCast (theTarget) ->Set( anObject );
  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : Translate the contents of <theSource> and put it
//           into <theTarget>.
//           anObject is stored as a Name of class derived from TObj_Object
//=======================================================================

void BinTObjDrivers_ObjectDriver::Paste
                         (const Handle(TDF_Attribute)& theSource,
                          BinObjMgt_Persistent&        theTarget,
                          BinObjMgt_SRelocationTable&) const
{
  Handle(TObj_TObject) aTObj =
    Handle(TObj_TObject)::DownCast( theSource );
  Handle(TObj_Object) anIObject = aTObj->Get();
  if (anIObject.IsNull()) return;

  Handle(Standard_Type) aType = anIObject->DynamicType();

  Standard_Integer anID = TObj_Assistant::FindTypeIndex(anIObject->DynamicType());

  if(anID == 0) 
  {
    // we first meet this type;
    // register a type and store a type name as a string
    TObj_Assistant::BindType(aType);
    TCollection_AsciiString aName = aType->Name();
    theTarget << aName;
  }
  else 
  {
    // store the integer type ID
    theTarget << anID;
  }
}
