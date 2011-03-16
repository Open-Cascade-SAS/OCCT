// File:	MXCAFDoc_VolumeStorageDriver.cxx
// Created:	Fri Sep  8 18:37:02 2000
// Author:	data exchange team
//		<det@nordox.nnov.matra-dtv.fr>


#include <MXCAFDoc_VolumeStorageDriver.ixx>
#include <XCAFDoc_Volume.hxx>
#include <PXCAFDoc_Volume.hxx>

//=======================================================================
//function : MXCAFDoc_VolumeStorageDriver
//purpose  : 
//=======================================================================

MXCAFDoc_VolumeStorageDriver::MXCAFDoc_VolumeStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver) : MDF_ASDriver (theMsgDriver)
{}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MXCAFDoc_VolumeStorageDriver::VersionNumber() const
{ return 0; }

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

 Handle(Standard_Type) MXCAFDoc_VolumeStorageDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(XCAFDoc_Volume);
  return sourceType;
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

 Handle(PDF_Attribute) MXCAFDoc_VolumeStorageDriver::NewEmpty() const
{
  return new PXCAFDoc_Volume();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

 void MXCAFDoc_VolumeStorageDriver::Paste(const Handle(TDF_Attribute)& Source,
						  const Handle(PDF_Attribute)& Target,
						  const Handle(MDF_SRelocationTable)& RelocTable) const
{
  Handle(XCAFDoc_Volume) S = Handle(XCAFDoc_Volume)::DownCast (Source);
  Handle(PXCAFDoc_Volume) T = Handle(PXCAFDoc_Volume)::DownCast (Target);
  
#ifdef DEB
  PTColStd_TransientPersistentMap& TPMap = 
#endif
    RelocTable->OtherTable();
  T->Set(S->Get());
}
