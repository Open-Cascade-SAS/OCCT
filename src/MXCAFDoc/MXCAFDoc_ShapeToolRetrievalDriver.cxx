// File:	MXCAFDoc_ShapeToolRetrievalDriver.cxx
// Created:	Tue Aug 15 15:37:48 2000
// Author:	data exchange team
//		<det@strelox.nnov.matra-dtv.fr>


#include <MDF_RRelocationTable.hxx>
#include <MXCAFDoc_ShapeToolRetrievalDriver.ixx>
#include <MgtTopLoc.hxx>
#include <PTColStd_PersistentTransientMap.hxx>
#include <PXCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>


//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

MXCAFDoc_ShapeToolRetrievalDriver::MXCAFDoc_ShapeToolRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver) : MDF_ARDriver (theMsgDriver)
{
}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

 Standard_Integer MXCAFDoc_ShapeToolRetrievalDriver::VersionNumber() const
{
  return 0;
}

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

 Handle(Standard_Type) MXCAFDoc_ShapeToolRetrievalDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(PXCAFDoc_ShapeTool);
  return sourceType;
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

 Handle(TDF_Attribute) MXCAFDoc_ShapeToolRetrievalDriver::NewEmpty() const
{
  return new XCAFDoc_ShapeTool();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MXCAFDoc_ShapeToolRetrievalDriver::Paste (const Handle(PDF_Attribute)& /* Source */,
					       const Handle(TDF_Attribute)& /* Target*/,
					       const Handle(MDF_RRelocationTable)& /*RelocTable*/) const
{
}

