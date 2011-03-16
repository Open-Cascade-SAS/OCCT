#include <MDataXtd_AxisRetrievalDriver.ixx>
#include <PDataXtd_Axis.hxx>
#include <TDataXtd_Axis.hxx>
#include <CDM_MessageDriver.hxx>

MDataXtd_AxisRetrievalDriver::MDataXtd_AxisRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ARDriver(theMsgDriver)
{
}

Standard_Integer MDataXtd_AxisRetrievalDriver::VersionNumber() const
{ return 0; }

Handle(Standard_Type) MDataXtd_AxisRetrievalDriver::SourceType() const
{ return STANDARD_TYPE(PDataXtd_Axis); }

Handle(TDF_Attribute) MDataXtd_AxisRetrievalDriver::NewEmpty() const
{ return new TDataXtd_Axis (); }

//void MDataXtd_AxisRetrievalDriver::Paste(const Handle(PDF_Attribute)& Source,const Handle(TDF_Attribute)& Target,const Handle(MDF_RRelocationTable)& RelocTable) const
void MDataXtd_AxisRetrievalDriver::Paste(const Handle(PDF_Attribute)& ,const Handle(TDF_Attribute)& ,const Handle(MDF_RRelocationTable)& ) const
{
}

