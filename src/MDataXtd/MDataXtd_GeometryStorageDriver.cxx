// File:	MDataXtd_GeometryStorageDriver.cxx
// Created:	Wed Nov 19 16:00:50 1997
// Author:	Denis PASCAL
//		<dp@dingox.paris1.matra-dtv.fr>


#include <MDataXtd_GeometryStorageDriver.ixx> 
#include <TDataXtd_GeometryEnum.hxx>
#include <MDataXtd.hxx>
#include <TDataXtd_Geometry.hxx>
#include <PDataXtd_Geometry.hxx>
#include <CDM_MessageDriver.hxx>


//=======================================================================
//function : MDataXtd_GeometryStorageDriver
//purpose  : 
//=======================================================================

MDataXtd_GeometryStorageDriver::MDataXtd_GeometryStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ASDriver(theMsgDriver)
{
}


//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MDataXtd_GeometryStorageDriver::VersionNumber() const
{ return 0; }


//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

Handle(Standard_Type) MDataXtd_GeometryStorageDriver::SourceType() const
{ return STANDARD_TYPE (TDataXtd_Geometry); }


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(PDF_Attribute) MDataXtd_GeometryStorageDriver::NewEmpty() const
{ return new PDataXtd_Geometry; }


//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MDataXtd_GeometryStorageDriver::Paste (
  const Handle(TDF_Attribute)&        Source,
  const Handle(PDF_Attribute)&        Target,
//  const Handle(MDF_SRelocationTable)& RelocTable) const
  const Handle(MDF_SRelocationTable)& ) const
{
  Handle(TDataXtd_Geometry) S = 
     Handle(TDataXtd_Geometry)::DownCast (Source);
  Handle(PDataXtd_Geometry) T = 
    Handle(PDataXtd_Geometry)::DownCast (Target);
  
  T->SetType (MDataXtd::GeometryTypeToInteger(S->GetType ()));

}





