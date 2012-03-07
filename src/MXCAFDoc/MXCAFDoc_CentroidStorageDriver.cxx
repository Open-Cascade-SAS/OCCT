// File:	MXCAFDoc_CentroidStorageDriver.cxx
// Created:	Mon Sep 11 15:23:00 2000
// Author:	data exchange team
//		<det@doomox>


#include <MXCAFDoc_CentroidStorageDriver.ixx>
#include <MDF_SRelocationTable.hxx>
#include <PTColStd_TransientPersistentMap.hxx>
#include <PXCAFDoc_Centroid.hxx>
#include <XCAFDoc_Centroid.hxx>

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

MXCAFDoc_CentroidStorageDriver::MXCAFDoc_CentroidStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver) : MDF_ASDriver (theMsgDriver)
{
}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

 Standard_Integer MXCAFDoc_CentroidStorageDriver::VersionNumber() const
{  return 0; } 

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

 Handle(Standard_Type) MXCAFDoc_CentroidStorageDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(XCAFDoc_Centroid);
  return sourceType;
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

 Handle(PDF_Attribute) MXCAFDoc_CentroidStorageDriver::NewEmpty() const
{
  return new PXCAFDoc_Centroid();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

 void MXCAFDoc_CentroidStorageDriver::Paste(const Handle(TDF_Attribute)& Source,
						  const Handle(PDF_Attribute)& Target,
						  const Handle(MDF_SRelocationTable)& RelocTable) const
{
  Handle(XCAFDoc_Centroid) S = Handle(XCAFDoc_Centroid)::DownCast (Source);
  Handle(PXCAFDoc_Centroid) T = Handle(PXCAFDoc_Centroid)::DownCast (Target);
  
  T->Set(S->Get());
}
