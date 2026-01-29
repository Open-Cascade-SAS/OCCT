// Created on: 1994-03-10
// Created by: Laurent BUCHARD
// Copyright (c) 1994-1999 Matra Datavision
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

// Modified:     Porting NT 7-5-97 DPF (stdio.h)
//              Apr 16 2002 eap, classification against infinite solid (occ299)

//  Modified by skv - Thu Sep  4 12:29:30 2003 OCC578

#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve2d.hxx>
#include <BRepBndLib.hxx>
#include <BRepClass3d_SolidExplorer.hxx>
#include <BRepClass_FaceClassifier.hxx>
#include <BRepClass_FacePassiveClassifier.hxx>
#include <BRepTopAdaptor_FClass2d.hxx>
#include <Extrema_ExtPS.hxx>
#include <gp.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <IntCurvesFace_Intersector.hxx>
#include <Precision.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopExp.hxx>

//=======================================================================
// function : FindAPointInTheFace
// purpose  : Compute a point P in the face  F. Param is a Real in
//            ]0,1[ and   is  used to  initialise  the algorithm. For
//            different values , different points are returned.
//=======================================================================
bool BRepClass3d_SolidExplorer::FindAPointInTheFace(const TopoDS_Face& _face,
                                                    gp_Pnt&            APoint_,
                                                    double&            param_)
{
  double u, v;
  bool   r = FindAPointInTheFace(_face, APoint_, u, v, param_);
  return r;
}

//=================================================================================================

bool BRepClass3d_SolidExplorer::FindAPointInTheFace(const TopoDS_Face& _face,
                                                    gp_Pnt&            APoint_,
                                                    double&            u_,
                                                    double&            v_,
                                                    double&            param_)
{
  gp_Vec aVecD1U, aVecD1V;
  return FindAPointInTheFace(_face, APoint_, u_, v_, param_, aVecD1U, aVecD1V);
}

//=================================================================================================

bool BRepClass3d_SolidExplorer::FindAPointInTheFace(const TopoDS_Face& _face,
                                                    gp_Pnt&            APoint_,
                                                    double&            u_,
                                                    double&            v_,
                                                    double&            param_,
                                                    gp_Vec&            theVecD1U,
                                                    gp_Vec&            theVecD1V)
{
  TopoDS_Face face = _face;
  face.Orientation(TopAbs_FORWARD);

  TopExp_Explorer     faceexplorer;
  BRepAdaptor_Curve2d c;
  gp_Vec2d            T;
  gp_Pnt2d            P;

  for (faceexplorer.Init(face, TopAbs_EDGE); faceexplorer.More(); faceexplorer.Next())
  {
    TopoDS_Edge Edge = TopoDS::Edge(faceexplorer.Current());
    c.Initialize(Edge, face);
    if (std::abs(c.FirstParameter()) < Precision::PConfusion()
        && std::abs(c.LastParameter()) < Precision::PConfusion())
    {
      continue;
    }
    c.D1((c.LastParameter() - c.FirstParameter()) * param_ + c.FirstParameter(), P, T);

    double x = T.X();
    double y = T.Y();
    if (Edge.Orientation() == TopAbs_FORWARD)
    {
      T.SetCoord(-y, x);
    }
    else
    {
      T.SetCoord(y, -x);
    }
    double ParamInit   = Precision::Infinite();
    double TolInit     = 0.00001;
    bool   APointExist = false;

    BRepClass_FacePassiveClassifier FClassifier;

    T.Normalize();
    P.SetCoord(P.X() + TolInit * T.X(), P.Y() + TolInit * T.Y());
    // clang-format off
    FClassifier.Reset (gp_Lin2d (P, T), ParamInit, RealEpsilon()); //-- Length and Tolerance #######
    // clang-format on

    TopExp_Explorer otherfaceexplorer;
    int             aNbEdges = 0;
    for (otherfaceexplorer.Init(face, TopAbs_EDGE); otherfaceexplorer.More();
         otherfaceexplorer.Next(), ++aNbEdges)
    {
      TopoDS_Edge OtherEdge = TopoDS::Edge(otherfaceexplorer.Current());
      if (OtherEdge.Orientation() != TopAbs_EXTERNAL && OtherEdge != Edge)
      {
        BRepClass_Edge AEdge(OtherEdge, face);
        FClassifier.Compare(AEdge, OtherEdge.Orientation());
        if (FClassifier.ClosestIntersection())
        {
          if (ParamInit > FClassifier.Parameter())
          {
            ParamInit   = FClassifier.Parameter();
            APointExist = true;
          }
        }
      }
    }

    if (aNbEdges == 1)
    {
      BRepClass_Edge AEdge(Edge, face);
      FClassifier.Compare(AEdge, Edge.Orientation());
      if (FClassifier.ClosestIntersection())
      {
        if (ParamInit > FClassifier.Parameter())
        {
          ParamInit   = FClassifier.Parameter();
          APointExist = true;
        }
      }
    }

    while (APointExist)
    {
      ParamInit *= 0.41234;
      u_ = P.X() + ParamInit * T.X();
      v_ = P.Y() + ParamInit * T.Y();

      // Additional check
      BRepTopAdaptor_FClass2d Classifier(face, Precision::Confusion());
      gp_Pnt2d                aPnt2d(u_, v_);
      TopAbs_State            StateOfResultingPoint = Classifier.Perform(aPnt2d);
      if (StateOfResultingPoint != TopAbs_IN)
        return false;

      BRepAdaptor_Surface s;
      s.Initialize(face, false);
      s.D1(u_, v_, APoint_, theVecD1U, theVecD1V);

      if (theVecD1U.CrossMagnitude(theVecD1V) > gp::Resolution())
        return true;

      if (ParamInit < Precision::PConfusion())
        return false;
    }
  }
  return false;
}

//=================================================================================================

bool BRepClass3d_SolidExplorer::PointInTheFace(const TopoDS_Face&                      Face,
                                               gp_Pnt&                                 APoint_,
                                               double&                                 u_,
                                               double&                                 v_,
                                               double&                                 param_,
                                               int&                                    IndexPoint,
                                               const occ::handle<BRepAdaptor_Surface>& surf,
                                               const double                            U1,
                                               const double                            V1,
                                               const double                            U2,
                                               const double                            V2) const
{
  gp_Vec aVecD1U, aVecD1V;
  return PointInTheFace(Face,
                        APoint_,
                        u_,
                        v_,
                        param_,
                        IndexPoint,
                        surf,
                        U1,
                        V1,
                        U2,
                        V2,
                        aVecD1U,
                        aVecD1V);
}

//=================================================================================================

TopAbs_State BRepClass3d_SolidExplorer::ClassifyUVPoint(
  const IntCurvesFace_Intersector&        theIntersector,
  const occ::handle<BRepAdaptor_Surface>& theSurf,
  const gp_Pnt2d&                         theP2d) const
{
  // first find if the point is near an edge/vertex
  gp_Pnt                              aP3d = theSurf->Value(theP2d.X(), theP2d.Y());
  BRepClass3d_BndBoxTreeSelectorPoint aSelectorPoint(myMapEV);
  aSelectorPoint.SetCurrentPoint(aP3d);
  int aSelsVE = myTree.Select(aSelectorPoint);
  if (aSelsVE > 0)
  {
    // The point is inside the tolerance area of vertices/edges => return ON state.
    return TopAbs_ON;
  }
  return theIntersector.ClassifyUVPoint(theP2d);
}

//=================================================================================================

bool BRepClass3d_SolidExplorer::PointInTheFace(const TopoDS_Face&                      Face,
                                               gp_Pnt&                                 APoint_,
                                               double&                                 u_,
                                               double&                                 v_,
                                               double&                                 param_,
                                               int&                                    IndexPoint,
                                               const occ::handle<BRepAdaptor_Surface>& surf,
                                               const double                            U1,
                                               const double                            V1,
                                               const double                            U2,
                                               const double                            V2,
                                               gp_Vec&                                 theVecD1U,
                                               gp_Vec& theVecD1V) const
{
  double u, du = (U2 - U1) / 6.0;
  double v, dv = (V2 - V1) / 6.0;
  if (du < 1e-12)
    du = 1e-12;
  if (dv < 1e-12)
    dv = 1e-12;
  bool IsNotUper = !surf->IsUPeriodic(), IsNotVper = !surf->IsVPeriodic();
  int  NbPntCalc = 0;
  if (myMapOfInter.IsBound(Face))
  {
    void* ptr      = (void*)(myMapOfInter.Find(Face));
    bool  IsInside = true;
    if (IsNotUper)
    {
      IsInside = (u_ >= U1) && (u_ <= U2);
    }
    if (IsNotVper)
    {
      IsInside &= (v_ >= V1) && (v_ <= V2);
    }
    if (ptr)
    {
      const IntCurvesFace_Intersector& TheIntersector = (*((IntCurvesFace_Intersector*)ptr));
      // Check if the point is already in the face
      if (IsInside && (ClassifyUVPoint(TheIntersector, surf, gp_Pnt2d(u_, v_)) == TopAbs_IN))
      {
        gp_Pnt aPnt;
        surf->D1(u_, v_, aPnt, theVecD1U, theVecD1V);
        if (aPnt.SquareDistance(APoint_) < Precision::Confusion() * Precision::Confusion())
          return true;
      }

      //-- Take 4 points in each Quarter of surface
      //-- -> Index : 1 -> 16
      //--
      //--
      //--  Then take a matrix of points on a tight grid
      //--
      for (u = du + (U1 + U2) * 0.5; u < U2; u += du)
      { //--  0  X    u increases
        for (v = dv + (V1 + V2) * 0.5; v < V2; v += dv)
        { //--  0  0    v increases
          if (++NbPntCalc >= IndexPoint)
          {
            if (ClassifyUVPoint(TheIntersector, surf, gp_Pnt2d(u, v)) == TopAbs_IN)
            {
              u_ = u;
              v_ = v;
              surf->D1(u, v, APoint_, theVecD1U, theVecD1V);
              IndexPoint = NbPntCalc;
              return (true);
            }
          }
        }
      }

      for (u = -du + (U1 + U2) * 0.5; u > U1; u -= du)
      { //--  0  0    u decreases
        for (v = -dv + (V1 + V2) * 0.5; v > V1; v -= dv)
        { //--  X  0    v decreases
          if (++NbPntCalc >= IndexPoint)
          {
            if (ClassifyUVPoint(TheIntersector, surf, gp_Pnt2d(u, v)) == TopAbs_IN)
            {
              u_ = u;
              v_ = v;
              surf->D1(u, v, APoint_, theVecD1U, theVecD1V);
              IndexPoint = NbPntCalc;
              return (true);
            }
          }
        }
      }
      for (u = -du + (U1 + U2) * 0.5; u > U1; u -= du)
      { //--  X  0    u decreases
        for (v = dv + (V1 + V2) * 0.5; v < V2; v += dv)
        { //--  0  0    v increases
          if (++NbPntCalc >= IndexPoint)
          {
            if (ClassifyUVPoint(TheIntersector, surf, gp_Pnt2d(u, v)) == TopAbs_IN)
            {
              u_ = u;
              v_ = v;
              surf->D1(u, v, APoint_, theVecD1U, theVecD1V);
              IndexPoint = NbPntCalc;
              return (true);
            }
          }
        }
      }
      for (u = du + (U1 + U2) * 0.5; u < U2; u += du)
      { //--  0  0     u increases
        for (v = -dv + (V1 + V2) * 0.5; v > V1; v -= dv)
        { //--  0  X     v decreases
          if (++NbPntCalc >= IndexPoint)
          {
            if (ClassifyUVPoint(TheIntersector, surf, gp_Pnt2d(u, v)) == TopAbs_IN)
            {
              u_ = u;
              v_ = v;
              surf->D1(u, v, APoint_, theVecD1U, theVecD1V);
              IndexPoint = NbPntCalc;
              return (true);
            }
          }
        }
      }
      //-- the remainder
      du = (U2 - U1) / 37.0;
      dv = (V2 - V1) / 37.0;
      if (du < 1e-12)
        du = 1e-12;
      if (dv < 1e-12)
        dv = 1e-12;

      for (u = du + U1; u < U2; u += du)
      {
        for (v = dv + V1; v < V2; v += dv)
        {
          if (++NbPntCalc >= IndexPoint)
          {
            if (ClassifyUVPoint(TheIntersector, surf, gp_Pnt2d(u, v)) == TopAbs_IN)
            {
              u_ = u;
              v_ = v;
              surf->D1(u, v, APoint_, theVecD1U, theVecD1V);
              IndexPoint = NbPntCalc;
              return (true);
            }
          }
        }
      }
      u = (U1 + U2) * 0.5;
      v = (V1 + V2) * 0.5;
      if (++NbPntCalc >= IndexPoint)
      {
        if (ClassifyUVPoint(TheIntersector, surf, gp_Pnt2d(u, v)) == TopAbs_IN)
        {
          u_ = u;
          v_ = v;
          surf->D1(u, v, APoint_, theVecD1U, theVecD1V);
          IndexPoint = NbPntCalc;
          return (true);
        }
      }
    }
    IndexPoint = NbPntCalc;
  }
  else
  {
    // printf("BRepClass3d_SolidExplorer Face not found ds the map \n");
  }

  return BRepClass3d_SolidExplorer ::FindAPointInTheFace(Face,
                                                         APoint_,
                                                         u_,
                                                         v_,
                                                         param_,
                                                         theVecD1U,
                                                         theVecD1V);
}

//=======================================================================
// function : LimitInfiniteUV
// purpose  : Limit infinite parameters
//=======================================================================
static void LimitInfiniteUV(double& U1, double& V1, double& U2, double& V2)
{
  bool infU1 = Precision::IsNegativeInfinite(U1), infV1 = Precision::IsNegativeInfinite(V1),
       infU2 = Precision::IsPositiveInfinite(U2), infV2 = Precision::IsPositiveInfinite(V2);

  if (infU1)
    U1 = -1e10;
  if (infV1)
    V1 = -1e10;
  if (infU2)
    U2 = 1e10;
  if (infV2)
    V2 = 1e10;
}

//=================================================================================================

static int IsInfiniteUV(double& U1, double& V1, double& U2, double& V2)
{
  int aVal = 0;

  if (Precision::IsInfinite(U1))
    aVal |= 1;

  if (Precision::IsInfinite(V1))
    aVal |= 2;

  if (Precision::IsInfinite(U2))
    aVal |= 4;

  if (Precision::IsInfinite(V2))
    aVal |= 8;

  return aVal;
}

//<-OCC454
//  Modified by skv - Tue Sep 16 13:50:39 2003 OCC578 End
//=======================================================================
// function : OtherSegment
// purpose  : Returns  in <L>, <Par>  a segment having at least
//           one  intersection  with  the  shape  boundary  to
//           compute  intersections.
//           The First Call to this method returns a line which
//           point to a point of the first face of the shape.
//           The Second Call provide a line to the second face
//           and so on.
//=======================================================================
int BRepClass3d_SolidExplorer::OtherSegment(const gp_Pnt& P, gp_Lin& L, double& _Par)
{
  constexpr double TolU = Precision::PConfusion();
  const double     TolV = TolU;

  TopoDS_Face     face;
  TopExp_Explorer faceexplorer;
  gp_Pnt          APoint;
  gp_Vec          aVecD1U, aVecD1V;
  double          maxscal = 0;
  bool            ptfound = false;
  double          Par;
  double          _u, _v;
  int             IndexPoint     = 0;
  int             NbPointsOK     = 0;
  int             NbFacesInSolid = 0;
  bool            aRestr         = true;
  bool            aTestInvert    = false;

  for (;;)
  {
    myFirstFace++;
    faceexplorer.Init(myShape, TopAbs_FACE);
    // look for point on face starting from myFirstFace
    //  Modified by skv - Thu Sep  4 14:31:12 2003 OCC578 Begin
    //     while (faceexplorer.More()) {
    NbFacesInSolid = 0;
    for (; faceexplorer.More(); faceexplorer.Next())
    {
      //  Modified by skv - Thu Sep  4 14:31:12 2003 OCC578 End
      NbFacesInSolid++;
      if (myFirstFace > NbFacesInSolid)
        continue;
      face = TopoDS::Face(faceexplorer.Current());

      occ::handle<BRepAdaptor_Surface> surf = new BRepAdaptor_Surface();
      if (aTestInvert)
      {
        BRepTopAdaptor_FClass2d aClass(face, Precision::Confusion());
        if (aClass.PerformInfinitePoint() == TopAbs_IN)
        {
          aRestr = false;
          if (myMapOfInter.IsBound(face))
          {
            myMapOfInter.UnBind(face);
            void* ptr =
              (void*)(new IntCurvesFace_Intersector(face, Precision::Confusion(), aRestr, false));
            myMapOfInter.Bind(face, ptr);
          }
        }
        else
        {
          aRestr = true;
        }
      }
      surf->Initialize(face, aRestr);
      double U1, V1, U2, V2;
      U1 = surf->FirstUParameter();
      V1 = surf->FirstVParameter();
      U2 = surf->LastUParameter();
      V2 = surf->LastVParameter();
      face.Orientation(TopAbs_FORWARD);
      //
      // avoid process faces from uncorrected shells
      constexpr double eps  = Precision::PConfusion();
      double           epsU = std::max(eps * std::max(std::abs(U2), std::abs(U1)), eps);
      double           epsV = std::max(eps * std::max(std::abs(V2), std::abs(V1)), eps);
      if (std::abs(U2 - U1) < epsU || std::abs(V2 - V1) < epsV)
      {
        return 2;
      }
      //
      double svmyparam = myParamOnEdge;
      //
      // Check if the point is on the face or the face is infinite.
      int anInfFlag = IsInfiniteUV(U1, V1, U2, V2);
      // default values
      _u = (U1 + U2) * 0.5;
      _v = (V1 + V2) * 0.5;

      GeomAdaptor_Surface GA(BRep_Tool::Surface(face));
      Extrema_ExtPS       Ext(P, GA, TolU, TolV, Extrema_ExtFlag_MIN);
      //
      if (Ext.IsDone() && Ext.NbExt() > 0)
      {
        int             i, iNear, iEnd;
        double          aUx, aVx, Dist2, Dist2Min;
        Extrema_POnSurf aPx;
        //
        iNear    = 1;
        Dist2Min = Ext.SquareDistance(1);
        iEnd     = Ext.NbExt();
        for (i = 2; i <= iEnd; i++)
        {
          aPx = Ext.Point(i);
          aPx.Parameter(aUx, aVx);
          if (aUx >= U1 && aUx <= U2 && aVx >= V1 && aVx <= V2)
          {
            Dist2 = Ext.SquareDistance(i);
            if (Dist2 < Dist2Min)
            {
              Dist2Min = Dist2;
              iNear    = i;
            }
          }
        }
        //
        double aDist2Tresh = 1.e-24;
        //
        if (Dist2Min < aDist2Tresh)
        {
          if (anInfFlag)
          {
            return 1;
          }
          else
          {
            BRepClass_FaceClassifier classifier2d;
            double                   aU;
            double                   aV;

            (Ext.Point(iNear)).Parameter(aU, aV);

            gp_Pnt2d aPuv(aU, aV);

            classifier2d.Perform(face, aPuv, Precision::PConfusion());

            TopAbs_State aState = classifier2d.State();

            if (aState == TopAbs_IN || aState == TopAbs_ON)
            {
              return 1;
            }
            else
            {
              return 3; // skv - the point is on surface but outside face.
            }
          }
        }
        if (anInfFlag)
        {
          APoint = (Ext.Point(iNear)).Value();
          gp_Vec V(P, APoint);
          _Par    = V.Magnitude();
          L       = gp_Lin(P, V);
          ptfound = true;
          return 0;
        }

        // set the parameters found by extrema
        aPx = Ext.Point(iNear);
        aPx.Parameter(_u, _v);
        APoint = aPx.Value();
      }
      // The point is not ON the face or surface. The face is restricted.
      //  find point in a face not too far from a projection of P on face
      do
      {
        if (PointInTheFace(face,
                           APoint,
                           _u,
                           _v,
                           myParamOnEdge,
                           ++IndexPoint,
                           surf,
                           U1,
                           V1,
                           U2,
                           V2,
                           aVecD1U,
                           aVecD1V))
        {
          ++NbPointsOK;
          gp_Vec V(P, APoint);
          Par = V.Magnitude();
          if (Par > gp::Resolution() && aVecD1U.Magnitude() > gp::Resolution()
              && aVecD1V.Magnitude() > gp::Resolution())
          {
            gp_Vec Norm = aVecD1U.Crossed(aVecD1V);
            double tt   = Norm.Magnitude();
            if (tt > gp::Resolution())
            {
              tt = std::abs(Norm.Dot(V)) / (tt * Par);
              if (tt > maxscal)
              {
                maxscal = tt;
                L       = gp_Lin(P, V);
                _Par    = Par;
                ptfound = true;
                if (maxscal > 0.2)
                {
                  myParamOnEdge = svmyparam;
                  return 0;
                }
              }
            }
          }
        }
      } while (IndexPoint < 200 && NbPointsOK < 16);

      myParamOnEdge = svmyparam;
      if (maxscal > 0.2)
      {
        return 0;
      }

      //  Modified by skv - Thu Sep  4 14:32:14 2003 OCC578 Begin
      // Next is done in the main for(..) loop.
      //       faceexplorer.Next();
      //  Modified by skv - Thu Sep  4 14:32:14 2003 OCC578 End
      IndexPoint = 0;

      bool encoreuneface = faceexplorer.More();
      if (!ptfound && !encoreuneface)
      {
        if (myParamOnEdge < 0.0001)
        {
          //-- This case takes place when the point is on the solid
          //-- and this solid is reduced to a face
          gp_Pnt PBidon(P.X() + 1.0, P.Y(), P.Z());
          gp_Vec V(P, PBidon);
          Par  = 1.0;
          _Par = Par;
          L    = gp_Lin(P, V);
          return 0;
        }
      }
    } //-- Exploration of the faces

    if (NbFacesInSolid == 0)
    {
      _Par     = 0.0;
      myReject = true;
#ifdef OCCT_DEBUG
      std::cout << "\nWARNING : BRepClass3d_SolidExplorer.cxx  (Solid without face)" << std::endl;
#endif
      return 0;
    }

    if (ptfound)
    {
      return 0;
    }
    myFirstFace = 0;
    if (myParamOnEdge == 0.512345)
      myParamOnEdge = 0.4;
    else if (myParamOnEdge == 0.4)
      myParamOnEdge = 0.6;
    else if (myParamOnEdge == 0.6)
      myParamOnEdge = 0.3;
    else if (myParamOnEdge == 0.3)
      myParamOnEdge = 0.7;
    else if (myParamOnEdge == 0.7)
      myParamOnEdge = 0.2;
    else if (myParamOnEdge == 0.2)
      myParamOnEdge = 0.8;
    else if (myParamOnEdge == 0.8)
      myParamOnEdge = 0.1;
    else if (myParamOnEdge == 0.1)
      myParamOnEdge = 0.9;
    //
    else
    {
      myParamOnEdge *= 0.5;
      if (myParamOnEdge < 0.0001)
      {
        gp_Pnt PBidon(P.X() + 1.0, P.Y(), P.Z());
        gp_Vec V(P, PBidon);
        Par  = 1.0;
        _Par = Par;
        L    = gp_Lin(P, V);
        return 0;
      }
    }
    aTestInvert = true;
  } //-- for(;;) { ...  }
}

//  Modified by skv - Thu Sep  4 12:30:14 2003 OCC578 Begin
//=======================================================================
// function : GetFaceSegmentIndex
// purpose  : Returns the index of face for which last segment is calculated.
//=======================================================================

int BRepClass3d_SolidExplorer::GetFaceSegmentIndex() const
{
  return myFirstFace;
}

//  Modified by skv - Thu Sep  4 12:30:14 2003 OCC578 End

//=================================================================================================

bool BRepClass3d_SolidExplorer::PointInTheFace(const TopoDS_Face& _face,
                                               gp_Pnt&            APoint_,
                                               double&            u_,
                                               double&            v_,
                                               double&            param_,
                                               int&               IndexPoint) const
{
  TopoDS_Face Face = _face;
  Face.Orientation(TopAbs_FORWARD);
  occ::handle<BRepAdaptor_Surface> surf = new BRepAdaptor_Surface();
  surf->Initialize(Face);
  double U1, V1, U2, V2; //,u,v;
  U1 = surf->FirstUParameter();
  V1 = surf->FirstVParameter();
  U2 = surf->LastUParameter();
  V2 = surf->LastVParameter();
  LimitInfiniteUV(U1, V1, U2, V2);
  return (PointInTheFace(Face, APoint_, u_, v_, param_, IndexPoint, surf, U1, V1, U2, V2));
}

//=================================================================================================

bool BRepClass3d_SolidExplorer::FindAPointInTheFace(const TopoDS_Face& _face,
                                                    gp_Pnt&            APoint_,
                                                    double&            u_,
                                                    double&            v_)
{
  double param = 0.1;
  bool   r     = FindAPointInTheFace(_face, APoint_, u_, v_, param);
  return r;
}

//=================================================================================================

bool BRepClass3d_SolidExplorer::FindAPointInTheFace(const TopoDS_Face& _face, gp_Pnt& APoint_)
{
  double u, v;
  bool   r = FindAPointInTheFace(_face, APoint_, u, v);
  return r;
}

//=================================================================================================

bool BRepClass3d_SolidExplorer::FindAPointInTheFace(const TopoDS_Face& _face,
                                                    double&            u_,
                                                    double&            v_)
{
  gp_Pnt APoint;
  bool   r = FindAPointInTheFace(_face, APoint, u_, v_);
  return r;
}

//=================================================================================================

BRepClass3d_SolidExplorer::BRepClass3d_SolidExplorer()
    : myReject(true),
      myFirstFace(0),
      myParamOnEdge(0.0)
{
}

//=================================================================================================

BRepClass3d_SolidExplorer::BRepClass3d_SolidExplorer(const TopoDS_Shape& S)
{
  InitShape(S);
}

//=================================================================================================

BRepClass3d_SolidExplorer::~BRepClass3d_SolidExplorer()
{
  Destroy();
}

//=======================================================================
// function : Destroy
// purpose  : C++: alias ~
//=======================================================================

void BRepClass3d_SolidExplorer::Destroy()
{
  NCollection_DataMap<TopoDS_Shape, void*, TopTools_ShapeMapHasher>::Iterator iter(myMapOfInter);
  for (; iter.More(); iter.Next())
  {
    void* ptr = iter.Value();
    if (ptr)
    {
      delete (IntCurvesFace_Intersector*)ptr;
      myMapOfInter.ChangeFind(iter.Key()) = nullptr;
    }
  }
  myMapOfInter.Clear();
}

//=================================================================================================

void BRepClass3d_SolidExplorer::InitShape(const TopoDS_Shape& S)
{
  myMapEV.Clear();
  myTree.Clear();

  myShape       = S;
  myFirstFace   = 0;
  myParamOnEdge = 0.512345;
  //-- Exploring of the Map and removal of allocated objects

  NCollection_DataMap<TopoDS_Shape, void*, TopTools_ShapeMapHasher>::Iterator iter(myMapOfInter);
  for (; iter.More(); iter.Next())
  {
    void* ptr = iter.Value();
    if (ptr)
    {
      delete (IntCurvesFace_Intersector*)ptr;
      myMapOfInter.ChangeFind(iter.Key()) = nullptr;
    }
  }

  myMapOfInter.Clear();

  myReject = true; //-- case of infinite solid (without any face)

  TopExp_Explorer Expl;
  for (Expl.Init(S, TopAbs_FACE); Expl.More(); Expl.Next())
  {
    const TopoDS_Face Face = TopoDS::Face(Expl.Current());
    void* ptr = (void*)(new IntCurvesFace_Intersector(Face, Precision::Confusion(), true, false));
    myMapOfInter.Bind(Face, ptr);
    myReject = false; //-- at least one face in the solid
  }

#ifdef OCCT_DEBUG
  if (myReject)
  {
    std::cout << "\nWARNING : BRepClass3d_SolidExplorer.cxx  (Solid without face)" << std::endl;
  }
#endif

  // since the internal/external parts should be avoided in tree filler,
  // there is no need to add these parts in the EV map as well
  TopExp_Explorer aExpF(myShape, TopAbs_FACE);
  for (; aExpF.More(); aExpF.Next())
  {
    const TopoDS_Shape& aF = aExpF.Current();
    //
    TopAbs_Orientation anOrF = aF.Orientation();
    if (anOrF == TopAbs_INTERNAL || anOrF == TopAbs_EXTERNAL)
    {
      continue;
    }
    //
    TopExp_Explorer aExpE(aF, TopAbs_EDGE);
    for (; aExpE.More(); aExpE.Next())
    {
      const TopoDS_Shape& aE = aExpE.Current();
      //
      TopAbs_Orientation anOrE = aE.Orientation();
      if (anOrE == TopAbs_INTERNAL || anOrE == TopAbs_EXTERNAL)
      {
        continue;
      }
      //
      if (BRep_Tool::Degenerated(TopoDS::Edge(aE)))
      {
        continue;
      }
      //
      TopExp::MapShapes(aE, myMapEV);
    }
  }
  //
  // Fill mapEV with vertices and edges from shape
  NCollection_UBTreeFiller<int, Bnd_Box> aTreeFiller(myTree);
  //
  int i, aNbEV = myMapEV.Extent();
  for (i = 1; i <= aNbEV; ++i)
  {
    const TopoDS_Shape& aS = myMapEV(i);
    //
    Bnd_Box aBox;
    BRepBndLib::Add(aS, aBox);
    aTreeFiller.Add(i, aBox);
  }
  aTreeFiller.Fill();
}

//=======================================================================
// function : Reject
// purpose  : Should return True if P outside of bounding vol. of the shape
//=======================================================================

// bool  BRepClass3d_SolidExplorer::Reject(const gp_Pnt& P) const
bool BRepClass3d_SolidExplorer::Reject(const gp_Pnt&) const
{
  return (myReject); // case of solid without face
}

//=======================================================================
// function : InitShell
// purpose  : Starts an exploration of the shells.
//=======================================================================

void BRepClass3d_SolidExplorer::InitShell()
{
  myShellExplorer.Init(myShape, TopAbs_SHELL);
}

//=======================================================================
// function : MoreShell
// purpose  : Returns True if there is a current shell.
//=======================================================================

bool BRepClass3d_SolidExplorer::MoreShell() const
{
  return (myShellExplorer.More());
}

//=======================================================================
// function : NextShell
// purpose  : Sets the explorer to the next shell.
//=======================================================================

void BRepClass3d_SolidExplorer::NextShell()
{
  myShellExplorer.Next();
}

//=======================================================================
// function : CurrentShell
// purpose  : Returns the current shell.
//=======================================================================

TopoDS_Shell BRepClass3d_SolidExplorer::CurrentShell() const
{
  return (TopoDS::Shell(myShellExplorer.Current()));
}

//=======================================================================
// function : RejectShell
// purpose  : Returns True if the Shell is rejected.
//=======================================================================

bool BRepClass3d_SolidExplorer::RejectShell(const gp_Lin&) const
{
  return (false);
}

//=======================================================================
// function : InitFace
// purpose  : Starts an exploration of the faces of the current shell.
//=======================================================================

void BRepClass3d_SolidExplorer::InitFace()
{
  myFaceExplorer.Init(TopoDS::Shell(myShellExplorer.Current()), TopAbs_FACE);
}

//=======================================================================
// function : MoreFace
// purpose  : Returns True if current face in current shell.
//=======================================================================

bool BRepClass3d_SolidExplorer::MoreFace() const
{
  return (myFaceExplorer.More());
}

//=======================================================================
// function : NextFace
// purpose  : Sets the explorer to the next Face of the current shell.
//=======================================================================

void BRepClass3d_SolidExplorer::NextFace()
{
  myFaceExplorer.Next();
}

//=======================================================================
// function : CurrentFace
// purpose  : Returns the current face.
//=======================================================================

TopoDS_Face BRepClass3d_SolidExplorer::CurrentFace() const
{
  return (TopoDS::Face(myFaceExplorer.Current()));
}

//=======================================================================
// function : RejectFace
// purpose  : returns True if the face is rejected.
//=======================================================================

bool BRepClass3d_SolidExplorer::RejectFace(const gp_Lin&) const
{
  return (false);
}

//=======================================================================
// function : Segment
// purpose  : Returns  in <L>, <Par>  a segment having at least
//           one  intersection  with  the  shape  boundary  to
//           compute  intersections.
//=======================================================================
int BRepClass3d_SolidExplorer::Segment(const gp_Pnt& P, gp_Lin& L, double& Par)
{
  int bRetFlag;
  myFirstFace = 0;
  bRetFlag    = OtherSegment(P, L, Par);
  return bRetFlag;
}

//=================================================================================================

IntCurvesFace_Intersector& BRepClass3d_SolidExplorer::Intersector(const TopoDS_Face& F) const
{
  void*                      ptr  = (void*)(myMapOfInter.Find(F));
  IntCurvesFace_Intersector& curr = (*((IntCurvesFace_Intersector*)ptr));
  return curr;
}

//=================================================================================================

void BRepClass3d_SolidExplorer::DumpSegment(const gp_Pnt&,
                                            const gp_Lin&,
                                            const double,
                                            const TopAbs_State) const
{
#ifdef OCCT_DEBUG

#endif
}

const TopoDS_Shape& BRepClass3d_SolidExplorer::GetShape() const
{
  return (myShape);
}
