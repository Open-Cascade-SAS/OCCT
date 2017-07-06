// Created on: 1993-05-06
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
#include <MAT_Bisector.hxx>
#include <MAT_DataMapIteratorOfDataMapOfIntegerBasicElt.hxx>
#include <MAT_DataMapOfIntegerArc.hxx>
#include <MAT_DataMapOfIntegerBasicElt.hxx>
#include <MAT_Edge.hxx>
#include <MAT_Graph.hxx>
#include <MAT_ListOfBisector.hxx>
#include <MAT_Node.hxx>
#include <MAT_SequenceOfArc.hxx>
#include <MAT_Zone.hxx>
#include <Precision.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(MAT_Graph,Standard_Transient)

//------------------
// functions static.
//-------------------
  static Handle(MAT_Arc) MakeArc(const Handle(MAT_Bisector)&     aBisector,
				 MAT_DataMapOfIntegerBasicElt&   TheBasicElts,
				 MAT_DataMapOfIntegerArc&        TheArcs,
				 Standard_Integer&               IndTabArcs);

// =====================================================================
// Constructeur vide.
// =====================================================================
MAT_Graph::MAT_Graph() {}

// =====================================================================
// function : Perform
// purpose  : Creation du graphe contenant le resultat.
// =====================================================================
void MAT_Graph::Perform(const Standard_Boolean             SemiInfinite,
			const Handle(MAT_ListOfBisector)&  TheRoots,
			const Standard_Integer             NbBasicElts,
			const Standard_Integer             NbArcs)
{
  Standard_Integer        NbRoots;
  Handle(MAT_Arc)         FirstArc;
  Handle(MAT_Arc)         CurrentArc;
  Handle(MAT_Node)        Extremite;
  Standard_Integer        IndTabArcs = 1;
  Standard_Integer        IndTabNodes;
  Standard_Integer        i;
  Standard_Real           DistExt;
  Standard_Integer        IndExt;
  Handle(MAT_Arc)         PreviousArc = CurrentArc;

  //------------------------
  // Construction du graphe.
  //------------------------

  if (SemiInfinite) {
    NbRoots               = TheRoots->Number();
    numberOfInfiniteNodes = NbRoots;
  }
  else {
    NbRoots               = 1;
    numberOfInfiniteNodes = 0;
  } 

  numberOfArcs         = NbArcs;
  numberOfBasicElts    = NbBasicElts;
  numberOfNodes        = NbRoots + NbArcs;
  IndTabNodes          = numberOfNodes;

  //---------------------------
  //... Creation des BasicElts.
  //---------------------------
  for (i = 1; i <= NbBasicElts; i++) {
    theBasicElts.Bind(i,new MAT_BasicElt(i));
    theBasicElts(i)->SetGeomIndex(i);
  }

  //--------------------------------------------------------------------
  // ... Creation des ARCS et des NODES.
  //     Construction des arbres d arcs a partir des <Bisector> racines.
  //--------------------------------------------------------------------

  if (SemiInfinite) {

    // Plusieurs points d entree a l infini.
    //--------------------------------------
    TheRoots->First();

    while (TheRoots->More()) {
      CurrentArc = MakeArc(TheRoots->Current(),
			   theBasicElts,
			   theArcs,
			   IndTabArcs);
      Extremite = new MAT_Node (0,CurrentArc,Precision::Infinite());
      Extremite ->SetIndex     (IndTabNodes);
      CurrentArc->SetSecondNode(Extremite);
      theNodes.Bind(IndTabNodes,Extremite);
      TheRoots->Next();     
      IndTabNodes--;
    }
  }
  else {
    // -----------------------------------------------
    // Un seul point d entree .
    // Creation d un premier ARC et du NODE racine.
    // -----------------------------------------------
    NbRoots = 1;
    TheRoots->First();
    CurrentArc = MakeArc(TheRoots->Current(),
			 theBasicElts,
			 theArcs,
			 IndTabArcs);
    DistExt   = TheRoots->Current()->FirstEdge()->Distance();
    IndExt    = TheRoots->Current()->EndPoint();

    Extremite = new MAT_Node(IndExt,CurrentArc,DistExt);
    Extremite ->SetIndex     (IndTabNodes);
    CurrentArc->SetSecondNode(Extremite);
    theNodes.Bind(IndTabNodes,Extremite);
    IndTabNodes--;
    
    // -----------------------------------------------------------
    // ...Creation des ARCs issues de la racine.
    //    Codage des voisinages sur ces arcs et mise a jour de la
    //    sequence des arcs issue du Node racine.
    // -----------------------------------------------------------
    FirstArc     = CurrentArc;
    PreviousArc  = FirstArc;
    TheRoots->Next();

    while (TheRoots->More()) {
      CurrentArc = MakeArc(TheRoots->Current(),
			   theBasicElts,
			   theArcs,
			   IndTabArcs);
      CurrentArc ->SetSecondNode(Extremite);
      CurrentArc ->SetNeighbour (MAT_Left,Extremite ,PreviousArc);
      PreviousArc->SetNeighbour (MAT_Right,Extremite,CurrentArc);

      PreviousArc = CurrentArc;
      TheRoots->Next();
    }
    FirstArc  ->SetNeighbour (MAT_Left ,Extremite,CurrentArc);
    CurrentArc->SetNeighbour (MAT_Right,Extremite,FirstArc);
  }

  // ----------------------------------------------------
  // Les sequence des Arcs des Nodes racines sont a jour.
  // Mise a jour des sequences des autres Nodes.
  // ----------------------------------------------------
  UpDateNodes(IndTabNodes);
}

//=============================================================================
//function : Arc
//Purpose  :
//=============================================================================
Handle(MAT_Arc)  MAT_Graph::Arc(const Standard_Integer Index) const
{
  return theArcs(Index);
}

//=============================================================================
//function : BasicElt
//Purpose  :
//=============================================================================
Handle(MAT_BasicElt)  MAT_Graph::BasicElt(const Standard_Integer Index) const
{
  return theBasicElts(Index);
}

//=============================================================================
//function : Node
//Purpose  :
//=============================================================================
Handle(MAT_Node)  MAT_Graph::Node(const Standard_Integer Index) const
{
  return theNodes(Index);
}

//=============================================================================
//function : NumberOfArcs
//Purpose  :
//=============================================================================
Standard_Integer  MAT_Graph::NumberOfArcs() const
{
  return numberOfArcs;
}

//=============================================================================
//function : NumberOfNodes
//Purpose  :
//=============================================================================
Standard_Integer  MAT_Graph::NumberOfNodes() const
{
  return numberOfNodes;
}

//=============================================================================
//function : NumberOfInfiniteNodes
//Purpose  :
//=============================================================================
Standard_Integer  MAT_Graph::NumberOfInfiniteNodes() const
{
  return numberOfInfiniteNodes;
}

//=============================================================================
//function : NumberOfBasicElts
//Purpose  :
//=============================================================================
Standard_Integer  MAT_Graph::NumberOfBasicElts() const
{
  return numberOfBasicElts;
}

//=============================================================================
//function : FusionOfBasicElts 
//Purpose  :
//=============================================================================
void MAT_Graph::FusionOfBasicElts(const Standard_Integer  IndexElt1, 
		                  const Standard_Integer  IndexElt2,
				        Standard_Boolean& MergeArc1,
				        Standard_Integer& IGeomArc1,
				        Standard_Integer& IGeomArc2,
				        Standard_Boolean& MergeArc2,
				        Standard_Integer& IGeomArc3,
				        Standard_Integer& IGeomArc4)
{
  Handle(MAT_BasicElt) Elt1 = theBasicElts(IndexElt1);
  Handle(MAT_BasicElt) Elt2 = theBasicElts(IndexElt2);
  
  if (Elt1 == Elt2) return;

  Standard_Integer i;
  Handle(MAT_Zone) Zone2   = new MAT_Zone(Elt2);

  //--------------------------------------------------------------------
  // Les arcs de la zone de Elt2 ne separent plus Elt2 et qq chose mais
  // Elt1 et qq chose.
  //--------------------------------------------------------------------
  for (i = 1; i <= Zone2->NumberOfArcs(); i++) {
     if (Zone2->ArcOnFrontier(i)->FirstElement() == Elt2) {
       theArcs(Zone2->ArcOnFrontier(i)->Index())->SetFirstElement(Elt1);
     }
     else {
       theArcs(Zone2->ArcOnFrontier(i)->Index())->SetSecondElement(Elt1);
     }
   }

  //-------------------------------------------------------------------
  // le EndArc de Elt1 et le StartArc de Elt2 peuvent separes les memes
  // elements de base => Fusion des deux arcs et mise a jour des noeuds.
  //-------------------------------------------------------------------
  Handle(MAT_Arc) EA1 = Elt1->EndArc();
  Handle(MAT_Arc) SA2 = Elt2->StartArc();

  Handle(MAT_BasicElt) E1 = EA1->FirstElement();
  Handle(MAT_BasicElt) E2 = EA1->SecondElement();
  Handle(MAT_BasicElt) E3 = SA2->FirstElement();
  Handle(MAT_BasicElt) E4 = SA2->SecondElement();
  MergeArc1 = Standard_False;

  if ( (E1 == E3 || E1 == E4) && (E2 == E3 || E2 == E4)) {
    FusionOfArcs(theArcs(EA1->Index()),theArcs(SA2->Index()));
    MergeArc1 = Standard_True;
    IGeomArc1 = EA1->GeomIndex();
    IGeomArc2 = SA2->GeomIndex();
  }
  
  //-------------------------------------------------
  // La fin de Elt1 devient la fin de Elt2.
  //-------------------------------------------------
  Elt1->SetEndArc(Elt2->EndArc());

  //-------------------------------------------------------------------
  // le EndArc de Elt1 et le StartArc de Elt1 peuvent separer les memes
  // elements de base.
  // si les noeuds des arcs ne sont pas sur le contour 
  //    => fusion des arcs.(contour ferme compose d un seul BasicElt)
  // sinon rien            (contour ferme compose de deux BasicElts)
  //-------------------------------------------------------------------
  Handle(MAT_Arc) SA1 = Elt1->StartArc();
  EA1 = Elt1->EndArc();

  if ( EA1 != SA1 ) {
    E1 = EA1->FirstElement ();
    E2 = EA1->SecondElement();
    E3 = SA1->FirstElement ();
    E4 = SA1->SecondElement();

    Standard_Boolean OnFig = (EA1->FirstNode() ->OnBasicElt() ||
			      EA1->SecondNode()->OnBasicElt() ||
			      SA1->FirstNode() ->OnBasicElt() ||
			      SA1->SecondNode()->OnBasicElt() );

    MergeArc2 = Standard_False;

    if ((E1 == E3 || E1 == E4) && (E2 == E3 || E2 == E4) && !OnFig) {
      FusionOfArcs(theArcs(EA1->Index()),theArcs(SA1->Index()));
      MergeArc2 = Standard_True;
      IGeomArc3 = EA1->GeomIndex();
      IGeomArc4 = SA1->GeomIndex();
    }
  }
  
  //----------------------------------------------------
  // un element de base a ete elimine.
  //----------------------------------------------------
  theBasicElts.UnBind(Elt2->Index());
  numberOfBasicElts--;
}

//=============================================================================
// function : FusionOfArcs
// purpose  : Fusion de deux arcs separant les memes elements.
//            l <Arc1> ira du Second noeud de <Arc2> au second Noeud de <Arc1>.
//=============================================================================
void MAT_Graph::FusionOfArcs(const Handle(MAT_Arc)& Arc1, 
		             const Handle(MAT_Arc)& Arc2)
{

  Handle(MAT_Node) OldNode1 = Arc1->FirstNode();
  Handle(MAT_Node) OldNode2 = Arc2->FirstNode();

  Arc1->SetFirstNode(Arc2->SecondNode());
  
  //--------------------------------------------------------------------
  // Mise a jour des voisinages autour du nouveau premier noeud de Arc1.
  //--------------------------------------------------------------------
  if (!Arc2->SecondNode()->Infinite()) {
    Handle(MAT_Arc) LNeighbour = Arc2->Neighbour(Arc2->SecondNode(),MAT_Left);
    Handle(MAT_Arc) RNeighbour = Arc2->Neighbour(Arc2->SecondNode(),MAT_Right);
  
    Arc1->SetFirstArc(MAT_Left,LNeighbour);
    Arc1->SetFirstArc(MAT_Right,RNeighbour);
    theArcs(LNeighbour->Index())->SetNeighbour(MAT_Right,
					       Arc2->SecondNode(),
					       Arc1);
    theArcs(RNeighbour->Index())->SetNeighbour(MAT_Left,
					       Arc2->SecondNode(),
					       Arc1);
  }
  else {
    Handle(MAT_Arc) EmptyArc;
    Arc1->SetFirstArc(MAT_Left ,EmptyArc);
    Arc1->SetFirstArc(MAT_Right,EmptyArc);
  }

  //-------------------------------------------------------------------
  // Mise a jour du premier noeud Arc1.
  //-----------------------------------------------------------------
  Arc1->FirstNode()->SetLinkedArc(Arc1);

  //------------------------------------
  // Elimination de Arc2 et des OldNode 
  //------------------------------------
  if (theNodes.IsBound(OldNode1->Index())) {
    theNodes.UnBind(OldNode1->Index());
    numberOfNodes--;
  }
  if (theNodes.IsBound(OldNode2->Index())) {
    theNodes.UnBind(OldNode2->Index());
    numberOfNodes--;
  }

  // Note: the Arc2 is actually a reference to a handle contained in theArcs map;
  // it is necessary to create copy of that handle and use only it to access
  // that object, since the handle contained in the map is destroyed by UnBind()
  Handle(MAT_Arc) anArc2 = Arc2;
  theArcs .UnBind(Arc2->Index());
  numberOfArcs--;
  
  for (Standard_Integer i = 1; i <= 2; i++){
    Handle(MAT_BasicElt) BE;
    if (i == 1)
      BE  = theBasicElts(anArc2->FirstElement ()->Index());
    else
      BE  = theBasicElts(anArc2->SecondElement ()->Index());

    if (BE->StartArc()  == anArc2) { BE->SetStartArc(Arc1);}
    if (BE->EndArc  ()  == anArc2) { BE->SetEndArc  (Arc1);}
  }
}

//=============================================================================
// function : CompactArcs
// purpose  : Decalage des Arcs pour boucher les trous.
//=============================================================================
void MAT_Graph::CompactArcs() 
{
  Standard_Integer IFind      = 0;  
  Standard_Integer i          = 1;
  Standard_Boolean YaDecalage = Standard_False;

  while (IFind < numberOfArcs) {
    if (!theArcs.IsBound(i)) {
      YaDecalage = Standard_True;
    }
    else {
      IFind++;
      if (YaDecalage) {
	theArcs(i)->SetIndex(IFind);
	theArcs.Bind(IFind,theArcs(i));
	theArcs.UnBind(i);
      }
    }
    i++;
  }
}

//=============================================================================
// function : CompactNodes
// purpose  : Decalage des Nodes pour boucher les trous.
//=============================================================================
void MAT_Graph::CompactNodes() 
{
  Standard_Integer IFind      = 0;  
  Standard_Integer i          = 1;
  Standard_Boolean YaDecalage = Standard_False;

  while (IFind < numberOfNodes) {
    if (!theNodes.IsBound(i)) {
      YaDecalage = Standard_True;
    }
    else {
      IFind++;
      if (YaDecalage) {
	theNodes(i)->SetIndex(IFind);
	theNodes.Bind(IFind,theNodes(i));
	theNodes.UnBind(i);
      }
    }
    i++;
  }
}

//=============================================================================
// function : ChangeBasicElts
// purpose  : 
//=============================================================================
void MAT_Graph::ChangeBasicElts(const MAT_DataMapOfIntegerBasicElt& NewMap) 
{
  theBasicElts = NewMap;   
  MAT_DataMapIteratorOfDataMapOfIntegerBasicElt Ite;
  for (Ite.Initialize(theBasicElts); Ite.More(); Ite.Next()) {
    Ite.Value()->SetIndex(Ite.Key());
  }
}

//=============================================================================
//function : ChangeBasicElt
//Purpose  :
//=============================================================================
Handle(MAT_BasicElt)  MAT_Graph::ChangeBasicElt(const Standard_Integer Index) 
{
  return theBasicElts(Index);
}

//=============================================================================
// function : UpDateNodes
// purpose  : Mise a jour des sequence d'ARC de chaque FirstNode de chaque arc.
//            et stockage de chaque noeud dans la table des noeuds.
//            Les noeuds racines sont traites dans PERFORM.
//=============================================================================
void MAT_Graph::UpDateNodes (Standard_Integer&  IndTabNodes)
{  
  Standard_Integer    i;
  Handle(MAT_Node)    Bout;
  Handle(MAT_Arc)     CurrentArc;
  
  for (i = 1; i <= numberOfArcs; i++) {
    Bout = theArcs(i)->FirstNode();
    theNodes.Bind(IndTabNodes,Bout);
    Bout->SetIndex(IndTabNodes);
    IndTabNodes--;
    Bout->SetLinkedArc(theArcs(i));
  }
}


//=============================================================================
// function : MakeArc
// purpose  : Creation des <ARCS> en parcourant l'arbre issue de <aBisector>.
//=============================================================================
static Handle(MAT_Arc) MakeArc(const Handle(MAT_Bisector)&     aBisector,
			       MAT_DataMapOfIntegerBasicElt&   TheBasicElts,
                               MAT_DataMapOfIntegerArc&        TheArcs,
			       Standard_Integer&               IndTabArcs)
{
  Handle(MAT_Arc)              CurrentArc;
  Handle(MAT_Arc)              PrevArc;
  Handle(MAT_Arc)              NextArc;
  Handle(MAT_Node)             Extremite;
  Handle(MAT_ListOfBisector)   BisectorList;
  Standard_Real                DistExt;
  
#ifdef OCCT_DEBUG_Graph
  cout<<"Construction Arc : Index"<<aBisector->IndexNumber()<<endl;
  cout<<"Construction Arc : Bisector"<<aBisector->BisectorNumber()<<endl;
#endif
  
  CurrentArc = new MAT_Arc(IndTabArcs,
			   aBisector->BisectorNumber(),
			   TheBasicElts(aBisector->FirstEdge()->EdgeNumber()),
			   TheBasicElts(aBisector->SecondEdge()->EdgeNumber())
			   );
  DistExt   = aBisector->DistIssuePoint();
  if (DistExt == Precision::Infinite()) {
    DistExt = 1.0;
#ifdef OCCT_DEBUG_Graph
    cout<<"PB:RECUPERATION DISTANCE SUR ISSUEPOINT."<<endl;
#endif
  }
  
  Extremite = new MAT_Node(aBisector->IssuePoint(),CurrentArc,DistExt);

  CurrentArc->SetFirstNode(Extremite);
  BisectorList = aBisector->List();
  BisectorList->First();

  if (!BisectorList->More()) {
    // -------------------
    // Arc sur le contour.
    // -------------------
    TheBasicElts(aBisector->SecondEdge()->EdgeNumber())
      ->SetStartArc(CurrentArc);
    TheBasicElts(aBisector->FirstEdge()->EdgeNumber())
      ->SetEndArc(CurrentArc);
  }
  else {
    PrevArc = CurrentArc;
      
    while (BisectorList->More()) {
      NextArc = MakeArc(BisectorList->Current(),
			TheBasicElts,
			TheArcs,
			IndTabArcs);
      NextArc->SetSecondNode(Extremite);
      NextArc->SetNeighbour (MAT_Left ,Extremite,PrevArc);
      PrevArc->SetNeighbour (MAT_Right,Extremite,NextArc);
      PrevArc  = NextArc;
      BisectorList->Next();
    }
    CurrentArc->SetNeighbour(MAT_Left ,Extremite,NextArc);
    NextArc   ->SetNeighbour(MAT_Right,Extremite,CurrentArc);
  }
  
#ifdef OCCT_DEBUG_Graph
  cout<<"IndTabArcs = "<<IndTabArcs<<endl;
  cout<<"ArcIndex   = "<<CurrentArc->ArcIndex()<<endl;
#endif  
  CurrentArc->SetIndex(IndTabArcs);
  TheArcs.Bind(IndTabArcs,CurrentArc);
  IndTabArcs          = IndTabArcs + 1;

  return CurrentArc;
}





