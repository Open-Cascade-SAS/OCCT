// File:      BinMXCAFDoc_DimTolToolDriver.cxx
// Created:   10.12.08 13:47:58
// Author:    Pavel TELKOV
// Copyright: Open CASCADE 2008

#include <BinMXCAFDoc_DimTolToolDriver.ixx>
#include <XCAFDoc_DimTolTool.hxx>

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
BinMXCAFDoc_DimTolToolDriver::BinMXCAFDoc_DimTolToolDriver
  (const Handle(CDM_MessageDriver)& theMsgDriver)
: BinMDF_ADriver(theMsgDriver, STANDARD_TYPE(XCAFDoc_DimTolTool)->Name())
{
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) BinMXCAFDoc_DimTolToolDriver::NewEmpty() const
{
  return new XCAFDoc_DimTolTool();
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
Standard_Boolean BinMXCAFDoc_DimTolToolDriver::Paste
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
void BinMXCAFDoc_DimTolToolDriver::Paste
  (const Handle(TDF_Attribute)& /*theSource*/,
   BinObjMgt_Persistent& /*theTarget*/,
   BinObjMgt_SRelocationTable& /*theRelocTable*/) const {
}
