// File:	MDataXtd_GeometryRetrievalDriver.cxx
// Created:	Wed Nov 19 15:59:14 1997
// Author:	Denis PASCAL
//		<dp@dingox.paris1.matra-dtv.fr>


#include <MDataXtd_GeometryRetrievalDriver.ixx>
#include <TDataXtd_GeometryEnum.hxx>
#include <TDataXtd_Geometry.hxx>
#include <PDataXtd_Geometry.hxx>
#include <MDataXtd.hxx>
#include <CDM_MessageDriver.hxx>



//=======================================================================
//function : MDataXtd_GeometryRetrievalDriver
//purpose  : 
//=======================================================================

MDataXtd_GeometryRetrievalDriver::MDataXtd_GeometryRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ARDriver(theMsgDriver)
{
}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MDataXtd_GeometryRetrievalDriver::VersionNumber() const
{ return 0; }


//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

Handle(Standard_Type) MDataXtd_GeometryRetrievalDriver::SourceType() const
{ return STANDARD_TYPE(PDataXtd_Geometry);  }




//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) MDataXtd_GeometryRetrievalDriver::NewEmpty() const
{ return new TDataXtd_Geometry (); }



//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MDataXtd_GeometryRetrievalDriver::Paste (
  const Handle(PDF_Attribute)&        Source,
  const Handle(TDF_Attribute)&        Target,
//  const Handle(MDF_RRelocationTable)& RelocTable) const
  const Handle(MDF_RRelocationTable)& ) const
{
  Handle(PDataXtd_Geometry) S = 
     Handle(PDataXtd_Geometry)::DownCast (Source);
  Handle(TDataXtd_Geometry) T = 
    Handle(TDataXtd_Geometry)::DownCast (Target);

  T->SetType (MDataXtd::IntegerToGeometryType (S->GetType ()));
}



