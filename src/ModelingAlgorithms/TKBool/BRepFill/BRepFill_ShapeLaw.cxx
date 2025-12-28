// Created on: 1998-08-17
// Created by: Philippe MANGIN
// Copyright (c) 1998-1999 Matra Datavision
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

#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepFill_ShapeLaw.hxx>
#include <BRepLProp.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Line.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomConvert_CompCurveToBSplineCurve.hxx>
#include <GeomFill_EvolvedSection.hxx>
#include <GeomFill_SectionLaw.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <GeomFill_SectionLaw.hxx>
#include <GeomFill_UniformSection.hxx>
#include <Law_Function.hxx>
#include <Precision.hxx>
#include <Standard_Type.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepFill_ShapeLaw, BRepFill_SectionLaw)

//=======================================================================
// function : Create
// purpose  : Process the case of Vertex by constructing a line
//           with the vertex in the origin
//=======================================================================
BRepFill_ShapeLaw::BRepFill_ShapeLaw(const TopoDS_Vertex& V, const bool Build)
    : vertex(true),
      myShape(V)
{
  TheLaw.Nullify();
  uclosed = false;
  vclosed = true; // constant law
  myEdges = new (NCollection_HArray1<TopoDS_Shape>)(1, 1);
  myEdges->SetValue(1, V);

  if (Build)
  {
    myLaws = new (NCollection_HArray1<occ::handle<GeomFill_SectionLaw>>)(1, 1);
    //    gp_Pnt Origine;
    gp_Dir                    D(gp_Dir::D::X); // Following the normal
    occ::handle<Geom_Line>         L    = new (Geom_Line)(BRep_Tool::Pnt(V), D);
    double             Last = 2 * BRep_Tool::Tolerance(V) + Precision::PConfusion();
    occ::handle<Geom_TrimmedCurve> TC   = new (Geom_TrimmedCurve)(L, 0, Last);

    myLaws->ChangeValue(1) = new (GeomFill_UniformSection)(TC);
  }
  myDone = true;
}

//=================================================================================================

BRepFill_ShapeLaw::BRepFill_ShapeLaw(const TopoDS_Wire& W, const bool Build)
    : vertex(false),
      myShape(W)

{
  TheLaw.Nullify();
  Init(Build);
  myDone = true;
}

//=================================================================================================

BRepFill_ShapeLaw::BRepFill_ShapeLaw(const TopoDS_Wire&          W,
                                     const occ::handle<Law_Function>& L,
                                     const bool      Build)
    : vertex(false),
      myShape(W)

{
  TheLaw = L;
  Init(Build);
  myDone = true;
}

//=======================================================================
// function : Init
// purpose  : Case of the wire : Create a table of GeomFill_SectionLaw
//=======================================================================
void BRepFill_ShapeLaw::Init(const bool Build)
{
  vclosed = true;
  BRepTools_WireExplorer wexp;
  TopoDS_Edge            E;
  int       NbEdge, ii;
  double          First, Last;
  TopoDS_Wire            W;
  W = TopoDS::Wire(myShape);

  for (NbEdge = 0, wexp.Init(W); wexp.More(); wexp.Next())
  {
    E = wexp.Current();
    if (!E.IsNull() && !BRep_Tool::Degenerated(E))
    {
      occ::handle<Geom_Curve> C = BRep_Tool::Curve(E, First, Last);
      if (!C.IsNull())
      {
        NbEdge++;
      }
    }
  }

  myLaws  = new NCollection_HArray1<occ::handle<GeomFill_SectionLaw>>(1, NbEdge);
  myEdges = new NCollection_HArray1<TopoDS_Shape>(1, NbEdge);

  ii = 1;

  for (wexp.Init(W); wexp.More(); wexp.Next())
  {
    E = wexp.Current();
    if (!E.IsNull() && !BRep_Tool::Degenerated(wexp.Current()))
    {
      occ::handle<Geom_Curve> C = BRep_Tool::Curve(E, First, Last);
      if (!C.IsNull())
      {
        myEdges->SetValue(ii, E);
        myIndices.Bind(E, ii);
        if (Build)
        {
          // occ::handle<Geom_Curve> C = BRep_Tool::Curve(E,First,Last);
          if (E.Orientation() == TopAbs_REVERSED)
          {
            double      aux;
            occ::handle<Geom_Curve> CBis;
            CBis  = C->Reversed(); // To avoid the deterioration of the topology
            aux   = C->ReversedParameter(First);
            First = C->ReversedParameter(Last);
            Last  = aux;
            C     = CBis;
          }

          bool IsClosed = BRep_Tool::IsClosed(E);
          if (IsClosed && std::abs(C->FirstParameter() - First) > Precision::PConfusion())
            IsClosed = false; // trimmed curve differs

          if ((ii > 1) || !IsClosed)
          { // Trim C
            occ::handle<Geom_TrimmedCurve> TC = new Geom_TrimmedCurve(C, First, Last);
            C                            = TC;
          }
          // otherwise preserve the integrity of the curve
          if (TheLaw.IsNull())
          {
            myLaws->ChangeValue(ii) = new GeomFill_UniformSection(C);
          }
          else
          {
            myLaws->ChangeValue(ii) = new GeomFill_EvolvedSection(C, TheLaw);
          }
        }
        ii++;
      }
    }
  }

  //  std::cout << "new law" << std::endl;

  //  Is the law closed by U ?
  uclosed = W.Closed();
  if (!uclosed)
  {
    // if not sure about the flag, make check
    TopoDS_Edge   Edge1, Edge2;
    TopoDS_Vertex V1, V2;
    Edge1 = TopoDS::Edge(myEdges->Value(myEdges->Length()));
    Edge2 = TopoDS::Edge(myEdges->Value(1));

    if (Edge1.Orientation() == TopAbs_REVERSED)
    {
      V1 = TopExp::FirstVertex(Edge1);
    }
    else
    {
      V1 = TopExp::LastVertex(Edge1);
    }

    if (Edge2.Orientation() == TopAbs_REVERSED)
    {
      V2 = TopExp::LastVertex(Edge2);
    }
    else
    {
      V2 = TopExp::FirstVertex(Edge2);
    }
    if (V1.IsSame(V2))
    {
      uclosed = true;
    }
    else
    {
      BRepAdaptor_Curve Curve1(Edge1);
      BRepAdaptor_Curve Curve2(Edge2);
      double     U1  = BRep_Tool::Parameter(V1, Edge1);
      double     U2  = BRep_Tool::Parameter(V2, Edge2);
      double     Eps = BRep_Tool::Tolerance(V2) + BRep_Tool::Tolerance(V1);

      uclosed = Curve1.Value(U1).IsEqual(Curve2.Value(U2), Eps);
    }
  }
}

//=================================================================================================

bool BRepFill_ShapeLaw::IsVertex() const
{
  return vertex;
}

//=================================================================================================

bool BRepFill_ShapeLaw::IsConstant() const
{
  return TheLaw.IsNull();
}

//=================================================================================================

TopoDS_Vertex BRepFill_ShapeLaw::Vertex(const int Index,
                                        const double    Param) const
{
  TopoDS_Edge   E;
  TopoDS_Vertex V;
  if (Index <= myEdges->Length())
  {
    E = TopoDS::Edge(myEdges->Value(Index));
    if (E.Orientation() == TopAbs_REVERSED)
      V = TopExp::LastVertex(E);
    else
      V = TopExp::FirstVertex(E);
  }
  else if (Index == myEdges->Length() + 1)
  {
    E = TopoDS::Edge(myEdges->Value(Index - 1));
    if (E.Orientation() == TopAbs_REVERSED)
      V = TopExp::FirstVertex(E);
    else
      V = TopExp::LastVertex(E);
  }

  if (!TheLaw.IsNull())
  {
    gp_Trsf T;
    T.SetScale(gp_Pnt(0, 0, 0), TheLaw->Value(Param));
    // TopLoc_Location L(T);
    // V.Move(L);
    V = TopoDS::Vertex(BRepBuilderAPI_Transform(V, T));
  }
  return V;
}

///=======================================================================
// function : VertexTol
// purpose  : Evaluate the hole between 2 edges of the section
//=======================================================================
double BRepFill_ShapeLaw::VertexTol(const int Index,
                                           const double    Param) const
{
  double    Tol = Precision::Confusion();
  int I1, I2;
  if ((Index == 0) || (Index == myEdges->Length()))
  {
    if (!uclosed)
      return Tol; // The least possible error
    I1 = myEdges->Length();
    I2 = 1;
  }
  else
  {
    I1 = Index;
    I2 = I1 + 1;
  }

  occ::handle<GeomFill_SectionLaw>      Loi;
  int                 NbPoles, NbKnots, Degree;
  occ::handle<NCollection_HArray1<gp_Pnt>>      Poles;
  occ::handle<NCollection_HArray1<double>>    Knots, Weigth;
  occ::handle<NCollection_HArray1<int>> Mults;
  occ::handle<Geom_BSplineCurve>        BS;
  gp_Pnt                           PFirst;

  Loi = myLaws->Value(I1);
  Loi->SectionShape(NbPoles, NbKnots, Degree);
  Poles  = new (NCollection_HArray1<gp_Pnt>)(1, NbPoles);
  Weigth = new (NCollection_HArray1<double>)(1, NbPoles);
  Loi->D0(Param, Poles->ChangeArray1(), Weigth->ChangeArray1());
  Knots = new (NCollection_HArray1<double>)(1, NbKnots);
  Loi->Knots(Knots->ChangeArray1());
  Mults = new (NCollection_HArray1<int>)(1, NbKnots);
  Loi->Mults(Mults->ChangeArray1());
  BS     = new (Geom_BSplineCurve)(Poles->Array1(),
                               Weigth->Array1(),
                               Knots->Array1(),
                               Mults->Array1(),
                               Degree,
                               Loi->IsUPeriodic());
  PFirst = BS->Value(Knots->Value(Knots->Length()));

  Loi = myLaws->Value(I2);
  Loi->SectionShape(NbPoles, NbKnots, Degree);
  Poles  = new (NCollection_HArray1<gp_Pnt>)(1, NbPoles);
  Weigth = new (NCollection_HArray1<double>)(1, NbPoles);
  Loi->D0(Param, Poles->ChangeArray1(), Weigth->ChangeArray1());
  Knots = new (NCollection_HArray1<double>)(1, NbKnots);
  Loi->Knots(Knots->ChangeArray1());
  Mults = new (NCollection_HArray1<int>)(1, NbKnots);
  Loi->Mults(Mults->ChangeArray1());
  BS = new (Geom_BSplineCurve)(Poles->Array1(),
                               Weigth->Array1(),
                               Knots->Array1(),
                               Mults->Array1(),
                               Degree,
                               Loi->IsUPeriodic());
  Tol += PFirst.Distance(BS->Value(Knots->Value(1)));
  return Tol;
}

//=================================================================================================

occ::handle<GeomFill_SectionLaw> BRepFill_ShapeLaw::ConcatenedLaw() const
{
  occ::handle<GeomFill_SectionLaw> Law;
  if (myLaws->Length() == 1)
    return myLaws->Value(1);
  else
  {
    TopoDS_Wire   W;
    TopoDS_Vertex V;
    W = TopoDS::Wire(myShape);
    if (!W.IsNull())
    {
      //  Concatenation of edges
      int          ii;
      double             epsV, f, l;
      bool          Bof;
      occ::handle<Geom_Curve>        Composite;
      occ::handle<Geom_TrimmedCurve> TC;
      Composite = BRep_Tool::Curve(Edge(1), f, l);
      TC        = new (Geom_TrimmedCurve)(Composite, f, l);
      GeomConvert_CompCurveToBSplineCurve Concat(TC);

      for (ii = 2, Bof = true; ii <= myEdges->Length() && Bof; ii++)
      {
        Composite = BRep_Tool::Curve(Edge(ii), f, l);
        TC        = new (Geom_TrimmedCurve)(Composite, f, l);
        Bof       = TopExp::CommonVertex(Edge(ii - 1), Edge(ii), V);
        if (Bof)
        {
          epsV = BRep_Tool::Tolerance(V);
        }
        else
          epsV = 10 * Precision::PConfusion();
        Bof = Concat.Add(TC, epsV, true, false, 20);
        if (!Bof)
          Bof = Concat.Add(TC, 200 * epsV, true, false, 20);
#ifdef OCCT_DEBUG
        if (!Bof)
          std::cout << "BRepFill_ShapeLaw::ConcatenedLaw INCOMPLET !!!" << std::endl;
#endif
      }
      Composite = Concat.BSplineCurve();

      if (TheLaw.IsNull())
      {
        Law = new (GeomFill_UniformSection)(Composite);
      }
      else
      {
        Law = new (GeomFill_EvolvedSection)(Composite, TheLaw);
      }
    }
  }
  return Law;
}

//=================================================================================================

GeomAbs_Shape BRepFill_ShapeLaw::Continuity(const int Index,
                                            const double    TolAngular) const
{

  TopoDS_Edge Edge1, Edge2;
  if ((Index == 0) || (Index == myEdges->Length()))
  {
    if (!uclosed)
      return GeomAbs_C0; // The least possible error

    Edge1 = TopoDS::Edge(myEdges->Value(myEdges->Length()));
    Edge2 = TopoDS::Edge(myEdges->Value(1));
  }
  else
  {
    Edge1 = TopoDS::Edge(myEdges->Value(Index));
    Edge2 = TopoDS::Edge(myEdges->Value(Index + 1));
  }

  TopoDS_Vertex V1, V2; // common vertex
  TopoDS_Vertex vv1, vv2, vv3, vv4;
  TopExp::Vertices(Edge1, vv1, vv2);
  TopExp::Vertices(Edge2, vv3, vv4);
  if (vv1.IsSame(vv3))
  {
    V1 = vv1;
    V2 = vv3;
  }
  else if (vv1.IsSame(vv4))
  {
    V1 = vv1;
    V2 = vv4;
  }
  else if (vv2.IsSame(vv3))
  {
    V1 = vv2;
    V2 = vv3;
  }
  else
  {
    V1 = vv2;
    V2 = vv4;
  }

  double     U1 = BRep_Tool::Parameter(V1, Edge1);
  double     U2 = BRep_Tool::Parameter(V2, Edge2);
  BRepAdaptor_Curve Curve1(Edge1);
  BRepAdaptor_Curve Curve2(Edge2);
  double     Eps = BRep_Tool::Tolerance(V2) + BRep_Tool::Tolerance(V1);
  GeomAbs_Shape     cont;
  cont = BRepLProp::Continuity(Curve1, Curve2, U1, U2, Eps, TolAngular);

  return cont;
}

//=================================================================================================

void BRepFill_ShapeLaw::D0(const double U, TopoDS_Shape& S)
{
  S = myShape;
  if (!TheLaw.IsNull())
  {
    gp_Trsf T;
    T.SetScale(gp_Pnt(0, 0, 0), TheLaw->Value(U));
    // TopLoc_Location L(T);
    // S.Move(L);
    S = BRepBuilderAPI_Transform(S, T);
  }
}
