// Created on: 1993-11-19
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

#include <Adaptor2d_OffsetCurve.hxx>
#include <Geom2d_CartesianPoint.hxx>
#include <Geom2d_Geometry.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor_HCurve.hxx>
#include <Geom2dInt_GInter.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <MAT2d_BiInt.hxx>
#include <MAT2d_Circuit.hxx>
#include <MAT2d_Connexion.hxx>
#include <MAT2d_DataMapIteratorOfDataMapOfBiIntSequenceOfInteger.hxx>
#include <MAT2d_DataMapOfBiIntSequenceOfInteger.hxx>
#include <MAT2d_DataMapOfIntegerConnexion.hxx>
#include <MAT2d_MiniPath.hxx>
#include <MAT2d_SequenceOfConnexion.hxx>
#include <MAT2d_SequenceOfSequenceOfGeometry.hxx>
#include <Precision.hxx>
#include <Standard_Type.hxx>
#include <TColStd_Array1OfBoolean.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_SequenceOfInteger.hxx>

IMPLEMENT_STANDARD_RTTIEXT(MAT2d_Circuit,Standard_Transient)

#ifdef OCCT_DEBUG
#include <GCE2d_MakeSegment.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2d_CartesianPoint.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_Circle.hxx>
#endif

#ifdef DRAW
#include <Draw_Appli.hxx>
#include <DrawTrSurf_Curve2d.hxx>
#include <Draw_Marker2D.hxx>
  static Handle(DrawTrSurf_Curve2d) draw;
  Standard_EXPORT Draw_Viewer dout;
#endif
#ifdef OCCT_DEBUG
  static void MAT2d_DrawCurve(const Handle(Geom2d_Curve)& aCurve,
			      const Standard_Integer      Indice);
  static Standard_Boolean AffichCircuit = 0;
#endif

// static functions:

static Standard_Real CrossProd(const Handle(Geom2d_Geometry)& Geom1,
			       const Handle(Geom2d_Geometry)& Geom2,
			             Standard_Real&           DotProd);


//=============================================================================
//function : Constructor
//purpose :
//=============================================================================
MAT2d_Circuit::MAT2d_Circuit(const GeomAbs_JoinType aJoinType,
                             const Standard_Boolean IsOpenResult)
{
  myJoinType = aJoinType;
  myIsOpenResult = IsOpenResult;
}

//=============================================================================
//function : Perform
//purpose :
//=============================================================================
void  MAT2d_Circuit::Perform
  (       MAT2d_SequenceOfSequenceOfGeometry& FigItem,
   const TColStd_SequenceOfBoolean          & IsClosed,
   const Standard_Integer                     IndRefLine,
   const Standard_Boolean                     Trigo)
{
  Standard_Integer          NbLines = FigItem.Length();
  Standard_Integer          i;
  TColStd_Array1OfBoolean   Open(1,NbLines);
  MAT2d_SequenceOfConnexion SVide;
  Handle(MAT2d_Connexion)   ConnexionNul;

  if (Trigo) direction = 1.; else direction = -1.;

  //---------------------
  // Reinitialisation SD.
  //---------------------
  geomElements.Clear();
  connexionMap.Clear();
  linkRefEqui.Clear();
  linesLength.Clear();

  //----------------------------
  // Detection Lignes ouvertes.
  //----------------------------
  for ( i = 1; i <= NbLines; i++) {    
    Handle(Geom2d_TrimmedCurve) Curve;
    Curve = Handle(Geom2d_TrimmedCurve)::DownCast(FigItem.Value(i).First());
    gp_Pnt2d P1 = Curve->StartPoint();  
    Curve = Handle(Geom2d_TrimmedCurve)::DownCast(FigItem.Value(i).Last());
    gp_Pnt2d P2 = Curve->EndPoint();
//  Modified by Sergey KHROMOV - Wed Mar  6 16:59:01 2002 Begin
//     if ( P1.IsEqual(P2,Precision::Confusion()))  Open(i) = Standard_False;
//     else                                         Open(i) = Standard_True;
    if (IsClosed(i))                                Open(i) = Standard_False;
    else if (P1.IsEqual(P2,Precision::Confusion())) Open(i) = Standard_False;
    else                                            Open(i) = Standard_True;
//  Modified by Sergey KHROMOV - Wed Mar  6 16:59:04 2002 End
  }
    
  //---------------------------------------------------------------
  // Insertion des cassures saillantes ou
  // ajout des extremites de chaque courbe si la ligne est ouverte.
  //---------------------------------------------------------------
  for ( i = 1; i <= NbLines; i++) {
    if (Open(i)) {
      InitOpen(FigItem.ChangeValue(i));  
      linesLength.Append(FigItem.Value(i).Length());
    }
    else {
      InsertCorner(FigItem.ChangeValue(i));
      linesLength.Append(FigItem.Value(i).Length());
    }
  }

  //---------------------------------
  // Une seule ligne => Rien a faire.
  //---------------------------------
  if (NbLines == 1) {
    if (Open(1)) {
      DoubleLine(FigItem.ChangeValue(1),SVide,ConnexionNul,direction);  
      linesLength.SetValue(1,FigItem.Value(1).Length());
    }
    geomElements  = FigItem.Value(1);
    UpDateLink(1,1,1,geomElements.Length());     
    linesLength.Append(FigItem.Value(1).Length());
    return;
  }
    
  //------------------
  // Plusieurs lignes.
  //------------------
  
  //---------------------------------------------------------
  // Calcul de l ensemble des connexions realisant le chemin. 
  //---------------------------------------------------------
  MAT2d_MiniPath  Road;
  Road.Perform(FigItem,IndRefLine,Trigo);
  
  //------------------------
  // Fermeture ligne ouverte.
  //-------------------------
  for ( i = 1; i <= NbLines; i++) {
    if (Open(i)) {
      Handle(MAT2d_Connexion)  CF;
      if (Road.IsRoot(i))      CF = ConnexionNul; else CF = Road.Father(i);
      if (Road.IsConnexionsFrom(i)) {
	DoubleLine(FigItem.ChangeValue(i),Road.ConnexionsFrom(i),
		   CF,direction);  
      }
      else {
	DoubleLine(FigItem.ChangeValue(i),SVide,CF,direction);  
      }
      linesLength.SetValue(i,FigItem.Value(i).Length());
    }
  }

  //------------------------
  // Construction du chemin.
  //------------------------
  Road.RunOnConnexions();

#ifdef OCCT_DEBUG
  if (AffichCircuit) {
    Standard_Integer NbConnexions = Road.Path().Length();
    for (i = 1; i <= NbConnexions; i++) {
      Handle(Geom2d_TrimmedCurve) edge;
      edge = GCE2d_MakeSegment(Road.Path().Value(i)->PointOnFirst(),
			       Road.Path().Value(i)->PointOnSecond());
      MAT2d_DrawCurve(edge,2);
    }
  }
#endif

  //-------------------------
  // Construction du Circuit.
  //-------------------------
  ConstructCircuit(FigItem,IndRefLine,Road);
}

//=======================================================================
//function : IsSharpCorner
//purpose  : Return True Si le point commun entre <Geom1> et <Geom2> est 
//           une cassure saillante par rapport <Direction>
//=======================================================================

Standard_Boolean MAT2d_Circuit::IsSharpCorner(const Handle(Geom2d_Geometry)& Geom1,
                                              const Handle(Geom2d_Geometry)& Geom2,
                                              const Standard_Real Direction) const
{
  Standard_Real    DotProd;
  Standard_Real    ProVec = CrossProd (Geom1,Geom2,DotProd);
  Standard_Integer NbTest = 1;
  Standard_Real    DU = Precision::Confusion();
  Handle(Geom2d_TrimmedCurve) C1,C2;

  C1= Handle(Geom2d_TrimmedCurve)::DownCast(Geom1);
  C2= Handle(Geom2d_TrimmedCurve)::DownCast(Geom2);
//  Modified by Sergey KHROMOV - Thu Oct 24 19:02:46 2002 Begin
// Add the same criterion as it is in MAT2d_Circuit::InitOpen(..)
//  Standard_Real  TolAng = 1.E-5;
  Standard_Real  TolAng = 1.E-8;
//  Modified by Sergey KHROMOV - Thu Oct 24 19:02:47 2002 End

  if (myJoinType == GeomAbs_Arc)
  {
    while (NbTest <= 10) {
      if      ((ProVec)*Direction < -TolAng)                 
        return Standard_True;                // Saillant.
      if      ((ProVec)*Direction >  TolAng)
        return Standard_False;              // Rentrant.
      else { 
        if (DotProd > 0) {
          return Standard_False;            // Plat.
        }
        TolAng = 1.E-8;
        Standard_Real U1 = C1->LastParameter()  - NbTest*DU;
        Standard_Real U2 = C2->FirstParameter() + NbTest*DU;
        gp_Dir2d Dir1(C1->DN(U1,1));
        gp_Dir2d Dir2(C2->DN(U2,1));
        DotProd = Dir1.Dot(Dir2);
        ProVec  =  Dir1^Dir2;
        NbTest++;
      } 
    }
    
    
    
    // Rebroussement.
    // on calculde des paralleles aux deux courbes du cote du domaine
    // de calcul
    // Si pas dintersection => saillant.
    // Sinon                => rentrant.
    Standard_Real D ;
    Standard_Real Tol   = Precision::Confusion();
    Standard_Real MilC1 = (C1->LastParameter() + C1->FirstParameter())*0.5;
    Standard_Real MilC2 = (C2->LastParameter() + C2->FirstParameter())*0.5;
    gp_Pnt2d      P     = C1->Value(C1->LastParameter());
    gp_Pnt2d      P1    = C1->Value(MilC1);
    gp_Pnt2d      P2    = C2->Value(MilC2);
    
    D = Min(P1.Distance(P),P2.Distance(P));
    D /= 10;
    
    if (Direction > 0.) D = -D;
    
    Handle(Geom2dAdaptor_HCurve) HC1 = new Geom2dAdaptor_HCurve(C1);
    Handle(Geom2dAdaptor_HCurve) HC2 = new Geom2dAdaptor_HCurve(C2);
    Adaptor2d_OffsetCurve OC1(HC1,D,MilC1,C1->LastParameter());
    Adaptor2d_OffsetCurve OC2(HC2,D,C2->FirstParameter(),MilC2);
    Geom2dInt_GInter Intersect; 
    Intersect.Perform(OC1,OC2,Tol,Tol);
    
#ifdef OCCT_DEBUG
    static Standard_Boolean Affich = 0;
    if (Affich) {
#ifdef DRAW
      Standard_Real DU1 = (OC1.LastParameter() - OC1.FirstParameter())/9.;
      Standard_Real DU2 = (OC2.LastParameter() - OC2.FirstParameter())/9.;
      for (Standard_Integer ki = 0; ki <= 9; ki++) {
        gp_Pnt2d P1 = OC1.Value(OC1.FirstParameter()+ki*DU1);
        gp_Pnt2d P2 = OC2.Value(OC2.FirstParameter()+ki*DU2);
        Handle(Draw_Marker2D) dr1 = new Draw_Marker2D(P1,Draw_Plus,Draw_vert);
        Handle(Draw_Marker2D) dr2 = new Draw_Marker2D(P2,Draw_Plus,Draw_rouge); 
        dout << dr1;
        dout << dr2;
      }
      dout.Flush();
#endif
    }
#endif
    
    if (Intersect.IsDone() && !Intersect.IsEmpty()) {
      return Standard_False;
    }
    else {
      return Standard_True;
    }
  } //end of if (myJoinType == GeomAbs_Arc)
  else if (myJoinType == GeomAbs_Intersection)
  {
    if (Abs(ProVec) <= TolAng &&
        DotProd < 0)
    {
      while (NbTest <= 10)
      {
        Standard_Real U1 = C1->LastParameter()  - NbTest*DU;
        Standard_Real U2 = C2->FirstParameter() + NbTest*DU;
        gp_Dir2d Dir1(C1->DN(U1,1));
        gp_Dir2d Dir2(C2->DN(U2,1));
        DotProd = Dir1.Dot(Dir2);
        ProVec  =  Dir1^Dir2;
        if      ((ProVec)*Direction < -TolAng)                 
          return Standard_True;                // Saillant.
        if      ((ProVec)*Direction >  TolAng)
          return Standard_False;              // Rentrant.
        
        NbTest++;
      }
      return Standard_False;
    }
    else
      return Standard_False;
  }
  return Standard_False;
}

//=======================================================================
//function : SubSequence
//purpose  : 
//=======================================================================
static void SubSequence(const TColGeom2d_SequenceOfGeometry& S1,
			      Standard_Integer               IF,
			      Standard_Integer               IL,
			      TColGeom2d_SequenceOfGeometry& S2)
{
  S2.Clear();
  for (Standard_Integer i = IF; i<= IL; i++){
    S2.Append(S1.Value(i));
  }
}

			
//=============================================================================
//function : ConstructCircuit
//purpose :
//=============================================================================
void  MAT2d_Circuit::ConstructCircuit
  (const MAT2d_SequenceOfSequenceOfGeometry& FigItem, 
   const Standard_Integer                    IndRefLine,
   const MAT2d_MiniPath&                     Road)
{
  Handle(MAT2d_Connexion)            PrevC,CurC;
  TColGeom2d_SequenceOfGeometry      SetOfItem;
  Standard_Integer                   NbConnexions;
  Standard_Integer                   ILastItem;
  Standard_Integer                   IndLast;
  Standard_Integer                   i;
  
  NbConnexions = Road.Path().Length();
  //-----------------------------------------------------
  // Depart du premier element de la ligne de reference.
  //-----------------------------------------------------
  PrevC = Road.Path().Value(1);
  SubSequence(FigItem.Value(IndRefLine),
	      1,
	      PrevC->IndexItemOnFirst(),
	      geomElements);
  UpDateLink(1,IndRefLine,1,PrevC->IndexItemOnFirst());
  connexionMap.Bind(geomElements.Length()+1,PrevC);
  ILastItem = geomElements.Length();
  
  //-----------------------------------------------------------------------
  // Ajout des portion de lignes delimites par deux connexions successives.
  //-----------------------------------------------------------------------
  for ( i = 2; i <= NbConnexions; i++) {
    CurC = Road.Path().Value(i);
    if (PassByLast(PrevC,CurC)) {
      //------------------------------------------------------
      // La portion passe par le dernier element de la ligne.
      // - ajout de la portion de PrevC au dernier element 
      //   de la ligne.
      // - Si la ligne contient plus d'un element ajout de la 
      //   portion du premier element de la ligne a CurC.
      //------------------------------------------------------
      IndLast = FigItem.Value(CurC->IndexFirstLine()).Length();
      SubSequence (FigItem.Value(CurC->IndexFirstLine()),
		   PrevC->IndexItemOnSecond(),
		   IndLast,
		   SetOfItem);
      UpDateLink(ILastItem+1,CurC->IndexFirstLine(),
		 PrevC->IndexItemOnSecond(),IndLast);
      geomElements.Append(SetOfItem);
      ILastItem = geomElements.Length();

      if (FigItem.Value(CurC->IndexFirstLine()).Length() > 1) {
	SubSequence(FigItem.Value(CurC->IndexFirstLine()),
		    1,
		    CurC->IndexItemOnFirst(),
		    SetOfItem);
	UpDateLink(ILastItem+1,CurC->IndexFirstLine(),
		   1,CurC->IndexItemOnFirst());
	geomElements.Append(SetOfItem);
	ILastItem = geomElements.Length();
      }
      connexionMap.Bind(ILastItem+1,CurC);
    }
    else{     

      //------------------------------------------------------
      // La portion ne passe par le dernier element de la ligne.
      //------------------------------------------------------
      SubSequence(FigItem.Value(CurC->IndexFirstLine()),
		  PrevC->IndexItemOnSecond(),
		  CurC ->IndexItemOnFirst(),
		  SetOfItem);
      UpDateLink(ILastItem+1,CurC->IndexFirstLine(),
		 PrevC->IndexItemOnSecond(),CurC->IndexItemOnFirst());
      geomElements.Append(SetOfItem);
      ILastItem = geomElements.Length();
      connexionMap.Bind(ILastItem+1,CurC);
    }
    PrevC = CurC;
  }

  //-------------------------------------------------------------
  // Fermeture : de la derniere connexion au dernier element de la
  //             ligne de reference.
  //-------------------------------------------------------------
  IndLast = FigItem.Value(IndRefLine).Length();
  if (IndLast == 1) {
    connexionMap.Bind(1,CurC);
    connexionMap.UnBind(ILastItem+1);
  }
  else {
    SubSequence(FigItem.Value(IndRefLine),
		PrevC->IndexItemOnSecond(),
		IndLast,
		SetOfItem);
    UpDateLink(ILastItem+1,IndRefLine,PrevC->IndexItemOnSecond(),IndLast);
    geomElements.Append(SetOfItem);
    ILastItem = geomElements.Length();
  }

  //--------------------------------------
  // Tri des RefToEqui pour chaque element.
  //--------------------------------------
  MAT2d_DataMapIteratorOfDataMapOfBiIntSequenceOfInteger Ite;

  for ( Ite.Initialize(linkRefEqui); Ite.More(); Ite.Next()) {
    if (Ite.Value().Length() > 1) {
      SortRefToEqui(Ite.Key());
    }
  }

#ifdef OCCT_DEBUG
  if (AffichCircuit) {
    ILastItem = geomElements.Length();
    for (i = 1; i <= ILastItem; i++) {
      if (geomElements.Value(i)->DynamicType() != STANDARD_TYPE(Geom2d_CartesianPoint) ){
	MAT2d_DrawCurve
	  (Handle(Geom2d_Curve)::DownCast(geomElements.Value(i)),2);
      }
    }
  }
#endif
}

//=============================================================================
//function : InitOpen
//purpose  :
//=============================================================================
void MAT2d_Circuit::InitOpen (TColGeom2d_SequenceOfGeometry& Line) const 
{ 
  Handle(Geom2d_TrimmedCurve) Curve;
  Standard_Real               DotProd;

  Curve = Handle(Geom2d_TrimmedCurve)::DownCast(Line.First());
  Line.InsertBefore(1,new Geom2d_CartesianPoint(Curve->StartPoint()));
  Curve = Handle(Geom2d_TrimmedCurve)::DownCast(Line.Last());
  Line.Append(new Geom2d_CartesianPoint(Curve->EndPoint()));
  
  for ( Standard_Integer i = 2; i <= Line.Length() - 2; i++) {
    if ( Abs(CrossProd(Line.Value(i),Line.Value(i+1),DotProd)) > 1.E-8 ||
	 DotProd < 0. ) {
      Curve = Handle(Geom2d_TrimmedCurve)::DownCast(Line.Value(i));
      Line.InsertAfter(i,new Geom2d_CartesianPoint(Curve->EndPoint()));
      i++;
    }
  }
}

//=============================================================================
//function : DoubleLine
//purpose  :
//=============================================================================
void MAT2d_Circuit::DoubleLine 
  (      TColGeom2d_SequenceOfGeometry& Line,
         MAT2d_SequenceOfConnexion&     ConnexionFrom,
   const Handle(MAT2d_Connexion)&       ConnexionFather,
   const Standard_Real                  SideRef)
const
{ 
  Handle(Standard_Type)          Type;
  Handle(Geom2d_TrimmedCurve)    Curve;
  Standard_Integer               NbItems = Line.Length();
  Standard_Integer               i;
  Standard_Real                  ProVec,DotProd;
  Handle(MAT2d_Connexion)        CC;

  //--------------------------
  // Completion de la ligne.
  //--------------------------
  if (!myIsOpenResult)
  {
    for ( i = NbItems - 1; i > 1; i--){
      Type = Line.Value(i)->DynamicType();
      if ( Type == STANDARD_TYPE(Geom2d_CartesianPoint) ){
        Line.Append(Line.Value(i));
      }
      else {
        Curve = Handle(Geom2d_TrimmedCurve)::DownCast(Line.Value(i)->Copy());
        Curve->Reverse();
        Line.Append(Curve);
      }
    }
  }

  //------------------------------------------
  // Repartition des connexions sur la ligne
  //------------------------------------------
  Standard_Integer IAfter       = ConnexionFrom.Length();
  Standard_Integer NbConnexions = IAfter;
  Standard_Integer IndCOF;

  for (i = 1; i <= IAfter; i++) {
    CC     = ConnexionFrom.Value(i);
    IndCOF = CC->IndexItemOnFirst();
    Type = Line.Value(IndCOF)->DynamicType();

    if ( Type == STANDARD_TYPE(Geom2d_CartesianPoint) ){
      if (IndCOF!= NbItems && IndCOF!= 1) {
	ProVec = CrossProd(Line.Value(IndCOF - 1),Line.Value(IndCOF + 1),DotProd);
	if ((ProVec)*SideRef > 0){
	   CC->IndexItemOnFirst(2*NbItems - IndCOF);
	   ConnexionFrom.InsertAfter(IAfter,CC);
	   ConnexionFrom.Remove(i);
	   IAfter--;
	   i--;
	 }
      }
    }
    else  if (Side(CC,Line) != SideRef){
      Curve = Handle(Geom2d_TrimmedCurve)::DownCast(Line.Value(IndCOF));
      CC->IndexItemOnFirst(2*NbItems - IndCOF);
      CC->ParameterOnFirst(Curve->ReversedParameter(CC->ParameterOnFirst()));
      ConnexionFrom.InsertAfter(IAfter,CC);
      ConnexionFrom.Remove(i);
      IAfter--;
      i--;
    }
  }

  //---------------------------
  // Mise a jour connexion pere.  
  //---------------------------
  if (!ConnexionFather.IsNull()) {
    CC     = ConnexionFather->Reverse();
    IndCOF = CC->IndexItemOnFirst();
    Type = Line.Value(IndCOF)->DynamicType();

    if ( Type == STANDARD_TYPE(Geom2d_CartesianPoint) ){
      if (IndCOF != NbItems && IndCOF != 1) {
	ProVec = CrossProd(Line.Value(IndCOF - 1),Line.Value(IndCOF + 1),DotProd);
	if ((ProVec)*SideRef > 0){
	  ConnexionFather->IndexItemOnSecond(2*NbItems - IndCOF);
	 }
      }
    }
    else  if (Side(CC,Line) != SideRef){
      Curve = Handle(Geom2d_TrimmedCurve)::DownCast(Line.Value(IndCOF));
      ConnexionFather->IndexItemOnSecond(2*NbItems - IndCOF);
      ConnexionFather->ParameterOnSecond
	(Curve->ReversedParameter(ConnexionFather->ParameterOnSecond()));
    }
  } 
  
  //-------------------------------------
  // Suppression des cassures rentrantes.
  //-------------------------------------
  Standard_Integer        IndLine = 1;
  Standard_Integer        ICorres = 1;
  TColStd_Array1OfInteger Corres(1,Line.Length());
    
  while (Line.Value(IndLine) != Line.Last()){
    Corres(ICorres) = IndLine;
    Type = Line.Value(IndLine)->DynamicType();

    if (Type == STANDARD_TYPE(Geom2d_CartesianPoint) && 
	ICorres != 1 && ICorres != NbItems)  {

      if (!IsSharpCorner(Line.Value(IndLine - 1),
			 Line.Value(IndLine + 1),SideRef)){
	Line.Remove(IndLine);
	IndLine--;
	Corres(ICorres) =0;
      }
    }
    IndLine++;
    ICorres++;
  }
  Corres(ICorres) = IndLine;

  if (!myIsOpenResult)
  {
    for (i = 1; i < 2*NbItems - 2; i++) {
      if (Corres(i) == 0)
        Corres(i) = Corres(2*NbItems - i);
    }
    
#ifdef OCCT_DEBUG
    if (AffichCircuit) {
      for (i = 1; i <= 2*NbItems - 2; i++) {
        cout<< "Correspondance "<< i<<" -> "<<Corres(i)<<endl;
      }
    }
#endif
    
    //----------------------------
    // Mise a jour des Connexions.
    //----------------------------
    for ( i = 1; i <= NbConnexions; i++){
      CC = ConnexionFrom.ChangeValue(i);
      CC->IndexItemOnFirst(Corres(CC->IndexItemOnFirst()));
    }
    
    if (!ConnexionFather.IsNull()) {
      ConnexionFather
        ->IndexItemOnSecond(Corres(ConnexionFather->IndexItemOnSecond()));
    }
  }
}


//=============================================================================
//function : InsertCorner
//purpose :
//=============================================================================
void  MAT2d_Circuit::InsertCorner (TColGeom2d_SequenceOfGeometry& Line) const
{
  Standard_Integer              i,isuiv;
  Handle(Geom2d_TrimmedCurve)   Curve;
  Standard_Boolean              Insert;

  for ( i = 1; i <= Line.Length(); i++) {
    isuiv  = (i == Line.Length()) ? 1 : i + 1;
    Insert = IsSharpCorner(Line.Value(i),Line.Value(isuiv),direction);

#ifdef OCCT_DEBUG
  if (AffichCircuit) {
    if (Insert) {
      Curve      = Handle(Geom2d_TrimmedCurve)::DownCast(Line.Value(isuiv));
#ifdef DRAW
      gp_Pnt2d P = Curve->StartPoint();
      Handle(Draw_Marker2D) dr = new Draw_Marker2D(P,Draw_Plus,Draw_vert); 
      dout << dr;
      dout.Flush();
#endif
    }
  }
#endif

    if (Insert) {     
      Curve = Handle(Geom2d_TrimmedCurve)::DownCast(Line.Value(isuiv));
      Line.InsertAfter(i,new Geom2d_CartesianPoint(Curve->StartPoint()));
      i++;
    }
  }
}

//=============================================================================
//function : NumberOfItem
//purpose :
//=============================================================================
Standard_Integer  MAT2d_Circuit::NumberOfItems()const 
{
  return geomElements.Length();
}

//=============================================================================
//function : LineLength 
//purpose  :
//=============================================================================
Standard_Integer MAT2d_Circuit::LineLength(const Standard_Integer I) const
{
  return linesLength(I);
}

//=============================================================================
//function : Value
//purpose  :
//=============================================================================
Handle(Geom2d_Geometry)  MAT2d_Circuit::Value 
       (const Standard_Integer Index)const
{
  return geomElements.Value(Index);
}

//=============================================================================
//function : RefToEqui
//purpose  :
//=============================================================================
const TColStd_SequenceOfInteger&  MAT2d_Circuit::RefToEqui
       (const Standard_Integer IndLine, 
	const Standard_Integer IndCurve) const
{
  MAT2d_BiInt Key(IndLine,IndCurve);
  return linkRefEqui(Key);
}

//=============================================================================
//function : SortRefToEqui
//purpose  :
//=============================================================================
void MAT2d_Circuit::SortRefToEqui (const MAT2d_BiInt& BiRef)
{
  Standard_Integer i;
  TColStd_SequenceOfInteger&  S = linkRefEqui.ChangeFind(BiRef);
  TColStd_SequenceOfInteger   SFin;

  for( i = 1; i <=  S.Length(); i++){
    if (!ConnexionOn(S.Value(i))) break;
  }
  if ( i > 1 && i <= S.Length()) {
    SFin = S;
    SFin.Split(i,S);
    S.Append(SFin);
  }
}

//=============================================================================
//function : Connexion
//purpose  :
//=============================================================================
Handle(MAT2d_Connexion) MAT2d_Circuit::Connexion(const Standard_Integer I)const
{
  return connexionMap(I);
}

//=============================================================================
//function : ConnexionOn
//purpose  :
//=============================================================================
Standard_Boolean  MAT2d_Circuit::ConnexionOn(const Standard_Integer I)const
{
  return connexionMap.IsBound(I);
}

//=============================================================================
//function : Side
//purpose  :
//=============================================================================
Standard_Real MAT2d_Circuit::Side
  (const Handle(MAT2d_Connexion)&               C1,
   const TColGeom2d_SequenceOfGeometry&         Line)
const
{
  Handle(Geom2d_TrimmedCurve) Curve;

  gp_Vec2d Vect1(C1->PointOnSecond().X() - C1->PointOnFirst().X(),
		 C1->PointOnSecond().Y() - C1->PointOnFirst().Y());
  Curve = Handle(Geom2d_TrimmedCurve)::DownCast
                                       (Line.Value(C1->IndexItemOnFirst()));
  gp_Vec2d Vect2 = Curve->DN(C1->ParameterOnFirst(),1);  
  if ( (Vect1^Vect2) > 0.) return - 1.; else return 1.;
}

//=============================================================================
//function : PassByLast
//purpose  :
//=============================================================================
Standard_Boolean MAT2d_Circuit::PassByLast
  (const Handle(MAT2d_Connexion)& C1,
   const Handle(MAT2d_Connexion)& C2) const
{
  if (C2->IndexFirstLine() == C1->IndexSecondLine()){
    if (C2->IndexItemOnFirst() < C1->IndexItemOnSecond()) {
      return Standard_True;
    }
    else if (C2->IndexItemOnFirst() == C1->IndexItemOnSecond()) {
      if (C1->IndexFirstLine() == C2->IndexSecondLine()) {
	return Standard_True;
      }
      if (C2->ParameterOnFirst() == C1->ParameterOnSecond()) {
	gp_Vec2d Vect1(C1->PointOnSecond(),C1->PointOnFirst());
	gp_Vec2d Vect2(C2->PointOnFirst(),C2->PointOnSecond());
	if ((Vect1^Vect2)*direction > 0) {
	  return Standard_True;
	}
      }
      else if (C2->ParameterOnFirst() < C1->ParameterOnSecond()) {
	return Standard_True;
      }
    }
  }
  return Standard_False;
}

//=============================================================================
//function : UpDateLink
//purpose  :
//=============================================================================
void MAT2d_Circuit::UpDateLink(const Standard_Integer IFirst,
			       const Standard_Integer ILine,
			       const Standard_Integer ICurveFirst,
			       const Standard_Integer ICurveLast)
{
  Standard_Integer IEqui = IFirst;
  Standard_Integer i;

  for (i = ICurveFirst; i <= ICurveLast; i++) {
    MAT2d_BiInt Key(ILine,i);
    if (linkRefEqui.IsBound(Key)) {
      linkRefEqui(Key).Append(IEqui);
    }
    else {
      TColStd_SequenceOfInteger L;
      linkRefEqui.Bind(Key,L);
      linkRefEqui(Key).Append(IEqui);
    }
    IEqui++;
  }
}

//==========================================================================
//function : CrossProd 
//purpose  : Calcul le produit vectoriel et scalaire  entre les directions des
//            tangentes a la fin de Geom1 et au debut de Geom2.
//            Geom1 et Geom2 doivent etre des courbes.
//==========================================================================
static Standard_Real CrossProd(const Handle(Geom2d_Geometry)& Geom1,
			       const Handle(Geom2d_Geometry)& Geom2,
			             Standard_Real&           DotProd)
{
  Handle(Geom2d_TrimmedCurve) Curve;

  Curve = Handle(Geom2d_TrimmedCurve)::DownCast(Geom1);
  gp_Dir2d Dir1(Curve->DN(Curve->LastParameter(),1));
  Curve = Handle(Geom2d_TrimmedCurve)::DownCast(Geom2);
  gp_Dir2d Dir2(Curve->DN(Curve->FirstParameter(),1));
  DotProd = Dir1.Dot(Dir2);
  return Dir1^Dir2;
}





#ifdef OCCT_DEBUG
//==========================================================================
//function : MAT2d_DrawCurve
//purpose  : Affichage d une courbe <aCurve> de Geom2d. dans une couleur
//            definie par <Indice>.
//            Indice = 1 jaune,
//            Indice = 2 bleu,
//            Indice = 3 rouge,
//            Indice = 4 vert.
//==========================================================================
void MAT2d_DrawCurve(const Handle(Geom2d_Curve)& aCurve,
		     const Standard_Integer      /*Indice*/)
{  
  Handle(Standard_Type)      type = aCurve->DynamicType();
  Handle(Geom2d_Curve)       curve,CurveDraw;
#ifdef DRAW
  Handle(DrawTrSurf_Curve2d) dr;
  Draw_Color                 Couleur;
#endif

  if (type == STANDARD_TYPE(Geom2d_TrimmedCurve)) {
    curve = Handle(Geom2d_TrimmedCurve)::DownCast(aCurve)->BasisCurve();
    type = curve->DynamicType();    
    // PB de representation des courbes semi_infinies.
    gp_Parab2d gpParabola;
    gp_Hypr2d  gpHyperbola;
    Standard_Real Focus;
    Standard_Real Limit = 50000.;
    Standard_Real delta = 400;

    // PB de representation des courbes semi_infinies.
    if (aCurve->LastParameter() == Precision::Infinite()) {
      
      if (type == STANDARD_TYPE(Geom2d_Parabola)) {
	gpParabola = Handle(Geom2d_Parabola)::DownCast(curve)->Parab2d();
	Focus = gpParabola.Focal();
	Standard_Real Val1 = Sqrt(Limit*Focus);
	Standard_Real Val2 = Sqrt(Limit*Limit);
	              delta= (Val1 <= Val2 ? Val1:Val2);
      }
      else if (type == STANDARD_TYPE(Geom2d_Hyperbola)) {
	gpHyperbola = Handle(Geom2d_Hyperbola)::DownCast(curve)->Hypr2d();
	Standard_Real Majr  = gpHyperbola.MajorRadius();
	Standard_Real Minr  = gpHyperbola.MinorRadius();
	Standard_Real Valu1 = Limit/Majr;
	Standard_Real Valu2 = Limit/Minr;
	Standard_Real Val1  = Log(Valu1+Sqrt(Valu1*Valu1-1));
	Standard_Real Val2  = Log(Valu2+Sqrt(Valu2*Valu2+1));
	              delta  = (Val1 <= Val2 ? Val1:Val2);
      }
      CurveDraw = new Geom2d_TrimmedCurve(aCurve,
					  aCurve->FirstParameter(),
					  aCurve->FirstParameter() + delta);
    }
    else {
      CurveDraw = aCurve;
    }
    // fin PB.
  }
  else {
    CurveDraw = aCurve;
  }

#ifdef DRAW
  if      (Indice == 1) Couleur = Draw_jaune;
  else if (Indice == 2) Couleur = Draw_bleu;
  else if (Indice == 3) Couleur = Draw_rouge;
  else if (Indice == 4) Couleur = Draw_vert;

  if (type == STANDARD_TYPE(Geom2d_Circle))
    dr = new DrawTrSurf_Curve2d(CurveDraw,Couleur,30);
  else if (type  == STANDARD_TYPE(Geom2d_Line))
    dr = new DrawTrSurf_Curve2d(CurveDraw,Couleur,2);
  else
    dr = new DrawTrSurf_Curve2d(CurveDraw,Couleur,500);

  dout << dr;
  dout.Flush();
#endif
}

#endif

