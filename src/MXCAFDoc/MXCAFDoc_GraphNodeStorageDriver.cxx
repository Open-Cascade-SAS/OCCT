// File:	MXCAFDoc_GraphNodeStorageDriver.cxx
// Created:	Fri Sep 29 10:46:06 2000
// Author:	data exchange team
//		<det@nordox.nnov.matra-dtv.fr>


#include <MXCAFDoc_GraphNodeStorageDriver.ixx>

#include <XCAFDoc_GraphNode.hxx>
#include <PXCAFDoc_GraphNode.hxx>
#include <Standard_NoSuchObject.hxx>


//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

MXCAFDoc_GraphNodeStorageDriver::MXCAFDoc_GraphNodeStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver) : MDF_ASDriver (theMsgDriver)
{}


//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MXCAFDoc_GraphNodeStorageDriver::VersionNumber() const
{ return 0; }


//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

Handle(Standard_Type) MXCAFDoc_GraphNodeStorageDriver::SourceType() const
{ return STANDARD_TYPE(XCAFDoc_GraphNode); }


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(PDF_Attribute) MXCAFDoc_GraphNodeStorageDriver::NewEmpty() const
{ return new PXCAFDoc_GraphNode(); }


//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MXCAFDoc_GraphNodeStorageDriver::Paste(const Handle(TDF_Attribute)&        Source,
					    const Handle(PDF_Attribute)&        Target,
					    const Handle(MDF_SRelocationTable)& RelocTable) const
{
  Handle(XCAFDoc_GraphNode) S = Handle(XCAFDoc_GraphNode)::DownCast (Source);
  Handle(PXCAFDoc_GraphNode) T = Handle(PXCAFDoc_GraphNode)::DownCast (Target);

  Handle (PXCAFDoc_GraphNode) TF;
  Handle (XCAFDoc_GraphNode) F;
  Standard_Integer i;

  for ( i = 1; i <= S->NbFathers(); i++) {
    F = S->GetFather(i);
    if (!F.IsNull ()) {
      if (!RelocTable->HasRelocation(F, TF)) {
	Standard_NoSuchObject::Raise("MXCAFDoc_GraphNodeStorageDriver::Paste");
	}
      T->SetFather(TF);
    }
  }
  
  for ( i = 1; i <= S->NbChildren(); i++) {
    F = S->GetChild(i);
    if (!F.IsNull()) {
      if (!RelocTable->HasRelocation(F, TF)) {
	Standard_NoSuchObject::Raise("MXCAFDoc_GraphNodeStorageDriver::Paste");
	}
      T->SetChild(TF);
    }
  }
  
  T->SetGraphID(S->ID());
}
