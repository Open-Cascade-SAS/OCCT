// File:	MXCAFDoc_ColorRetrievalDriver.cxx
// Created:	Tue Aug 15 15:37:48 2000
// Author:	data exchange team
//		<det@strelox.nnov.matra-dtv.fr>


#include <MDF_RRelocationTable.hxx>
#include <MXCAFDoc_ColorRetrievalDriver.ixx>
#include <MgtTopLoc.hxx>
#include <PTColStd_PersistentTransientMap.hxx>
#include <PXCAFDoc_Color.hxx>
#include <XCAFDoc_Color.hxx>


//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

MXCAFDoc_ColorRetrievalDriver::MXCAFDoc_ColorRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver) : MDF_ARDriver (theMsgDriver)
{
}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

 Standard_Integer MXCAFDoc_ColorRetrievalDriver::VersionNumber() const
{
  return 0;
}

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

 Handle(Standard_Type) MXCAFDoc_ColorRetrievalDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(PXCAFDoc_Color);
  return sourceType;
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

 Handle(TDF_Attribute) MXCAFDoc_ColorRetrievalDriver::NewEmpty() const
{
  return new XCAFDoc_Color();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

 void MXCAFDoc_ColorRetrievalDriver::Paste(const Handle(PDF_Attribute)& Source,
						    const Handle(TDF_Attribute)& Target,
						    const Handle(MDF_RRelocationTable)&// RelocTable
						    ) const
{
  Handle(PXCAFDoc_Color) S = Handle(PXCAFDoc_Color)::DownCast (Source);
  Handle(XCAFDoc_Color) T = Handle(XCAFDoc_Color)::DownCast (Target);
  
  T->Set(S->Get());

}

