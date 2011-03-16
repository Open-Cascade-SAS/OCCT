#include <MDataXtd_PlacementRetrievalDriver.ixx>
#include <PDataXtd_Placement.hxx>
#include <TDataXtd_Placement.hxx>
#include <CDM_MessageDriver.hxx>


//=======================================================================
//function : MDataXtd_PlacementRetrievalDriver
//purpose  : 
//=======================================================================

MDataXtd_PlacementRetrievalDriver::MDataXtd_PlacementRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ARDriver(theMsgDriver)
{
}


//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MDataXtd_PlacementRetrievalDriver::VersionNumber() const
{ return 0; }



//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

Handle(Standard_Type) MDataXtd_PlacementRetrievalDriver::SourceType() const
{ return STANDARD_TYPE(PDataXtd_Placement);
}



//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) MDataXtd_PlacementRetrievalDriver::NewEmpty() const
{ return new TDataXtd_Placement; }



//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MDataXtd_PlacementRetrievalDriver::Paste (
//  const Handle(PDF_Attribute)&        Source,
  const Handle(PDF_Attribute)&        ,
//  const Handle(TDF_Attribute)&        Target,
  const Handle(TDF_Attribute)&        ,
//  const Handle(MDF_RRelocationTable)& RelocTable) const
  const Handle(MDF_RRelocationTable)& ) const
{
}
