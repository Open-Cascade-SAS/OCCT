// File:	HLRBRep_FaceIterator.cxx
// Created:	Thu Apr 17 16:00:19 1997
// Author:	Christophe MARION
//		<cma@partox.paris1.matra-dtv.fr>
#ifndef No_Exception
#define No_Exception
#endif
#include <HLRBRep_FaceIterator.ixx>

//=======================================================================
//function : FaceIterator
//purpose  : 
//=======================================================================

HLRBRep_FaceIterator::HLRBRep_FaceIterator()
{}

//=======================================================================
//function : InitEdge
//purpose  : 
//=======================================================================

void HLRBRep_FaceIterator::InitEdge(HLRBRep_FaceData& fd)
{
  iWire = 0;
  myWires = fd.Wires();
  nbWires = myWires->NbWires();

  iEdge = 0;
  nbEdges = 0;
  NextEdge();
}

//=======================================================================
//function : NextEdge
//purpose  : 
//=======================================================================

void HLRBRep_FaceIterator::NextEdge()
{
  iEdge++;
  if (iEdge > nbEdges) {
    iWire++;
    if (iWire <= nbWires) {
      iEdge = 1;
      myEdges = myWires->Wire(iWire);
      nbEdges = myEdges->NbEdges();
    }
  }
}

