// File:	MXCAFDoc_LayerToolStorageDriver.cxx
// Created:	Tue Aug 15 15:32:19 2000
// Author:	data exchange team
//		<det@strelox.nnov.matra-dtv.fr>


#include <MDF_SRelocationTable.hxx>
#include <MXCAFDoc_LayerToolStorageDriver.ixx>
#include <PXCAFDoc_LayerTool.hxx>
#include <XCAFDoc_LayerTool.hxx>

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

MXCAFDoc_LayerToolStorageDriver::MXCAFDoc_LayerToolStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver) : MDF_ASDriver (theMsgDriver)
{
}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

 Standard_Integer MXCAFDoc_LayerToolStorageDriver::VersionNumber() const
{  return 0; }

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

 Handle(Standard_Type) MXCAFDoc_LayerToolStorageDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(XCAFDoc_LayerTool);
  return sourceType;
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

 Handle(PDF_Attribute) MXCAFDoc_LayerToolStorageDriver::NewEmpty() const
{
  return new PXCAFDoc_LayerTool();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MXCAFDoc_LayerToolStorageDriver::Paste (const Handle(TDF_Attribute)& /* Source */,
					     const Handle(PDF_Attribute)& /* Target */,
					     const Handle(MDF_SRelocationTable)& /*RelocTable*/) const
{
}

