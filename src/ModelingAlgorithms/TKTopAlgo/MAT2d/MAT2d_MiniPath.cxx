// Created on: 1993-10-07
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

#include <Extrema_ExtCC2d.hxx>
#include <Extrema_ExtPC2d.hxx>
#include <Extrema_POnCurv2d.hxx>
#include <Geom2d_CartesianPoint.hxx>
#include <Geom2d_Point.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <MAT2d_Connexion.hxx>
#include <NCollection_Array2.hxx>
#include <MAT2d_Connexion.hxx>
#include <MAT2d_MiniPath.hxx>
#include <Standard_NotImplemented.hxx>
#include <Geom2d_Geometry.hxx>
#include <NCollection_Sequence.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Sequence.hxx>

//=================================================================================================

MAT2d_MiniPath::MAT2d_MiniPath()
    : theDirection(1.0),
      indStart(0)
{
}

//============================================================================
// function : Perform
// purpose  : Calcul du chemin reliant les differents elements de <aFigure>.
//           le chemin part de la ligne <IndStart>.
//           <Sense> = True les lignes sont orientes dans le sens trigo.
//============================================================================
void MAT2d_MiniPath::Perform(const NCollection_Sequence<NCollection_Sequence<occ::handle<Geom2d_Geometry>>>& Figure,
                             const int                    IndStart,
                             const bool                    Sense)
{

  int        i, j;
  int        NbLines = Figure.Length();
  NCollection_Array2<occ::handle<MAT2d_Connexion>> Connexion(1, NbLines, 1, NbLines);

  indStart     = IndStart;
  theDirection = 1.;
  if (Sense)
    theDirection = -1.;

  //----------------------------------------------------------------------
  // Calcul des connexions qui realisent le minimum de distance entre les
  // differents elements de la figure.
  //----------------------------------------------------------------------
  for (i = 1; i < NbLines; i++)
  {
    for (j = i + 1; j <= NbLines; j++)
    {
      Connexion(i, j) = MinimumL1L2(Figure, i, j);
      Connexion(j, i) = Connexion(i, j)->Reverse();
    }
  }

  NCollection_Sequence<int> Set1;
  NCollection_Sequence<int> Set2;
  double             DistS1S2;
  int          IndiceLine1, IndiceLine2;
  int          ISuiv = 0, MinOnSet1 = 0, MinOnSet2 = 0;
  //---------------------------------------------------------------------------
  // - 0 Set1 est initialise avec la ligne de depart.
  //     Set2 contient toutes les autres.
  //---------------------------------------------------------------------------

  Set1.Append(IndStart);

  for (i = 1; i <= NbLines; i++)
  {
    if (i != IndStart)
    {
      Set2.Append(i);
    }
  }

  //---------------------------------------------------------------------------
  // - 1 Recherche de la connexion C la plus courte entre Set1 et Set2.
  // - 2 La ligne de Set2 realisant le minimum de distance est inseree dans
  //     Set1 et supprime dans Set2.
  // - 3 Insertion de la connexion dans l ensemble des connexions.
  // - 4 Si Set2 est non vide retour en 1.
  //---------------------------------------------------------------------------

  while (!Set2.IsEmpty())
  {
    DistS1S2 = RealLast();
    for (i = 1; i <= Set1.Length(); i++)
    {
      IndiceLine1 = Set1.Value(i);
      for (j = 1; j <= Set2.Length(); j++)
      {
        IndiceLine2 = Set2.Value(j);
        if (Connexion(IndiceLine1, IndiceLine2)->Distance() < DistS1S2)
        {
          ISuiv     = j;
          DistS1S2  = Connexion(IndiceLine1, IndiceLine2)->Distance();
          MinOnSet1 = IndiceLine1;
          MinOnSet2 = IndiceLine2;
        }
      }
    }
    Set1.Append(Set2.Value(ISuiv));
    Set2.Remove(ISuiv);
    Append(Connexion(MinOnSet1, MinOnSet2));
  }

  //----------------------------------------------------------------
  // Construction du chemin en parcourant l ensemble des connexions.
  //----------------------------------------------------------------
  RunOnConnexions();
}

//============================================================================
// function : Append
// purpose  : Insertion d une nouvelle connexion dans le chemin.
//
//           Les connexions et les lignes constituent un arbre dont
//           - les noeuds sont les lignes.
//           - les connexions sont les branches.
//
//============================================================================
void MAT2d_MiniPath::Append(const occ::handle<MAT2d_Connexion>& C)
{
  occ::handle<MAT2d_Connexion> CC;

  if (!theConnexions.IsBound(C->IndexFirstLine()))
  {
    NCollection_Sequence<occ::handle<MAT2d_Connexion>> Seq;
    theConnexions.Bind(C->IndexFirstLine(), Seq);
    theConnexions(C->IndexFirstLine()).Append(C);
    theFather.Bind(C->IndexSecondLine(), C);
    return;
  }

  NCollection_Sequence<occ::handle<MAT2d_Connexion>>& Seq          = theConnexions(C->IndexFirstLine());
  int           IndexAfter   = 0;
  int           NbConnexions = Seq.Length();

  for (int i = 1; i <= NbConnexions; i++)
  {
    CC = Seq.Value(i);
    if (CC->IsAfter(C, theDirection))
    {
      IndexAfter = i;
      break;
    }
  }
  //----------------------------------------------------------------------
  // Insertion de <C> avant <IAfter>.
  // Si <IAfter> = 0 => Pas de connexions apres <C> => <C> est la
  // derniere.
  //----------------------------------------------------------------------
  if (IndexAfter == 0)
  {
    Seq.Append(C);
  }
  else
  {
    Seq.InsertBefore(IndexAfter, C);
  }
  theFather.Bind(C->IndexSecondLine(), C);
  return;
}

//============================================================================
// function : Path
// purpose  : Retour de la sequence de connexions definissant le chemin.
//============================================================================
const NCollection_Sequence<occ::handle<MAT2d_Connexion>>& MAT2d_MiniPath::Path() const
{
  return thePath;
}

//=================================================================================================

bool MAT2d_MiniPath::IsConnexionsFrom(const int i) const
{
  return (theConnexions.IsBound(i));
}

//============================================================================
// function : Connexions
// purpose  : Retour de la sequence de connexions issue de la ligne <i>.
//============================================================================
NCollection_Sequence<occ::handle<MAT2d_Connexion>>& MAT2d_MiniPath::ConnexionsFrom(const int i)
{
  return theConnexions.ChangeFind(i);
}

//=================================================================================================

bool MAT2d_MiniPath::IsRoot(const int ILine) const
{
  return (ILine == indStart);
}

//============================================================================
// function : Father
// purpose  : Retour de la premiere connexion qui arrive sur la ligne i
//============================================================================
occ::handle<MAT2d_Connexion> MAT2d_MiniPath::Father(const int ILine)
{
  return theFather.ChangeFind(ILine);
}

//============================================================================
// function : RunOnConnexions
// purpose  : Construction de <thePath> en parcourant <theConnexions>.
//============================================================================
void MAT2d_MiniPath::RunOnConnexions()
{
  int                 i;
  occ::handle<MAT2d_Connexion>          C;
  const NCollection_Sequence<occ::handle<MAT2d_Connexion>>& SC = theConnexions(indStart);

  thePath.Clear();

  for (i = 1; i <= SC.Length(); i++)
  {
    C = SC.Value(i);
    thePath.Append(C);
    ExploSons(thePath, C);
    thePath.Append(C->Reverse());
  }
}

//=================================================================================================

void MAT2d_MiniPath::ExploSons(NCollection_Sequence<occ::handle<MAT2d_Connexion>>&     CResult,
                               const occ::handle<MAT2d_Connexion>& CRef)
{
  int i;
  int Index = CRef->IndexSecondLine();

  if (!theConnexions.IsBound(Index))
    return;

  const NCollection_Sequence<occ::handle<MAT2d_Connexion>>& SC  = theConnexions(Index);
  occ::handle<MAT2d_Connexion>          CRR = CRef->Reverse();
  occ::handle<MAT2d_Connexion>          C;

  for (i = 1; i <= SC.Length(); i++)
  {
    C = SC.Value(i);
    if (C->IsAfter(CRR, theDirection))
    {
      CResult.Append(C);
      ExploSons(CResult, C);
      CResult.Append(C->Reverse());
    }
  }

  for (i = 1; i <= SC.Length(); i++)
  {
    C = SC.Value(i);
    if (!C->IsAfter(CRR, theDirection))
    {
      CResult.Append(C);
      ExploSons(CResult, C);
      CResult.Append(C->Reverse());
    }
    else
    {
      break;
    }
  }
}

//============================================================================
// function : MinimumL1L2
// purpose  : Calcul de la connexion realisant le minimum de distance entre les
//           lignes d indice <IL1> et <IL2> dans <Figure>.
//============================================================================
occ::handle<MAT2d_Connexion> MAT2d_MiniPath::MinimumL1L2(
  const NCollection_Sequence<NCollection_Sequence<occ::handle<Geom2d_Geometry>>>& Figure,
  const int                    IL1,
  const int                    IL2) const
{
  Extrema_POnCurv2d             PointOnCurv1, PointOnCurv2;
  int              IC1, IC2, IMinC1 = 0, IMinC2 = 0, i;
  double                 DistL1L2_2, DistP1P2_2;
  double                 ParameterOnC1 = 0., ParameterOnC2 = 0.;
  NCollection_Sequence<occ::handle<Geom2d_Geometry>> L1, L2;
  gp_Pnt2d                      Point1, Point2, P1, P2;
  occ::handle<Geom2d_Curve>          Item1;
  occ::handle<Geom2d_Curve>          Item2;

  L1 = Figure.Value(IL1);
  L2 = Figure.Value(IL2);

  DistL1L2_2 = RealLast();

  //---------------------------------------------------------------------------
  // Calcul des extremas de distances entre les composants de L1 et de L2.
  //---------------------------------------------------------------------------

  for (IC1 = 1; IC1 <= L1.Length(); IC1++)
  {

    occ::handle<Standard_Type> Type1 = L1.Value(IC1)->DynamicType();
    if (Type1 != STANDARD_TYPE(Geom2d_CartesianPoint))
    {
      Item1 = occ::down_cast<Geom2d_Curve>(L1.Value(IC1));
    }
    else
    {
      P1 = occ::down_cast<Geom2d_Point>(L1.Value(IC1))->Pnt2d();
    }

    for (IC2 = 1; IC2 <= L2.Length(); IC2++)
    {

      occ::handle<Standard_Type> Type2 = L2.Value(IC2)->DynamicType();
      if (Type2 != STANDARD_TYPE(Geom2d_CartesianPoint))
      {
        Item2 = occ::down_cast<Geom2d_Curve>(L2.Value(IC2));
      }
      else
      {
        P2 = occ::down_cast<Geom2d_Point>(L2.Value(IC2))->Pnt2d();
      }

      if (Type1 == STANDARD_TYPE(Geom2d_CartesianPoint)
          && Type2 == STANDARD_TYPE(Geom2d_CartesianPoint))
      {
        DistP1P2_2 = P1.SquareDistance(P2);
        if (DistP1P2_2 <= DistL1L2_2)
        {
          DistL1L2_2    = DistP1P2_2;
          IMinC1        = IC1;
          IMinC2        = IC2;
          Point1        = P1;
          Point2        = P2;
          ParameterOnC1 = 0.;
          ParameterOnC2 = 0.;
        }
      }
      else if (Type1 == STANDARD_TYPE(Geom2d_CartesianPoint))
      {
        Geom2dAdaptor_Curve C2(Item2);
        Extrema_ExtPC2d     Extremas(P1, C2);
        if (Extremas.IsDone())
        {
          for (i = 1; i <= Extremas.NbExt(); i++)
          {
            if (Extremas.SquareDistance(i) < DistL1L2_2)
            {
              DistL1L2_2    = Extremas.SquareDistance(i);
              IMinC1        = IC1;
              IMinC2        = IC2;
              PointOnCurv2  = Extremas.Point(i);
              ParameterOnC1 = 0.;
              ParameterOnC2 = PointOnCurv2.Parameter();
              Point1        = P1;
              Point2        = PointOnCurv2.Value();
            }
          }
        }
      }
      else if (Type2 == STANDARD_TYPE(Geom2d_CartesianPoint))
      {
        Geom2dAdaptor_Curve C1(Item1);
        Extrema_ExtPC2d     Extremas(P2, C1);
        if (Extremas.IsDone())
        {
          for (i = 1; i <= Extremas.NbExt(); i++)
          {
            if (Extremas.SquareDistance(i) < DistL1L2_2)
            {
              DistL1L2_2    = Extremas.SquareDistance(i);
              IMinC1        = IC1;
              IMinC2        = IC2;
              PointOnCurv1  = Extremas.Point(i);
              ParameterOnC2 = 0.;
              ParameterOnC1 = PointOnCurv1.Parameter();
              Point1        = PointOnCurv1.Value();
              Point2        = P2;
            }
          }
        }
      }
      else
      {
        Geom2dAdaptor_Curve C1(Item1);
        Geom2dAdaptor_Curve C2(Item2);
        Extrema_ExtCC2d     Extremas(C1, C2);
        if (!Extremas.IsParallel() && Extremas.IsDone())
        {
          for (i = 1; i <= Extremas.NbExt(); i++)
          {
            if (Extremas.SquareDistance(i) < DistL1L2_2)
            {
              DistL1L2_2 = Extremas.SquareDistance(i);
              IMinC1     = IC1;
              IMinC2     = IC2;
              Extremas.Points(i, PointOnCurv1, PointOnCurv2);
              ParameterOnC1 = PointOnCurv1.Parameter();
              ParameterOnC2 = PointOnCurv2.Parameter();
              Point1        = PointOnCurv1.Value();
              Point2        = PointOnCurv2.Value();
            }
          }
        }
      }
    }
  }
  occ::handle<MAT2d_Connexion> ConnexionL1L2;
  ConnexionL1L2 = new MAT2d_Connexion(IL1,
                                      IL2,
                                      IMinC1,
                                      IMinC2,
                                      sqrt(DistL1L2_2),
                                      ParameterOnC1,
                                      ParameterOnC2,
                                      Point1,
                                      Point2);
  return ConnexionL1L2;
}
