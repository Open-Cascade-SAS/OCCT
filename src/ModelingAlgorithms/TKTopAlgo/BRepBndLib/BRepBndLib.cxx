// Copyright (c) 1995-1999 Matra Datavision
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

#include <Bnd_Box.hxx>
#include <BndLib_Add3dCurve.hxx>
#include <BndLib_AddSurface.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepBndLib.hxx>
#include <Geom_Surface.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangulation.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <Adaptor3d_Curve.hxx>
#include <Adaptor3d_Surface.hxx>
#include <BRepTools.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_BezierSurface.hxx>
#include <Bnd_Box2d.hxx>
#include <BndLib_Add2dCurve.hxx>
#include <BRepTopAdaptor_FClass2d.hxx>
#include <ElSLib.hxx>
#include <ElCLib.hxx>
#include <Geom_Plane.hxx>
#include <Extrema_ExtSS.hxx>
#include <GeomAdaptor_Surface.hxx>
//
static bool CanUseEdges(const Adaptor3d_Surface& BS);
//
static void FindExactUVBounds(const TopoDS_Face&  F,
                              double&      umin,
                              double&      umax,
                              double&      vmin,
                              double&      vmax,
                              const double Tol,
                              bool&   isNaturalRestriction);
//
static void AdjustFaceBox(const BRepAdaptor_Surface& BS,
                          const double        umin,
                          const double        umax,
                          const double        vmin,
                          const double        vmax,
                          Bnd_Box&                   FaceBox,
                          const Bnd_Box&             EdgeBox,
                          const double        Tol);
//
static bool IsModifySize(const BRepAdaptor_Surface&     theBS,
                                     const gp_Pln&                  thePln,
                                     const gp_Pnt&                  theP,
                                     const double            umin,
                                     const double            umax,
                                     const double            vmin,
                                     const double            vmax,
                                     const BRepTopAdaptor_FClass2d& theFClass,
                                     const double            theTolU,
                                     const double            theTolV);

//
//=======================================================================
// function : Add
// purpose  : Add a shape bounding to a box
//=======================================================================
void BRepBndLib::Add(const TopoDS_Shape& S, Bnd_Box& B, bool useTriangulation)
{
  TopExp_Explorer ex;

  // Add the faces
  BRepAdaptor_Surface BS;
  TopLoc_Location     l, aDummyLoc;
  int    i, nbNodes;
  BRepAdaptor_Curve   BC;

  for (ex.Init(S, TopAbs_FACE); ex.More(); ex.Next())
  {
    const TopoDS_Face&                F  = TopoDS::Face(ex.Current());
    const occ::handle<Poly_Triangulation>& T  = BRep_Tool::Triangulation(F, l);
    const occ::handle<Geom_Surface>&       GS = BRep_Tool::Surface(F, aDummyLoc);
    if ((useTriangulation || GS.IsNull()) && !T.IsNull() && T->MinMax(B, l))
    {
      //       B.Enlarge(T->Deflection());
      B.Enlarge(T->Deflection() + BRep_Tool::Tolerance(F));
    }
    else
    {
      if (!GS.IsNull())
      {
        BS.Initialize(F, false);
        if (BS.GetType() != GeomAbs_Plane)
        {
          BS.Initialize(F);
          BndLib_AddSurface::Add(BS, BRep_Tool::Tolerance(F), B);
        }
        else
        {
          // on travaille directement sur les courbes 3d.
          TopExp_Explorer ex2(F, TopAbs_EDGE);
          if (!ex2.More())
          {
            BS.Initialize(F);
            BndLib_AddSurface::Add(BS, BRep_Tool::Tolerance(F), B);
          }
          else
          {
            for (; ex2.More(); ex2.Next())
            {
              const TopoDS_Edge& anEdge = TopoDS::Edge(ex2.Current());
              if (BRep_Tool::IsGeometric(anEdge))
              {
                BC.Initialize(anEdge);
                BndLib_Add3dCurve::Add(BC, BRep_Tool::Tolerance(anEdge), B);
              }
            }
            B.Enlarge(BRep_Tool::Tolerance(F));
          }
        }
      }
    }
  }

  // Add the edges not in faces
  occ::handle<NCollection_HArray1<int>>    HIndices;
  occ::handle<Poly_PolygonOnTriangulation> Poly;
  occ::handle<Poly_Triangulation>          T;
  for (ex.Init(S, TopAbs_EDGE, TopAbs_FACE); ex.More(); ex.Next())
  {
    const TopoDS_Edge& E = TopoDS::Edge(ex.Current());

    if (!useTriangulation && BRep_Tool::IsGeometric(E))
    {
      BC.Initialize(E);
      BndLib_Add3dCurve::Add(BC, BRep_Tool::Tolerance(E), B);
      continue;
    }

    occ::handle<Poly_Polygon3D> P3d = BRep_Tool::Polygon3D(E, l);
    if (!P3d.IsNull() && P3d->NbNodes() > 0)
    {
      const NCollection_Array1<gp_Pnt>& Nodes = P3d->Nodes();
      nbNodes                         = P3d->NbNodes();
      for (i = 1; i <= nbNodes; i++)
      {
        if (l.IsIdentity())
          B.Add(Nodes[i]);
        else
          B.Add(Nodes[i].Transformed(l));
      }
      //       B.Enlarge(P3d->Deflection());
      B.Enlarge(P3d->Deflection() + BRep_Tool::Tolerance(E));
    }
    else
    {
      BRep_Tool::PolygonOnTriangulation(E, Poly, T, l);
      if (!Poly.IsNull() && !T.IsNull() && T->NbNodes() > 0)
      {
        const NCollection_Array1<int>& Indices = Poly->Nodes();
        nbNodes                                = Indices.Length();
        if (l.IsIdentity())
        {
          for (i = 1; i <= nbNodes; i++)
          {
            B.Add(T->Node(Indices[i]));
          }
        }
        else
        {
          for (i = 1; i <= nbNodes; i++)
          {
            B.Add(T->Node(Indices[i]).Transformed(l));
          }
        }
        // 	B.Enlarge(T->Deflection());
        B.Enlarge(Poly->Deflection() + BRep_Tool::Tolerance(E));
      }
      else
      {
        if (BRep_Tool::IsGeometric(E))
        {
          BC.Initialize(E);
          BndLib_Add3dCurve::Add(BC, BRep_Tool::Tolerance(E), B);
        }
      }
    }
  }

  // Add the vertices not in edges

  for (ex.Init(S, TopAbs_VERTEX, TopAbs_EDGE); ex.More(); ex.Next())
  {
    B.Add(BRep_Tool::Pnt(TopoDS::Vertex(ex.Current())));
    B.Enlarge(BRep_Tool::Tolerance(TopoDS::Vertex(ex.Current())));
  }
}

//=======================================================================
// function : AddClose
// purpose  : Add a precise shape bounding to a box
//=======================================================================

void BRepBndLib::AddClose(const TopoDS_Shape& S, Bnd_Box& B)
{
  TopExp_Explorer ex;

  // No faces

  // Add the edges

  BRepAdaptor_Curve BC;

  for (ex.Init(S, TopAbs_EDGE); ex.More(); ex.Next())
  {
    const TopoDS_Edge& anEdge = TopoDS::Edge(ex.Current());
    if (BRep_Tool::IsGeometric(anEdge))
    {
      BC.Initialize(anEdge);
      BndLib_Add3dCurve::Add(BC, 0., B);
    }
  }

  // Add the vertices not in edges

  for (ex.Init(S, TopAbs_VERTEX, TopAbs_EDGE); ex.More(); ex.Next())
  {
    B.Add(BRep_Tool::Pnt(TopoDS::Vertex(ex.Current())));
  }
}

//=======================================================================
// function : AddOptimal
// purpose  : Add a shape bounding to a box
//=======================================================================
void BRepBndLib::AddOptimal(const TopoDS_Shape&    S,
                            Bnd_Box&               B,
                            const bool useTriangulation,
                            const bool useShapeTolerance)
{
  TopExp_Explorer ex;

  // Add the faces
  BRepAdaptor_Surface        BS;
  occ::handle<Poly_Triangulation> T;
  TopLoc_Location            l;
  int           i, nbNodes;
  BRepAdaptor_Curve          BC;

  for (ex.Init(S, TopAbs_FACE); ex.More(); ex.Next())
  {
    const TopoDS_Face& F = TopoDS::Face(ex.Current());
    T                    = BRep_Tool::Triangulation(F, l);
    Bnd_Box aLocBox;
    if (useTriangulation && !T.IsNull() && T->MinMax(aLocBox, l))
    {
      //       B.Enlarge(T->Deflection());
      aLocBox.Enlarge(T->Deflection() + BRep_Tool::Tolerance(F));
      double xmin, ymin, zmin, xmax, ymax, zmax;
      aLocBox.Get(xmin, ymin, zmin, xmax, ymax, zmax);
      B.Update(xmin, ymin, zmin, xmax, ymax, zmax);
    }
    else
    {
      const occ::handle<Geom_Surface>& GS = BRep_Tool::Surface(F, l);
      if (!GS.IsNull())
      {
        BS.Initialize(F, false);
        if (CanUseEdges(BS))
        {
          TopExp_Explorer ex2(F, TopAbs_EDGE);
          if (!ex2.More())
          {
            BS.Initialize(F);
            double Tol = useShapeTolerance ? BRep_Tool::Tolerance(F) : 0.;
            BndLib_AddSurface::AddOptimal(BS, Tol, aLocBox);
          }
          else
          {
            double Tol;
            for (; ex2.More(); ex2.Next())
            {
              Bnd_Box            anEBox;
              const TopoDS_Edge& anE = TopoDS::Edge(ex2.Current());
              if (BRep_Tool::Degenerated(anE) || !BRep_Tool::IsGeometric(anE))
              {
                continue;
              }
              BC.Initialize(anE);
              Tol = useShapeTolerance ? BRep_Tool::Tolerance(anE) : 0.;
              BndLib_Add3dCurve::AddOptimal(BC, Tol, anEBox);
              aLocBox.Add(anEBox);
            }
          }
        }
        else
        {
          double    umin, umax, vmin, vmax;
          bool isNaturalRestriction = false;
          double    Tol                  = useShapeTolerance ? BRep_Tool::Tolerance(F) : 0.;
          FindExactUVBounds(F, umin, umax, vmin, vmax, Tol, isNaturalRestriction);
          BndLib_AddSurface::AddOptimal(BS, umin, umax, vmin, vmax, Tol, aLocBox);
          //
          if (!isNaturalRestriction)
          {
            TopExp_Explorer ex2(F, TopAbs_EDGE);
            Bnd_Box         EBox;
            for (; ex2.More(); ex2.Next())
            {
              Bnd_Box            anEBox;
              const TopoDS_Edge& anE = TopoDS::Edge(ex2.Current());
              if (BRep_Tool::Degenerated(anE) || !BRep_Tool::IsGeometric(anE))
              {
                continue;
              }
              BC.Initialize(anE);
              Tol = useShapeTolerance ? BRep_Tool::Tolerance(anE) : 0.;
              BndLib_Add3dCurve::AddOptimal(BC, Tol, anEBox);
              EBox.Add(anEBox);
            }
            Tol = useShapeTolerance ? BRep_Tool::Tolerance(F) : 0.;
            AdjustFaceBox(BS, umin, umax, vmin, vmax, aLocBox, EBox, Tol);
          }
        }

        if (!aLocBox.IsVoid())
        {
          double xmin, ymin, zmin, xmax, ymax, zmax;
          aLocBox.Get(xmin, ymin, zmin, xmax, ymax, zmax);
          B.Update(xmin, ymin, zmin, xmax, ymax, zmax);
        }
      }
    }
  }

  // Add the edges not in faces
  occ::handle<NCollection_HArray1<int>>    HIndices;
  occ::handle<Poly_PolygonOnTriangulation> Poly;

  for (ex.Init(S, TopAbs_EDGE, TopAbs_FACE); ex.More(); ex.Next())
  {
    const TopoDS_Edge&     E = TopoDS::Edge(ex.Current());
    Bnd_Box                aLocBox;
    occ::handle<Poly_Polygon3D> P3d = BRep_Tool::Polygon3D(E, l);
    if (useTriangulation && !P3d.IsNull() && P3d->NbNodes() > 0)
    {
      const NCollection_Array1<gp_Pnt>& Nodes = P3d->Nodes();
      nbNodes                         = P3d->NbNodes();
      for (i = 1; i <= nbNodes; i++)
      {
        if (l.IsIdentity())
          aLocBox.Add(Nodes[i]);
        else
          aLocBox.Add(Nodes[i].Transformed(l));
      }
      double Tol = useShapeTolerance ? BRep_Tool::Tolerance(E) : 0.;
      aLocBox.Enlarge(P3d->Deflection() + Tol);
    }
    else
    {
      BRep_Tool::PolygonOnTriangulation(E, Poly, T, l);
      if (useTriangulation && !Poly.IsNull() && !T.IsNull() && T->NbNodes() > 0)
      {
        const NCollection_Array1<int>& Indices = Poly->Nodes();
        nbNodes                                = Indices.Length();
        for (i = 1; i <= nbNodes; i++)
        {
          if (l.IsIdentity())
          {
            aLocBox.Add(T->Node(Indices[i]));
          }
          else
          {
            aLocBox.Add(T->Node(Indices[i]).Transformed(l));
          }
        }
        double Tol = useShapeTolerance ? BRep_Tool::Tolerance(E) : 0.;
        aLocBox.Enlarge(Poly->Deflection() + Tol);
      }
      else
      {
        if (BRep_Tool::IsGeometric(E))
        {
          BC.Initialize(E);
          double Tol = useShapeTolerance ? BRep_Tool::Tolerance(E) : 0.;
          BndLib_Add3dCurve::AddOptimal(BC, Tol, aLocBox);
        }
      }
    }
    if (!aLocBox.IsVoid())
    {
      double xmin, ymin, zmin, xmax, ymax, zmax;
      aLocBox.Get(xmin, ymin, zmin, xmax, ymax, zmax);
      B.Update(xmin, ymin, zmin, xmax, ymax, zmax);
    }
  }

  // Add the vertices not in edges

  for (ex.Init(S, TopAbs_VERTEX, TopAbs_EDGE); ex.More(); ex.Next())
  {
    Bnd_Box              aLocBox;
    const TopoDS_Vertex& aV = TopoDS::Vertex(ex.Current());
    aLocBox.Add(BRep_Tool::Pnt(aV));
    double Tol = useShapeTolerance ? BRep_Tool::Tolerance(aV) : 0.;
    aLocBox.Enlarge(Tol);
    double xmin, ymin, zmin, xmax, ymax, zmax;
    aLocBox.Get(xmin, ymin, zmin, xmax, ymax, zmax);
    B.Update(xmin, ymin, zmin, xmax, ymax, zmax);
  }
}

//=======================================================================
// function : CanUseEdges
// purpose  : Define is it possible using only edges bnd boxes
//           to get face bnd box
//=======================================================================
bool CanUseEdges(const Adaptor3d_Surface& BS)
{
  GeomAbs_SurfaceType aST = BS.GetType();
  if (aST == GeomAbs_Plane || aST == GeomAbs_Cylinder || aST == GeomAbs_Cone
      || aST == GeomAbs_SurfaceOfExtrusion)
  {
    return true;
  }
  else if (aST == GeomAbs_SurfaceOfRevolution)
  {
    const occ::handle<Adaptor3d_Curve>& aBC = BS.BasisCurve();
    if (aBC->GetType() == GeomAbs_Line)
    {
      return true;
    }
    else
    {
      return false;
    }
  }
  else if (aST == GeomAbs_OffsetSurface)
  {
    const occ::handle<Adaptor3d_Surface>& aS = BS.BasisSurface();
    return CanUseEdges(*aS);
  }
  else if (aST == GeomAbs_BSplineSurface)
  {
    occ::handle<Geom_BSplineSurface> aBSpl = BS.BSpline();
    if ((aBSpl->UDegree() == 1 && aBSpl->NbUKnots() == 2)
        || (aBSpl->VDegree() == 1 && aBSpl->NbVKnots() == 2))
    {
      return true;
    }
    else
    {
      return false;
    }
  }
  else if (aST == GeomAbs_BezierSurface)
  {
    occ::handle<Geom_BezierSurface> aBz = BS.Bezier();
    if ((aBz->UDegree() == 1) || (aBz->VDegree() == 1))
    {
      return true;
    }
    else
    {
      return false;
    }
  }
  return false;
}

//=================================================================================================

void FindExactUVBounds(const TopoDS_Face&  FF,
                       double&      umin,
                       double&      umax,
                       double&      vmin,
                       double&      vmax,
                       const double Tol,
                       bool&   isNaturalRestriction)
{
  TopoDS_Face F = FF;
  F.Orientation(TopAbs_FORWARD);
  TopExp_Explorer ex(F, TopAbs_EDGE);
  //
  // Check Natural restriction
  isNaturalRestriction = BRep_Tool::NaturalRestriction(F); // Can we trust this flag?
  BRepAdaptor_Surface aBAS(F, false);
  if (!isNaturalRestriction)
  {
    // Check by comparing pcurves and surface boundaries
    umin                         = aBAS.FirstUParameter();
    umax                         = aBAS.LastUParameter();
    vmin                         = aBAS.FirstVParameter();
    vmax                         = aBAS.LastVParameter();
    bool isUperiodic = aBAS.IsUPeriodic(), isVperiodic = aBAS.IsVPeriodic();
    double    aT1, aT2;
    double    TolU = std::max(aBAS.UResolution(Tol), Precision::PConfusion());
    double    TolV = std::max(aBAS.VResolution(Tol), Precision::PConfusion());
    int Nu = 0, Nv = 0, NbEdges = 0;
    gp_Vec2d         Du(1, 0), Dv(0, 1);
    gp_Pnt2d         aP;
    gp_Vec2d         aV;
    for (; ex.More(); ex.Next())
    {
      NbEdges++;
      if (NbEdges > 4)
      {
        break;
      }
      const TopoDS_Edge&         aE   = TopoDS::Edge(ex.Current());
      const occ::handle<Geom2d_Curve> aC2D = BRep_Tool::CurveOnSurface(aE, F, aT1, aT2);
      if (aC2D.IsNull())
      {
        break;
      }
      //
      aC2D->D1((aT1 + aT2) / 2., aP, aV);
      double magn = aV.SquareMagnitude();
      if (magn < gp::Resolution())
      {
        break;
      }
      else
      {
        aV /= std::sqrt(magn);
      }
      double u = aP.X(), v = aP.Y();
      if (isUperiodic)
      {
        ElCLib::InPeriod(u, umin, umax);
      }
      if (isVperiodic)
      {
        ElCLib::InPeriod(v, vmin, vmax);
      }
      //
      if (std::abs(u - umin) <= TolU || std::abs(u - umax) <= TolU)
      {
        double d = Dv * aV;
        if (1. - std::abs(d) <= Precision::PConfusion())
        {
          Nu++;
          if (Nu > 2)
          {
            break;
          }
        }
        else
        {
          break;
        }
      }
      else if (std::abs(v - vmin) <= TolV || std::abs(v - vmax) <= TolV)
      {
        double d = Du * aV;
        if (1. - std::abs(d) <= Precision::PConfusion())
        {
          Nv++;
          if (Nv > 2)
          {
            break;
          }
        }
        else
        {
          break;
        }
      }
      else
      {
        break;
      }
    }
    if (Nu == 2 && Nv == 2)
    {
      isNaturalRestriction = true;
    }
  }
  //
  if (isNaturalRestriction)
  {
    umin = aBAS.FirstUParameter();
    umax = aBAS.LastUParameter();
    vmin = aBAS.FirstVParameter();
    vmax = aBAS.LastVParameter();
    return;
  }

  // fill box for the given face
  double aT1, aT2;
  double TolU  = std::max(aBAS.UResolution(Tol), Precision::PConfusion());
  double TolV  = std::max(aBAS.VResolution(Tol), Precision::PConfusion());
  double TolUV = std::max(TolU, TolV);
  Bnd_Box2d     aBox;
  ex.Init(F, TopAbs_EDGE);
  for (; ex.More(); ex.Next())
  {
    const TopoDS_Edge&         aE   = TopoDS::Edge(ex.Current());
    const occ::handle<Geom2d_Curve> aC2D = BRep_Tool::CurveOnSurface(aE, F, aT1, aT2);
    if (aC2D.IsNull())
    {
      continue;
    }
    //
    BndLib_Add2dCurve::AddOptimal(aC2D, aT1, aT2, TolUV, aBox);
    //
  }

  // In some cases no edges are found
  if (!aBox.IsVoid())
  {
    aBox.Get(umin, vmin, umax, vmax);
  }

  //
  TopLoc_Location      aLoc;
  occ::handle<Geom_Surface> aS = BRep_Tool::Surface(FF, aLoc);
  double        aUmin, aUmax, aVmin, aVmax;
  aS->Bounds(aUmin, aUmax, aVmin, aVmax);
  if (!aS->IsUPeriodic())
  {
    umin = std::max(aUmin, umin);
    umax = std::min(aUmax, umax);
  }
  else
  {
    if (umax - umin > aS->UPeriod())
    {
      double delta = umax - umin - aS->UPeriod();
      umin += delta / 2.;
      umax -= delta / 2;
    }
  }
  //
  if (!aS->IsVPeriodic())
  {
    vmin = std::max(aVmin, vmin);
    vmax = std::min(aVmax, vmax);
  }
  else
  {
    if (vmax - vmin > aS->VPeriod())
    {
      double delta = vmax - vmin - aS->VPeriod();
      vmin += delta / 2.;
      vmax -= delta / 2;
    }
  }
}

//=================================================================================================

inline void Reorder(double& a, double& b)
{
  if (a > b)
  {
    double t = a;
    a               = b;
    b               = t;
  }
}

//=================================================================================================

bool IsModifySize(const BRepAdaptor_Surface&     theBS,
                              const gp_Pln&                  thePln,
                              const gp_Pnt&                  theP,
                              const double            umin,
                              const double            umax,
                              const double            vmin,
                              const double            vmax,
                              const BRepTopAdaptor_FClass2d& theFClass,
                              const double            theTolU,
                              const double            theTolV)
{
  double pu1 = 0, pu2, pv1 = 0, pv2;
  ElSLib::PlaneParameters(thePln.Position(), theP, pu2, pv2);
  Reorder(pu1, pu2);
  Reorder(pv1, pv2);
  occ::handle<Geom_Plane>  aPlane = new Geom_Plane(thePln);
  GeomAdaptor_Surface aGAPln(aPlane, pu1, pu2, pv1, pv2);
  Extrema_ExtSS anExtr(aGAPln, theBS, pu1, pu2, pv1, pv2, umin, umax, vmin, vmax, theTolU, theTolV);
  if (anExtr.IsDone())
  {
    if (anExtr.NbExt() > 0)
    {
      int i, imin = 0;
      double    dmin  = RealLast();
      double    uextr = 0., vextr = 0.;
      Extrema_POnSurf  P1, P2;
      for (i = 1; i <= anExtr.NbExt(); ++i)
      {
        double d = anExtr.SquareDistance(i);
        if (d < dmin)
        {
          imin = i;
          dmin = d;
        }
      }
      if (imin > 0)
      {
        anExtr.Points(imin, P1, P2);
        P2.Parameter(uextr, vextr);
      }
      else
      {
        return false;
      }
      //
      gp_Pnt2d     aP2d(uextr, vextr);
      TopAbs_State aSt = theFClass.Perform(aP2d);
      if (aSt != TopAbs_IN)
      {
        return true;
      }
    }
    else
    {
      return true; // extrema point seems to be out of face UV bounds
    }
  }
  //
  return false;
}

//
//=================================================================================================

void AdjustFaceBox(const BRepAdaptor_Surface& BS,
                   const double        umin,
                   const double        umax,
                   const double        vmin,
                   const double        vmax,
                   Bnd_Box&                   FaceBox,
                   const Bnd_Box&             EdgeBox,
                   const double        Tol)
{
  if (EdgeBox.IsVoid())
  {
    return;
  }
  if (FaceBox.IsVoid())
  {
    FaceBox = EdgeBox;
    return;
  }

  double fxmin, fymin, fzmin, fxmax, fymax, fzmax;
  double exmin, eymin, ezmin, exmax, eymax, ezmax;
  //
  FaceBox.Get(fxmin, fymin, fzmin, fxmax, fymax, fzmax);
  EdgeBox.Get(exmin, eymin, ezmin, exmax, eymax, ezmax);
  //
  double           TolU = std::max(BS.UResolution(Tol), Precision::PConfusion());
  double           TolV = std::max(BS.VResolution(Tol), Precision::PConfusion());
  BRepTopAdaptor_FClass2d FClass(BS.Face(), std::max(TolU, TolV));
  //
  bool isModified = false;
  if (exmin > fxmin)
  {
    //
    gp_Pln pl(gp_Ax3(gp_Pnt(fxmin, fymin, fzmin), gp::DX()));
    gp_Pnt aP(fxmin, fymax, fzmax);
    if (IsModifySize(BS, pl, aP, umin, umax, vmin, vmax, FClass, TolU, TolV))
    {
      fxmin      = exmin;
      isModified = true;
    }
  }
  if (exmax < fxmax)
  {
    //
    gp_Pln pl(gp_Ax3(gp_Pnt(fxmax, fymax, fzmax), gp::DX()));
    gp_Pnt aP(fxmax, fymin, fzmin);
    if (IsModifySize(BS, pl, aP, umin, umax, vmin, vmax, FClass, TolU, TolV))
    {
      fxmax      = exmax;
      isModified = true;
    }
  }
  //
  if (eymin > fymin)
  {
    //
    gp_Pln pl(gp_Ax3(gp_Pnt(fxmin, fymin, fzmin), gp::DY()));
    gp_Pnt aP(fxmax, fymin, fzmax);
    if (IsModifySize(BS, pl, aP, umin, umax, vmin, vmax, FClass, TolU, TolV))
    {
      fymin      = eymin;
      isModified = true;
    }
  }
  if (eymax < fymax)
  {
    //
    gp_Pln pl(gp_Ax3(gp_Pnt(fxmax, fymax, fzmax), gp::DY()));
    gp_Pnt aP(fxmin, fymax, fzmin);
    if (IsModifySize(BS, pl, aP, umin, umax, vmin, vmax, FClass, TolU, TolV))
    {
      fymax      = eymax;
      isModified = true;
    }
  }
  //
  if (ezmin > fzmin)
  {
    //
    gp_Pln pl(gp_Ax3(gp_Pnt(fxmin, fymin, fzmin), gp::DZ()));
    gp_Pnt aP(fxmax, fymax, fzmin);
    if (IsModifySize(BS, pl, aP, umin, umax, vmin, vmax, FClass, TolU, TolV))
    {
      fzmin      = ezmin;
      isModified = true;
    }
  }
  if (ezmax < fzmax)
  {
    //
    gp_Pln pl(gp_Ax3(gp_Pnt(fxmax, fymax, fzmax), gp::DZ()));
    gp_Pnt aP(fxmin, fymin, fzmax);
    if (IsModifySize(BS, pl, aP, umin, umax, vmin, vmax, FClass, TolU, TolV))
    {
      fzmax      = ezmax;
      isModified = true;
    }
  }
  //
  if (isModified)
  {
    FaceBox.SetVoid();
    FaceBox.Update(fxmin, fymin, fzmin, fxmax, fymax, fzmax);
  }
}
