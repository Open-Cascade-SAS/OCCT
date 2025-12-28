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

#define Debug(expr) std::cout << " MAT2d_Tool2d.cxx  :  expr :" << expr << std::endl;
// #define OCCT_DEBUG
// #define DRAW
#ifdef DRAW
  #include <DBRep.hxx>
  #include <DrawTrSurf.hxx>
  #include <stdio.h>
#endif

#ifdef DRAW
  #include <Draw_Appli.hxx>
  #include <DrawTrSurf_Curve2d.hxx>
  #include <GCE2d_MakeSegment.hxx>
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
#include <gp_Hypr2d.hxx>
#include <gp_Parab2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <IntRes2d_Domain.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <IntRes2d_IntersectionSegment.hxx>
#include <MAT2d_Circuit.hxx>
#include <MAT2d_Connexion.hxx>
#include <MAT2d_Tool2d.hxx>
#include <MAT_Bisector.hxx>
#include <MAT_Edge.hxx>
#include <Precision.hxx>
#include <Standard_NotImplemented.hxx>
#include <StdFail_NotDone.hxx>
#include <NCollection_Array1.hxx>

#ifdef DRAW
static occ::handle<DrawTrSurf_Curve2d> draw;
static int           AffichBis = false;
#endif
#ifdef OCCT_DEBUG
static void MAT2d_DrawCurve(const occ::handle<Geom2d_Curve>& aCurve, const int Indice);
static bool Store      = false;
static bool AffichDist = false;
#endif

//=====================================================================
//  static functions
//=====================================================================
static IntRes2d_Domain Domain(const occ::handle<Geom2d_TrimmedCurve>& Bisector1,
                              const double                Tolerance);

static occ::handle<Standard_Type> Type(const occ::handle<Geom2d_Geometry>& acurve);

static bool AreNeighbours(const int IEdge1,
                                      const int IEdge2,
                                      const int NbEdge);

static void             SetTrim(Bisector_Bisec& Bis, const occ::handle<Geom2d_Curve>& Line1);
static bool CheckEnds(const occ::handle<Geom2d_Geometry>& Elt,
                                  const gp_Pnt2d&                PCom,
                                  const double            Distance,
                                  const double            Tol);

static double MAT2d_TOLCONF = 1.e-7;

//=================================================================================================

MAT2d_Tool2d::MAT2d_Tool2d()
{
  theDirection         = 1.;
  theJoinType          = GeomAbs_Arc; // default
  theNumberOfBisectors = 0;
  theNumberOfVecs      = 0;
  theNumberOfPnts      = 0;
}

//=================================================================================================

void MAT2d_Tool2d::InitItems(const occ::handle<MAT2d_Circuit>& EquiCircuit)
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

//=================================================================================================

void MAT2d_Tool2d::Sense(const MAT_Side aside)
{
  if (aside == MAT_Left)
    theDirection = 1.;
  else
    theDirection = -1.;
}

//=================================================================================================

void MAT2d_Tool2d::SetJoinType(const GeomAbs_JoinType aJoinType)
{
  theJoinType = aJoinType;
}

//=================================================================================================

int MAT2d_Tool2d::NumberOfItems() const
{
  return theCircuit->NumberOfItems();
}

//=================================================================================================

double MAT2d_Tool2d::ToleranceOfConfusion() const
{
  return 2 * MAT2d_TOLCONF;
}

//=================================================================================================

int MAT2d_Tool2d::FirstPoint(const int anitem, double& dist)
{
  occ::handle<Geom2d_Curve> curve;
  occ::handle<Geom2d_Point> point;
  theNumberOfPnts++;

  if (theCircuit->ConnexionOn(anitem))
  {
    gp_Pnt2d P1 = theCircuit->Connexion(anitem)->PointOnFirst();
    gp_Pnt2d P2 = theCircuit->Connexion(anitem)->PointOnSecond();
    theGeomPnts.Bind(theNumberOfPnts, gp_Pnt2d((P1.X() + P2.X()) * 0.5, (P1.Y() + P2.Y()) * 0.5));
    dist = P1.Distance(P2) * 0.5;
    return theNumberOfPnts;
  }

  occ::handle<Standard_Type> type;
  type = theCircuit->Value(anitem)->DynamicType();
  dist = 0.;

  if (type != STANDARD_TYPE(Geom2d_CartesianPoint))
  {
    curve = occ::down_cast<Geom2d_Curve>(theCircuit->Value(anitem));
    theGeomPnts.Bind(theNumberOfPnts, curve->Value(curve->FirstParameter()));
  }
  else
  {
    point = occ::down_cast<Geom2d_Point>(theCircuit->Value(anitem));
    theGeomPnts.Bind(theNumberOfPnts, point->Pnt2d());
  }
  return theNumberOfPnts;
}

//=================================================================================================

int MAT2d_Tool2d::TangentBefore(const int anitem,
                                             const bool IsOpenResult)
{
  int     item;
  occ::handle<Geom2d_Curve> curve;
  theNumberOfVecs++;

  if (!IsOpenResult)
    item = (anitem == theCircuit->NumberOfItems()) ? 1 : (anitem + 1);
  else
    item = (anitem == theCircuit->NumberOfItems()) ? (anitem - 1) : (anitem + 1);
  if (theCircuit->ConnexionOn(item))
  {
    double x1, y1, x2, y2;
    theCircuit->Connexion(item)->PointOnFirst().Coord(x1, y1);
    theCircuit->Connexion(item)->PointOnSecond().Coord(x2, y2);
    theGeomVecs.Bind(theNumberOfVecs, gp_Vec2d((x2 - x1), (y2 - y1)));
    return theNumberOfVecs;
  }

  occ::handle<Standard_Type> type;
  type = theCircuit->Value(anitem)->DynamicType();
  if (type != STANDARD_TYPE(Geom2d_CartesianPoint))
  {
    curve = occ::down_cast<Geom2d_Curve>(theCircuit->Value(anitem));
#ifdef DRAW
    char* name = new char[100];
    Sprintf(name, "c%d", anitem);
    DrawTrSurf::Set(name, curve);
    delete[] name;
#endif
    theGeomVecs.Bind(theNumberOfVecs, curve->DN(curve->LastParameter(), 1));
  }
  else
  {
    curve = occ::down_cast<Geom2d_Curve>(theCircuit->Value(item));
#ifdef DRAW
    char* name = new char[100];
    Sprintf(name, "c%d", item);
    DrawTrSurf::Set(name, curve);
    delete[] name;
#endif
    double param = (IsOpenResult && anitem == theCircuit->NumberOfItems())
                            ? curve->LastParameter()
                            : curve->FirstParameter();
    theGeomVecs.Bind(theNumberOfVecs, curve->DN(param, 1));
  }

  return theNumberOfVecs;
}

//=================================================================================================

int MAT2d_Tool2d::TangentAfter(const int anitem,
                                            const bool IsOpenResult)
{
  int     item;
  occ::handle<Geom2d_Curve> curve;
  gp_Vec2d             thevector;
  theNumberOfVecs++;

  if (theCircuit->ConnexionOn(anitem))
  {
    double x1, y1, x2, y2;
    theCircuit->Connexion(anitem)->PointOnFirst().Coord(x1, y1);
    theCircuit->Connexion(anitem)->PointOnSecond().Coord(x2, y2);
    theGeomVecs.Bind(theNumberOfVecs, gp_Vec2d((x1 - x2), (y1 - y2)));
    return theNumberOfVecs;
  }

  occ::handle<Standard_Type> type;
  type = theCircuit->Value(anitem)->DynamicType();
  if (type != STANDARD_TYPE(Geom2d_CartesianPoint))
  {
    curve = occ::down_cast<Geom2d_Curve>(theCircuit->Value(anitem));
#ifdef DRAW
    char* name = new char[100];
    Sprintf(name, "c%d", anitem);
    DrawTrSurf::Set(name, curve);
    delete[] name;
#endif
    thevector = curve->DN(curve->FirstParameter(), 1);
  }
  else
  {
    if (!IsOpenResult)
      item = (anitem == 1) ? theCircuit->NumberOfItems() : (anitem - 1);
    else
      item = (anitem == 1) ? 2 : (anitem - 1);

    curve = occ::down_cast<Geom2d_Curve>(theCircuit->Value(item));
#ifdef DRAW
    char* name = new char[100];
    Sprintf(name, "c%d", item);
    DrawTrSurf::Set(name, curve);
    delete[] name;
#endif
    double param =
      (IsOpenResult && anitem == 1) ? curve->FirstParameter() : curve->LastParameter();
    thevector = curve->DN(param, 1);
  }
  theGeomVecs.Bind(theNumberOfVecs, thevector.Reversed());
  return theNumberOfVecs;
}

//=================================================================================================

int MAT2d_Tool2d::Tangent(const int bisector)
{
  theNumberOfVecs++;
  theGeomVecs.Bind(theNumberOfVecs,
                   GeomBis(bisector).Value()->DN(GeomBis(bisector).Value()->LastParameter(), 1));
  return theNumberOfVecs;
}

//=================================================================================================

void MAT2d_Tool2d::CreateBisector(const occ::handle<MAT_Bisector>& abisector)
{
  occ::handle<Geom2d_Point>    point1, point2;
  occ::handle<Geom2d_Geometry> elt1, elt2;
  Bisector_Bisec          bisector;
  double           tolerance = MAT2d_TOLCONF;

  int edge1number      = abisector->FirstEdge()->EdgeNumber();
  int edge2number      = abisector->SecondEdge()->EdgeNumber();
  bool ontheline        = AreNeighbours(edge1number, edge2number, NumberOfItems());
  bool InitialNeighbour = ontheline;

  if (theCircuit->ConnexionOn(edge2number))
    ontheline = false;

  elt1 = theCircuit->Value(edge1number);
  elt2 = theCircuit->Value(edge2number);

  occ::handle<Standard_Type> type1;
  type1 = theCircuit->Value(edge1number)->DynamicType();
  occ::handle<Standard_Type> type2;
  type2 = theCircuit->Value(edge2number)->DynamicType();
  occ::handle<Geom2d_Curve> item1;
  occ::handle<Geom2d_Curve> item2;

  if (type1 != STANDARD_TYPE(Geom2d_CartesianPoint))
  {
    item1 = occ::down_cast<Geom2d_Curve>(elt1);
  }

  if (type2 != STANDARD_TYPE(Geom2d_CartesianPoint))
  {
    item2 = occ::down_cast<Geom2d_Curve>(elt2);
  }

#ifdef OCCT_DEBUG
  bool Affich = false;
  if (Affich)
  {
    std::cout << std::endl;
    std::cout << "BISECTOR number :  " << theNumberOfBisectors + 1 << std::endl;
    std::cout << "  Item 1 : " << std::endl;
    std::cout << edge1number << std::endl;
    std::cout << std::endl;
    //    elt1->Dump(1,1);
    std::cout << std::endl;
    std::cout << "  Item 2 : " << std::endl;
    std::cout << edge2number << std::endl;
    std::cout << std::endl;
    //  elt2->Dump(1,1);
    std::cout << std::endl;
  }
#endif

  if (type1 != STANDARD_TYPE(Geom2d_CartesianPoint)
      && type2 != STANDARD_TYPE(Geom2d_CartesianPoint))
  {
    bisector.Perform(item1,
                     item2,
                     GeomPnt(abisector->IssuePoint()),
                     GeomVec(abisector->FirstVector()),
                     GeomVec(abisector->SecondVector()),
                     theDirection,
                     theJoinType,
                     tolerance,
                     ontheline);
  }
  else if (type1 == STANDARD_TYPE(Geom2d_CartesianPoint)
           && type2 == STANDARD_TYPE(Geom2d_CartesianPoint))
  {
    point1 = occ::down_cast<Geom2d_Point>(elt1);
    point2 = occ::down_cast<Geom2d_Point>(elt2);
    bisector.Perform(point1,
                     point2,
                     GeomPnt(abisector->IssuePoint()),
                     GeomVec(abisector->FirstVector()),
                     GeomVec(abisector->SecondVector()),
                     theDirection,
                     tolerance,
                     ontheline);
  }
  else if (type1 == STANDARD_TYPE(Geom2d_CartesianPoint))
  {
    point1 = occ::down_cast<Geom2d_Point>(elt1);
    bisector.Perform(point1,
                     item2,
                     GeomPnt(abisector->IssuePoint()),
                     GeomVec(abisector->FirstVector()),
                     GeomVec(abisector->SecondVector()),
                     theDirection,
                     tolerance,
                     ontheline);
  }
  else
  {
    point2 = occ::down_cast<Geom2d_Point>(elt2);
    bisector.Perform(item1,
                     point2,
                     GeomPnt(abisector->IssuePoint()),
                     GeomVec(abisector->FirstVector()),
                     GeomVec(abisector->SecondVector()),
                     theDirection,
                     tolerance,
                     ontheline);
  }

  //------------------------------
  // Restriction de la bisectrice.
  //-----------------------------
  TrimBisec(bisector, edge1number, InitialNeighbour, 1);
  TrimBisec(bisector, edge2number, InitialNeighbour, 2);

  theNumberOfBisectors++;
  theGeomBisectors.Bind(theNumberOfBisectors, bisector);

  abisector->BisectorNumber(theNumberOfBisectors);
  abisector->Sense(1);

#ifdef DRAW
  char* name = new char[100];
  Sprintf(name, "b%d", theNumberOfBisectors);
  DrawTrSurf::Set(name, bisector.Value());
  Dump(abisector->BisectorNumber(), 1);
  delete[] name;
#endif

#ifdef OCCT_DEBUG
  bool AffichDraw = false;
  if (AffichDraw)
    Dump(abisector->BisectorNumber(), 1);
  if (Store)
  {
    occ::handle<Standard_Type> Type1 = Type(bisector.Value()->BasisCurve());
    occ::handle<Geom2d_Curve>  BasisCurve;
    if (Type1 == STANDARD_TYPE(Bisector_BisecAna))
    {
      BasisCurve =
        occ::down_cast<Bisector_BisecAna>(bisector.Value()->BasisCurve())->Geom2dCurve();
  #ifdef DRAW
      char* name = new char[100];
      Sprintf(name, "BISSEC_%d", abisector->BisectorNumber());
      DrawTrSurf::Set(name, BasisCurve);
      delete[] name;
  #endif
    }
  }
#endif
}

//=============================================================================
// function : TrimBisec
// purpose  : Restriction de la bisectrice.
//           Restriction des bissectrice separant deux elements lies par une
//           connexion ou l un au moins des elements est un cercle.
//           Cette restriction est necessaire a la logique de l algorithme.
//=============================================================================
void MAT2d_Tool2d::TrimBisec(Bisector_Bisec&        B1,
                             const int IndexEdge,
                             const bool InitialNeighbour,
                             const int StartOrEnd) const
{
  occ::handle<Geom2d_Curve>        Curve;
  occ::handle<Geom2d_TrimmedCurve> LineSupportDomain, Line;
  occ::handle<Geom2d_Line>         Line1, Line2;

  // gp_Vec2d             Tan1,Tan2;
  gp_Pnt2d         Ori; // PEdge;
  int INext;
  INext = (IndexEdge == theCircuit->NumberOfItems()) ? 1 : (IndexEdge + 1);

  occ::handle<Standard_Type> EdgeType = theCircuit->Value(IndexEdge)->DynamicType();

  if (EdgeType != STANDARD_TYPE(Geom2d_CartesianPoint))
  {
    if (!InitialNeighbour)
    {
      Curve    = occ::down_cast<Geom2d_TrimmedCurve>(theCircuit->Value(IndexEdge))->BasisCurve();
      EdgeType = Curve->DynamicType();
      //-------------------------------------------------------------------
      // si l edge est liee a sa voisine  precedente par une connexion.
      //-------------------------------------------------------------------
      if (theCircuit->ConnexionOn(IndexEdge) && StartOrEnd == 1)
      {
        if (EdgeType == STANDARD_TYPE(Geom2d_Circle))
        {
          Ori         = occ::down_cast<Geom2d_Circle>(Curve)->Location();
          gp_Pnt2d P2 = theCircuit->Connexion(IndexEdge)->PointOnFirst();
          Line1       = new Geom2d_Line(Ori, gp_Dir2d(P2.X() - Ori.X(), P2.Y() - Ori.Y()));
        }
      }
      //-----------------------------------------------------------------------
      // Si l edge est liee a sa voisine suivante par une connexion.
      //-----------------------------------------------------------------------
      if (theCircuit->ConnexionOn(INext) && StartOrEnd == 2)
      {
        if (EdgeType == STANDARD_TYPE(Geom2d_Circle))
        {
          Ori         = occ::down_cast<Geom2d_Circle>(Curve)->Location();
          gp_Pnt2d P2 = theCircuit->Connexion(INext)->PointOnSecond();
          Line2       = new Geom2d_Line(Ori, gp_Dir2d(P2.X() - Ori.X(), P2.Y() - Ori.Y()));
        }
      }
      if (Line1.IsNull() && Line2.IsNull())
        return;

      //-----------------------------------------------------------------------
      // Restriction de la bisectrice par les demi-droites liees aux connexions
      // si elles existent.
      //-----------------------------------------------------------------------
      if (!Line1.IsNull())
      {
        Line = new Geom2d_TrimmedCurve(Line1, 0., Precision::Infinite());
        SetTrim(B1, Line);
      }
      if (!Line2.IsNull())
      {
        Line = new Geom2d_TrimmedCurve(Line2, 0., Precision::Infinite());
        SetTrim(B1, Line);
      }
    }
  }
}

//=================================================================================================

bool MAT2d_Tool2d::TrimBisector(const occ::handle<MAT_Bisector>& abisector)
{
  double param = abisector->FirstParameter();

#ifdef OCCT_DEBUG
  bool Affich = false;
  if (Affich)
    std::cout << "TRIM de " << abisector->BisectorNumber() << std::endl;
#endif

  occ::handle<Geom2d_TrimmedCurve> bisector = ChangeGeomBis(abisector->BisectorNumber()).ChangeValue();

  if (bisector->BasisCurve()->IsPeriodic() && param == Precision::Infinite())
  {
    param = bisector->FirstParameter() + 2 * M_PI;
  }
  if (param > bisector->BasisCurve()->LastParameter())
  {
    param = bisector->BasisCurve()->LastParameter();
  }
  if (bisector->FirstParameter() == param)
    return false;

  bisector->SetTrim(bisector->FirstParameter(), param);
  return true;
}

//=================================================================================================

bool MAT2d_Tool2d::TrimBisector(const occ::handle<MAT_Bisector>& abisector,
                                            const int      apoint)
{
  double               Param;
  occ::handle<Geom2d_TrimmedCurve> Bisector = ChangeGeomBis(abisector->BisectorNumber()).ChangeValue();

  occ::handle<Bisector_Curve> Bis = occ::down_cast<Bisector_Curve>(Bisector->BasisCurve());

  //  Param = ParameterOnCurve(Bisector,theGeomPnts.Value(apoint));
  Param = Bis->Parameter(GeomPnt(apoint));

  if (Bisector->BasisCurve()->IsPeriodic())
  {
    if (Bisector->FirstParameter() > Param)
      Param = Param + 2 * M_PI;
  }
  if (Bisector->FirstParameter() >= Param)
    return false;
  if (Bisector->LastParameter() < Param)
    return false;
  Bisector->SetTrim(Bisector->FirstParameter(), Param);

#ifdef OCCT_DEBUG
  bool Affich = false;
  if (Affich)
    MAT2d_DrawCurve(Bisector, 2);
#endif

  return true;
}

//=================================================================================================

bool MAT2d_Tool2d::Projection(const int IEdge,
                                          const gp_Pnt2d&        PCom,
                                          double&         Distance) const
{
  gp_Pnt2d                    PEdge;
  occ::handle<Geom2d_Geometry>     Elt  = theCircuit->Value(IEdge);
  occ::handle<Standard_Type>       Type = Elt->DynamicType();
  occ::handle<Geom2d_TrimmedCurve> Curve;
  int            INext;
  double               Eps = MAT2d_TOLCONF; //*10.;

  if (Type == STANDARD_TYPE(Geom2d_CartesianPoint))
  {
    PEdge    = occ::down_cast<Geom2d_Point>(Elt)->Pnt2d();
    Distance = PCom.Distance(PEdge);
  }
  else
  {
    Distance = Precision::Infinite();
    Curve    = occ::down_cast<Geom2d_TrimmedCurve>(Elt);
    //-----------------------------------------------------------------------
    // Calcul des parametres MinMax sur l edge si celui ci est lies a ses
    // voisins par des connexions la courbe de calcul est limitee par
    // celles_ci.
    //-----------------------------------------------------------------------
    double ParamMin = Curve->FirstParameter();
    double ParamMax = Curve->LastParameter();
    if (theCircuit->ConnexionOn(IEdge))
    {
      ParamMin = theCircuit->Connexion(IEdge)->ParameterOnSecond();
    }
    INext = (IEdge == theCircuit->NumberOfItems()) ? 1 : (IEdge + 1);
    if (theCircuit->ConnexionOn(INext))
    {
      ParamMax = theCircuit->Connexion(INext)->ParameterOnFirst();
      if (Curve->BasisCurve()->IsPeriodic())
      {
        ElCLib::AdjustPeriodic(0., 2 * M_PI, Eps, ParamMin, ParamMax);
      }
    }
    //---------------------------------------------------------------------
    // Constuction de la courbe pour les extremas et ajustement des bornes.
    //---------------------------------------------------------------------
    Geom2dAdaptor_Curve C1(Curve);
    GeomAbs_CurveType   TypeC1 = C1.GetType();
    if (TypeC1 == GeomAbs_Circle)
    {
      double R       = C1.Circle().Radius();
      double EpsCirc = 100. * Eps;
      if (R < 1.)
        EpsCirc = Eps / R;
      if (((ParamMax - ParamMin + 2 * EpsCirc) < 2 * M_PI))
      {
        ParamMax = ParamMax + EpsCirc;
        ParamMin = ParamMin - EpsCirc;
      }
    }
    else
    {
      ParamMax = ParamMax + Eps;
      ParamMin = ParamMin - Eps;
    }
    //-----------------------------------------------------
    // Calcul des extremas et stockage minimum de distance.
    //-----------------------------------------------------
    Extrema_ExtPC2d Extremas(PCom, C1, ParamMin, ParamMax);
    if (Extremas.IsDone())
    {
      Distance = Precision::Infinite();
      if (Extremas.NbExt() < 1)
      {
        return false;
      }
      for (int i = 1; i <= Extremas.NbExt(); i++)
      {
        if (Extremas.SquareDistance(i) < Distance)
        {
          Distance = Extremas.SquareDistance(i);
        }
      }
      Distance = std::sqrt(Distance);
    }
    else
    {
      if (TypeC1 == GeomAbs_Circle)
      {
        Distance = C1.Circle().Radius();
      }
    }
  }
  return true;
}

//=================================================================================================

bool MAT2d_Tool2d::IsSameDistance(const occ::handle<MAT_Bisector>& BisectorOne,
                                              const occ::handle<MAT_Bisector>& BisectorTwo,
                                              const gp_Pnt2d&             PCom,
                                              double&              Distance) const
{
  NCollection_Array1<double> Dist(1, 4);
  const double  eps = 1.e-7;
  int     IEdge1, IEdge2, IEdge3, IEdge4;

  IEdge1 = BisectorOne->FirstEdge()->EdgeNumber();
  IEdge2 = BisectorOne->SecondEdge()->EdgeNumber();
  IEdge3 = BisectorTwo->FirstEdge()->EdgeNumber();
  IEdge4 = BisectorTwo->SecondEdge()->EdgeNumber();

  bool isDone1 = Projection(IEdge1, PCom, Dist(1));
  bool isDone2 = Projection(IEdge2, PCom, Dist(2));

  if (isDone1)
  {
    if (!isDone2)
    {
      occ::handle<Geom2d_Geometry> Elt = theCircuit->Value(IEdge2);
      double           Tol = std::max(Precision::Confusion(), eps * Dist(1));
      if (CheckEnds(Elt, PCom, Dist(1), Tol))
      {
        Dist(2) = Dist(1);
      }
    }
  }
  else
  {
    if (isDone2)
    {
      occ::handle<Geom2d_Geometry> Elt = theCircuit->Value(IEdge1);
      double           Tol = std::max(Precision::Confusion(), eps * Dist(2));
      if (CheckEnds(Elt, PCom, Dist(2), Tol))
      {
        Dist(1) = Dist(2);
      }
    }
  }

  bool isDone3 = true, isDone4 = true;
  if (IEdge3 == IEdge1)
    Dist(3) = Dist(1);
  else if (IEdge3 == IEdge2)
    Dist(3) = Dist(2);
  else
    isDone3 = Projection(IEdge3, PCom, Dist(3));

  if (IEdge4 == IEdge1)
    Dist(4) = Dist(1);
  else if (IEdge4 == IEdge2)
    Dist(4) = Dist(2);
  else
    isDone4 = Projection(IEdge4, PCom, Dist(4));
  //
  if (isDone3)
  {
    if (!isDone4)
    {
      occ::handle<Geom2d_Geometry> Elt = theCircuit->Value(IEdge4);
      double           Tol = std::max(Precision::Confusion(), eps * Dist(3));
      if (CheckEnds(Elt, PCom, Dist(3), Tol))
      {
        Dist(4) = Dist(3);
      }
    }
  }
  else
  {
    if (isDone4)
    {
      occ::handle<Geom2d_Geometry> Elt = theCircuit->Value(IEdge3);
      double           Tol = std::max(Precision::Confusion(), eps * Dist(4));
      if (CheckEnds(Elt, PCom, Dist(4), Tol))
      {
        Dist(3) = Dist(4);
      }
    }
  }

#ifdef OCCT_DEBUG
  if (AffichDist)
    for (int j = 1; j <= 4; j++)
    {
      std::cout << "Distance number : " << j << " is :" << Dist(j) << std::endl;
    }
#endif

  double EpsDist = MAT2d_TOLCONF * 300.;
  Distance              = Dist(1);
  if (theJoinType == GeomAbs_Intersection && Precision::IsInfinite(Distance))
  {
    for (int i = 2; i <= 4; i++)
      if (!Precision::IsInfinite(Dist(i)))
      {
        Distance = Dist(i);
        break;
      }
  }
  for (int i = 1; i <= 4; i++)
  {
    if (theJoinType == GeomAbs_Intersection && Precision::IsInfinite(Dist(i)))
      continue;
    if (std::abs(Dist(i) - Distance) > EpsDist)
    {
      Distance = Precision::Infinite();
      return false;
    }
  }
  return true;
}

//=================================================================================================

double MAT2d_Tool2d::IntersectBisector(const occ::handle<MAT_Bisector>& BisectorOne,
                                              const occ::handle<MAT_Bisector>& BisectorTwo,
                                              int&           IntPnt)
{
  double    Tolerance = MAT2d_TOLCONF;
  double    Param1, Param2;
  double    Parama, Paramb;
  double    Distance = 0., DistanceMini;
  bool SolutionValide;
  gp_Pnt2d         PointSolution;

  occ::handle<Geom2d_TrimmedCurve> Bisector1 =
    ChangeGeomBis(BisectorOne->BisectorNumber()).ChangeValue();

  occ::handle<Geom2d_TrimmedCurve> Bisector2 =
    ChangeGeomBis(BisectorTwo->BisectorNumber()).ChangeValue();

  if (Bisector1.IsNull() || Bisector2.IsNull())
    return Precision::Infinite();

  //-------------------------------------------------------------------------
  // Si les deux bissectrices separent des elements consecutifs et qu elles
  // sont issues des connexions C1 et C2.
  // Si C1 est la reverse de C2 ,alors les deux bissectrices sont issues
  // du meme point. Dans ce cas l intersection n est pas validee.
  //-------------------------------------------------------------------------
  int IS1 = BisectorOne->SecondEdge()->EdgeNumber();
  int IS2 = BisectorTwo->SecondEdge()->EdgeNumber();
  int IF1 = BisectorOne->FirstEdge()->EdgeNumber();
  int IF2 = BisectorTwo->FirstEdge()->EdgeNumber();

  if (AreNeighbours(IF1, IS1, NumberOfItems()) && AreNeighbours(IF2, IS2, NumberOfItems())
      && theCircuit->ConnexionOn(IS2) && theCircuit->ConnexionOn(IS1))
  {
    occ::handle<MAT2d_Connexion> C1, C2;
    C1 = theCircuit->Connexion(IS1);
    C2 = theCircuit->Connexion(IS2);
    if (C2->IndexFirstLine() == C1->IndexSecondLine()
        && C1->IndexFirstLine() == C2->IndexSecondLine())
      return Precision::Infinite();
  }

  // -----------------------------------------
  // Construction des domaines d intersection.
  // -----------------------------------------
  IntRes2d_Domain Domain1 = Domain(Bisector1, Tolerance);
  IntRes2d_Domain Domain2 = Domain(Bisector2, Tolerance);

  if (Domain1.LastParameter() - Domain1.FirstParameter() < Tolerance)
    return Precision::Infinite();
  if (Domain2.LastParameter() - Domain2.FirstParameter() < Tolerance)
    return Precision::Infinite();

#ifdef OCCT_DEBUG
  bool Affich = false;
  if (Affich)
  {
    std::cout << std::endl;
    std::cout << "INTERSECTION de " << BisectorOne->BisectorNumber() << " et de "
              << BisectorTwo->BisectorNumber() << std::endl;
    std::cout << "  Bisector 1 : " << std::endl;
    //    (Bisector1->BasisCurve())->Dump(-1,1);
    std::cout << std::endl;
    Debug(Domain1.FirstParameter());
    Debug(Domain1.LastParameter());
    std::cout << "-----------------" << std::endl;
    std::cout << "  Bisector 2 : " << std::endl;
    //    (Bisector2->BasisCurve())->Dump(-1,1);
    std::cout << std::endl;
    Debug(Domain2.FirstParameter());
    Debug(Domain2.LastParameter());
    std::cout << "-----------------" << std::endl;
  }
#endif

  // -------------------------
  // Calcul de l intersection.
  // -------------------------

  Bisector_Inter Intersect;
  Intersect.Perform(GeomBis(BisectorOne->BisectorNumber()),
                    Domain1,
                    GeomBis(BisectorTwo->BisectorNumber()),
                    Domain2,
                    Tolerance,
                    Tolerance,
                    true);

  //  Geom2dInt_GInter Intersect;
  //  Intersect.Perform(Bisector1,Domain1,Bisector2,Domain2,Tolerance,Tolerance);

  // -------------------------------------------------------------------------
  // Exploitation du resultat de l intersection et selection du point solution
  // equidistant des deux edges et le plus proche en parametre de l origine
  // des bissectrices.
  // -------------------------------------------------------------------------

  if (!Intersect.IsDone())
    return Precision::Infinite();

  if (Intersect.IsEmpty())
    return Precision::Infinite();

  DistanceMini   = Precision::Infinite();
  Param1         = Precision::Infinite();
  Param2         = Precision::Infinite();
  SolutionValide = false;

  if (Intersect.NbSegments() >= 1)
  {
    double MaxSegmentLength = 10. * Tolerance;
    for (int i = 1; i <= Intersect.NbSegments(); i++)
    {
      IntRes2d_IntersectionSegment Segment     = Intersect.Segment(i);
      bool             PointRetenu = false;
      gp_Pnt2d                     PointOnSegment;
      // ----------------------------------------------------------------
      // Si les segments sont petits, recherche des points sur le segment
      // equidistants des edges.
      // ----------------------------------------------------------------
      if ((Segment.HasFirstPoint() && Segment.HasLastPoint()))
      {
        gp_Pnt2d      P1, P2;
        double SegmentLength;
        P1            = Segment.FirstPoint().Value();
        P2            = Segment.LastPoint().Value();
        SegmentLength = P1.Distance(P2);
        if (SegmentLength <= Tolerance)
        {
          PointOnSegment = P1;
          if (IsSameDistance(BisectorOne, BisectorTwo, PointOnSegment, Distance))
            PointRetenu = true;
        }
        else if (SegmentLength <= MaxSegmentLength)
        {
          gp_Dir2d      Dir(P2.X() - P1.X(), P2.Y() - P1.Y());
          double Dist = 0.;
          while (Dist <= SegmentLength + Tolerance)
          {
            PointOnSegment = P1.Translated(Dist * Dir);
            if (IsSameDistance(BisectorOne, BisectorTwo, PointOnSegment, Distance))
            {
              PointRetenu = true;
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
      if (PointRetenu)
      {
        Parama =
          occ::down_cast<Bisector_Curve>(Bisector1->BasisCurve())->Parameter(PointOnSegment);
        Paramb =
          occ::down_cast<Bisector_Curve>(Bisector2->BasisCurve())->Parameter(PointOnSegment);
        if (Parama < Param1 && Paramb < Param2)
        {
          Param1         = Parama;
          Param2         = Paramb;
          DistanceMini   = Distance;
          PointSolution  = PointOnSegment;
          SolutionValide = true;
        }
      }
    }
  }

  if (Intersect.NbPoints() != 1)
  {
    for (int i = 1; i <= Intersect.NbPoints(); i++)
    {
      if (IsSameDistance(BisectorOne, BisectorTwo, Intersect.Point(i).Value(), Distance)
          && Distance > Tolerance)
      {
        Parama = Intersect.Point(i).ParamOnFirst();
        Paramb = Intersect.Point(i).ParamOnSecond();
        if (Parama < Param1 && Paramb < Param2)
        {
          Param1         = Parama;
          Param2         = Paramb;
          DistanceMini   = Distance;
          PointSolution  = Intersect.Point(i).Value();
          SolutionValide = true;
        }
      }
    }
  }
  else
  {
    PointSolution  = Intersect.Point(1).Value();
    Param1         = Intersect.Point(1).ParamOnFirst();
    Param2         = Intersect.Point(1).ParamOnSecond();
    SolutionValide = IsSameDistance(BisectorOne, BisectorTwo, PointSolution, DistanceMini);
  }

  if (!SolutionValide)
    return Precision::Infinite();
  theNumberOfPnts++;
  theGeomPnts.Bind(theNumberOfPnts, PointSolution);
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

  int IndexEdge1, IndexEdge2, IndexEdge3, IndexEdge4;
  bool ExtremiteControle = true;

  IndexEdge1 = BisectorOne->FirstEdge()->EdgeNumber();
  IndexEdge2 = BisectorOne->SecondEdge()->EdgeNumber();
  IndexEdge3 = BisectorTwo->FirstEdge()->EdgeNumber();
  IndexEdge4 = BisectorTwo->SecondEdge()->EdgeNumber();

  if (theCircuit->ConnexionOn(IndexEdge2))
  {
    // --------------------------------------
    // BisectorOne est issue d une connexion.
    // --------------------------------------
    if (AreNeighbours(IndexEdge1, IndexEdge2, NumberOfItems())
        && AreNeighbours(IndexEdge3, IndexEdge4, NumberOfItems()) && IndexEdge2 == IndexEdge3)
    {
      ExtremiteControle = false;
      Param1            = Param1 + Tolerance;
    }
  }

  if (theCircuit->ConnexionOn(IndexEdge4))
  {
    //--------------------------------------
    // BisectorTwo est issue d une connexion.
    //--------------------------------------
    if (AreNeighbours(IndexEdge1, IndexEdge2, NumberOfItems())
        && AreNeighbours(IndexEdge3, IndexEdge4, NumberOfItems()) && IndexEdge2 == IndexEdge3)
    {
      ExtremiteControle = false;
      Param2            = Param2 + Tolerance;
    }
  }

  // if (ExtremiteControle) {
  //   if(Bisector1->StartPoint().Distance(PointSolution) < Tolerance ||
  //     Bisector2->StartPoint().Distance(PointSolution) < Tolerance  )
  //     return Precision::Infinite();
  // }

  if (ExtremiteControle)
  {
    if (Bisector1->StartPoint().Distance(PointSolution) < Tolerance)
    {
#ifdef DRAW
      if (AffichBis)
      {
        DrawTrSurf::Set("Bis1", Bisector1);
        DrawTrSurf::Set("Bis2", Bisector2);
      }
#endif
      return Precision::Infinite();
    }
    if (Bisector2->StartPoint().Distance(PointSolution) < Tolerance)
    {

#ifdef DRAW
      if (AffichBis)
      {
        DrawTrSurf::Set("Bis1", Bisector1);
        DrawTrSurf::Set("Bis2", Bisector2);
      }
#endif
      return Precision::Infinite();
    }
  }

  if (BisectorOne->SecondParameter() < Precision::Infinite()
      && BisectorOne->SecondParameter() < Param1 * (1. - Tolerance))
    return Precision::Infinite();

  if (BisectorTwo->FirstParameter() < Precision::Infinite()
      && BisectorTwo->FirstParameter() < Param2 * (1. - Tolerance))
    return Precision::Infinite();

  BisectorOne->SecondParameter(Param1);
  BisectorTwo->FirstParameter(Param2);

#ifdef OCCT_DEBUG
  if (Affich)
  {
    std::cout << "   coordonnees    : " << GeomPnt(IntPnt).X() << " " << GeomPnt(IntPnt).Y()
              << std::endl;
    std::cout << "   parametres     : " << Param1 << " " << Param2 << std::endl;
    std::cout << "   distancemini   : " << DistanceMini << std::endl;
  }
#endif

  return DistanceMini;
}

//=================================================================================================

double MAT2d_Tool2d::Distance(const occ::handle<MAT_Bisector>& Bis,
                                     const double         Param1,
                                     const double         Param2) const
{
  double Dist = Precision::Infinite();

  if (Param1 != Precision::Infinite() && Param2 != Precision::Infinite())
  {
    gp_Pnt2d P1 = GeomBis(Bis->BisectorNumber()).Value()->Value(Param1);
    gp_Pnt2d P2 = GeomBis(Bis->BisectorNumber()).Value()->Value(Param2);
    Dist        = P1.Distance(P2);
  }
  return Dist;
}

//=================================================================================================

#ifndef OCCT_DEBUG
void MAT2d_Tool2d::Dump(const int, const int) const
{
  throw Standard_NotImplemented();
#else
void MAT2d_Tool2d::Dump(const int bisector, const int) const
{
  if (bisector == -1)
    return;
  if (bisector > theNumberOfBisectors)
    return;

  occ::handle<Geom2d_Curve> thebisector = (occ::handle<Geom2d_Curve>)GeomBis(bisector).Value();

  MAT2d_DrawCurve(thebisector, 3);

#endif
}

//=================================================================================================

const Bisector_Bisec& MAT2d_Tool2d::GeomBis(const int Index) const
{
  return theGeomBisectors.Find(Index);
}

//=================================================================================================

Bisector_Bisec& MAT2d_Tool2d::ChangeGeomBis(const int Index)
{
  return theGeomBisectors.ChangeFind(Index);
}

//=================================================================================================

occ::handle<Geom2d_Geometry> MAT2d_Tool2d::GeomElt(const int Index) const
{
  return theCircuit->Value(Index);
}

//=================================================================================================

const gp_Pnt2d& MAT2d_Tool2d::GeomPnt(const int Index) const
{
  return theGeomPnts.Find(Index);
}

//=================================================================================================

const gp_Vec2d& MAT2d_Tool2d::GeomVec(const int Index) const
{
  return theGeomVecs.Find(Index);
}

//=================================================================================================

occ::handle<MAT2d_Circuit> MAT2d_Tool2d::Circuit() const
{
  return theCircuit;
}

//=================================================================================================

void MAT2d_Tool2d::BisecFusion(const int I1, const int I2)
{
  double               DU, UL1, UF1;
  occ::handle<Geom2d_TrimmedCurve> Bisector1;
  occ::handle<Geom2d_TrimmedCurve> Bisector2;

  Bisector1 = GeomBis(I1).Value();
  Bisector2 = GeomBis(I2).Value();
  UF1       = Bisector1->FirstParameter();
  UL1       = Bisector1->LastParameter();

  occ::handle<Standard_Type> Type1 = Bisector1->BasisCurve()->DynamicType();
  if (Type1 == STANDARD_TYPE(Bisector_BisecCC))
  {
    //------------------------------------------------------------------------------------
    // les bissectrice courbe/courbe sont  construites avec un point de depart
    // elles ne peuvent pas etre trimes par un point se trouvant de l autre cote du
    // point de depart.
    // pour faire la fusion des deux bissectrices on reconstruit la bissectrice entre les
    // deux courbes avec comme point de depart le dernier point de la Bisector2.
    // on trime ensuite la courbe par le dernier point de Bisector1.
    //------------------------------------------------------------------------------------
    double            Tolerance = MAT2d_TOLCONF;
    Bisector_Bisec           Bis;
    gp_Vec2d                 VBid(1, 0);
    gp_Pnt2d                 P2   = Bisector2->Value(Bisector2->LastParameter());
    gp_Pnt2d                 P1   = Bisector1->Value(Bisector1->LastParameter());
    occ::handle<Bisector_BisecCC> BCC1 = occ::down_cast<Bisector_BisecCC>(Bisector1->BasisCurve());

    Bis.Perform(BCC1->Curve(2),
                BCC1->Curve(1),
                P2,
                VBid,
                VBid,
                theDirection,
                theJoinType,
                Tolerance,
                false);

    Bisector1 = Bis.Value();
    BCC1      = occ::down_cast<Bisector_BisecCC>(Bisector1->BasisCurve());
    UF1       = BCC1->FirstParameter();
    UL1       = BCC1->Parameter(P1);
    Bisector1->SetTrim(UF1, UL1);
    theGeomBisectors.Bind(I1, Bis);
  }
  else
  {
    DU  = Bisector2->LastParameter() - Bisector2->FirstParameter();
    UF1 = UF1 - DU;

    occ::handle<Bisector_BisecAna> BAna = occ::down_cast<Bisector_BisecAna>(Bisector1->BasisCurve());
    //---------------------------- uncomment if new method Bisector_BisecAna::SetTrim(f,l) is not
    // used
    //    occ::handle<Geom2d_Curve> C2d = BAna->Geom2dCurve();
    //    occ::handle<Geom2d_TrimmedCurve> trimC2d = new Geom2d_TrimmedCurve(C2d, UF1, UL1);
    //    BAna->Init(trimC2d);
    //--------------------------- end
    BAna->SetTrim(UF1, UL1); // put comment if SetTrim(f,l) is not used

    Bisector1->SetTrim(UF1, UL1);
  }
}

//=================================================================================================

static occ::handle<Standard_Type> Type(const occ::handle<Geom2d_Geometry>& aGeom)
{
  occ::handle<Standard_Type> type = aGeom->DynamicType();
  occ::handle<Geom2d_Curve>  curve;

  if (type == STANDARD_TYPE(Geom2d_TrimmedCurve))
  {
    curve = occ::down_cast<Geom2d_TrimmedCurve>(aGeom)->BasisCurve();
    type  = curve->DynamicType();
  }
  return type;
}

//==========================================================================
// function : AreNeighbours
// purpose  : Return TRUE si IEdge1 et IEdge2 correspondent a des elements
//           consecutifs sur un contour ferme de NbEdge elements.
//==========================================================================
bool AreNeighbours(const int IEdge1,
                               const int IEdge2,
                               const int NbEdge)
{
  if (std::abs(IEdge1 - IEdge2) == 1)
    return true;
  else if (std::abs(IEdge1 - IEdge2) == NbEdge - 1)
    return true;
  else
    return false;
}

//=================================================================================================

static void SetTrim(Bisector_Bisec& Bis, const occ::handle<Geom2d_Curve>& Line1)
{
  Geom2dInt_GInter            Intersect;
  double               Distance;
  double               Tolerance = MAT2d_TOLCONF;
  occ::handle<Geom2d_TrimmedCurve> Bisector  = Bis.ChangeValue();

  IntRes2d_Domain Domain1 = Domain(Bisector, Tolerance);
  double   UB1     = Bisector->FirstParameter();
  double   UB2     = Bisector->LastParameter();

  gp_Pnt2d      FirstPointBisector = Bisector->Value(UB1);
  double UTrim              = Precision::Infinite();

  Geom2dAdaptor_Curve AdapBisector(Bisector);
  Geom2dAdaptor_Curve AdapLine1(Line1);
  Intersect.Perform(AdapBisector, Domain1, AdapLine1, Tolerance, Tolerance);

  if (Intersect.IsDone() && !Intersect.IsEmpty())
  {
    for (int i = 1; i <= Intersect.NbPoints(); i++)
    {
      gp_Pnt2d PInt = Intersect.Point(i).Value();
      Distance      = FirstPointBisector.Distance(PInt);
      if (Distance > 10. * Tolerance && Intersect.Point(i).ParamOnFirst() < UTrim)
      {
        UTrim = Intersect.Point(i).ParamOnFirst();
      }
    }
  }
  // ------------------------------------------------------------------------
  // Restriction de la Bissectrice par le point d intersection de plus petit
  // parametre.
  // ------------------------------------------------------------------------
  if (UTrim < UB2 && UTrim > UB1)
    Bisector->SetTrim(UB1, UTrim);
}

//=================================================================================================

IntRes2d_Domain Domain(const occ::handle<Geom2d_TrimmedCurve>& Bisector1, const double Tolerance)
{
  double Param1 = Bisector1->FirstParameter();
  double Param2 = Bisector1->LastParameter();
  if (Param2 > 10000.)
  {
    Param2                      = 10000.;
    occ::handle<Standard_Type> Type1 = Type(Bisector1->BasisCurve());
    occ::handle<Geom2d_Curve>  BasisCurve;
    if (Type1 == STANDARD_TYPE(Bisector_BisecAna))
    {
      BasisCurve = occ::down_cast<Bisector_BisecAna>(Bisector1->BasisCurve())->Geom2dCurve();
      Type1      = BasisCurve->DynamicType();
    }
    gp_Parab2d    gpParabola;
    gp_Hypr2d     gpHyperbola;
    double Focus;
    double Limit = 50000.;
    if (Type1 == STANDARD_TYPE(Geom2d_Parabola))
    {
      gpParabola         = occ::down_cast<Geom2d_Parabola>(BasisCurve)->Parab2d();
      Focus              = gpParabola.Focal();
      double Val1 = std::sqrt(Limit * Focus);
      double Val2 = std::sqrt(Limit * Limit);
      Param2             = (Val1 <= Val2 ? Val1 : Val2);
    }
    else if (Type1 == STANDARD_TYPE(Geom2d_Hyperbola))
    {
      gpHyperbola         = occ::down_cast<Geom2d_Hyperbola>(BasisCurve)->Hypr2d();
      double Majr  = gpHyperbola.MajorRadius();
      double Minr  = gpHyperbola.MinorRadius();
      double Valu1 = Limit / Majr;
      double Valu2 = Limit / Minr;
      double Val1  = std::log(Valu1 + std::sqrt(Valu1 * Valu1 - 1));
      double Val2  = std::log(Valu2 + std::sqrt(Valu2 * Valu2 + 1));
      Param2              = (Val1 <= Val2 ? Val1 : Val2);
    }
  }

  IntRes2d_Domain Domain1(Bisector1->Value(Param1),
                          Param1,
                          Tolerance,
                          Bisector1->Value(Param2),
                          Param2,
                          Tolerance);
  if (Bisector1->BasisCurve()->IsPeriodic())
  {
    Domain1.SetEquivalentParameters(0., 2. * M_PI);
  }
  return Domain1;
}

//=================================================================================================

bool CheckEnds(const occ::handle<Geom2d_Geometry>& Elt,
                           const gp_Pnt2d&                PCom,
                           const double            Distance,
                           const double            Tol)

{
  occ::handle<Standard_Type>       Type = Elt->DynamicType();
  occ::handle<Geom2d_TrimmedCurve> Curve;

  if (Type == STANDARD_TYPE(Geom2d_CartesianPoint))
  {
    return false;
  }
  else
  {
    Curve             = occ::down_cast<Geom2d_TrimmedCurve>(Elt);
    gp_Pnt2d      aPf = Curve->StartPoint();
    gp_Pnt2d      aPl = Curve->EndPoint();
    double df  = PCom.Distance(aPf);
    double dl  = PCom.Distance(aPl);
    if (std::abs(df - Distance) <= Tol)
      return true;
    if (std::abs(dl - Distance) <= Tol)
      return true;
  }
  return false;
}

#ifdef OCCT_DEBUG
//==========================================================================
// function : MAT2d_DrawCurve
// purpose  : Affichage d une courbe <aCurve> de Geom2d. dans une couleur
//           definie par <Indice>.
//            Indice = 1 jaune,
//            Indice = 2 bleu,
//            Indice = 3 rouge,
//            Indice = 4 vert.
//==========================================================================
void MAT2d_DrawCurve(const occ::handle<Geom2d_Curve>& aCurve, const int /*Indice*/)
{
  occ::handle<Standard_Type> type = aCurve->DynamicType();
  occ::handle<Geom2d_Curve>  curve, CurveDraw;
  #ifdef DRAW
  occ::handle<DrawTrSurf_Curve2d> dr;
  Draw_Color                 Couleur;
  #endif

  if (type == STANDARD_TYPE(Geom2d_TrimmedCurve))
  {
    curve = occ::down_cast<Geom2d_TrimmedCurve>(aCurve)->BasisCurve();
    type  = curve->DynamicType();
    // PB de representation des courbes semi_infinies.
    gp_Parab2d    gpParabola;
    gp_Hypr2d     gpHyperbola;
    double Focus;
    double Limit = 50000.;
    double delta = 400;

    // PB de representation des courbes semi_infinies.
    if (aCurve->LastParameter() == Precision::Infinite())
    {

      if (type == STANDARD_TYPE(Geom2d_Parabola))
      {
        gpParabola         = occ::down_cast<Geom2d_Parabola>(curve)->Parab2d();
        Focus              = gpParabola.Focal();
        double Val1 = std::sqrt(Limit * Focus);
        double Val2 = std::sqrt(Limit * Limit);
        delta              = (Val1 <= Val2 ? Val1 : Val2);
      }
      else if (type == STANDARD_TYPE(Geom2d_Hyperbola))
      {
        gpHyperbola         = occ::down_cast<Geom2d_Hyperbola>(curve)->Hypr2d();
        double Majr  = gpHyperbola.MajorRadius();
        double Minr  = gpHyperbola.MinorRadius();
        double Valu1 = Limit / Majr;
        double Valu2 = Limit / Minr;
        double Val1  = Log(Valu1 + std::sqrt(Valu1 * Valu1 - 1));
        double Val2  = Log(Valu2 + std::sqrt(Valu2 * Valu2 + 1));
        delta               = (Val1 <= Val2 ? Val1 : Val2);
      }
      CurveDraw =
        new Geom2d_TrimmedCurve(aCurve, aCurve->FirstParameter(), aCurve->FirstParameter() + delta);
    }
    else
    {
      CurveDraw = aCurve;
    }
    // fin PB.
  }
  else
  {
    CurveDraw = aCurve;
  }

  #ifdef DRAW
  int Indice = 1;
  if (Indice == 1)
    Couleur = Draw_jaune;
  else if (Indice == 2)
    Couleur = Draw_bleu;
  else if (Indice == 3)
    Couleur = Draw_rouge;
  else if (Indice == 4)
    Couleur = Draw_vert;

  if (type == STANDARD_TYPE(Geom2d_Circle))
    dr = new DrawTrSurf_Curve2d(CurveDraw, Couleur, 30);
  else if (type == STANDARD_TYPE(Geom2d_Line))
    dr = new DrawTrSurf_Curve2d(CurveDraw, Couleur, 2);
  else
    dr = new DrawTrSurf_Curve2d(CurveDraw, Couleur, 500);

    // dout << dr;
    // dout.Flush();
  #endif
}

#endif
