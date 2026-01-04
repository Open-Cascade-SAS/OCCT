// Created on: 1997-10-13
// Created by: Roman BORISOV
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

#include <Adaptor3d_Curve.hxx>
#include <Approx_CurveOnSurface.hxx>
#include <BRep_Builder.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepAlgo_NormalProjection.hxx>
#include <BRepAlgoAPI_Section.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <BRepLib_MakeVertex.hxx>
#include <BRepLib_MakeWire.hxx>
#include <BRepTopAdaptor_FClass2d.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <GeomAdaptor.hxx>
#include <Precision.hxx>
#include <ProjLib_HCompProjectedCurve.hxx>
#include <StdFail_NotDone.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Integer.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>

#ifdef OCCT_DEBUG_CHRONO
  #include <OSD_Timer.hxx>

OSD_Chronometer chr_total, chr_init, chr_approx, chr_booltool;

double                 t_total, t_init, t_approx, t_booltool;
Standard_IMPORT double t_init_point, t_dicho_bound;
Standard_IMPORT int    init_point_count, dicho_bound_count;

void InitChron(OSD_Chronometer& ch)
{
  ch.Reset();
  ch.Start();
}

void ResultChron(OSD_Chronometer& ch, double& time)
{
  double tch;
  ch.Stop();
  ch.Show(tch);
  time = time + tch;
}

#endif
//=================================================================================================

BRepAlgo_NormalProjection::BRepAlgo_NormalProjection()
    : myIsDone(false),
      myMaxDist(-1.),
      myWith3d(true),
      myFaceBounds(true)
{
  BRep_Builder BB;
  BB.MakeCompound(TopoDS::Compound(myToProj));
  SetDefaultParams();
}

//=================================================================================================

BRepAlgo_NormalProjection::BRepAlgo_NormalProjection(const TopoDS_Shape& S)
    : myIsDone(false),
      myMaxDist(-1.),
      myWith3d(true),
      myFaceBounds(true)
{
  BRep_Builder BB;
  BB.MakeCompound(TopoDS::Compound(myToProj));
  SetDefaultParams();
  Init(S);
}

//=================================================================================================

void BRepAlgo_NormalProjection::Init(const TopoDS_Shape& S)
{
  myShape = S;
}

//=================================================================================================

void BRepAlgo_NormalProjection::Add(const TopoDS_Shape& ToProj)
{
  BRep_Builder BB;
  BB.Add(myToProj, ToProj);
}

//=================================================================================================

void BRepAlgo_NormalProjection::SetParams(const double        Tol3D,
                                          const double        Tol2D,
                                          const GeomAbs_Shape InternalContinuity,
                                          const int           MaxDegree,
                                          const int           MaxSeg)
{
  myTol3d      = Tol3D;
  myTol2d      = Tol2D;
  myContinuity = InternalContinuity;
  myMaxDegree  = MaxDegree;
  myMaxSeg     = MaxSeg;
}

//=================================================================================================

void BRepAlgo_NormalProjection::SetDefaultParams()
{
  myTol3d      = 1.e-4;
  myTol2d      = std::pow(myTol3d, 2. / 3);
  myContinuity = GeomAbs_C1;
  myMaxDegree  = 14;
  myMaxSeg     = 16;
}

//=================================================================================================

void BRepAlgo_NormalProjection::SetLimit(const bool FaceBounds)
{
  myFaceBounds = FaceBounds;
}

//=================================================================================================

void BRepAlgo_NormalProjection::SetMaxDistance(const double MaxDist)
{
  myMaxDist = MaxDist;
}

//=================================================================================================

void BRepAlgo_NormalProjection::Compute3d(const bool With3d)
{
  myWith3d = With3d;
}

//=================================================================================================

void BRepAlgo_NormalProjection::Build()
{
#ifdef OCCT_DEBUG_CHRONO
  int init_count = 0, approx_count = 0, booltool_count = 0;
  t_total    = 0;
  t_init     = 0;
  t_approx   = 0;
  t_booltool = 0;

  t_init_point     = 0;
  init_point_count = 0;

  t_dicho_bound     = 0;
  dicho_bound_count = 0;

  InitChron(chr_total);
#endif
  myIsDone = false;

  occ::handle<NCollection_HSequence<TopoDS_Shape>> Edges =
    new NCollection_HSequence<TopoDS_Shape>();
  occ::handle<NCollection_HSequence<TopoDS_Shape>> Faces =
    new NCollection_HSequence<TopoDS_Shape>();
  NCollection_List<TopoDS_Shape> DescenList;
  int                            NbEdges = 0, NbFaces = 0, i, j, k;
  TopExp_Explorer                ExpOfWire, ExpOfShape;
  double                         Udeb, Ufin;
  TopoDS_Shape                   VertexRes;
  bool                           Only3d, Only2d, Elementary;

  // for isoparametric cases
  NCollection_Array1<gp_Pnt2d> Poles(1, 2);
  NCollection_Array1<double>   Knots(1, 2);
  NCollection_Array1<int>      Mults(1, 2);
  int                          Deg;
  Deg      = 1;
  Mults(1) = Deg + 1;
  Mults(2) = Deg + 1;
  //

  for (ExpOfWire.Init(myToProj, TopAbs_EDGE); ExpOfWire.More(); ExpOfWire.Next(), NbEdges++)
  {
    Edges->Append(ExpOfWire.Current());
  }

  for (ExpOfShape.Init(myShape, TopAbs_FACE); ExpOfShape.More(); ExpOfShape.Next(), NbFaces++)
  {
    Faces->Append(ExpOfShape.Current());
  }

  BRep_Builder BB;
  BB.MakeCompound(TopoDS::Compound(myRes));
  BB.MakeCompound(TopoDS::Compound(VertexRes));
  bool YaVertexRes = false;

  for (i = 1; i <= NbEdges; i++)
  {
    DescenList.Clear();
    occ::handle<BRepAdaptor_Curve> hcur = new BRepAdaptor_Curve(TopoDS::Edge(Edges->Value(i)));
    Elementary                          = IsElementary(*hcur);
    for (j = 1; j <= NbFaces; j++)
    {
      occ::handle<BRepAdaptor_Surface> hsur =
        new BRepAdaptor_Surface(TopoDS::Face(Faces->Value(j)));

      // computation of  TolU and TolV

      double TolU, TolV;

      TolU = hsur->UResolution(myTol3d) / 20;
      TolV = hsur->VResolution(myTol3d) / 20;
      // Projection
#ifdef OCCT_DEBUG_CHRONO
      InitChron(chr_init);
#endif
      occ::handle<ProjLib_HCompProjectedCurve> HProjector =
        new ProjLib_HCompProjectedCurve(hsur, hcur, TolU, TolV, myMaxDist);
#ifdef OCCT_DEBUG_CHRONO
      ResultChron(chr_init, t_init);
      init_count++;
#endif
      //
      TopoDS_Shape prj;
      bool         Degenerated = false;
      gp_Pnt2d     P2d, Pdeb, Pfin;
      gp_Pnt       P;
      double       UIso, VIso;

      occ::handle<Adaptor2d_Curve2d> HPCur;
      occ::handle<Geom2d_Curve>      PCur2d; // Only for isoparametric projection

      for (k = 1; k <= HProjector->NbCurves(); k++)
      {
        if (HProjector->IsSinglePnt(k, P2d))
        {
#ifdef OCCT_DEBUG
          std::cout << "Projection of edge " << i << " on face " << j;
          std::cout << " is punctual" << std::endl << std::endl;
#endif
          HProjector->GetSurface()->D0(P2d.X(), P2d.Y(), P);
          prj = BRepLib_MakeVertex(P).Shape();
          DescenList.Append(prj);
          BB.Add(VertexRes, prj);
          YaVertexRes = true;

          myAncestorMap.Bind(prj, Edges->Value(i));
        }
        else
        {
          Only2d = Only3d = false;
          HProjector->Bounds(k, Udeb, Ufin);

          /**************************************************************/
          if (HProjector->IsUIso(k, UIso))
          {
#ifdef OCCT_DEBUG
            std::cout << "Projection of edge " << i << " on face " << j;
            std::cout << " is U-isoparametric" << std::endl << std::endl;
#endif
            HProjector->D0(Udeb, Pdeb);
            HProjector->D0(Ufin, Pfin);
            Poles(1) = Pdeb;
            Poles(2) = Pfin;
            Knots(1) = Udeb;
            Knots(2) = Ufin;
            occ::handle<Geom2d_BSplineCurve> BS2d =
              new Geom2d_BSplineCurve(Poles, Knots, Mults, Deg);
            PCur2d = new Geom2d_TrimmedCurve(BS2d, Udeb, Ufin);
            HPCur  = new Geom2dAdaptor_Curve(PCur2d);
            Only3d = true;
          }
          else if (HProjector->IsVIso(k, VIso))
          {
#ifdef OCCT_DEBUG
            std::cout << "Projection of edge " << i << " on face " << j;
            std::cout << " is V-isoparametric" << std::endl << std::endl;
#endif
            HProjector->D0(Udeb, Pdeb);
            HProjector->D0(Ufin, Pfin);
            Poles(1) = Pdeb;
            Poles(2) = Pfin;
            Knots(1) = Udeb;
            Knots(2) = Ufin;
            occ::handle<Geom2d_BSplineCurve> BS2d =
              new Geom2d_BSplineCurve(Poles, Knots, Mults, Deg);
            PCur2d = new Geom2d_TrimmedCurve(BS2d, Udeb, Ufin);
            HPCur  = new Geom2dAdaptor_Curve(PCur2d);
            Only3d = true;
          }
          else
            HPCur = HProjector;

          if ((!myWith3d || Elementary) && (HProjector->MaxDistance(k) <= myTol3d))
            Only2d = true;

          if (Only2d && Only3d)
          {
            BRepLib_MakeEdge MKed(GeomAdaptor::MakeCurve(*hcur), Udeb, Ufin);
            prj = MKed.Edge();
            BB.UpdateEdge(TopoDS::Edge(prj), PCur2d, TopoDS::Face(Faces->Value(j)), myTol3d);
            BB.UpdateVertex(TopExp::FirstVertex(TopoDS::Edge(prj)), myTol3d);
            BB.UpdateVertex(TopExp::LastVertex(TopoDS::Edge(prj)), myTol3d);
          }
          else
          {
#ifdef OCCT_DEBUG_CHRONO
            InitChron(chr_approx);
#endif
            Approx_CurveOnSurface appr(HPCur, hsur, Udeb, Ufin, myTol3d);
            appr.Perform(myMaxSeg, myMaxDegree, myContinuity, Only3d, Only2d);

            if (appr.MaxError3d() > 1.e3 * myTol3d)
              continue;

#ifdef OCCT_DEBUG_CHRONO
            ResultChron(chr_approx, t_approx);
            approx_count++;

            std::cout << "Approximation.IsDone = " << appr.IsDone() << std::endl;
            if (!Only2d)
              std::cout << "MaxError3d = " << appr.MaxError3d() << std::endl << std::endl;
            if (!Only3d)
            {
              std::cout << "MaxError2dU = " << appr.MaxError2dU() << std::endl;
              std::cout << "MaxError2dV = " << appr.MaxError2dV() << std::endl << std::endl;
            }
#endif

            if (!Only3d)
              PCur2d = appr.Curve2d();
            if (Only2d)
            {
              BRepLib_MakeEdge MKed(GeomAdaptor::MakeCurve(*hcur), Udeb, Ufin);
              prj = MKed.Edge();
            }
            else
            {
              // It is tested if the solution is not degenerated to set the
              // flag on edge, one takes several points, checks if the cloud of
              // points has less diameter than the tolerance 3D
              Degenerated = true;
              double                         Dist;
              occ::handle<Geom_BSplineCurve> BS3d = appr.Curve3d();
              gp_Pnt                         P1(0., 0., 0.), PP; // skl : I change "P" to "PP"
              int                            NbPoint, ii;        // skl : I change "i" to "ii"
              double                         Par, DPar;
              // start from 3 points to reject non degenerated edges
              // very fast
              NbPoint = 3;
              DPar    = (BS3d->LastParameter() - BS3d->FirstParameter()) / (NbPoint - 1);
              for (ii = 0; ii < NbPoint; ii++)
              {
                Par = BS3d->FirstParameter() + ii * DPar;
                PP  = BS3d->Value(Par);
                P1.SetXYZ(P1.XYZ() + PP.XYZ() / NbPoint);
              }
              for (ii = 0; ii < NbPoint && Degenerated; ii++)
              {
                Par  = BS3d->FirstParameter() + ii * DPar;
                PP   = BS3d->Value(Par);
                Dist = P1.Distance(PP);
                if (Dist > myTol3d)
                {
                  Degenerated = false;
                  break;
                }
              }
              // if the test passes a more exact test with 10 points
              if (Degenerated)
              {
                P1.SetCoord(0., 0., 0.);
                NbPoint = 10;
                DPar    = (BS3d->LastParameter() - BS3d->FirstParameter()) / (NbPoint - 1);
                for (ii = 0; ii < NbPoint; ii++)
                {
                  Par = BS3d->FirstParameter() + ii * DPar;
                  PP  = BS3d->Value(Par);
                  P1.SetXYZ(P1.XYZ() + PP.XYZ() / NbPoint);
                }
                for (ii = 0; ii < NbPoint && Degenerated; ii++)
                {
                  Par  = BS3d->FirstParameter() + ii * DPar;
                  PP   = BS3d->Value(Par);
                  Dist = P1.Distance(PP);
                  if (Dist > myTol3d)
                  {
                    Degenerated = false;
                    break;
                  }
                }
              }
              if (Degenerated)
              {
#ifdef OCCT_DEBUG
                std::cout << "Projection of edge " << i << " on face " << j;
                std::cout << " is degenerated " << std::endl << std::endl;
#endif
                TopoDS_Vertex VV;
                BB.MakeVertex(VV);
                BB.UpdateVertex(VV, P1, myTol3d);
                BB.MakeEdge(TopoDS::Edge(prj));
                BB.Add(TopoDS::Edge(prj), VV.Oriented(TopAbs_FORWARD));
                BB.Add(TopoDS::Edge(prj), VV.Oriented(TopAbs_REVERSED));
                BB.Degenerated(TopoDS::Edge(prj), true);
              }
              else
              {
                prj = BRepLib_MakeEdge(BS3d).Edge();
              }
            }

            BB.UpdateEdge(TopoDS::Edge(prj),
                          PCur2d,
                          TopoDS::Face(Faces->Value(j)),
                          appr.MaxError3d());
            BB.UpdateVertex(TopExp::FirstVertex(TopoDS::Edge(prj)), appr.MaxError3d());
            BB.UpdateVertex(TopExp::LastVertex(TopoDS::Edge(prj)), appr.MaxError3d());
            if (Degenerated)
            {
              BB.Range(TopoDS::Edge(prj), TopoDS::Face(Faces->Value(j)), Udeb, Ufin);
            }
          }

          if (myFaceBounds)
          {
            // Trimming edges by face bounds
            // if the solution is degenerated, use of BoolTool is avoided
#ifdef OCCT_DEBUG_CHRONO
            InitChron(chr_booltool);
#endif
            if (!Degenerated)
            {
              // Perform Boolean COMMON operation to get parts of projected edge
              // inside the face
              BRepAlgoAPI_Section aSection(Faces->Value(j), prj);
              if (aSection.IsDone())
              {
                const TopoDS_Shape& aRC = aSection.Shape();
                //
                TopExp_Explorer aExpE(aRC, TopAbs_EDGE);
                for (; aExpE.More(); aExpE.Next())
                {
                  const TopoDS_Shape& aE = aExpE.Current();
                  BB.Add(myRes, aE);
                  myAncestorMap.Bind(aE, Edges->Value(i));
                  myCorresp.Bind(aE, Faces->Value(j));
                }
              }
              else
              {
                // if the common operation has failed, try to classify the part
                BRepTopAdaptor_FClass2d classifier(TopoDS::Face(Faces->Value(j)),
                                                   Precision::Confusion());
                gp_Pnt2d                Puv;
                double                  f    = PCur2d->FirstParameter();
                double                  l    = PCur2d->LastParameter();
                double                  pmil = (f + l) / 2;
                PCur2d->D0(pmil, Puv);
                TopAbs_State state;
                state = classifier.Perform(Puv);
                if (state == TopAbs_IN || state == TopAbs_ON)
                {
                  BB.Add(myRes, prj);
                  DescenList.Append(prj);
                  myAncestorMap.Bind(prj, Edges->Value(i));
                  myCorresp.Bind(prj, Faces->Value(j));
                }
              }
            }
            else
            {
#ifdef OCCT_DEBUG
              std::cout << " BooleanOperations : no solution " << std::endl;
#endif

              BRepTopAdaptor_FClass2d classifier(TopoDS::Face(Faces->Value(j)),
                                                 Precision::Confusion());
              gp_Pnt2d                Puv;
              double                  f    = PCur2d->FirstParameter();
              double                  l    = PCur2d->LastParameter();
              double                  pmil = (f + l) / 2;
              PCur2d->D0(pmil, Puv);
              TopAbs_State state;
              state = classifier.Perform(Puv);
              if (state == TopAbs_IN || state == TopAbs_ON)
              {
                BB.Add(myRes, prj);
                DescenList.Append(prj);
                myAncestorMap.Bind(prj, Edges->Value(i));
                myCorresp.Bind(prj, Faces->Value(j));
              }
#ifdef OCCT_DEBUG_CHRONO
              ResultChron(chr_booltool, t_booltool);
              booltool_count++;
#endif
            }
          }
          else
          {
            BB.Add(myRes, prj);
            DescenList.Append(prj);
            myAncestorMap.Bind(prj, Edges->Value(i));
            myCorresp.Bind(prj, Faces->Value(j));
          }
        }
      }
    }
    myDescendants.Bind(Edges->Value(i), DescenList);
  }
  // JPI : eventual wire creation is reported in a specific method
  //       BuilWire that can be called by the user. Otherwise, the
  //       relations of map myAncestorMap, myCorresp will be lost.

  if (YaVertexRes)
    BB.Add(myRes, VertexRes);

  myIsDone = true;

#ifdef OCCT_DEBUG_CHRONO
  ResultChron(chr_total, t_total);

  std::cout << "Build - Total time  : " << t_total << " includes:" << std::endl;
  std::cout << "- Projection           : " << t_init << std::endl;
  std::cout << "  -- Initial point search : " << t_init_point << std::endl;
  std::cout << "  -- DichoBound search : " << t_dicho_bound << std::endl;
  std::cout << "- Approximation        : " << t_approx << std::endl;
  std::cout << "- Boolean operation    : " << t_booltool << std::endl;
  std::cout << "- Rest of time         : " << t_total - (t_init + t_approx + t_booltool)
            << std::endl
            << std::endl;
  if (init_count != 0)
    t_init /= init_count;
  if (init_point_count != 0)
    t_init_point /= init_point_count;
  if (dicho_bound_count != 0)
    t_dicho_bound /= dicho_bound_count;
  if (approx_count != 0)
    t_approx /= approx_count;
  if (booltool_count != 0)
    t_booltool /= booltool_count;

  std::cout << "Unitary average time  : " << std::endl;
  std::cout << "- Projection          : " << t_init << std::endl;
  std::cout << "  -- Initial point search: " << t_init_point << std::endl;
  std::cout << "  -- DichoBound search : " << t_dicho_bound << std::endl;
  std::cout << "- Approximation       : " << t_approx << std::endl;
  std::cout << "- Boolean operation   :" << t_booltool << std::endl;
  std::cout << std::endl
            << "Number of initial point computations is " << init_point_count << std::endl
            << std::endl;
#endif
}

//=================================================================================================

bool BRepAlgo_NormalProjection::IsDone() const
{
  return myIsDone;
}

//=================================================================================================

const TopoDS_Shape& BRepAlgo_NormalProjection::Projection() const
{
  return myRes;
}

//=================================================================================================

const TopoDS_Shape& BRepAlgo_NormalProjection::Ancestor(const TopoDS_Edge& E) const
{
  return myAncestorMap.Find(E);
}

//=================================================================================================

const TopoDS_Shape& BRepAlgo_NormalProjection::Couple(const TopoDS_Edge& E) const
{
  return myCorresp.Find(E);
}

//=================================================================================================

const NCollection_List<TopoDS_Shape>& BRepAlgo_NormalProjection::Generated(const TopoDS_Shape& S)
{
  return myDescendants.Find(S);
}

//=================================================================================================

bool BRepAlgo_NormalProjection::IsElementary(const Adaptor3d_Curve& C) const
{
  GeomAbs_CurveType type;
  type = C.GetType();
  switch (type)
  {
    case GeomAbs_Line:
    case GeomAbs_Circle:
    case GeomAbs_Ellipse:
    case GeomAbs_Hyperbola:
    case GeomAbs_Parabola:
      return true;
    default:
      return false;
  }
}

//=================================================================================================

bool BRepAlgo_NormalProjection::BuildWire(NCollection_List<TopoDS_Shape>& ListOfWire) const
{
  TopExp_Explorer ExpOfWire, ExpOfShape;
  bool            IsWire = false;
  ExpOfShape.Init(myRes, TopAbs_EDGE);
  if (ExpOfShape.More())
  {
    NCollection_List<TopoDS_Shape> List;

    for (; ExpOfShape.More(); ExpOfShape.Next())
    {
      const TopoDS_Shape& CurE = ExpOfShape.Current();
      List.Append(CurE);
    }
    BRepLib_MakeWire MW;
    MW.Add(List);
    if (MW.IsDone())
    {
      const TopoDS_Shape& Wire = MW.Shape();
      // If the resulting wire contains the same edge as at the beginning OK
      // otherwise the result really consists of several wires.
      TopExp_Explorer exp2(Wire, TopAbs_EDGE);
      int             NbEdges = 0;
      for (; exp2.More(); exp2.Next())
        NbEdges++;
      if (NbEdges == List.Extent())
      {
        ListOfWire.Append(Wire);
        IsWire = true;
      }
    }
  }
  return IsWire;
}
