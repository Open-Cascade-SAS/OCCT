// File:	MXCAFDoc_AreaRetrievalDriver.cxx
// Created:	Fri Sep  8 18:31:04 2000
// Author:	data exchange team
//		<det@nordox.nnov.matra-dtv.fr>


#include <MXCAFDoc_AreaRetrievalDriver.ixx>
#include <PXCAFDoc_Area.hxx>
#include <XCAFDoc_Area.hxx>

//=======================================================================
//function : MCAFDoc_AreaRetrievalDriver
//purpose  : 
//=======================================================================

MXCAFDoc_AreaRetrievalDriver::MXCAFDoc_AreaRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver) : MDF_ARDriver (theMsgDriver)
{}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MXCAFDoc_AreaRetrievalDriver::VersionNumber() const
{ return 0; }

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

 Handle(Standard_Type) MXCAFDoc_AreaRetrievalDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(PXCAFDoc_Area);
  return sourceType;
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

 Handle(TDF_Attribute) MXCAFDoc_AreaRetrievalDriver::NewEmpty() const
{
  return new XCAFDoc_Area();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

 void MXCAFDoc_AreaRetrievalDriver::Paste(const Handle(PDF_Attribute)& Source,
						    const Handle(TDF_Attribute)& Target,
						    const Handle(MDF_RRelocationTable)& RelocTable) const
{
  Handle(PXCAFDoc_Area) S = Handle(PXCAFDoc_Area)::DownCast (Source);
  Handle(XCAFDoc_Area) T = Handle(XCAFDoc_Area)::DownCast (Target);
  
  T->Set(S->Get());
}
