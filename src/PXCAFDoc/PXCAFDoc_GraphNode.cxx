// File:	PXCAFDoc_GraphNode.cxx
// Created:	Fri Sep 29 09:43:11 2000
// Author:	data exchange team
//		<det@nordox.nnov.matra-dtv.fr>


#include <PXCAFDoc_GraphNode.ixx>
#include <Standard_GUID.hxx>

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

PXCAFDoc_GraphNode::PXCAFDoc_GraphNode()
{
  myFathers  = new PXCAFDoc_GraphNodeSequence;
  myChildren = new PXCAFDoc_GraphNodeSequence;
}

//=======================================================================
//function : SetGraphID
//purpose  : 
//=======================================================================

void PXCAFDoc_GraphNode::SetGraphID (const Standard_GUID& explicitID)
{
  myGraphID = explicitID;
}

//=======================================================================
//function : GetGraphID
//purpose  : 
//=======================================================================

Standard_GUID PXCAFDoc_GraphNode::GetGraphID() const
{
  return myGraphID;
}

//=======================================================================
//function : SetFather
//purpose  : 
//=======================================================================

Standard_Integer PXCAFDoc_GraphNode::SetFather(const Handle(PXCAFDoc_GraphNode)& F) 
{
  Standard_Integer Findex = myFathers->Length();
  myFathers->Append(F);
  return ++Findex;
}

//=======================================================================
//function : SetChild
//purpose  : 
//=======================================================================

Standard_Integer PXCAFDoc_GraphNode::SetChild(const Handle(PXCAFDoc_GraphNode)& Ch) 
{
  Standard_Integer Chindex = myChildren->Length();
  myChildren->Append(Ch);
  return ++Chindex;
}

//=======================================================================
//function : GetFather
//purpose  : 
//=======================================================================

 Handle(PXCAFDoc_GraphNode) PXCAFDoc_GraphNode::GetFather(const Standard_Integer Findex) const
{
  Handle(PXCAFDoc_GraphNode) F = myFathers->Value(Findex);
  return F;
}

//=======================================================================
//function : GetChild
//purpose  : 
//=======================================================================

 Handle(PXCAFDoc_GraphNode) PXCAFDoc_GraphNode::GetChild(const Standard_Integer Chindex) const
{
  Handle(PXCAFDoc_GraphNode) Ch = myChildren->Value(Chindex);
  return Ch;
}

//=======================================================================
//function : FatherIndex
//purpose  : 
//=======================================================================

 Standard_Integer PXCAFDoc_GraphNode::FatherIndex(const Handle(PXCAFDoc_GraphNode)& F) const
{
  Standard_Integer Findex = 0;
  for ( ; Findex <= NbFathers(); Findex++) {
    if ( F == myFathers->Value(Findex)) return Findex;
  }
  return 0;
}

//=======================================================================
//function : ChildIndex
//purpose  : 
//=======================================================================

 Standard_Integer PXCAFDoc_GraphNode::ChildIndex(const Handle(PXCAFDoc_GraphNode)& Ch) const
{
  Standard_Integer Chindex = 0;
  for ( ; Chindex <= NbChildren(); Chindex++) {
    if ( Ch == myChildren->Value(Chindex)) return Chindex;
  }
  return 0;
}

//=======================================================================
//function : NbFathers
//purpose  : 
//=======================================================================

 Standard_Integer PXCAFDoc_GraphNode::NbFathers() const
{
  return myFathers->Length();
}

//=======================================================================
//function : NbChildren
//purpose  : 
//=======================================================================

 Standard_Integer PXCAFDoc_GraphNode::NbChildren() const
{
  return myChildren->Length();
}
