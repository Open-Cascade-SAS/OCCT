#include <MDataXtd_AxisStorageDriver.ixx>
#include <PDataXtd_Axis.hxx>
#include <TDataXtd_Axis.hxx>
#include <CDM_MessageDriver.hxx>

MDataXtd_AxisStorageDriver::MDataXtd_AxisStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ASDriver(theMsgDriver)
{
}
 Standard_Integer MDataXtd_AxisStorageDriver::VersionNumber() const
{ return 0; }

Handle(Standard_Type) MDataXtd_AxisStorageDriver::SourceType() const
{ return STANDARD_TYPE(TDataXtd_Axis); }

Handle(PDF_Attribute) MDataXtd_AxisStorageDriver::NewEmpty() const
{ return new PDataXtd_Axis (); }

//void MDataXtd_AxisStorageDriver::Paste(const Handle(TDF_Attribute)& Source,const Handle(PDF_Attribute)& Target,const Handle(MDF_SRelocationTable)& RelocTable) const
void MDataXtd_AxisStorageDriver::Paste(const Handle(TDF_Attribute)& ,const Handle(PDF_Attribute)& ,const Handle(MDF_SRelocationTable)& ) const
{
}

