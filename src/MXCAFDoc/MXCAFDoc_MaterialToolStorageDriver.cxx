// File:      MXCAFDoc_MaterialToolStorageDriver.cxx
// Created:   10.12.08 09:08:37
// Author:    Pavel TELKOV
// Copyright: Open CASCADE 2008

#include <MDF_SRelocationTable.hxx>
#include <MXCAFDoc_MaterialToolStorageDriver.ixx>
#include <PXCAFDoc_MaterialTool.hxx>
#include <XCAFDoc_MaterialTool.hxx>

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

MXCAFDoc_MaterialToolStorageDriver::MXCAFDoc_MaterialToolStorageDriver
  (const Handle(CDM_MessageDriver)& theMsgDriver) : MDF_ASDriver (theMsgDriver)
{
}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MXCAFDoc_MaterialToolStorageDriver::VersionNumber() const
{  return 0; }

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

Handle(Standard_Type) MXCAFDoc_MaterialToolStorageDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(XCAFDoc_MaterialTool);
  return sourceType;
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(PDF_Attribute) MXCAFDoc_MaterialToolStorageDriver::NewEmpty() const
{
  return new PXCAFDoc_MaterialTool();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MXCAFDoc_MaterialToolStorageDriver::Paste 
  (const Handle(TDF_Attribute)& /* Source */,
   const Handle(PDF_Attribute)& /* Target */,
   const Handle(MDF_SRelocationTable)& /*RelocTable*/) const
{
}
