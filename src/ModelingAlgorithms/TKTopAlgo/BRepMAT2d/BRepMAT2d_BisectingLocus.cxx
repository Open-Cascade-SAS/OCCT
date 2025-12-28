// Created on: 1993-07-13
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

#include <Bisector_Bisec.hxx>
#include <BRepMAT2d_BisectingLocus.hxx>
#include <BRepMAT2d_Explorer.hxx>
#include <Geom2d_Geometry.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <gp_Pnt2d.hxx>
#include <MAT2d_BiInt.hxx>
#include <MAT2d_Circuit.hxx>
#include <MAT2d_CutCurve.hxx>
#include <MAT2d_Mat2d.hxx>
#include <NCollection_Sequence.hxx>
#include <MAT2d_Tool2d.hxx>
#include <MAT_BasicElt.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_DataMap.hxx>
#include <MAT_Graph.hxx>
#include <MAT_ListOfBisector.hxx>
#include <MAT_Node.hxx>
#include <Precision.hxx>
#include <Geom2d_Curve.hxx>

static void CutSketch(
  NCollection_Sequence<NCollection_Sequence<occ::handle<Geom2d_Geometry>>>& Figure,
  NCollection_DataMap<MAT2d_BiInt, int>&                                    NbSect);

//=================================================================================================

BRepMAT2d_BisectingLocus::BRepMAT2d_BisectingLocus()
    : isDone(false),
      nbContours(0)
{
}

//=============================================================================
// function : Compute
// purpose  : Calcul de la carte des lieux bisecteurs sur le contour defini par
//           <anExplo>.
//=============================================================================
void BRepMAT2d_BisectingLocus::Compute(BRepMAT2d_Explorer&    anExplo,
                                       const int              IndexLine,
                                       const MAT_Side         aSide,
                                       const GeomAbs_JoinType aJoinType,
                                       const bool             IsOpenResult)
{
  MAT2d_Mat2d                     TheMAT(IsOpenResult);
  occ::handle<MAT_ListOfBisector> TheRoots = new MAT_ListOfBisector();
  NCollection_Sequence<NCollection_Sequence<occ::handle<Geom2d_Geometry>>> Figure;
  int                                                                      i;

  nbSect.Clear();
  theGraph   = new MAT_Graph();
  nbContours = anExplo.NumberOfContours();
  if (nbContours == 0)
  {
    return;
  }

  //---------------------------------
  // Lecture des donnees de anExplo.
  //---------------------------------
  for (i = 1; i <= anExplo.NumberOfContours(); i++)
  {
    NCollection_Sequence<occ::handle<Geom2d_Geometry>> Line;
    Figure.Append(Line);
    for (anExplo.Init(i); anExplo.More(); anExplo.Next())
    {
      Figure.ChangeValue(i).Append(anExplo.Value());
    }
  }

  //-----------------------
  // Decoupage des courbes.
  //-----------------------
  CutSketch(Figure, nbSect);

  //----------------------------------------------------------
  // Construction du circuit sur lequel est calcule la carte.
  //----------------------------------------------------------
  occ::handle<MAT2d_Circuit> ACircuit = new MAT2d_Circuit(aJoinType, IsOpenResult);
  //  Modified by Sergey KHROMOV - Wed Mar  6 17:43:47 2002 Begin
  //   ACircuit->Perform(Figure,IndexLine,(aSide == MAT_Left));
  ACircuit->Perform(Figure, anExplo.GetIsClosed(), IndexLine, (aSide == MAT_Left));
  //  Modified by Sergey KHROMOV - Wed Mar  6 17:43:48 2002 End

  // -----------------------
  // Initialistion du Tool.
  // -----------------------
  theTool.Sense(aSide);
  theTool.SetJoinType(aJoinType);
  theTool.InitItems(ACircuit);

  // --------------------------------------------
  // Initialisation et execution de l algorithme.
  // --------------------------------------------
  if (IsOpenResult)
    TheMAT.CreateMatOpen(theTool);
  else
    TheMAT.CreateMat(theTool);

  isDone = TheMAT.IsDone();
  if (!isDone)
    return;

  // ----------------------------------------------------------------
  // Recuperation du resultat de l algorithme et creation du graphe.
  // ----------------------------------------------------------------
  for (TheMAT.Init(); TheMAT.More(); TheMAT.Next())
  {
    TheRoots->BackAdd(TheMAT.Bisector());
  }

  theGraph->Perform(TheMAT.SemiInfinite(),
                    TheRoots,
                    theTool.NumberOfItems(),
                    TheMAT.NumberOfBisectors());

  //-----------------------------------------------------------------------
  // Fusion des elements de base doubles si plusieurs lignes dans Exploset.
  //-----------------------------------------------------------------------
  if (anExplo.NumberOfContours() > 1)
  {
    NCollection_DataMap<int, occ::handle<MAT_BasicElt>> NewMap;
    int                                                 IndexLast = 1;

    //-----------------------------------------------------------------------
    // Construction de NewMap dont les elements sont ordonnes suivant les
    // lignes du contour et qui ne contient pas d element dupliques.
    // em meme temps fusion des arcs dupliques et mise a jour des noeuds.
    //-----------------------------------------------------------------------
    for (i = 1; i <= anExplo.NumberOfContours(); i++)
    {
      RenumerationAndFusion(i, theTool.Circuit()->LineLength(i), IndexLast, NewMap);
    }

    //-----------------------------------------------------------------------
    // Chargement dans le graph de la nouvelle map.
    // et compactage de la map des Arcs (ie  Elimination des trous du a la
    // fusion d arcs ).et  de celle des Nodes.
    //-----------------------------------------------------------------------
    theGraph->ChangeBasicElts(NewMap);
    theGraph->CompactArcs();
    theGraph->CompactNodes();
  }
}

//=================================================================================================

void BRepMAT2d_BisectingLocus::RenumerationAndFusion(
  const int                                            ILine,
  const int                                            LengthLine,
  int&                                                 IndexLast,
  NCollection_DataMap<int, occ::handle<MAT_BasicElt>>& NewMap)
{
  int  IndFirst;
  int  i, j;
  int  GeomIndexArc1, GeomIndexArc2, GeomIndexArc3, GeomIndexArc4;
  bool MergeArc1, MergeArc2;

  for (i = 1; i <= LengthLine; i++)
  {
    const NCollection_Sequence<int>& S = theTool.Circuit()->RefToEqui(ILine, i);

    IndFirst = S.Value(1);
    NewMap.Bind(IndexLast, theGraph->ChangeBasicElt(IndFirst));
    IndexLast++;

    for (j = 2; j <= S.Length(); j++)
    {
      theGraph->FusionOfBasicElts(IndFirst,
                                  S.Value(j),
                                  MergeArc1,
                                  GeomIndexArc1,
                                  GeomIndexArc2,
                                  MergeArc2,
                                  GeomIndexArc3,
                                  GeomIndexArc4);
      if (MergeArc1)
      {
        theTool.BisecFusion(GeomIndexArc1, GeomIndexArc2);
      }
      if (MergeArc2)
      {
        theTool.BisecFusion(GeomIndexArc3, GeomIndexArc4);
      }
    }
  }
}

//=================================================================================================

bool BRepMAT2d_BisectingLocus::IsDone() const
{
  return isDone;
}

//=============================================================================
// function : Graph
//
//=============================================================================
occ::handle<MAT_Graph> BRepMAT2d_BisectingLocus::Graph() const
{
  return theGraph;
}

//=============================================================================
// function : NumberOfContours
//
//=============================================================================
int BRepMAT2d_BisectingLocus::NumberOfContours() const
{
  return nbContours;
}

//=============================================================================
// function : NumberOfElts
//
//=============================================================================
int BRepMAT2d_BisectingLocus::NumberOfElts(const int IndLine) const
{
  return theTool.Circuit()->LineLength(IndLine);
}

//=============================================================================
// function : NumberOfSect
//
//=============================================================================
int BRepMAT2d_BisectingLocus::NumberOfSections(const int IndLine, const int Index) const
{
  MAT2d_BiInt B(IndLine, Index);
  return nbSect(B);
}

//=============================================================================
// function : BasicElt
//
//=============================================================================
occ::handle<MAT_BasicElt> BRepMAT2d_BisectingLocus::BasicElt(const int IndLine,
                                                             const int Index) const
{
  int i;
  int Ind = Index;

  for (i = 1; i < IndLine; i++)
  {
    Ind = Ind + theTool.Circuit()->LineLength(i);
  }
  return theGraph->BasicElt(Ind);
}

//=============================================================================
// function : GeomBis
//
//=============================================================================
Bisector_Bisec BRepMAT2d_BisectingLocus::GeomBis(const occ::handle<MAT_Arc>& anArc,
                                                 bool&                       Reverse) const
{
  Reverse = false;

  occ::handle<Geom2d_Curve> Bis(theTool.GeomBis(anArc->GeomIndex()).Value());

  if (Bis->FirstParameter() <= -Precision::Infinite())
  {
    Reverse = true;
  }
  else if (Bis->LastParameter() < Precision::Infinite())
  {
    gp_Pnt2d PF    = Bis->Value(Bis->FirstParameter());
    gp_Pnt2d PL    = Bis->Value(Bis->LastParameter());
    gp_Pnt2d PNode = GeomElt(anArc->FirstNode());
    if (PNode.SquareDistance(PF) > PNode.SquareDistance(PL))
      Reverse = true;
  }
  return theTool.GeomBis(anArc->GeomIndex());
}

//=============================================================================
// function : GeomElt
//
//=============================================================================
occ::handle<Geom2d_Geometry> BRepMAT2d_BisectingLocus::GeomElt(
  const occ::handle<MAT_BasicElt>& aBasicElt) const
{
  return theTool.GeomElt(aBasicElt->GeomIndex());
}

//=============================================================================
// function : GeomElt
//
//=============================================================================
gp_Pnt2d BRepMAT2d_BisectingLocus::GeomElt(const occ::handle<MAT_Node>& aNode) const
{
  return theTool.GeomPnt(aNode->GeomIndex());
}

//=============================================================================
// function : CutSketch
//
//=============================================================================
static void CutSketch(
  NCollection_Sequence<NCollection_Sequence<occ::handle<Geom2d_Geometry>>>& Figure,
  NCollection_DataMap<MAT2d_BiInt, int>&                                    NbSect)
{
  MAT2d_CutCurve Cuter;
  int            i, j, k, ico;
  int            ICurveInit;
  int            NbSection;

  for (i = 1; i <= Figure.Length(); i++)
  {
    NCollection_Sequence<occ::handle<Geom2d_Geometry>>& Contour = Figure.ChangeValue(i);
    ICurveInit                                                  = 0;

    for (j = 1; j <= Contour.Length(); j++)
    {
      ICurveInit++;
      Cuter.Perform(occ::down_cast<Geom2d_Curve>(Contour.ChangeValue(j)));
      NbSection = 1;
      if (!Cuter.UnModified())
      {
        ico       = j;
        NbSection = Cuter.NbCurves();
        for (k = 1; k <= NbSection; k++)
        {
          Contour.InsertAfter(j, Cuter.Value(k));
          j++;
        }
        Contour.Remove(ico);
        j--;
      }
      MAT2d_BiInt B(i, ICurveInit);
      NbSect.Bind(B, NbSection);
    }
  }
}
