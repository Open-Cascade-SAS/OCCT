// File:      MXCAFDoc_DimTolToolStorageDriver.cxx
// Created:   10.12.08 09:08:37
// Author:    Pavel TELKOV
// Copyright: Open CASCADE 2008

#include <MDF_SRelocationTable.hxx>
#include <MXCAFDoc_DimTolToolStorageDriver.ixx>
#include <PXCAFDoc_DimTolTool.hxx>
#include <XCAFDoc_DimTolTool.hxx>

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

MXCAFDoc_DimTolToolStorageDriver::MXCAFDoc_DimTolToolStorageDriver
  (const Handle(CDM_MessageDriver)& theMsgDriver) : MDF_ASDriver (theMsgDriver)
{
}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MXCAFDoc_DimTolToolStorageDriver::VersionNumber() const
{  return 0; }

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

 Handle(Standard_Type) MXCAFDoc_DimTolToolStorageDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(XCAFDoc_DimTolTool);
  return sourceType;
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

 Handle(PDF_Attribute) MXCAFDoc_DimTolToolStorageDriver::NewEmpty() const
{
  return new PXCAFDoc_DimTolTool();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MXCAFDoc_DimTolToolStorageDriver::Paste 
  (const Handle(TDF_Attribute)& /* Source */,
   const Handle(PDF_Attribute)& /* Target */,
   const Handle(MDF_SRelocationTable)& /*RelocTable*/) const
{
}
