// File:	MXCAFDoc_CentroidRetrievalDriver.cxx
// Created:	Mon Sep 11 15:20:10 2000
// Author:	data exchange team
//		<det@doomox>


#include <MXCAFDoc_CentroidRetrievalDriver.ixx>
#include <MDF_RRelocationTable.hxx>
#include <PTColStd_PersistentTransientMap.hxx>
#include <PXCAFDoc_Centroid.hxx>
#include <XCAFDoc_Centroid.hxx>

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

MXCAFDoc_CentroidRetrievalDriver::MXCAFDoc_CentroidRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver) : MDF_ARDriver (theMsgDriver)
{
}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

 Standard_Integer MXCAFDoc_CentroidRetrievalDriver::VersionNumber() const
{
  return 0;
}

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

 Handle(Standard_Type) MXCAFDoc_CentroidRetrievalDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(PXCAFDoc_Centroid);
  return sourceType;
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

 Handle(TDF_Attribute) MXCAFDoc_CentroidRetrievalDriver::NewEmpty() const
{
  return new XCAFDoc_Centroid();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

 void MXCAFDoc_CentroidRetrievalDriver::Paste(const Handle(PDF_Attribute)& Source,
						    const Handle(TDF_Attribute)& Target,
						    const Handle(MDF_RRelocationTable)& RelocTable) const
{
  Handle(PXCAFDoc_Centroid) S = Handle(PXCAFDoc_Centroid)::DownCast (Source);
  Handle(XCAFDoc_Centroid) T = Handle(XCAFDoc_Centroid)::DownCast (Target);

  T->Set(S->Get());
}
