// File      : BinTObjDrivers_IntSparseArrayDriver.cxx
// Created   : 30.03.2007
// Author    : Michael SAZONOV
// Copyright:   Open CASCADE  2007
// The original implementation Copyright: (C) RINA S.p.A

#include <BinTObjDrivers_IntSparseArrayDriver.hxx>
#include <CDM_MessageDriver.hxx>
#include <TDF_Attribute.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <TObj_TIntSparseArray.hxx>
#include <TObj_Assistant.hxx>
#include <TCollection_AsciiString.hxx>
#include <TDF_Tool.hxx>

IMPLEMENT_STANDARD_HANDLE(BinTObjDrivers_IntSparseArrayDriver,BinMDF_ADriver)
IMPLEMENT_STANDARD_RTTIEXT(BinTObjDrivers_IntSparseArrayDriver,BinMDF_ADriver)

//=======================================================================
//function : BinTObjDrivers_IntSparseArrayDriver
//purpose  : constructor
//=======================================================================

BinTObjDrivers_IntSparseArrayDriver::BinTObjDrivers_IntSparseArrayDriver
                         (const Handle(CDM_MessageDriver)& theMessageDriver)
: BinMDF_ADriver( theMessageDriver, NULL)
{
}

//=======================================================================
//function : NewEmpty
//purpose  : Creates a new attribute
//=======================================================================

Handle(TDF_Attribute) BinTObjDrivers_IntSparseArrayDriver::NewEmpty() const
{
  return new TObj_TIntSparseArray;
}

//=======================================================================
//function : Paste
//purpose  : Retrieve. Translate the contents of <theSource> and put it
//           into <theTarget>.
//=======================================================================

Standard_Boolean BinTObjDrivers_IntSparseArrayDriver::Paste
                         (const BinObjMgt_Persistent&  theSource,
                          const Handle(TDF_Attribute)& theTarget,
                          BinObjMgt_RRelocationTable&) const
{
  Handle(TObj_TIntSparseArray) aTarget =
    Handle(TObj_TIntSparseArray)::DownCast(theTarget);

  // get pairs (ID, value) while ID != 0
  Standard_Integer anId;
  if (!(theSource >> anId) || anId < 0)
    return Standard_False;
  while (anId)
  {
    Standard_Integer aValue;
    if (!(theSource >> aValue) || aValue <= 0)
      return Standard_False;

    // store the value in the target array
    aTarget->SetDoBackup (Standard_False);
    aTarget->SetValue (anId, aValue);
    aTarget->SetDoBackup (Standard_True);

    // get next ID
    if (!(theSource >> anId) || anId < 0)
      return Standard_False;
  }
  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : Store. Translate the contents of <theSource> and put it
//           into <theTarget>
//=======================================================================

void BinTObjDrivers_IntSparseArrayDriver::Paste
                         (const Handle(TDF_Attribute)& theSource,
                          BinObjMgt_Persistent&        theTarget,
                          BinObjMgt_SRelocationTable&) const
{
  Handle(TObj_TIntSparseArray) aSource =
    Handle(TObj_TIntSparseArray)::DownCast (theSource);

  // put only non-null values as pairs (ID, value)
  // terminate the list by ID=0
  TObj_TIntSparseArray::Iterator anIt = aSource->GetIterator();
  for ( ; anIt.More() ; anIt.Next() )
  {
    Standard_Integer aValue = anIt.Value();
    if (aValue == 0)
      continue;
    
    // store ID and value
    theTarget << (Standard_Integer) anIt.Index() << aValue;
  }
  // zero indicates end of the entities
  theTarget << (Standard_Integer) 0;
}
