// File:	MXCAFDoc_AreaStorageDriver.cxx
// Created:	Fri Sep  8 18:37:02 2000
// Author:	data exchange team
//		<det@nordox.nnov.matra-dtv.fr>


#include <MXCAFDoc_AreaStorageDriver.ixx>
#include <XCAFDoc_Area.hxx>
#include <PXCAFDoc_Area.hxx>

//=======================================================================
//function : MXCAFDoc_AreaStorageDriver
//purpose  : 
//=======================================================================

MXCAFDoc_AreaStorageDriver::MXCAFDoc_AreaStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver) : MDF_ASDriver (theMsgDriver)
{}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MXCAFDoc_AreaStorageDriver::VersionNumber() const
{ return 0; }

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

 Handle(Standard_Type) MXCAFDoc_AreaStorageDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(XCAFDoc_Area);
  return sourceType;
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

 Handle(PDF_Attribute) MXCAFDoc_AreaStorageDriver::NewEmpty() const
{
  return new PXCAFDoc_Area();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

 void MXCAFDoc_AreaStorageDriver::Paste(const Handle(TDF_Attribute)& Source,
						  const Handle(PDF_Attribute)& Target,
						  const Handle(MDF_SRelocationTable)& RelocTable) const
{
  Handle(XCAFDoc_Area) S = Handle(XCAFDoc_Area)::DownCast (Source);
  Handle(PXCAFDoc_Area) T = Handle(PXCAFDoc_Area)::DownCast (Target);
  
  T->Set(S->Get());
}
