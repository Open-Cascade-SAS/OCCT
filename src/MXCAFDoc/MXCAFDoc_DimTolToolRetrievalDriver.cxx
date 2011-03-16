// File:      MXCAFDoc_DimTolToolRetrievalDriver.cxx
// Created:   10.12.08 09:12:17
// Author:    Pavel TELKOV
// Copyright: Open CASCADE 2008

#include <MDF_RRelocationTable.hxx>
#include <MXCAFDoc_DimTolToolRetrievalDriver.ixx>
#include <PXCAFDoc_DimTolTool.hxx>
#include <XCAFDoc_DimTolTool.hxx>


//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

MXCAFDoc_DimTolToolRetrievalDriver::MXCAFDoc_DimTolToolRetrievalDriver
  (const Handle(CDM_MessageDriver)& theMsgDriver) : MDF_ARDriver (theMsgDriver)
{
}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MXCAFDoc_DimTolToolRetrievalDriver::VersionNumber() const
{
  return 0;
}

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

Handle(Standard_Type) MXCAFDoc_DimTolToolRetrievalDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(PXCAFDoc_DimTolTool);
  return sourceType;
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) MXCAFDoc_DimTolToolRetrievalDriver::NewEmpty() const
{
  return new XCAFDoc_DimTolTool();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MXCAFDoc_DimTolToolRetrievalDriver::Paste 
  (const Handle(PDF_Attribute)& /* Source */,
   const Handle(TDF_Attribute)& /* Target */,
   const Handle(MDF_RRelocationTable)& /*RelocTable*/) const
{
}
