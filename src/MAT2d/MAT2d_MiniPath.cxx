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
#include <Geom2d_Curve.hxx>
#include <Geom2d_Point.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <MAT2d_Array2OfConnexion.hxx>
#include <MAT2d_Connexion.hxx>
#include <MAT2d_MiniPath.hxx>
#include <Standard_NotImplemented.hxx>
#include <TColGeom2d_SequenceOfGeometry.hxx>
#include <TColStd_SequenceOfInteger.hxx>

//============================================================================
//function : MAT2d_MiniPath()
//purpose  :
//============================================================================
MAT2d_MiniPath::MAT2d_MiniPath()
{
}

//============================================================================
//function : Perform
//purpose  : Calcul du chemin reliant les differents elements de <aFigure>.
//           le chemin part de la ligne <IndStart>.
//           <Sense> = True les lignes sont orientes dans le sens trigo.
//============================================================================
void MAT2d_MiniPath::Perform
  (const MAT2d_SequenceOfSequenceOfGeometry& Figure,
   const Standard_Integer                    IndStart,
   const Standard_Boolean                    Sense)
{

  Standard_Integer         i,j;
  Standard_Integer         NbLines   = Figure.Length();
  MAT2d_Array2OfConnexion  Connexion (1,NbLines,1,NbLines);
  
  indStart     = IndStart;
  theDirection = 1.;
  if (Sense) theDirection = -1.;

  //----------------------------------------------------------------------
  // Calcul des connexions qui realisent le minimum de distance entre les
  // differents elements de la figure.
  //----------------------------------------------------------------------
  for (i = 1; i < NbLines; i++) {
    for (j =i+1; j <= NbLines; j++){
      Connexion(i,j) = MinimumL1L2(Figure,i,j);
      Connexion(j,i) = Connexion(i,j)->Reverse();
    }
  }
 
  TColStd_SequenceOfInteger Set1;
  TColStd_SequenceOfInteger Set2;
  Standard_Real             DistS1S2;
  Standard_Integer          IndiceLine1,IndiceLine2;
  Standard_Integer          ISuiv =0,MinOnSet1 =0,MinOnSet2 =0;
  //---------------------------------------------------------------------------
  // - 0 Set1 est initialise avec la ligne de depart.
  //     Set2 contient toutes les autres.
  //---------------------------------------------------------------------------

  Set1.Append(IndStart);

  for (i=1 ; i<=NbLines ; i++){
    if (i != IndStart){
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

  while (!Set2.IsEmpty()){
    DistS1S2 = RealLast();
    for (i = 1; i <= Set1.Length(); i++) {
      IndiceLine1 = Set1.Value(i);
      for (j = 1 ; j<= Set2.Length() ;j++) {
	IndiceLine2 = Set2.Value(j);
	if(Connexion(IndiceLine1,IndiceLine2)->Distance() < DistS1S2) {
	  ISuiv     = j;
	  DistS1S2  = Connexion(IndiceLine1,IndiceLine2)->Distance();
	  MinOnSet1 = IndiceLine1;
	  MinOnSet2 = IndiceLine2;
	}
      }
    }
    Set1.Append(Set2.Value(ISuiv)); 
    Set2.Remove(ISuiv);
    Append(Connexion(MinOnSet1,MinOnSet2));
  }

  //----------------------------------------------------------------
  // Construction du chemin en parcourant l ensemble des connexions.
  //----------------------------------------------------------------
  RunOnConnexions() ;
}


//============================================================================
//function : Append
//purpose  : Insertion d une nouvelle connexion dans le chemin.
//
//           Les connexions et les lignes constituent un arbre dont
//           - les noeuds sont les lignes.
//           - les connexions sont les branches.
//         
//============================================================================
void MAT2d_MiniPath::Append(const Handle(MAT2d_Connexion)& C)
{
  Handle(MAT2d_Connexion) CC;

  if (!theConnexions.IsBound(C->IndexFirstLine())) {
    MAT2d_SequenceOfConnexion Seq;
    theConnexions.Bind(C->IndexFirstLine(),Seq);
    theConnexions(C->IndexFirstLine()).Append(C);
    theFather.Bind(C->IndexSecondLine(),C);
    return;
  }

  MAT2d_SequenceOfConnexion& Seq  = theConnexions(C->IndexFirstLine());	
  Standard_Integer IndexAfter     = 0;
  Standard_Integer NbConnexions   = Seq.Length();

  for (Standard_Integer i = 1; i <= NbConnexions; i++) {
    CC = Seq.Value(i);
    if (CC->IsAfter(C,theDirection)){
      IndexAfter = i;
      break;
    }
  }
  //----------------------------------------------------------------------
  // Insertion de <C> avant <IAfter>.
  // Si <IAfter> = 0 => Pas de connexions apres <C> => <C> est la
  // derniere.
  //----------------------------------------------------------------------
  if (IndexAfter == 0) {
    Seq.Append(C);
  }
  else {
    Seq.InsertBefore(IndexAfter,C);
  }
  theFather.Bind(C->IndexSecondLine(),C);
  return;
}

//============================================================================
//function : Path
//purpose  : Retour de la sequence de connexions definissant le chemin.
//============================================================================
const MAT2d_SequenceOfConnexion& MAT2d_MiniPath::Path() const
{
  return thePath;
}

//============================================================================
//function : IsConnexionsFrom
//purpose :
//============================================================================
Standard_Boolean MAT2d_MiniPath::IsConnexionsFrom
  (const Standard_Integer i) const
{
  return (theConnexions.IsBound(i));
}

//============================================================================
//function : Connexions
//purpose  : Retour de la sequence de connexions issue de la ligne <i>.
//============================================================================
MAT2d_SequenceOfConnexion& MAT2d_MiniPath::ConnexionsFrom 
  (const Standard_Integer i)
{
  return theConnexions.ChangeFind(i);
}

//============================================================================
//function : IsRoot
//purpose  :
//============================================================================
Standard_Boolean MAT2d_MiniPath::IsRoot(const Standard_Integer ILine) const
{
  return (ILine == indStart);
}

//============================================================================
//function : Father
//purpose  : Retour de la premiere connexion qui arrive sur la ligne i
//============================================================================
Handle(MAT2d_Connexion) MAT2d_MiniPath::Father(const Standard_Integer ILine)
{
  return theFather.ChangeFind(ILine);
}


//============================================================================
//function : RunOnConnexions
//purpose  : Construction de <thePath> en parcourant <theConnexions>.
//============================================================================
void MAT2d_MiniPath::RunOnConnexions() 
{
  Standard_Integer                  i;
  Handle(MAT2d_Connexion)           C;
  const MAT2d_SequenceOfConnexion&  SC = theConnexions(indStart);

  thePath.Clear();
  
  for ( i = 1; i <= SC.Length(); i++) {
    C = SC.Value(i);
    thePath.Append(C);
    ExploSons(thePath,C);
    thePath.Append(C->Reverse());
  }
}

//============================================================================
//function : ExploSons
//purpose  : 
//============================================================================
void MAT2d_MiniPath::ExploSons(      MAT2d_SequenceOfConnexion& CResult,
			       const Handle(MAT2d_Connexion)&   CRef   ) 
{
  Standard_Integer                 i;  
  Standard_Integer                 Index = CRef->IndexSecondLine();

  if (!theConnexions.IsBound(Index)) return;

  const MAT2d_SequenceOfConnexion& SC    = theConnexions(Index);
  Handle(MAT2d_Connexion)          CRR   = CRef->Reverse();
  Handle(MAT2d_Connexion)          C;

  for ( i = 1; i <= SC.Length(); i++) {
    C = SC.Value(i);
    if (C->IsAfter(CRR,theDirection)) {
      CResult.Append(C);
      ExploSons(CResult,C);
      CResult.Append(C->Reverse());
    }
  }

  for ( i = 1; i <= SC.Length(); i++) {
    C = SC.Value(i);
    if (!C->IsAfter(CRR,theDirection)) {
      CResult.Append(C);
      ExploSons(CResult,C);
      CResult.Append(C->Reverse());
    }
    else {
      break;
    }
  }
}

      
//============================================================================
//function : MinimumL1L2
//purpose  : Calcul de la connexion realisant le minimum de distance entre les
//           lignes d indice <IL1> et <IL2> dans <Figure>.
//============================================================================
Handle(MAT2d_Connexion) MAT2d_MiniPath::MinimumL1L2
  (const MAT2d_SequenceOfSequenceOfGeometry& Figure,
   const Standard_Integer                    IL1,
   const Standard_Integer                    IL2) const
{
  Extrema_POnCurv2d              PointOnCurv1,PointOnCurv2;
  Standard_Integer               IC1,IC2,IMinC1 =0,IMinC2 =0,i;
  Standard_Real                  DistL1L2_2,DistP1P2_2;
  Standard_Real                  ParameterOnC1 =0.,ParameterOnC2 =0.;
  TColGeom2d_SequenceOfGeometry  L1,L2;
  gp_Pnt2d                       Point1,Point2,P1,P2;
  Handle(Geom2d_Curve)           Item1;
  Handle(Geom2d_Curve)           Item2;

  L1 = Figure.Value(IL1);
  L2 = Figure.Value(IL2);

  DistL1L2_2 = RealLast();

  //---------------------------------------------------------------------------
  // Calcul des extremas de distances entre les composants de L1 et de L2.
  //---------------------------------------------------------------------------

  for (IC1 = 1; IC1 <= L1.Length(); IC1++) {

    Handle(Standard_Type)   Type1 = L1.Value(IC1)->DynamicType();
    if (Type1 != STANDARD_TYPE(Geom2d_CartesianPoint)) {
      Item1 = Handle(Geom2d_Curve)::DownCast(L1.Value(IC1));
    }
    else {
	P1 = Handle(Geom2d_Point)::DownCast(L1.Value(IC1))->Pnt2d();
    }

    for (IC2 = 1; IC2 <= L2.Length(); IC2++) {

      Handle(Standard_Type)   Type2 = L2.Value(IC2)->DynamicType();
      if (Type2 != STANDARD_TYPE(Geom2d_CartesianPoint)) {
	Item2 = Handle(Geom2d_Curve)::DownCast(L2.Value(IC2));
      }
      else {
	P2 = Handle(Geom2d_Point)::DownCast(L2.Value(IC2))->Pnt2d();
      }

      if (Type1 == STANDARD_TYPE(Geom2d_CartesianPoint) &&
	  Type2 == STANDARD_TYPE(Geom2d_CartesianPoint)   ) {
	DistP1P2_2 = P1.SquareDistance(P2);
	if (DistP1P2_2 <= DistL1L2_2) {
	  DistL1L2_2      = DistP1P2_2;
	  IMinC1        = IC1;
	  IMinC2        = IC2;
	  Point1        = P1;
	  Point2        = P2;
	  ParameterOnC1 = 0.;
	  ParameterOnC2 = 0.;
	}
      }
      else if (Type1 == STANDARD_TYPE(Geom2d_CartesianPoint)) {
	Geom2dAdaptor_Curve C2(Item2);
	Extrema_ExtPC2d Extremas(P1,C2);
	if (Extremas.IsDone()){
	  for (i = 1; i <= Extremas.NbExt(); i++) {
	    if (Extremas.SquareDistance(i) < DistL1L2_2) {
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
      else if (Type2 == STANDARD_TYPE(Geom2d_CartesianPoint)) {
	Geom2dAdaptor_Curve C1(Item1);
	Extrema_ExtPC2d Extremas(P2,C1);
	if (Extremas.IsDone()){
	  for (i=1;i<=Extremas.NbExt();i++) {
	    if (Extremas.SquareDistance(i) < DistL1L2_2) {
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
      else {
	Geom2dAdaptor_Curve C1(Item1);
	Geom2dAdaptor_Curve C2(Item2);
	Extrema_ExtCC2d Extremas(C1,C2);
	if (!Extremas.IsParallel() && Extremas.IsDone()){
	  for ( i=1; i <= Extremas.NbExt(); i++) {
	    if (Extremas.SquareDistance(i) < DistL1L2_2) {
	      DistL1L2_2    = Extremas.SquareDistance(i);
	      IMinC1        = IC1;
	      IMinC2        = IC2;
	      Extremas.Points(i,PointOnCurv1,PointOnCurv2);
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
  Handle(MAT2d_Connexion) ConnexionL1L2;
  ConnexionL1L2 = new MAT2d_Connexion(IL1,IL2,IMinC1,IMinC2,sqrt (DistL1L2_2),
				      ParameterOnC1,ParameterOnC2,
				      Point1,Point2);
  return ConnexionL1L2;
}























