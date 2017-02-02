// Created on: 1993-07-12
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

#define Debug(expr)  cout<<" MAT2d_Tool2d.cxx  :  expr :"<<expr<<endl;
//#define OCCT_DEBUG
//#define DRAW
#ifdef DRAW
#include <DBRep.hxx>
#include <DrawTrSurf.hxx>
#include <stdio.h>
#endif

#ifdef DRAW
#include <Draw_Appli.hxx>
#include <DrawTrSurf_Curve2d.hxx>
#include <GCE2d_MakeSegment.hxx>
#include <DrawTrSurf.hxx>
#endif


#include <Bisector_Bisec.hxx>
#include <Bisector_BisecAna.hxx>
#include <Bisector_BisecCC.hxx>
#include <Bisector_Curve.hxx>
#include <Bisector_Inter.hxx>
#include <ElCLib.hxx>
#include <Extrema_ExtPC2d.hxx>
#include <Geom2d_CartesianPoint.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Geometry.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_Point.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dInt_GInter.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Parab2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <IntRes2d_Domain.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <IntRes2d_IntersectionSegment.hxx>
#include <MAT2d_Circuit.hxx>
#include <MAT2d_Connexion.hxx>
#include <MAT2d_MiniPath.hxx>
#include <MAT2d_SequenceOfSequenceOfGeometry.hxx>
#include <MAT2d_Tool2d.hxx>
#include <MAT_Bisector.hxx>
#include <MAT_Edge.hxx>
#include <Precision.hxx>
#include <Standard_NotImplemented.hxx>
#include <StdFail_NotDone.hxx>
#include <TColStd_Array1OfReal.hxx>

#ifdef DRAW
static Handle(DrawTrSurf_Curve2d) draw;
static Standard_Integer AffichBis = Standard_False;
#endif
#ifdef OCCT_DEBUG
static void MAT2d_DrawCurve(const Handle(Geom2d_Curve)& aCurve,
  const Standard_Integer      Indice);
static Standard_Boolean Store = Standard_False;
static Standard_Boolean AffichDist = Standard_False;
#endif

//=====================================================================
//  static functions 
//=====================================================================
static IntRes2d_Domain Domain
  (const Handle(Geom2d_TrimmedCurve)& Bisector1,
  const Standard_Real                Tolerance);

static Handle(Standard_Type) Type (const Handle(Geom2d_Geometry)& acurve);

static Standard_Boolean AreNeighbours(const Standard_Integer IEdge1,
  const Standard_Integer IEdge2,
  const Standard_Integer NbEdge);

static void SetTrim(Bisector_Bisec&  Bis, const Handle(Geom2d_Curve)& Line1);
static Standard_Boolean CheckEnds (const Handle(Geom2d_Geometry)& Elt    ,
                                   const gp_Pnt2d&                PCom   ,
                                   const Standard_Real            Distance,
                                   const Standard_Real            Tol); 

static Standard_Real MAT2d_TOLCONF = 1.e-7;

//============================================================================
//function : 
//purpose  :
//============================================================================
MAT2d_Tool2d::MAT2d_Tool2d()
{
  theDirection         = 1.;
  theJoinType = GeomAbs_Arc; //default
  theNumberOfBisectors = 0;
  theNumberOfVecs      = 0;
  theNumberOfPnts      = 0;
}

//=============================================================================
//function : InitItems
//purpose  :
//=============================================================================
void  MAT2d_Tool2d::InitItems(const Handle(MAT2d_Circuit)& EquiCircuit) 
{
  theGeomBisectors.Clear();
  theGeomPnts.Clear();
  theGeomVecs.Clear();
  theLinesLength.Clear();
  theNumberOfBisectors = 0;
  theNumberOfVecs      = 0;
  theNumberOfPnts      = 0; 

  theCircuit = EquiCircuit;
}

//=============================================================================
//function : Sense
//purpose  :
//=============================================================================
void MAT2d_Tool2d::Sense(const MAT_Side aside)
{
  if(aside == MAT_Left) theDirection =  1.;
  else                  theDirection = -1.;
}

//=============================================================================
//function : SetJoinType
//purpose  :
//=============================================================================
void MAT2d_Tool2d::SetJoinType(const GeomAbs_JoinType aJoinType)
{
  theJoinType = aJoinType;
}

//=============================================================================
//function : NumberOfItems
//purpose  :
//=============================================================================
Standard_Integer MAT2d_Tool2d::NumberOfItems() const
{
  return theCircuit->NumberOfItems();
}

//=============================================================================
//function : ToleranceOfConfusion
//purpose  :
//=============================================================================
Standard_Real MAT2d_Tool2d::ToleranceOfConfusion() const
{
  return 2*MAT2d_TOLCONF;
}

//=============================================================================
//function : FirstPoint
//purpose  :
//=============================================================================
Standard_Integer MAT2d_Tool2d::FirstPoint(const Standard_Integer anitem,
  Standard_Real&   dist  ) 
{
  Handle(Geom2d_Curve) curve;
  Handle(Geom2d_Point) point;
  theNumberOfPnts++;

  if (theCircuit->ConnexionOn(anitem)){
    gp_Pnt2d P1 = theCircuit->Connexion(anitem)->PointOnFirst();
    gp_Pnt2d P2 = theCircuit->Connexion(anitem)->PointOnSecond();
    theGeomPnts.Bind(theNumberOfPnts,gp_Pnt2d((P1.X() + P2.X())*0.5,
      (P1.Y() + P2.Y())*0.5));
    dist = P1.Distance(P2)*0.5;
    return theNumberOfPnts;
  }

  Handle(Standard_Type) type;
  type = theCircuit->Value(anitem)->DynamicType();
  dist = 0.;

  if ( type != STANDARD_TYPE(Geom2d_CartesianPoint)){
    curve = Handle(Geom2d_Curve)::DownCast(theCircuit->Value(anitem));
    theGeomPnts.Bind(theNumberOfPnts,curve->Value(curve->FirstParameter()));
  }
  else{
    point = Handle(Geom2d_Point)::DownCast(theCircuit->Value(anitem));
    theGeomPnts.Bind(theNumberOfPnts,point->Pnt2d());
  }
  return theNumberOfPnts;
}

//=============================================================================
//function : TangentBefore
//purpose  :
//=============================================================================
Standard_Integer MAT2d_Tool2d::TangentBefore(const Standard_Integer anitem,
                                             const Standard_Boolean IsOpenResult)
{
  Standard_Integer     item;
  Handle(Geom2d_Curve) curve;
  theNumberOfVecs++;

  if (!IsOpenResult)
  item  = (anitem == theCircuit->NumberOfItems()) ? 1 : (anitem + 1);
  else
    item = (anitem == theCircuit->NumberOfItems()) ? (anitem - 1) : (anitem + 1);
  if (theCircuit->ConnexionOn(item)){
    Standard_Real x1,y1,x2,y2;
    theCircuit->Connexion(item)->PointOnFirst().Coord(x1,y1);
    theCircuit->Connexion(item)->PointOnSecond().Coord(x2,y2);
    theGeomVecs.Bind(theNumberOfVecs,gp_Vec2d((x2-x1),(y2-y1)));
    return theNumberOfVecs;
  }

  Handle(Standard_Type) type;
  type = theCircuit->Value(anitem)->DynamicType();
  if ( type != STANDARD_TYPE(Geom2d_CartesianPoint)){
    curve = Handle(Geom2d_Curve)::DownCast(theCircuit->Value(anitem));
    theGeomVecs.Bind(theNumberOfVecs,curve->DN(curve->LastParameter(),1));
  }
  else {
    curve = Handle(Geom2d_Curve)::DownCast(theCircuit->Value(item));
    Standard_Real param = (IsOpenResult && anitem == theCircuit->NumberOfItems())?
      curve->LastParameter() : curve->FirstParameter();
    theGeomVecs.Bind(theNumberOfVecs,curve->DN(param,1));
  }

  return theNumberOfVecs;
}

//=============================================================================
//function : TangentAfter
//purpose  :
//=============================================================================
Standard_Integer MAT2d_Tool2d::TangentAfter(const Standard_Integer anitem,
                                            const Standard_Boolean IsOpenResult)
{
  Standard_Integer     item;
  Handle(Geom2d_Curve) curve;
  gp_Vec2d             thevector;
  theNumberOfVecs++;

  if (theCircuit->ConnexionOn(anitem)){
    Standard_Real x1,y1,x2,y2;
    theCircuit->Connexion(anitem)->PointOnFirst().Coord(x1,y1);
    theCircuit->Connexion(anitem)->PointOnSecond().Coord(x2,y2);
    theGeomVecs.Bind(theNumberOfVecs,gp_Vec2d((x1-x2),(y1-y2)));
    return theNumberOfVecs;
  }

  Handle(Standard_Type) type;
  type = theCircuit->Value(anitem)->DynamicType();
  if ( type != STANDARD_TYPE(Geom2d_CartesianPoint)){
    curve     = Handle(Geom2d_Curve)::DownCast(theCircuit->Value(anitem));
    thevector = curve->DN(curve->FirstParameter(),1);
  }
  else {
    if (!IsOpenResult)
    item      = (anitem == 1) ? theCircuit->NumberOfItems() : (anitem - 1);
    else
      item = (anitem == 1) ? 2 : (anitem - 1);
    curve     = Handle(Geom2d_Curve)::DownCast(theCircuit->Value(item));
    Standard_Real param = (IsOpenResult && anitem == 1)?
      curve->FirstParameter() : curve->LastParameter();
    thevector = curve->DN(param,1);
  }
  theGeomVecs.Bind(theNumberOfVecs,thevector.Reversed());
  return theNumberOfVecs;
}

//=============================================================================
//function : Tangent
//purpose  :
//=============================================================================
Standard_Integer MAT2d_Tool2d::Tangent(const Standard_Integer bisector)
{
  theNumberOfVecs++;
  theGeomVecs.Bind(theNumberOfVecs,GeomBis(bisector).Value()
    ->DN(GeomBis(bisector).Value()
    ->LastParameter(),1));
  return theNumberOfVecs;
}

//=============================================================================
//function : CreateBisector
//purpose  :
//=============================================================================
void MAT2d_Tool2d::CreateBisector(const Handle(MAT_Bisector)& abisector)
{
  Handle(Geom2d_Point)    point1,point2;
  Handle(Geom2d_Geometry) elt1,elt2;
  Bisector_Bisec          bisector;
  Standard_Real           tolerance    = MAT2d_TOLCONF ;

  Standard_Integer edge1number  = abisector->FirstEdge()->EdgeNumber();
  Standard_Integer edge2number  = abisector->SecondEdge()->EdgeNumber();
  Standard_Boolean ontheline    = AreNeighbours(edge1number,
    edge2number,
    NumberOfItems());
  Standard_Boolean InitialNeighbour = ontheline;

  if(theCircuit->ConnexionOn(edge2number)) ontheline = Standard_False;

  elt1 = theCircuit->Value(edge1number);
  elt2 = theCircuit->Value(edge2number);

  Handle(Standard_Type) type1;
  type1 = theCircuit->Value(edge1number)->DynamicType();
  Handle(Standard_Type) type2;
  type2 = theCircuit->Value(edge2number)->DynamicType();
  Handle(Geom2d_Curve)  item1;
  Handle(Geom2d_Curve)  item2;

  if ( type1 != STANDARD_TYPE(Geom2d_CartesianPoint)){
    item1 = Handle(Geom2d_Curve)::DownCast(elt1);
  }

  if ( type2 != STANDARD_TYPE(Geom2d_CartesianPoint)){
    item2 = Handle(Geom2d_Curve)::DownCast(elt2);
  }

#ifdef OCCT_DEBUG
  Standard_Boolean Affich = Standard_False;
  if (Affich) {
    cout<<endl; 
    cout<<"BISECTOR number :  "<<theNumberOfBisectors+1<<endl;
    cout<<"  Item 1 : "<<endl;
    cout<<edge1number<<endl;
    cout<<endl;
    //    elt1->Dump(1,1);
    cout<<endl;
    cout<<"  Item 2 : "<<endl;
    cout<<edge2number<<endl;
    cout<<endl;
    //  elt2->Dump(1,1);
    cout<<endl;
  }
#endif

  if(type1 != STANDARD_TYPE(Geom2d_CartesianPoint) && 
     type2 != STANDARD_TYPE(Geom2d_CartesianPoint)) {
    bisector.Perform(item1,item2,
		     GeomPnt (abisector->IssuePoint()),
		     GeomVec (abisector->FirstVector()),
		     GeomVec (abisector->SecondVector()),
		     theDirection,theJoinType,tolerance,ontheline);
  }
  else if(type1 == STANDARD_TYPE(Geom2d_CartesianPoint) && 
    type2 == STANDARD_TYPE(Geom2d_CartesianPoint)) {
      point1 = Handle(Geom2d_Point)::DownCast(elt1);
      point2 = Handle(Geom2d_Point)::DownCast(elt2);
      bisector.Perform(point1,point2,
        GeomPnt (abisector->IssuePoint()),
        GeomVec (abisector->FirstVector()),
        GeomVec (abisector->SecondVector()),
        theDirection,tolerance,ontheline);
  }
  else if(type1 == STANDARD_TYPE(Geom2d_CartesianPoint)) {
    point1 = Handle(Geom2d_Point)::DownCast(elt1);
    bisector.Perform(point1,item2,
      GeomPnt (abisector->IssuePoint()),
      GeomVec (abisector->FirstVector()),
      GeomVec (abisector->SecondVector()),
      theDirection,tolerance,ontheline);
  }
  else {
    point2 = Handle(Geom2d_Point)::DownCast(elt2);
    bisector.Perform(item1,point2,
      GeomPnt (abisector->IssuePoint()),
      GeomVec (abisector->FirstVector()),
      GeomVec (abisector->SecondVector()),
      theDirection,tolerance,ontheline);
  }

  //------------------------------
  // Restriction de la bisectrice.
  //-----------------------------
  TrimBisec(bisector,edge1number,InitialNeighbour,1);
  TrimBisec(bisector,edge2number,InitialNeighbour,2);

  theNumberOfBisectors++;
  theGeomBisectors.Bind(theNumberOfBisectors,bisector);

  abisector->BisectorNumber(theNumberOfBisectors);
  abisector->Sense(1);

#ifdef OCCT_DEBUG
  Standard_Boolean AffichDraw = Standard_False;
  if (AffichDraw) Dump(abisector->BisectorNumber(),1);
  if (Store) {    
    Handle(Standard_Type) Type1 = Type(bisector.Value()->BasisCurve());    
    Handle(Geom2d_Curve)  BasisCurve;
    if (Type1 == STANDARD_TYPE(Bisector_BisecAna)) {
      BasisCurve = Handle(Bisector_BisecAna)
        ::DownCast(bisector.Value()->BasisCurve())->Geom2dCurve();
#ifdef DRAW
      char  *name = new char[100];
      sprintf(name,"BISSEC_%d",abisector->BisectorNumber());
      DrawTrSurf::Set(name,BasisCurve);
      delete [] name;
#endif
    }
  }
#endif
}

//=============================================================================
//function : TrimBisec
//purpose  : Restriction de la bisectrice.
//           Restriction des bissectrice separant deux elements lies par une
//           connexion ou l un au moins des elements est un cercle.
//           Cette restriction est necessaire a la logique de l algorithme.
//=============================================================================
void MAT2d_Tool2d::TrimBisec (      Bisector_Bisec&  B1,
  const Standard_Integer IndexEdge,
  const Standard_Boolean InitialNeighbour,
  const Standard_Integer StartOrEnd      ) const
{
  Handle(Geom2d_Curve)        Curve;
  Handle(Geom2d_TrimmedCurve) LineSupportDomain,Line;
  Handle(Geom2d_Line)         Line1,Line2;

  //gp_Vec2d             Tan1,Tan2;
  gp_Pnt2d             Ori; //PEdge;
  Standard_Integer     INext;
  INext = (IndexEdge == theCircuit->NumberOfItems()) ? 1  : (IndexEdge + 1);

  Handle(Standard_Type) EdgeType = theCircuit->Value(IndexEdge)->DynamicType();

  if (EdgeType != STANDARD_TYPE(Geom2d_CartesianPoint)) {
    if(!InitialNeighbour) {
      Curve = Handle(Geom2d_TrimmedCurve)
        ::DownCast(theCircuit->Value(IndexEdge))->BasisCurve();
      EdgeType = Curve->DynamicType();
      //-------------------------------------------------------------------
      // si l edge est liee a sa voisine  precedente par une connexion.
      //-------------------------------------------------------------------
      if (theCircuit->ConnexionOn(IndexEdge) && StartOrEnd == 1){
        if (EdgeType == STANDARD_TYPE(Geom2d_Circle)) {
          Ori = Handle(Geom2d_Circle)::DownCast(Curve)->Location();
          gp_Pnt2d P2 = theCircuit->Connexion(IndexEdge)->PointOnFirst();
          Line1       = new Geom2d_Line (Ori,gp_Dir2d(P2.X() - Ori.X(),
            P2.Y() - Ori.Y()));
        }     
      }
      //-----------------------------------------------------------------------
      // Si l edge est liee a sa voisine suivante par une connexion.
      //-----------------------------------------------------------------------
      if (theCircuit->ConnexionOn(INext) && StartOrEnd == 2){
        if (EdgeType == STANDARD_TYPE(Geom2d_Circle)) {
          Ori = Handle(Geom2d_Circle)::DownCast(Curve)->Location();
          gp_Pnt2d P2 = theCircuit->Connexion(INext)->PointOnSecond();
          Line2       = new Geom2d_Line (Ori,gp_Dir2d(P2.X() - Ori.X(),
            P2.Y() - Ori.Y()));
        }
      }
      if (Line1.IsNull() && Line2.IsNull()) return;

      //-----------------------------------------------------------------------
      // Restriction de la bisectrice par les demi-droites liees aux connexions
      // si elles existent.
      //-----------------------------------------------------------------------
      if (!Line1.IsNull()) {
        Line = new Geom2d_TrimmedCurve(Line1,0.,Precision::Infinite());
        SetTrim(B1,Line);
      }
      if (!Line2.IsNull()) {
        Line = new Geom2d_TrimmedCurve(Line2,0.,Precision::Infinite());
        SetTrim(B1,Line);
      }
    }
  }
}

//=============================================================================
//function : TrimBisector
//purpose  :
//=============================================================================
Standard_Boolean MAT2d_Tool2d::TrimBisector
  (const Handle(MAT_Bisector)& abisector)
{
  Standard_Real param = abisector->FirstParameter();

#ifdef OCCT_DEBUG
  Standard_Boolean Affich = Standard_False;
  if (Affich) cout<<"TRIM de "<<abisector->BisectorNumber()<<endl;
#endif

  Handle(Geom2d_TrimmedCurve) bisector = 
    ChangeGeomBis(abisector->BisectorNumber()).ChangeValue();

  if(bisector->BasisCurve()->IsPeriodic() && param == Precision::Infinite()) {
    param = bisector->FirstParameter() + 2*M_PI;
  }
  if (param > bisector->BasisCurve()->LastParameter()) {
    param = bisector->BasisCurve()->LastParameter(); 
  }
  if(bisector->FirstParameter() == param) return Standard_False;

  bisector->SetTrim(bisector->FirstParameter(),param);
  return Standard_True;
}

//=============================================================================
//function : TrimBisector
//purpose  :
//=============================================================================
Standard_Boolean MAT2d_Tool2d::TrimBisector
  (const Handle(MAT_Bisector)& abisector,
  const Standard_Integer      apoint)
{
  Standard_Real Param;
  Handle(Geom2d_TrimmedCurve) Bisector =
    ChangeGeomBis(abisector->BisectorNumber()).ChangeValue();

  Handle(Bisector_Curve) Bis = Handle(Bisector_Curve)::
    DownCast(Bisector->BasisCurve());

  //  Param = ParameterOnCurve(Bisector,theGeomPnts.Value(apoint));
  Param = Bis->Parameter(GeomPnt (apoint));

  if (Bisector->BasisCurve()->IsPeriodic()) {
    if (Bisector->FirstParameter() > Param) Param = Param + 2*M_PI;
  }
  if(Bisector->FirstParameter() >= Param)return Standard_False;
  if(Bisector->LastParameter()  <  Param)return Standard_False;
  Bisector->SetTrim(Bisector->FirstParameter(),Param);

#ifdef OCCT_DEBUG
  Standard_Boolean Affich = Standard_False;
  if (Affich) MAT2d_DrawCurve(Bisector,2);
#endif  

  return Standard_True;
}

//=============================================================================
//function : Projection
//purpose  :
//=============================================================================
Standard_Boolean MAT2d_Tool2d::Projection (const Standard_Integer IEdge   ,
  const gp_Pnt2d&        PCom    ,
  Standard_Real&   Distance) 
  const
{  
  gp_Pnt2d                    PEdge;
  Handle(Geom2d_Geometry)     Elt    = theCircuit->Value(IEdge);
  Handle(Standard_Type)       Type   = Elt->DynamicType();	
  Handle(Geom2d_TrimmedCurve) Curve; 
  Standard_Integer            INext;   
  Standard_Real               Eps = MAT2d_TOLCONF;//*10.;

  if (Type == STANDARD_TYPE(Geom2d_CartesianPoint)) {	
    PEdge     = Handle(Geom2d_Point)::DownCast(Elt)->Pnt2d();
    Distance  =	PCom.Distance(PEdge); 	
  }
  else {
    Distance = Precision::Infinite();
    Curve    = Handle(Geom2d_TrimmedCurve)::DownCast(Elt);	
    //-----------------------------------------------------------------------
    // Calcul des parametres MinMax sur l edge si celui ci est lies a ses
    // voisins par des connexions la courbe de calcul est limitee par 
    // celles_ci. 	  
    //-----------------------------------------------------------------------
    Standard_Real ParamMin = Curve->FirstParameter();
    Standard_Real ParamMax = Curve->LastParameter();
    if (theCircuit->ConnexionOn(IEdge)) {
      ParamMin = theCircuit->Connexion(IEdge)->ParameterOnSecond(); 
    }
    INext = (IEdge == theCircuit->NumberOfItems()) ? 1 : (IEdge + 1);
    if (theCircuit->ConnexionOn(INext)) {
      ParamMax = theCircuit->Connexion(INext)->ParameterOnFirst(); 
      if (Curve->BasisCurve()->IsPeriodic()){
        ElCLib::AdjustPeriodic(0.,2*M_PI,Eps,ParamMin,ParamMax);
      }
    }
    //---------------------------------------------------------------------
    // Constuction de la courbe pour les extremas et ajustement des bornes.
    //---------------------------------------------------------------------
    Geom2dAdaptor_Curve C1(Curve);
    GeomAbs_CurveType TypeC1 = C1.GetType();
    if (TypeC1 == GeomAbs_Circle) {
      Standard_Real R       = C1.Circle().Radius();
      Standard_Real EpsCirc = 100.*Eps;
      if ( R < 1.)  EpsCirc = Eps/R;
      if (((ParamMax - ParamMin + 2*EpsCirc) < 2*M_PI)) {
        ParamMax = ParamMax + EpsCirc; ParamMin = ParamMin - EpsCirc;
      }
    }
    else {
      ParamMax = ParamMax + Eps; ParamMin = ParamMin - Eps; 
    }
    //-----------------------------------------------------
    // Calcul des extremas et stockage minimum de distance.
    //-----------------------------------------------------
    Extrema_ExtPC2d Extremas(PCom,C1,ParamMin,ParamMax);
    if (Extremas.IsDone()){
      Distance = Precision::Infinite();
      if(Extremas.NbExt() < 1) 
      {
        return Standard_False;
      }
      for (Standard_Integer i = 1; i <= Extremas.NbExt(); i++) {
        if (Extremas.SquareDistance(i) < Distance) {
          Distance      = Extremas.SquareDistance(i);
        }
      }
      Distance = Sqrt(Distance);
    }
    else {
      if (TypeC1 == GeomAbs_Circle) {
        Distance = C1.Circle().Radius();
      }
    }
  }
  return Standard_True;
}

//=============================================================================
//function : IsSameDistance
// purpose :
//=============================================================================
Standard_Boolean MAT2d_Tool2d::IsSameDistance (
  const Handle(MAT_Bisector)& BisectorOne,
  const Handle(MAT_Bisector)& BisectorTwo,
  const gp_Pnt2d&             PCom,
  Standard_Real&              Distance) const
{
  TColStd_Array1OfReal Dist(1,4);
  const Standard_Real eps = 1.e-7;
  Standard_Integer     IEdge1,IEdge2,IEdge3,IEdge4;

  IEdge1 = BisectorOne->FirstEdge() ->EdgeNumber();
  IEdge2 = BisectorOne->SecondEdge()->EdgeNumber();
  IEdge3 = BisectorTwo->FirstEdge() ->EdgeNumber();
  IEdge4 = BisectorTwo->SecondEdge()->EdgeNumber();

  Standard_Boolean isDone1 = Projection(IEdge1,PCom,Dist(1));
  Standard_Boolean isDone2 = Projection(IEdge2,PCom,Dist(2));

  if(isDone1)
  {
    if(!isDone2)
    {
      Handle(Geom2d_Geometry) Elt = theCircuit->Value(IEdge2);
      Standard_Real Tol = Max(Precision::Confusion(), eps*Dist(1));
      if(CheckEnds (Elt, PCom, Dist(1), Tol))
      { 
        Dist(2) = Dist(1);
      }   
    }
  }
  else
  {
    if(isDone2)
    {
      Handle(Geom2d_Geometry) Elt = theCircuit->Value(IEdge1);
      Standard_Real Tol = Max(Precision::Confusion(), eps*Dist(2));
      if(CheckEnds (Elt, PCom, Dist(2), Tol))
      { 
        Dist(1) = Dist(2);
      }   
    }
  }

  Standard_Boolean isDone3 = Standard_True, isDone4 = Standard_True;
  if      (IEdge3 == IEdge1) Dist(3)  = Dist(1);
  else if (IEdge3 == IEdge2) Dist(3)  = Dist(2);  
  else    isDone3 = Projection(IEdge3,PCom,Dist(3));

  if      (IEdge4 == IEdge1) Dist(4)  = Dist(1);
  else if (IEdge4 == IEdge2) Dist(4)  = Dist(2);  
  else    isDone4 = Projection(IEdge4,PCom,Dist(4));
  //
  if(isDone3)
  {
    if(!isDone4)
    {
      Handle(Geom2d_Geometry) Elt = theCircuit->Value(IEdge4);
      Standard_Real Tol = Max(Precision::Confusion(), eps*Dist(3));
      if(CheckEnds (Elt, PCom, Dist(3), Tol))
      { 
        Dist(4) = Dist(3);
      }   
    }
  }
  else
  {
    if(isDone4)
    {
      Handle(Geom2d_Geometry) Elt = theCircuit->Value(IEdge3);
      Standard_Real Tol = Max(Precision::Confusion(), eps*Dist(4));
      if(CheckEnds (Elt, PCom, Dist(4), Tol))
      { 
        Dist(3) = Dist(4);
      }   
    }
  }

#ifdef OCCT_DEBUG
  if (AffichDist)
    for (Standard_Integer j = 1; j <= 4;j++){
      cout <<"Distance number : "<<j<<" is :"<< Dist(j)<<endl;
    }
#endif

    Standard_Real EpsDist = MAT2d_TOLCONF*300. ;
    Distance = Dist(1);
    if (theJoinType == GeomAbs_Intersection &&
        Precision::IsInfinite(Distance))
    {
      for (Standard_Integer i = 2; i <= 4; i++)
        if (!Precision::IsInfinite(Dist(i)))
        {
          Distance = Dist(i);
          break;
        }
    }
    for (Standard_Integer i = 1; i <= 4; i++){
      if (theJoinType == GeomAbs_Intersection &&
          Precision::IsInfinite(Dist(i)))
        continue;
      if (Abs(Dist(i) - Distance) > EpsDist) {
        Distance = Precision::Infinite();
        return Standard_False;
      }
    }
    return Standard_True;
}

//=============================================================================
//function : IntersectBisector
//purpose  :
//=============================================================================
Standard_Real MAT2d_Tool2d::IntersectBisector (
  const Handle(MAT_Bisector)& BisectorOne,
  const Handle(MAT_Bisector)& BisectorTwo,
  Standard_Integer&           IntPnt)
{
  Standard_Real    Tolerance     = MAT2d_TOLCONF;
  Standard_Real    Param1,Param2;
  Standard_Real    Parama,Paramb;
  Standard_Real    Distance = 0.,DistanceMini;
  Standard_Boolean SolutionValide;
  gp_Pnt2d         PointSolution;

  Handle(Geom2d_TrimmedCurve) Bisector1 =
    ChangeGeomBis(BisectorOne->BisectorNumber()).ChangeValue();

  Handle(Geom2d_TrimmedCurve) Bisector2 =
    ChangeGeomBis(BisectorTwo->BisectorNumber()).ChangeValue();

  if(Bisector1.IsNull() || Bisector2.IsNull()) return Precision::Infinite();

  //-------------------------------------------------------------------------
  // Si les deux bissectrices separent des elements consecutifs et qu elles
  // sont issues des connexions C1 et C2.
  // Si C1 est la reverse de C2 ,alors les deux bissectrices sont issues
  // du meme point. Dans ce cas l intersection n est pas validee.
  //-------------------------------------------------------------------------
  Standard_Integer IS1 = BisectorOne->SecondEdge()->EdgeNumber();
  Standard_Integer IS2 = BisectorTwo->SecondEdge()->EdgeNumber();
  Standard_Integer IF1 = BisectorOne->FirstEdge() ->EdgeNumber();
  Standard_Integer IF2 = BisectorTwo->FirstEdge() ->EdgeNumber();

  if (AreNeighbours(IF1,IS1,NumberOfItems()) && 
    AreNeighbours(IF2,IS2,NumberOfItems()) &&
    theCircuit->ConnexionOn(IS2)           && 
    theCircuit->ConnexionOn(IS1)             ) {
      Handle(MAT2d_Connexion) C1,C2;
      C1 = theCircuit->Connexion(IS1);
      C2 = theCircuit->Connexion(IS2); 
      if (C2->IndexFirstLine() == C1->IndexSecondLine() &&
        C1->IndexFirstLine() == C2->IndexSecondLine()  )
        return Precision::Infinite();
  }

  // -----------------------------------------
  // Construction des domaines d intersection.
  // -----------------------------------------
  IntRes2d_Domain Domain1 = Domain(Bisector1,Tolerance);
  IntRes2d_Domain Domain2 = Domain(Bisector2,Tolerance);

  if (Domain1.LastParameter() - Domain1.FirstParameter()  < Tolerance) 
    return Precision::Infinite();
  if (Domain2.LastParameter() - Domain2.FirstParameter()  < Tolerance) 
    return Precision::Infinite();

#ifdef OCCT_DEBUG
  Standard_Boolean Affich = Standard_False;
  if (Affich) {
    cout<<endl;
    cout<<"INTERSECTION de "<<BisectorOne->BisectorNumber()<<
      " et de "<<BisectorTwo->BisectorNumber()<<endl;
    cout<<"  Bisector 1 : "<<endl;
    //    (Bisector1->BasisCurve())->Dump(-1,1);
    cout<<endl;
    Debug(Domain1.FirstParameter());
    Debug(Domain1.LastParameter());
    cout<<"-----------------"<<endl;
    cout<<"  Bisector 2 : "<<endl;
    //    (Bisector2->BasisCurve())->Dump(-1,1);
    cout<<endl;
    Debug(Domain2.FirstParameter());
    Debug(Domain2.LastParameter());
    cout<<"-----------------"<<endl;
  }
#endif

  // -------------------------
  // Calcul de l intersection.
  // -------------------------

  Bisector_Inter Intersect;
  Intersect.Perform (GeomBis(BisectorOne->BisectorNumber()),Domain1,
    GeomBis(BisectorTwo->BisectorNumber()),Domain2,
    Tolerance,Tolerance,Standard_True);

  //  Geom2dInt_GInter Intersect;
  //  Intersect.Perform(Bisector1,Domain1,Bisector2,Domain2,Tolerance,Tolerance);

  // -------------------------------------------------------------------------
  // Exploitation du resultat de l intersection et selection du point solution
  // equidistant des deux edges et le plus proche en parametre de l origine 
  // des bissectrices.
  // -------------------------------------------------------------------------

  if(!Intersect.IsDone()) return Precision::Infinite();

  if(Intersect.IsEmpty()) return Precision::Infinite();

  DistanceMini   = Precision::Infinite();
  Param1         = Precision::Infinite();
  Param2         = Precision::Infinite();
  SolutionValide = Standard_False;

  if(Intersect.NbSegments() >= 1) {              
    Standard_Real    MaxSegmentLength = 10.*Tolerance;
    for (Standard_Integer i=1;i<=Intersect.NbSegments();i++) {
      IntRes2d_IntersectionSegment Segment     = Intersect.Segment(i);
      Standard_Boolean             PointRetenu = Standard_False;
      gp_Pnt2d                     PointOnSegment;
      // ----------------------------------------------------------------
      // Si les segments sont petits, recherche des points sur le segment
      // equidistants des edges.
      // ----------------------------------------------------------------
      if ((Segment.HasFirstPoint() && Segment.HasLastPoint())) { 
        gp_Pnt2d      P1,P2;
        Standard_Real SegmentLength;
        P1 = Segment.FirstPoint().Value();
        P2 = Segment.LastPoint().Value();
        SegmentLength = P1.Distance(P2);
        if (SegmentLength <= Tolerance) {
          PointOnSegment = P1;
          if(IsSameDistance(BisectorOne,BisectorTwo,
            PointOnSegment,Distance)) 
            PointRetenu = Standard_True;
        }
        else if (SegmentLength <= MaxSegmentLength) {
          gp_Dir2d  Dir(P2.X()-P1.X(),P2.Y()-P1.Y());
          Standard_Real Dist = 0.;  
          while (Dist <= SegmentLength + Tolerance){
            PointOnSegment = P1.Translated(Dist*Dir);
            if(IsSameDistance(BisectorOne,BisectorTwo,
              PointOnSegment,Distance)) {
                PointRetenu = Standard_True;
                break;
            }
            Dist = Dist + Tolerance;
          }
        }
      }  

      // ----------------------------------------------------------------
      // Sauvegarde du point equidistant des edges de plus petit 
      // parametre sur les bissectrices.
      // ----------------------------------------------------------------
      if(PointRetenu) {
        Parama = Handle(Bisector_Curve)::DownCast(Bisector1->BasisCurve())
          ->Parameter(PointOnSegment);
        Paramb = Handle(Bisector_Curve)::DownCast(Bisector2->BasisCurve())
          ->Parameter(PointOnSegment);
        if(Parama < Param1 && Paramb < Param2) {
          Param1         = Parama;
          Param2         = Paramb;
          DistanceMini   = Distance;
          PointSolution  = PointOnSegment;
          SolutionValide = Standard_True;
        }
      }
    }
  }

  if(Intersect.NbPoints() != 1) {
    for(Standard_Integer i=1; i<=Intersect.NbPoints(); i++) {
      if(IsSameDistance(BisectorOne,BisectorTwo,
        Intersect.Point(i).Value(),Distance) &&
        Distance > Tolerance                                   ) {
          Parama = Intersect.Point(i).ParamOnFirst();
          Paramb = Intersect.Point(i).ParamOnSecond();
          if (Parama < Param1 && Paramb < Param2) {
            Param1         = Parama;
            Param2         = Paramb;
            DistanceMini   = Distance;
            PointSolution  = Intersect.Point(i).Value();
            SolutionValide = Standard_True;
          }
      }
    }
  }
  else {
    PointSolution  = Intersect.Point(1).Value();
    Param1         = Intersect.Point(1).ParamOnFirst();
    Param2         = Intersect.Point(1).ParamOnSecond();
    SolutionValide = IsSameDistance(BisectorOne,BisectorTwo,
      PointSolution,DistanceMini);
  }

  if (!SolutionValide) return Precision::Infinite();
  theNumberOfPnts++;
  theGeomPnts.Bind(theNumberOfPnts,PointSolution);
  IntPnt = theNumberOfPnts;

  //-----------------------------------------------------------------------
  // Si le point d intersection est quasi confondue avec une des extremites
  // de l une ou l autre des bisectrices, l intersection n est pas validee.
  //
  // SAUF si une des bisectrices est issue d une connexion et que les 
  // edges separes par les bissectrices sont des voisines sur le contour
  // initiales.
  // en effet le milieu de la connexion P qui est l origine d une des 
  // bissectrices peut etre sur l autre bissectrice. 
  // P est donc point d intersection
  // et la bissectrice issue de la connexion est de longueur nulle.
  // (ex : un rectangle dans un rectangle ou la connexion est entre un coin
  // et un cote).
  //-----------------------------------------------------------------------

  Standard_Integer IndexEdge1,IndexEdge2,IndexEdge3,IndexEdge4;
  Standard_Boolean ExtremiteControle = Standard_True;

  IndexEdge1 = BisectorOne->FirstEdge() ->EdgeNumber();
  IndexEdge2 = BisectorOne->SecondEdge()->EdgeNumber();
  IndexEdge3 = BisectorTwo->FirstEdge() ->EdgeNumber();
  IndexEdge4 = BisectorTwo->SecondEdge()->EdgeNumber();

  if (theCircuit->ConnexionOn(IndexEdge2)){
    // --------------------------------------
    // BisectorOne est issue d une connexion.  
    // --------------------------------------
    if (AreNeighbours(IndexEdge1,IndexEdge2,NumberOfItems()) && 
      AreNeighbours(IndexEdge3,IndexEdge4,NumberOfItems()) && 
      IndexEdge2 == IndexEdge3                               ){
        ExtremiteControle = Standard_False;
        Param1             = Param1 + Tolerance;
    }
  }

  if (theCircuit->ConnexionOn(IndexEdge4)){
     //--------------------------------------
     //BisectorTwo est issue d une connexion.   
     //--------------------------------------
    if (AreNeighbours(IndexEdge1,IndexEdge2,NumberOfItems()) && 
      AreNeighbours(IndexEdge3,IndexEdge4,NumberOfItems()) &&
      IndexEdge2 == IndexEdge3                               ){
        ExtremiteControle = Standard_False;
        Param2            = Param2 + Tolerance;
    }
  }

  //if (ExtremiteControle) {
  //  if(Bisector1->StartPoint().Distance(PointSolution) < Tolerance ||
  //    Bisector2->StartPoint().Distance(PointSolution) < Tolerance  ) 
  //    return Precision::Infinite();
  //}

  if(ExtremiteControle)
  {
    if(Bisector1->StartPoint().Distance(PointSolution) < Tolerance)
    {
#ifdef DRAW
      if(AffichBis)
      {
        DrawTrSurf::Set("Bis1", Bisector1);
        DrawTrSurf::Set("Bis2", Bisector2);
      }
#endif
      return Precision::Infinite();
    }
    if(Bisector2->StartPoint().Distance(PointSolution) < Tolerance)
    {
        
#ifdef DRAW
      if(AffichBis)
      {
        DrawTrSurf::Set("Bis1", Bisector1);
        DrawTrSurf::Set("Bis2", Bisector2);
      }
#endif
      return Precision::Infinite();
    }
  }



  if(BisectorOne->SecondParameter() < Precision::Infinite() &&
    BisectorOne->SecondParameter() < Param1*(1. - Tolerance )) 
    return Precision::Infinite();

  if(BisectorTwo->FirstParameter() < Precision::Infinite() &&
    BisectorTwo->FirstParameter() < Param2*(1.- Tolerance)) 
    return Precision::Infinite();

  BisectorOne->SecondParameter(Param1);
  BisectorTwo->FirstParameter (Param2);

  
#ifdef OCCT_DEBUG
  if (Affich) {
    cout<<"   coordonnees    : "<<GeomPnt  (IntPnt).X()<<" "
      <<GeomPnt  (IntPnt).Y()<<endl;
    cout<<"   parametres     : "<<Param1<<" "<<Param2<<endl;
    cout<<"   distancemini   : "<<DistanceMini<<endl;
  }
#endif

  return DistanceMini;
}

//=============================================================================
//function : Distance
//purpose  :
//=============================================================================
Standard_Real MAT2d_Tool2d::Distance(const Handle(MAT_Bisector)& Bis,
  const Standard_Real         Param1,
  const Standard_Real         Param2) const
{
  Standard_Real Dist = Precision::Infinite();

  if (Param1 != Precision::Infinite() && Param2 != Precision::Infinite()) {
    gp_Pnt2d P1 = GeomBis(Bis->BisectorNumber()).Value()->Value(Param1);
    gp_Pnt2d P2 = GeomBis(Bis->BisectorNumber()).Value()->Value(Param2);
    Dist        = P1.Distance(P2);
  }
  return Dist;
}

//=============================================================================
//function : Dump
//purpose  :
//=============================================================================
#ifndef OCCT_DEBUG
void MAT2d_Tool2d::Dump(const Standard_Integer ,
  const Standard_Integer ) const
{
  throw Standard_NotImplemented();
#else
void MAT2d_Tool2d::Dump(const Standard_Integer bisector,
  const Standard_Integer) const
{
  if(bisector == -1) return;
  if(bisector > theNumberOfBisectors) return;

  Handle(Geom2d_Curve) thebisector = (Handle(Geom2d_Curve)) GeomBis(bisector).Value();

  MAT2d_DrawCurve(thebisector,3);

#endif
}


//=============================================================================
//function : GeomBis
//purpose  :
//=============================================================================
const Bisector_Bisec&  MAT2d_Tool2d::GeomBis (const Standard_Integer Index) 
  const
{
  return theGeomBisectors.Find(Index);
}

//=============================================================================
//function : ChangeGeomBis
//purpose  :
//=============================================================================
Bisector_Bisec&  MAT2d_Tool2d::ChangeGeomBis(const Standard_Integer Index)
{
  return theGeomBisectors.ChangeFind(Index);
}


//=============================================================================
//function : GeomElt
//purpose  :
//=============================================================================
Handle(Geom2d_Geometry)  MAT2d_Tool2d::GeomElt(const Standard_Integer Index)
  const 
{
  return  theCircuit->Value(Index);
}


//=============================================================================
//function : GeomPnt
//purpose  :
//=============================================================================
const gp_Pnt2d&  MAT2d_Tool2d::GeomPnt(const Standard_Integer Index) const
{
  return theGeomPnts.Find(Index);
}

//=============================================================================
//function : GeomVec
//purpose  :
//=============================================================================
const gp_Vec2d&  MAT2d_Tool2d::GeomVec(const Standard_Integer Index)const 
{
  return theGeomVecs.Find(Index);
}

//=============================================================================
//function : Circuit
//purpose  :
//=============================================================================
Handle(MAT2d_Circuit) MAT2d_Tool2d::Circuit()const 
{
  return theCircuit;
}

//=============================================================================
//function : BisecFusion
//purpose  :
//=============================================================================
void MAT2d_Tool2d::BisecFusion(const Standard_Integer I1,
  const Standard_Integer I2) 
{
  Standard_Real               DU,UL1,UF1;
  Handle(Geom2d_TrimmedCurve) Bisector1;
  Handle(Geom2d_TrimmedCurve) Bisector2;

  Bisector1 = GeomBis(I1).Value();
  Bisector2 = GeomBis(I2).Value();
  UF1       = Bisector1->FirstParameter();
  UL1       = Bisector1->LastParameter();

  Handle(Standard_Type) Type1 = Bisector1->BasisCurve()->DynamicType();
  if (Type1 == STANDARD_TYPE(Bisector_BisecCC)) {
    //------------------------------------------------------------------------------------
    // les bissectrice courbe/courbe sont  construites avec un point de depart
    // elles ne peuvent pas etre trimes par un point se trouvant de l autre cote du
    // point de depart.
    // pour faire la fusion des deux bissectrices on reconstruit la bissectrice entre les
    // deux courbes avec comme point de depart le dernier point de la Bisector2.
    // on trime ensuite la courbe par le dernier point de Bisector1.
    //------------------------------------------------------------------------------------
    Standard_Real            Tolerance    = MAT2d_TOLCONF;
    Bisector_Bisec           Bis;
    gp_Vec2d                 VBid(1,0);
    gp_Pnt2d                 P2   = Bisector2->Value(Bisector2->LastParameter());     
    gp_Pnt2d                 P1   = Bisector1->Value(Bisector1->LastParameter());   
    Handle(Bisector_BisecCC) BCC1 = Handle(Bisector_BisecCC)::DownCast(Bisector1->BasisCurve());

    Bis.Perform(BCC1->Curve(2), BCC1->Curve(1), P2, VBid, VBid, 
		theDirection, theJoinType, Tolerance, Standard_False); 

    Bisector1 = Bis.Value();
    BCC1      = Handle(Bisector_BisecCC)   ::DownCast(Bisector1->BasisCurve());	
    UF1       = BCC1->FirstParameter();
    UL1       = BCC1->Parameter(P1);
    Bisector1->SetTrim(UF1,UL1);
    theGeomBisectors.Bind(I1,Bis);
  }
  else {
    DU        = Bisector2->LastParameter() - Bisector2->FirstParameter();
    UF1       = UF1 - DU;

    Handle(Bisector_BisecAna) BAna = Handle(Bisector_BisecAna)::DownCast(Bisector1->BasisCurve());
    //---------------------------- uncomment if new method Bisector_BisecAna::SetTrim(f,l) is not used
    //    Handle(Geom2d_Curve) C2d = BAna->Geom2dCurve();
    //    Handle(Geom2d_TrimmedCurve) trimC2d = new Geom2d_TrimmedCurve(C2d, UF1, UL1);
    //    BAna->Init(trimC2d);
    //--------------------------- end
    BAna->SetTrim(UF1,UL1); // put comment if SetTrim(f,l) is not used

    Bisector1->SetTrim(UF1,UL1);
  }
}

//=============================================================================
//function : Type
//purpose  :
//=============================================================================
static Handle(Standard_Type) Type(const Handle(Geom2d_Geometry)& aGeom) 
{
  Handle(Standard_Type) type = aGeom->DynamicType();
  Handle(Geom2d_Curve)  curve;

  if (type == STANDARD_TYPE(Geom2d_TrimmedCurve)) {
    curve = Handle(Geom2d_TrimmedCurve)::DownCast(aGeom)->BasisCurve();
    type  = curve->DynamicType();
  }
  return type;
}

//==========================================================================
//function : AreNeighbours
//purpose  : Return TRUE si IEdge1 et IEdge2 correspondent a des elements 
//           consecutifs sur un contour ferme de NbEdge elements.
//==========================================================================
Standard_Boolean AreNeighbours(const Standard_Integer IEdge1,
  const Standard_Integer IEdge2,
  const Standard_Integer NbEdge)
{
  if      (Abs(IEdge1 - IEdge2) == 1)         return Standard_True;
  else if (Abs(IEdge1 - IEdge2) == NbEdge -1) return Standard_True;
  else                                        return Standard_False; 
}

//==========================================================================
//function : SetTrim
//purpose  :
//==========================================================================
static void SetTrim(Bisector_Bisec& Bis, const Handle(Geom2d_Curve)& Line1)
{  
  Geom2dInt_GInter Intersect; 
  Standard_Real    Distance;  
  Standard_Real    Tolerance = MAT2d_TOLCONF;  
  Handle(Geom2d_TrimmedCurve) Bisector = Bis.ChangeValue();

  IntRes2d_Domain  Domain1   = Domain(Bisector,Tolerance);
  Standard_Real    UB1       = Bisector->FirstParameter();
  Standard_Real    UB2       = Bisector->LastParameter();

  gp_Pnt2d         FirstPointBisector = Bisector->Value(UB1);
  Standard_Real    UTrim              = Precision::Infinite();

  Geom2dAdaptor_Curve AdapBisector(Bisector);
  Geom2dAdaptor_Curve AdapLine1   (Line1);
  Intersect.Perform(AdapBisector, Domain1, 
    AdapLine1, Tolerance, Tolerance);

  if (Intersect.IsDone() && !Intersect.IsEmpty()) {
    for (Standard_Integer i = 1; i <= Intersect.NbPoints(); i++) {
      gp_Pnt2d PInt = Intersect.Point(i).Value();
      Distance      = FirstPointBisector.Distance(PInt);
      if (Distance > 10.*Tolerance                     && 
        Intersect.Point(i).ParamOnFirst() < UTrim ) {
          UTrim = Intersect.Point(i).ParamOnFirst();
      }
    }
  } 
  // ------------------------------------------------------------------------
  // Restriction de la Bissectrice par le point d intersection de plus petit
  // parametre.
  // ------------------------------------------------------------------------
  if (UTrim < UB2 && UTrim > UB1) Bisector->SetTrim(UB1,UTrim);
}

//==========================================================================
//function : Domain
//purpose  :
//==========================================================================
IntRes2d_Domain  Domain(const Handle(Geom2d_TrimmedCurve)& Bisector1,
  const Standard_Real                Tolerance)
{
  Standard_Real Param1 = Bisector1->FirstParameter();
  Standard_Real Param2 = Bisector1->LastParameter();
  if(Param2 > 10000.) {
    Param2 = 10000.;
    Handle(Standard_Type) Type1 = Type(Bisector1->BasisCurve());    
    Handle(Geom2d_Curve)  BasisCurve;
    if (Type1 == STANDARD_TYPE(Bisector_BisecAna)) {
      BasisCurve = Handle(Bisector_BisecAna)
        ::DownCast(Bisector1->BasisCurve())->Geom2dCurve();
      Type1      = BasisCurve->DynamicType();
    }
    gp_Parab2d gpParabola;
    gp_Hypr2d  gpHyperbola;
    Standard_Real Focus;
    Standard_Real Limit = 50000.;
    if (Type1 == STANDARD_TYPE(Geom2d_Parabola)) {
      gpParabola = Handle(Geom2d_Parabola)::DownCast(BasisCurve)->Parab2d();
      Focus = gpParabola.Focal();
      Standard_Real Val1 = Sqrt(Limit*Focus);
      Standard_Real Val2 = Sqrt(Limit*Limit);
      Param2 = (Val1 <= Val2 ? Val1:Val2);
    }
    else if (Type1 == STANDARD_TYPE(Geom2d_Hyperbola)) {
      gpHyperbola = Handle(Geom2d_Hyperbola)::DownCast(BasisCurve)->Hypr2d();
      Standard_Real Majr  = gpHyperbola.MajorRadius();
      Standard_Real Minr  = gpHyperbola.MinorRadius();
      Standard_Real Valu1 = Limit/Majr;
      Standard_Real Valu2 = Limit/Minr;
      Standard_Real Val1  = Log(Valu1+Sqrt(Valu1*Valu1-1));
      Standard_Real Val2  = Log(Valu2+Sqrt(Valu2*Valu2+1));
      Param2 = (Val1 <= Val2 ? Val1:Val2);
    }
  }

  IntRes2d_Domain Domain1(Bisector1->Value(Param1),Param1,Tolerance,
    Bisector1->Value(Param2),Param2,Tolerance);
  if(Bisector1->BasisCurve()->IsPeriodic()) {
    Domain1.SetEquivalentParameters(0.,2.*M_PI);
  }
  return Domain1;
}

//=============================================================================
//function : CheckEnds
//purpose  :
//=============================================================================
Standard_Boolean CheckEnds (const Handle(Geom2d_Geometry)& Elt    ,
                            const gp_Pnt2d&                PCom   ,
                            const Standard_Real            Distance,
                            const Standard_Real            Tol) 
 
{  
  Handle(Standard_Type)       Type   = Elt->DynamicType();	
  Handle(Geom2d_TrimmedCurve) Curve; 

  if (Type == STANDARD_TYPE(Geom2d_CartesianPoint)) {	
    return Standard_False;
  }
  else {
    Curve    = Handle(Geom2d_TrimmedCurve)::DownCast(Elt);	
    gp_Pnt2d aPf = Curve->StartPoint();
    gp_Pnt2d aPl = Curve->EndPoint();
    Standard_Real df = PCom.Distance(aPf);
    Standard_Real dl = PCom.Distance(aPl);
    if(Abs(df - Distance) <= Tol)
      return Standard_True;
    if(Abs(dl - Distance) <= Tol)
      return Standard_True;
  }
  return Standard_False;
}


#ifdef OCCT_DEBUG
//==========================================================================
//function : MAT2d_DrawCurve
//purpose  : Affichage d une courbe <aCurve> de Geom2d. dans une couleur
//           definie par <Indice>.
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
  Standard_Integer Indice = 1;
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

  //dout << dr;
  //dout.Flush();
#endif
}

#endif

