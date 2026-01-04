// Created on: 1996-07-15
// Created by: Laurent BUCHARD
// Copyright (c) 1996-1999 Matra Datavision
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

//  Modified by skv - Thu Sep  4 11:22:05 2003 OCC578

#include <BRep_Tool.hxx>
#include <BRepClass3d_Intersector3d.hxx>
#include <BRepClass3d_SClassifier.hxx>
#include <BRepClass3d_SolidExplorer.hxx>
#include <ElCLib.hxx>
#include <Geom_Surface.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <IntCurvesFace_Intersector.hxx>
#include <math_BullardGenerator.hxx>
#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <Extrema_ExtPS.hxx>

#include <vector>

// modified by NIZHNY-MKK  Mon Jun 21 15:13:40 2004
static bool FaceNormal(const TopoDS_Face& aF, const double U, const double V, gp_Dir& aDN);

static double GetAddToParam(const gp_Lin& L, const double P, const Bnd_Box& B);

// gets transition of line <L> passing through/near the edge <e> of faces <f1>, <f2>. <param> is
//  a parameter on the edge where the minimum distance between <l> and <e> was found
static int GetTransi(const TopoDS_Face&                 f1,
                     const TopoDS_Face&                 f2,
                     const TopoDS_Edge&                 e,
                     double                             param,
                     const gp_Lin&                      L,
                     IntCurveSurface_TransitionOnCurve& trans);

static bool GetNormalOnFaceBound(const TopoDS_Edge& E,
                                 const TopoDS_Face& F,
                                 double             param,
                                 gp_Dir&            OutDir);

static void Trans(double parmin, IntCurveSurface_TransitionOnCurve& tran, int& state);

//=================================================================================================

BRepClass3d_SClassifier::BRepClass3d_SClassifier()
    : myState(0)
{
}

//=================================================================================================

BRepClass3d_SClassifier::BRepClass3d_SClassifier(BRepClass3d_SolidExplorer& S,
                                                 const gp_Pnt&              P,
                                                 const double               Tol)
{
  if (S.Reject(P))
  {
    myState = 3; //-- in ds solid case without face
  }
  else
  {
    Perform(S, P, Tol);
  }
}

//=================================================================================================

void BRepClass3d_SClassifier::PerformInfinitePoint(BRepClass3d_SolidExplorer& aSE,
                                                   const double /*Tol*/)
{

  // Take a normal to the first extracted face in its random inner point
  // and intersect this reversed normal with the faces of the solid.
  // If the min.par.-intersection point is
  //  a) inner point of a face
  //  b) transition is not TANGENT
  //     (the line does not touch the face but pierces it)
  // then set <myState> to IN or OUT according to transition
  // else take the next random point inside the min.par.-intersected face
  // and continue

  if (aSE.Reject(gp_Pnt(0, 0, 0)))
  {
    myState = 3; //-- in ds solid case without face
    return;
  }
  //
  //------------------------------------------------------------
  // 1
  bool   bFound;
  double aParam, aU = 0., aV = 0.;
  gp_Pnt aPoint;
  gp_Dir aDN;

  math_BullardGenerator aRandomGenerator;
  myFace.Nullify();
  myState = 2;

  // Collect faces in sequence to iterate
  std::vector<TopoDS_Face> aFaces;
  for (aSE.InitShell(); aSE.MoreShell(); aSE.NextShell())
  {
    for (aSE.InitFace(); aSE.MoreFace(); aSE.NextFace())
    {
      aFaces.push_back(aSE.CurrentFace());
    }
  }

  // iteratively try up to 10 probing points from each face
  const int NB_MAX_POINTS_PER_FACE = 10;
  for (int itry = 0; itry < NB_MAX_POINTS_PER_FACE; itry++)
  {
    for (std::vector<TopoDS_Face>::iterator iFace = aFaces.begin(); iFace != aFaces.end(); ++iFace)
    {
      TopoDS_Face aF = *iFace;

      TopAbs_State                      aState      = TopAbs_OUT;
      IntCurveSurface_TransitionOnCurve aTransition = IntCurveSurface_Tangent;

      aParam = 0.1 + 0.8 * aRandomGenerator.NextReal(); // random number in range [0.1, 0.9]
      bFound = BRepClass3d_SolidExplorer::FindAPointInTheFace(aF, aPoint, aU, aV, aParam);
      if (!bFound || !FaceNormal(aF, aU, aV, aDN))
        continue;

      gp_Lin aLin(aPoint, -aDN);
      double parmin = RealLast();
      for (aSE.InitShell(); aSE.MoreShell(); aSE.NextShell())
      {
        if (!aSE.RejectShell(aLin))
        {
          for (aSE.InitFace(); aSE.MoreFace(); aSE.NextFace())
          {
            if (!aSE.RejectFace(aLin))
            {
              TopoDS_Shape               aLocalShape   = aSE.CurrentFace();
              TopoDS_Face                CurFace       = TopoDS::Face(aLocalShape);
              IntCurvesFace_Intersector& Intersector3d = aSE.Intersector(CurFace);
              Intersector3d.Perform(aLin, -RealLast(), parmin);

              if (Intersector3d.IsDone())
              {
                if (Intersector3d.NbPnt())
                {
                  int imin = 1;
                  for (int i = 2; i <= Intersector3d.NbPnt(); i++)
                    if (Intersector3d.WParameter(i) < Intersector3d.WParameter(imin))
                      imin = i;
                  parmin      = Intersector3d.WParameter(imin);
                  aState      = Intersector3d.State(imin);
                  aTransition = Intersector3d.Transition(imin);
                }
              }
            }
          }
        }
        else
          myState = 1;
      } // end of loop on the whole solid

      if (aState == TopAbs_IN)
      {
        if (aTransition == IntCurveSurface_Out)
        {
          //-- The line is going from inside the solid to outside
          //-- the solid.
          myState = 3; //-- IN --
          return;
        }
        else if (aTransition == IntCurveSurface_In)
        {
          myState = 4; //-- OUT --
          return;
        }
      }
    } // iteration by faces
  } // iteration by points
}

//=================================================================================================

void BRepClass3d_SClassifier::Perform(BRepClass3d_SolidExplorer& SolidExplorer,
                                      const gp_Pnt&              P,
                                      const double               Tol)
{
  if (SolidExplorer.Reject(P))
  {
    // Solid without faces => the whole space. Always in.
    myState = 3; // IN
    return;
  }

  const NCollection_UBTree<int, Bnd_Box>& aTree = SolidExplorer.GetTree();
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& aMapEV =
    SolidExplorer.GetMapEV();

  // Vertices/Edges vs Point.
  BRepClass3d_BndBoxTreeSelectorPoint aSelectorPoint(aMapEV);
  aSelectorPoint.SetCurrentPoint(P);

  int SelsVE = 0;
  SelsVE     = aTree.Select(aSelectorPoint);

  if (SelsVE > 0)
  {
    // The point P lays inside the tolerance area of vertices/edges => return ON state.
    myState = 2; // ON.
    return;
  }

  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    mapEF;
  TopExp::MapShapesAndAncestors(SolidExplorer.GetShape(), TopAbs_EDGE, TopAbs_FACE, mapEF);

  BRepClass3d_BndBoxTreeSelectorLine aSelectorLine(aMapEV);

  myFace.Nullify();
  myState = 0;

  gp_Lin L;
  double Par;

  // We compute the intersection between the line built in the Solid Explorer and the shape.
  //-- --------------------------------------------------------------------------------
  // Calculate intersection with the face closest to the direction of bounding boxes
  // by priority so that to have the smallest possible parmin.
  // Optimization to produce as much as possible rejections with other faces.
  int iFlag;

  // If found line passes through a bound of any face, it means that the line
  // is not found properly and it is necessary to repeat whole procedure.
  // That's why the main loop while is added.
  bool   isFaultyLine = true;
  int    anIndFace    = 0;
  double parmin       = 0.0;
  while (isFaultyLine)
  {
    if (anIndFace == 0)
      iFlag = SolidExplorer.Segment(P, L, Par);
    else
      iFlag = SolidExplorer.OtherSegment(P, L, Par);

    int aCurInd = SolidExplorer.GetFaceSegmentIndex();

    if (aCurInd > anIndFace)
      anIndFace = aCurInd;
    else
    {
      myState = 1; // Faulty.
      return;
    }

    if (iFlag == 1)
    {
      // IsOnFace iFlag==1 i.e face is Infinite
      myState = 2; // ON.
      return;
    }
    if (iFlag == 2)
    {
      myState = 4; // OUT.
      return;
    }

    // Check if the point is ON surface but OUT of the face.
    // Just skip this face because it is bad for classification.
    if (iFlag == 3)
      continue;

    isFaultyLine = false;
    parmin       = RealLast();

    double NearFaultPar = RealLast(); // Parameter on line.
    aSelectorLine.ClearResults();
    aSelectorLine.SetCurrentLine(L, Par);
    int SelsEVL = 0;
    SelsEVL = aTree.Select(aSelectorLine); // SelsEE > 0 => Line/Edges & Line/Vertex intersection

    if (SelsEVL > 0)
    {
      // Line and edges / vertices interference.
      int           VLInterNb = aSelectorLine.GetNbVertParam();
      TopoDS_Vertex NearIntVert;
      NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> LVInts;
      for (int i = 1; i <= VLInterNb; i++)
      {
        // Line and vertex.
        double        LP = 0;
        TopoDS_Vertex V;
        aSelectorLine.GetVertParam(i, V, LP);

        LVInts.Add(V);
        if (std::abs(LP) < std::abs(NearFaultPar))
          NearFaultPar = LP;
      }

      double      param = 0.0;
      TopoDS_Edge EE;
      double      Lpar = RealLast();
      for (int i = 1; i <= aSelectorLine.GetNbEdgeParam(); i++)
      {
        // Line and edge.
        aSelectorLine.GetEdgeParam(i, EE, param, Lpar);
        const NCollection_List<TopoDS_Shape>& ffs = mapEF.FindFromKey(EE); // ffs size == 2
        if (ffs.Extent() != 2)
          continue;
        TopoDS_Face   f1 = TopoDS::Face(ffs.First());
        TopoDS_Face   f2 = TopoDS::Face(ffs.Last());
        TopoDS_Vertex V1, V2;
        TopExp::Vertices(EE, V1, V2);
        if (LVInts.Contains(V1) || LVInts.Contains(V2))
        {
          continue;
        }

        IntCurveSurface_TransitionOnCurve tran = IntCurveSurface_Tangent;
        int                               Tst  = GetTransi(f1, f2, EE, param, L, tran);
        if (Tst == 1 && std::abs(Lpar) < std::abs(parmin))
        {
          parmin = Lpar;
          Trans(parmin, tran, myState);
        }
        else if (std::abs(Lpar) < std::abs(NearFaultPar))
          NearFaultPar = Lpar;
      }
    }

    for (SolidExplorer.InitShell(); SolidExplorer.MoreShell() && !isFaultyLine;
         SolidExplorer.NextShell())
    {
      if (!SolidExplorer.RejectShell(L))
      {
        for (SolidExplorer.InitFace(); SolidExplorer.MoreFace() && !isFaultyLine;
             SolidExplorer.NextFace())
        {
          if (!SolidExplorer.RejectFace(L))
          {
            TopoDS_Shape               aLocalShape   = SolidExplorer.CurrentFace();
            TopoDS_Face                f             = TopoDS::Face(aLocalShape);
            IntCurvesFace_Intersector& Intersector3d = SolidExplorer.Intersector(f);

            // Prolong segment, since there are cases when
            // the intersector does not find intersection points with the original
            // segment due to rough triangulation of a parameterized surface.
            double addW = std::max(10 * Tol, 0.01 * Par);
            double AddW = addW;

            Bnd_Box aBoxF = Intersector3d.Bounding();

            // The box must be finite in order to correctly prolong the segment to its bounds.
            if (!aBoxF.IsVoid() && !aBoxF.IsWhole())
            {
              double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
              aBoxF.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

              double boxaddW = GetAddToParam(L, Par, aBoxF);
              addW           = std::max(addW, boxaddW);
            }

            double minW = -AddW;
            double maxW = std::min(Par * 10, Par + addW);
            Intersector3d.Perform(L, minW, maxW);
            if (Intersector3d.IsDone())
            {
              if (Intersector3d.NbPnt() == 0)
              {
                if (Intersector3d.IsParallel())
                {
                  // Check distance between surface and point
                  BRepAdaptor_Surface aBAS(f, false);
                  Extrema_ExtPS       aProj(P,
                                      aBAS,
                                      Precision::PConfusion(),
                                      Precision::PConfusion(),
                                      Extrema_ExtFlag_MIN);
                  if (aProj.IsDone() && aProj.NbExt() > 0)
                  {
                    int    i, indmin = 0;
                    double d = RealLast();
                    for (i = 1; i <= aProj.NbExt(); ++i)
                    {
                      if (aProj.SquareDistance(i) < d)
                      {
                        d      = aProj.SquareDistance(i);
                        indmin = i;
                      }
                    }
                    if (indmin > 0)
                    {
                      if (d <= Tol * Tol)
                      {
                        const Extrema_POnSurf& aPonS = aProj.Point(indmin);
                        double                 anU, anV;
                        aPonS.Parameter(anU, anV);
                        gp_Pnt2d     aP2d(anU, anV);
                        TopAbs_State aSt = Intersector3d.ClassifyUVPoint(aP2d);
                        if (aSt == TopAbs_IN || aSt == TopAbs_ON)
                        {
                          myState = 2;
                          myFace  = f;
                          parmin  = 0.;
                          break;
                        }
                      }
                    }
                  }
                }
              }
              for (int i = 1; i <= Intersector3d.NbPnt(); i++)
              {
                if (std::abs(Intersector3d.WParameter(i))
                    < std::abs(parmin) - Precision::PConfusion())
                {
                  parmin              = Intersector3d.WParameter(i);
                  TopAbs_State aState = Intersector3d.State(i);
                  if (std::abs(parmin) <= Tol)
                  {
                    myState = 2;
                    myFace  = f;
                    break;
                  }
                  //  Treatment of case TopAbs_ON separately.
                  else if (aState == TopAbs_IN)
                  {
                    //-- The intersection point between the line and a face F
                    // -- of the solid is in the face F

                    IntCurveSurface_TransitionOnCurve tran = Intersector3d.Transition(i);
                    if (tran == IntCurveSurface_Tangent)
                    {
#ifdef OCCT_DEBUG
                      std::cout << "*Problem ds BRepClass3d_SClassifier.cxx" << std::endl;
#endif
                      continue; // ignore this point
                    }

                    Trans(parmin, tran, myState);
                    myFace = f;
                  }
                  // If the state is TopAbs_ON, it is necessary to chose
                  // another line and to repeat the whole procedure.
                  else if (aState == TopAbs_ON)
                  {
                    isFaultyLine = true;
                    break;
                  }
                }
                else
                {
                  //-- No point has been found by the Intersector3d.
                  //-- Or a Point has been found with a greater parameter.
                }
              } //-- loop by intersection points
              if (myState == 2)
              {
                break;
              }
            } //-- Face has not been rejected
            else
              myState = 1;
          }
        } //-- Exploration of the faces
        if (myState == 2)
        {
          break;
        }
      } //-- Shell has not been rejected
      else
        myState = 1;
    } //-- Exploration of the shells

    if (NearFaultPar != RealLast()
        && std::abs(parmin) >= std::abs(NearFaultPar) - Precision::PConfusion())
    {
      isFaultyLine = true;
    }
  }

#ifdef OCCT_DEBUG
  // #################################################
  SolidExplorer.DumpSegment(P, L, parmin, State());
  // #################################################
#endif
}

TopAbs_State BRepClass3d_SClassifier::State() const
{
  if (myState == 2)
    return (TopAbs_ON);
  else if (myState == 3)
    return (TopAbs_IN);
  else if (myState == 4)
    return (TopAbs_OUT);

  // return OUT state when there is an error during execution.
  return (TopAbs_OUT);
}

TopoDS_Face BRepClass3d_SClassifier::Face() const
{
  return (myFace);
}

bool BRepClass3d_SClassifier::Rejected() const
{
  return (myState == 1);
}

bool BRepClass3d_SClassifier::IsOnAFace() const
{
  return (myState == 2);
}

void BRepClass3d_SClassifier::ForceIn()
{
  myState = 3;
}

void BRepClass3d_SClassifier::ForceOut()
{
  myState = 4;
}

double GetAddToParam(const gp_Lin& L, const double P, const Bnd_Box& B)
{
  double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  B.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
  double x[2] = {aXmin, aXmax}, y[2] = {aYmin, aYmax}, z[2] = {aZmin, aZmax};
  int    i = 0, j = 0, k = 0;
  double Par = P;
  for (i = 0; i < 2; i++)
  {
    for (j = 0; j < 2; j++)
    {
      for (k = 0; k < 2; k++)
      {
        double X = fabs(x[i] - L.Location().X());
        double Y = fabs(y[j] - L.Location().Y());
        double Z = fabs(z[k] - L.Location().Z());
        if (X < 1.e+20 && Y < 1.e+20 && Z < 1.e+20)
        {
          gp_Pnt aP(x[i], y[j], z[k]);
          double par = ElCLib::Parameter(L, aP);
          if (par > Par)
            Par = par;
        }
        else
          return 1.e+20;
      }
    }
  }
  return Par - P;
}

//=================================================================================================

bool FaceNormal(const TopoDS_Face& aF, const double U, const double V, gp_Dir& aDN)
{
  gp_Pnt                    aPnt;
  gp_Vec                    aD1U, aD1V, aN;
  occ::handle<Geom_Surface> aS;

  aS = BRep_Tool::Surface(aF);
  aS->D1(U, V, aPnt, aD1U, aD1V);
  aN = aD1U.Crossed(aD1V);
  if (aN.Magnitude() <= gp::Resolution())
    return false;

  aN.Normalize();
  aDN.SetXYZ(aN.XYZ());
  if (aF.Orientation() == TopAbs_REVERSED)
  {
    aDN.Reverse();
  }
  return true;
}

//=================================================================================================

static bool GetNormalOnFaceBound(const TopoDS_Edge& E,
                                 const TopoDS_Face& F,
                                 const double       param,
                                 gp_Dir&            OutDir)
{
  double f = 0, l = 0;

  gp_Pnt2d                  P2d;
  occ::handle<Geom2d_Curve> c2d = BRep_Tool::CurveOnSurface(E, F, f, l);
  if (c2d.IsNull())
    return false;
  if (param < f || param > l)
    return false;
  c2d->D0(param, P2d);
  return FaceNormal(F, P2d.X(), P2d.Y(), OutDir);
}

//=================================================================================================

static int GetTransi(const TopoDS_Face&                 f1,
                     const TopoDS_Face&                 f2,
                     const TopoDS_Edge&                 e,
                     const double                       param,
                     const gp_Lin&                      L,
                     IntCurveSurface_TransitionOnCurve& trans)
{
  // return statuses:
  // 1 => OK
  // 0 => skip
  //-1 => probably a faulty line
  gp_Dir nf1, nf2;
  if (!GetNormalOnFaceBound(e, f1, param, nf1))
    return -1;
  if (!GetNormalOnFaceBound(e, f2, param, nf2))
    return -1;

  const gp_Dir& LDir = L.Direction();

  if (std::abs(LDir.Dot(nf1)) < Precision::Angular()
      || std::abs(LDir.Dot(nf2)) < Precision::Angular())
  {
    // line is orthogonal to normal(s)
    // trans = IntCurveSurface_Tangent;
    return -1;
  }

  if (nf1.IsParallel(nf2, Precision::Angular()))
  {
    double angD = nf1.Dot(LDir);
    if (std::abs(angD) < Precision::Angular())
      return -1;
    else if (angD > 0)
      trans = IntCurveSurface_Out;
    else // angD < -Precision::Angular())
      trans = IntCurveSurface_In;
    return 1;
  }

  gp_Vec N = nf1 ^ nf2;
  // clang-format off
  gp_Dir ProjL = N.XYZ() ^ LDir.XYZ() ^ N.XYZ(); //proj LDir on the plane defined by nf1/nf2 directions
  // clang-format on

  double fAD = nf1.Dot(ProjL);
  double sAD = nf2.Dot(ProjL);

  if (fAD < -Precision::Angular() && sAD < -Precision::Angular())
    trans = IntCurveSurface_In;
  else if (fAD > Precision::Angular() && sAD > Precision::Angular())
    trans = IntCurveSurface_Out;
  else
    return 0;
  return 1;
}

//=================================================================================================

static void Trans(const double parmin, IntCurveSurface_TransitionOnCurve& tran, int& state)
{
  // if parmin is negative we should reverse transition
  if (parmin < 0)
    tran = (tran == IntCurveSurface_Out ? IntCurveSurface_In : IntCurveSurface_Out);

  if (tran == IntCurveSurface_Out)
    //-- The line is going from inside the solid to outside
    //-- the solid.
    state = 3; // IN
  else
    state = 4; // OUT
}
