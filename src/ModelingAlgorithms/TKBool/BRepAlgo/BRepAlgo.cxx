// Created on: 1997-03-10
// Created by: Stagiaire Francois DUMONT
// Copyright (c) 1997-1999 Matra Datavision
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
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Curve2d.hxx>
#include <BRepAlgo.hxx>
#include <BRepLib.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <BRepLib_MakeWire.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <ElCLib.hxx>
#include <Geom_Curve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dConvert_ApproxArcsSegments.hxx>
#include <GeomAbs_CurveType.hxx>
#include <GeomConvert.hxx>
#include <GeomConvert_CompCurveToBSplineCurve.hxx>
#include <GeomLProp.hxx>
#include <NCollection_Vector.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <ShapeFix_Shape.hxx>
#include <Geom_BSplineCurve.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <NCollection_Sequence.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>

// The minimal tolerance of approximation (edges can be defined with yet smaller tolerance)
static const double MINIMAL_TOLERANCE = 0.0001;

namespace
{

struct OrientedCurve
{
  occ::handle<Geom2d_TrimmedCurve> Curve;
  bool                             IsReverse;

  inline gp_Pnt2d Point(const bool isEnd) const
  {
    if (isEnd == IsReverse)
      return Curve->StartPoint();
    return Curve->EndPoint();
  }
};

} // namespace

//=================================================================================================

TopoDS_Wire BRepAlgo::ConvertWire(const TopoDS_Wire& theWire,
                                  const double       theAngleTol,
                                  const TopoDS_Face& theFace)
{
  TopoDS_Wire                       aResult;
  double                            aMaxTol(0.);
  const occ::handle<Geom_Surface>   aSurf = BRep_Tool::Surface(theFace);
  NCollection_Vector<OrientedCurve> vecCurve;

  BRepTools_WireExplorer anExpE(theWire, theFace);
  // Explore the edges in the current wire, in their connection order
  for (; anExpE.More(); anExpE.Next())
  {
    const TopoDS_Edge&  anEdge = anExpE.Current();
    BRepAdaptor_Curve2d aCurve(anEdge, theFace);
    double              aTol = BRep_Tool::Tolerance(anEdge);
    if (aTol < MINIMAL_TOLERANCE)
      aTol = MINIMAL_TOLERANCE;
    if (aTol > aMaxTol)
      aMaxTol = aTol;
    Geom2dConvert_ApproxArcsSegments                       anAlgo(aCurve, aTol, theAngleTol);
    const NCollection_Sequence<occ::handle<Geom2d_Curve>>& aResultApprox = anAlgo.GetResult();

    // Form the array of approximated elementary curves
    if (anEdge.Orientation() == TopAbs_REVERSED)
    {
      for (int iCrv = aResultApprox.Length(); iCrv > 0; iCrv--)
      {
        const occ::handle<Geom2d_Curve>& aCrv = aResultApprox(iCrv);
        if (!aCrv.IsNull())
        {
          OrientedCurve& anOCurve = vecCurve.Append(OrientedCurve());
          anOCurve.Curve          = occ::down_cast<Geom2d_TrimmedCurve>(aCrv);
          anOCurve.IsReverse      = true;
        }
      }
    }
    else
    {
      for (int iCrv = 1; iCrv <= aResultApprox.Length(); iCrv++)
      {
        const occ::handle<Geom2d_Curve>& aCrv = aResultApprox(iCrv);
        if (!aCrv.IsNull())
        {
          OrientedCurve& anOCurve = vecCurve.Append(OrientedCurve());
          anOCurve.Curve          = occ::down_cast<Geom2d_TrimmedCurve>(aCrv);
          anOCurve.IsReverse      = false;
        }
      }
    }
  }

  if (vecCurve.Length() > 0)
  {
    // Build the first vertex
    BRep_Builder aVBuilder;
    gp_Pnt2d     aPnt[2] = {vecCurve(0).Point(false), vecCurve(vecCurve.Length() - 1).Point(true)};
    double       aDist   = aPnt[0].Distance(aPnt[1]);
    if (aDist > aMaxTol + Precision::Confusion())
      aDist = Precision::Confusion();
    else
    {
      aDist   = 0.5 * aDist + Precision::Confusion();
      aPnt[0] = 0.5 * (aPnt[0].XY() + aPnt[1].XY());
    }
    gp_Pnt aPnt3d;
    aSurf->D0(aPnt[0].X(), aPnt[0].Y(), aPnt3d);
    TopoDS_Vertex aFirstVertex;
    aVBuilder.MakeVertex(aFirstVertex, aPnt3d, aDist);

    // Loop creating edges
    BRepBuilderAPI_MakeWire aMkWire;
    TopoDS_Edge             anEdgeRes;
    TopoDS_Vertex           aVertex = aFirstVertex;
    for (int iCrv = 0; iCrv < vecCurve.Length(); iCrv++)
    {
      const OrientedCurve& anOCurve = vecCurve(iCrv);
      TopoDS_Vertex        aNextVertex;
      aPnt[0] = anOCurve.Point(true);
      if (iCrv == vecCurve.Length() - 1)
      {
        aPnt[1] = vecCurve(0).Point(false);
        aDist   = aPnt[0].Distance(aPnt[1]);
        if (aDist > aMaxTol + Precision::Confusion())
        {
          aSurf->D0(aPnt[0].X(), aPnt[0].Y(), aPnt3d);
          aVBuilder.MakeVertex(aNextVertex, aPnt3d, Precision::Confusion());
        }
        else
        {
          aNextVertex = aFirstVertex;
        }
      }
      else
      {
        aPnt[1] = vecCurve(iCrv + 1).Point(false);
        aDist   = 0.5 * (aPnt[0].Distance(aPnt[1])) + Precision::Confusion();
        aPnt[0] = 0.5 * (aPnt[0].XY() + aPnt[1].XY());
        aSurf->D0(aPnt[0].X(), aPnt[0].Y(), aPnt3d);
        aVBuilder.MakeVertex(aNextVertex, aPnt3d, aDist);
      }
      const double aParam[2] = {anOCurve.Curve->FirstParameter(), anOCurve.Curve->LastParameter()};
      if (anOCurve.IsReverse)
      {
        BRepBuilderAPI_MakeEdge aMkEdge(anOCurve.Curve,
                                        aSurf,
                                        aNextVertex,
                                        aVertex,
                                        aParam[0],
                                        aParam[1]);
        anEdgeRes = aMkEdge.Edge();
        anEdgeRes.Orientation(TopAbs_REVERSED);
      }
      else
      {
        BRepBuilderAPI_MakeEdge aMkEdge(anOCurve.Curve,
                                        aSurf,
                                        aVertex,
                                        aNextVertex,
                                        aParam[0],
                                        aParam[1]);
        anEdgeRes = aMkEdge.Edge();
      }
      aVertex = aNextVertex;
      aMkWire.Add(anEdgeRes);
    }

    if (aMkWire.IsDone())
      aResult = aMkWire.Wire();
  }
  return aResult;
}

//=================================================================================================

TopoDS_Face BRepAlgo::ConvertFace(const TopoDS_Face& theFace, const double theAngleTolerance)
{
  TopoDS_Face                     aResult;
  const occ::handle<Geom_Surface> aSurf = BRep_Tool::Surface(theFace);
  BRepBuilderAPI_MakeFace         aMkFace(aSurf, Precision::Confusion());

  TopExp_Explorer anExp(theFace, TopAbs_WIRE);
  for (; anExp.More(); anExp.Next())
  {
    const TopoDS_Wire& aWire    = TopoDS::Wire(anExp.Current());
    const TopoDS_Wire  aNewWire = ConvertWire(aWire, theAngleTolerance, theFace);
    aMkFace.Add(aNewWire);
  }
  if (aMkFace.IsDone())
  {
    aResult = aMkFace.Face();
  }
  return aResult;
}

//=================================================================================================

TopoDS_Wire BRepAlgo::ConcatenateWire(const TopoDS_Wire&  W,
                                      const GeomAbs_Shape Option,
                                      const double        TolAngular)
{

  int nb_curve, // number of curves in the Wire
    index;
  BRepTools_WireExplorer WExp(W);
  TopoDS_Edge            edge;
  TopLoc_Location        L;
  double                 First = 0., Last = 0., // extremal values for the curve
    First0 = 0., toler = 0., tolleft, tolright; // Vertex tolerances
  TopoDS_Vertex Vfirst, Vlast;                  // Vertex of the Wire
  gp_Pnt        Pfirst, Plast;                  //, Pint;  corresponding points

  BRepLib_MakeWire MakeResult;
  double           closed_tolerance = 0.0;
  bool             closed_flag      = false;

  nb_curve = 0;

  while (WExp.More())
  { // computation of the curve number
    nb_curve++;
    WExp.Next();
  }

  if (nb_curve > 1)
  {
    NCollection_Array1<occ::handle<Geom_BSplineCurve>> tab(0,
                                                           nb_curve
                                                             - 1); // array of the wire's curve
    NCollection_Array1<double> tabtolvertex(0, nb_curve - 2); // array of the tolerance's vertex

    WExp.Init(W);

    for (index = 0; index < nb_curve; index++)
    { // main loop
      edge                                    = WExp.Current();
      const occ::handle<Geom_Curve>& aCurve   = BRep_Tool::Curve(edge, L, First, Last);
      occ::handle<Geom_TrimmedCurve> aTrCurve = new Geom_TrimmedCurve(aCurve, First, Last);
      tab(index) = GeomConvert::CurveToBSplineCurve(aTrCurve); // storage in a array
      tab(index)->Transform(L.Transformation());
      GeomConvert::C0BSplineToC1BSplineCurve(tab(index), Precision::Confusion());

      if (index >= 1)
      { // continuity test loop
        if (edge.Orientation() == TopAbs_REVERSED)
          tab(index)->Reverse();
        tolleft                 = BRep_Tool::Tolerance(TopExp::LastVertex(edge));
        tolright                = BRep_Tool::Tolerance(TopExp::FirstVertex(edge));
        tabtolvertex(index - 1) = std::max(tolleft, tolright);
      }

      if (index == 0)
      { // storage of the first edge features
        First0 = First;
        if (edge.Orientation() == TopAbs_REVERSED)
        { //(useful for the closed wire)
          Vfirst = TopExp::LastVertex(edge);
          tab(index)->Reverse();
        }
        else
          Vfirst = TopExp::FirstVertex(edge);
      }

      if (index == nb_curve - 1)
      { // storage of the last edge features
        if (edge.Orientation() == TopAbs_REVERSED)
          Vlast = TopExp::FirstVertex(edge);
        else
          Vlast = TopExp::LastVertex(edge);
      }
      WExp.Next();
    }

    if (BRep_Tool::Tolerance(Vfirst)
        > BRep_Tool::Tolerance(Vlast)) // computation of the closing tolerance
      toler = BRep_Tool::Tolerance(Vfirst);
    else
      toler = BRep_Tool::Tolerance(Vlast);

    Pfirst = BRep_Tool::Pnt(Vfirst);
    Plast  = BRep_Tool::Pnt(Vlast);

    if ((Pfirst.Distance(Plast) <= toler) && // C0 continuity test at the closing point
        (GeomLProp::
           Continuity(tab(nb_curve - 1), tab(0), Last, First0, true, true, toler, TolAngular)
         >= GeomAbs_G1))
    {
      // clang-format off
      closed_tolerance =toler;                                        //if ClosedG1!=0 it will be True and
      // clang-format on
      closed_flag = true;
    } // with the toler value
    occ::handle<NCollection_HArray1<occ::handle<Geom_BSplineCurve>>>
                                          concatcurve;    // array of the concatenated curves
    occ::handle<NCollection_HArray1<int>> ArrayOfIndices; // array of the remaining Vertex
    if (Option == GeomAbs_G1)
      GeomConvert::ConcatG1(tab,
                            tabtolvertex,
                            concatcurve,
                            closed_flag,
                            closed_tolerance); // G1 concatenation
    else
      GeomConvert::ConcatC1(tab,
                            tabtolvertex,
                            ArrayOfIndices,
                            concatcurve,
                            closed_flag,
                            closed_tolerance); // C1 concatenation

    for (index = 0; index <= (concatcurve->Length() - 1); index++)
    { // building of the resulting Wire
      BRepLib_MakeEdge EdgeBuilder(concatcurve->Value(index));
      edge = EdgeBuilder.Edge();
      MakeResult.Add(edge);
    }
  }
  else
  {

    WExp.Init(W);

    edge                              = WExp.Current();
    const occ::handle<Geom_Curve>& aC = BRep_Tool::Curve(edge, L, First, Last);
    occ::handle<Geom_BSplineCurve> aBS =
      GeomConvert::CurveToBSplineCurve(new Geom_TrimmedCurve(aC, First, Last));
    aBS->Transform(L.Transformation());
    GeomConvert::C0BSplineToC1BSplineCurve(aBS, Precision::Confusion());
    if (edge.Orientation() == TopAbs_REVERSED)
    {
      aBS->Reverse();
    }

    BRepLib_MakeEdge EdgeBuilder(aBS);
    edge = EdgeBuilder.Edge();
    MakeResult.Add(edge);
  }
  return MakeResult.Wire();
}

//=================================================================================================

TopoDS_Edge BRepAlgo::ConcatenateWireC0(const TopoDS_Wire& aWire)
{
  double LinTol = Precision::Confusion();
  double AngTol = Precision::Angular();

  TopoDS_Edge ResEdge;

  TopoDS_Wire theWire = aWire;
  BRepLib::BuildCurves3d(theWire);
  occ::handle<ShapeFix_Shape> Fixer = new ShapeFix_Shape(theWire);
  Fixer->SetPrecision(LinTol);
  Fixer->SetMaxTolerance(LinTol);
  Fixer->Perform();
  theWire = TopoDS::Wire(Fixer->Shape());

  NCollection_Sequence<occ::handle<Geom_Curve>> CurveSeq;
  NCollection_Sequence<double>                  FparSeq;
  NCollection_Sequence<double>                  LparSeq;
  NCollection_Sequence<double>                  TolSeq;
  NCollection_Sequence<bool>                    IsFwdSeq;
  GeomAbs_CurveType                             CurType = GeomAbs_OtherCurve;
  TopoDS_Vertex                                 FirstVertex, LastVertex;

  BRepTools_WireExplorer wexp(theWire);

  for (; wexp.More(); wexp.Next())
  {
    const TopoDS_Edge&      anEdge = wexp.Current();
    double                  fpar, lpar;
    occ::handle<Geom_Curve> aCurve = BRep_Tool::Curve(anEdge, fpar, lpar);

    if (aCurve.IsNull())
      continue;

    GeomAdaptor_Curve              aGACurve(aCurve);
    GeomAbs_CurveType              aType       = aGACurve.GetType();
    const occ::handle<Geom_Curve>& aBasisCurve = aGACurve.Curve();
    bool                           isFwd       = (wexp.Orientation() != TopAbs_REVERSED);

    if (aBasisCurve->IsPeriodic())
    {
      ElCLib::AdjustPeriodic(aBasisCurve->FirstParameter(),
                             aBasisCurve->LastParameter(),
                             Precision::PConfusion(),
                             fpar,
                             lpar);
    }

    if (CurveSeq.IsEmpty())
    {
      CurveSeq.Append(aCurve);
      FparSeq.Append(fpar);
      LparSeq.Append(lpar);
      IsFwdSeq.Append(isFwd);
      CurType     = aType;
      FirstVertex = wexp.CurrentVertex();
    }
    else
    {
      bool              isSameCurve = false;
      double            NewFpar = RealFirst(), NewLpar = RealLast();
      GeomAdaptor_Curve GAprevcurve(CurveSeq.Last());

      if (aCurve == CurveSeq.Last())
      {
        NewFpar     = fpar;
        NewLpar     = lpar;
        isSameCurve = true;
      }
      else if (aType == CurType)
      {
        switch (aType)
        {
          case GeomAbs_Line: {
            gp_Lin aLine    = aGACurve.Line();
            gp_Lin PrevLine = GAprevcurve.Line();

            if (aLine.Contains(PrevLine.Location(), LinTol)
                && aLine.Direction().IsParallel(PrevLine.Direction(), AngTol))
            {
              gp_Pnt P1 = ElCLib::Value(fpar, aLine);
              gp_Pnt P2 = ElCLib::Value(lpar, aLine);

              NewFpar     = ElCLib::Parameter(PrevLine, P1);
              NewLpar     = ElCLib::Parameter(PrevLine, P2);
              isSameCurve = true;
            }
            break;
          }
          case GeomAbs_Circle: {
            gp_Circ aCircle    = aGACurve.Circle();
            gp_Circ PrevCircle = GAprevcurve.Circle();

            if (aCircle.Location().Distance(PrevCircle.Location()) <= LinTol
                && std::abs(aCircle.Radius() - PrevCircle.Radius()) <= LinTol
                && aCircle.Axis().IsParallel(PrevCircle.Axis(), AngTol))
            {
              gp_Pnt P1 = ElCLib::Value(fpar, aCircle);
              gp_Pnt P2 = ElCLib::Value(lpar, aCircle);

              NewFpar     = ElCLib::Parameter(PrevCircle, P1);
              NewLpar     = ElCLib::Parameter(PrevCircle, P2);
              isSameCurve = true;
            }
            break;
          }
          case GeomAbs_Ellipse: {
            gp_Elips anEllipse   = aGACurve.Ellipse();
            gp_Elips PrevEllipse = GAprevcurve.Ellipse();

            if (anEllipse.Focus1().Distance(PrevEllipse.Focus1()) <= LinTol
                && anEllipse.Focus2().Distance(PrevEllipse.Focus2()) <= LinTol
                && std::abs(anEllipse.MajorRadius() - PrevEllipse.MajorRadius()) <= LinTol
                && std::abs(anEllipse.MinorRadius() - PrevEllipse.MinorRadius()) <= LinTol
                && anEllipse.Axis().IsParallel(PrevEllipse.Axis(), AngTol))
            {
              gp_Pnt P1 = ElCLib::Value(fpar, anEllipse);
              gp_Pnt P2 = ElCLib::Value(lpar, anEllipse);

              NewFpar     = ElCLib::Parameter(PrevEllipse, P1);
              NewLpar     = ElCLib::Parameter(PrevEllipse, P2);
              isSameCurve = true;
            }
            break;
          }
          case GeomAbs_Hyperbola: {
            gp_Hypr aHypr    = aGACurve.Hyperbola();
            gp_Hypr PrevHypr = GAprevcurve.Hyperbola();

            if (aHypr.Focus1().Distance(PrevHypr.Focus1()) <= LinTol
                && aHypr.Focus2().Distance(PrevHypr.Focus2()) <= LinTol
                && std::abs(aHypr.MajorRadius() - PrevHypr.MajorRadius()) <= LinTol
                && std::abs(aHypr.MinorRadius() - PrevHypr.MinorRadius()) <= LinTol
                && aHypr.Axis().IsParallel(PrevHypr.Axis(), AngTol))
            {
              gp_Pnt P1 = ElCLib::Value(fpar, aHypr);
              gp_Pnt P2 = ElCLib::Value(lpar, aHypr);

              NewFpar     = ElCLib::Parameter(PrevHypr, P1);
              NewLpar     = ElCLib::Parameter(PrevHypr, P2);
              isSameCurve = true;
            }
            break;
          }
          case GeomAbs_Parabola: {
            gp_Parab aParab    = aGACurve.Parabola();
            gp_Parab PrevParab = GAprevcurve.Parabola();

            if (aParab.Location().Distance(PrevParab.Location()) <= LinTol
                && aParab.Focus().Distance(PrevParab.Focus()) <= LinTol
                && std::abs(aParab.Focal() - PrevParab.Focal()) <= LinTol
                && aParab.Axis().IsParallel(PrevParab.Axis(), AngTol))
            {
              gp_Pnt P1 = ElCLib::Value(fpar, aParab);
              gp_Pnt P2 = ElCLib::Value(lpar, aParab);

              NewFpar     = ElCLib::Parameter(PrevParab, P1);
              NewLpar     = ElCLib::Parameter(PrevParab, P2);
              isSameCurve = true;
            }
            break;
          }
          default:
            break;
        } // end of switch
      } // end of else

      if (isSameCurve)
      {
        const bool isSameDir = (isFwd == IsFwdSeq.Last());

        if (aBasisCurve->IsPeriodic())
        {
          // Treat periodic curves.
          const double aPeriod = aBasisCurve->Period();

          if (isSameDir)
          {
            // Check if first parameter is greater then the last one.
            while (NewFpar > NewLpar)
            {
              NewFpar -= aPeriod;
            }
          }
          else
          { // !isSameDir
            // Check if last parameter is greater then the first one.
            while (NewLpar > NewFpar)
            {
              NewLpar -= aPeriod;
            }

            // Change parameters
            const double aTmpPar = NewLpar;

            NewLpar = NewFpar;
            NewFpar = aTmpPar;
          }

          // Udjust parameters on periodic curves.
          if (IsFwdSeq.Last())
          {
            // The current curve should be after the previous one.
            ElCLib::AdjustPeriodic(LparSeq.Last(),
                                   LparSeq.Last() + aPeriod,
                                   Precision::PConfusion(),
                                   NewFpar,
                                   NewLpar);
          }
          else
          {
            // The current curve should be before the previous one.
            ElCLib::AdjustPeriodic(FparSeq.Last() - aPeriod,
                                   FparSeq.Last(),
                                   Precision::PConfusion(),
                                   NewFpar,
                                   NewLpar);
          }
        }
        else if (!isSameDir)
        {
          // Not periodic curves. Opposite dirs.
          const double aTmpPar = NewLpar;

          NewLpar = NewFpar;
          NewFpar = aTmpPar;
        }

        if (IsFwdSeq.Last())
        {
          // Update last parameter
          LparSeq(LparSeq.Length()) = NewLpar;
        }
        else
        {
          // Update first parameter
          FparSeq(FparSeq.Length()) = NewFpar;
        }
      }
      else
      {
        // Add new curve.
        CurveSeq.Append(aCurve);
        FparSeq.Append(fpar);
        LparSeq.Append(lpar);
        IsFwdSeq.Append(isFwd);
        TolSeq.Append(BRep_Tool::Tolerance(wexp.CurrentVertex()));
        CurType = aType;
      }
    }
  }

  LastVertex = wexp.CurrentVertex();
  TolSeq.Append(BRep_Tool::Tolerance(LastVertex));

  bool isReverse = false;

  if (!IsFwdSeq.IsEmpty())
  {
    isReverse = !IsFwdSeq(1);
  }

  TopoDS_Vertex FirstVtx_final, LastVtx_final;
  if (isReverse)
  {
    FirstVtx_final = LastVertex;
    LastVtx_final  = FirstVertex;
  }
  else
  {
    FirstVtx_final = FirstVertex;
    LastVtx_final  = LastVertex;
  }
  FirstVtx_final.Orientation(TopAbs_FORWARD);
  LastVtx_final.Orientation(TopAbs_REVERSED);

  if (CurveSeq.IsEmpty())
    return ResEdge;

  int nb_curve = CurveSeq.Length();                                        // number of curves
  NCollection_Array1<occ::handle<Geom_BSplineCurve>> tab(0, nb_curve - 1); // array of the curves
  NCollection_Array1<double>                         tabtolvertex(0,
                                          nb_curve
                                            - 1); //(0,nb_curve-2);  //array of the tolerances

  int i;

  if (nb_curve > 1)
  {
    for (i = 1; i <= nb_curve; i++)
    {
      if (CurveSeq(i)->IsInstance(STANDARD_TYPE(Geom_TrimmedCurve)))
        CurveSeq(i) = (*((occ::handle<Geom_TrimmedCurve>*)&(CurveSeq(i))))->BasisCurve();

      occ::handle<Geom_TrimmedCurve> aTrCurve =
        new Geom_TrimmedCurve(CurveSeq(i), FparSeq(i), LparSeq(i));
      tab(i - 1) = GeomConvert::CurveToBSplineCurve(aTrCurve);
      GeomConvert::C0BSplineToC1BSplineCurve(tab(i - 1), Precision::Confusion());

      if (!IsFwdSeq(i))
      {
        tab(i - 1)->Reverse();
      }

      // Temporary
      // char* name = new char[100];
      // Sprintf(name, "c%d", i);
      // DrawTrSurf::Set(name, tab(i-1));

      if (i > 1)
        tabtolvertex(i - 2) = TolSeq(i - 1) * 5.;
    }
    tabtolvertex(nb_curve - 1) = TolSeq(TolSeq.Length()) * 5.;

    bool   closed_flag      = false;
    double closed_tolerance = 0.;
    if (FirstVertex.IsSame(LastVertex)
        && GeomLProp::Continuity(tab(0),
                                 tab(nb_curve - 1),
                                 tab(0)->FirstParameter(),
                                 tab(nb_curve - 1)->LastParameter(),
                                 false,
                                 false,
                                 LinTol,
                                 AngTol)
             >= GeomAbs_G1)
    {
      closed_flag      = true;
      closed_tolerance = BRep_Tool::Tolerance(FirstVertex);
    }

    occ::handle<NCollection_HArray1<occ::handle<Geom_BSplineCurve>>>
                                          concatcurve;    // array of the concatenated curves
    occ::handle<NCollection_HArray1<int>> ArrayOfIndices; // array of the remaining Vertex
    GeomConvert::ConcatC1(tab,
                          tabtolvertex,
                          ArrayOfIndices,
                          concatcurve,
                          closed_flag,
                          closed_tolerance); // C1 concatenation

    if (concatcurve->Length() > 1)
    {
      double MaxTolVer = LinTol;
      for (i = 1; i <= TolSeq.Length(); i++)
        if (TolSeq(i) > MaxTolVer)
          MaxTolVer = TolSeq(i);
      MaxTolVer *= 5.;

      GeomConvert_CompCurveToBSplineCurve Concat(concatcurve->Value(concatcurve->Lower()));

      for (i = concatcurve->Lower() + 1; i <= concatcurve->Upper(); i++)
        Concat.Add(concatcurve->Value(i), MaxTolVer, true);

      concatcurve->SetValue(concatcurve->Lower(), Concat.BSplineCurve());
    }

    if (isReverse)
    {
      concatcurve->ChangeValue(concatcurve->Lower())->Reverse();
    }
    ResEdge = BRepLib_MakeEdge(concatcurve->Value(concatcurve->Lower()),
                               FirstVtx_final,
                               LastVtx_final,
                               concatcurve->Value(concatcurve->Lower())->FirstParameter(),
                               concatcurve->Value(concatcurve->Lower())->LastParameter());
  }
  else
  {
    if (CurveSeq(1)->IsInstance(STANDARD_TYPE(Geom_TrimmedCurve)))
      CurveSeq(1) = (*((occ::handle<Geom_TrimmedCurve>*)&(CurveSeq(1))))->BasisCurve();

    occ::handle<Geom_Curve> aCopyCurve = occ::down_cast<Geom_Curve>(CurveSeq(1)->Copy());

    ResEdge = BRepLib_MakeEdge(aCopyCurve, FirstVtx_final, LastVtx_final, FparSeq(1), LparSeq(1));
  }

  if (isReverse)
    ResEdge.Reverse();

  return ResEdge;
}
