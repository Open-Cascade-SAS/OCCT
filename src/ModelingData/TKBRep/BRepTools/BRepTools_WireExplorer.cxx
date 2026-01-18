// Created on: 1993-01-21
// Created by: Remi LEQUETTE
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

#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepTools.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Surface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <gp_Pnt2d.hxx>
#include <Precision.hxx>
#include <Standard_NoSuchObject.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_Map.hxx>

//=======================================================================
// forward declarations of aux functions
//=======================================================================
static bool SelectDouble(NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& Doubles,
                         NCollection_List<TopoDS_Shape>&                         L,
                         TopoDS_Edge&                                            E);

static bool SelectDegenerated(NCollection_List<TopoDS_Shape>& L, TopoDS_Edge& E);

static double GetNextParamOnPC(const occ::handle<Geom2d_Curve>& aPC,
                               const gp_Pnt2d&                  aPRef,
                               const double&                    fP,
                               const double&                    lP,
                               const double&                    tolU,
                               const double&                    tolV,
                               const bool&                      reverse);

//=================================================================================================

BRepTools_WireExplorer::BRepTools_WireExplorer()
    : myReverse(false),
      myTolU(0.0),
      myTolV(0.0)
{
}

//=================================================================================================

BRepTools_WireExplorer::BRepTools_WireExplorer(const TopoDS_Wire& W)
{
  TopoDS_Face F = TopoDS_Face();
  Init(W, F);
}

//=================================================================================================

BRepTools_WireExplorer::BRepTools_WireExplorer(const TopoDS_Wire& W, const TopoDS_Face& F)
{
  Init(W, F);
}

//=================================================================================================

void BRepTools_WireExplorer::Init(const TopoDS_Wire& W)
{
  TopoDS_Face F = TopoDS_Face();
  Init(W, F);
}

//=================================================================================================

void BRepTools_WireExplorer::Init(const TopoDS_Wire& W, const TopoDS_Face& F)
{
  myEdge   = TopoDS_Edge();
  myVertex = TopoDS_Vertex();
  myMap.Clear();
  myDoubles.Clear();

  if (W.IsNull())
    return;

  double UMin(0.0), UMax(0.0), VMin(0.0), VMax(0.0);
  if (!F.IsNull())
  {
    // For the faces based on Cone, BSpline and Bezier compute the
    // UV bounds to precise the UV tolerance values
    const GeomAbs_SurfaceType aSurfType = BRepAdaptor_Surface(F, false).GetType();
    if (aSurfType == GeomAbs_Cone || aSurfType == GeomAbs_BSplineSurface
        || aSurfType == GeomAbs_BezierSurface)
    {
      BRepTools::UVBounds(F, UMin, UMax, VMin, VMax);
    }
  }

  Init(W, F, UMin, UMax, VMin, VMax);
}

//=================================================================================================

void BRepTools_WireExplorer::Init(const TopoDS_Wire& W,
                                  const TopoDS_Face& F,
                                  const double       UMin,
                                  const double       UMax,
                                  const double       VMin,
                                  const double       VMax)
{
  myEdge   = TopoDS_Edge();
  myVertex = TopoDS_Vertex();
  myMap.Clear();
  myDoubles.Clear();

  if (W.IsNull())
    return;

  myFace             = F;
  double dfVertToler = 0.;
  myReverse          = false;

  if (!myFace.IsNull())
  {
    TopLoc_Location                  aL;
    const occ::handle<Geom_Surface>& aSurf = BRep_Tool::Surface(myFace, aL);
    GeomAdaptor_Surface              aGAS(aSurf);
    TopExp_Explorer                  anExp(W, TopAbs_VERTEX);
    for (; anExp.More(); anExp.Next())
    {
      const TopoDS_Vertex& aV = TopoDS::Vertex(anExp.Current());
      dfVertToler             = std::max(BRep_Tool::Tolerance(aV), dfVertToler);
    }
    if (dfVertToler < Precision::Confusion())
    {
      // Use tolerance of edges
      for (TopoDS_Iterator it(W); it.More(); it.Next())
        dfVertToler = std::max(BRep_Tool::Tolerance(TopoDS::Edge(it.Value())), dfVertToler);

      if (dfVertToler < Precision::Confusion())
        // empty wire
        return;
    }
    myTolU = 2. * aGAS.UResolution(dfVertToler);
    myTolV = 2. * aGAS.VResolution(dfVertToler);

    // uresolution for cone with infinite vmin vmax is too small.
    if (aGAS.GetType() == GeomAbs_Cone)
    {
      gp_Pnt aP;
      gp_Vec aD1U, aD1V;
      aGAS.D1(UMin, VMin, aP, aD1U, aD1V);
      double tol1, tol2, maxtol = .0005 * (UMax - UMin);
      double a = aD1U.Magnitude();

      if (a <= Precision::Confusion())
        tol1 = maxtol;
      else
        tol1 = std::min(maxtol, dfVertToler / a);

      aGAS.D1(UMin, VMax, aP, aD1U, aD1V);
      a = aD1U.Magnitude();
      if (a <= Precision::Confusion())
        tol2 = maxtol;
      else
        tol2 = std::min(maxtol, dfVertToler / a);

      myTolU = 2. * std::max(tol1, tol2);
    }

    if (aGAS.GetType() == GeomAbs_BSplineSurface || aGAS.GetType() == GeomAbs_BezierSurface)
    {
      double maxTol = std::max(myTolU, myTolV);
      gp_Pnt aP;
      gp_Vec aDU, aDV;
      aGAS.D1((UMax - UMin) / 2., (VMax - VMin) / 2., aP, aDU, aDV);
      double mod = std::sqrt(aDU * aDU + aDV * aDV);
      if (mod > gp::Resolution())
      {
        if (mod * maxTol / dfVertToler < 1.5)
        {
          maxTol = 1.5 * dfVertToler / mod;
        }
        myTolU = maxTol;
        myTolV = maxTol;
      }
    }

    myReverse = (myFace.Orientation() == TopAbs_REVERSED);
  }

  // map of vertices to know if the wire is open
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> vmap;
  //  map of infinite edges
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> anInfEmap;

  // list the vertices
  TopoDS_Vertex                  V1, V2;
  NCollection_List<TopoDS_Shape> empty;

  TopoDS_Iterator it(W);
  while (it.More())
  {
    const TopoDS_Edge& E    = TopoDS::Edge(it.Value());
    TopAbs_Orientation Eori = E.Orientation();
    if (Eori == TopAbs_INTERNAL || Eori == TopAbs_EXTERNAL)
    {
      it.Next();
      continue;
    }
    TopExp::Vertices(E, V1, V2, true);

    if (!V1.IsNull())
    {
      myMap.TryBound(V1, empty)->Append(E);

      // add or remove in the vertex map
      V1.Orientation(TopAbs_FORWARD);
      int currsize = vmap.Extent(), ind = vmap.Add(V1);
      if (currsize >= ind)
      {
        vmap.RemoveKey(V1);
      }
    }

    if (!V2.IsNull())
    {
      V2.Orientation(TopAbs_REVERSED);
      int currsize = vmap.Extent(), ind = vmap.Add(V2);
      if (currsize >= ind)
      {
        vmap.RemoveKey(V2);
      }
    }

    if (V1.IsNull() || V2.IsNull())
    {
      double aF = 0., aL = 0.;
      BRep_Tool::Range(E, aF, aL);

      if (Eori == TopAbs_FORWARD)
      {
        if (aF == -Precision::Infinite())
          anInfEmap.Add(E);
      }
      else
      { // Eori == TopAbs_REVERSED
        if (aL == Precision::Infinite())
          anInfEmap.Add(E);
      }
    }
    it.Next();
  }

  // Construction of the set of double edges.
  TopoDS_Iterator                                        it2(W);
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> emap;
  while (it2.More())
  {
    if (!emap.Add(it2.Value()))
      myDoubles.Add(it2.Value());
    it2.Next();
  }

  // if vmap is not empty the wire is open, let us find the first vertex
  if (!vmap.IsEmpty())
  {
    // NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator itt(vmap);
    // while (itt.Key().Orientation() != TopAbs_FORWARD) {
    //   itt.Next();
    //   if (!itt.More()) break;
    // }
    // if (itt.More()) V1 = TopoDS::Vertex(itt.Key());
    int ind = 0;
    for (ind = 1; ind <= vmap.Extent(); ++ind)
    {
      if (vmap(ind).Orientation() == TopAbs_FORWARD)
      {
        V1 = TopoDS::Vertex(vmap(ind));
        break;
      }
    }
  }
  else
  {
    //   The wire is infinite Try to find the first vertex. It may be NULL.
    if (!anInfEmap.IsEmpty())
    {
      NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator itt(anInfEmap);

      for (; itt.More(); itt.Next())
      {
        TopoDS_Edge        anEdge = TopoDS::Edge(itt.Key());
        TopAbs_Orientation anOri  = anEdge.Orientation();
        double             aF;
        double             aL;

        BRep_Tool::Range(anEdge, aF, aL);
        if ((anOri == TopAbs_FORWARD && aF == -Precision::Infinite())
            || (anOri == TopAbs_REVERSED && aL == Precision::Infinite()))
        {
          myEdge   = anEdge;
          myVertex = TopoDS_Vertex();

          return;
        }
      }
    }

    // use the first vertex in iterator
    it.Initialize(W);
    while (it.More())
    {
      const TopoDS_Edge& E    = TopoDS::Edge(it.Value());
      TopAbs_Orientation Eori = E.Orientation();
      if (Eori == TopAbs_INTERNAL || Eori == TopAbs_EXTERNAL)
      {
        // JYL 10-03-97 : waiting for correct processing
        // of INTERNAL/EXTERNAL edges
        it.Next();
        continue;
      }
      TopExp::Vertices(E, V1, V2, true);
      break;
    }
  }

  if (V1.IsNull())
    return;
  NCollection_List<TopoDS_Shape>* pList = myMap.ChangeSeek(V1);
  if (!pList)
    return;

  NCollection_List<TopoDS_Shape>& l = *pList;
  myEdge                            = TopoDS::Edge(l.First());
  l.RemoveFirst();
  myVertex = TopExp::FirstVertex(myEdge, true);
}

//=================================================================================================

bool BRepTools_WireExplorer::More() const
{
  return !myEdge.IsNull();
}

//=================================================================================================

void BRepTools_WireExplorer::Next()
{
  myVertex = TopExp::LastVertex(myEdge, true);

  if (myVertex.IsNull())
  {
    myEdge = TopoDS_Edge();
    return;
  }
  NCollection_List<TopoDS_Shape>* pList = myMap.ChangeSeek(myVertex);
  if (!pList)
  {
    myEdge = TopoDS_Edge();
    return;
  }

  NCollection_List<TopoDS_Shape>& l = *pList;

  if (l.IsEmpty())
  {
    myEdge = TopoDS_Edge();
  }
  else if (l.Extent() == 1)
  {
    //  Modified by Sergey KHROMOV - Fri Jun 21 10:28:01 2002 OCC325 Begin
    TopoDS_Vertex aV1;
    TopoDS_Vertex aV2;
    TopoDS_Edge   aNextEdge = TopoDS::Edge(l.First());

    TopExp::Vertices(aNextEdge, aV1, aV2, true);

    if (!aV1.IsSame(myVertex))
    {
      myEdge = TopoDS_Edge();
      return;
    }
    if (!myFace.IsNull() && aV1.IsSame(aV2))
    {
      occ::handle<Geom2d_Curve> aPrevPC;
      occ::handle<Geom2d_Curve> aNextPC;
      double                    aPar11, aPar12;
      double                    aPar21, aPar22;
      double                    aPrevPar;
      double                    aNextFPar;
      double                    aNextLPar;

      aPrevPC = BRep_Tool::CurveOnSurface(myEdge, myFace, aPar11, aPar12);
      aNextPC = BRep_Tool::CurveOnSurface(aNextEdge, myFace, aPar21, aPar22);

      if (aPrevPC.IsNull() || aNextPC.IsNull())
      {
        myEdge = TopoDS_Edge();
        return;
      }

      if (myEdge.Orientation() == TopAbs_FORWARD)
        aPrevPar = aPar12;
      else
        aPrevPar = aPar11;

      if (aNextEdge.Orientation() == TopAbs_FORWARD)
      {
        aNextFPar = aPar21;
        aNextLPar = aPar22;
      }
      else
      {
        aNextFPar = aPar22;
        aNextLPar = aPar21;
      }

      gp_Pnt2d aPPrev  = aPrevPC->Value(aPrevPar);
      gp_Pnt2d aPNextF = aNextPC->Value(aNextFPar);
      gp_Pnt2d aPNextL = aNextPC->Value(aNextLPar);

      if (aPPrev.SquareDistance(aPNextF) > aPPrev.SquareDistance(aPNextL))
      {
        myEdge = TopoDS_Edge();
        return;
      }
    }
    //  Modified by Sergey KHROMOV - Fri Jun 21 11:08:16 2002 End
    myEdge = TopoDS::Edge(l.First());
    l.Clear();
  }
  else
  {
    if (myFace.IsNull())
    {
      // Without Face - try to return edges
      // as logically as possible
      // At first degenerated edges.
      TopoDS_Edge E = myEdge;
      if (SelectDegenerated(l, E))
      {
        myEdge = E;
        return;
      }
      // At second double edges.
      E = myEdge;
      if (SelectDouble(myDoubles, l, E))
      {
        myEdge = E;
        return;
      }

      NCollection_List<TopoDS_Shape>::Iterator it(l);
      bool                                     notfound = true;
      while (it.More())
      {
        if (!it.Value().IsSame(myEdge))
        {
          myEdge = TopoDS::Edge(it.Value());
          l.Remove(it);
          notfound = false;
          break;
        }
        it.Next();
      }

      if (notfound)
      {
        myEdge = TopoDS_Edge();
        return;
      }
    }
    else
    {
      // If we have more than one edge attached to the list
      // probably wire that we explore contains a loop or loops.
      double                    dfFPar = 0., dfLPar = 0.;
      occ::handle<Geom2d_Curve> aPCurve = BRep_Tool::CurveOnSurface(myEdge, myFace, dfFPar, dfLPar);
      if (aPCurve.IsNull())
      {
        myEdge = TopoDS_Edge();
        return;
      }
      // Note: current < myVertex > which is last on < myEdge >
      //       equals in 2D to following 2D points:
      //       edge is FORWARD  - point with MAX parameter on PCurve;
      //       edge is REVERSED - point with MIN parameter on PCurve.

      // Get 2D point equals to < myVertex > in 2D for current edge.
      gp_Pnt2d PRef;
      if (myEdge.Orientation() == TopAbs_REVERSED)
        aPCurve->D0(dfFPar, PRef);
      else
        aPCurve->D0(dfLPar, PRef);

      // Get next 2D point from current edge's PCurve with parameter
      // F + dP (REV) or L - dP (FOR)
      bool   isrevese = (myEdge.Orientation() == TopAbs_REVERSED);
      double dfMPar   = GetNextParamOnPC(aPCurve, PRef, dfFPar, dfLPar, myTolU, myTolV, isrevese);

      gp_Pnt2d PRefm;
      aPCurve->D0(dfMPar, PRefm);
      // Get vector from PRef to PRefm
      gp_Vec2d anERefDir(PRef, PRefm);
      if (anERefDir.SquareMagnitude() < gp::Resolution())
      {
        myEdge = TopoDS_Edge();
        return;
      }

      // Search the list of edges looking for the edge having hearest
      // 2D point of connected vertex to current one and smallest angle.
      // First process all degenerated edges, then - all others.

      NCollection_List<TopoDS_Shape>::Iterator it;
      int                                      k = 1, kMin = 0, iDone = 0;
      bool                                     isDegenerated = true;
      double                                   dmin          = RealLast();
      double                                   dfMinAngle = 3.0 * M_PI, dfCurAngle = 3.0 * M_PI;

      for (iDone = 0; iDone < 2; iDone++)
      {
        it.Initialize(l);
        while (it.More())
        {
          const TopoDS_Edge& E = TopoDS::Edge(it.Value());
          if (E.IsSame(myEdge))
          {
            it.Next();
            k++;
            continue;
          }

          TopoDS_Vertex aVert1, aVert2;
          TopExp::Vertices(E, aVert1, aVert2, true);
          if (aVert1.IsNull() || aVert2.IsNull())
          {
            it.Next();
            k++;
            continue;
          }

          aPCurve = BRep_Tool::CurveOnSurface(E, myFace, dfFPar, dfLPar);
          if (aPCurve.IsNull())
          {
            it.Next();
            k++;
            continue;
          }

          gp_Pnt2d aPEb, aPEe;
          if (aVert1.IsSame(aVert2) == isDegenerated)
          {
            if (E.Orientation() == TopAbs_REVERSED)
              aPCurve->D0(dfLPar, aPEb);
            else
              aPCurve->D0(dfFPar, aPEb);

            if (std::abs(dfLPar - dfFPar) > Precision::PConfusion())
            {
              isrevese = (E.Orientation() == TopAbs_REVERSED);
              isrevese = !isrevese;
              double aEPm =
                GetNextParamOnPC(aPCurve, aPEb, dfFPar, dfLPar, myTolU, myTolV, isrevese);

              aPCurve->D0(aEPm, aPEe);
              if (aPEb.SquareDistance(aPEe) <= gp::Resolution())
              {
                // seems to be very short curve
                gp_Vec2d aD;
                aPCurve->D1(aEPm, aPEe, aD);
                if (E.Orientation() == TopAbs_REVERSED)
                  aPEe.SetXY(aPEb.XY() - aD.XY());
                else
                  aPEe.SetXY(aPEb.XY() + aD.XY());

                if (aPEb.SquareDistance(aPEe) <= gp::Resolution())
                {
                  it.Next();
                  k++;
                  continue;
                }
              }
              gp_Vec2d anEDir(aPEb, aPEe);
              dfCurAngle = std::abs(anEDir.Angle(anERefDir));
            }

            if (dfCurAngle <= dfMinAngle)
            {
              double d = PRef.SquareDistance(aPEb);
              if (d <= Precision::PConfusion())
                d = 0.;
              if (std::abs(aPEb.X() - PRef.X()) < myTolU && std::abs(aPEb.Y() - PRef.Y()) < myTolV)
              {
                if (d <= dmin)
                {
                  dfMinAngle = dfCurAngle;
                  kMin       = k;
                  dmin       = d;
                }
              }
            }
          }
          it.Next();
          k++;
        } // while it

        if (kMin == 0)
        {
          isDegenerated = false;
          k             = 1;
          dmin          = RealLast();
        }
        else
          break;
      } // for iDone

      if (kMin == 0)
      {
        // probably unclosed in 2d space wire
        myEdge = TopoDS_Edge();
        return;
      }

      // Selection the edge.
      it.Initialize(l);
      k = 1;
      while (it.More())
      {
        if (k == kMin)
        {
          myEdge = TopoDS::Edge(it.Value());
          l.Remove(it);
          break;
        }
        it.Next();
        k++;
      }
    } // else face != NULL && l > 1
  } // else l > 1
}

//=================================================================================================

const TopoDS_Edge& BRepTools_WireExplorer::Current() const
{
  return myEdge;
}

//=================================================================================================

TopAbs_Orientation BRepTools_WireExplorer::Orientation() const
{
  if (myVertex.IsNull() && !myEdge.IsNull())
  {
    // infinite edge
    return TopAbs_FORWARD;
  }

  TopoDS_Iterator it(myEdge, false);
  while (it.More())
  {
    if (myVertex.IsSame(it.Value()))
      return it.Value().Orientation();
    it.Next();
  }
  throw Standard_NoSuchObject("BRepTools_WireExplorer::Orientation");
}

//=================================================================================================

const TopoDS_Vertex& BRepTools_WireExplorer::CurrentVertex() const
{
  return myVertex;
}

//=================================================================================================

void BRepTools_WireExplorer::Clear()
{
  myMap.Clear();
  myDoubles.Clear();
  myEdge   = TopoDS_Edge();
  myFace   = TopoDS_Face();
  myVertex = TopoDS_Vertex();
}

//=================================================================================================

bool SelectDouble(NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& Doubles,
                  NCollection_List<TopoDS_Shape>&                         L,
                  TopoDS_Edge&                                            E)
{
  NCollection_List<TopoDS_Shape>::Iterator it(L);

  for (; it.More(); it.Next())
  {
    const TopoDS_Shape& CE = it.Value();
    if (Doubles.Contains(CE) && (!E.IsSame(CE)))
    {
      E = TopoDS::Edge(CE);
      L.Remove(it);
      return true;
    }
  }
  return false;
}

//=================================================================================================

bool SelectDegenerated(NCollection_List<TopoDS_Shape>& L, TopoDS_Edge& E)
{
  NCollection_List<TopoDS_Shape>::Iterator it(L);
  while (it.More())
  {
    if (!it.Value().IsSame(E))
    {
      E = TopoDS::Edge(it.Value());
      if (BRep_Tool::Degenerated(E))
      {
        L.Remove(it);
        return true;
      }
    }
    it.Next();
  }
  return false;
}

//=================================================================================================

double GetNextParamOnPC(const occ::handle<Geom2d_Curve>& aPC,
                        const gp_Pnt2d&                  aPRef,
                        const double&                    fP,
                        const double&                    lP,
                        const double&                    tolU,
                        const double&                    tolV,
                        const bool&                      reverse)
{
  double result = (reverse) ? fP : lP;
  double dP     = std::abs(lP - fP) / 1000.; // was / 16.;
  if (reverse)
  {
    double startPar      = fP;
    bool   nextPntOnEdge = false;
    while (!nextPntOnEdge && startPar < lP)
    {
      gp_Pnt2d pnt;
      startPar += dP;
      aPC->D0(startPar, pnt);
      if (std::abs(aPRef.X() - pnt.X()) < tolU && std::abs(aPRef.Y() - pnt.Y()) < tolV)
        continue;
      else
      {
        result        = startPar;
        nextPntOnEdge = true;
        break;
      }
    }

    if (!nextPntOnEdge)
      result = lP;

    if (result > lP)
      result = lP;
  }
  else
  {
    double startPar      = lP;
    bool   nextPntOnEdge = false;
    while (!nextPntOnEdge && startPar > fP)
    {
      gp_Pnt2d pnt;
      startPar -= dP;
      aPC->D0(startPar, pnt);
      if (std::abs(aPRef.X() - pnt.X()) < tolU && std::abs(aPRef.Y() - pnt.Y()) < tolV)
        continue;
      else
      {
        result        = startPar;
        nextPntOnEdge = true;
        break;
      }
    }

    if (!nextPntOnEdge)
      result = fP;

    if (result < fP)
      result = fP;
  }

  return result;
}
