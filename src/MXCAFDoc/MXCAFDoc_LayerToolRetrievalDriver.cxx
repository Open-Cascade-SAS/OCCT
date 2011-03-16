// File:	MXCAFDoc_LayerToolRetrievalDriver.cxx
// Created:	Tue Aug 15 15:37:48 2000
// Author:	data exchange team
//		<det@strelox.nnov.matra-dtv.fr>


#include <MDF_RRelocationTable.hxx>
#include <MXCAFDoc_LayerToolRetrievalDriver.ixx>
#include <PXCAFDoc_LayerTool.hxx>
#include <XCAFDoc_LayerTool.hxx>


//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

MXCAFDoc_LayerToolRetrievalDriver::MXCAFDoc_LayerToolRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver) : MDF_ARDriver (theMsgDriver)
{
}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

 Standard_Integer MXCAFDoc_LayerToolRetrievalDriver::VersionNumber() const
{
  return 0;
}

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

 Handle(Standard_Type) MXCAFDoc_LayerToolRetrievalDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(PXCAFDoc_LayerTool);
  return sourceType;
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

 Handle(TDF_Attribute) MXCAFDoc_LayerToolRetrievalDriver::NewEmpty() const
{
  return new XCAFDoc_LayerTool();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MXCAFDoc_LayerToolRetrievalDriver::Paste (const Handle(PDF_Attribute)& /* Source */,
					       const Handle(TDF_Attribute)& /* Target */,
					       const Handle(MDF_RRelocationTable)& /*RelocTable*/) const
{
}

