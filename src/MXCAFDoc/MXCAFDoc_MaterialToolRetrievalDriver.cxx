// File:      MXCAFDoc_MaterialToolRetrievalDriver.cxx
// Created:   10.12.08 09:12:17
// Author:    Pavel TELKOV
// Copyright: Open CASCADE 2008

#include <MDF_RRelocationTable.hxx>
#include <MXCAFDoc_MaterialToolRetrievalDriver.ixx>
#include <PXCAFDoc_MaterialTool.hxx>
#include <XCAFDoc_MaterialTool.hxx>


//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

MXCAFDoc_MaterialToolRetrievalDriver::MXCAFDoc_MaterialToolRetrievalDriver
  (const Handle(CDM_MessageDriver)& theMsgDriver) : MDF_ARDriver (theMsgDriver)
{
}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MXCAFDoc_MaterialToolRetrievalDriver::VersionNumber() const
{
  return 0;
}

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

Handle(Standard_Type) MXCAFDoc_MaterialToolRetrievalDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(PXCAFDoc_MaterialTool);
  return sourceType;
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) MXCAFDoc_MaterialToolRetrievalDriver::NewEmpty() const
{
  return new XCAFDoc_MaterialTool();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MXCAFDoc_MaterialToolRetrievalDriver::Paste 
  (const Handle(PDF_Attribute)& /* Source */,
   const Handle(TDF_Attribute)& /* Target */,
   const Handle(MDF_RRelocationTable)& /*RelocTable*/) const
{
}
