#include <MDataXtd_PlaneRetrievalDriver.ixx>
#include <PDataXtd_Plane.hxx>
#include <TDataXtd_Plane.hxx>
#include <CDM_MessageDriver.hxx>

MDataXtd_PlaneRetrievalDriver::MDataXtd_PlaneRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ARDriver(theMsgDriver)
{
}

Standard_Integer MDataXtd_PlaneRetrievalDriver::VersionNumber() const
{ return 0; }

Handle(Standard_Type) MDataXtd_PlaneRetrievalDriver::SourceType() const
{ return STANDARD_TYPE (PDataXtd_Plane); }

Handle(TDF_Attribute) MDataXtd_PlaneRetrievalDriver::NewEmpty() const
{ return new TDataXtd_Plane; }

//void MDataXtd_PlaneRetrievalDriver::Paste(const Handle(PDF_Attribute)& Source,const Handle(TDF_Attribute)& Target,const Handle(MDF_RRelocationTable)& RelocTable) const
void MDataXtd_PlaneRetrievalDriver::Paste(const Handle(PDF_Attribute)& ,const Handle(TDF_Attribute)& ,const Handle(MDF_RRelocationTable)& ) const
{
}

