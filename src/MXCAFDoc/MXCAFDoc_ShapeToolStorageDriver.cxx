// File:	MXCAFDoc_ShapeToolStorageDriver.cxx
// Created:	Tue Aug 15 15:32:19 2000
// Author:	data exchange team
//		<det@strelox.nnov.matra-dtv.fr>


#include <MDF_SRelocationTable.hxx>
#include <MXCAFDoc_ShapeToolStorageDriver.ixx>
#include <MgtTopLoc.hxx>
#include <PTColStd_TransientPersistentMap.hxx>
#include <PXCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

MXCAFDoc_ShapeToolStorageDriver::MXCAFDoc_ShapeToolStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver) : MDF_ASDriver (theMsgDriver)
{
}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

 Standard_Integer MXCAFDoc_ShapeToolStorageDriver::VersionNumber() const
{  return 0; }

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

 Handle(Standard_Type) MXCAFDoc_ShapeToolStorageDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(XCAFDoc_ShapeTool);
  return sourceType;
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

 Handle(PDF_Attribute) MXCAFDoc_ShapeToolStorageDriver::NewEmpty() const
{
  return new PXCAFDoc_ShapeTool();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MXCAFDoc_ShapeToolStorageDriver::Paste (const Handle(TDF_Attribute)& /* Source */,
					     const Handle(PDF_Attribute)& /* Target */,
					     const Handle(MDF_SRelocationTable)& /*RelocTable*/) const
{
}

