// File:	MXCAFDoc_DocumentToolRetrievalDriver.cxx
// Created:	Tue Aug 15 15:37:48 2000
// Author:	data exchange team
//		<det@strelox.nnov.matra-dtv.fr>


#include <MDF_RRelocationTable.hxx>
#include <MXCAFDoc_DocumentToolRetrievalDriver.ixx>
#include <MgtTopLoc.hxx>
#include <PTColStd_PersistentTransientMap.hxx>
#include <PXCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <TDF_Label.hxx>


//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

MXCAFDoc_DocumentToolRetrievalDriver::MXCAFDoc_DocumentToolRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver) : MDF_ARDriver (theMsgDriver)
{
}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

 Standard_Integer MXCAFDoc_DocumentToolRetrievalDriver::VersionNumber() const
{
  return 0;
}

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

 Handle(Standard_Type) MXCAFDoc_DocumentToolRetrievalDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(PXCAFDoc_DocumentTool);
  return sourceType;
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

 Handle(TDF_Attribute) MXCAFDoc_DocumentToolRetrievalDriver::NewEmpty() const
{
  return new XCAFDoc_DocumentTool();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

 void MXCAFDoc_DocumentToolRetrievalDriver::Paste(const Handle(PDF_Attribute)& ,//Source,
						    const Handle(TDF_Attribute)& Target,
						    const Handle(MDF_RRelocationTable)&// RelocTable
						    ) const
{
  Handle(XCAFDoc_DocumentTool) T = Handle(XCAFDoc_DocumentTool)::DownCast (Target);
  T->Init();
//  TDF_Label L = T->Label();
//  if (L.IsNull()) {
//    cout<<"XCAFDoc_DocumentTool::Paste  FUCK!!!"<<endl;
//    return;
//  }
//  L.ForgetAttribute(XCAFDoc_DocumentTool::GetID());
//  XCAFDoc_DocumentTool::Set(L, Standard_False);
}

