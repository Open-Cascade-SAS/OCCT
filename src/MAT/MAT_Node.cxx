// File:	MAT_Node.cxx
// Created:	Thu May  6 18:34:58 1993
// Author:	Yves FRICAUD
//		<yfr@phylox>

#include <MAT_Node.ixx>
#include <MAT_BasicElt.hxx>
#include <MAT_Arc.hxx>
#include <Precision.hxx>

//=============================================================================
//function : 
//Purpose  :
//=============================================================================
MAT_Node::MAT_Node(const Standard_Integer     GeomIndex, 
		   const Handle(MAT_Arc)&     LinkedArc, 
		   const Standard_Real        Distance)
     : geomIndex(GeomIndex),
       distance(Distance)
{
  aLinkedArc = LinkedArc.operator->();
}

//=============================================================================
//function : GeomIndex 
//Purpose  :
//=============================================================================
Standard_Integer  MAT_Node::GeomIndex() const
{
  return geomIndex;
}

//=============================================================================
//function : Index
//Purpose  :
//=============================================================================
Standard_Integer  MAT_Node::Index() const
{
  return nodeIndex;
}

//=============================================================================
//function : LinkedArcs
//Purpose  :
//=============================================================================
void MAT_Node::LinkedArcs(MAT_SequenceOfArc& S) const
{
  S.Clear();
  Handle(MAT_Node) Me = this;
  Handle(MAT_Arc)  LA((MAT_Arc*)aLinkedArc);

  S.Append(LA);

  if (LA->HasNeighbour(Me, MAT_Left)) {
    Handle(MAT_Arc)  CA = LA->Neighbour(Me, MAT_Left);
    while (CA != LA) {
      S.Append(CA);
      CA = CA->Neighbour(Me, MAT_Left);
    }
  }
}

//=============================================================================
//function : NearElts 
//Purpose  :
//=============================================================================
void MAT_Node::NearElts(MAT_SequenceOfBasicElt& S) const
{
  S.Clear();

  Handle(MAT_Node) Me = this;
  Handle(MAT_Arc)  LA((MAT_Arc*)aLinkedArc);

  S.Append(LA->FirstElement());
  S.Append(LA->SecondElement());

  if (LA->HasNeighbour(Me, MAT_Left)) {

    Handle(MAT_Arc)  CA = LA->Neighbour(Me, MAT_Left);
    Standard_Boolean Pair = Standard_False;
    
    //---------------------------------------------------------
    // Recuperation des deux elements separes pour un arc sur
    // deux.
    //---------------------------------------------------------
    
    while (CA != LA) {
      if (Pair) {
	S.Append(CA->FirstElement());
	S.Append(CA->SecondElement());
      }
      else {
	Pair = Standard_True;
      }
      CA = CA->Neighbour(Me, MAT_Left);
    }
  }
}
  
//=============================================================================
//function : Distance
//Purpose  :
//=============================================================================
Standard_Real  MAT_Node::Distance() const
{
  return distance;
}


//=============================================================================
//function : PendingNode
//Purpose  :
//=============================================================================
Standard_Boolean  MAT_Node::PendingNode() const
{
  Handle(MAT_Node) Me = this;
  return (!((MAT_Arc*)aLinkedArc)->HasNeighbour(Me,MAT_Left));
}

//=============================================================================
//function : NodeOnFig
//Purpose  :
//=============================================================================
Standard_Boolean  MAT_Node::OnBasicElt() const
{
  return (Distance() == 0.0);
}

//=============================================================================
//function : NodeInfinite
//Purpose  :
//=============================================================================
Standard_Boolean  MAT_Node::Infinite() const
{
  return (Distance() == Precision::Infinite());
}

//=============================================================================
//function : SetLinkedArcs
//Purpose  :
//=============================================================================
void MAT_Node::SetLinkedArc (const Handle(MAT_Arc)& LinkedArc)
{
  aLinkedArc = LinkedArc.operator->();
}

//=============================================================================
//function : SetIndex
//Purpose  :
//=============================================================================
void MAT_Node::SetIndex (const Standard_Integer anIndex)
{
  nodeIndex = anIndex;
}




