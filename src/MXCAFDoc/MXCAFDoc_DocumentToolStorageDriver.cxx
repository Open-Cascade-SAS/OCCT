// File:	MXCAFDoc_DocumentToolStorageDriver.cxx
// Created:	Tue Aug 15 15:32:19 2000
// Author:	data exchange team
//		<det@strelox.nnov.matra-dtv.fr>


#include <MDF_SRelocationTable.hxx>
#include <MXCAFDoc_DocumentToolStorageDriver.ixx>
#include <MgtTopLoc.hxx>
#include <PTColStd_TransientPersistentMap.hxx>
#include <PXCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_DocumentTool.hxx>

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

MXCAFDoc_DocumentToolStorageDriver::MXCAFDoc_DocumentToolStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver) : MDF_ASDriver (theMsgDriver)
{
}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

 Standard_Integer MXCAFDoc_DocumentToolStorageDriver::VersionNumber() const
{  return 0; }

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

 Handle(Standard_Type) MXCAFDoc_DocumentToolStorageDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(XCAFDoc_DocumentTool);
  return sourceType;
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

 Handle(PDF_Attribute) MXCAFDoc_DocumentToolStorageDriver::NewEmpty() const
{
  return new PXCAFDoc_DocumentTool();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MXCAFDoc_DocumentToolStorageDriver::Paste(const Handle(TDF_Attribute)& /* Source */,
					       const Handle(PDF_Attribute)& /* Target */,
					       const Handle(MDF_SRelocationTable)& /*RelocTable*/) const
{
}

