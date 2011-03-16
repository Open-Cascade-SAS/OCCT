// File:	MXCAFDoc_ColorToolRetrievalDriver.cxx
// Created:	Tue Aug 15 15:37:48 2000
// Author:	data exchange team
//		<det@strelox.nnov.matra-dtv.fr>


#include <MDF_RRelocationTable.hxx>
#include <MXCAFDoc_ColorToolRetrievalDriver.ixx>
#include <MgtTopLoc.hxx>
#include <PTColStd_PersistentTransientMap.hxx>
#include <PXCAFDoc_ColorTool.hxx>
#include <XCAFDoc_ColorTool.hxx>


//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

MXCAFDoc_ColorToolRetrievalDriver::MXCAFDoc_ColorToolRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver) : MDF_ARDriver (theMsgDriver)
{
}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

 Standard_Integer MXCAFDoc_ColorToolRetrievalDriver::VersionNumber() const
{
  return 0;
}

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

 Handle(Standard_Type) MXCAFDoc_ColorToolRetrievalDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(PXCAFDoc_ColorTool);
  return sourceType;
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

 Handle(TDF_Attribute) MXCAFDoc_ColorToolRetrievalDriver::NewEmpty() const
{
  return new XCAFDoc_ColorTool();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MXCAFDoc_ColorToolRetrievalDriver::Paste (const Handle(PDF_Attribute)& /* Source */,
					       const Handle(TDF_Attribute)& /* Target */,
					       const Handle(MDF_RRelocationTable)& /*RelocTable*/) const
{
}

