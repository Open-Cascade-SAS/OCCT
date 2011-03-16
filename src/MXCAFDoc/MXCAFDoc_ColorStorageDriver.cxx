// File:	MXCAFDoc_ColorStorageDriver.cxx
// Created:	Tue Aug 15 15:32:19 2000
// Author:	data exchange team
//		<det@strelox.nnov.matra-dtv.fr>


#include <MDF_SRelocationTable.hxx>
#include <MXCAFDoc_ColorStorageDriver.ixx>
#include <MgtTopLoc.hxx>
#include <PTColStd_TransientPersistentMap.hxx>
#include <PXCAFDoc_Color.hxx>
#include <XCAFDoc_Color.hxx>

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

MXCAFDoc_ColorStorageDriver::MXCAFDoc_ColorStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver) : MDF_ASDriver (theMsgDriver)
{
}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

 Standard_Integer MXCAFDoc_ColorStorageDriver::VersionNumber() const
{  return 0; }

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

 Handle(Standard_Type) MXCAFDoc_ColorStorageDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(XCAFDoc_Color);
  return sourceType;
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

 Handle(PDF_Attribute) MXCAFDoc_ColorStorageDriver::NewEmpty() const
{
  return new PXCAFDoc_Color();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

 void MXCAFDoc_ColorStorageDriver::Paste(const Handle(TDF_Attribute)& Source,
						  const Handle(PDF_Attribute)& Target,
						  const Handle(MDF_SRelocationTable)& //RelocTable
						  ) const
{
  Handle(XCAFDoc_Color) S = Handle(XCAFDoc_Color)::DownCast (Source);
  Handle(PXCAFDoc_Color) T = Handle(PXCAFDoc_Color)::DownCast (Target);
  
  T->Set(S->GetColor());
}

