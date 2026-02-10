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
#include <BRepLib.hxx>
#include <BRepTools.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_ElementarySurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomLib.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <ShapeAnalysis_CheckSmallFace.hxx>
#include <ShapeAnalysis_Curve.hxx>
#include <ShapeAnalysis_Wire.hxx>
#include <ShapeAnalysis_WireOrder.hxx>
#include <ShapeExtend.hxx>
#include <ShapeExtend_WireData.hxx>
#include <Standard_ErrorHandler.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_List.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Builder.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>

// #include <GeomLProp_SLProps.hxx>
// #include <ShapeFix_Wire.hxx>
//=================================================================================================

ShapeAnalysis_CheckSmallFace::ShapeAnalysis_CheckSmallFace()
{
  myStatusSpot      = ShapeExtend::EncodeStatus(ShapeExtend_OK);
  myStatusStrip     = ShapeExtend::EncodeStatus(ShapeExtend_OK);
  myStatusPin       = ShapeExtend::EncodeStatus(ShapeExtend_OK);
  myStatusTwisted   = ShapeExtend::EncodeStatus(ShapeExtend_OK);
  myStatusSplitVert = ShapeExtend::EncodeStatus(ShapeExtend_OK);
}

static void MinMaxPnt(const gp_Pnt& p,
                      int&          nb,
                      double&       minx,
                      double&       miny,
                      double&       minz,
                      double&       maxx,
                      double&       maxy,
                      double&       maxz)
{
  double x, y, z;
  p.Coord(x, y, z);
  if (nb < 1)
  {
    minx = maxx = x;
    miny = maxy = y;
    minz = maxz = z;
  }
  else
  {
    if (minx > x)
      minx = x;
    if (maxx < x)
      maxx = x;
    if (miny > y)
      miny = y;
    if (maxy < y)
      maxy = y;
    if (minz > z)
      minz = z;
    if (maxz < z)
      maxz = z;
  }
  nb++;
}

static bool MinMaxSmall(const double minx,
                        const double miny,
                        const double minz,
                        const double maxx,
                        const double maxy,
                        const double maxz,
                        const double toler)
{
  double dx = maxx - minx;
  double dy = maxy - miny;
  double dz = maxz - minz;

  return (dx <= toler || Precision::IsInfinite(dx)) && (dy <= toler || Precision::IsInfinite(dy))
         && (dz <= toler || Precision::IsInfinite(dz));
}

//=================================================================================================

int ShapeAnalysis_CheckSmallFace::IsSpotFace(const TopoDS_Face& F,
                                             gp_Pnt&            spot,
                                             double&            spotol,
                                             const double       tol) const
{

  double toler = tol;
  double tolv  = tol;
  //  Compute tolerance to get : from greatest tol of vertices
  //  In addition, also computes min-max of vertices
  //  To finally compare mini-max box with tolerance
  // gka Mar2000 Protection against faces without wires
  // but they occur due to bugs in the algorithm itself, it needs to be fixed
  bool isWir = false;
  for (TopoDS_Iterator itw(F, false); itw.More(); itw.Next())
  {
    if (itw.Value().ShapeType() != TopAbs_WIRE)
      continue;
    TopoDS_Wire w1 = TopoDS::Wire(itw.Value());
    if (!w1.IsNull())
    {
      isWir = true;
      break;
    }
  }
  if (!isWir)
    return true;
  int    nbv  = 0;
  double minx = 0, miny = 0, minz = 0, maxx = Precision::Infinite(), maxy = Precision::Infinite(),
         maxz = Precision::Infinite();
  TopoDS_Vertex V0;
  bool          same = true;
  for (TopExp_Explorer iv(F, TopAbs_VERTEX); iv.More(); iv.Next())
  {
    TopoDS_Vertex V = TopoDS::Vertex(iv.Current());
    if (V0.IsNull())
      V0 = V;
    else if (same)
    {
      if (!V0.IsSame(V))
        same = false;
    }

    gp_Pnt pnt = BRep_Tool::Pnt(V);
    // double x,y,z;
    MinMaxPnt(pnt, nbv, minx, miny, minz, maxx, maxy, maxz);

    if (tol < 0)
    {
      tolv = BRep_Tool::Tolerance(V);
      if (tolv > toler)
        toler = tolv;
    }
  }

  //   Now, testing
  if (!MinMaxSmall(minx, miny, minz, maxx, maxy, maxz, toler))
    return 0;

  //   All vertices are confused
  //   Check edges (a closed edge may be a non-null length edge !)
  //   By picking intermediate point on each one
  for (TopExp_Explorer ie(F, TopAbs_EDGE); ie.More(); ie.Next())
  {
    TopoDS_Edge             E = TopoDS::Edge(ie.Current());
    double                  cf, cl;
    occ::handle<Geom_Curve> C3D = BRep_Tool::Curve(E, cf, cl);
    if (C3D.IsNull())
      continue;
    gp_Pnt debut  = C3D->Value(cf);
    gp_Pnt milieu = C3D->Value((cf + cl) / 2);
    if (debut.SquareDistance(milieu) > toler * toler)
      return 0;
  }

  spot.SetCoord((minx + maxx) / 2., (miny + maxy) / 2., (minz + maxz) / 2.);
  spotol = maxx - minx;
  spotol = std::max(spotol, maxy - miny);
  spotol = std::max(spotol, maxz - minz);
  spotol = spotol / 2.;

  return (same ? 2 : 1);
}

//=================================================================================================

bool ShapeAnalysis_CheckSmallFace::CheckSpotFace(const TopoDS_Face& F, const double tol)
{
  gp_Pnt spot;
  double spotol;
  int    stat = IsSpotFace(F, spot, spotol, tol);
  if (!stat)
    return false;
  switch (stat)
  {
    case 1:
      myStatusSpot = ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
      break;
    case 2:
      myStatusSpot = ShapeExtend::EncodeStatus(ShapeExtend_DONE2);
      break;
    default:
      break;
  }
  return true;
}

//=================================================================================================

bool ShapeAnalysis_CheckSmallFace::IsStripSupport(const TopoDS_Face& F, const double tol)
{

  double toler = tol;
  if (toler < 0)
    toler = 1.e-07; // ?? better to compute tolerance zones

  TopLoc_Location           loc;
  occ::handle<Geom_Surface> surf = BRep_Tool::Surface(F, loc);
  if (surf.IsNull())
    return false;

  //  Checking on poles for bezier-bspline
  //  A more general way is to check Values by scanning ISOS (slower)

  occ::handle<Geom_BSplineSurface> bs = occ::down_cast<Geom_BSplineSurface>(surf);
  occ::handle<Geom_BezierSurface>  bz = occ::down_cast<Geom_BezierSurface>(surf);

  // int stat = 2;  // 2 : small in V direction
  if (!bs.IsNull() || !bz.IsNull())
  {
    bool cbz = (!bz.IsNull());
    int  iu, iv, nbu, nbv;
    if (cbz)
    {
      nbu = bz->NbUPoles(), nbv = bz->NbVPoles();
    }
    else
    {
      nbu = bs->NbUPoles(), nbv = bs->NbVPoles();
    }
    // double dx = 0, dy = 0, dz = 0;
    // double    x,y,z;
    double minx = 0., miny = 0., minz = 0., maxx = 0., maxy = 0., maxz = 0.;
    bool   issmall = true;

    for (iu = 1; iu <= nbu; iu++)
    {
      //    for each U line, scan poles in V (V direction)
      int nb = 0;
      for (iv = 1; iv <= nbv; iv++)
      {
        gp_Pnt unp = (cbz ? bz->Pole(iu, iv) : bs->Pole(iu, iv));
        MinMaxPnt(unp, nb, minx, miny, minz, maxx, maxy, maxz);
      }
      if (!MinMaxSmall(minx, miny, minz, maxx, maxy, maxz, toler))
      {
        issmall = false;
        break;
      } // small in V ?
    }
    if (issmall)
    {
      myStatusStrip = ShapeExtend::EncodeStatus(ShapeExtend_DONE2);
      return issmall; // OK, small in V
    }
    issmall = true;
    for (iv = 1; iv <= nbv; iv++)
    {
      //    for each V line, scan poles in U (U direction)
      int nb = 0;
      for (iu = 1; iu <= nbu; iu++)
      {
        gp_Pnt unp = (cbz ? bz->Pole(iu, iv) : bs->Pole(iu, iv));
        MinMaxPnt(unp, nb, minx, miny, minz, maxx, maxy, maxz);
      }
      if (!MinMaxSmall(minx, miny, minz, maxx, maxy, maxz, toler))
      {
        issmall = false;
        break;
      } // small in U ?
    }
    if (issmall)
    {
      myStatusStrip = ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
      return issmall;
    } // OK, small in U
  }

  return false;
}

//=================================================================================================

bool ShapeAnalysis_CheckSmallFace::CheckStripEdges(const TopoDS_Edge& E1,
                                                   const TopoDS_Edge& E2,
                                                   const double       tol,
                                                   double&            dmax) const
{
  //  We have the topological configuration OK : 2 edges, 2 vertices
  //  But, are these two edges well confused ?
  double toler = tol;
  if (tol < 0)
  {
    double tole = BRep_Tool::Tolerance(E1) + BRep_Tool::Tolerance(E2);
    if (toler < tole / 2.)
      toler = tole / 2.;
  }

  //   We project a list of points from each curve, on the opposite one,
  //   we check the distance
  int nbint = 10;

  ShapeAnalysis_Curve SAC;
  double              cf1, cl1, cf2, cl2, u;
  dmax = 0;
  occ::handle<Geom_Curve> C1, C2;
  C1 = BRep_Tool::Curve(E1, cf1, cl1);
  C2 = BRep_Tool::Curve(E2, cf2, cl2);
  if (C1.IsNull() || C2.IsNull())
    return false;
  cf1                                = std::max(cf1, C1->FirstParameter());
  cl1                                = std::min(cl1, C1->LastParameter());
  occ::handle<Geom_TrimmedCurve> C1T = new Geom_TrimmedCurve(C1, cf1, cl1, true);
  // pdn protection against feature in Trimmed_Curve
  cf1 = C1T->FirstParameter();
  cl1 = C1T->LastParameter();
  occ::handle<Geom_TrimmedCurve> CC;
  cf2                                = std::max(cf2, C2->FirstParameter());
  cl2                                = std::min(cl2, C2->LastParameter());
  occ::handle<Geom_TrimmedCurve> C2T = new Geom_TrimmedCurve(C2, cf2, cl2, true);
  cf2                                = C2T->FirstParameter();
  cl2                                = C2T->LastParameter();

  double cd1 = (cl1 - cf1) / nbint;
  double cd2 = (cl2 - cf2) / nbint;
  double f, l;
  f = cf2;
  l = cl2;
  for (int numcur = 0; numcur < 2; numcur++)
  {
    u = cf1;
    if (numcur)
    {
      CC  = C1T;
      C1T = C2T;
      C2T = CC;
      cd1 = cd2; // smh added replacing step and replacing first
      u   = cf2; // parameter
      f   = cf1;
      l   = cl1;
    }
    for (int nump = 0; nump <= nbint; nump++)
    {
      gp_Pnt p2, p1 = C1T->Value(u);
      double para;
      // pdn Adaptor curve is used to avoid of enhancing of domain.
      GeomAdaptor_Curve GAC(C2T);
      double            dist = SAC.Project(GAC, p1, toler, p2, para);
      // pdn check if parameter of projection is in the domain of the edge.
      if (para < f || para > l)
        return false;
      if (dist > dmax)
        dmax = dist;
      if (dist > toler)
        return false;
      u += cd1;
    }
  }
  return (dmax < toler);
}

//=================================================================================================

bool ShapeAnalysis_CheckSmallFace::FindStripEdges(const TopoDS_Face& F,
                                                  TopoDS_Edge&       E1,
                                                  TopoDS_Edge&       E2,
                                                  const double       tol,
                                                  double&            dmax)
{
  E1.Nullify();
  E2.Nullify();
  int nb = 0;
  for (TopExp_Explorer ex(F, TopAbs_EDGE); ex.More(); ex.Next())
  {
    TopoDS_Edge E = TopoDS::Edge(ex.Current());
    if (nb == 1 && E.IsSame(E1))
      continue; // ignore seam edge
    TopoDS_Vertex V1, V2;
    TopExp::Vertices(E, V1, V2);
    gp_Pnt p1, p2;
    p1           = BRep_Tool::Pnt(V1);
    p2           = BRep_Tool::Pnt(V2);
    double toler = tol;
    if (toler <= 0)
      toler = (BRep_Tool::Tolerance(V1) + BRep_Tool::Tolerance(V2)) / 2.;

    //    Extremities
    double dist = p1.Distance(p2);
    //    Middle point
    double                  cf, cl;
    occ::handle<Geom_Curve> CC;
    CC                = BRep_Tool::Curve(E, cf, cl);
    bool isNullLength = true;
    if (!CC.IsNull())
    {
      gp_Pnt pp = CC->Value((cf + cl) / 2.);
      if (pp.Distance(p1) < toler && pp.Distance(p2) < toler)
        continue;
      isNullLength = false;
    }
    if (dist <= toler && isNullLength)
      continue; // smh
    nb++;
    if (nb == 1)
      E1 = E;
    else if (nb == 2)
      E2 = E;
    else
      return false;
  }
  //   Now, check these two edge to define a strip !
  if (!E1.IsNull() && !E2.IsNull())
  {
    if (!CheckStripEdges(E1, E2, tol, dmax))
      return false;
    else
    {
      myStatusStrip = ShapeExtend::EncodeStatus(ShapeExtend_DONE3);
      return true;
    }
  }
  return false;
}

//=================================================================================================

bool ShapeAnalysis_CheckSmallFace::CheckSingleStrip(const TopoDS_Face& F,
                                                    TopoDS_Edge&       E1,
                                                    TopoDS_Edge&       E2,
                                                    const double       tol)
{
  double toler = tol;
  double minx, miny, minz, maxx, maxy, maxz;

  // In this case, we have 2 vertices and 2 great edges. Plus possibly 2 small
  //    edges, one on each vertex
  TopoDS_Vertex V1, V2;
  int           nb = 0;
  for (TopExp_Explorer itv(F, TopAbs_VERTEX); itv.More(); itv.Next())
  {
    TopoDS_Vertex V = TopoDS::Vertex(itv.Current());
    if (V1.IsNull())
      V1 = V;
    else if (V1.IsSame(V))
      continue;
    else if (V2.IsNull())
      V2 = V;
    else if (V2.IsSame(V))
      continue;
    else
      return false;
  }

  // Checking edges
  // TopoDS_Edge E1,E2;
  nb = 0;
  for (TopExp_Explorer ite(F, TopAbs_EDGE); ite.More(); ite.Next())
  {
    TopoDS_Edge E = TopoDS::Edge(ite.Current());
    if (nb == 1 && E.IsSame(E1))
      continue; // ignore seam edge
    TopoDS_Vertex VA, VB;
    TopExp::Vertices(E, VA, VB);
    if (tol < 0)
    {
      double tolv;
      tolv = BRep_Tool::Tolerance(VA);
      if (toler < tolv)
        toler = tolv;
      tolv = BRep_Tool::Tolerance(VB);
      if (toler < tolv)
        toler = tolv;
    }

    //    Edge on same vertex : small one ?
    if (VA.IsSame(VB))
    {
      double                  cf = 0., cl = 0.;
      occ::handle<Geom_Curve> C3D;
      if (!BRep_Tool::Degenerated(E))
        C3D = BRep_Tool::Curve(E, cf, cl);
      if (C3D.IsNull())
        continue; // DGNR
      int    np  = 0;
      gp_Pnt deb = C3D->Value(cf);
      MinMaxPnt(deb, np, minx, miny, minz, maxx, maxy, maxz);
      gp_Pnt fin = C3D->Value(cl);
      MinMaxPnt(fin, np, minx, miny, minz, maxx, maxy, maxz);
      gp_Pnt mid = C3D->Value((cf + cl) / 2.);
      MinMaxPnt(mid, np, minx, miny, minz, maxx, maxy, maxz);
      if (!MinMaxSmall(minx, miny, minz, maxx, maxy, maxz, toler))
        return false;
    }
    else
    {
      //    Other case : two maximum allowed
      nb++;
      if (nb > 2)
        return false;
      if (nb == 1)
      {
        V1 = VA;
        V2 = VB;
        E1 = E;
      }
      else if (nb == 2)
      {
        if (V1.IsSame(VA) && !V2.IsSame(VB))
          return false;
        if (V1.IsSame(VB) && !V2.IsSame(VA))
          return false;
        E2 = E;
      }
      else
        return false;
    }
  }

  if (nb < 2)
    return false; // only one vertex : cannot be a strip ...

  //   Checking if E1 and E2 define a Strip
  double dmax;
  if (!CheckStripEdges(E1, E2, tol, dmax))
    return false;
  myStatusStrip = ShapeExtend::EncodeStatus(ShapeExtend_DONE3);
  return true;
}

//=================================================================================================

bool ShapeAnalysis_CheckSmallFace::CheckStripFace(const TopoDS_Face& F,
                                                  TopoDS_Edge&       E1,
                                                  TopoDS_Edge&       E2,
                                                  const double       tol)
{

  // int stat;
  if (CheckSingleStrip(F, E1, E2, tol))
    return true; // it is a strip

  //    IsStripSupport used as rejection. But this kind of test may be done
  //    on ANY face, once we are SURE that FindStripEdges is reliable (and fast
  //    enough)

  //  ?? record a diagnostic StripFace, but without yet lists of edges
  //  ??  Record Diagnostic "StripFace", no data (should be "Edges1" "Edges2")
  //      but direction is known (1:U  2:V)
  // TopoDS_Edge E1,E2;
  double dmax;
  return FindStripEdges(F, E1, E2, tol, dmax);
}

//=================================================================================================

int ShapeAnalysis_CheckSmallFace::CheckSplittingVertices(
  const TopoDS_Face& F,
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
                                                                                        MapEdges,
  NCollection_DataMap<TopoDS_Shape, NCollection_List<double>, TopTools_ShapeMapHasher>& MapParam,
  TopoDS_Compound&                                                                      theAllVert)
{

  //  Prepare array of vertices with their locations //TopTools
  int nbv = 0, nbp = 0;
  // TopoDS_Compound theAllVert;
  BRep_Builder theBuilder;
  // theBuilder.MakeCompound(theAllVert);
  TopExp_Explorer itv; // svv Jan11 2000 : porting on DEC
  for (itv.Init(F, TopAbs_VERTEX); itv.More(); itv.Next())
    nbv++;

  if (nbv == 0)
    return 0;
  NCollection_Array1<TopoDS_Shape> vtx(1, nbv);
  NCollection_Array1<gp_Pnt>       vtp(1, nbv);
  NCollection_Array1<double>       vto(1, nbv);

  nbp = 0;
  for (itv.Init(F, TopAbs_VERTEX); itv.More(); itv.Next())
  {
    nbp++;
    TopoDS_Vertex unv = TopoDS::Vertex(itv.Current());
    vtx.SetValue(nbp, unv);
    gp_Pnt unp = BRep_Tool::Pnt(unv);
    vtp.SetValue(nbp, unp);
    double unt = myPrecision;
    if (unt < 0)
      unt = BRep_Tool::Tolerance(unv);
    vto.SetValue(nbp, unt);
  }
  nbv = nbp;
  nbp = 0; // now, counting splitting vertices

  //  Check edges : are vertices (other than extremities) confused with it ?
  ShapeAnalysis_Curve SAC;
  for (int iv = 1; iv <= nbv; iv++)
  {
    TopoDS_Vertex                  V = TopoDS::Vertex(vtx.Value(iv));
    NCollection_List<TopoDS_Shape> listEdge;
    NCollection_List<double>       listParam;
    bool                           issplit = false;
    for (TopExp_Explorer ite(F, TopAbs_EDGE); ite.More(); ite.Next())
    {
      TopoDS_Edge   E = TopoDS::Edge(ite.Current());
      TopoDS_Vertex V1, V2;
      TopExp::Vertices(E, V1, V2);
      double                  cf, cl;
      occ::handle<Geom_Curve> C3D = BRep_Tool::Curve(E, cf, cl);
      if (C3D.IsNull())
        continue;
      if (V.IsSame(V1) || V.IsSame(V2))
        continue;
      gp_Pnt unp = vtp.Value(iv);
      double unt = vto.Value(iv);
      gp_Pnt proj;
      double param;
      double dist = SAC.Project(C3D, unp, unt * 10., proj, param, cf, cl);
      if (dist == 0.0)
        continue; // smh
      //  Splitting Vertex to record ?
      if (dist < unt)
      {
        //  If Split occurs at beginning or end, it is not a split ...
        double fpar, lpar, eps = 1.e-06;
        if (param >= cl || param <= cf)
          continue; // Out of range
        fpar = param - cf;
        lpar = param - cl;
        if ((std::abs(fpar) < eps) || (std::abs(lpar) < eps))
          continue; // Near end or start
        listEdge.Append(E);
        listParam.Append(param);
        issplit = true;
      }
    }
    if (issplit)
    {
      nbp++;
      theBuilder.Add(theAllVert, V);
      MapEdges.Bind(V, listEdge);
      MapParam.Bind(V, listParam);
    }
  }
  if (nbp != 0)
    myStatusSplitVert = ShapeExtend::EncodeStatus(ShapeExtend_DONE);
  return nbp;
}

static int IsoStat(const NCollection_Array2<gp_Pnt>& poles,
                   const int                         uorv,
                   const int                         rank,
                   const double                      tolpin,
                   const double                      toler)
{
  int    i, np = 0;
  int    i0   = (uorv == 1 ? poles.LowerCol() : poles.LowerRow());
  int    i1   = (uorv == 1 ? poles.UpperCol() : poles.UpperRow());
  double xmin = 0., ymin = 0., zmin = 0., xmax = 0., ymax = 0., zmax = 0.;
  for (i = i0; i <= i1; i++)
  {
    if (uorv == 1)
      MinMaxPnt(poles(rank, i), np, xmin, ymin, zmin, xmax, ymax, zmax);
    else
      MinMaxPnt(poles(i, rank), np, xmin, ymin, zmin, xmax, ymax, zmax);
  }
  if (MinMaxSmall(xmin, ymin, zmin, xmax, ymax, zmax, tolpin))
    return 0;
  if (MinMaxSmall(xmin, ymin, zmin, xmax, ymax, zmax, toler))
    return 1;
  return 2;
}

static bool CheckPoles(const NCollection_Array2<gp_Pnt>& poles, int uorv, int rank)
{
  int i0 = (uorv == 1 ? poles.LowerCol() : poles.LowerRow());
  int i1 = (uorv == 1 ? poles.UpperCol() : poles.UpperRow());
  for (int i = i0; i <= i1 - 1; i++)
  {
    if (uorv == 1)
    {
      if (poles(rank, i).IsEqual(poles(rank, i + 1), 1e-15))
        return true;
    }
    else if (poles(i, rank).IsEqual(poles(i + 1, rank), 1e-15))
      return true;
  }
  return false;
}

//=================================================================================================

bool ShapeAnalysis_CheckSmallFace::CheckPin(const TopoDS_Face& F, int& whatrow, int& sens)
{
  TopLoc_Location           loc;
  occ::handle<Geom_Surface> surf = BRep_Tool::Surface(F, loc);
  if (surf->IsKind(STANDARD_TYPE(Geom_ElementarySurface)))
    return false;

  double toler = myPrecision;
  if (toler < 0)
    toler = 1.e-4;
  double tolpin = 1.e-9; // for sharp sharp pin

  //  Checking the poles

  //  Take the poles : they give good idea of sharpness of a pin
  int                              nbu = 0, nbv = 0;
  occ::handle<Geom_BSplineSurface> bs = occ::down_cast<Geom_BSplineSurface>(surf);
  occ::handle<Geom_BezierSurface>  bz = occ::down_cast<Geom_BezierSurface>(surf);
  if (!bs.IsNull())
  {
    nbu = bs->NbUPoles();
    nbv = bs->NbVPoles();
  }
  if (!bz.IsNull())
  {
    nbu = bz->NbUPoles();
    nbv = bz->NbVPoles();
  }
  if (nbu == 0 || nbv == 0)
    return false;

  const NCollection_Array2<gp_Pnt>* allpolesPtr = nullptr;
  if (!bs.IsNull())
    allpolesPtr = &bs->Poles();
  if (!bz.IsNull())
    allpolesPtr = &bz->Poles();
  const NCollection_Array2<gp_Pnt>& allpoles = *allpolesPtr;

  //  Check each natural bound if it is a singularity (i.e. a pin)

  sens     = 0;
  int stat = 0; // 0 none, 1 in U, 2 in V
  whatrow  = 0; // 0 no row, else rank of row
  stat     = IsoStat(allpoles, 1, 1, tolpin, toler);
  if (stat)
  {
    sens    = 1;
    whatrow = nbu;
  }

  stat = IsoStat(allpoles, 1, nbu, tolpin, toler);
  if (stat)
  {
    sens    = 1;
    whatrow = nbu;
  }

  stat = IsoStat(allpoles, 2, 1, tolpin, toler);
  if (stat)
  {
    sens    = 2;
    whatrow = 1;
  }

  stat = IsoStat(allpoles, 2, nbv, tolpin, toler);
  if (stat)
  {
    sens    = 2;
    whatrow = nbv;
  }

  if (!sens)
    return false; // no pin

  switch (stat)
  {
    case 1:
      myStatusPin = ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
      break;
    case 2:
      myStatusPin = ShapeExtend::EncodeStatus(ShapeExtend_DONE2);
      break;
    default:
      break;
  }
  //  std::cout<<(whatstat == 1 ? "Smooth" : "Sharp")<<" Pin on "<<(sens == 1 ? "U" : "V")<<" Row n0
  //  "<<whatrow<<std::endl;
  if (stat == 1)
  {
    // bool EqualPoles = false;
    if (CheckPoles(allpoles, 2, nbv) || CheckPoles(allpoles, 2, 1) || CheckPoles(allpoles, 1, nbu)
        || CheckPoles(allpoles, 1, 1))
      myStatusPin = ShapeExtend::EncodeStatus(ShapeExtend_DONE3);
  }

  return true;
}

static double TwistedNorm(const double x1,
                          const double y1,
                          const double z1,
                          const double x2,
                          const double y2,
                          const double z2)
{
  return (x1 * x2) + (y1 * y2) + (z1 * z2);
}

//=================================================================================================

bool ShapeAnalysis_CheckSmallFace::CheckTwisted(const TopoDS_Face& F,
                                                double&            paramu,
                                                double&            paramv)
{
  TopLoc_Location           loc;
  occ::handle<Geom_Surface> surf = BRep_Tool::Surface(F, loc);
  if (surf->IsKind(STANDARD_TYPE(Geom_ElementarySurface)))
    return false;

  double toler = myPrecision;
  if (toler < 0)
    toler = 1.e-4;
  ////  GeomLProp_SLProps GLS (surf,2,toler);
  GeomAdaptor_Surface GAS(surf);

  // to be done : on isos of the surface
  //  and on edges, at least of outer wire
  int                        nbint = 5;
  NCollection_Array2<double> nx(1, nbint + 1, 1, nbint + 1);
  NCollection_Array2<double> ny(1, nbint + 1, 1, nbint + 1);
  NCollection_Array2<double> nz(1, nbint + 1, 1, nbint + 1);
  int                        iu, iv;
  double                     umin, umax, vmin, vmax;
  surf->Bounds(umin, umax, vmin, vmax);
  double u = umin, du = (umax - umin) / nbint;
  double v = vmin, dv = (umax - umin) / nbint;

  // gp_Dir norm;
  for (iu = 1; iu <= nbint; iu++)
  {
    for (iv = 1; iv <= nbint; iv++)
    {
      //      GLS.SetParameters (u,v);
      //      if (GLS.IsNormalDefined()) norm = GLS.Normal();
      gp_Pnt curp;
      gp_Vec V1, V2, VXnorm;
      GAS.D1(u, v, curp, V1, V2);
      VXnorm = V1.Crossed(V2);
      nx.SetValue(iu, iv, VXnorm.X());
      ny.SetValue(iu, iv, VXnorm.Y());
      nz.SetValue(iu, iv, VXnorm.Z());
      v += dv;
    }
    u += du;
    v = vmin;
  }

  //  Now, comparing normals on support surface, in both senses
  //  In principle, it suffuces to check within outer bound

  for (iu = 1; iu < nbint; iu++)
  {
    for (iv = 1; iv < nbint; iv++)
    {
      // We here check each normal (iu,iv) with (iu,iv+1) and with (iu+1,iv)
      // if for each test, we have negative scalar product, this means angle > 90deg
      // it is the criterion to say it is twisted
      if (TwistedNorm(nx(iu, iv),
                      ny(iu, iv),
                      nz(iu, iv),
                      nx(iu, iv + 1),
                      ny(iu, iv + 1),
                      nz(iu, iv + 1))
            < 0.
          || TwistedNorm(nx(iu, iv),
                         ny(iu, iv),
                         nz(iu, iv),
                         nx(iu + 1, iv),
                         ny(iu + 1, iv),
                         nz(iu + 1, iv))
               < 0.)
      {
        myStatusTwisted = ShapeExtend::EncodeStatus(ShapeExtend_DONE);
        paramu          = umin + du * iu - du / 2;
        paramv          = vmin + dv * iv - dv / 2;
        return true;
      }
    }
  }

  //   Now, comparing normals on edges ... to be done

  return false;
}

//=================================================================================================

// Warning: This function not tested on many examples

bool ShapeAnalysis_CheckSmallFace::CheckPinFace(
  const TopoDS_Face&                                                        F,
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& mapEdges,
  const double                                                              toler)
{
  // ShapeFix_Wire sfw;
  TopExp_Explorer exp_w(F, TopAbs_WIRE);
  exp_w.More();
  double                            coef1      = 0, coef2; // =0 for deleting warning (skl)
  TopoDS_Wire                       theCurWire = TopoDS::Wire(exp_w.Current());
  ShapeAnalysis_WireOrder           wi;
  ShapeAnalysis_Wire                sfw;
  occ::handle<ShapeExtend_WireData> sbwd = new ShapeExtend_WireData(theCurWire);
  sfw.Load(sbwd);
  sfw.CheckOrder(wi);
  occ::handle<NCollection_HSequence<TopoDS_Shape>> newedges =
    new NCollection_HSequence<TopoDS_Shape>();
  int nb = wi.NbEdges();
  int i  = 0;
  for (i = 1; i <= nb; i++)
    newedges->Append(sbwd->Edge(wi.Ordered(i)));
  for (i = 1; i <= nb; i++)
    sbwd->Set(TopoDS::Edge(newedges->Value(i)), i);
  // sfw.Init(theCurWire,  F, Precision::Confusion());
  // sfw.FixReorder();
  // theCurWire = sfw.Wire();
  theCurWire       = sbwd->Wire();
  i                = 1;
  bool        done = false;
  double      tol  = Precision::Confusion();
  TopoDS_Edge theFirstEdge, theSecondEdge;
  double      d1 = 0, d2 = 0;
  for (TopExp_Explorer exp_e(F, TopAbs_EDGE); exp_e.More(); exp_e.Next())
  {
    TopoDS_Vertex V1, V2;
    gp_Pnt        p1, p2;
    if (i == 1)
    {
      theFirstEdge = TopoDS::Edge(exp_e.Current());
      V1           = TopExp::FirstVertex(theFirstEdge);
      V2           = TopExp::LastVertex(theFirstEdge);
      p1           = BRep_Tool::Pnt(V1);
      p2           = BRep_Tool::Pnt(V2);
      tol          = std::max(BRep_Tool::Tolerance(V1), BRep_Tool::Tolerance(V2));
      if (toler > 0) // tol = std::max(tol, toler); gka
        tol = toler;
      d1 = p1.Distance(p2);
      if (d1 == 0)
        return false;
      if (d1 / tol >= 1)
        coef1 = d1 / tol;
      else
        continue;
      if (coef1 <= 3)
        continue;
      i++;
      continue;
    }
    // Check the length of edge
    theSecondEdge = TopoDS::Edge(exp_e.Current());
    V1            = TopExp::FirstVertex(theSecondEdge);
    V2            = TopExp::LastVertex(theSecondEdge);

    p1 = BRep_Tool::Pnt(V1);
    p2 = BRep_Tool::Pnt(V2);
    if (toler == -1)
      tol = std::max(BRep_Tool::Tolerance(V1), BRep_Tool::Tolerance(V2));
    else
      tol = toler;
    if (p1.Distance(p2) > tol)
      continue;
    // If there are two pin edges, record them in diagnostic
    d2 = p1.Distance(p2); // gka
    if (d2 == 0)
      return false;
    if (d2 / tol >= 1)
      coef2 = d2 / tol;
    else
      continue;
    if (coef2 <= 3)
      continue;
    if (coef1 > coef2 * 10)
      continue;
    if (coef2 > coef1 * 10)
    {
      theFirstEdge = theSecondEdge;
      coef1        = coef2;
      continue;
    }

    if (CheckPinEdges(theFirstEdge, theSecondEdge, coef1, coef2, toler))
    {
      mapEdges.Bind(theFirstEdge, theSecondEdge);
      myStatusPinFace = ShapeExtend::EncodeStatus(ShapeExtend_DONE);
      done            = true;
    }

    theFirstEdge = theSecondEdge;
    coef1        = coef2;
    // d1 = d2;
  }
  return done;
}

//=================================================================================================

// Warning: This function not tested on many examples

bool ShapeAnalysis_CheckSmallFace::CheckPinEdges(const TopoDS_Edge& theFirstEdge,
                                                 const TopoDS_Edge& theSecondEdge,
                                                 const double       coef1,
                                                 const double       coef2,
                                                 const double       toler) const
{

  double                  cf1, cl1, cf2, cl2;
  occ::handle<Geom_Curve> C1, C2, C3;
  C1 = BRep_Tool::Curve(theFirstEdge, cf1, cl1);
  C2 = BRep_Tool::Curve(theSecondEdge, cf2, cl2);
  gp_Pnt p1, p2, pp1, pp2, pv;
  double d1 = (cf1 - cl1) / coef1;
  double d2 = (cf2 - cl2) / coef2;
  // double d1 = cf1-cl1/30; //10; gka
  // double d2 = cf2-cl2/30; //10;
  p1  = C1->Value(cf1);
  p2  = C1->Value(cl1);
  pp1 = C2->Value(cf2);
  pp2 = C2->Value(cl2);
  double        tol;
  double        paramc1 = 0, paramc2 = 0; // =0 for deleting warning (skl)
  TopoDS_Vertex theSharedV = TopExp::LastVertex(theFirstEdge);
  if (toler == -1)
    tol = BRep_Tool::Tolerance(theSharedV);
  else
    tol = toler;
  pv = BRep_Tool::Pnt(theSharedV);
  if (pv.Distance(p1) <= tol)
    paramc1 = cf1;
  else if (pv.Distance(p2) <= tol)
    paramc1 = cl1;
  if (pv.Distance(pp1) <= tol)
    paramc2 = cf2;
  else if (pv.Distance(pp2) <= tol)
    paramc2 = cl2;
  // Computing first derivative vectors and compare angle
  //   gp_Vec V11, V12, V21, V22;
  //   gp_Pnt tmp;
  //   C1->D2(paramc1, tmp, V11, V21);
  //   C2->D2(paramc2, tmp, V12, V22);
  //   double angle1, angle2;
  //   try{
  //     angle1 = V11.Angle(V12);
  //     angle2 = V21.Angle(V22);
  //   }
  //   catch (Standard_Failure)
  //     {
  //       std::cout << "Couldn't compute angle between derivative vectors"  <<std::endl;
  //       return false;
  //     }
  //   std::cout << "angle1 "   << angle1<< std::endl;
  //   std::cout << "angle2 "   << angle2<< std::endl;
  //   if (angle1<=0.0001) return true;
  gp_Pnt proj;
  if (p1.Distance(p2) < pp1.Distance(pp2))
  {
    C3 = C1;
    if (paramc1 == cf1)
      proj = C1->Value(paramc1 + (coef1 - 3) * d1);
    else
      proj = C1->Value(paramc1 - 3 * d1);
    // proj = C1->Value(paramc1 + 9*d1);
    // else proj = C1->Value(paramc1-d1);
  }
  else
  {
    C3 = C2;
    if (paramc2 == cf2)
      proj = C2->Value(paramc2 + (coef2 - 3) * d2);
    else
      proj = C2->Value(paramc2 - 3 * d2);
    // proj = C2->Value(paramc2 + 9*d2);
    // else proj = C2->Value(paramc2 -d2);
  }
  double              param;
  GeomAdaptor_Curve   GAC(C3);
  double              f = C3->FirstParameter();
  double              l = C3->LastParameter();
  gp_Pnt              result;
  ShapeAnalysis_Curve SAC;
  double              dist = SAC.Project(GAC, proj, tol, result, param);
  // pdn check if parameter of projection is in the domain of the edge.
  if (param < f || param > l)
    return false;
  if (dist > tol)
    return false;
  if (dist <= tol)
  {
    // Computing first derivative vectors and compare angle
    gp_Vec V11, V12, V21, V22;
    gp_Pnt tmp;
    C1->D2(paramc1, tmp, V11, V21);
    C2->D2(paramc2, tmp, V12, V22);
    double angle1 = 0, angle2 = 0;
    try
    {
      angle1 = V11.Angle(V12);
      angle2 = V21.Angle(V22);
    }
    catch (Standard_Failure const&)
    {
#ifdef OCCT_DEBUG
      std::cout << "Couldn't compute angle between derivative vectors" << std::endl;
#endif
      return false;
    }
    //       std::cout << "angle1 "   << angle1<< std::endl;
    //       std::cout << "angle2 "   << angle2<< std::endl;
    return (angle1 <= 0.001 && angle2 <= 0.01)
           || ((M_PI - angle2) <= 0.001 && (M_PI - angle2) <= 0.01);
  }

  return false;
}
