// File:      BinMXCAFDoc_DatumDriver.cxx
// Created:   10.12.08 13:15:38
// Author:    Pavel TELKOV
// Copyright: Open CASCADE 2008

#include <BinMXCAFDoc_DatumDriver.ixx>
#include <XCAFDoc_Datum.hxx>

#include <TCollection_HAsciiString.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_HArray1OfReal.hxx>

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
BinMXCAFDoc_DatumDriver::BinMXCAFDoc_DatumDriver
  (const Handle(CDM_MessageDriver)& theMsgDriver)
: BinMDF_ADriver(theMsgDriver, STANDARD_TYPE(XCAFDoc_Datum)->Name())
{
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) BinMXCAFDoc_DatumDriver::NewEmpty() const
{
  return new XCAFDoc_Datum();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
Standard_Boolean BinMXCAFDoc_DatumDriver::Paste(const BinObjMgt_Persistent& theSource,
                                                 const Handle(TDF_Attribute)& theTarget,
                                                 BinObjMgt_RRelocationTable& /*theRelocTable*/) const 
{
  Handle(XCAFDoc_Datum) anAtt = Handle(XCAFDoc_Datum)::DownCast(theTarget);
  TCollection_AsciiString aName, aDescr, anId;
  if ( !(theSource >> aName >> aDescr >> anId) )
    return Standard_False;

  anAtt->Set(new TCollection_HAsciiString( aName ),
             new TCollection_HAsciiString( aDescr ),
             new TCollection_HAsciiString( anId ));
  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void BinMXCAFDoc_DatumDriver::Paste(const Handle(TDF_Attribute)& theSource,
                                     BinObjMgt_Persistent& theTarget,
                                     BinObjMgt_SRelocationTable& /*theRelocTable*/) const
{
  Handle(XCAFDoc_Datum) anAtt = Handle(XCAFDoc_Datum)::DownCast(theSource);
  if ( !anAtt->GetName().IsNull() )
    theTarget << anAtt->GetName()->String();
  else
    theTarget << TCollection_AsciiString("");

  if ( !anAtt->GetDescription().IsNull() )
    theTarget << anAtt->GetDescription()->String();
  else
    theTarget << TCollection_AsciiString("");

  if ( !anAtt->GetIdentification().IsNull() )
    theTarget << anAtt->GetIdentification()->String();
  else
    theTarget << TCollection_AsciiString("");
}
