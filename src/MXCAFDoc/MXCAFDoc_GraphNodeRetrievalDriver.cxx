// File:	MXCAFDoc_GraphNodeRetrievalDriver.cxx
// Created:	Fri Sep 29 10:45:58 2000
// Author:	Pavel TELKOV
//		<det@nordox.nnov.matra-dtv.fr>


#include <MXCAFDoc_GraphNodeRetrievalDriver.ixx>
#include <PXCAFDoc_GraphNode.hxx>
#include <XCAFDoc_GraphNode.hxx>
#include <Standard_NoSuchObject.hxx>

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

MXCAFDoc_GraphNodeRetrievalDriver::MXCAFDoc_GraphNodeRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver) : MDF_ARDriver (theMsgDriver)
{}


//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MXCAFDoc_GraphNodeRetrievalDriver::VersionNumber() const
{ return 0; }


//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

Handle(Standard_Type) MXCAFDoc_GraphNodeRetrievalDriver::SourceType() const
{ return STANDARD_TYPE(PXCAFDoc_GraphNode);  }


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) MXCAFDoc_GraphNodeRetrievalDriver::NewEmpty() const
{ return new XCAFDoc_GraphNode(); }


//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MXCAFDoc_GraphNodeRetrievalDriver::Paste (const Handle(PDF_Attribute)&        Source,
					      const Handle(TDF_Attribute)&        Target,
					      const Handle(MDF_RRelocationTable)& RelocTable) const
{
  Handle(PXCAFDoc_GraphNode) S = Handle(PXCAFDoc_GraphNode)::DownCast(Source);
  Handle(XCAFDoc_GraphNode) T = Handle(XCAFDoc_GraphNode)::DownCast(Target);
  T->SetGraphID(S->GetGraphID());

  Handle(XCAFDoc_GraphNode) TFirst;
  Handle(PXCAFDoc_GraphNode) PFirst;
  Standard_Integer i;
  for ( i = 1; i <= S->NbFathers(); i++) {
    PFirst = S->GetFather(i); 
    if ( !PFirst.IsNull() ) {
      if (!RelocTable->HasRelocation(PFirst, TFirst)) { 
	Standard_NoSuchObject::Raise("MXCAFDoc_GraphNodeRetrievalDriver::Paste");
	}
      if (!TFirst.IsNull()) {
	TFirst->SetGraphID(S->GetGraphID());
	T->SetFather(TFirst);
      }
    }
  }

  for ( i = 1; i <= S->NbChildren(); i++) {
    PFirst = S->GetChild(i);
    if ( !PFirst.IsNull() ) {
      if (!RelocTable->HasRelocation(PFirst, TFirst)) { 
	Standard_NoSuchObject::Raise("MXCAFDoc_GraphNodeRetrievalDriver::Paste");
	}
      if (!TFirst.IsNull()) {
	TFirst->SetGraphID(S->GetGraphID());
	T->SetChild(TFirst);
      }
    }
  }
  
  
}
