// File:	MXCAFDoc_LocationStorageDriver.cxx
// Created:	Tue Aug 15 15:32:19 2000
// Author:	data exchange team
//		<det@strelox.nnov.matra-dtv.fr>


#include <MDF_SRelocationTable.hxx>
#include <MXCAFDoc_LocationStorageDriver.ixx>
#include <MgtTopLoc.hxx>
#include <PTColStd_TransientPersistentMap.hxx>
#include <PXCAFDoc_Location.hxx>
#include <XCAFDoc_Location.hxx>

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

MXCAFDoc_LocationStorageDriver::MXCAFDoc_LocationStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver) : MDF_ASDriver (theMsgDriver)
{
}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

 Standard_Integer MXCAFDoc_LocationStorageDriver::VersionNumber() const
{  return 0; } 

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

 Handle(Standard_Type) MXCAFDoc_LocationStorageDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(XCAFDoc_Location);
  return sourceType;
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

 Handle(PDF_Attribute) MXCAFDoc_LocationStorageDriver::NewEmpty() const
{
  return new PXCAFDoc_Location();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

 void MXCAFDoc_LocationStorageDriver::Paste(const Handle(TDF_Attribute)& Source,
						  const Handle(PDF_Attribute)& Target,
						  const Handle(MDF_SRelocationTable)& RelocTable) const
{
  Handle(XCAFDoc_Location) S = Handle(XCAFDoc_Location)::DownCast (Source);
  Handle(PXCAFDoc_Location) T = Handle(PXCAFDoc_Location)::DownCast (Target);
  
  PTColStd_TransientPersistentMap& TPMap = RelocTable->OtherTable();
  T->Set(MgtTopLoc::Translate(S->Get(), TPMap));
}

