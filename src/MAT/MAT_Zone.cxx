// Created on: 1993-05-05
// Created by: Yves FRICAUD
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <MAT_Arc.hxx>
#include <MAT_BasicElt.hxx>
#include <MAT_Node.hxx>
#include <MAT_SequenceOfArc.hxx>
#include <MAT_Zone.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(MAT_Zone,Standard_Transient)

//========================================================================
// function:
// purpose :
//========================================================================
MAT_Zone::MAT_Zone ()
{}

//========================================================================
// function:
// purpose :
//========================================================================
MAT_Zone::MAT_Zone(const Handle(MAT_BasicElt)& aBasicElt) 
{
  Perform (aBasicElt);
}

//========================================================================
// function: Perform
// purpose :
//========================================================================
void MAT_Zone::Perform (const Handle(MAT_BasicElt)& aBasicElt) 
{
  Handle (MAT_Node)       NextNode, StartNode;
  Handle (MAT_Arc)        CurrentArc;

  limited = Standard_True;
  frontier.Clear();
  // ------------------------------------------------------------------------
  // Si le premier arc correspondant a la zone est Null => Sequence vide.
  // ------------------------------------------------------------------------
  if (aBasicElt->EndArc().IsNull()) return;

  // ----------------------------
  // Angle rentrant => Zone Vide.
  // ----------------------------
  // if(aBasicElt->EndArc() == aBasicElt->StartArc()) return;

  // --------------------------------
  // Initialisation de la frontier.
  // --------------------------------
  CurrentArc = aBasicElt->EndArc();
  frontier.Append(CurrentArc);

  // --------------------------------------------------------------------------  
  // Determination du premier noeud qui permet de construire la zone en tournant
  // surla gauche.
  // --------------------------------------------------------------------------
  NextNode  = NodeForTurn(CurrentArc,aBasicElt,MAT_Left);
  StartNode = CurrentArc->TheOtherNode(NextNode);

  // -------------------------------------------------------------------------
  // Exploration du Graph toujours sur les arcs voisins a gauche jusqu'a
  // - retour sur la Figure .
  // - l acces a un noeud infini .
  // (Ces deux  cas correspondent a des noeuds pendants.)
  // - retour sur l arc de depart si le basicElt est ferme.
  // -------------------------------------------------------------------------
  
  while (!NextNode->PendingNode() && (NextNode != StartNode)) {
    CurrentArc = CurrentArc->Neighbour(NextNode,MAT_Left);
    frontier.Append(CurrentArc);
    NextNode   = CurrentArc->TheOtherNode(NextNode);
  }

  // -----------------------------------------------------------------------
  // Si NextNode est a l infini : exploration du graph a partir du StartArc 
  //   sur <aBasicElt>.
  //   exploration sur les arcs voisins a droite.
  // Sinon => Fin.
  // -----------------------------------------------------------------------

  if (NextNode->Infinite()) {
    limited    = Standard_False;
    CurrentArc = aBasicElt->StartArc();  
    frontier.Append(CurrentArc);
    // --------------------------------------------------------------------------
    // Determination du premier noeud qui permet de construire la zone en 
    //tournan surla droite.
    // --------------------------------------------------------------------------
    NextNode = NodeForTurn(CurrentArc,aBasicElt,MAT_Right);
    
    // -----------------------------------------------------
    // Cette branche est aussi terminee par un noeud infini.
    // -----------------------------------------------------
    while (!NextNode->Infinite()) {
      CurrentArc = CurrentArc->Neighbour(NextNode,MAT_Right);
      frontier.Append(CurrentArc);
      NextNode   = CurrentArc->TheOtherNode(NextNode);
    }
  }
}

//========================================================================
// function: NumberOfArcs
// purpose :
//========================================================================
Standard_Integer  MAT_Zone::NumberOfArcs()const 
{
  return frontier.Length();
}

//========================================================================
// function: ArcOnFrontier
// purpose :
//========================================================================
Handle(MAT_Arc)  MAT_Zone::ArcOnFrontier(const Standard_Integer Index)const 
{
  return frontier.Value(Index);
}

//========================================================================
// function: NoEmptyZone
// purpose :
//========================================================================
Standard_Boolean  MAT_Zone::NoEmptyZone()const 
{
   return (!frontier.IsEmpty());
}

//========================================================================
// function: Limited
// purpose :
//========================================================================
Standard_Boolean  MAT_Zone::Limited()const 
{
   return limited;
}

//========================================================================
// function:
// purpose :
//========================================================================
Handle(MAT_Node) MAT_Zone::NodeForTurn (const Handle(MAT_Arc)&      anArc,
					const Handle(MAT_BasicElt)& aBE,
					const MAT_Side              aSide) 
     const
{
  Handle(MAT_Arc)  NeighbourArc;
  Handle(MAT_Node) NodeSol     ;

  NodeSol      = anArc->FirstNode();
  NeighbourArc = anArc->Neighbour(NodeSol,aSide);
  if (NeighbourArc.IsNull()) {    
    NodeSol      = anArc->SecondNode();
    NeighbourArc = anArc->Neighbour(NodeSol,aSide);
  }
  if (NeighbourArc.IsNull()) {
    return NodeSol;
  }
  if (NeighbourArc->FirstElement() == aBE) {
    return NodeSol;
  }
  else if (NeighbourArc->SecondElement() == aBE) { 
    return NodeSol;
  }
  else {
    return anArc->TheOtherNode(NodeSol);
  }
}
