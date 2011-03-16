// File:      BinMXCAFDoc_MaterialToolDriver.cxx
// Created:   10.12.08 13:47:58
// Author:    Pavel TELKOV
// Copyright: Open CASCADE 2008

#include <BinMXCAFDoc_MaterialToolDriver.ixx>
#include <XCAFDoc_MaterialTool.hxx>

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
BinMXCAFDoc_MaterialToolDriver::BinMXCAFDoc_MaterialToolDriver
  (const Handle(CDM_MessageDriver)& theMsgDriver)
: BinMDF_ADriver(theMsgDriver, STANDARD_TYPE(XCAFDoc_MaterialTool)->Name())
{
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) BinMXCAFDoc_MaterialToolDriver::NewEmpty() const
{
  return new XCAFDoc_MaterialTool();
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
Standard_Boolean BinMXCAFDoc_MaterialToolDriver::Paste
  (const BinObjMgt_Persistent& /*theSource*/,
   const Handle(TDF_Attribute)& /*theTarget*/,
   BinObjMgt_RRelocationTable& /*theRelocTable*/) const 
{
  return Standard_True;
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
void BinMXCAFDoc_MaterialToolDriver::Paste
  (const Handle(TDF_Attribute)& /*theSource*/,
   BinObjMgt_Persistent& /*theTarget*/,
   BinObjMgt_SRelocationTable& /*theRelocTable*/) const {
}
