#include <MDataXtd_ShapeRetrievalDriver.ixx>
#include <PDataXtd_Shape.hxx>
#include <TDataXtd_Shape.hxx>
#include <CDM_MessageDriver.hxx>

MDataXtd_ShapeRetrievalDriver::MDataXtd_ShapeRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ARDriver(theMsgDriver)
{}

Standard_Integer MDataXtd_ShapeRetrievalDriver::VersionNumber() const
{ return 0; }

Handle(Standard_Type) MDataXtd_ShapeRetrievalDriver::SourceType() const
{ return STANDARD_TYPE(PDataXtd_Shape); }

Handle(TDF_Attribute) MDataXtd_ShapeRetrievalDriver::NewEmpty() const
{ return new TDataXtd_Shape; }

void MDataXtd_ShapeRetrievalDriver::Paste(const Handle(PDF_Attribute)& Source,const Handle(TDF_Attribute)& Target,const Handle(MDF_RRelocationTable)& RelocTable) const
{
}

