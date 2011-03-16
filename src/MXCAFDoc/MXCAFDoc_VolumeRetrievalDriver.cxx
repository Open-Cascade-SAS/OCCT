// File:	MXCAFDoc_VolumeRetrievalDriver.cxx
// Created:	Fri Sep  8 18:31:04 2000
// Author:	data exchange team
//		<det@nordox.nnov.matra-dtv.fr>


#include <MXCAFDoc_VolumeRetrievalDriver.ixx>
#include <PXCAFDoc_Volume.hxx>
#include <XCAFDoc_Volume.hxx>

//=======================================================================
//function : MCAFDoc_VolumeRetrievalDriver
//purpose  : 
//=======================================================================

MXCAFDoc_VolumeRetrievalDriver::MXCAFDoc_VolumeRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver) : MDF_ARDriver (theMsgDriver)
{}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MXCAFDoc_VolumeRetrievalDriver::VersionNumber() const
{ return 0; }

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

 Handle(Standard_Type) MXCAFDoc_VolumeRetrievalDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(PXCAFDoc_Volume);
  return sourceType;
}
//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

 Handle(TDF_Attribute) MXCAFDoc_VolumeRetrievalDriver::NewEmpty() const
{
  return new XCAFDoc_Volume();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

 void MXCAFDoc_VolumeRetrievalDriver::Paste(const Handle(PDF_Attribute)& Source,
						    const Handle(TDF_Attribute)& Target,
						    const Handle(MDF_RRelocationTable)& RelocTable) const
{
  Handle(PXCAFDoc_Volume) S = Handle(PXCAFDoc_Volume)::DownCast (Source);
  Handle(XCAFDoc_Volume) T = Handle(XCAFDoc_Volume)::DownCast (Target);
  
#ifdef DEB
  PTColStd_PersistentTransientMap& PTMap = 
#endif
    RelocTable->OtherTable();
  T->Set(S->Get());
}
