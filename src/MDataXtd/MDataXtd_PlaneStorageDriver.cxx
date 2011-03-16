#include <MDataXtd_PlaneStorageDriver.ixx>
#include <PDataXtd_Plane.hxx>
#include <TDataXtd_Plane.hxx>
#include <CDM_MessageDriver.hxx>

MDataXtd_PlaneStorageDriver::MDataXtd_PlaneStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ASDriver(theMsgDriver)
{
}

Standard_Integer MDataXtd_PlaneStorageDriver::VersionNumber() const
{ return 0; }

Handle(Standard_Type) MDataXtd_PlaneStorageDriver::SourceType() const
{ return STANDARD_TYPE(TDataXtd_Plane); }

Handle(PDF_Attribute) MDataXtd_PlaneStorageDriver::NewEmpty() const
{ return new PDataXtd_Plane (); }

//void MDataXtd_PlaneStorageDriver::Paste(const Handle(TDF_Attribute)& Source,const Handle(PDF_Attribute)& Target,const Handle(MDF_SRelocationTable)& RelocTable) const
void MDataXtd_PlaneStorageDriver::Paste(const Handle(TDF_Attribute)& ,const Handle(PDF_Attribute)& ,const Handle(MDF_SRelocationTable)& ) const
{
}

