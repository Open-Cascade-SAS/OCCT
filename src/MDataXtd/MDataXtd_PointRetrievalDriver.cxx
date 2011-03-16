#include <MDataXtd_PointRetrievalDriver.ixx>
#include <PDataXtd_Point.hxx>
#include <TDataXtd_Point.hxx>
#include <CDM_MessageDriver.hxx>

MDataXtd_PointRetrievalDriver::MDataXtd_PointRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ARDriver(theMsgDriver)
{
}

Standard_Integer MDataXtd_PointRetrievalDriver::VersionNumber() const
{ return 0; }

Handle(Standard_Type) MDataXtd_PointRetrievalDriver::SourceType() const
{ return STANDARD_TYPE(PDataXtd_Point); }

Handle(TDF_Attribute) MDataXtd_PointRetrievalDriver::NewEmpty() const
{ return new TDataXtd_Point (); }

//void MDataXtd_PointRetrievalDriver::Paste(const Handle(PDF_Attribute)& Source,const Handle(TDF_Attribute)& Target,const Handle(MDF_RRelocationTable)& RelocTable) const
void MDataXtd_PointRetrievalDriver::Paste(const Handle(PDF_Attribute)& ,const Handle(TDF_Attribute)& ,const Handle(MDF_RRelocationTable)& ) const
{
}

