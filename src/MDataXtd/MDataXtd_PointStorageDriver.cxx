#include <MDataXtd_PointStorageDriver.ixx>
#include <PDataXtd_Point.hxx>
#include <TDataXtd_Point.hxx>
#include <CDM_MessageDriver.hxx>

MDataXtd_PointStorageDriver::MDataXtd_PointStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ASDriver(theMsgDriver) {}

Standard_Integer MDataXtd_PointStorageDriver::VersionNumber() const

{ return 0; }

Handle(Standard_Type) MDataXtd_PointStorageDriver::SourceType() const
{ return STANDARD_TYPE(TDataXtd_Point);}

Handle(PDF_Attribute) MDataXtd_PointStorageDriver::NewEmpty() const
{ return new PDataXtd_Point (); }

//void MDataXtd_PointStorageDriver::Paste(const Handle(TDF_Attribute)& Source,const Handle(PDF_Attribute)& Target,const Handle(MDF_SRelocationTable)& RelocTable) const
void MDataXtd_PointStorageDriver::Paste(const Handle(TDF_Attribute)& ,const Handle(PDF_Attribute)& ,const Handle(MDF_SRelocationTable)& ) const
{
}

