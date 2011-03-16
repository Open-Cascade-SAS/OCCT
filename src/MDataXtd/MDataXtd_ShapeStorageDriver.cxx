#include <MDataXtd_ShapeStorageDriver.ixx>
#include <PDataXtd_Shape.hxx>
#include <TDataXtd_Shape.hxx>
#include <CDM_MessageDriver.hxx>

MDataXtd_ShapeStorageDriver::MDataXtd_ShapeStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ASDriver(theMsgDriver)
{
}

Standard_Integer MDataXtd_ShapeStorageDriver::VersionNumber() const
{ return 0; }

Handle(Standard_Type) MDataXtd_ShapeStorageDriver::SourceType() const
{ return STANDARD_TYPE(TDataXtd_Shape); }

Handle(PDF_Attribute) MDataXtd_ShapeStorageDriver::NewEmpty() const
{ return new PDataXtd_Shape; }

//void MDataXtd_ShapeStorageDriver::Paste(const Handle(TDF_Attribute)& Source,const Handle(PDF_Attribute)& Target,const Handle(MDF_SRelocationTable)& RelocTable) const
void MDataXtd_ShapeStorageDriver::Paste(const Handle(TDF_Attribute)& ,const Handle(PDF_Attribute)& ,const Handle(MDF_SRelocationTable)& ) const
{
}

