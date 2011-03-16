
// File:	GraphTools_SC.gxx
// Created:	Thu Sep 30 17:50:19 1993
// Author:	Denis PASCAL
//		<dp@bravox>

#include <GraphTools_SC.ixx>

//=======================================================================
//function : GraphTools_SC
//purpose  : 
//=======================================================================

GraphTools_SC::GraphTools_SC () {}


//=======================================================================
//function : Reset
//purpose  : 
//=======================================================================

void  GraphTools_SC::Reset()
{
  myBackSC.Clear();
  myVertices.Clear();
  myFrontSC.Clear();
}


//=======================================================================
//function : AddVertex
//purpose  : 
//=======================================================================

void  GraphTools_SC::AddVertex(const Standard_Integer V) 
{
  myVertices.Append (V);
}


//=======================================================================
//function : NbVertices
//purpose  : 
//=======================================================================

Standard_Integer  GraphTools_SC::NbVertices() const
{
  return myVertices.Length();
}



//=======================================================================
//function : GetVertex
//purpose  : 
//=======================================================================

Standard_Integer  GraphTools_SC::GetVertex
  (const Standard_Integer index) const
{
  return myVertices(index);
}

//=======================================================================
//function : AddFrontSC
//purpose  : 
//=======================================================================

void  GraphTools_SC::AddFrontSC(const Handle(GraphTools_SC)& SC)
{
  myFrontSC.Append(SC);
}


//=======================================================================
//function : GetFrontSC
//purpose  : 
//=======================================================================

const GraphTools_SCList&  GraphTools_SC::GetFrontSC() const
{
  return myFrontSC;
}


//=======================================================================
//function : AddBackSC
//purpose  : 
//=======================================================================

void  GraphTools_SC::AddBackSC(const Handle(GraphTools_SC)& SC)
{
  myBackSC.Append(SC);
}

//=======================================================================
//function : GetBackSC
//purpose  : 
//=======================================================================

const GraphTools_SCList&  GraphTools_SC::GetBackSC() const
{
  return myBackSC;
}


