// Created on: 1998-12-29
// Created by: Joelle CHAUVET
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

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepFill_NSections.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <BRepLib_MakeWire.hxx>
#include <BRepLProp.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <BSplCLib.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomConvert.hxx>
#include <GeomConvert_ApproxCurve.hxx>
#include <GeomConvert_CompCurveToBSplineCurve.hxx>
#include <GeomFill_AppSurf.hxx>
#include <GeomFill_SectionLaw.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <GeomFill_Line.hxx>
#include <GeomFill_NSections.hxx>
#include <GeomFill_SectionGenerator.hxx>
#include <Precision.hxx>
#include <Standard_Type.hxx>
#include <NCollection_Sequence.hxx>
#include <gp_Pnt.hxx>
#include <Standard_Integer.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>

#include <cstdio>
IMPLEMENT_STANDARD_RTTIEXT(BRepFill_NSections, BRepFill_SectionLaw)

#ifdef OCCT_DEBUG
static bool Affich = 0;
#endif

//=======================================================================
// function : EdgeToBSpline
// purpose  : auxiliary -- get curve from edge and convert it to bspline
//           parameterized from 0 to 1
//=======================================================================

// NOTE: this code duplicates the same function in BRepOffsetAPI_ThruSections.cxx
static occ::handle<Geom_BSplineCurve> EdgeToBSpline(const TopoDS_Edge& theEdge)
{
  occ::handle<Geom_BSplineCurve> aBSCurve;
  if (BRep_Tool::Degenerated(theEdge))
  {
    // degenerated edge : construction of a point curve
    NCollection_Array1<double> aKnots(1, 2);
    aKnots(1) = 0.;
    aKnots(2) = 1.;

    NCollection_Array1<int> aMults(1, 2);
    aMults(1) = 2;
    aMults(2) = 2;

    NCollection_Array1<gp_Pnt> aPoles(1, 2);
    TopoDS_Vertex              vf, vl;
    TopExp::Vertices(theEdge, vl, vf);
    aPoles(1) = BRep_Tool::Pnt(vf);
    aPoles(2) = BRep_Tool::Pnt(vl);

    aBSCurve = new Geom_BSplineCurve(aPoles, aKnots, aMults, 1);
  }
  else
  {
    // get the curve of the edge
    TopLoc_Location         aLoc;
    double                  aFirst, aLast;
    occ::handle<Geom_Curve> aCurve = BRep_Tool::Curve(theEdge, aLoc, aFirst, aLast);

    // convert its part used by edge to bspline; note that if edge curve is bspline,
    // approximation or conversion made via trimmed curve is still needed -- it will copy it,
    // segment as appropriate, and remove periodicity if it is periodic (deadly for approximator)
    occ::handle<Geom_TrimmedCurve> aTrimCurve = new Geom_TrimmedCurve(aCurve, aFirst, aLast);

    const occ::handle<Geom_Curve>& aCurveTemp = aTrimCurve; // to avoid ambiguity
    GeomConvert_ApproxCurve        anAppr(aCurveTemp, Precision::Confusion(), GeomAbs_C1, 16, 14);
    if (anAppr.HasResult())
      aBSCurve = anAppr.Curve();

    if (aBSCurve.IsNull())
      aBSCurve = GeomConvert::CurveToBSplineCurve(aTrimCurve);

    // apply transformation if needed
    if (!aLoc.IsIdentity())
      aBSCurve->Transform(aLoc.Transformation());

    // reparameterize to [0,1]
    NCollection_Array1<double> aKnots(aBSCurve->Knots());
    BSplCLib::Reparametrize(0., 1., aKnots);
    aBSCurve->SetKnots(aKnots);
  }

  // reverse curve if edge is reversed
  if (theEdge.Orientation() == TopAbs_REVERSED)
    aBSCurve->Reverse();

  return aBSCurve;
}

//=================================================================================================

static occ::handle<Geom_BSplineSurface> totalsurf(const NCollection_Array2<TopoDS_Shape>& shapes,
                                                  const int                               NbSects,
                                                  const int                               NbEdges,
                                                  const NCollection_Sequence<double>&     params,
                                                  const bool                              w1Point,
                                                  const bool                              w2Point,
                                                  const bool                              uClosed,
                                                  const bool                              vClosed,
                                                  const double                            myPres3d)
{
  int           i, j, jdeb = 1, jfin = NbSects;
  TopoDS_Edge   edge;
  TopoDS_Vertex vf, vl;

  GeomFill_SectionGenerator        section;
  occ::handle<Geom_BSplineSurface> surface;
  occ::handle<Geom_BSplineCurve>   BS, BS1;

  if (w1Point)
  {
    jdeb++;
    edge = TopoDS::Edge(shapes.Value(1, 1));
    TopExp::Vertices(edge, vl, vf);
    NCollection_Array1<gp_Pnt> Extremities(1, 2);
    Extremities(1) = BRep_Tool::Pnt(vf);
    Extremities(2) = BRep_Tool::Pnt(vl);
    NCollection_Array1<double> Bounds(1, 2);
    Bounds(1) = 0.;
    Bounds(2) = 1.;
    NCollection_Array1<int> Mult(1, 2);
    Mult(1)                                = 2;
    Mult(2)                                = 2;
    occ::handle<Geom_BSplineCurve> BSPoint = new Geom_BSplineCurve(Extremities, Bounds, Mult, 1);
    section.AddCurve(BSPoint);
  }

  if (w2Point)
  {
    jfin--;
  }

  for (j = jdeb; j <= jfin; j++)
  {

    // case of looping sections
    if (j == jfin && vClosed)
    {
      section.AddCurve(BS1);
    }

    else
    {
      // read the first edge to initialise CompBS;
      TopoDS_Edge                    aPrevEdge = TopoDS::Edge(shapes.Value(1, j));
      occ::handle<Geom_BSplineCurve> curvBS    = EdgeToBSpline(aPrevEdge);

      // initialization
      GeomConvert_CompCurveToBSplineCurve CompBS(curvBS);

      for (i = 2; i <= NbEdges; i++)
      {
        // read the edge
        TopoDS_Edge aNextEdge = TopoDS::Edge(shapes.Value(i, j));
        curvBS                = EdgeToBSpline(aNextEdge);

        // concatenation
        TopoDS_Vertex ComV;
        double        epsV;
        bool          Bof = TopExp::CommonVertex(aPrevEdge, aNextEdge, ComV);
        if (Bof)
          epsV = BRep_Tool::Tolerance(ComV);
        else
          epsV = Precision::Confusion();
        Bof = CompBS.Add(curvBS, epsV, true, false, 1);
        if (!Bof)
          Bof = CompBS.Add(curvBS, 200 * epsV, true, false, 1);

        // remember previous edge
        aPrevEdge = aNextEdge;
      }

      // return the final section
      BS = CompBS.BSplineCurve();
      section.AddCurve(BS);

      // case of looping sections
      if (j == jdeb && vClosed)
      {
        BS1 = BS;
      }
    }
  }

  if (w2Point)
  {
    edge = TopoDS::Edge(shapes.Value(NbEdges, NbSects));
    TopExp::Vertices(edge, vl, vf);
    NCollection_Array1<gp_Pnt> Extremities(1, 2);
    Extremities(1) = BRep_Tool::Pnt(vf);
    Extremities(2) = BRep_Tool::Pnt(vl);
    NCollection_Array1<double> Bounds(1, 2);
    Bounds(1) = 0.;
    Bounds(2) = 1.;
    NCollection_Array1<int> Mult(1, 2);
    Mult(1)                                = 2;
    Mult(2)                                = 2;
    occ::handle<Geom_BSplineCurve> BSPoint = new Geom_BSplineCurve(Extremities, Bounds, Mult, 1);
    section.AddCurve(BSPoint);
  }

  occ::handle<NCollection_HArray1<double>> HPar =
    new NCollection_HArray1<double>(1, params.Length());
  for (i = 1; i <= params.Length(); i++)
  {
    HPar->SetValue(i, params(i));
  }
  section.SetParam(HPar);
  section.Perform(Precision::PConfusion());
  occ::handle<GeomFill_Line> line = new GeomFill_Line(NbSects);
  int                        nbIt = 0, degmin = 2, degmax = 6;
  bool                       knownP = true;
  GeomFill_AppSurf           anApprox(degmin, degmax, myPres3d, myPres3d, nbIt, knownP);
  bool                       SpApprox = true;
  anApprox.Perform(line, section, SpApprox);
  bool uperiodic = uClosed;
  bool vperiodic = vClosed;
  int  nup = anApprox.SurfPoles().ColLength(), nvp = anApprox.SurfPoles().RowLength();
  NCollection_Array1<int> Umults(1, anApprox.SurfUKnots().Length());
  Umults = anApprox.SurfUMults();
  NCollection_Array1<int> Vmults(1, anApprox.SurfVKnots().Length());
  Vmults = anApprox.SurfVMults();

  if (uperiodic)
  {
    int nbuk = anApprox.SurfUKnots().Length();
    Umults(1)--;
    Umults(nbuk)--;
    nup--;
  }

  if (vperiodic)
  {
    int nbvk = anApprox.SurfVKnots().Length();
    Vmults(1)--;
    Vmults(nbvk)--;
    nvp--;
  }

  NCollection_Array2<gp_Pnt> poles(1, nup, 1, nvp);
  NCollection_Array2<double> weights(1, nup, 1, nvp);
  for (j = 1; j <= nvp; j++)
  {
    for (i = 1; i <= nup; i++)
    {
      poles(i, j)   = anApprox.SurfPoles()(i, j);
      weights(i, j) = anApprox.SurfWeights()(i, j);
    }
  }

  // To create non-rational surface if possible
  double TolEps    = 1.e-13;
  bool   Vrational = false, Urational = false;
  for (j = 1; j <= weights.UpperCol(); j++)
    if (!Vrational)
      for (i = 1; i <= weights.UpperRow() - 1; i++)
      {
        // double signeddelta = weights(i,j) - weights(i+1,j);
        double delta = std::abs(weights(i, j) - weights(i + 1, j));
        //	  double eps = Epsilon( std::abs(weights(i,j)) );
        if (delta > TolEps /* || delta > 3.*eps*/)
        {
          Vrational = true;
          break;
        }
      }
  for (i = 1; i <= weights.UpperRow(); i++)
    if (!Urational)
      for (j = 1; j <= weights.UpperCol() - 1; j++)
      {
        // double signeddelta = weights(i,j) - weights(i,j+1);
        double delta = std::abs(weights(i, j) - weights(i, j + 1));
        //	  double eps = Epsilon( std::abs(weights(i,j)) );
        if (delta > TolEps /* || delta > 3.*eps*/)
        {
          Urational = true;
          break;
        }
      }
  if (!Vrational && !Urational)
  {
    double theWeight = weights(1, 1);
    for (i = 1; i <= weights.UpperRow(); i++)
      for (j = 1; j <= weights.UpperCol(); j++)
        weights(i, j) = theWeight;
  }

  surface = new Geom_BSplineSurface(poles,
                                    weights,
                                    anApprox.SurfUKnots(),
                                    anApprox.SurfVKnots(),
                                    Umults,
                                    Vmults,
                                    anApprox.UDegree(),
                                    anApprox.VDegree(),
                                    uperiodic,
                                    vperiodic);
  return surface;
}

//=================================================================================================

BRepFill_NSections::BRepFill_NSections(const NCollection_Sequence<TopoDS_Shape>& S,
                                       const bool                                Build)

{
  myShapes = S;
  VFirst   = 0.;
  VLast    = 1.;
  NCollection_Sequence<double> par;
  par.Clear();
  for (int i = 1; i <= S.Length(); i++)
  {
    par.Append(i - 1);
  }
  myParams = par;
  Init(par, Build);
  myDone = true;
}

//=======================================================================
// function : Create
// purpose  : WSeq + Param
//=======================================================================

BRepFill_NSections::BRepFill_NSections(const NCollection_Sequence<TopoDS_Shape>& S,
                                       const NCollection_Sequence<gp_Trsf>&      Transformations,
                                       const NCollection_Sequence<double>&       P,
                                       const double                              VF,
                                       const double                              VL,
                                       const bool                                Build)

{
#ifdef OCCT_DEBUG
  if (Affich)
  {
  }
#endif
  bool ok = true;
  for (int iseq = 1; iseq < P.Length(); iseq++)
  {
    ok = ok && (P.Value(iseq) < P.Value(iseq + 1));
  }
  if (ok)
  {
    myParams = P;
    myShapes = S;
    myTrsfs  = Transformations;
    VFirst   = VF;
    VLast    = VL;
    Init(P, Build);
    myDone = true;
  }
  else
    myDone = false;
}

//=======================================================================
// function : Init
// purpose  : Create a table of GeomFill_SectionLaw
//=======================================================================
void BRepFill_NSections::Init(const NCollection_Sequence<double>& P, const bool Build)
{
  BRepTools_WireExplorer wexp;
  // Class BRep_Tool without fields and without Constructor :
  //  BRep_Tool B;
  TopoDS_Edge E;
  int         ii, NbEdge, jj, NbSects = P.Length();
  int         ideb = 1, ifin   = NbSects;
  bool        wClosed, w1Point = true, w2Point = true;
  double      First, Last;
  TopoDS_Wire W;

  // Check if the start and end wires are punctual
  W = TopoDS::Wire(myShapes(1));
  for (wexp.Init(W); wexp.More(); wexp.Next())
    //    w1Point = w1Point && B.Degenerated(wexp.Current());
    w1Point = w1Point && BRep_Tool::Degenerated(wexp.Current());
  if (w1Point)
    ideb++;
  W = TopoDS::Wire(myShapes(NbSects));
  for (wexp.Init(W); wexp.More(); wexp.Next())
    //    w2Point = w2Point && B.Degenerated(wexp.Current());
    w2Point = w2Point && BRep_Tool::Degenerated(wexp.Current());
  if (w2Point)
    ifin--;

  // Check if the start and end wires are identical
  vclosed = myShapes(1).IsSame(myShapes(NbSects));

  // Count the number of non-degenerated edges
  W = TopoDS::Wire(myShapes(ideb));
  for (NbEdge = 0, wexp.Init(W); wexp.More(); wexp.Next())
    //    if (! B.Degenerated(wexp.Current())) NbEdge++;
    if (!BRep_Tool::Degenerated(wexp.Current()))
      NbEdge++;

  myEdges = new (NCollection_HArray2<TopoDS_Shape>)(1, NbEdge, 1, NbSects);

  // Fill tables
  uclosed = true;
  for (jj = ideb; jj <= ifin; jj++)
  {

    W = TopoDS::Wire(myShapes(jj));

    for (ii = 1, wexp.Init(W); ii <= NbEdge; wexp.Next(), ii++)
    {
      E = wexp.Current();

      //      if ( ! B.Degenerated(E)) {
      if (!BRep_Tool::Degenerated(E))
      {
        myEdges->SetValue(ii, jj, E);
        if (E.Orientation() == TopAbs_FORWARD)
          myIndices.Bind(E, ii);
        else
          myIndices.Bind(E, -ii);
      }
    }

    // Is the law closed by U ?

    wClosed = W.Closed();
    if (!wClosed)
    {
      // if unsure about the flag, make check
      TopoDS_Edge   Edge1, Edge2;
      TopoDS_Vertex V1, V2;
      Edge1 = TopoDS::Edge(myEdges->Value(NbEdge, jj));
      Edge2 = TopoDS::Edge(myEdges->Value(1, jj));

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
        wClosed = true;
      }
      else
      {
        BRepAdaptor_Curve Curve1(Edge1);
        BRepAdaptor_Curve Curve2(Edge2);
        double            U1  = BRep_Tool::Parameter(V1, Edge1);
        double            U2  = BRep_Tool::Parameter(V2, Edge2);
        double            Eps = BRep_Tool::Tolerance(V2) + BRep_Tool::Tolerance(V1);

        wClosed = Curve1.Value(U1).IsEqual(Curve2.Value(U2), Eps);
      }
    }
    if (!wClosed)
      uclosed = false;
  }

  // point sections at end
  if (w1Point)
  {
    W = TopoDS::Wire(myShapes(1));
    wexp.Init(W);
    E = wexp.Current();
    for (ii = 1; ii <= NbEdge; ii++)
    {
      myEdges->SetValue(ii, 1, E);
    }
  }

  if (w2Point)
  {
    W = TopoDS::Wire(myShapes(NbSects));
    wexp.Init(W);
    E = wexp.Current();
    for (ii = 1; ii <= NbEdge; ii++)
    {
      myEdges->SetValue(ii, NbSects, E);
    }
  }

  myLaws = new (NCollection_HArray1<occ::handle<GeomFill_SectionLaw>>)(1, NbEdge);

  constexpr double tol = Precision::Confusion();
  mySurface            = totalsurf(myEdges->Array2(),
                        myShapes.Length(),
                        NbEdge,
                        myParams,
                        w1Point,
                        w2Point,
                        uclosed,
                        vclosed,
                        tol);

  // Increase the degree so that the position D2
  // on GeomFill_NSections could be correct
  // see comments in GeomFill_NSections
  if (mySurface->VDegree() < 2)
  {
    mySurface->IncreaseDegree(mySurface->UDegree(), 2);
  }

  // Fill tables
  if (Build)
  {
    for (ii = 1; ii <= NbEdge; ii++)
    {
      NCollection_Sequence<occ::handle<Geom_Curve>> NC;
      NC.Clear();
      for (jj = 1; jj <= NbSects; jj++)
      {
        E = TopoDS::Edge(myEdges->Value(ii, jj));
        occ::handle<Geom_Curve> C;
        //	if (B.Degenerated(E)) {
        if (BRep_Tool::Degenerated(E))
        {
          TopoDS_Vertex vf, vl;
          TopExp::Vertices(E, vl, vf);
          NCollection_Array1<gp_Pnt> Extremities(1, 2);
          Extremities(1) = BRep_Tool::Pnt(vf);
          Extremities(2) = BRep_Tool::Pnt(vl);
          NCollection_Array1<double> Bounds(1, 2);
          Bounds(1) = 0.;
          Bounds(2) = 1.;
          NCollection_Array1<int> Mult(1, 2);
          Mult(1) = 2;
          Mult(2) = 2;
          occ::handle<Geom_BSplineCurve> BSPoint =
            new Geom_BSplineCurve(Extremities, Bounds, Mult, 1);
          C = BSPoint;
        }
        else
        {
          C = BRep_Tool::Curve(E, First, Last);

          if (E.Orientation() == TopAbs_REVERSED)
          {
            double                  aux;
            occ::handle<Geom_Curve> CBis;
            CBis  = C->Reversed(); // To avoid the spoiling of the topology
            aux   = C->ReversedParameter(First);
            First = C->ReversedParameter(Last);
            Last  = aux;
            C     = CBis;
          }
          if ((ii > 1) || (!BRep_Tool::IsClosed(E)))
          { // Cut C
            occ::handle<Geom_TrimmedCurve> TC = new (Geom_TrimmedCurve)(C, First, Last);
            C                                 = TC;
          }
          //  otherwise preserve the integrity of the curve
        }
        NC.Append(C);
      }

      double Ufirst = ii - 1;
      double Ulast  = ii;
      myLaws->ChangeValue(ii) =
        new (GeomFill_NSections)(NC, myTrsfs, myParams, Ufirst, Ulast, VFirst, VLast, mySurface);
    }
  }
}

//=================================================================================================

bool BRepFill_NSections::IsVertex() const
{
  return false;
}

//=================================================================================================

bool BRepFill_NSections::IsConstant() const
{
  return false;
}

//=================================================================================================

TopoDS_Vertex BRepFill_NSections::Vertex(const int Index, const double Param) const
{
  BRep_Builder  B;
  TopoDS_Vertex V;
  B.MakeVertex(V);
  gp_Pnt P;

  if (Index <= myEdges->ColLength())
  {
    occ::handle<Geom_BSplineCurve> Curve =
      occ::down_cast<Geom_BSplineCurve>(myLaws->Value(Index)->BSplineSurface()->VIso(Param));
    double first = Curve->FirstParameter();
    Curve->D0(first, P);
    B.UpdateVertex(V, P, Precision::Confusion());
  }
  else if (Index == myEdges->ColLength() + 1)
  {
    occ::handle<Geom_BSplineCurve> Curve =
      occ::down_cast<Geom_BSplineCurve>(myLaws->Value(Index - 1)->BSplineSurface()->VIso(Param));
    double last = Curve->LastParameter();
    Curve->D0(last, P);
    B.UpdateVertex(V, P, Precision::Confusion());
  }

  return V;
}

///=======================================================================
// function : VertexTol
// purpose  : Evaluate the hole between 2 edges of the section
//=======================================================================
double BRepFill_NSections::VertexTol(const int Index, const double Param) const
{
  double Tol = Precision::Confusion();
  int    I1, I2;
  if ((Index == 0) || (Index == myEdges->ColLength()))
  {
    if (!uclosed)
      return Tol; // The least possible error
    I1 = myEdges->ColLength();
    I2 = 1;
  }
  else
  {
    I1 = Index;
    I2 = I1 + 1;
  }

  occ::handle<GeomFill_SectionLaw>         Loi;
  int                                      NbPoles, NbKnots, Degree;
  occ::handle<NCollection_HArray1<gp_Pnt>> Poles;
  occ::handle<NCollection_HArray1<double>> Knots, Weigth;
  occ::handle<NCollection_HArray1<int>>    Mults;
  occ::handle<Geom_BSplineCurve>           BS;
  gp_Pnt                                   PFirst;

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

occ::handle<GeomFill_SectionLaw> BRepFill_NSections::ConcatenedLaw() const
{
  occ::handle<GeomFill_SectionLaw> Law;
  if (myLaws->Length() == 1)
    return myLaws->Value(1);
  else
  {
    double Ufirst, Ulast, Vfirst, Vlast;
    mySurface->Bounds(Ufirst, Ulast, Vfirst, Vlast);
    NCollection_Sequence<occ::handle<Geom_Curve>> NCompo;
    NCompo.Clear();
    for (int jj = 1; jj <= myShapes.Length(); jj++)
    {
      NCompo.Append(mySurface->VIso(myParams(jj)));
    }
    Law =
      new (GeomFill_NSections)(NCompo, myTrsfs, myParams, Ufirst, Ulast, Vfirst, Vlast, mySurface);
  }
  return Law;
}

//=================================================================================================

GeomAbs_Shape BRepFill_NSections::Continuity(const int Index, const double TolAngular) const
{

  int           jj;
  GeomAbs_Shape cont_jj;
  GeomAbs_Shape cont = GeomAbs_C0;

  for (jj = 1; jj <= myShapes.Length(); jj++)
  {

    TopoDS_Edge Edge1, Edge2;
    if ((Index == 0) || (Index == myEdges->ColLength()))
    {
      if (!uclosed)
        return GeomAbs_C0; // The least possible error

      Edge1 = TopoDS::Edge(myEdges->Value(myEdges->ColLength(), jj));
      Edge2 = TopoDS::Edge(myEdges->Value(1, jj));
    }
    else
    {
      Edge1 = TopoDS::Edge(myEdges->Value(Index, jj));
      Edge2 = TopoDS::Edge(myEdges->Value(Index + 1, jj));
    }

    TopoDS_Vertex V1, V2;
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

    if (BRep_Tool::Degenerated(Edge1) || BRep_Tool::Degenerated(Edge2))
      cont_jj = GeomAbs_CN;
    else
    {
      double            U1 = BRep_Tool::Parameter(V1, Edge1);
      double            U2 = BRep_Tool::Parameter(V2, Edge2);
      BRepAdaptor_Curve Curve1(Edge1);
      BRepAdaptor_Curve Curve2(Edge2);
      double            Eps = BRep_Tool::Tolerance(V2) + BRep_Tool::Tolerance(V1);
      cont_jj               = BRepLProp::Continuity(Curve1, Curve2, U1, U2, Eps, TolAngular);
    }

    if (jj == 1)
      cont = cont_jj;
    if (cont > cont_jj)
      cont = cont_jj;
  }

  return cont;
}

//=================================================================================================

void BRepFill_NSections::D0(const double V, TopoDS_Shape& S)
{
  TopoDS_Wire      W;
  BRepLib_MakeWire MW;
  int              ii, NbEdge = myLaws->Length();
  for (ii = 1; ii <= NbEdge; ii++)
  {
    occ::handle<Geom_BSplineCurve> Curve =
      occ::down_cast<Geom_BSplineCurve>(myLaws->Value(ii)->BSplineSurface()->VIso(V));
    double      first = Curve->FirstParameter(), last = Curve->LastParameter();
    TopoDS_Edge E = BRepLib_MakeEdge(Curve, first, last);
    MW.Add(E);
  }
  TopAbs_Orientation Orien       = TopAbs_FORWARD;
  TopoDS_Shape       aLocalShape = MW.Wire().Oriented(Orien);
  S                              = TopoDS::Wire(aLocalShape);
  //  S = TopoDS::Wire(MW.Wire().Oriented(Orien));
}
