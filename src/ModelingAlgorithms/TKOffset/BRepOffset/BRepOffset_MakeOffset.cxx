// Created on: 1995-10-27
// Created by: Yves FRICAUD
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

//  Modified by skv - Tue Mar 15 16:20:43 2005
// Add methods for supporting history.
//  Modified by skv - Mon Jan 12 11:50:02 2004 OCC4455

#include <Adaptor3d_CurveOnSurface.hxx>
#include <BRep_Builder.hxx>
#include <BRep_PointRepresentation.hxx>
#include <NCollection_List.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_Tool.hxx>
#include <BRep_TVertex.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Curve2d.hxx>
#include <BRepAlgo_AsDes.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepCheck_Edge.hxx>
#include <BRepCheck_Vertex.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <BRepGProp.hxx>
#include <BRepLib.hxx>
#include <BRepLib_FindSurface.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <BRepLib_MakeFace.hxx>
#include <BRepLib_MakeVertex.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_Map.hxx>
#include <NCollection_DataMap.hxx>
#include <BRepOffset_Inter2d.hxx>
#include <BRepOffset_Inter3d.hxx>
#include <BRepOffset_MakeOffset.hxx>
#include <BRepOffset_Offset.hxx>
#include <BRepOffset_Tool.hxx>
#include <BRepTools.hxx>
#include <BRepTools_Quilt.hxx>
#include <BRepTools_Substitution.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <ElCLib.hxx>
#include <ElSLib.hxx>
#include <GC_MakeCylindricalSurface.hxx>
#include <GCE2d_MakeLine.hxx>
#include <gce_MakeCone.hxx>
#include <gce_MakeDir.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <GeomFill_Generator.hxx>
#include <GeomLib.hxx>
#include <GeomLib_IsPlanarSurface.hxx>
#include <gp_Cone.hxx>
#include <gp_Pnt.hxx>
#include <GProp_GProps.hxx>
#include <IntTools_FClass2d.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_NotImplemented.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <NCollection_IndexedMap.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <Geom_Line.hxx>
#include <NCollection_Vector.hxx>
#include <NCollection_IncAllocator.hxx>
//
#include <BOPAlgo_MakerVolume.hxx>
#include <BOPTools_AlgoTools.hxx>

#include <stdio.h>
// POP for NT
#ifdef DRAW

  #include <DBRep.hxx>
#endif
#ifdef OCCT_DEBUG
  #include <OSD_Chronometer.hxx>
// #define DEB_VERB
bool                   AffichInt2d = false;
bool                   AffichOffC  = false;
bool                   ChronBuild  = false;
int                    NbAE        = 0;
int                    NbAF        = 0;
int                    NVP         = 0;
int                    NVM         = 0;
int                    NVN         = 0;
static OSD_Chronometer Clock;
char                   name[100];

//=================================================================================================

static void DEBVerticesControl(
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& NewEdges,
  occ::handle<BRepAlgo_AsDes>                                          AsDes)
{
  NCollection_List<TopoDS_Shape>           LVP;
  NCollection_List<TopoDS_Shape>::Iterator it1LE;
  NCollection_List<TopoDS_Shape>::Iterator it2LE;

  int i;
  for (i = 1; i <= NewEdges.Extent(); i++)
  {
    const TopoDS_Edge& NE = TopoDS::Edge(NewEdges(i));
    if (AsDes->HasDescendant(NE))
    {
      for (it1LE.Initialize(AsDes->Descendant(NE)); it1LE.More(); it1LE.Next())
      {
        if (AsDes->Ascendant(it1LE.Value()).Extent() < 3)
        {
          LVP.Append(it1LE.Value());
          std::cout << "Vertex on at least 3 edges." << std::endl;
  #ifdef DRAW
          if (AffichInt2d)
          {
            Sprintf(name, "VP_%d", NVP++);
            DBRep::Set(name, it1LE.Value());
          }
  #endif
        }
        else if (AsDes->Ascendant(it1LE.Value()).Extent() > 3)
        {
          std::cout << "Vertex on more than 3 edges." << std::endl;
  #ifdef DRAW
          if (AffichInt2d)
          {
            Sprintf(name, "VM_%d", NVM++);
            DBRep::Set(name, it1LE.Value());
          }
  #endif
        }
        else
        {
  #ifdef DRAW
          if (AffichInt2d)
          {
            Sprintf(name, "VN_%d", NVN++);
            DBRep::Set(name, it1LE.Value());
          }
  #endif
        }
      }
    }
  }
  //------------------------------------------------
  // Try to mix spoiled vertices.
  //------------------------------------------------
  BRep_Builder                             B;
  NCollection_List<TopoDS_Shape>::Iterator it1(LVP);
  double                                   TolConf = 1.e-5;
  double                                   Tol     = Precision::Confusion();
  // int                   i = 1;

  i = 1;
  for (; it1.More(); it1.Next())
  {
    TopoDS_Shape                             V1      = it1.Value();
    gp_Pnt                                   P1      = BRep_Tool::Pnt(TopoDS::Vertex(V1));
    double                                   distmin = Precision::Infinite();
    NCollection_List<TopoDS_Shape>::Iterator it2(LVP);
    int                                      j = 1;

    for (; it2.More(); it2.Next())
    {
      if (j > i)
      {
        TopoDS_Shape V2 = it2.Value();
        gp_Pnt       P2 = BRep_Tool::Pnt(TopoDS::Vertex(V2));
        if (!V1.IsSame(V2))
        {
          double dist = P1.Distance(P2);
          if (dist < distmin)
            distmin = dist;
          if (dist < TolConf)
          {
            double                                   UV2;
            TopoDS_Edge                              EWE2;
            const NCollection_List<TopoDS_Shape>&    EdgeWithV2 = AsDes->Ascendant(V2);
            NCollection_List<TopoDS_Shape>::Iterator itAsDes;
            for (itAsDes.Initialize(EdgeWithV2); itAsDes.More(); itAsDes.Next())
            {
              EWE2                     = TopoDS::Edge(itAsDes.Value());
              TopoDS_Shape aLocalShape = V2.Oriented(TopAbs_INTERNAL);
              UV2                      = BRep_Tool::Parameter(TopoDS::Vertex(aLocalShape), EWE2);
              aLocalShape              = V1.Oriented(TopAbs_INTERNAL);
              B.UpdateVertex(TopoDS::Vertex(aLocalShape), UV2, EWE2, Tol);
              //              UV2   =
              //                BRep_Tool::Parameter(TopoDS::Vertex(),EWE2);
              //              B.UpdateVertex(TopoDS::Vertex(V1.Oriented(TopAbs_INTERNAL)),
              //                             UV2,EWE2,Tol);
            }
            AsDes->Replace(V2, V1);
          }
        }
      }
      j++;
    }
    i++;
    std::cout << " distmin between VP : " << distmin << std::endl;
  }
}
#endif

namespace
{
//=======================================================================
// function : BRepOffset_PIOperation
// purpose  : List of operations to be supported by the Progress Indicator
//=======================================================================
enum BRepOffset_PIOperation
{
  PIOperation_CheckInputData = 0,
  PIOperation_Analyse,
  PIOperation_BuildOffsetBy,
  PIOperation_Intersection,
  PIOperation_MakeMissingWalls,
  PIOperation_MakeShells,
  PIOperation_MakeSolid,
  PIOperation_Sewing,
  PIOperation_Last
};

//=======================================================================
// function : normalizeSteps
// purpose  : Normalization of progress steps
//=======================================================================
static void normalizeSteps(const double theWhole, NCollection_Array1<double>& theSteps)
{
  double aSum = 0.;
  for (int i = theSteps.Lower(); i <= theSteps.Upper(); ++i)
  {
    aSum += theSteps(i);
  }

  // Normalize steps
  for (int i = theSteps.Lower(); i <= theSteps.Upper(); ++i)
  {
    theSteps(i) = theWhole * theSteps(i) / aSum;
  }
}

} // namespace

//=======================================================================
// static methods
//=======================================================================
static void GetEnlargedFaces(
  const NCollection_List<TopoDS_Shape>&                                                theFaces,
  const NCollection_DataMap<TopoDS_Shape, BRepOffset_Offset, TopTools_ShapeMapHasher>& theMapSF,
  const NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>&      theMES,
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& theFacesOrigins,
  BRepAlgo_Image&                                                           theImage,
  NCollection_List<TopoDS_Shape>&                                           theLSF);

static bool BuildShellsCompleteInter(const NCollection_List<TopoDS_Shape>& theLF,
                                     BRepAlgo_Image&                       theImage,
                                     TopoDS_Shape&                         theShells,
                                     const Message_ProgressRange&          theRange);

static bool GetSubShapes(const TopoDS_Shape&    theShape,
                         const TopAbs_ShapeEnum theSSType,
                         TopoDS_Shape&          theResult);

static void UpdateInitOffset(BRepAlgo_Image&         myInitOffset,
                             BRepAlgo_Image&         myImageOffset,
                             const TopoDS_Shape&     myOffsetShape,
                             const TopAbs_ShapeEnum& theShapeType);

static void RemoveShapes(TopoDS_Shape& theS, const NCollection_List<TopoDS_Shape>& theLS);

static bool IsSolid(const TopoDS_Shape& theS);

static void UpdateHistory(const NCollection_List<TopoDS_Shape>& theLF,
                          BOPAlgo_Builder&                      theGF,
                          BRepAlgo_Image&                       theImage);

static void RemoveSeamAndDegeneratedEdges(const TopoDS_Face& theFace,
                                          const TopoDS_Face& theOldFace);

static bool TrimEdge(
  TopoDS_Edge&                                                              NE,
  const occ::handle<BRepAlgo_AsDes>&                                        AsDes2d,
  occ::handle<BRepAlgo_AsDes>&                                              AsDes,
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& theETrimEInf);

static bool TrimEdges(
  const TopoDS_Shape&                                                            theShape,
  const double                                                                   theOffset,
  const BRepOffset_Analyse&                                                      Analyse,
  NCollection_DataMap<TopoDS_Shape, BRepOffset_Offset, TopTools_ShapeMapHasher>& theMapSF,
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>&      theMES,
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>&      theBuild,
  occ::handle<BRepAlgo_AsDes>&                                                   theAsDes,
  occ::handle<BRepAlgo_AsDes>&                                                   theAsDes2d,
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>&                 theNewEdges,
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>&      theETrimEInf,
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    theEdgesOrigins);

static void AppendToList(NCollection_List<TopoDS_Shape>& theL, const TopoDS_Shape& theS);

static BRepOffset_Error checkSinglePoint(const double                      theUParam,
                                         const double                      theVParam,
                                         const occ::handle<Geom_Surface>&  theSurf,
                                         const NCollection_Vector<gp_Pnt>& theBadPoints);

//---------------------------------------------------------------------
static void UpdateTolerance(
  TopoDS_Shape&                                                        theShape,
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theFaces,
  const TopoDS_Shape&                                                  theInitShape);

static double ComputeMaxDist(const gp_Pln&                  thePlane,
                             const occ::handle<Geom_Curve>& theCrv,
                             const double                   theFirst,
                             const double                   theLast);

static void CorrectSolid(TopoDS_Solid& theSol, NCollection_List<TopoDS_Shape>& theSolList);

//---------------------------------------------------------------------

static TopAbs_Orientation OrientationOfEdgeInFace(const TopoDS_Edge& theEdge,
                                                  const TopoDS_Face& theFace)
{
  TopAbs_Orientation anOr = TopAbs_EXTERNAL;

  TopExp_Explorer Explo(theFace, TopAbs_EDGE);
  for (; Explo.More(); Explo.Next())
  {
    const TopoDS_Shape& anEdge = Explo.Current();
    if (anEdge.IsSame(theEdge))
    {
      anOr = anEdge.Orientation();
      break;
    }
  }

  return anOr;
}

//
static bool FindParameter(const TopoDS_Vertex& V, const TopoDS_Edge& E, double& U)
{
  // Search the vertex in the edge

  bool               rev = false;
  TopoDS_Shape       VF;
  TopAbs_Orientation orient = TopAbs_INTERNAL;

  TopoDS_Iterator itv(E.Oriented(TopAbs_FORWARD));

  // if the edge has no vertices
  // and is degenerated use the vertex orientation
  // RLE, june 94

  if (!itv.More() && BRep_Tool::Degenerated(E))
  {
    orient = V.Orientation();
  }

  while (itv.More())
  {
    const TopoDS_Shape& Vcur = itv.Value();
    if (V.IsSame(Vcur))
    {
      if (VF.IsNull())
      {
        VF = Vcur;
      }
      else
      {
        rev = E.Orientation() == TopAbs_REVERSED;
        if (Vcur.Orientation() == V.Orientation())
        {
          VF = Vcur;
        }
      }
    }
    itv.Next();
  }

  if (!VF.IsNull())
    orient = VF.Orientation();

  double f, l;

  if (orient == TopAbs_FORWARD)
  {
    BRep_Tool::Range(E, f, l);
    // return (rev) ? l : f;
    U = (rev) ? l : f;
    return true;
  }

  else if (orient == TopAbs_REVERSED)
  {
    BRep_Tool::Range(E, f, l);
    // return (rev) ? f : l;
    U = (rev) ? f : l;
    return true;
  }

  else
  {
    TopLoc_Location                L;
    const occ::handle<Geom_Curve>& C = BRep_Tool::Curve(E, L, f, l);
    L                                = L.Predivided(V.Location());
    if (!C.IsNull() || BRep_Tool::Degenerated(E))
    {
      NCollection_List<occ::handle<BRep_PointRepresentation>>::Iterator itpr(
        (*((occ::handle<BRep_TVertex>*)&V.TShape()))->Points());

      while (itpr.More())
      {
        const occ::handle<BRep_PointRepresentation>& pr = itpr.Value();
        if (pr->IsPointOnCurve(C, L))
        {
          double p   = pr->Parameter();
          double res = p; // SVV 4 nov 99 - to avoid warnings on Linux
          if (!C.IsNull())
          {
            // Closed curves RLE 16 june 94
            if (Precision::IsNegativeInfinite(f))
            {
              // return pr->Parameter();//p;
              U = pr->Parameter();
              return true;
            }
            if (Precision::IsPositiveInfinite(l))
            {
              // return pr->Parameter();//p;
              U = pr->Parameter();
              return true;
            }
            gp_Pnt Pf  = C->Value(f).Transformed(L.Transformation());
            gp_Pnt Pl  = C->Value(l).Transformed(L.Transformation());
            double tol = BRep_Tool::Tolerance(V);
            if (Pf.Distance(Pl) < tol)
            {
              if (Pf.Distance(BRep_Tool::Pnt(V)) < tol)
              {
                if (V.Orientation() == TopAbs_FORWARD)
                  res = f; // p = f;
                else
                  res = l; // p = l;
              }
            }
          }
          // return res;//p;
          U = res;
          return true;
        }
        itpr.Next();
      }
    }
    else
    {
      // no 3d curve !!
      // let us try with the first pcurve
      occ::handle<Geom2d_Curve> PC;
      occ::handle<Geom_Surface> S;
      BRep_Tool::CurveOnSurface(E, PC, S, L, f, l);
      L = L.Predivided(V.Location());
      NCollection_List<occ::handle<BRep_PointRepresentation>>::Iterator itpr(
        (*((occ::handle<BRep_TVertex>*)&V.TShape()))->Points());

      while (itpr.More())
      {
        const occ::handle<BRep_PointRepresentation>& pr = itpr.Value();
        if (pr->IsPointOnCurveOnSurface(PC, S, L))
        {
          double p = pr->Parameter();
          // Closed curves RLE 16 june 94
          if (PC->IsClosed())
          {
            if ((p == PC->FirstParameter()) || (p == PC->LastParameter()))
            {
              if (V.Orientation() == TopAbs_FORWARD)
                p = PC->FirstParameter();
              else
                p = PC->LastParameter();
            }
          }
          // return p;
          U = p;
          return true;
        }
        itpr.Next();
      }
    }
  }

  // throw Standard_NoSuchObject("BRep_Tool:: no parameter on edge");
  return false;
}

//=======================================================================
// function : GetEdgePoints
// purpose  : gets the first, last and middle points of the edge
//=======================================================================
static void GetEdgePoints(const TopoDS_Edge& anEdge,
                          const TopoDS_Face& aFace,
                          gp_Pnt&            fPnt,
                          gp_Pnt&            mPnt,
                          gp_Pnt&            lPnt)
{
  double                    f, l;
  occ::handle<Geom2d_Curve> theCurve = BRep_Tool::CurveOnSurface(anEdge, aFace, f, l);
  gp_Pnt2d                  fPnt2d   = theCurve->Value(f);
  gp_Pnt2d                  lPnt2d   = theCurve->Value(l);
  gp_Pnt2d                  mPnt2d   = theCurve->Value(0.5 * (f + l));
  occ::handle<Geom_Surface> aSurf    = BRep_Tool::Surface(aFace);
  fPnt                               = aSurf->Value(fPnt2d.X(), fPnt2d.Y());
  lPnt                               = aSurf->Value(lPnt2d.X(), lPnt2d.Y());
  mPnt                               = aSurf->Value(mPnt2d.X(), mPnt2d.Y());
}

//=======================================================================
// function : FillContours
// purpose  : fills free boundary contours and faces connected (MapEF)
//=======================================================================
static void FillContours(
  const TopoDS_Shape&       aShape,
  const BRepOffset_Analyse& Analyser,
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
                                                                            Contours,
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& MapEF)
{
  NCollection_List<TopoDS_Shape> Edges;

  TopExp_Explorer        Explo(aShape, TopAbs_FACE);
  BRepTools_WireExplorer Wexp;

  for (; Explo.More(); Explo.Next())
  {
    TopoDS_Face     aFace = TopoDS::Face(Explo.Current());
    TopoDS_Iterator itf(aFace);
    for (; itf.More(); itf.Next())
    {
      TopoDS_Wire aWire = TopoDS::Wire(itf.Value());
      for (Wexp.Init(aWire, aFace); Wexp.More(); Wexp.Next())
      {
        const TopoDS_Edge& anEdge = Wexp.Current();
        if (BRep_Tool::Degenerated(anEdge))
          continue;
        const NCollection_List<BRepOffset_Interval>& Lint = Analyser.Type(anEdge);
        if (!Lint.IsEmpty() && Lint.First().Type() == ChFiDS_FreeBound)
        {
          MapEF.Bind(anEdge, aFace);
          Edges.Append(anEdge);
        }
      }
    }
  }

  NCollection_List<TopoDS_Shape>::Iterator itl;
  while (!Edges.IsEmpty())
  {
    TopoDS_Edge StartEdge = TopoDS::Edge(Edges.First());
    Edges.RemoveFirst();
    TopoDS_Vertex StartVertex, CurVertex;
    TopExp::Vertices(StartEdge, StartVertex, CurVertex, true);
    NCollection_List<TopoDS_Shape> aContour;
    aContour.Append(StartEdge);
    while (!CurVertex.IsSame(StartVertex))
      for (itl.Initialize(Edges); itl.More(); itl.Next())
      {
        TopoDS_Edge   anEdge = TopoDS::Edge(itl.Value());
        TopoDS_Vertex V1, V2;
        TopExp::Vertices(anEdge, V1, V2);
        if (V1.IsSame(CurVertex) || V2.IsSame(CurVertex))
        {
          aContour.Append(anEdge);
          CurVertex = (V1.IsSame(CurVertex)) ? V2 : V1;
          Edges.Remove(itl);
          break;
        }
      }
    Contours.Add(StartVertex, aContour);
  }
}

//
//-----------------------------------------------------------------------
//
//=================================================================================================

BRepOffset_MakeOffset::BRepOffset_MakeOffset()
{
  myAsDes = new BRepAlgo_AsDes();
}

//=================================================================================================

BRepOffset_MakeOffset::BRepOffset_MakeOffset(const TopoDS_Shape&          S,
                                             const double                 Offset,
                                             const double                 Tol,
                                             const BRepOffset_Mode        Mode,
                                             const bool                   Inter,
                                             const bool                   SelfInter,
                                             const GeomAbs_JoinType       Join,
                                             const bool                   Thickening,
                                             const bool                   RemoveIntEdges,
                                             const Message_ProgressRange& theRange)
    : myOffset(Offset),
      myTol(Tol),
      myInitialShape(S),
      myShape(S),
      myMode(Mode),
      myInter(Inter),
      mySelfInter(SelfInter),
      myJoin(Join),
      myThickening(Thickening),
      myRemoveIntEdges(RemoveIntEdges),
      myDone(false)
{
  myAsDes                  = new BRepAlgo_AsDes();
  myIsLinearizationAllowed = true;

  MakeOffsetShape(theRange);
}

//=================================================================================================

void BRepOffset_MakeOffset::Initialize(const TopoDS_Shape&    S,
                                       const double           Offset,
                                       const double           Tol,
                                       const BRepOffset_Mode  Mode,
                                       const bool             Inter,
                                       const bool             SelfInter,
                                       const GeomAbs_JoinType Join,
                                       const bool             Thickening,
                                       const bool             RemoveIntEdges)
{
  myOffset                 = Offset;
  myInitialShape           = S;
  myShape                  = S;
  myTol                    = Tol;
  myMode                   = Mode;
  myInter                  = Inter;
  mySelfInter              = SelfInter;
  myJoin                   = Join;
  myThickening             = Thickening;
  myRemoveIntEdges         = RemoveIntEdges;
  myIsLinearizationAllowed = true;
  myDone                   = false;
  myIsPerformSewing        = false;
  myIsPlanar               = false;
  Clear();
}

//=================================================================================================

void BRepOffset_MakeOffset::Clear()
{
  myOffsetShape.Nullify();
  myInitOffsetFace.Clear();
  myInitOffsetEdge.Clear();
  myImageOffset.Clear();
  myImageVV.Clear();
  myFaces.Clear();
  myOriginalFaces.Clear();
  myFaceOffset.Clear();
  myEdgeIntEdges.Clear();
  myAsDes->Clear();
  myDone = false;
  myGenerated.Clear();
  myResMap.Clear();
}

//=================================================================================================

void BRepOffset_MakeOffset::AllowLinearization(const bool theIsAllowed)
{
  myIsLinearizationAllowed = theIsAllowed;
}

//=================================================================================================

void BRepOffset_MakeOffset::AddFace(const TopoDS_Face& F)
{

  myOriginalFaces.Add(F);
}

//=================================================================================================

void BRepOffset_MakeOffset::SetOffsetOnFace(const TopoDS_Face& F, const double Off)
{
  myFaceOffset.Bind(F, Off);
}

//=================================================================================================

static void RemoveCorks(TopoDS_Shape&                                                  S,
                        NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& Faces)
{
  TopoDS_Compound SS;
  BRep_Builder    B;
  B.MakeCompound(SS);
  //-----------------------------------------------------
  // Construction of a shape without caps.
  // and Orientation of caps as in shape S.
  //-----------------------------------------------------
  TopExp_Explorer exp(S, TopAbs_FACE);
  for (; exp.More(); exp.Next())
  {
    const TopoDS_Shape& Cork = exp.Current();
    if (!Faces.Contains(Cork))
    {
      B.Add(SS, Cork);
    }
    else
    {
      Faces.RemoveKey(Cork);
      Faces.Add(Cork); // to reset it with proper orientation.
    }
  }
  S = SS;
#ifdef DRAW
  if (AffichOffC)
    DBRep::Set("myInit", SS);
#endif
}

//=================================================================================================

static bool IsConnectedShell(const TopoDS_Shape& S)
{
  BRepTools_Quilt Glue;
  Glue.Add(S);

  TopoDS_Shape    SS = Glue.Shells();
  TopExp_Explorer Explo(SS, TopAbs_SHELL);
  Explo.Next();
  if (Explo.More())
    return false;

  return true;
}

//=================================================================================================

static void MakeList(NCollection_List<TopoDS_Shape>& OffsetFaces,
                     const BRepAlgo_Image&           myInitOffsetFace,
                     const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& myFaces)
{
  NCollection_List<TopoDS_Shape>::Iterator itLOF(myInitOffsetFace.Roots());
  for (; itLOF.More(); itLOF.Next())
  {
    const TopoDS_Shape& Root = itLOF.Value();
    if (!myFaces.Contains(Root))
    {
      if (myInitOffsetFace.HasImage(Root))
      {
        NCollection_List<TopoDS_Shape>::Iterator aItLS(myInitOffsetFace.Image(Root));
        for (; aItLS.More(); aItLS.Next())
        {
          OffsetFaces.Append(aItLS.Value());
        }
      }
    }
  }
}

//=================================================================================================

static void EvalMax(const TopoDS_Shape& S, double& Tol)
{
  TopExp_Explorer exp;
  for (exp.Init(S, TopAbs_VERTEX); exp.More(); exp.Next())
  {
    const TopoDS_Vertex& V    = TopoDS::Vertex(exp.Current());
    double               TolV = BRep_Tool::Tolerance(V);
    if (TolV > Tol)
      Tol = TolV;
  }
}

//=================================================================================================

void BRepOffset_MakeOffset::SetFaces()
{
  for (int ii = 1; ii <= myOriginalFaces.Extent(); ii++)
  {
    TopoDS_Face         aFace     = TopoDS::Face(myOriginalFaces(ii));
    const TopoDS_Shape* aPlanface = myFacePlanfaceMap.Seek(aFace);
    if (aPlanface)
      aFace = TopoDS::Face(*aPlanface);

    myFaces.Add(aFace);
    //-------------
    // MAJ SD.
    //-------------
    myInitOffsetFace.SetRoot(aFace);
    myInitOffsetFace.Bind(aFace, aFace);
    myImageOffset.SetRoot(aFace);
  }
}

//=================================================================================================

void BRepOffset_MakeOffset::SetFacesWithOffset()
{
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator anItmap(
    myFacePlanfaceMap);
  for (; anItmap.More(); anItmap.Next())
  {
    TopoDS_Face aFace     = TopoDS::Face(anItmap.Key());
    TopoDS_Face aPlanface = TopoDS::Face(anItmap.Value());
    if (myFaceOffset.IsBound(aFace))
    {
      double anOffset = myFaceOffset(aFace);
      myFaceOffset.UnBind(aFace);
      myFaceOffset.Bind(aPlanface, anOffset);
    }
  }
}

//=================================================================================================

void BRepOffset_MakeOffset::MakeOffsetShape(const Message_ProgressRange& theRange)
{
  myDone = false;
  //

  // check if shape consists of only planar faces
  myIsPlanar = IsPlanar();

  SetFaces();
  SetFacesWithOffset();

  BuildFaceComp();

  //------------------------------------------
  // Construction of myShape without caps.
  //------------------------------------------
  if (!myFaces.IsEmpty())
  {
    RemoveCorks(myShape, myOriginalFaces);
    RemoveCorks(myFaceComp, myFaces);
  }

  Message_ProgressScope aPS(theRange, "Making offset shape", 100);

  NCollection_Array1<double> aSteps(0, PIOperation_Last - 1);
  analyzeProgress(100., aSteps);

  if (!CheckInputData(aPS.Next(aSteps(PIOperation_CheckInputData)))
      || myError != BRepOffset_NoError)
  {
    // There is error in input data.
    // Check Error() method.
    return;
  }
  myError           = BRepOffset_NoError;
  TopAbs_State Side = TopAbs_IN;
  if (myOffset < 0.)
    Side = TopAbs_OUT;

  // ------------
  // Preanalyse.
  // ------------
  EvalMax(myShape, myTol);
  // There are possible second variant: analytical continuation of arcsin.
  double TolAngleCoeff = std::min(myTol / (std::abs(myOffset * 0.5) + Precision::Confusion()), 1.0);
  double TolAngle      = 4 * std::asin(TolAngleCoeff);
  if ((myJoin == GeomAbs_Intersection) && myInter && myIsPlanar)
  {
    myAnalyse.SetOffsetValue(myOffset);
    myAnalyse.SetFaceOffsetMap(myFaceOffset);
  }
  myAnalyse.Perform(myFaceComp, TolAngle, aPS.Next(aSteps(PIOperation_Analyse)));
  TopExp_Explorer anEExp(myFaceComp, TopAbs_EDGE);
  for (; anEExp.More(); anEExp.Next())
  {
    const TopoDS_Edge&                           anE = TopoDS::Edge(anEExp.Current());
    const NCollection_List<BRepOffset_Interval>& aLI = myAnalyse.Type(anE);
    if (aLI.IsEmpty())
      continue;
    if (aLI.Last().Type() == ChFiDS_Mixed)
    {
      myError = BRepOffset_MixedConnectivity;
      return;
    }
  }
  if (!aPS.More())
  {
    myError = BRepOffset_UserBreak;
    return;
  }
  //---------------------------------------------------
  // Construction of Offset from preanalysis.
  //---------------------------------------------------
  //----------------------------
  // MaJ of SD Face - Offset
  //----------------------------
  UpdateFaceOffset();

  if (myJoin == GeomAbs_Arc)
    BuildOffsetByArc(aPS.Next(aSteps(PIOperation_BuildOffsetBy)));
  else if (myJoin == GeomAbs_Intersection)
    BuildOffsetByInter(aPS.Next(aSteps(PIOperation_BuildOffsetBy)));
  if (myError != BRepOffset_NoError)
  {
    return;
  }
  //-----------------
  // Auto unwinding.
  //-----------------
  // if (mySelfInter)  SelfInter(Modif);
  //-----------------
  // Intersection 3d .
  //-----------------
  Message_ProgressScope aPSInter(aPS.Next(aSteps(PIOperation_Intersection)), nullptr, 100);
  aPSInter.SetName((myJoin == GeomAbs_Arc) ? "Connect offset faces by arc"
                                           : "Connect offset faces by intersection");

  BRepOffset_Inter3d Inter(myAsDes, Side, myTol);
  Intersection3D(Inter, aPSInter.Next(90));
  if (myError != BRepOffset_NoError)
  {
    return;
  }
  //-----------------
  // Intersection2D
  //-----------------
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& Modif    = Inter.TouchedFaces();
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& NewEdges = Inter.NewEdges();

  if (!Modif.IsEmpty())
  {
    Intersection2D(Modif, NewEdges, aPSInter.Next(4));
    if (myError != BRepOffset_NoError)
    {
      return;
    }
  }

  //-------------------------------------------------------
  // Unwinding 2D and reconstruction of modified faces
  //----------------------------------------------------
  MakeLoops(Modif, aPSInter.Next(4));
  if (myError != BRepOffset_NoError)
  {
    return;
  }
  //-----------------------------------------------------
  // Reconstruction of non modified faces sharing
  // reconstructed edges
  //------------------------------------------------------
  if (!Modif.IsEmpty())
  {
    MakeFaces(Modif, aPSInter.Next(2));
    if (myError != BRepOffset_NoError)
    {
      return;
    }
  }

  aPSInter.Close();

  if (myThickening)
  {
    MakeMissingWalls(aPS.Next(aSteps(PIOperation_MakeMissingWalls)));
    if (myError != BRepOffset_NoError)
    {
      return;
    }
  }

  //-------------------------
  // Construction of shells.
  //-------------------------
  MakeShells(aPS.Next(aSteps(PIOperation_MakeShells)));
  if (myError != BRepOffset_NoError)
  {
    return;
  }
  if (myOffsetShape.IsNull())
  {
    // not done
    myDone = false;
    return;
  }
  //--------------
  // Unwinding 3D.
  //--------------
  SelectShells();
  //----------------------------------
  // Remove INTERNAL edges if necessary
  //----------------------------------
  if (myRemoveIntEdges)
  {
    RemoveInternalEdges();
  }
  //----------------------------------
  // Coding of regularities.
  //----------------------------------
  EncodeRegularity();
  //----------------------------------
  // Replace roots in history maps
  //----------------------------------
  ReplaceRoots();
  //----------------------
  // Creation of solids.
  //----------------------
  MakeSolid(aPS.Next(aSteps(PIOperation_MakeSolid)));
  if (myError != BRepOffset_NoError)
  {
    return;
  }
  //-----------------------------
  // MAJ Tolerance edge and Vertex
  // ----------------------------
  if (!myOffsetShape.IsNull())
  {
    if (myThickening)
    {
      UpdateTolerance(myOffsetShape, myFaces, myShape);
    }
    else
    {
      TopoDS_Shape aDummy;
      UpdateTolerance(myOffsetShape, myFaces, aDummy);
    }
    BRepLib::UpdateTolerances(myOffsetShape);
  }

  CorrectConicalFaces();

  // Result solid should be computed in MakeOffset scope.
  if (myThickening && myIsPerformSewing)
  {
    BRepBuilderAPI_Sewing aSew(myTol);
    aSew.Add(myOffsetShape);
    aSew.Perform(aPS.Next(aSteps(PIOperation_Sewing) / 2.));
    if (!aPS.More())
    {
      myError = BRepOffset_UserBreak;
      return;
    }
    myOffsetShape = aSew.SewedShape();

    // Rebuild solid.
    // Offset shape expected to be really closed after sewing.
    myOffsetShape.Closed(true);
    MakeSolid(aPS.Next(aSteps(PIOperation_Sewing) / 2.));
    if (myError != BRepOffset_NoError)
    {
      return;
    }
  }

  myDone = true;
}

//=================================================================================================

void BRepOffset_MakeOffset::MakeThickSolid(const Message_ProgressRange& theRange)
{
  //--------------------------------------------------------------
  // Construction of shell parallel to shell (initial without cap).
  //--------------------------------------------------------------
  MakeOffsetShape(theRange);

  if (!myDone)
  {
    // Save return code and myDone state.
    return;
  }

  //--------------------------------------------------------------------
  // Construction of a solid with the initial shell, parallel shell
  // limited by caps.
  //--------------------------------------------------------------------
  if (!myFaces.IsEmpty())
  {
    TopoDS_Solid    Res;
    TopExp_Explorer exp;
    BRep_Builder    B;
    int             NbF = myFaces.Extent();

    B.MakeSolid(Res);

    BRepTools_Quilt Glue;
    for (exp.Init(myShape, TopAbs_FACE); exp.More(); exp.Next())
    {
      NbF++;
      Glue.Add(exp.Current());
    }
    bool YaResult = 0;
    if (!myOffsetShape.IsNull())
    {
      for (exp.Init(myOffsetShape, TopAbs_FACE); exp.More(); exp.Next())
      {
        YaResult = 1;
        Glue.Add(exp.Current().Reversed());
      }
#ifdef OCCT_DEBUG
      if (YaResult == 0)
      {
        std::cout << "OffsetShape does not contain a FACES." << std::endl;
      }
#endif
    }
#ifdef OCCT_DEBUG
    else
    {
      std::cout << "OffsetShape is null!" << std::endl;
    }
#endif

    if (YaResult == 0)
    {
      myDone  = false;
      myError = BRepOffset_UnknownError;
      return;
    }

    myOffsetShape = Glue.Shells();
    for (exp.Init(myOffsetShape, TopAbs_SHELL); exp.More(); exp.Next())
    {
      B.Add(Res, exp.Current());
    }
    Res.Closed(true);
    myOffsetShape = Res;

    // Test of Validity of the result of thick Solid
    // more face than the initial solid.
    int NbOF = 0;
    for (exp.Init(myOffsetShape, TopAbs_FACE); exp.More(); exp.Next())
    {
      NbOF++;
    }
    if (NbOF < NbF)
    {
      myDone  = false;
      myError = BRepOffset_UnknownError;
      return;
    }
    if (NbOF == NbF)
    {
      myOffset = 0;
    }
  }

  if (myOffset > 0)
    myOffsetShape.Reverse();

  myDone = true;
}

//=================================================================================================

bool BRepOffset_MakeOffset::IsDone() const
{
  return myDone;
}

//=================================================================================================

BRepOffset_Error BRepOffset_MakeOffset::Error() const
{
  return myError;
}

//=================================================================================================

const TopoDS_Shape& BRepOffset_MakeOffset::Shape() const
{
  return myOffsetShape;
}

//=================================================================================================

void BRepOffset_MakeOffset::MakeOffsetFaces(
  NCollection_DataMap<TopoDS_Shape, BRepOffset_Offset, TopTools_ShapeMapHasher>& theMapSF,
  const Message_ProgressRange&                                                   theRange)
{
  double                                                                   aCurOffset;
  NCollection_List<TopoDS_Shape>                                           aLF;
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> ShapeTgt;
  NCollection_List<TopoDS_Shape>::Iterator                                 aItLF;
  //
  bool OffsetOutside = (myOffset > 0.);
  //
  BRepLib::SortFaces(myFaceComp, aLF);
  //
  Message_ProgressScope aPS(theRange, "Making offset faces", aLF.Size());
  aItLF.Initialize(aLF);
  for (; aItLF.More(); aItLF.Next(), aPS.Next())
  {
    if (!aPS.More())
    {
      myError = BRepOffset_UserBreak;
      return;
    }
    const TopoDS_Face& aF = TopoDS::Face(aItLF.Value());
    aCurOffset            = myFaceOffset.IsBound(aF) ? myFaceOffset(aF) : myOffset;
    BRepOffset_Offset              OF(aF, aCurOffset, ShapeTgt, OffsetOutside, myJoin);
    NCollection_List<TopoDS_Shape> Let;
    myAnalyse.Edges(aF, ChFiDS_Tangential, Let);
    NCollection_List<TopoDS_Shape>::Iterator itl(Let);
    for (; itl.More(); itl.Next())
    {
      const TopoDS_Edge& Cur = TopoDS::Edge(itl.Value());
      if (!ShapeTgt.IsBound(Cur) && !myAnalyse.HasGenerated(Cur))
      {
        TopoDS_Shape       aLocalShape = OF.Generated(Cur);
        const TopoDS_Edge& OTE         = TopoDS::Edge(aLocalShape);
        ShapeTgt.Bind(Cur, OF.Generated(Cur));
        TopoDS_Vertex V1, V2, OV1, OV2;
        TopExp::Vertices(Cur, V1, V2);
        TopExp::Vertices(OTE, OV1, OV2);
        NCollection_List<TopoDS_Shape> LE;
        if (!ShapeTgt.IsBound(V1))
        {
          myAnalyse.Edges(V1, ChFiDS_Tangential, LE);
          const NCollection_List<TopoDS_Shape>& LA = myAnalyse.Ancestors(V1);
          if (LE.Extent() == LA.Extent())
            ShapeTgt.Bind(V1, OV1);
        }
        if (!ShapeTgt.IsBound(V2))
        {
          LE.Clear();
          myAnalyse.Edges(V2, ChFiDS_Tangential, LE);
          const NCollection_List<TopoDS_Shape>& LA = myAnalyse.Ancestors(V2);
          if (LE.Extent() == LA.Extent())
            ShapeTgt.Bind(V2, OV2);
        }
      }
    }
    theMapSF.Bind(aF, OF);
  }
  //
  const NCollection_List<TopoDS_Shape>& aNewFaces = myAnalyse.NewFaces();
  for (NCollection_List<TopoDS_Shape>::Iterator it(aNewFaces); it.More(); it.Next())
  {
    const TopoDS_Face& aF = TopoDS::Face(it.Value());
    BRepOffset_Offset  OF(aF, 0.0, ShapeTgt, OffsetOutside, myJoin);
    theMapSF.Bind(aF, OF);
  }
}

//=================================================================================================

void BRepOffset_MakeOffset::BuildOffsetByInter(const Message_ProgressRange& theRange)
{
#ifdef OCCT_DEBUG
  if (ChronBuild)
  {
    std::cout << " CONSTRUCTION OF OFFSETS :" << std::endl;
    Clock.Reset();
    Clock.Start();
  }
#endif

  Message_ProgressScope aPSOuter(theRange, "Connect offset faces by intersection", 100);

  // just for better management and visualization of the progress steps
  // define a nested enum listing all the steps of the current method.
  enum BuildOffsetByInter_PISteps
  {
    BuildOffsetByInter_MakeOffsetFaces = 0,
    BuildOffsetByInter_ConnexIntByInt,
    BuildOffsetByInter_ContextIntByInt,
    BuildOffsetByInter_IntersectEdges,
    BuildOffsetByInter_CompleteEdgesIntersection,
    BuildOffsetByInter_BuildFaces,
    BuildOffsetByInter_FillHistoryForOffsets,
    BuildOffsetByInter_FillHistoryForDeepenings,
    BuildOffsetByInter_Last
  };

  double aNbFaces      = myFaceComp.NbChildren() + myAnalyse.NewFaces().Extent() + myFaces.Extent();
  double anOffsetsPart = (myFaceComp.NbChildren() + myAnalyse.NewFaces().Extent()) / aNbFaces;
  double aDeepeningsPart = myFaces.Extent() / aNbFaces;

  NCollection_Array1<double> aSteps(0, BuildOffsetByInter_Last - 1);
  {
    aSteps.Init(0);

    bool   isInter                                       = myJoin == GeomAbs_Intersection;
    double aFaceInter                                    = isInter ? 25. : 50.;
    double aBuildFaces                                   = isInter ? 50. : 25.;
    aSteps(BuildOffsetByInter_MakeOffsetFaces)           = 5.;
    aSteps(BuildOffsetByInter_ConnexIntByInt)            = aFaceInter * anOffsetsPart;
    aSteps(BuildOffsetByInter_ContextIntByInt)           = aFaceInter * aDeepeningsPart;
    aSteps(BuildOffsetByInter_IntersectEdges)            = 10.;
    aSteps(BuildOffsetByInter_CompleteEdgesIntersection) = 5.;
    aSteps(BuildOffsetByInter_BuildFaces)                = aBuildFaces;
    aSteps(BuildOffsetByInter_FillHistoryForOffsets)     = 5. * anOffsetsPart;
    aSteps(BuildOffsetByInter_FillHistoryForDeepenings)  = 5. * aDeepeningsPart;
    normalizeSteps(100., aSteps);
  }

  TopExp_Explorer                          Exp, Exp2, ExpC;
  NCollection_List<TopoDS_Shape>::Iterator itLF;

  //--------------------------------------------------------
  // Construction of faces parallel to initial faces
  //--------------------------------------------------------
  NCollection_DataMap<TopoDS_Shape, BRepOffset_Offset, TopTools_ShapeMapHasher> MapSF;
  MakeOffsetFaces(MapSF, aPSOuter.Next(aSteps(BuildOffsetByInter_MakeOffsetFaces)));
  if (!aPSOuter.More())
  {
    myError = BRepOffset_UserBreak;
    return;
  }
  //--------------------------------------------------------------------
  // MES   : Map of OffsetShape -> Extended Shapes.
  // Build : Map of Initial SS  -> OffsetShape build by Inter.
  //                               can be an edge or a compound of edges
  //---------------------------------------------------------------------
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> MES;
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> Build;
  NCollection_List<TopoDS_Shape>                                           Failed;
  TopAbs_State                                                             Side = TopAbs_IN;
  occ::handle<BRepAlgo_AsDes> AsDes = new BRepAlgo_AsDes();

  //-------------------------------------------------------------------
  // Extension of faces and calculation of new edges of intersection.
  //-------------------------------------------------------------------
  bool ExtentContext = 0;
  if (myOffset > 0)
    ExtentContext = 1;

  BRepOffset_Inter3d Inter3(AsDes, Side, myTol);
  // Intersection between parallel faces
  Inter3.ConnexIntByInt(myFaceComp,
                        MapSF,
                        myAnalyse,
                        MES,
                        Build,
                        Failed,
                        aPSOuter.Next(aSteps(BuildOffsetByInter_ConnexIntByInt)),
                        myIsPlanar);
  if (!aPSOuter.More())
  {
    myError = BRepOffset_UserBreak;
    return;
  }
  // Intersection with caps.
  Inter3.ContextIntByInt(myFaces,
                         ExtentContext,
                         MapSF,
                         myAnalyse,
                         MES,
                         Build,
                         Failed,
                         aPSOuter.Next(aSteps(BuildOffsetByInter_ContextIntByInt)),
                         myIsPlanar);
  if (!aPSOuter.More())
  {
    myError = BRepOffset_UserBreak;
    return;
  }

  NCollection_List<TopoDS_Shape> aLFaces;
  for (Exp.Init(myFaceComp, TopAbs_FACE); Exp.More(); Exp.Next())
    aLFaces.Append(Exp.Current());
  for (NCollection_List<TopoDS_Shape>::Iterator it(myAnalyse.NewFaces()); it.More(); it.Next())
    aLFaces.Append(it.Value());
  //---------------------------------------------------------------------------------
  // Extension of neighbor edges of new edges and intersection between neighbors.
  //--------------------------------------------------------------------------------
  occ::handle<BRepAlgo_AsDes> AsDes2d = new BRepAlgo_AsDes();
  IntersectEdges(aLFaces,
                 MapSF,
                 MES,
                 Build,
                 AsDes,
                 AsDes2d,
                 aPSOuter.Next(aSteps(BuildOffsetByInter_IntersectEdges)));
  if (myError != BRepOffset_NoError)
  {
    return;
  }
  //-----------------------------------------------------------
  // Great restriction of new edges and update of AsDes.
  //------------------------------------------ ----------------
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    anEdgesOrigins; // offset edge - initial edges
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> NewEdges;
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>
    aETrimEInf; // trimmed - not trimmed edges
  //
  // Map of edges obtained after FACE-FACE (offsetted) intersection.
  // Key1 is edge trimmed by intersection points with other edges;
  // Item is not-trimmed edge.
  if (!TrimEdges(myFaceComp,
                 myOffset,
                 myAnalyse,
                 MapSF,
                 MES,
                 Build,
                 AsDes,
                 AsDes2d,
                 NewEdges,
                 aETrimEInf,
                 anEdgesOrigins))
  {
    myError = BRepOffset_CannotTrimEdges;
    return;
  }
  //
  //---------------------------------
  // Intersection 2D on //
  //---------------------------------
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    aDMVV;
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>
                                 aFacesOrigins; // offset face - initial face
  NCollection_List<TopoDS_Shape> LFE;
  BRepAlgo_Image                 IMOE;
  GetEnlargedFaces(aLFaces, MapSF, MES, aFacesOrigins, IMOE, LFE);
  //
  NCollection_List<TopoDS_Shape>::Iterator itLFE(LFE);
  Message_ProgressScope aPS2d(aPSOuter.Next(aSteps(BuildOffsetByInter_CompleteEdgesIntersection)),
                              nullptr,
                              2);
  Message_ProgressScope aPS2dOffsets(aPS2d.Next(2. * anOffsetsPart), nullptr, LFE.Size());
  for (; itLFE.More(); itLFE.Next())
  {
    if (!aPS2dOffsets.More())
    {
      myError = BRepOffset_UserBreak;
      return;
    }
    const TopoDS_Face& NEF          = TopoDS::Face(itLFE.Value());
    double             aCurrFaceTol = BRep_Tool::Tolerance(NEF);
    BRepOffset_Inter2d::Compute(AsDes,
                                NEF,
                                NewEdges,
                                aCurrFaceTol,
                                myEdgeIntEdges,
                                aDMVV,
                                aPS2dOffsets.Next());
  }
  //----------------------------------------------
  // Intersections 2d on caps.
  //----------------------------------------------
  int                   i;
  Message_ProgressScope aPS2dCaps(aPS2d.Next(2. * aDeepeningsPart), nullptr, myFaces.Extent());
  for (i = 1; i <= myFaces.Extent(); i++)
  {
    if (!aPS2dCaps.More())
    {
      myError = BRepOffset_UserBreak;
      return;
    }
    const TopoDS_Face& Cork         = TopoDS::Face(myFaces(i));
    double             aCurrFaceTol = BRep_Tool::Tolerance(Cork);
    BRepOffset_Inter2d::Compute(AsDes,
                                Cork,
                                NewEdges,
                                aCurrFaceTol,
                                myEdgeIntEdges,
                                aDMVV,
                                aPS2dCaps.Next());
  }
  //
  BRepOffset_Inter2d::FuseVertices(aDMVV, AsDes, myImageVV);
  //-------------------------------
  // Unwinding of extended Faces.
  //-------------------------------
  //
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMFDone;
  //
  if ((myJoin == GeomAbs_Intersection) && myInter && myIsPlanar)
  {
    BuildSplitsOfExtendedFaces(LFE,
                               myAnalyse,
                               AsDes,
                               anEdgesOrigins,
                               aFacesOrigins,
                               aETrimEInf,
                               IMOE,
                               aPSOuter.Next(aSteps(BuildOffsetByInter_BuildFaces)));
    if (myError != BRepOffset_NoError)
    {
      return;
    }
    //
    NCollection_List<TopoDS_Shape>::Iterator aItLF(LFE);
    for (; aItLF.More(); aItLF.Next())
    {
      const TopoDS_Shape& aS = aItLF.Value();
      aMFDone.Add(aS);
    }
  }
  else
  {
    myMakeLoops.Build(LFE,
                      AsDes,
                      IMOE,
                      myImageVV,
                      aPSOuter.Next(aSteps(BuildOffsetByInter_BuildFaces)));
    if (!aPSOuter.More())
    {
      myError = BRepOffset_UserBreak;
      return;
    }
  }
  //
#ifdef OCCT_DEBUG
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> COES;
#endif
  //---------------------------
  // MAJ SD. for faces //
  //---------------------------
  Message_ProgressScope aPSHist(aPSOuter.Next(aSteps(BuildOffsetByInter_FillHistoryForOffsets)),
                                "Fill history for offset faces",
                                aLFaces.Size());
  for (NCollection_List<TopoDS_Shape>::Iterator it(aLFaces); it.More(); it.Next(), aPSHist.Next())
  {
    if (!aPSHist.More())
    {
      myError = BRepOffset_UserBreak;
      return;
    }
    const TopoDS_Shape& FI = it.Value();
    myInitOffsetFace.SetRoot(FI);
    TopoDS_Face OF = MapSF(FI).Face();
    if (MES.IsBound(OF))
    {
      OF = TopoDS::Face(MES(OF));
      if (IMOE.HasImage(OF))
      {
        const NCollection_List<TopoDS_Shape>& LOFE = IMOE.Image(OF);
        myInitOffsetFace.Bind(FI, LOFE);
        for (itLF.Initialize(LOFE); itLF.More(); itLF.Next())
        {
          const TopoDS_Shape& OFE = itLF.Value();
          myImageOffset.SetRoot(OFE);
#ifdef DRAW
          if (AffichInt2d)
          {
            Sprintf(name, "AF_%d", NbAF++);
            DBRep::Set(name, OFE);
          }
#endif
          NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> View;
          for (Exp2.Init(OFE.Oriented(TopAbs_FORWARD), TopAbs_EDGE); Exp2.More(); Exp2.Next())
          {
            const TopoDS_Edge& COE = TopoDS::Edge(Exp2.Current());

            myAsDes->Add(OFE, COE);
#ifdef DRAW
            if (AffichInt2d)
            {
              Sprintf(name, "AE_%d", NbAE++);
              DBRep::Set(name, COE);
              COES.Add(COE);
            }
#endif
            if (View.Add(COE))
            {
              if (!myAsDes->HasDescendant(COE))
              {
                TopoDS_Vertex CV1, CV2;
                TopExp::Vertices(COE, CV1, CV2);
                if (!CV1.IsNull())
                  myAsDes->Add(COE, CV1.Oriented(TopAbs_FORWARD));
                if (!CV2.IsNull())
                  myAsDes->Add(COE, CV2.Oriented(TopAbs_REVERSED));
              }
            }
          }
        }
      }
      else
      {
        if (aMFDone.Contains(OF))
        {
          continue;
        }
        //
        myInitOffsetFace.Bind(FI, OF);
        myImageOffset.SetRoot(OF);
#ifdef DRAW
        if (AffichInt2d)
        {
          Sprintf(name, "AF_%d", NbAF++);
          DBRep::Set(name, OF);
        }
#endif
        const NCollection_List<TopoDS_Shape>& LE = AsDes->Descendant(OF);
        for (itLF.Initialize(LE); itLF.More(); itLF.Next())
        {
          const TopoDS_Edge& OE = TopoDS::Edge(itLF.Value());
          if (IMOE.HasImage(OE))
          {
            const NCollection_List<TopoDS_Shape>&    LOE = IMOE.Image(OE);
            NCollection_List<TopoDS_Shape>::Iterator itLOE(LOE);
            for (; itLOE.More(); itLOE.Next())
            {
              TopoDS_Shape       aLocalShape = itLOE.Value().Oriented(OE.Orientation());
              const TopoDS_Edge& COE         = TopoDS::Edge(aLocalShape);
              //              const TopoDS_Edge& COE =
              //              TopoDS::Edge(itLOE.Value().Oriented(OE.Orientation()));
              myAsDes->Add(OF, COE);
#ifdef DRAW
              if (AffichInt2d)
              {
                Sprintf(name, "AE_%d", NbAE++);
                DBRep::Set(name, COE);
                COES.Add(COE);
              }
#endif

              if (!myAsDes->HasDescendant(COE))
              {
                TopoDS_Vertex CV1, CV2;
                TopExp::Vertices(COE, CV1, CV2);
                if (!CV1.IsNull())
                  myAsDes->Add(COE, CV1.Oriented(TopAbs_FORWARD));
                if (!CV2.IsNull())
                  myAsDes->Add(COE, CV2.Oriented(TopAbs_REVERSED));
              }
            }
          }
          else
          {
            myAsDes->Add(OF, OE);
#ifdef DRAW
            if (AffichInt2d)
            {
              Sprintf(name, "AE_%d", NbAE++);
              DBRep::Set(name, OE);
              COES.Add(OE);
            }
#endif

            const NCollection_List<TopoDS_Shape>& LV = AsDes->Descendant(OE);
            myAsDes->Add(OE, LV);
          }
        }
      }
    }
    else
    {
      myInitOffsetFace.Bind(FI, OF);
      myImageOffset.SetRoot(OF);
      NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> View;
      for (Exp2.Init(OF.Oriented(TopAbs_FORWARD), TopAbs_EDGE); Exp2.More(); Exp2.Next())
      {

        const TopoDS_Edge& COE = TopoDS::Edge(Exp2.Current());
        myAsDes->Add(OF, COE);
#ifdef DRAW
        if (AffichInt2d)
        {
          Sprintf(name, "AE_%d", NbAE++);
          DBRep::Set(name, COE);
          COES.Add(COE);
        }
#endif

        if (View.Add(Exp2.Current()))
        {
          if (!myAsDes->HasDescendant(COE))
          {
            TopoDS_Vertex CV1, CV2;
            TopExp::Vertices(COE, CV1, CV2);
            if (!CV1.IsNull())
              myAsDes->Add(COE, CV1.Oriented(TopAbs_FORWARD));
            if (!CV2.IsNull())
              myAsDes->Add(COE, CV2.Oriented(TopAbs_REVERSED));
          }
        }
      }
    }
  }
  //  Modified by skv - Tue Mar 15 16:20:43 2005
  // Add methods for supporting history.
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMapEdges;

  for (NCollection_List<TopoDS_Shape>::Iterator it(aLFaces); it.More(); it.Next())
  {
    const TopoDS_Shape& aFaceRef = it.Value();
    Exp2.Init(aFaceRef.Oriented(TopAbs_FORWARD), TopAbs_EDGE);

    for (; Exp2.More(); Exp2.Next())
    {
      const TopoDS_Shape& anEdgeRef = Exp2.Current();

      if (aMapEdges.Add(anEdgeRef))
      {
        myInitOffsetEdge.SetRoot(anEdgeRef);
        if (Build.IsBound(anEdgeRef))
        {
          TopoDS_Shape aNewShape = Build(anEdgeRef);

          if (aNewShape.ShapeType() == TopAbs_EDGE)
          {
            if (IMOE.HasImage(aNewShape))
            {
              const NCollection_List<TopoDS_Shape>& aListNewE = IMOE.Image(aNewShape);

              myInitOffsetEdge.Bind(anEdgeRef, aListNewE);
            }
            else
              myInitOffsetEdge.Bind(anEdgeRef, aNewShape);
          }
          else
          { // aNewShape != TopAbs_EDGE
            NCollection_List<TopoDS_Shape> aListNewEdge;

            for (ExpC.Init(aNewShape, TopAbs_EDGE); ExpC.More(); ExpC.Next())
            {
              const TopoDS_Shape& aResEdge = ExpC.Current();

              if (IMOE.HasImage(aResEdge))
              {
                const NCollection_List<TopoDS_Shape>&    aListNewE = IMOE.Image(aResEdge);
                NCollection_List<TopoDS_Shape>::Iterator aNewEIter(aListNewE);

                for (; aNewEIter.More(); aNewEIter.Next())
                  aListNewEdge.Append(aNewEIter.Value());
              }
              else
                aListNewEdge.Append(aResEdge);
            }

            myInitOffsetEdge.Bind(anEdgeRef, aListNewEdge);
          }
        }
        else
        { // Free boundary.
          TopoDS_Shape aNewEdge = MapSF(aFaceRef).Generated(anEdgeRef);

          if (MES.IsBound(aNewEdge))
            aNewEdge = MES(aNewEdge);

          if (IMOE.HasImage(aNewEdge))
          {
            const NCollection_List<TopoDS_Shape>& aListNewE = IMOE.Image(aNewEdge);

            myInitOffsetEdge.Bind(anEdgeRef, aListNewE);
          }
          else
            myInitOffsetEdge.Bind(anEdgeRef, aNewEdge);
        }
      }
    }
  }
  //  Modified by skv - Tue Mar 15 16:20:43 2005

  //---------------------------
  // MAJ SD. for caps
  //---------------------------
  // NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> View;
  Message_ProgressScope aPSHist2(aPSOuter.Next(aSteps(BuildOffsetByInter_FillHistoryForDeepenings)),
                                 "Fill history for deepening faces",
                                 myFaces.Extent());
  for (i = 1; i <= myFaces.Extent(); i++, aPSHist2.Next())
  {
    if (!aPSHist2.More())
    {
      myError = BRepOffset_UserBreak;
      return;
    }
    const TopoDS_Shape&                   Cork = myFaces(i);
    const NCollection_List<TopoDS_Shape>& LE   = AsDes->Descendant(Cork);
    for (itLF.Initialize(LE); itLF.More(); itLF.Next())
    {
      const TopoDS_Edge& OE = TopoDS::Edge(itLF.Value());
      if (IMOE.HasImage(OE))
      {
        const NCollection_List<TopoDS_Shape>&    LOE = IMOE.Image(OE);
        NCollection_List<TopoDS_Shape>::Iterator itLOE(LOE);
        for (; itLOE.More(); itLOE.Next())
        {
          const TopoDS_Edge& COE = TopoDS::Edge(itLOE.Value());
          myAsDes->Add(Cork, COE.Oriented(OE.Orientation()));
#ifdef DRAW
          if (AffichInt2d)
          {
            Sprintf(name, "AE_%d", NbAE++);
            DBRep::Set(name, COE);
            COES.Add(COE);
          }
#endif

          if (!myAsDes->HasDescendant(COE))
          {
            TopoDS_Vertex CV1, CV2;
            TopExp::Vertices(COE, CV1, CV2);
            if (!CV1.IsNull())
              myAsDes->Add(COE, CV1.Oriented(TopAbs_FORWARD));
            if (!CV2.IsNull())
              myAsDes->Add(COE, CV2.Oriented(TopAbs_REVERSED));
          }
        }
      }
      else
      {
        myAsDes->Add(Cork, OE);
        if (AsDes->HasDescendant(OE))
        {
          myAsDes->Add(OE, AsDes->Descendant(OE));
        }
#ifdef DRAW
        if (AffichInt2d)
        {
          Sprintf(name, "AE_%d", NbAE++);
          DBRep::Set(name, OE);
          COES.Add(OE);
        }
#endif
      }
    }
  }

#ifdef OCCT_DEBUG
  DEBVerticesControl(COES, myAsDes);
  if (ChronBuild)
    Clock.Show();
#endif
}

//=================================================================================================

void BRepOffset_MakeOffset::ReplaceRoots()
{
  // Replace the artificial faces and edges in InitOffset maps with the original ones.
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> View;
  for (TopExp_Explorer anExpF(myFaceComp, TopAbs_EDGE); anExpF.More(); anExpF.Next())
  {
    const TopoDS_Shape& aF = anExpF.Current();
    for (TopExp_Explorer anExpE(aF, TopAbs_EDGE); anExpE.More(); anExpE.Next())
    {
      const TopoDS_Shape& aE = anExpE.Current();
      if (!View.Add(aE))
        continue;

      TopoDS_Shape aFGen = myAnalyse.Generated(aE);
      if (aFGen.IsNull())
        continue;

      myInitOffsetFace.ReplaceRoot(aFGen, aE);

      for (TopoDS_Iterator itV(aE); itV.More(); itV.Next())
      {
        const TopoDS_Shape& aV = itV.Value();
        if (!View.Add(aV))
          continue;

        TopoDS_Shape aEGen = myAnalyse.Generated(aV);
        if (aEGen.IsNull())
          continue;

        myInitOffsetEdge.ReplaceRoot(aEGen, aV);
      }
    }
  }
}

//=======================================================================
// function : BuildFaceComp
// purpose  : Make a compound containing actual faces (including planar faces instead of their
// originals)
//=======================================================================
void BRepOffset_MakeOffset::BuildFaceComp()
{
  BRep_Builder aBB;
  aBB.MakeCompound(myFaceComp);
  TopExp_Explorer anExplo(myShape, TopAbs_FACE);
  for (; anExplo.More(); anExplo.Next())
  {
    TopoDS_Shape        aFace     = anExplo.Current();
    TopAbs_Orientation  anOr      = aFace.Orientation();
    const TopoDS_Shape* aPlanface = myFacePlanfaceMap.Seek(aFace);
    if (aPlanface)
      aFace = *aPlanface;
    aBB.Add(myFaceComp, aFace.Oriented(anOr));
  }
}

//=================================================================================================

void BRepOffset_MakeOffset::BuildOffsetByArc(const Message_ProgressRange& theRange)
{
#ifdef OCCT_DEBUG
  if (ChronBuild)
  {
    std::cout << " CONSTRUCTION OF OFFSETS :" << std::endl;
    Clock.Reset();
    Clock.Start();
  }
#endif

  TopExp_Explorer                                        Exp;
  NCollection_List<TopoDS_Shape>::Iterator               itLF;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> Done;
  Message_ProgressScope                                  aPSOuter(theRange, nullptr, 10);
  //--------------------------------------------------------
  // Construction of faces parallel to initial faces
  //--------------------------------------------------------
  NCollection_DataMap<TopoDS_Shape, BRepOffset_Offset, TopTools_ShapeMapHasher> MapSF;
  MakeOffsetFaces(MapSF, aPSOuter.Next());
  if (myError != BRepOffset_NoError)
  {
    return;
  }
  //--------------------------------------------------------
  // Construction of tubes on edge.
  //--------------------------------------------------------
  ChFiDS_TypeOfConcavity OT = ChFiDS_Convex;
  if (myOffset < 0.)
    OT = ChFiDS_Concave;

  Message_ProgressScope aPS1(aPSOuter.Next(4), "Constructing tubes on edges", 1, true);
  for (Exp.Init(myFaceComp, TopAbs_EDGE); Exp.More(); Exp.Next(), aPS1.Next())
  {
    if (!aPS1.More())
    {
      myError = BRepOffset_UserBreak;
      return;
    }
    const TopoDS_Edge& E = TopoDS::Edge(Exp.Current());
    if (Done.Add(E))
    {
      const NCollection_List<TopoDS_Shape>& Anc = myAnalyse.Ancestors(E);
      if (Anc.Extent() == 2)
      {
        const NCollection_List<BRepOffset_Interval>& L = myAnalyse.Type(E);
        if (!L.IsEmpty() && L.First().Type() == OT)
        {
          double CurOffset = myOffset;
          if (myFaceOffset.IsBound(Anc.First()))
            CurOffset = myFaceOffset(Anc.First());
          TopoDS_Shape aLocalShapeGen = MapSF(Anc.First()).Generated(E);
          TopoDS_Edge  EOn1           = TopoDS::Edge(aLocalShapeGen);
          aLocalShapeGen              = MapSF(Anc.Last()).Generated(E);
          TopoDS_Edge EOn2            = TopoDS::Edge(aLocalShapeGen);
          //          TopoDS_Edge EOn1 = TopoDS::Edge(MapSF(Anc.First()).Generated(E));
          //          TopoDS_Edge EOn2 = TopoDS::Edge(MapSF(Anc.Last()) .Generated(E));
          // find if exits tangent edges in the original shape
          TopoDS_Edge   E1f, E1l;
          TopoDS_Vertex V1f, V1l;
          TopExp::Vertices(E, V1f, V1l);
          NCollection_List<TopoDS_Shape> TangE;
          myAnalyse.TangentEdges(E, V1f, TangE);
          // find if the pipe on the tangent edges are soon created.
          NCollection_List<TopoDS_Shape>::Iterator itl(TangE);
          bool                                     Find = false;
          for (; itl.More() && !Find; itl.Next())
          {
            if (MapSF.IsBound(itl.Value()))
            {
              TopoDS_Shape aLocalShape = MapSF(itl.Value()).Generated(V1f);
              E1f                      = TopoDS::Edge(aLocalShape);
              //              E1f  = TopoDS::Edge(MapSF(itl.Value()).Generated(V1f));
              Find = true;
            }
          }
          TangE.Clear();
          myAnalyse.TangentEdges(E, V1l, TangE);
          // find if the pipe on the tangent edges are soon created.
          itl.Initialize(TangE);
          Find = false;
          for (; itl.More() && !Find; itl.Next())
          {
            if (MapSF.IsBound(itl.Value()))
            {
              TopoDS_Shape aLocalShape = MapSF(itl.Value()).Generated(V1l);
              E1l                      = TopoDS::Edge(aLocalShape);
              //              E1l  = TopoDS::Edge(MapSF(itl.Value()).Generated(V1l));
              Find = true;
            }
          }
          BRepOffset_Offset OF(E, EOn1, EOn2, CurOffset, E1f, E1l);
          MapSF.Bind(E, OF);
        }
      }
      else
      {
        // ----------------------
        // free border.
        // ----------------------
        TopoDS_Shape aLocalShape = MapSF(Anc.First()).Generated(E);
        TopoDS_Edge  EOn1        = TopoDS::Edge(aLocalShape);
        ///        TopoDS_Edge EOn1 = TopoDS::Edge(MapSF(Anc.First()).Generated(E));
        myInitOffsetEdge.SetRoot(E); // skv: supporting history.
        myInitOffsetEdge.Bind(E, EOn1);
      }
    }
  }

  //--------------------------------------------------------
  // Construction of spheres on vertex.
  //--------------------------------------------------------
  Done.Clear();
  NCollection_List<TopoDS_Shape>::Iterator it;
  Message_ProgressScope aPS2(aPSOuter.Next(4), "Constructing spheres on vertices", 1, true);
  for (Exp.Init(myFaceComp, TopAbs_VERTEX); Exp.More(); Exp.Next(), aPS2.Next())
  {
    if (!aPS2.More())
    {
      myError = BRepOffset_UserBreak;
      return;
    }
    const TopoDS_Vertex& V = TopoDS::Vertex(Exp.Current());
    if (Done.Add(V))
    {
      const NCollection_List<TopoDS_Shape>& LA = myAnalyse.Ancestors(V);
      NCollection_List<TopoDS_Shape>        LE;
      myAnalyse.Edges(V, OT, LE);

      if (LE.Extent() >= 3 && LE.Extent() == LA.Extent())
      {
        NCollection_List<TopoDS_Shape> LOE;
        //--------------------------------------------------------
        // Return connected edges on tubes.
        //--------------------------------------------------------
        for (it.Initialize(LE); it.More(); it.Next())
        {
          LOE.Append(MapSF(it.Value()).Generated(V).Reversed());
        }
        //----------------------
        // construction sphere.
        //-----------------------
        const NCollection_List<TopoDS_Shape>& LLA       = myAnalyse.Ancestors(LA.First());
        const TopoDS_Shape&                   FF        = LLA.First();
        double                                CurOffset = myOffset;
        if (myFaceOffset.IsBound(FF))
          CurOffset = myFaceOffset(FF);

        BRepOffset_Offset OF(V, LOE, CurOffset);
        MapSF.Bind(V, OF);
      }
      //--------------------------------------------------------------
      // Particular processing if V is at least a free border.
      //-------------------------------------------------------------
      NCollection_List<TopoDS_Shape> LBF;
      myAnalyse.Edges(V, ChFiDS_FreeBound, LBF);
      if (!LBF.IsEmpty())
      {
        bool First = true;
        for (it.Initialize(LE); it.More(); it.Next())
        {
          if (First)
          {
            myInitOffsetEdge.SetRoot(V); // skv: supporting history.
            myInitOffsetEdge.Bind(V, MapSF(it.Value()).Generated(V));
            First = false;
          }
          else
          {
            myInitOffsetEdge.Add(V, MapSF(it.Value()).Generated(V));
          }
        }
      }
    }
  }

  //------------------------------------------------------------
  // Extension of parallel faces to the context.
  // Extended faces are ordered in DS and removed from MapSF.
  //------------------------------------------------------------
  if (!myFaces.IsEmpty())
    ToContext(MapSF);

  //------------------------------------------------------
  // MAJ SD.
  //------------------------------------------------------
  ChFiDS_TypeOfConcavity RT = ChFiDS_Concave;
  if (myOffset < 0.)
    RT = ChFiDS_Convex;
  NCollection_DataMap<TopoDS_Shape, BRepOffset_Offset, TopTools_ShapeMapHasher>::Iterator It(MapSF);
  Message_ProgressScope aPS3(aPSOuter.Next(), nullptr, MapSF.Size());
  for (; It.More(); It.Next(), aPS3.Next())
  {
    if (!aPS3.More())
    {
      myError = BRepOffset_UserBreak;
      return;
    }
    const TopoDS_Shape&      SI = It.Key();
    const BRepOffset_Offset& SF = It.Value();
    if (SF.Status() == BRepOffset_Reversed || SF.Status() == BRepOffset_Degenerated)
    {
      //------------------------------------------------
      // Degenerated or returned faces are not stored.
      //------------------------------------------------
      continue;
    }

    const TopoDS_Face& OF = It.Value().Face();
    myInitOffsetFace.Bind(SI, OF);
    myInitOffsetFace.SetRoot(SI); // Initial<-> Offset
    myImageOffset.SetRoot(OF);    // FaceOffset root of images

    if (SI.ShapeType() == TopAbs_FACE)
    {
      for (Exp.Init(SI.Oriented(TopAbs_FORWARD), TopAbs_EDGE); Exp.More(); Exp.Next())
      {
        //--------------------------------------------------------------------
        // To each face are associatedthe edges that restrict that
        // The edges that do not generate tubes or are not tangent
        // to two faces are removed.
        //--------------------------------------------------------------------
        const TopoDS_Edge&                           E = TopoDS::Edge(Exp.Current());
        const NCollection_List<BRepOffset_Interval>& L = myAnalyse.Type(E);
        if (!L.IsEmpty() && L.First().Type() != RT)
        {
          TopAbs_Orientation OO          = E.Orientation();
          TopoDS_Shape       aLocalShape = It.Value().Generated(E);
          TopoDS_Edge        OE          = TopoDS::Edge(aLocalShape);
          //          TopoDS_Edge        OE  = TopoDS::Edge(It.Value().Generated(E));
          myAsDes->Add(OF, OE.Oriented(OO));
        }
      }
    }
    else
    {
      for (Exp.Init(OF.Oriented(TopAbs_FORWARD), TopAbs_EDGE); Exp.More(); Exp.Next())
      {
        myAsDes->Add(OF, Exp.Current());
      }
    }
  }

#ifdef OCCT_DEBUG
  if (ChronBuild)
    Clock.Show();
#endif
}

//=================================================================================================

void BRepOffset_MakeOffset::SelfInter(
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& /*Modif*/)
{
#ifdef OCCT_DEBUG
  if (ChronBuild)
  {
    std::cout << " AUTODEBOUCLAGE:" << std::endl;
    Clock.Reset();
    Clock.Start();
  }
#endif

  throw Standard_NotImplemented();
}

//=================================================================================================

void BRepOffset_MakeOffset::ToContext(
  NCollection_DataMap<TopoDS_Shape, BRepOffset_Offset, TopTools_ShapeMapHasher>& MapSF)
{
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> Created;
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> MEF;
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>            FacesToBuild;
  NCollection_List<TopoDS_Shape>::Iterator                                 itl;
  TopExp_Explorer                                                          exp;

  //  TopAbs_State       Side = TopAbs_IN;
  //  if (myOffset < 0.) Side = TopAbs_OUT;

  TopAbs_State Side = TopAbs_OUT;

  /*
  int i;
  for (i = 1; i <= myFaces.Extent(); i++) {
    const TopoDS_Face& CF = TopoDS::Face(myFaces(i));
    for (exp.Init(CF.Oriented(TopAbs_FORWARD),TopAbs_EDGE);
         exp.More(); exp.Next()) {
      const TopoDS_Edge& E = TopoDS::Edge(exp.Current());
      if (!myAnalyse.HasAncestor(E)) {
        //----------------------------------------------------------------
        // The edges of context faces that are not in the initial shape
        // can appear in the result.
        //----------------------------------------------------------------
        //myAsDes->Add(CF,E);
      }
    }
  }
  */

  //--------------------------------------------------------
  // Determine the edges and faces reconstructed by
  // intersection.
  //---------------------------------------------------------
  int j;
  for (j = 1; j <= myFaces.Extent(); j++)
  {
    const TopoDS_Face& CF = TopoDS::Face(myFaces(j));
    for (exp.Init(CF.Oriented(TopAbs_FORWARD), TopAbs_EDGE); exp.More(); exp.Next())
    {
      const TopoDS_Edge& E = TopoDS::Edge(exp.Current());
      if (myAnalyse.HasAncestor(E))
      {
        const NCollection_List<TopoDS_Shape>& LEA = myAnalyse.Ancestors(E);
        for (itl.Initialize(LEA); itl.More(); itl.Next())
        {
          const BRepOffset_Offset& OF = MapSF(itl.Value());
          FacesToBuild.Add(itl.Value());
          MEF.Bind(OF.Generated(E), CF);
        }
        TopoDS_Vertex V[2];
        TopExp::Vertices(E, V[0], V[1]);
        for (int i = 0; i < 2; i++)
        {
          const NCollection_List<TopoDS_Shape>& LVA = myAnalyse.Ancestors(V[i]);
          for (itl.Initialize(LVA); itl.More(); itl.Next())
          {
            const TopoDS_Edge& EV = TopoDS::Edge(itl.Value());
            if (MapSF.IsBound(EV))
            {
              const BRepOffset_Offset& OF = MapSF(EV);
              FacesToBuild.Add(EV);
              MEF.Bind(OF.Generated(V[i]), CF);
            }
          }
        }
      }
    }
  }
  //---------------------------
  // Reconstruction of faces.
  //---------------------------
  TopoDS_Face            F, NF;
  ChFiDS_TypeOfConcavity RT = ChFiDS_Concave;
  if (myOffset < 0.)
    RT = ChFiDS_Convex;
  TopoDS_Shape       OE, NE;
  TopAbs_Orientation Or;

  for (j = 1; j <= FacesToBuild.Extent(); j++)
  {
    const TopoDS_Shape& S = FacesToBuild(j);
    BRepOffset_Offset   BOF;
    BOF = MapSF(S);
    F   = TopoDS::Face(BOF.Face());
    BRepOffset_Tool::ExtentFace(F, Created, MEF, Side, myTol, NF);
    MapSF.UnBind(S);
    //--------------
    // MAJ SD.
    //--------------
    myInitOffsetFace.Bind(S, NF);
    myInitOffsetFace.SetRoot(S); // Initial<-> Offset
    myImageOffset.SetRoot(NF);

    if (S.ShapeType() == TopAbs_FACE)
    {
      for (exp.Init(S.Oriented(TopAbs_FORWARD), TopAbs_EDGE); exp.More(); exp.Next())
      {

        const TopoDS_Edge&                           E = TopoDS::Edge(exp.Current());
        const NCollection_List<BRepOffset_Interval>& L = myAnalyse.Type(E);
        OE                                             = BOF.Generated(E);
        Or                                             = E.Orientation();
        OE.Orientation(Or);
        if (!L.IsEmpty() && L.First().Type() != RT)
        {
          if (Created.IsBound(OE))
          {
            NE = Created(OE);
            if (NE.Orientation() == TopAbs_REVERSED)
              NE.Orientation(TopAbs::Reverse(Or));
            else
              NE.Orientation(Or);
            myAsDes->Add(NF, NE);
          }
          else
          {
            myAsDes->Add(NF, OE);
          }
        }
      }
    }
    else
    {
      //------------------
      // Tube
      //---------------------
      for (exp.Init(NF.Oriented(TopAbs_FORWARD), TopAbs_EDGE); exp.More(); exp.Next())
      {
        myAsDes->Add(NF, exp.Current());
      }
    }
    MapSF.UnBind(S);
  }

  //------------------
  // MAJ free borders
  //------------------
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator itc;
  for (itc.Initialize(Created); itc.More(); itc.Next())
  {
    OE = itc.Key();
    NE = itc.Value();
    if (myInitOffsetEdge.IsImage(OE))
    {
      TopoDS_Shape E = myInitOffsetEdge.ImageFrom(OE);
      Or             = myInitOffsetEdge.Image(E).First().Orientation();
      if (NE.Orientation() == TopAbs_REVERSED)
        NE.Orientation(TopAbs::Reverse(Or));
      else
        NE.Orientation(Or);
      myInitOffsetEdge.Remove(OE);
      myInitOffsetEdge.Bind(E, NE);
    }
  }
}

//=================================================================================================

void BRepOffset_MakeOffset::UpdateFaceOffset()
{
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>             M;
  NCollection_DataMap<TopoDS_Shape, double, TopTools_ShapeMapHasher> CopiedMap;
  CopiedMap.Assign(myFaceOffset);
  NCollection_DataMap<TopoDS_Shape, double, TopTools_ShapeMapHasher>::Iterator it(CopiedMap);

  ChFiDS_TypeOfConcavity RT = ChFiDS_Convex;
  if (myOffset < 0.)
    RT = ChFiDS_Concave;

  for (; it.More(); it.Next())
  {
    const TopoDS_Face& F         = TopoDS::Face(it.Key());
    double             CurOffset = CopiedMap(F);
    if (!M.Add(F))
      continue;
    TopoDS_Compound Co;
    BRep_Builder    Build;
    Build.MakeCompound(Co);
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> Dummy;
    Build.Add(Co, F);
    if (myJoin == GeomAbs_Arc)
      myAnalyse.AddFaces(F, Co, Dummy, ChFiDS_Tangential, RT);
    else
      myAnalyse.AddFaces(F, Co, Dummy, ChFiDS_Tangential);

    TopExp_Explorer exp(Co, TopAbs_FACE);
    for (; exp.More(); exp.Next())
    {
      const TopoDS_Face& FF = TopoDS::Face(exp.Current());
      if (!M.Add(FF))
        continue;
      if (myFaceOffset.IsBound(FF))
        myFaceOffset.UnBind(FF);
      myFaceOffset.Bind(FF, CurOffset);
    }
  }
}

//=================================================================================================

void BRepOffset_MakeOffset::CorrectConicalFaces()
{
  if (myOffsetShape.IsNull())
  {
    return;
  }
  //
  NCollection_Sequence<TopoDS_Shape> Cones;
  NCollection_Sequence<TopoDS_Shape> Circs;
  NCollection_Sequence<TopoDS_Shape> Seams;
  double                             TolApex = 1.e-5;

  int i;

  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    FacesOfCone;
  // NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> DegEdges;
  TopExp_Explorer Explo(myOffsetShape, TopAbs_FACE);
  if (myJoin == GeomAbs_Arc)
  {
    for (; Explo.More(); Explo.Next())
    {
      TopoDS_Face               aFace = TopoDS::Face(Explo.Current());
      occ::handle<Geom_Surface> aSurf = BRep_Tool::Surface(aFace);
      // if (aSurf->DynamicType() == STANDARD_TYPE(Geom_OffsetSurface))
      // aSurf = (occ::down_cast<Geom_OffsetSurface>(aSurf))->BasisSurface(); //???

      NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> Emap;
      TopExp::MapShapes(aFace, TopAbs_EDGE, Emap);
      for (i = 1; i <= Emap.Extent(); i++)
      {
        TopoDS_Edge anEdge = TopoDS::Edge(Emap(i));
        // double f, l;
        // occ::handle<Geom_Curve> theCurve = BRep_Tool::Curve( anEdge, f, l );
        // occ::handle<BRep_TEdge>& TE = *((occ::handle<BRep_TEdge>*) &anEdge.TShape());
        if (BRep_Tool::Degenerated(anEdge))
        {
          // Check if anEdge is a really degenerated edge or not
          BRepAdaptor_Curve BACurve(anEdge, aFace);
          gp_Pnt            Pfirst, Plast, Pmid;
          Pfirst = BACurve.Value(BACurve.FirstParameter());
          Plast  = BACurve.Value(BACurve.LastParameter());
          Pmid   = BACurve.Value((BACurve.FirstParameter() + BACurve.LastParameter()) / 2.);
          if (Pfirst.Distance(Plast) <= TolApex && Pfirst.Distance(Pmid) <= TolApex)
            continue;
          // Cones.Append( aFace );
          // Circs.Append( anEdge );
          // TopoDS_Vertex Vdeg = TopExp::FirstVertex( anEdge );
          TopoDS_Edge   OrEdge = TopoDS::Edge(myInitOffsetEdge.Root(anEdge));
          TopoDS_Vertex VF     = TopExp::FirstVertex(OrEdge);
          if (FacesOfCone.IsBound(VF))
          {
            // add a face to the existing list
            NCollection_List<TopoDS_Shape>& aFaces = FacesOfCone.ChangeFind(VF);
            aFaces.Append(aFace);
            // DegEdges.Bind(aFace, anEdge);
          }
          else
          {
            // the vertex is not in the map => create a new key and items
            NCollection_List<TopoDS_Shape> aFaces;
            aFaces.Append(aFace);
            FacesOfCone.Bind(VF, aFaces);
            // DegEdges.Bind(aFace, anEdge);
          }
        }
      } // for (i = 1; i <= Emap.Extent(); i++)
    } // for (; fexp.More(); fexp.Next())
  } // if (myJoin == GeomAbs_Arc)

  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>::
    Iterator      Cone(FacesOfCone);
  BRep_Builder    BB;
  TopLoc_Location L;
  bool            IsModified = false;
  for (; Cone.More(); Cone.Next())
  {
    gp_Sphere                          theSphere;
    occ::handle<Geom_SphericalSurface> aSphSurf;
    TopoDS_Wire                        SphereWire;
    BB.MakeWire(SphereWire);
    TopoDS_Vertex                            anApex = TopoDS::Vertex(Cone.Key());
    const NCollection_List<TopoDS_Shape>&    Faces  = Cone.Value(); // FacesOfCone(anApex);
    NCollection_List<TopoDS_Shape>::Iterator itFaces(Faces);
    bool                                     isFirstFace = true;
    gp_Pnt                                   FirstPoint;
    TopoDS_Vertex                            theFirstVertex, CurFirstVertex;
    for (; itFaces.More(); itFaces.Next())
    {
      TopoDS_Face aFace = TopoDS::Face(itFaces.Value()); // TopoDS::Face(Faces.First());
      TopoDS_Edge DegEdge;                               // = TopoDS::Edge(DegEdges(aFace));
      for (Explo.Init(aFace, TopAbs_EDGE); Explo.More(); Explo.Next())
      {
        DegEdge = TopoDS::Edge(Explo.Current());
        if (BRep_Tool::Degenerated(DegEdge))
        {
          TopoDS_Edge   OrEdge = TopoDS::Edge(myInitOffsetEdge.Root(DegEdge));
          TopoDS_Vertex VF     = TopExp::FirstVertex(OrEdge);
          if (VF.IsSame(anApex))
            break;
        }
      }
      TopoDS_Shape aLocalDegShape = DegEdge.Oriented(TopAbs_FORWARD);
      TopoDS_Edge  CurEdge        = TopoDS::Edge(aLocalDegShape);
      BB.Degenerated(CurEdge, false);
      BB.SameRange(CurEdge, false);
      BB.SameParameter(CurEdge, false);
      gp_Pnt fPnt, lPnt, mPnt;
      GetEdgePoints(CurEdge, aFace, fPnt, mPnt, lPnt);
      double f, l;
      BRep_Tool::Range(CurEdge, f, l);
      if (isFirstFace)
      {
        gp_Vec aVec1(fPnt, mPnt);
        gp_Vec aVec2(fPnt, lPnt);
        gp_Vec aNorm   = aVec1.Crossed(aVec2);
        gp_Pnt theApex = BRep_Tool::Pnt(anApex);
        gp_Vec ApexToFpnt(theApex, fPnt);
        gp_Vec Ydir = aNorm ^ ApexToFpnt;
        gp_Vec Xdir = Ydir ^ aNorm;
        // Xdir.Rotate(gp_Ax1(theApex, aNorm), -f);
        gp_Ax2 anAx2(theApex, gp_Dir(aNorm), gp_Dir(Xdir));
        theSphere.SetRadius(myOffset);
        theSphere.SetPosition(gp_Ax3(anAx2) /*gp_Ax3(theApex, gp_Dir(aNorm))*/);
        aSphSurf       = new Geom_SphericalSurface(theSphere);
        FirstPoint     = fPnt;
        theFirstVertex = BRepLib_MakeVertex(fPnt);
        CurFirstVertex = theFirstVertex;
      }

      TopoDS_Vertex v1, v2, FirstVert, EndVert;
      TopExp::Vertices(CurEdge, v1, v2);
      FirstVert = CurFirstVertex;
      if (lPnt.Distance(FirstPoint) <= Precision::Confusion())
        EndVert = theFirstVertex;
      else
        EndVert = BRepLib_MakeVertex(lPnt);
      CurEdge.Free(true);
      BB.Remove(CurEdge, v1);
      BB.Remove(CurEdge, v2);
      BB.Add(CurEdge, FirstVert.Oriented(TopAbs_FORWARD));
      BB.Add(CurEdge, EndVert.Oriented(TopAbs_REVERSED));
      // take the curve from sphere an put it to the edge
      double Uf, Vf, Ul, Vl;
      ElSLib::Parameters(theSphere, fPnt, Uf, Vf);
      ElSLib::Parameters(theSphere, lPnt, Ul, Vl);
      if (std::abs(Ul) <= Precision::Confusion())
        Ul = 2. * M_PI;
      occ::handle<Geom_Curve> aCurv = aSphSurf->VIso(Vf);
      /*
        if (!isFirstFace)
        {
        gp_Circ aCircle = (occ::down_cast<Geom_Circle>(aCurv))->Circ();
        if (std::abs(Uf - f) > Precision::Confusion())
        {
        aCircle.Rotate(aCircle.Axis(), f - Uf);
        aCurv = new Geom_Circle(aCircle);
        }
        }
      */
      occ::handle<Geom_TrimmedCurve> aTrimCurv = new Geom_TrimmedCurve(aCurv, Uf, Ul);
      BB.UpdateEdge(CurEdge, aTrimCurv, Precision::Confusion());
      BB.Range(CurEdge, Uf, Ul, true);
      occ::handle<Geom2d_Line>         theLin2d     = new Geom2d_Line(gp_Pnt2d(0., Vf), gp::DX2d());
      occ::handle<Geom2d_TrimmedCurve> theTrimLin2d = new Geom2d_TrimmedCurve(theLin2d, Uf, Ul);
      BB.UpdateEdge(CurEdge, theTrimLin2d, aSphSurf, L, Precision::Confusion());
      BB.Range(CurEdge, aSphSurf, L, Uf, Ul);
      BRepLib::SameParameter(CurEdge);
      BB.Add(SphereWire, CurEdge);
      // Modifying correspondent edges in aFace: substitute vertices common with CurEdge
      BRepAdaptor_Curve2d BAc2d(CurEdge, aFace);
      gp_Pnt2d            fPnt2d, lPnt2d;
      fPnt2d = BAc2d.Value(BAc2d.FirstParameter());
      lPnt2d = BAc2d.Value(BAc2d.LastParameter());
      NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> Emap;
      TopExp::MapShapes(aFace, TopAbs_EDGE, Emap);
      TopoDS_Edge EE[2];
      int         j = 0, k;
      for (k = 1; k <= Emap.Extent(); k++)
      {
        const TopoDS_Edge& anEdge = TopoDS::Edge(Emap(k));
        if (!BRep_Tool::Degenerated(anEdge))
        {
          TopoDS_Vertex V1, V2;
          TopExp::Vertices(anEdge, V1, V2);
          if (V1.IsSame(v1) || V2.IsSame(v1))
            EE[j++] = anEdge;
        }
      }
      for (k = 0; k < j; k++)
      {
        TopoDS_Shape aLocalShape = EE[k].Oriented(TopAbs_FORWARD);
        TopoDS_Edge  Eforward    = TopoDS::Edge(aLocalShape);
        Eforward.Free(true);
        TopoDS_Vertex V1, V2;
        TopExp::Vertices(Eforward, V1, V2);
        BRepAdaptor_Curve2d EEc(Eforward, aFace);
        gp_Pnt2d            p2d1, p2d2;
        p2d1 = EEc.Value(EEc.FirstParameter());
        p2d2 = EEc.Value(EEc.LastParameter());
        if (V1.IsSame(v1))
        {
          TopoDS_Vertex NewV =
            (p2d1.Distance(fPnt2d) <= Precision::Confusion()) ? FirstVert : EndVert;
          BB.Remove(Eforward, V1);
          BB.Add(Eforward, NewV.Oriented(TopAbs_FORWARD));
        }
        else
        {
          TopoDS_Vertex NewV =
            (p2d2.Distance(fPnt2d) <= Precision::Confusion()) ? FirstVert : EndVert;
          BB.Remove(Eforward, V2);
          BB.Add(Eforward, NewV.Oriented(TopAbs_REVERSED));
        }
      }

      isFirstFace    = false;
      CurFirstVertex = EndVert;
    }
    // Building new spherical face
    double                         Ufirst = RealLast(), Ulast = RealFirst();
    gp_Pnt2d                       p2d1, p2d2;
    NCollection_List<TopoDS_Shape> EdgesOfWire;
    TopoDS_Iterator                itw(SphereWire);
    for (; itw.More(); itw.Next())
    {
      const TopoDS_Edge& anEdge = TopoDS::Edge(itw.Value());
      EdgesOfWire.Append(anEdge);
      double                    f, l;
      occ::handle<Geom2d_Curve> aC2d = BRep_Tool::CurveOnSurface(anEdge, aSphSurf, L, f, l);
      p2d1                           = aC2d->Value(f);
      p2d2                           = aC2d->Value(l);
      if (p2d1.X() < Ufirst)
        Ufirst = p2d1.X();
      if (p2d1.X() > Ulast)
        Ulast = p2d1.X();
      if (p2d2.X() < Ufirst)
        Ufirst = p2d2.X();
      if (p2d2.X() > Ulast)
        Ulast = p2d2.X();
    }
    NCollection_List<TopoDS_Shape>           NewEdges;
    TopoDS_Edge                              FirstEdge;
    NCollection_List<TopoDS_Shape>::Iterator itl(EdgesOfWire);
    for (; itl.More(); itl.Next())
    {
      FirstEdge = TopoDS::Edge(itl.Value());
      double                    f, l;
      occ::handle<Geom2d_Curve> aC2d = BRep_Tool::CurveOnSurface(FirstEdge, aSphSurf, L, f, l);
      p2d1                           = aC2d->Value(f);
      p2d2                           = aC2d->Value(l);
      if (std::abs(p2d1.X() - Ufirst) <= Precision::Confusion())
      {
        EdgesOfWire.Remove(itl);
        break;
      }
    }
    NewEdges.Append(FirstEdge.Oriented(TopAbs_FORWARD));
    TopoDS_Vertex Vf1, CurVertex;
    TopExp::Vertices(FirstEdge, Vf1, CurVertex);
    itl.Initialize(EdgesOfWire);
    while (itl.More())
    {
      const TopoDS_Edge& anEdge = TopoDS::Edge(itl.Value());
      TopoDS_Vertex      V1, V2;
      TopExp::Vertices(anEdge, V1, V2);
      if (V1.IsSame(CurVertex) || V2.IsSame(CurVertex))
      {
        NewEdges.Append(anEdge.Oriented(TopAbs_FORWARD));
        CurVertex = (V1.IsSame(CurVertex)) ? V2 : V1;
        EdgesOfWire.Remove(itl);
      }
      else
        itl.Next();
    }

    double Vfirst, Vlast;
    if (p2d1.Y() > 0.)
    {
      Vfirst = p2d1.Y();
      Vlast  = M_PI / 2.;
    }
    else
    {
      Vfirst = -M_PI / 2.;
      Vlast  = p2d1.Y();
    }
    TopoDS_Face NewSphericalFace =
      BRepLib_MakeFace(aSphSurf, Ufirst, Ulast, Vfirst, Vlast, Precision::Confusion());
    TopoDS_Edge OldEdge, DegEdge;
    for (Explo.Init(NewSphericalFace, TopAbs_EDGE); Explo.More(); Explo.Next())
    {
      DegEdge = TopoDS::Edge(Explo.Current());
      if (BRep_Tool::Degenerated(DegEdge))
        break;
    }
    TopoDS_Vertex DegVertex = TopExp::FirstVertex(DegEdge);
    for (Explo.Init(NewSphericalFace, TopAbs_EDGE); Explo.More(); Explo.Next())
    {
      OldEdge = TopoDS::Edge(Explo.Current());
      TopoDS_Vertex V1, V2;
      TopExp::Vertices(OldEdge, V1, V2);
      if (!V1.IsSame(DegVertex) && !V2.IsSame(DegVertex))
        break;
    }
    TopoDS_Vertex V1, V2;
    TopExp::Vertices(OldEdge, V1, V2);
    NCollection_List<TopoDS_Shape> LV1, LV2;
    LV1.Append(Vf1.Oriented(TopAbs_FORWARD));
    LV2.Append(CurVertex.Oriented(TopAbs_FORWARD));
    BRepTools_Substitution theSubstitutor;
    theSubstitutor.Substitute(V1.Oriented(TopAbs_FORWARD), LV1);
    if (!V1.IsSame(V2))
      theSubstitutor.Substitute(V2.Oriented(TopAbs_FORWARD), LV2);
    theSubstitutor.Substitute(OldEdge.Oriented(TopAbs_FORWARD), NewEdges);
    theSubstitutor.Build(NewSphericalFace);
    if (theSubstitutor.IsCopied(NewSphericalFace))
    {
      const NCollection_List<TopoDS_Shape>& listSh = theSubstitutor.Copy(NewSphericalFace);
      NewSphericalFace                             = TopoDS::Face(listSh.First());
    }

    // Adding NewSphericalFace to the shell
    Explo.Init(myOffsetShape, TopAbs_SHELL);
    TopoDS_Shape theShell = Explo.Current();
    theShell.Free(true);
    BB.Add(theShell, NewSphericalFace);
    IsModified = true;
    if (!theShell.Closed())
    {
      if (BRep_Tool::IsClosed(theShell))
      {
        theShell.Closed(true);
      }
    }
  }
  //
  if (!IsModified)
  {
    return;
  }
  //
  if (myShape.ShapeType() == TopAbs_SOLID || myThickening)
  {
    // Explo.Init( myOffsetShape, TopAbs_SHELL );

    // if (Explo.More()) {
    //   TopoDS_Shape theShell = Explo.Current();
    //   theShell.Closed( true );
    // }

    int             NbShell = 0;
    TopoDS_Compound NC;
    TopoDS_Shape    S1;
    BB.MakeCompound(NC);

    TopoDS_Solid Sol;
    BB.MakeSolid(Sol);
    Sol.Closed(true);
    for (Explo.Init(myOffsetShape, TopAbs_SHELL); Explo.More(); Explo.Next())
    {
      TopoDS_Shell Sh = TopoDS::Shell(Explo.Current());
      // if (myThickening && myOffset > 0.)
      //   Sh.Reverse();
      NbShell++;
      if (Sh.Closed())
      {
        BB.Add(Sol, Sh);
      }
      else
      {
        BB.Add(NC, Sh);
        if (NbShell == 1)
        {
          S1 = Sh;
        }
      }
    }
    int  nbs       = Sol.NbChildren();
    bool SolIsNull = (nbs == 0);
    // Checking solid
    if (nbs > 1)
    {
      BRepCheck_Analyzer aCheck(Sol, false);
      if (!aCheck.IsValid())
      {
        NCollection_List<TopoDS_Shape> aSolList;
        CorrectSolid(Sol, aSolList);
        if (!aSolList.IsEmpty())
        {
          BB.Add(NC, Sol);
          NCollection_List<TopoDS_Shape>::Iterator aSLIt(aSolList);
          for (; aSLIt.More(); aSLIt.Next())
          {
            BB.Add(NC, aSLIt.Value());
          }
          SolIsNull = true;
        }
      }
    }
    //
    bool NCIsNull = (NC.NbChildren() == 0);
    if ((!SolIsNull) && (!NCIsNull))
    {
      BB.Add(NC, Sol);
      myOffsetShape = NC;
    }
    else if (SolIsNull && (!NCIsNull))
    {
      if (NbShell == 1)
      {
        myOffsetShape = S1;
      }
      else
      {
        myOffsetShape = NC;
      }
    }
    else if ((!SolIsNull) && NCIsNull)
    {
      myOffsetShape = Sol;
    }
    else
    {
      myOffsetShape = NC;
    }
  }
}

//=================================================================================================

void BRepOffset_MakeOffset::Intersection3D(BRepOffset_Inter3d&          Inter,
                                           const Message_ProgressRange& theRange)
{
#ifdef OCCT_DEBUG
  if (ChronBuild)
  {
    std::cout << " INTERSECTION 3D:" << std::endl;
    Clock.Reset();
    Clock.Start();
  }
#endif
  Message_ProgressScope aPS(theRange, nullptr, (myFaces.Extent() && myJoin == GeomAbs_Arc) ? 2 : 1);

  // In the Complete Intersection mode, implemented currently for planar
  // solids only, there is no need to intersect the faces here.
  // This intersection will be performed in the method BuildShellsCompleteInter
  // where the special treatment is applied to produced faces.
  //
  // Make sure to match the parameters in which the method
  // BuildShellsCompleteInter is called.
  if (myInter && (myJoin == GeomAbs_Intersection) && myIsPlanar && !myThickening
      && myFaces.IsEmpty() && IsSolid(myShape))
    return;

  NCollection_List<TopoDS_Shape> OffsetFaces; // list of faces // created.
  MakeList(OffsetFaces, myInitOffsetFace, myFaces);

  if (!myFaces.IsEmpty())
  {
    bool InSide = (myOffset < 0.); // Temporary
    // it is necessary to calculate Inside taking account of the concavity or convexity of edges
    // between the cap and the part.

    if (myJoin == GeomAbs_Arc)
      Inter.ContextIntByArc(myFaces,
                            InSide,
                            myAnalyse,
                            myInitOffsetFace,
                            myInitOffsetEdge,
                            aPS.Next());
  }
  if (myInter)
  {
    //-------------
    // Complete.
    //-------------
    Inter.CompletInt(OffsetFaces, myInitOffsetFace, aPS.Next());
    if (!aPS.More())
    {
      myError = BRepOffset_UserBreak;
      return;
    }
    NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& NewEdges = Inter.NewEdges();
    if (myJoin == GeomAbs_Intersection)
    {
      BRepOffset_Tool::CorrectOrientation(myFaceComp,
                                          NewEdges,
                                          myAsDes,
                                          myInitOffsetFace,
                                          myOffset);
    }
  }
  else
  {
    //--------------------------------
    // Only between neighbor faces.
    //--------------------------------
    Inter.ConnexIntByArc(OffsetFaces, myFaceComp, myAnalyse, myInitOffsetFace, aPS.Next());
    if (!aPS.More())
    {
      myError = BRepOffset_UserBreak;
      return;
    }
  }
#ifdef OCCT_DEBUG
  if (ChronBuild)
    Clock.Show();
#endif
}

//=================================================================================================

void BRepOffset_MakeOffset::Intersection2D(
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& Modif,
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& NewEdges,
  const Message_ProgressRange&                                         theRange)
{
#ifdef OCCT_DEBUG
  if (ChronBuild)
  {
    std::cout << " INTERSECTION 2D:" << std::endl;
    Clock.Reset();
    Clock.Start();
  }
#endif
  //--------------------------------------------------------
  // calculate intersections2d on faces concerned by
  // intersection3d
  //---------------------------------------------------------
  // NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator it(Modif);
  //-----------------------------------------------
  // Intersection of edges 2 by 2.
  //-----------------------------------------------
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
                        aDMVV;
  int                   i;
  Message_ProgressScope aPS(theRange, "Intersection 2D", Modif.Extent());
  for (i = 1; i <= Modif.Extent(); i++)
  {
    if (!aPS.More())
    {
      myError = BRepOffset_UserBreak;
      return;
    }
    const TopoDS_Face& F = TopoDS::Face(Modif(i));
    BRepOffset_Inter2d::Compute(myAsDes, F, NewEdges, myTol, myEdgeIntEdges, aDMVV, aPS.Next());
  }
  //
  BRepOffset_Inter2d::FuseVertices(aDMVV, myAsDes, myImageVV);
  //
#ifdef OCCT_DEBUG
  if (AffichInt2d)
  {
    DEBVerticesControl(NewEdges, myAsDes);
  }
  if (ChronBuild)
    Clock.Show();
#endif
}

//=================================================================================================

void BRepOffset_MakeOffset::MakeLoops(
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& Modif,
  const Message_ProgressRange&                                   theRange)
{
#ifdef OCCT_DEBUG
  if (ChronBuild)
  {
    std::cout << " DEBOUCLAGE 2D:" << std::endl;
    Clock.Reset();
    Clock.Start();
  }
#endif
  // NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator    it(Modif);
  NCollection_List<TopoDS_Shape> LF, LC;
  //-----------------------------------------
  // unwinding of faces // modified.
  //-----------------------------------------
  int i;
  for (i = 1; i <= Modif.Extent(); i++)
  {
    if (!myFaces.Contains(Modif(i)))
      LF.Append(Modif(i));
  }
  //
  Message_ProgressScope aPS(theRange, nullptr, LF.Extent() + myFaces.Extent());
  if ((myJoin == GeomAbs_Intersection) && myInter && myIsPlanar)
  {
    BuildSplitsOfTrimmedFaces(LF, myAsDes, myImageOffset, aPS.Next(LF.Extent()));
  }
  else
  {
    myMakeLoops.Build(LF, myAsDes, myImageOffset, myImageVV, aPS.Next(LF.Extent()));
  }
  if (!aPS.More())
  {
    myError = BRepOffset_UserBreak;
    return;
  }

  //-----------------------------------------
  // unwinding of caps.
  //-----------------------------------------
  for (i = 1; i <= myFaces.Extent(); i++)
    LC.Append(myFaces(i));

  bool InSide = 1;
  if (myOffset > 0)
    InSide = 0;
  myMakeLoops.BuildOnContext(LC, myAnalyse, myAsDes, myImageOffset, InSide, aPS.Next(LC.Extent()));

#ifdef OCCT_DEBUG
  if (ChronBuild)
    Clock.Show();
#endif
}

//=======================================================================
// function : MakeFaces
// purpose  : Reconstruction of topologically unchanged faces that
//           share edges that were reconstructed.
//=======================================================================

void BRepOffset_MakeOffset::MakeFaces(
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& /*Modif*/,
  const Message_ProgressRange& theRange)
{
#ifdef OCCT_DEBUG
  if (ChronBuild)
  {
    std::cout << " RECONSTRUCTION OF FACES:" << std::endl;
    Clock.Reset();
    Clock.Start();
  }
#endif
  NCollection_List<TopoDS_Shape>::Iterator itr;
  const NCollection_List<TopoDS_Shape>&    Roots = myInitOffsetFace.Roots();
  NCollection_List<TopoDS_Shape>           LOF;
  //----------------------------------
  // Loop on all faces //.
  //----------------------------------
  for (itr.Initialize(Roots); itr.More(); itr.Next())
  {
    TopoDS_Face F = TopoDS::Face(myInitOffsetFace.Image(itr.Value()).First());
    if (!myImageOffset.HasImage(F))
    {
      LOF.Append(F);
    }
  }
  //
  Message_ProgressScope aPS(theRange, nullptr, 1);
  if ((myJoin == GeomAbs_Intersection) && myInter && myIsPlanar)
  {
    BuildSplitsOfTrimmedFaces(LOF, myAsDes, myImageOffset, aPS.Next());
  }
  else
  {
    myMakeLoops.BuildFaces(LOF, myAsDes, myImageOffset, aPS.Next());
  }
  if (!aPS.More())
  {
    myError = BRepOffset_UserBreak;
    return;
  }
#ifdef OCCT_DEBUG
  if (ChronBuild)
    Clock.Show();
#endif
}

//=======================================================================
// function : UpdateInitOffset
// purpose  : Update and cleaning of myInitOffset
//=======================================================================
static void UpdateInitOffset(BRepAlgo_Image&         myInitOffset,
                             BRepAlgo_Image&         myImageOffset,
                             const TopoDS_Shape&     myOffsetShape,
                             const TopAbs_ShapeEnum& theShapeType) // skv
{
  BRepAlgo_Image                           NIOF;
  const NCollection_List<TopoDS_Shape>&    Roots = myInitOffset.Roots();
  NCollection_List<TopoDS_Shape>::Iterator it(Roots);
  for (; it.More(); it.Next())
  {
    NIOF.SetRoot(it.Value());
  }
  for (it.Initialize(Roots); it.More(); it.Next())
  {
    const TopoDS_Shape&            SI = it.Value();
    NCollection_List<TopoDS_Shape> LI;
    NCollection_List<TopoDS_Shape> L1;
    myInitOffset.LastImage(SI, L1);
    NCollection_List<TopoDS_Shape>::Iterator itL1(L1);
    for (; itL1.More(); itL1.Next())
    {
      const TopoDS_Shape&            O1 = itL1.Value();
      NCollection_List<TopoDS_Shape> L2;
      myImageOffset.LastImage(O1, L2);
      LI.Append(L2);
    }
    NIOF.Bind(SI, LI);
  }
  //  Modified by skv - Mon Apr  4 18:17:27 2005 Begin
  //  Supporting history.
  //   NIOF.Filter(myOffsetShape,TopAbs_FACE);
  NIOF.Filter(myOffsetShape, theShapeType);
  //  Modified by skv - Mon Apr  4 18:17:27 2005 End
  myInitOffset = NIOF;
}

//=================================================================================================

void BRepOffset_MakeOffset::MakeMissingWalls(const Message_ProgressRange& theRange)
{
  // clang-format off
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> Contours; //Start vertex + list of connected edges (free boundary)
  // clang-format on
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>
         MapEF; // Edges of contours: edge + face
  double OffsetVal = std::abs(myOffset);

  FillContours(myFaceComp, myAnalyse, Contours, MapEF);

  Message_ProgressScope aPS(theRange, "Making missing walls", Contours.Extent());
  for (int ic = 1; ic <= Contours.Extent(); ic++, aPS.Next())
  {
    if (!aPS.More())
    {
      myError = BRepOffset_UserBreak;
      return;
    }
    TopoDS_Vertex                            StartVertex = TopoDS::Vertex(Contours.FindKey(ic));
    TopoDS_Edge                              StartEdge;
    const NCollection_List<TopoDS_Shape>&    aContour = Contours(ic);
    NCollection_List<TopoDS_Shape>::Iterator itl(aContour);
    bool                                     FirstStep = true;
    TopoDS_Edge                              PrevEdge;
    TopoDS_Vertex                            PrevVertex         = StartVertex;
    bool                                     isBuildFromScratch = false; // Problems with edges.
    for (; itl.More(); itl.Next())
    {
      TopoDS_Edge anEdge      = TopoDS::Edge(itl.Value());
      TopoDS_Face aFaceOfEdge = TopoDS::Face(MapEF(anEdge));

      // Check for offset existence.
      if (!myInitOffsetEdge.HasImage(anEdge))
        continue;

      // Check for existence of two different vertices.
      NCollection_List<TopoDS_Shape> LOE, LOE2;
      myInitOffsetEdge.LastImage(anEdge, LOE);
      myImageOffset.LastImage(LOE.Last(), LOE2);
      TopoDS_Edge   OE = TopoDS::Edge(LOE2.Last());
      TopoDS_Vertex V1, V2, V3, V4;
      TopExp::Vertices(OE, V4, V3);
      TopExp::Vertices(anEdge, V1, V2);
      double                        aF, aL;
      const occ::handle<Geom_Curve> aC = BRep_Tool::Curve(anEdge, aF, aL);
      if (V3.IsNull() && V4.IsNull())
      {
        // Initially offset edge is created without vertices.
        // Then edge is trimmed by intersection line between
        // two adjacent extended offset faces and get vertices.
        // When intersection lines are invalid for any reason,
        // (one reason is mixed connectivity of faces)
        // algorithm of cutting offset edge by intersection line
        // can fail and offset edge cannot get vertices.
        // Following workaround is only to avoid exception if V3 and V4 are Null
        // Vertex points are invalid.
        double             anOEF, anOEL;
        TopAbs_Orientation anOEOri = OE.Orientation();
        OE.Orientation(TopAbs_FORWARD);
        occ::handle<Geom_Curve> anOEC = BRep_Tool::Curve(OE, anOEF, anOEL);
        BRep_Builder            aBB;
        gp_Pnt                  aP1 = anOEC->Value(aF);
        gp_Pnt                  aP2 = anOEC->Value(aL);
        TopoDS_Vertex           anOEV1, anOEV2;
        double                  aTol = std::max(BRep_Tool::Tolerance(V1), BRep_Tool::Tolerance(V2));
        aBB.MakeVertex(anOEV1, aP1, aTol);
        anOEV1.Orientation(TopAbs_FORWARD);
        aBB.MakeVertex(anOEV2, aP2, aTol);
        anOEV2.Orientation(TopAbs_REVERSED);
        aBB.Add(OE, anOEV1);
        aBB.Add(OE, anOEV2);
        aBB.Range(OE, aF, aL);
        OE.Orientation(anOEOri);
        TopExp::Vertices(OE, V4, V3);
      }
      if (!aC.IsNull() && (!aC->IsClosed() && !aC->IsPeriodic()))
      {
        gp_Pnt aPntF  = BRep_Tool::Pnt(V1);
        gp_Pnt aPntL  = BRep_Tool::Pnt(V2);
        double aDistE = aPntF.SquareDistance(aPntL);
        if (aDistE < Precision::SquareConfusion())
        {
          // Bad case: non closed, but vertexes mapped to same 3d point.
          continue;
        }

        double anEdgeTol = BRep_Tool::Tolerance(anEdge);
        if (aDistE < anEdgeTol)
        {
          // Potential problems not detected via checkshape.
          gp_Pnt aPntOF = BRep_Tool::Pnt(V4);
          gp_Pnt aPntOL = BRep_Tool::Pnt(V3);
          if (aPntOF.SquareDistance(aPntOL) > gp::Resolution())
          {
            // To avoid computation of complex analytical continuation of Sin / ArcSin.
            double aSinValue = std::min(2 * anEdgeTol / aPntOF.Distance(aPntOL), 1.0);
            double aMaxAngle = std::min(std::abs(std::asin(aSinValue)), M_PI_4); // Maximal angle.
            double aCurrentAngle = gp_Vec(aPntF, aPntL).Angle(gp_Vec(aPntOF, aPntOL));
            if (aC->IsKind(STANDARD_TYPE(Geom_Line)) && std::abs(aCurrentAngle) > aMaxAngle)
            {
              // anEdge not collinear to offset edge.
              isBuildFromScratch = true;
              myIsPerformSewing  = true;
              continue;
            }
          }
        }
      }

      bool ToReverse = false;
      if (!V1.IsSame(PrevVertex))
      {
        TopoDS_Vertex aVtx = V1;
        V1                 = V2;
        V2                 = aVtx;
        aVtx               = V3;
        V3                 = V4;
        V4                 = aVtx;
        ToReverse          = true;
      }

      OE.Orientation(TopAbs::Reverse(anEdge.Orientation()));
      TopoDS_Edge E3, E4;
      bool        ArcOnV2 = ((myJoin == GeomAbs_Arc) && (myInitOffsetEdge.HasImage(V2)));
      if (FirstStep || isBuildFromScratch)
      {
        E4 = BRepLib_MakeEdge(V1, V4);
        if (FirstStep)
          StartEdge = E4;
      }
      else
        E4 = PrevEdge;
      if (V2.IsSame(StartVertex) && !ArcOnV2)
        E3 = StartEdge;
      else
        E3 = BRepLib_MakeEdge(V2, V3);
      E4.Reverse();

      if (isBuildFromScratch)
      {
        E3.Reverse();
        E4.Reverse();
      }

      TopoDS_Shape       localAnEdge = anEdge.Oriented(TopAbs_FORWARD);
      const TopoDS_Edge& anEdgeFWD   = TopoDS::Edge(localAnEdge);
      double             ParV1       = BRep_Tool::Parameter(V1, anEdgeFWD);
      double             ParV2       = BRep_Tool::Parameter(V2, anEdgeFWD);
      BRep_Builder       BB;
      TopoDS_Wire        theWire;
      BB.MakeWire(theWire);
      if (ToReverse)
      {
        BB.Add(theWire, anEdge.Reversed());
        BB.Add(theWire, E3.Reversed());
        BB.Add(theWire, OE.Reversed());
        BB.Add(theWire, E4.Reversed());
      }
      else
      {
        BB.Add(theWire, anEdge);
        BB.Add(theWire, E3);
        BB.Add(theWire, OE);
        BB.Add(theWire, E4);
      }

      BRepLib::BuildCurves3d(theWire, myTol);
      theWire.Closed(true);
      TopoDS_Face               NewFace;
      occ::handle<Geom_Surface> theSurf;
      BRepAdaptor_Curve         BAcurve(anEdge);
      BRepAdaptor_Curve         BAcurveOE(OE);
      double                    fpar      = BAcurve.FirstParameter();
      double                    lpar      = BAcurve.LastParameter();
      gp_Pnt                    PonE      = BAcurve.Value(fpar);
      gp_Pnt                    PonOE     = BAcurveOE.Value(fpar);
      gp_Dir                    OffsetDir = gce_MakeDir(PonE, PonOE);
      occ::handle<Geom2d_Line>  EdgeLine2d, OELine2d, aLine2d, aLine2d2;
      bool                      IsPlanar = false;
      if (BAcurve.GetType() == GeomAbs_Circle && BAcurveOE.GetType() == GeomAbs_Circle)
      {
        gp_Circ aCirc   = BAcurve.Circle();
        gp_Circ aCircOE = BAcurveOE.Circle();
        gp_Lin  anAxisLine(aCirc.Axis());
        gp_Dir  CircAxisDir = aCirc.Axis().Direction();
        if (aCirc.Axis().IsParallel(aCircOE.Axis(), Precision::Confusion())
            && anAxisLine.Contains(aCircOE.Location(), Precision::Confusion()))
        { // cylinder, plane or cone
          if (std::abs(aCirc.Radius() - aCircOE.Radius())
              <= Precision::Confusion()) // case of cylinder
            theSurf = GC_MakeCylindricalSurface(aCirc).Value();
          else if (aCirc.Location().Distance(aCircOE.Location()) <= Precision::Confusion())
          { // case of plane
            IsPlanar = true;
            //
            gp_Pnt PonEL = BAcurve.Value(lpar);
            if (PonEL.Distance(PonE) <= Precision::PConfusion())
            {
              bool                    bIsHole;
              TopoDS_Edge             aE1, aE2;
              TopoDS_Wire             aW1, aW2;
              occ::handle<Geom_Plane> aPL;
              IntTools_FClass2d       aClsf;
              //
              if (aCirc.Radius() > aCircOE.Radius())
              {
                aE1 = anEdge;
                aE2 = OE;
              }
              else
              {
                aE1 = OE;
                aE2 = anEdge;
              }
              //
              BB.MakeWire(aW1);
              BB.Add(aW1, aE1);
              BB.MakeWire(aW2);
              BB.Add(aW2, aE2);
              //
              aPL = new Geom_Plane(aCirc.Location(), CircAxisDir);
              for (int i = 0; i < 2; ++i)
              {
                TopoDS_Wire& aW = (i == 0) ? aW1 : aW2;
                TopoDS_Edge& aE = (i == 0) ? aE1 : aE2;
                //
                TopoDS_Face aFace;
                BB.MakeFace(aFace, aPL, Precision::Confusion());
                BB.Add(aFace, aW);
                aClsf.Init(aFace, Precision::Confusion());
                bIsHole = aClsf.IsHole();
                if ((bIsHole && !i) || (!bIsHole && i))
                {
                  aW.Nullify();
                  BB.MakeWire(aW);
                  BB.Add(aW, aE.Reversed());
                }
              }
              //
              BB.MakeFace(NewFace, aPL, Precision::Confusion());
              BB.Add(NewFace, aW1);
              BB.Add(NewFace, aW2);
            }
          }
          else // case of cone
          {
            gp_Cone theCone =
              gce_MakeCone(aCirc.Location(), aCircOE.Location(), aCirc.Radius(), aCircOE.Radius());
            gp_Ax3 theAx3(aCirc.Position());
            if (CircAxisDir * theCone.Axis().Direction() < 0.)
            {
              theAx3.ZReverse();
              CircAxisDir.Reverse();
            }
            theCone.SetPosition(theAx3);
            theSurf = new Geom_ConicalSurface(theCone);
          }
          if (!IsPlanar)
          {
            TopLoc_Location Loc;
            EdgeLine2d = new Geom2d_Line(gp_Pnt2d(0., 0.), gp_Dir2d(gp_Dir2d::D::X));
            BB.UpdateEdge(anEdge, EdgeLine2d, theSurf, Loc, Precision::Confusion());
            double Coeff = (OffsetDir * CircAxisDir > 0.) ? 1. : -1.;
            OELine2d = new Geom2d_Line(gp_Pnt2d(0., OffsetVal * Coeff), gp_Dir2d(gp_Dir2d::D::X));
            BB.UpdateEdge(OE, OELine2d, theSurf, Loc, Precision::Confusion());
            aLine2d  = new Geom2d_Line(gp_Pnt2d(ParV2, 0.), gp_Dir2d(0., Coeff));
            aLine2d2 = new Geom2d_Line(gp_Pnt2d(ParV1, 0.), gp_Dir2d(0., Coeff));
            if (E3.IsSame(E4))
            {
              if (Coeff > 0.)
                BB.UpdateEdge(E3, aLine2d, aLine2d2, theSurf, Loc, Precision::Confusion());
              else
              {
                BB.UpdateEdge(E3, aLine2d2, aLine2d, theSurf, Loc, Precision::Confusion());
                theWire.Nullify();
                BB.MakeWire(theWire);
                BB.Add(theWire, anEdge.Oriented(TopAbs_REVERSED));
                BB.Add(theWire, E4);
                BB.Add(theWire, OE.Oriented(TopAbs_FORWARD));
                BB.Add(theWire, E3);
                theWire.Closed(true);
              }
            }
            else
            {
              BB.SameParameter(E3, false);
              BB.SameRange(E3, false);
              BB.SameParameter(E4, false);
              BB.SameRange(E4, false);
              BB.UpdateEdge(E3, aLine2d, theSurf, Loc, Precision::Confusion());
              BB.Range(E3, theSurf, Loc, 0., OffsetVal);
              BB.UpdateEdge(E4, aLine2d2, theSurf, Loc, Precision::Confusion());
              BB.Range(E4, theSurf, Loc, 0., OffsetVal);
            }
            NewFace = BRepLib_MakeFace(theSurf, theWire);
          }
        } // cylinder or cone
      } // if both edges are arcs of circles
      if (NewFace.IsNull())
      {
        double anEdgeTol = BRep_Tool::Tolerance(anEdge);
        // Tolerances of input shape should not be increased by BRepLib_MakeFace
        BRepLib_FindSurface aFindPlane(theWire, anEdgeTol, true); // only plane
        IsPlanar = false;
        if (aFindPlane.Found() && aFindPlane.ToleranceReached() <= anEdgeTol)
        {
          double                  f, l;
          occ::handle<Geom_Curve> aGC      = BRep_Tool::Curve(anEdge, f, l);
          occ::handle<Geom_Plane> aPln     = occ::down_cast<Geom_Plane>(aFindPlane.Surface());
          double                  aMaxDist = ComputeMaxDist(aPln->Pln(), aGC, f, l);
          if (aMaxDist <= anEdgeTol)
          {
            BRepLib_MakeFace MF(aPln->Pln(), theWire);
            if (MF.IsDone())
            {
              NewFace = MF.Face();
              TopoDS_Iterator anItE(theWire);
              for (; anItE.More(); anItE.Next())
              {
                const TopoDS_Edge& anE = TopoDS::Edge(anItE.Value());
                if (anE.IsSame(anEdge))
                  continue;
                aGC      = BRep_Tool::Curve(anE, f, l);
                aMaxDist = ComputeMaxDist(aPln->Pln(), aGC, f, l);
                BB.UpdateEdge(anE, aMaxDist);
              }
              IsPlanar = true;
            }
          }
        }
        //
        if (!IsPlanar) // Extrusion (by thrusections)
        {
          occ::handle<Geom_Curve>        EdgeCurve   = BRep_Tool::Curve(anEdge, fpar, lpar);
          occ::handle<Geom_TrimmedCurve> TrEdgeCurve = new Geom_TrimmedCurve(EdgeCurve, fpar, lpar);
          double                         fparOE, lparOE;
          occ::handle<Geom_Curve>        OffsetCurve = BRep_Tool::Curve(OE, fparOE, lparOE);
          occ::handle<Geom_TrimmedCurve> TrOffsetCurve =
            new Geom_TrimmedCurve(OffsetCurve, fparOE, lparOE);
          GeomFill_Generator ThrusecGenerator;
          ThrusecGenerator.AddCurve(TrEdgeCurve);
          ThrusecGenerator.AddCurve(TrOffsetCurve);
          ThrusecGenerator.Perform(Precision::PConfusion());
          theSurf = ThrusecGenerator.Surface();
          double Uf, Ul, Vf, Vl;
          theSurf->Bounds(Uf, Ul, Vf, Vl);
          TopLoc_Location Loc;
          EdgeLine2d = new Geom2d_Line(gp_Pnt2d(0., Vf), gp_Dir2d(gp_Dir2d::D::X));
          BB.UpdateEdge(anEdge, EdgeLine2d, theSurf, Loc, Precision::Confusion());
          OELine2d = new Geom2d_Line(gp_Pnt2d(0., Vl), gp_Dir2d(gp_Dir2d::D::X));
          BB.UpdateEdge(OE, OELine2d, theSurf, Loc, Precision::Confusion());
          double UonV1 = (ToReverse) ? Ul : Uf;
          double UonV2 = (ToReverse) ? Uf : Ul;
          aLine2d      = new Geom2d_Line(gp_Pnt2d(UonV2, 0.), gp_Dir2d(gp_Dir2d::D::Y));
          aLine2d2     = new Geom2d_Line(gp_Pnt2d(UonV1, 0.), gp_Dir2d(gp_Dir2d::D::Y));
          if (E3.IsSame(E4))
          {
            BB.UpdateEdge(E3, aLine2d, aLine2d2, theSurf, Loc, Precision::Confusion());
            occ::handle<Geom_Curve> BSplC34 = theSurf->UIso(Uf);
            BB.UpdateEdge(E3, BSplC34, Precision::Confusion());
            BB.Range(E3, Vf, Vl);
          }
          else
          {
            BB.SameParameter(E3, false);
            BB.SameRange(E3, false);
            BB.SameParameter(E4, false);
            BB.SameRange(E4, false);
            BB.UpdateEdge(E3, aLine2d, theSurf, Loc, Precision::Confusion());
            BB.Range(E3, theSurf, Loc, Vf, Vl);
            BB.UpdateEdge(E4, aLine2d2, theSurf, Loc, Precision::Confusion());
            BB.Range(E4, theSurf, Loc, Vf, Vl);
            occ::handle<Geom_Curve> BSplC3 = theSurf->UIso(UonV2);
            BB.UpdateEdge(E3, BSplC3, Precision::Confusion());
            BB.Range(E3, Vf, Vl, true); // only for 3d curve
            occ::handle<Geom_Curve> BSplC4 = theSurf->UIso(UonV1);
            BB.UpdateEdge(E4, BSplC4, Precision::Confusion());
            BB.Range(E4, Vf, Vl, true); // only for 3d curve
          }
          NewFace = BRepLib_MakeFace(theSurf, theWire);
        }
      }
      if (!IsPlanar)
      {
        double          fparOE = BAcurveOE.FirstParameter();
        double          lparOE = BAcurveOE.LastParameter();
        TopLoc_Location Loc;
        if (std::abs(fpar - fparOE) > Precision::Confusion())
        {
          const TopoDS_Edge& anE4   = (ToReverse) ? E3 : E4;
          gp_Pnt2d           fp2d   = EdgeLine2d->Value(fpar);
          gp_Pnt2d           fp2dOE = OELine2d->Value(fparOE);
          aLine2d2                  = GCE2d_MakeLine(fp2d, fp2dOE).Value();
          occ::handle<Geom_Curve>          aCurve;
          double                           FirstPar = 0., LastPar = fp2d.Distance(fp2dOE);
          Geom2dAdaptor_Curve              AC2d(aLine2d2, FirstPar, LastPar);
          GeomAdaptor_Surface              GAsurf(theSurf);
          occ::handle<Geom2dAdaptor_Curve> HC2d  = new Geom2dAdaptor_Curve(AC2d);
          occ::handle<GeomAdaptor_Surface> HSurf = new GeomAdaptor_Surface(GAsurf);
          Adaptor3d_CurveOnSurface         ConS(HC2d, HSurf);
          double                           max_deviation = 0., average_deviation;
          GeomLib::BuildCurve3d(Precision::Confusion(),
                                ConS,
                                FirstPar,
                                LastPar,
                                aCurve,
                                max_deviation,
                                average_deviation);
          BB.UpdateEdge(anE4, aCurve, max_deviation);
          BB.UpdateEdge(anE4, aLine2d2, theSurf, Loc, max_deviation);
          BB.Range(anE4, FirstPar, LastPar);
        }
        if (std::abs(lpar - lparOE) > Precision::Confusion())
        {
          const TopoDS_Edge& anE3   = (ToReverse) ? E4 : E3;
          gp_Pnt2d           lp2d   = EdgeLine2d->Value(lpar);
          gp_Pnt2d           lp2dOE = OELine2d->Value(lparOE);
          aLine2d                   = GCE2d_MakeLine(lp2d, lp2dOE).Value();
          occ::handle<Geom_Curve>          aCurve;
          double                           FirstPar = 0., LastPar = lp2d.Distance(lp2dOE);
          Geom2dAdaptor_Curve              AC2d(aLine2d, FirstPar, LastPar);
          GeomAdaptor_Surface              GAsurf(theSurf);
          occ::handle<Geom2dAdaptor_Curve> HC2d  = new Geom2dAdaptor_Curve(AC2d);
          occ::handle<GeomAdaptor_Surface> HSurf = new GeomAdaptor_Surface(GAsurf);
          Adaptor3d_CurveOnSurface         ConS(HC2d, HSurf);
          double                           max_deviation = 0., average_deviation;
          GeomLib::BuildCurve3d(Precision::Confusion(),
                                ConS,
                                FirstPar,
                                LastPar,
                                aCurve,
                                max_deviation,
                                average_deviation);
          BB.UpdateEdge(anE3, aCurve, max_deviation);
          BB.UpdateEdge(anE3, aLine2d, theSurf, Loc, max_deviation);
          BB.Range(anE3, FirstPar, LastPar);
        }
      }

      if (!IsPlanar)
      {
        // For planar faces these operations are useless,
        // because there are no curves on surface
        BRepLib::SameParameter(NewFace);
        BRepTools::Update(NewFace);
      }
      // Check orientation
      TopAbs_Orientation anOr        = OrientationOfEdgeInFace(anEdge, aFaceOfEdge);
      TopAbs_Orientation OrInNewFace = OrientationOfEdgeInFace(anEdge, NewFace);
      if (OrInNewFace != TopAbs::Reverse(anOr))
        NewFace.Reverse();
      ///////////////////
      myWalls.Append(NewFace);
      if (ArcOnV2)
      {
        TopoDS_Edge   anArc = TopoDS::Edge(myInitOffsetEdge.Image(V2).First());
        TopoDS_Vertex arcV1, arcV2;
        TopExp::Vertices(anArc, arcV1, arcV2);
        bool ArcReverse = false;
        if (!arcV1.IsSame(V3))
        {
          TopoDS_Vertex aVtx = arcV1;
          arcV1              = arcV2;
          arcV2              = aVtx;
          ArcReverse         = true;
        }
        TopoDS_Edge EA1, EA2;
        // EA1 = (ToReverse)? E3 : TopoDS::Edge(E3.Reversed());
        EA1 = E3;
        EA1.Reverse();
        if (ToReverse)
          EA1.Reverse();
        //////////////////////////////////////////////////////
        if (V2.IsSame(StartVertex))
          EA2 = StartEdge;
        else
          EA2 = BRepLib_MakeEdge(V2, arcV2);
        anArc.Orientation(((ArcReverse) ? TopAbs_REVERSED : TopAbs_FORWARD));
        if (EA1.Orientation() == TopAbs_REVERSED)
          anArc.Reverse();
        EA2.Orientation(TopAbs::Reverse(EA1.Orientation()));
        TopoDS_Wire arcWire;
        BB.MakeWire(arcWire);
        BB.Add(arcWire, EA1);
        BB.Add(arcWire, anArc);
        BB.Add(arcWire, EA2);
        BRepLib::BuildCurves3d(arcWire, myTol);
        arcWire.Closed(true);
        TopoDS_Face arcFace = BRepLib_MakeFace(arcWire, true);
        BRepTools::Update(arcFace);
        myWalls.Append(arcFace);
        TopoDS_Shape       localEA2 = EA2.Oriented(TopAbs_FORWARD);
        const TopoDS_Edge& CEA2     = TopoDS::Edge(localEA2);
        PrevEdge                    = CEA2;
        PrevVertex                  = V2;
      }
      else
      {
        if (isBuildFromScratch)
        {
          PrevEdge           = TopoDS::Edge(E4);
          PrevVertex         = V1;
          isBuildFromScratch = false;
        }
        else
        {
          PrevEdge   = E3;
          PrevVertex = V2;
        }
      }
      FirstStep = false;
    }
  }
}

//=================================================================================================

void BRepOffset_MakeOffset::MakeShells(const Message_ProgressRange& theRange)
{
#ifdef OCCT_DEBUG
  if (ChronBuild)
  {
    std::cout << " RECONSTRUCTION OF SHELLS:" << std::endl;
    Clock.Reset();
    Clock.Start();
  }
#endif
  //
  Message_ProgressScope aPS(theRange, "Making shells", 1);
  // Prepare list of splits of the offset faces to make the shells
  NCollection_List<TopoDS_Shape>           aLSF;
  const NCollection_List<TopoDS_Shape>&    R = myImageOffset.Roots();
  NCollection_List<TopoDS_Shape>::Iterator it(R);
  //
  for (; it.More(); it.Next())
  {
    if (!aPS.More())
    {
      myError = BRepOffset_UserBreak;
      return;
    }
    TopoDS_Shape aF = it.Value();
    if (myThickening) // offsetted faces must change their orientations
      aF.Reverse();
    //
    NCollection_List<TopoDS_Shape> Image;
    myImageOffset.LastImage(aF, Image);
    NCollection_List<TopoDS_Shape>::Iterator it2(Image);
    for (; it2.More(); it2.Next())
    {
      const TopoDS_Shape& aFIm = it2.Value();
      aLSF.Append(aFIm);
    }
  }
  //
  if (myThickening)
  {
    TopExp_Explorer Explo(myShape, TopAbs_FACE);
    for (; Explo.More(); Explo.Next())
    {
      const TopoDS_Shape& aF = Explo.Current();
      aLSF.Append(aF);
    }
    //
    it.Initialize(myWalls);
    for (; it.More(); it.Next())
    {
      const TopoDS_Shape& aF = it.Value();
      aLSF.Append(aF);
    }
  }
  //
  if (aLSF.IsEmpty())
  {
    return;
  }
  //
  bool bDone = false;
  if ((myJoin == GeomAbs_Intersection) && myInter && !myThickening && myFaces.IsEmpty()
      && IsSolid(myShape) && myIsPlanar)
  {
    //
    TopoDS_Shape aShells;
    bDone = BuildShellsCompleteInter(aLSF, myImageOffset, aShells, aPS.Next());
    if (bDone)
    {
      myOffsetShape = aShells;
    }
  }
  //
  if (!bDone)
  {
    BRepTools_Quilt                          Glue;
    NCollection_List<TopoDS_Shape>::Iterator aItLS(aLSF);
    for (; aItLS.More(); aItLS.Next())
    {
      Glue.Add(aItLS.Value());
    }
    myOffsetShape = Glue.Shells();
  }
  //
  // Set correct value for closed flag
  TopExp_Explorer Explo(myOffsetShape, TopAbs_SHELL);
  for (; Explo.More(); Explo.Next())
  {
    TopoDS_Shape aS = Explo.Current();
    if (!aS.Closed())
    {
      if (BRep_Tool::IsClosed(aS))
      {
        aS.Closed(true);
      }
    }
  }
}

//=================================================================================================

void BRepOffset_MakeOffset::MakeSolid(const Message_ProgressRange& theRange)
{
  if (myOffsetShape.IsNull())
    return;

  //  Modified by skv - Mon Apr  4 18:17:27 2005 Begin
  //  Supporting history.
  UpdateInitOffset(myInitOffsetFace, myImageOffset, myOffsetShape, TopAbs_FACE);
  UpdateInitOffset(myInitOffsetEdge, myImageOffset, myOffsetShape, TopAbs_EDGE);
  //  Modified by skv - Mon Apr  4 18:17:27 2005 End
  TopExp_Explorer exp;
  BRep_Builder    B;
  int             NbShell = 0;
  TopoDS_Compound NC;
  TopoDS_Shape    S1;
  B.MakeCompound(NC);

  Message_ProgressScope aPS(theRange, "Making solid", 1);

  TopoDS_Solid Sol;
  B.MakeSolid(Sol);
  Sol.Closed(true);
  bool aMakeSolid = (myShape.ShapeType() == TopAbs_SOLID) || myThickening;
  for (exp.Init(myOffsetShape, TopAbs_SHELL); exp.More(); exp.Next())
  {
    if (!aPS.More())
    {
      myError = BRepOffset_UserBreak;
      return;
    }
    TopoDS_Shell Sh = TopoDS::Shell(exp.Current());
    if (myThickening && myOffset > 0.)
      Sh.Reverse();
    NbShell++;
    if (Sh.Closed() && aMakeSolid)
    {
      B.Add(Sol, Sh);
    }
    else
    {
      B.Add(NC, Sh);
      if (NbShell == 1)
      {
        S1 = Sh;
      }
    }
  }
  int  nbs       = Sol.NbChildren();
  bool SolIsNull = (nbs == 0);
  // Checking solid
  if (nbs > 1)
  {
    BRepCheck_Analyzer aCheck(Sol, false);
    if (!aCheck.IsValid())
    {
      NCollection_List<TopoDS_Shape> aSolList;
      CorrectSolid(Sol, aSolList);
      if (!aSolList.IsEmpty())
      {
        B.Add(NC, Sol);
        NCollection_List<TopoDS_Shape>::Iterator aSLIt(aSolList);
        for (; aSLIt.More(); aSLIt.Next())
        {
          B.Add(NC, aSLIt.Value());
        }
        SolIsNull = true;
      }
    }
  }
  bool NCIsNull = (NC.NbChildren() == 0);
  if ((!SolIsNull) && (!NCIsNull))
  {
    B.Add(NC, Sol);
    myOffsetShape = NC;
  }
  else if (SolIsNull && (!NCIsNull))
  {
    if (NbShell == 1)
    {
      myOffsetShape = S1;
    }
    else
    {
      myOffsetShape = NC;
    }
  }
  else if ((!SolIsNull) && NCIsNull)
  {
    myOffsetShape = Sol;
  }
  else
  {
    myOffsetShape = NC;
  }
}

//=================================================================================================

void BRepOffset_MakeOffset::SelectShells()
{
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> FreeEdges;
  TopExp_Explorer                                        exp(myFaceComp, TopAbs_EDGE);
  //-------------------------------------------------------------
  // FreeEdges all edges that can have free border in the
  // parallel shell
  // 1 - free borders of myShape .
  //-------------------------------------------------------------
  for (; exp.More(); exp.Next())
  {
    const TopoDS_Edge&                    E  = TopoDS::Edge(exp.Current());
    const NCollection_List<TopoDS_Shape>& LA = myAnalyse.Ancestors(E);
    if (LA.Extent() < 2)
    {
      if (myAnalyse.Type(E).First().Type() == ChFiDS_FreeBound)
      {
        FreeEdges.Add(E);
      }
    }
  }
  // myShape has free borders and there are no caps
  // no unwinding 3d.
  if (!FreeEdges.IsEmpty() && myFaces.IsEmpty())
    return;

  myOffsetShape = BRepOffset_Tool::Deboucle3D(myOffsetShape, FreeEdges);
}

//=================================================================================================

const BRepAlgo_Image& BRepOffset_MakeOffset::OffsetFacesFromShapes() const
{
  return myInitOffsetFace;
}

//  Modified by skv - Tue Mar 15 16:20:43 2005 Begin

//=======================================================================
// function : GetJoinType
// purpose  : Query offset join type.
//=======================================================================

GeomAbs_JoinType BRepOffset_MakeOffset::GetJoinType() const
{
  return myJoin;
}

//=================================================================================================

const BRepAlgo_Image& BRepOffset_MakeOffset::OffsetEdgesFromShapes() const
{
  return myInitOffsetEdge;
}

//  Modified by skv - Tue Mar 15 16:20:43 2005 End

//=================================================================================================

const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& BRepOffset_MakeOffset::
  ClosingFaces() const
{
  return myOriginalFaces;
}

//=================================================================================================

void BRepOffset_MakeOffset::EncodeRegularity()
{
#ifdef OCCT_DEBUG
  if (ChronBuild)
  {
    std::cout << " CODING OF REGULARITIES:" << std::endl;
    Clock.Reset();
    Clock.Start();
  }
#endif

  if (myOffsetShape.IsNull())
    return;
  // find edges G1 in the result
  TopExp_Explorer exp(myOffsetShape, TopAbs_EDGE);

  BRep_Builder                                           B;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> MS;

  for (; exp.More(); exp.Next())
  {
    TopoDS_Edge OE = TopoDS::Edge(exp.Current());
    BRepLib::BuildCurve3d(OE, myTol);
    TopoDS_Edge ROE = OE;

    if (!MS.Add(OE))
      continue;

    if (myImageOffset.IsImage(OE))
      ROE = TopoDS::Edge(myImageOffset.Root(OE));

    const NCollection_List<TopoDS_Shape>& LofOF = myAsDes->Ascendant(ROE);

    if (LofOF.Extent() != 2)
    {
#ifdef OCCT_DEBUG_VERB
      std::cout << " Edge shared by " << LofOF.Extent() << " Faces" << std::endl;
#endif
      continue;
    }

    const TopoDS_Face& F1 = TopoDS::Face(LofOF.First());
    const TopoDS_Face& F2 = TopoDS::Face(LofOF.Last());

    if (F1.IsNull() || F2.IsNull())
      continue;

    const TopoDS_Shape& Root1 = myInitOffsetFace.Root(F1);
    const TopoDS_Shape& Root2 = myInitOffsetFace.Root(F2);

    TopAbs_ShapeEnum Type1 = Root1.ShapeType();
    TopAbs_ShapeEnum Type2 = Root2.ShapeType();

    if (F1.IsSame(F2))
    {
      if (BRep_Tool::IsClosed(OE, F1))
      {
        // Temporary Debug for the Bench.
        // Check with YFR.
        // In mode intersection, the edges are not coded in myInitOffsetEdge
        // so, manage case by case
        // Note DUB; for Hidden parts, it is NECESSARY to code CN
        // Analytic Surfaces.
        if (myJoin == GeomAbs_Intersection)
        {
          BRepAdaptor_Surface BS(F1, false);
          GeomAbs_SurfaceType SType = BS.GetType();
          if (SType == GeomAbs_Cylinder || SType == GeomAbs_Cone || SType == GeomAbs_Sphere
              || SType == GeomAbs_Torus)
          {
            B.Continuity(OE, F1, F1, GeomAbs_CN);
          }
          else
          {
            // See YFR : MaJ of myInitOffsetFace
          }
        }
        else if (myInitOffsetEdge.IsImage(ROE))
        {
          if (Type1 == TopAbs_FACE && Type2 == TopAbs_FACE)
          {
            const TopoDS_Face& FRoot = TopoDS::Face(Root1);
            const TopoDS_Edge& EI    = TopoDS::Edge(myInitOffsetEdge.ImageFrom(ROE));
            GeomAbs_Shape      Conti = BRep_Tool::Continuity(EI, FRoot, FRoot);
            if (Conti == GeomAbs_CN)
            {
              B.Continuity(OE, F1, F1, GeomAbs_CN);
            }
            else if (Conti > GeomAbs_C0)
            {
              B.Continuity(OE, F1, F1, GeomAbs_G1);
            }
          }
        }
      }
      continue;
    }

    //  code regularities G1 between :
    //    - sphere and tube : one root is a vertex, the other is an edge
    //                        and the vertex is included in the edge
    //    - face and tube   : one root is a face, the other an edge
    //                        and the edge is included in the face
    //    - face and face    : if two root faces are tangent in
    //                        the initial shape, they will be tangent in the offset shape
    //    - tube and tube  : if 2 edges generating tubes are
    //                        tangents, the 2 will be tangent either.
    if (Type1 == TopAbs_EDGE && Type2 == TopAbs_VERTEX)
    {
      TopoDS_Vertex V1, V2;
      TopExp::Vertices(TopoDS::Edge(Root1), V1, V2);
      if (V1.IsSame(Root2) || V2.IsSame(Root2))
      {
        B.Continuity(OE, F1, F2, GeomAbs_G1);
      }
    }
    else if (Type1 == TopAbs_VERTEX && Type2 == TopAbs_EDGE)
    {
      TopoDS_Vertex V1, V2;
      TopExp::Vertices(TopoDS::Edge(Root2), V1, V2);
      if (V1.IsSame(Root1) || V2.IsSame(Root1))
      {
        B.Continuity(OE, F1, F2, GeomAbs_G1);
      }
    }
    else if (Type1 == TopAbs_FACE && Type2 == TopAbs_EDGE)
    {
      TopExp_Explorer exp2(Root1, TopAbs_EDGE);
      for (; exp2.More(); exp2.Next())
      {
        if (exp2.Current().IsSame(Root2))
        {
          B.Continuity(OE, F1, F2, GeomAbs_G1);
          break;
        }
      }
    }
    else if (Type1 == TopAbs_EDGE && Type2 == TopAbs_FACE)
    {
      TopExp_Explorer exp2(Root2, TopAbs_EDGE);
      for (; exp2.More(); exp2.Next())
      {
        if (exp2.Current().IsSame(Root1))
        {
          B.Continuity(OE, F1, F2, GeomAbs_G1);
          break;
        }
      }
    }
    else if (Type1 == TopAbs_FACE && Type2 == TopAbs_FACE)
    {
      //  if two root faces are tangent in
      //  the initial shape, they will be tangent in the offset shape
      NCollection_List<TopoDS_Shape> LE;
      BRepOffset_Tool::FindCommonShapes(Root1, Root2, TopAbs_EDGE, LE);
      if (LE.Extent() == 1)
      {
        const TopoDS_Edge& Ed = TopoDS::Edge(LE.First());
        if (myAnalyse.HasAncestor(Ed))
        {
          const NCollection_List<BRepOffset_Interval>& LI = myAnalyse.Type(Ed);
          if (LI.Extent() == 1 && LI.First().Type() == ChFiDS_Tangential)
          {
            B.Continuity(OE, F1, F2, GeomAbs_G1);
          }
        }
      }
    }
    else if (Type1 == TopAbs_EDGE && Type2 == TopAbs_EDGE)
    {
      NCollection_List<TopoDS_Shape> LV;
      BRepOffset_Tool::FindCommonShapes(Root1, Root2, TopAbs_VERTEX, LV);
      if (LV.Extent() == 1)
      {
        NCollection_List<TopoDS_Shape> LEdTg;
        myAnalyse.TangentEdges(TopoDS::Edge(Root1), TopoDS::Vertex(LV.First()), LEdTg);
        NCollection_List<TopoDS_Shape>::Iterator it(LEdTg);
        for (; it.More(); it.Next())
        {
          if (it.Value().IsSame(Root2))
          {
            B.Continuity(OE, F1, F2, GeomAbs_G1);
            break;
          }
        }
      }
    }
  }

#ifdef OCCT_DEBUG
  if (ChronBuild)
    Clock.Show();
#endif
}

//=================================================================================================

double ComputeMaxDist(const gp_Pln&                  thePlane,
                      const occ::handle<Geom_Curve>& theCrv,
                      const double                   theFirst,
                      const double                   theLast)
{
  double aMaxDist = 0.;
  int    i, NCONTROL = 23;
  double aPrm, aDist2;
  gp_Pnt aP;
  for (i = 0; i < NCONTROL; i++)
  {
    aPrm = ((NCONTROL - 1 - i) * theFirst + i * theLast) / (NCONTROL - 1);
    aP   = theCrv->Value(aPrm);
    if (Precision::IsInfinite(aP.X()) || Precision::IsInfinite(aP.Y())
        || Precision::IsInfinite(aP.Z()))
    {
      return Precision::Infinite();
    }
    aDist2 = thePlane.SquareDistance(aP);
    if (aDist2 > aMaxDist)
      aMaxDist = aDist2;
  }
  return sqrt(aMaxDist) * 1.05;
}

//=================================================================================================

void UpdateTolerance(TopoDS_Shape&                                                        S,
                     const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& Faces,
                     const TopoDS_Shape& theInitShape)
{
  BRep_Builder                                           B;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> View;
  TopoDS_Vertex                                          V[2];

  // The edges of caps are not modified.
  int j;
  for (j = 1; j <= Faces.Extent(); j++)
  {
    const TopoDS_Shape& F = Faces(j);
    TopExp_Explorer     Exp;
    for (Exp.Init(F, TopAbs_EDGE); Exp.More(); Exp.Next())
    {
      View.Add(Exp.Current());
    }
  }

  // The edges of initial shape are  not modified
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMapInitF;
  if (!theInitShape.IsNull())
  {
    TopExp_Explorer anExpF(theInitShape, TopAbs_FACE);
    for (; anExpF.More(); anExpF.Next())
    {
      aMapInitF.Add(anExpF.Current());
      TopExp_Explorer anExpE;
      for (anExpE.Init(anExpF.Current(), TopAbs_EDGE); anExpE.More(); anExpE.Next())
      {
        View.Add(anExpE.Current());
        TopoDS_Iterator anItV(anExpE.Current());
        for (; anItV.More(); anItV.Next())
        {
          View.Add(anItV.Value());
        }
      }
    }
  }

  double          Tol;
  TopExp_Explorer anExpF(S, TopAbs_FACE);
  for (; anExpF.More(); anExpF.Next())
  {
    const TopoDS_Shape& F = anExpF.Current();
    if (Faces.Contains(F) || aMapInitF.Contains(F))
    {
      continue;
    }
    BRepAdaptor_Surface aBAS(TopoDS::Face(F), false);
    TopExp_Explorer     Exp;
    for (Exp.Init(F, TopAbs_EDGE); Exp.More(); Exp.Next())
    {
      TopoDS_Edge E         = TopoDS::Edge(Exp.Current());
      bool        isUpdated = false;
      double      aCurrTol  = BRep_Tool::Tolerance(E);
      if (aBAS.GetType() == GeomAbs_Plane)
      {
        // Edge does not seem to have pcurve on plane,
        // so EdgeCorrector does not include it in tolerance calculation
        double                  aFirst, aLast;
        occ::handle<Geom_Curve> aCrv     = BRep_Tool::Curve(E, aFirst, aLast);
        double                  aMaxDist = ComputeMaxDist(aBAS.Plane(), aCrv, aFirst, aLast);
        if (aMaxDist > aCurrTol)
        {
          B.UpdateEdge(E, aMaxDist);
          isUpdated = true;
        }
      }
      if (View.Add(E))
      {
        E.Locked(false);
        BRepCheck_Edge EdgeCorrector(E);
        Tol = EdgeCorrector.Tolerance();
        if (Tol > aCurrTol)
        {
          B.UpdateEdge(E, Tol);
          isUpdated = true;
        }
      }
      if (isUpdated)
      {
        Tol = BRep_Tool::Tolerance(E);
        // Update the vertices.
        TopExp::Vertices(E, V[0], V[1]);

        for (int i = 0; i <= 1; i++)
        {
          V[i].Locked(false);
          if (View.Add(V[i]))
          {
            occ::handle<BRep_TVertex> TV = occ::down_cast<BRep_TVertex>(V[i].TShape());
            TV->Tolerance(0.);
            BRepCheck_Vertex VertexCorrector(V[i]);
            B.UpdateVertex(V[i], VertexCorrector.Tolerance());
            // use the occasion to clean the vertices.
            (TV->ChangePoints()).Clear();
          }
          B.UpdateVertex(V[i], Tol);
        }
      }
    }
  }
}

//=================================================================================================

void CorrectSolid(TopoDS_Solid& theSol, NCollection_List<TopoDS_Shape>& theSolList)
{
  BRep_Builder             aBB;
  TopoDS_Shape             anOuterShell;
  NCollection_List<double> aVols;
  double                   aVolMax = 0., anOuterVol = 0.;

  TopoDS_Iterator anIt(theSol);
  for (; anIt.More(); anIt.Next())
  {
    const TopoDS_Shape& aSh = anIt.Value();
    GProp_GProps        aVProps;
    BRepGProp::VolumeProperties(aSh, aVProps, true);
    if (std::abs(aVProps.Mass()) > aVolMax)
    {
      anOuterVol   = aVProps.Mass();
      aVolMax      = std::abs(anOuterVol);
      anOuterShell = aSh;
    }
    aVols.Append(aVProps.Mass());
  }
  //
  if (std::abs(anOuterVol) < Precision::Confusion())
  {
    return;
  }
  if (anOuterVol < 0.)
  {
    anOuterShell.Reverse();
  }
  TopoDS_Solid aNewSol;
  aBB.MakeSolid(aNewSol);
  aNewSol.Closed(true);
  aBB.Add(aNewSol, anOuterShell);
  BRepClass3d_SolidClassifier aSolClass(aNewSol);
  //
  anIt.Initialize(theSol);
  NCollection_List<double>::Iterator aVIt(aVols);
  for (; anIt.More(); anIt.Next(), aVIt.Next())
  {
    TopoDS_Shell aSh = TopoDS::Shell(anIt.Value());
    if (aSh.IsSame(anOuterShell))
    {
      continue;
    }
    else
    {
      TopExp_Explorer      aVExp(aSh, TopAbs_VERTEX);
      const TopoDS_Vertex& aV = TopoDS::Vertex(aVExp.Current());
      gp_Pnt               aP = BRep_Tool::Pnt(aV);
      aSolClass.Perform(aP, BRep_Tool::Tolerance(aV));
      if (aSolClass.State() == TopAbs_IN)
      {
        if (aVIt.Value() > 0.)
        {
          aSh.Reverse();
        }
        aBB.Add(aNewSol, aSh);
      }
      else
      {
        if (aVIt.Value() < 0.)
        {
          aSh.Reverse();
        }
        TopoDS_Solid aSol;
        aBB.MakeSolid(aSol);
        aSol.Closed(true);
        aBB.Add(aSol, aSh);
        theSolList.Append(aSol);
      }
    }
  }
  theSol = aNewSol;
}

//=======================================================================
// function : CheckInputData
// purpose  : Check input data for possibility of offset perform.
//=======================================================================
bool BRepOffset_MakeOffset::CheckInputData(const Message_ProgressRange& theRange)
{
  // Set initial error state.
  myError = BRepOffset_NoError;
  TopoDS_Shape aTmpShape;
  myBadShape = aTmpShape;
  Message_ProgressScope aPS(theRange, nullptr, 1);
  // Non-null offset.
  if (std::abs(myOffset) <= myTol)
  {
    bool                                                                         isFound = false;
    NCollection_DataMap<TopoDS_Shape, double, TopTools_ShapeMapHasher>::Iterator anIter(
      myFaceOffset);
    for (; anIter.More(); anIter.Next())
    {
      if (std::abs(anIter.Value()) > myTol)
      {
        isFound = true;
        break;
      }
    }

    if (!isFound)
    {
      // No face with non-null offset found.
      myError = BRepOffset_NullOffset;
      return false;
    }
  }

  // Connectivity of input shape.
  if (!IsConnectedShell(myFaceComp))
  {
    myError = BRepOffset_NotConnectedShell;
    return false;
  }

  // Normals check and continuity check.
  const int                                   aPntPerDim = 20; // 21 points on each dimension.
  double                                      aUmin, aUmax, aVmin, aVmax;
  TopExp_Explorer                             anExpSF(myFaceComp, TopAbs_FACE);
  NCollection_Map<occ::handle<TopoDS_TShape>> aPresenceMap;
  TopLoc_Location                             L;
  gp_Pnt2d                                    aPnt2d;
  for (; anExpSF.More(); anExpSF.Next())
  {
    if (!aPS.More())
    {
      myError = BRepOffset_UserBreak;
      return false;
    }
    const TopoDS_Face& aF = TopoDS::Face(anExpSF.Current());

    if (aPresenceMap.Contains(aF.TShape()))
    {
      // Not perform computations with partner shapes,
      // since they are contain same geometry.
      continue;
    }
    aPresenceMap.Add(aF.TShape());

    const occ::handle<Geom_Surface>& aSurf = BRep_Tool::Surface(aF, L);
    BRepTools::UVBounds(aF, aUmin, aUmax, aVmin, aVmax);

    // Continuity check.
    if (aSurf->Continuity() == GeomAbs_C0)
    {
      myError = BRepOffset_C0Geometry;
      return false;
    }

    // Get degenerated points, to avoid check them.
    NCollection_Vector<gp_Pnt> aBad3dPnts;
    TopExp_Explorer            anExpFE(aF, TopAbs_EDGE);
    for (; anExpFE.More(); anExpFE.Next())
    {
      const TopoDS_Edge& aE = TopoDS::Edge(anExpFE.Current());
      if (BRep_Tool::Degenerated(aE))
      {
        aBad3dPnts.Append(BRep_Tool::Pnt((TopExp::FirstVertex(aE))));
      }
    }

    // Geometry grid check.
    for (int i = 0; i <= aPntPerDim; i++)
    {
      double aUParam = aUmin + (aUmax - aUmin) * i / aPntPerDim;
      for (int j = 0; j <= aPntPerDim; j++)
      {
        double aVParam = aVmin + (aVmax - aVmin) * j / aPntPerDim;

        myError = checkSinglePoint(aUParam, aVParam, aSurf, aBad3dPnts);
        if (myError != BRepOffset_NoError)
          return false;
      }
    }

    // Vertex list check.
    TopExp_Explorer anExpFV(aF, TopAbs_VERTEX);
    for (; anExpFV.More(); anExpFV.Next())
    {
      const TopoDS_Vertex& aV = TopoDS::Vertex(anExpFV.Current());
      aPnt2d                  = BRep_Tool::Parameters(aV, aF);

      myError = checkSinglePoint(aPnt2d.X(), aPnt2d.Y(), aSurf, aBad3dPnts);
      if (myError != BRepOffset_NoError)
        return false;
    }
  }

  return true;
}

//=======================================================================
// function : GetBadShape
// purpose  : Get shape where problems detected.
//=======================================================================
const TopoDS_Shape& BRepOffset_MakeOffset::GetBadShape() const
{
  return myBadShape;
}

//=================================================================================================

void BRepOffset_MakeOffset::RemoveInternalEdges()
{
  bool            bRemoveWire, bRemoveEdge;
  TopExp_Explorer aExpF, aExpW, aExpE;
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    aDMELF;
  //
  TopExp::MapShapesAndAncestors(myOffsetShape, TopAbs_EDGE, TopAbs_FACE, aDMELF);
  //
  aExpF.Init(myOffsetShape, TopAbs_FACE);
  for (; aExpF.More(); aExpF.Next())
  {
    TopoDS_Face& aF = *(TopoDS_Face*)&aExpF.Current();
    //
    NCollection_List<TopoDS_Shape> aLIW;
    //
    aExpW.Init(aF, TopAbs_WIRE);
    for (; aExpW.More(); aExpW.Next())
    {
      TopoDS_Wire& aW = *(TopoDS_Wire*)&aExpW.Current();
      //
      bRemoveWire = true;
      NCollection_List<TopoDS_Shape> aLIE;
      //
      aExpE.Init(aW, TopAbs_EDGE);
      for (; aExpE.More(); aExpE.Next())
      {
        const TopoDS_Edge& aE = *(TopoDS_Edge*)&aExpE.Current();
        if (aE.Orientation() != TopAbs_INTERNAL)
        {
          bRemoveWire = false;
          continue;
        }
        //
        const NCollection_List<TopoDS_Shape>& aLF = aDMELF.FindFromKey(aE);
        bRemoveEdge                               = (aLF.Extent() == 1);
        if (bRemoveEdge)
        {
          aLIE.Append(aE);
        }
        else
        {
          bRemoveWire = false;
        }
      }
      //
      if (bRemoveWire)
      {
        aLIW.Append(aW);
      }
      else if (aLIE.Extent())
      {
        RemoveShapes(aW, aLIE);
      }
    }
    //
    if (aLIW.Extent())
    {
      RemoveShapes(aF, aLIW);
    }
  }
}

//=======================================================================
// static methods implementation
//=======================================================================

//=======================================================================
// function : checkSinglePoint
// purpose  : Check single point on surface for bad normals
//=======================================================================
BRepOffset_Error checkSinglePoint(const double                      theUParam,
                                  const double                      theVParam,
                                  const occ::handle<Geom_Surface>&  theSurf,
                                  const NCollection_Vector<gp_Pnt>& theBadPoints)
{
  gp_Pnt aPnt;
  gp_Vec aD1U, aD1V;
  theSurf->D1(theUParam, theVParam, aPnt, aD1U, aD1V);

  if (aD1U.SquareMagnitude() < Precision::SquareConfusion()
      || aD1V.SquareMagnitude() < Precision::SquareConfusion())
  {
    bool isKnownBadPnt = false;
    for (int anIdx = theBadPoints.Lower(); anIdx <= theBadPoints.Upper(); ++anIdx)
    {
      if (aPnt.SquareDistance(theBadPoints(anIdx)) < Precision::SquareConfusion())
      {
        isKnownBadPnt = true;
        break;
      }
    } // for(int anIdx  = theBadPoints.Lower();

    if (!isKnownBadPnt)
    {
      return BRepOffset_BadNormalsOnGeometry;
    }
    else
    {
      return BRepOffset_NoError;
    }
  } //  if (aD1U.SquareMagnitude() < Precision::SquareConfusion() ||

  if (aD1U.IsParallel(aD1V, Precision::Confusion()))
  {
    // Isolines are collinear.
    return BRepOffset_BadNormalsOnGeometry;
  }

  return BRepOffset_NoError;
}

//=======================================================================
// function : RemoveShapes
// purpose  : Removes the shapes <theLS> from the shape <theS>
//=======================================================================
void RemoveShapes(TopoDS_Shape& theS, const NCollection_List<TopoDS_Shape>& theLS)
{
  BRep_Builder aBB;
  //
  bool bFree = theS.Free();
  theS.Free(true);
  //
  NCollection_List<TopoDS_Shape>::Iterator aIt(theLS);
  for (; aIt.More(); aIt.Next())
  {
    const TopoDS_Shape& aSI = aIt.Value();
    aBB.Remove(theS, aSI);
  }
  //
  theS.Free(bFree);
}

//=======================================================================
// function : UpdateHistory
// purpose  : Updates the history information
//=======================================================================
void UpdateHistory(const NCollection_List<TopoDS_Shape>& theLF,
                   BOPAlgo_Builder&                      theGF,
                   BRepAlgo_Image&                       theImage)
{
  NCollection_List<TopoDS_Shape>::Iterator aIt(theLF);
  for (; aIt.More(); aIt.Next())
  {
    const TopoDS_Shape&                   aF    = aIt.Value();
    const NCollection_List<TopoDS_Shape>& aLFIm = theGF.Modified(aF);
    if (aLFIm.Extent())
    {
      if (theImage.HasImage(aF))
      {
        theImage.Add(aF, aLFIm);
      }
      else
      {
        theImage.Bind(aF, aLFIm);
      }
    }
  }
}

//=================================================================================================

void BRepOffset_MakeOffset::IntersectEdges(
  const NCollection_List<TopoDS_Shape>&                                          theFaces,
  NCollection_DataMap<TopoDS_Shape, BRepOffset_Offset, TopTools_ShapeMapHasher>& theMapSF,
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>&      theMES,
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>&      theBuild,
  occ::handle<BRepAlgo_AsDes>&                                                   theAsDes,
  occ::handle<BRepAlgo_AsDes>&                                                   theAsDes2d,
  const Message_ProgressRange&                                                   theRange)
{
  double aTolF;
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    aDMVV;
  // intersect edges created from edges
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMFV;
  Message_ProgressScope                                         aPSOuter(theRange, nullptr, 2);
  Message_ProgressScope aPS1(aPSOuter.Next(), nullptr, theFaces.Size());
  for (NCollection_List<TopoDS_Shape>::Iterator it(theFaces); it.More(); it.Next())
  {
    const TopoDS_Face& aF = TopoDS::Face(it.Value());
    aTolF                 = BRep_Tool::Tolerance(aF);
    if (!BRepOffset_Inter2d::ConnexIntByInt(aF,
                                            theMapSF(aF),
                                            theMES,
                                            theBuild,
                                            theAsDes,
                                            theAsDes2d,
                                            myOffset,
                                            aTolF,
                                            myAnalyse,
                                            aMFV,
                                            myImageVV,
                                            myEdgeIntEdges,
                                            aDMVV,
                                            aPS1.Next()))
    {
      myError = BRepOffset_CannotExtentEdge;
      return;
    }
    if (!aPS1.More())
    {
      myError = BRepOffset_UserBreak;
      return;
    }
  }
  // intersect edges created from vertices
  int                   i, aNbF = aMFV.Extent();
  Message_ProgressScope aPS2(aPSOuter.Next(), "Intersecting edges created from vertices", aNbF);
  for (i = 1; i <= aNbF; ++i)
  {
    const TopoDS_Face& aF = TopoDS::Face(aMFV(i));
    aTolF                 = BRep_Tool::Tolerance(aF);
    BRepOffset_Inter2d::ConnexIntByIntInVert(aF,
                                             theMapSF(aF),
                                             theMES,
                                             theBuild,
                                             theAsDes,
                                             theAsDes2d,
                                             aTolF,
                                             myAnalyse,
                                             aDMVV,
                                             aPS2.Next());
    if (!aPS2.More())
    {
      myError = BRepOffset_UserBreak;
      return;
    }
  }
  //
  // fuse vertices on edges
  if (!BRepOffset_Inter2d::FuseVertices(aDMVV, theAsDes2d, myImageVV))
  {
    myError = BRepOffset_CannotFuseVertices;
    return;
  }
}

//=================================================================================================

bool TrimEdges(
  const TopoDS_Shape&                                                            theShape,
  const double                                                                   theOffset,
  const BRepOffset_Analyse&                                                      Analyse,
  NCollection_DataMap<TopoDS_Shape, BRepOffset_Offset, TopTools_ShapeMapHasher>& theMapSF,
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>&      theMES,
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>&      theBuild,
  occ::handle<BRepAlgo_AsDes>&                                                   theAsDes,
  occ::handle<BRepAlgo_AsDes>&                                                   theAsDes2d,
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>&                 theNewEdges,
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>&      theETrimEInf,
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    theEdgesOrigins)
{
  TopExp_Explorer Exp, Exp2, ExpC;
  TopoDS_Shape    NE;
  TopoDS_Edge     TNE;
  TopoDS_Face     NF;

  NCollection_List<TopoDS_Shape> aLFaces;
  for (Exp.Init(theShape, TopAbs_FACE); Exp.More(); Exp.Next())
    aLFaces.Append(Exp.Current());

  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMFGenerated;
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    aDMEF;
  for (NCollection_List<TopoDS_Shape>::Iterator it(Analyse.NewFaces()); it.More(); it.Next())
  {
    const TopoDS_Shape& aFG = it.Value();
    aLFaces.Append(aFG);
    aMFGenerated.Add(aFG);
    TopExp::MapShapesAndUniqueAncestors(aFG, TopAbs_EDGE, TopAbs_FACE, aDMEF);
  }

  for (NCollection_List<TopoDS_Shape>::Iterator it(aLFaces); it.More(); it.Next())
  {
    const TopoDS_Face& FI = TopoDS::Face(it.Value());
    NF                    = theMapSF(FI).Face();
    if (theMES.IsBound(NF))
    {
      NF = TopoDS::Face(theMES(NF));
    }
    //
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>        View;
    NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> VEmap;
    int                                                           i, aNb;
    //
    TopExp::MapShapes(FI.Oriented(TopAbs_FORWARD), TopAbs_EDGE, VEmap);
    TopExp::MapShapes(FI.Oriented(TopAbs_FORWARD), TopAbs_VERTEX, VEmap);
    //
    aNb = VEmap.Extent();
    for (i = 1; i <= aNb; ++i)
    {
      const TopoDS_Shape& aS = VEmap(i);
      if (!View.Add(aS))
      {
        continue;
      }
      //
      if (theBuild.IsBound(aS))
      {
        NE = theBuild(aS);
        // keep connection to original edges
        ExpC.Init(NE, TopAbs_EDGE);
        for (; ExpC.More(); ExpC.Next())
        {
          const TopoDS_Edge&              NEC   = TopoDS::Edge(ExpC.Current());
          NCollection_List<TopoDS_Shape>* pLEOr = theEdgesOrigins.ChangeSeek(NEC);
          if (!pLEOr)
          {
            pLEOr = theEdgesOrigins.Bound(NEC, NCollection_List<TopoDS_Shape>());
          }
          AppendToList(*pLEOr, aS);
        }
        // trim edges
        if (NE.ShapeType() == TopAbs_EDGE)
        {
          if (theNewEdges.Add(NE))
          {
            if (!TrimEdge(TopoDS::Edge(NE), theAsDes2d, theAsDes, theETrimEInf))
              return false;
          }
        }
        else
        {
          //------------------------------------------------------------
          // The Intersections are on several edges.
          // The pieces without intersections with neighbors
          // are removed from AsDes.
          //------------------------------------------------------------
          for (ExpC.Init(NE, TopAbs_EDGE); ExpC.More(); ExpC.Next())
          {
            TopoDS_Edge NEC = TopoDS::Edge(ExpC.Current());
            if (theNewEdges.Add(NEC))
            {
              if (!theAsDes2d->Descendant(NEC).IsEmpty())
              {
                if (!TrimEdge(NEC, theAsDes2d, theAsDes, theETrimEInf))
                  return false;
              }
              else
              {
                if (theAsDes->HasAscendant(NEC))
                {
                  theAsDes->Remove(NEC);
                }
              }
            }
          }
        }
      }
      else
      {
        if (aS.ShapeType() != TopAbs_EDGE)
        {
          continue;
        }
        if (aMFGenerated.Contains(FI) && aDMEF.FindFromKey(aS).Extent() == 1)
          continue;

        NE = theMapSF(FI).Generated(aS);
        //// modified by jgv, 19.12.03 for OCC4455 ////
        NE.Orientation(aS.Orientation());
        //
        NCollection_List<TopoDS_Shape>* pLEOr = theEdgesOrigins.ChangeSeek(NE);
        if (!pLEOr)
        {
          pLEOr = theEdgesOrigins.Bound(NE, NCollection_List<TopoDS_Shape>());
        }
        AppendToList(*pLEOr, aS);
        //
        if (theMES.IsBound(NE))
        {
          NE = theMES(NE);
          NE.Orientation(aS.Orientation());
          if (theNewEdges.Add(NE))
          {
            if (!TrimEdge(TopoDS::Edge(NE), theAsDes2d, theAsDes, theETrimEInf))
              return false;
          }
        }
        else
        {
          TopoDS_Edge&      anEdge = TopoDS::Edge(NE);
          BRepAdaptor_Curve aBAC(anEdge);
          if (aBAC.GetType() == GeomAbs_Line)
          {
            TopoDS_Edge aNewEdge;
            BRepOffset_Inter2d::ExtentEdge(anEdge, aNewEdge, theOffset);
            theETrimEInf.Bind(anEdge, aNewEdge);
          }
        }
        theAsDes->Add(NF, NE);
      }
    }
  }
  return true;
}

//=======================================================================
// function : TrimEdge
// purpose  : Trim the edge of the largest of descendants in AsDes2d.
//           Order in AsDes two vertices that have trimmed the edge.
//=======================================================================
bool TrimEdge(
  TopoDS_Edge&                                                              NE,
  const occ::handle<BRepAlgo_AsDes>&                                        AsDes2d,
  occ::handle<BRepAlgo_AsDes>&                                              AsDes,
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& theETrimEInf)
{
  TopoDS_Edge   aSourceEdge;
  TopoDS_Vertex V1, V2;
  double        aT1, aT2;
  //
  TopExp::Vertices(NE, V1, V2);
  BRep_Tool::Range(NE, aT1, aT2);
  //
  BOPTools_AlgoTools::MakeSplitEdge(NE, V1, aT1, V2, aT2, aSourceEdge);
  //
  //
  constexpr double aSameParTol = Precision::Confusion();

  double U    = 0.;
  double UMin = Precision::Infinite();
  double UMax = -UMin;

  const NCollection_List<TopoDS_Shape>& LE = AsDes2d->Descendant(NE);
  //
  bool bTrim = false;
  //
  if (LE.Extent() > 1)
  {
    NCollection_List<TopoDS_Shape>::Iterator it(LE);
    for (; it.More(); it.Next())
    {
      TopoDS_Vertex V = TopoDS::Vertex(it.Value());
      if (NE.Orientation() == TopAbs_REVERSED)
        V.Reverse();
      // V.Orientation(TopAbs_INTERNAL);
      if (!FindParameter(V, NE, U))
      {
        double                      f, l;
        occ::handle<Geom_Curve>     theCurve = BRep_Tool::Curve(NE, f, l);
        gp_Pnt                      thePoint = BRep_Tool::Pnt(V);
        GeomAPI_ProjectPointOnCurve Projector(thePoint, theCurve);
        if (Projector.NbPoints() == 0)
        {
          return false;
        }
        U = Projector.LowerDistanceParameter();
      }
      if (U < UMin)
      {
        UMin = U;
        V1   = V;
      }
      if (U > UMax)
      {
        UMax = U;
        V2   = V;
      }
    }
    //
    if (V1.IsNull() || V2.IsNull())
    {
      return false;
    }
    if (!V1.IsSame(V2))
    {
      NE.Free(true);
      BRep_Builder       B;
      TopAbs_Orientation Or = NE.Orientation();
      NE.Orientation(TopAbs_FORWARD);
      TopoDS_Vertex VF, VL;
      TopExp::Vertices(NE, VF, VL);
      B.Remove(NE, VF);
      B.Remove(NE, VL);
      B.Add(NE, V1.Oriented(TopAbs_FORWARD));
      B.Add(NE, V2.Oriented(TopAbs_REVERSED));
      B.Range(NE, UMin, UMax);
      NE.Orientation(Or);
      AsDes->Add(NE, V1.Oriented(TopAbs_FORWARD));
      AsDes->Add(NE, V2.Oriented(TopAbs_REVERSED));
      BRepLib::SameParameter(NE, aSameParTol, true);
      //
      bTrim = true;
    }
  }
  //
  if (!bTrim)
  {
    BRepAdaptor_Curve aBAC(NE);
    if (aBAC.GetType() == GeomAbs_Line)
    {
      if (AsDes->HasAscendant(NE))
      {
        AsDes->Remove(NE);
      }
    }
  }
  else
  {
    if (!theETrimEInf.IsBound(NE))
    {
      theETrimEInf.Bind(NE, aSourceEdge);
    }
  }
  return true;
}

//=================================================================================================

void GetEnlargedFaces(
  const NCollection_List<TopoDS_Shape>&                                                theFaces,
  const NCollection_DataMap<TopoDS_Shape, BRepOffset_Offset, TopTools_ShapeMapHasher>& theMapSF,
  const NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>&      theMES,
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& theFacesOrigins,
  BRepAlgo_Image&                                                           theImage,
  NCollection_List<TopoDS_Shape>&                                           theLSF)
{
  for (NCollection_List<TopoDS_Shape>::Iterator it(theFaces); it.More(); it.Next())
  {
    const TopoDS_Shape& FI  = it.Value();
    const TopoDS_Shape& OFI = theMapSF(FI).Face();
    if (theMES.IsBound(OFI))
    {
      const TopoDS_Face& aLocalFace = TopoDS::Face(theMES(OFI));
      theLSF.Append(aLocalFace);
      theImage.SetRoot(aLocalFace);
      theFacesOrigins.Bind(aLocalFace, FI);
    }
  }
}

//=======================================================================
// function : BuildShellsCompleteInter
// purpose  : Make the shells from list of faces using MakerVolume algorithm.
//           In case there will be more than just one solid, it will be
//           rebuilt using only outer faces.
//=======================================================================
bool BuildShellsCompleteInter(const NCollection_List<TopoDS_Shape>& theLF,
                              BRepAlgo_Image&                       theImage,
                              TopoDS_Shape&                         theShells,
                              const Message_ProgressRange&          theRange)
{
  Message_ProgressScope aPS(theRange, nullptr, 5);
  // make solids
  BOPAlgo_MakerVolume aMV1;
  aMV1.SetArguments(theLF);
  // we need to intersect the faces to process the tangential faces
  aMV1.SetIntersect(true);
  aMV1.SetAvoidInternalShapes(true);
  aMV1.Perform(aPS.Next(3));
  //
  bool bDone = !aMV1.HasErrors();
  if (!bDone)
  {
    return bDone;
  }
  //
  UpdateHistory(theLF, aMV1, theImage);
  //
  const TopoDS_Shape& aResult1 = aMV1.Shape();
  if (aResult1.ShapeType() == TopAbs_SOLID)
  {
    // result is the alone solid, nothing to do
    return GetSubShapes(aResult1, TopAbs_SHELL, theShells);
  }

  // Allocators for effective memory allocations
  // Global allocator for the long-living containers
  occ::handle<NCollection_IncAllocator> anAllocGlob = new NCollection_IncAllocator;
  // Local allocator for the local containers
  occ::handle<NCollection_IncAllocator> anAllocLoc = new NCollection_IncAllocator;

  // Since the <theImage> object does not support multiple ancestors,
  // prepare local copy of the origins, which will be used to resolve
  // non-manifold solids produced by Maker Volume algorithm by comparison
  // of the normal directions of the split faces with their origins.
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
                                           anOrigins(1, anAllocGlob);
  NCollection_List<TopoDS_Shape>::Iterator aItLR(theImage.Roots());
  for (; aItLR.More(); aItLR.Next())
  {
    const TopoDS_Shape& aFR = aItLR.Value();

    // Reset the local allocator
    anAllocLoc->Reset(false);
    // Find the last splits of the root face, including the ones
    // created during MakeVolume operation
    NCollection_List<TopoDS_Shape> aLFIm(anAllocLoc);
    theImage.LastImage(aFR, aLFIm);

    NCollection_List<TopoDS_Shape>::Iterator aItLFIm(aLFIm);
    for (; aItLFIm.More(); aItLFIm.Next())
    {
      const TopoDS_Shape&             aFIm  = aItLFIm.Value();
      NCollection_List<TopoDS_Shape>* pLFOr = anOrigins.ChangeSeek(aFIm);
      if (!pLFOr)
      {
        pLFOr = anOrigins.Bound(aFIm, NCollection_List<TopoDS_Shape>(anAllocGlob));
      }
      pLFOr->Append(aFR);
    }
  }

  // Reset the local allocator
  anAllocLoc->Reset(false);
  // It is necessary to rebuild the solids, avoiding internal faces
  // Map faces to solids
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    aDMFS(1, anAllocLoc);
  TopExp::MapShapesAndAncestors(aResult1, TopAbs_FACE, TopAbs_SOLID, aDMFS);
  //
  int i, aNb = aDMFS.Extent();
  bDone = (aNb > 0);
  if (!bDone)
  {
    // unable to build any solid
    return bDone;
  }
  //
  // get faces attached to only one solid
  NCollection_List<TopoDS_Shape> aLF(anAllocLoc);
  for (i = 1; i <= aNb; ++i)
  {
    const NCollection_List<TopoDS_Shape>& aLS = aDMFS(i);
    if (aLS.Extent() == 1)
    {
      const TopoDS_Shape& aF = aDMFS.FindKey(i);
      aLF.Append(aF);
    }
  }
  //
  // make solids from the new list
  BOPAlgo_MakerVolume aMV2;
  aMV2.SetArguments(aLF);
  // no need to intersect this time
  aMV2.SetIntersect(false);
  aMV2.SetAvoidInternalShapes(true);
  aMV2.Perform(aPS.Next());
  bDone = !aMV2.HasErrors();
  if (!bDone)
  {
    return bDone;
  }
  //
  const TopoDS_Shape& aResult2 = aMV2.Shape();
  if (aResult2.ShapeType() == TopAbs_SOLID)
  {
    return GetSubShapes(aResult2, TopAbs_SHELL, theShells);
  }
  //
  TopExp_Explorer aExp(aResult2, TopAbs_FACE);
  bDone = aExp.More();
  if (!bDone)
  {
    return bDone;
  }
  //
  aLF.Clear();
  aDMFS.Clear();
  anAllocLoc->Reset(true);

  // the result is non-manifold - resolve it comparing normal
  // directions of the offset faces and original faces
  for (; aExp.More(); aExp.Next())
  {
    const TopoDS_Face&                    aF    = TopoDS::Face(aExp.Current());
    const NCollection_List<TopoDS_Shape>* pLFOr = anOrigins.Seek(aF);
    if (!pLFOr)
    {
      Standard_ASSERT_INVOKE("BRepOffset_MakeOffset::BuildShellsCompleteInterSplit(): "
                             "Origins map does not contain the split face");
      continue;
    }
    // Check orientation
    NCollection_List<TopoDS_Shape>::Iterator aItLOr(*pLFOr);
    for (; aItLOr.More(); aItLOr.Next())
    {
      const TopoDS_Face& aFOr = TopoDS::Face(aItLOr.Value());
      if (BRepOffset_Tool::CheckPlanesNormals(aF, aFOr))
      {
        aLF.Append(aF);
        break;
      }
    }
  }
  //
  // make solid from most outer faces with correct normal direction
  BOPAlgo_MakerVolume aMV3;
  aMV3.SetArguments(aLF);
  aMV3.SetIntersect(false);
  aMV3.SetAvoidInternalShapes(true);
  aMV3.Perform(aPS.Next());
  bDone = !aMV3.HasErrors();
  if (!bDone)
  {
    return bDone;
  }
  //
  const TopoDS_Shape& aResult3 = aMV3.Shape();
  return GetSubShapes(aResult3, TopAbs_SHELL, theShells);
}

//=================================================================================================

const NCollection_List<TopoDS_Shape>& BRepOffset_MakeOffset::Generated(const TopoDS_Shape& theS)
{
  myGenerated.Clear();
  const TopAbs_ShapeEnum aType = theS.ShapeType();
  switch (aType)
  {
    case TopAbs_VERTEX: {
      if (myAnalyse.HasAncestor(theS))
      {
        NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMFence;
        const NCollection_List<TopoDS_Shape>&                  aLA = myAnalyse.Ancestors(theS);
        NCollection_List<TopoDS_Shape>::Iterator               itLA(aLA);
        for (; myGenerated.IsEmpty() && itLA.More(); itLA.Next())
        {
          const TopoDS_Shape& aE = itLA.Value();
          if (!myInitOffsetEdge.HasImage(aE))
            continue;
          NCollection_List<TopoDS_Shape> aLEIm;
          myInitOffsetEdge.LastImage(aE, aLEIm);
          NCollection_List<TopoDS_Shape>::Iterator itLEIm(aLEIm);
          for (; myGenerated.IsEmpty() && itLEIm.More(); itLEIm.Next())
          {
            TopoDS_Iterator itV(itLEIm.Value());
            for (; itV.More(); itV.Next())
            {
              if (!aMFence.Add(itV.Value()))
              {
                myGenerated.Append(itV.Value());
                break;
              }
            }
          }
        }
      }
    }
      [[fallthrough]];
    case TopAbs_EDGE: {
      if (myInitOffsetEdge.HasImage(theS))
      {
        myInitOffsetEdge.LastImage(theS, myGenerated);
      }
    }
      [[fallthrough]];
    case TopAbs_FACE: {
      TopoDS_Shape        aS        = theS;
      const TopoDS_Shape* aPlanface = myFacePlanfaceMap.Seek(aS);
      if (aPlanface)
        aS = TopoDS::Face(*aPlanface);

      if (!myFaces.Contains(aS) && myInitOffsetFace.HasImage(aS))
      {
        myInitOffsetFace.LastImage(aS, myGenerated);

        if (!myFaces.IsEmpty())
        {
          // Reverse generated shapes in case of small solids.
          // Useful only for faces without influence on others.
          NCollection_List<TopoDS_Shape>::Iterator it(myGenerated);
          for (; it.More(); it.Next())
            it.ChangeValue().Reverse();
        }
      }
      break;
    }
    case TopAbs_SOLID: {
      if (theS.IsSame(myShape))
        myGenerated.Append(myOffsetShape);
      break;
    }
    default:
      break;
  }

  if (myResMap.IsEmpty())
    TopExp::MapShapes(myOffsetShape, myResMap);

  for (NCollection_List<TopoDS_Shape>::Iterator it(myGenerated); it.More();)
  {
    if (myResMap.Contains(it.Value()))
      it.Next();
    else
      myGenerated.Remove(it);
  }

  return myGenerated;
}

//=================================================================================================

const NCollection_List<TopoDS_Shape>& BRepOffset_MakeOffset::Modified(const TopoDS_Shape& theShape)
{
  myGenerated.Clear();

  if (theShape.ShapeType() == TopAbs_FACE)
  {
    TopoDS_Shape        aS        = theShape;
    const TopoDS_Shape* aPlanface = myFacePlanfaceMap.Seek(aS);
    if (aPlanface)
      aS = TopoDS::Face(*aPlanface);

    if (myFaces.Contains(aS) && myInitOffsetFace.HasImage(aS))
    {
      myInitOffsetFace.LastImage(aS, myGenerated);

      if (!myFaces.IsEmpty())
      {
        // Reverse generated shapes in case of small solids.
        // Useful only for faces without influence on others.
        NCollection_List<TopoDS_Shape>::Iterator it(myGenerated);
        for (; it.More(); it.Next())
          it.ChangeValue().Reverse();
      }
    }
  }

  return myGenerated;
}

//=================================================================================================

bool BRepOffset_MakeOffset::IsDeleted(const TopoDS_Shape& theS)
{
  if (myResMap.IsEmpty())
    TopExp::MapShapes(myOffsetShape, myResMap);

  if (myResMap.Contains(theS))
    return false;

  return Generated(theS).IsEmpty() && Modified(theS).IsEmpty();
}

//=================================================================================================

bool GetSubShapes(const TopoDS_Shape&    theShape,
                  const TopAbs_ShapeEnum theSSType,
                  TopoDS_Shape&          theResult)
{
  TopExp_Explorer aExp(theShape, theSSType);
  if (!aExp.More())
  {
    return false;
  }
  //
  TopoDS_Compound aResult;
  BRep_Builder().MakeCompound(aResult);
  //
  for (; aExp.More(); aExp.Next())
  {
    const TopoDS_Shape& aSS = aExp.Current();
    BRep_Builder().Add(aResult, aSS);
  }
  theResult = aResult;
  return true;
}

//=================================================================================================

void BRepOffset_MakeOffset::analyzeProgress(const double                theWhole,
                                            NCollection_Array1<double>& theSteps) const
{
  theSteps.Init(0.0);

  // Set, approximately, the proportions for each operation.
  // It is not a problem that the sum of the set values will not
  // be equal to 100%, as the values will be normalized.
  // The main point is to make the proportions valid relatively each other.

  // Proportions will be different for different connection types
  bool isArc = (myJoin == GeomAbs_Arc);
  bool isPlanarIntCase =
    myInter && !isArc && myIsPlanar && !myThickening && myFaces.IsEmpty() && IsSolid(myShape);

  theSteps(PIOperation_CheckInputData) = 1.;
  theSteps(PIOperation_Analyse)        = 2.;
  theSteps(PIOperation_BuildOffsetBy)  = isPlanarIntCase ? 70. : (isArc ? 20. : 50.);
  theSteps(PIOperation_Intersection)   = isPlanarIntCase ? 0. : (isArc ? 50. : 20.);
  if (myThickening)
  {
    theSteps(PIOperation_MakeMissingWalls) = 5.;
  }
  theSteps(PIOperation_MakeShells) = isPlanarIntCase ? 25. : 5.;
  theSteps(PIOperation_MakeSolid)  = 5.;
  if (myIsPerformSewing && myThickening)
  {
    theSteps(PIOperation_Sewing) = 10.;
  }

  normalizeSteps(theWhole, theSteps);
}

//=======================================================================
// function : IsPlanar
// purpose  : Checks if all the faces of the shape are planes
//=======================================================================
bool BRepOffset_MakeOffset::IsPlanar()
{
  bool         aIsNonPlanarFound = false;
  BRep_Builder aBB;

  TopExp_Explorer aExp(myShape, TopAbs_FACE);
  for (; aExp.More(); aExp.Next())
  {
    const TopoDS_Face&  aF = *(TopoDS_Face*)&aExp.Current();
    BRepAdaptor_Surface aBAS(aF, false);
    if (aBAS.GetType() == GeomAbs_Plane)
      continue;

    if (myIsLinearizationAllowed)
    {
      // define the toleance
      double aTolForFace = BRep_Tool::Tolerance(aF);

      // try to linearize
      occ::handle<Geom_Surface> aSurf = BRep_Tool::Surface(aF);
      GeomLib_IsPlanarSurface   aPlanarityChecker(aSurf, Precision::Confusion());
      if (aPlanarityChecker.IsPlanar())
      {
        gp_Pln aPln = aPlanarityChecker.Plan();
        double u1, u2, v1, v2, um, vm;
        aSurf->Bounds(u1, u2, v1, v2);
        bool isInf1 = Precision::IsInfinite(u1), isInf2 = Precision::IsInfinite(u2);
        if (!isInf1 && !isInf2)
        {
          um = (u1 + u2) / 2.;
        }
        else if (isInf1 && !isInf2)
        {
          um = u2 - 1.;
        }
        else if (!isInf1 && isInf2)
        {
          um = u1 + 1.;
        }
        else // isInf1 && isInf2
        {
          um = 0.;
        }
        isInf1 = Precision::IsInfinite(v1), isInf2 = Precision::IsInfinite(v2);
        if (!isInf1 && !isInf2)
        {
          vm = (v1 + v2) / 2.;
        }
        else if (isInf1 && !isInf2)
        {
          vm = v2 - 1.;
        }
        else if (!isInf1 && isInf2)
        {
          vm = v1 + 1.;
        }
        else // isInf1 && isInf2
        {
          vm = 0.;
        }
        gp_Pnt aP;
        gp_Vec aD1, aD2;
        aBAS.D1(um, vm, aP, aD1, aD2);
        gp_Vec aNorm    = aD1.Crossed(aD2);
        gp_Dir aPlnNorm = aPln.Position().Direction();
        if (aNorm.Dot(aPlnNorm) < 0.)
        {
          aPlnNorm.Reverse();
          gp_Ax1 anAx(aPln.Position().Location(), aPlnNorm);
          aPln.SetAxis(anAx);
        }
        occ::handle<Geom_Plane> aPlane = new Geom_Plane(aPln);
        TopoDS_Face             aPlanarFace;
        aBB.MakeFace(aPlanarFace, aPlane, aTolForFace);
        TopoDS_Face aFaceForward = aF;
        aFaceForward.Orientation(TopAbs_FORWARD);
        TopoDS_Iterator anItFace(aFaceForward);
        for (; anItFace.More(); anItFace.Next())
        {
          const TopoDS_Shape& aWire = anItFace.Value();
          aBB.Add(aPlanarFace, aWire);
        }
        RemoveSeamAndDegeneratedEdges(aPlanarFace, aFaceForward);
        myFacePlanfaceMap.Bind(aF, aPlanarFace);
        if (myFaces.Contains(aF))
        {
          myFaces.RemoveKey(aF);
          myFaces.Add(aPlanarFace);
        }
      }
      else
        aIsNonPlanarFound = true;
    }
    else
      aIsNonPlanarFound = true;
  }

  return (!aIsNonPlanarFound);
}

//=======================================================================
// function : RemoveSeamAndDegeneratedEdges
// purpose  : Removes useless seam and degenerated edges from a face that becomes planar
//=======================================================================
void RemoveSeamAndDegeneratedEdges(const TopoDS_Face& theFace, const TopoDS_Face& theOldFace)
{
  TopoDS_Face aFace = theFace;
  aFace.Orientation(TopAbs_FORWARD);

  bool                               aIsDegOrSeamFound = false;
  NCollection_Sequence<TopoDS_Shape> aEseq;
  TopExp_Explorer                    anExplo(aFace, TopAbs_EDGE);
  for (; anExplo.More(); anExplo.Next())
  {
    const TopoDS_Edge& anEdge = TopoDS::Edge(anExplo.Current());
    if (BRep_Tool::Degenerated(anEdge) || BRepTools::IsReallyClosed(anEdge, theOldFace))
      aIsDegOrSeamFound = true;
    else
      aEseq.Append(anEdge);
  }

  if (!aIsDegOrSeamFound)
    return;

  // Reconstruct wires
  BRep_Builder                   aBB;
  NCollection_List<TopoDS_Shape> aWlist;
  TopoDS_Iterator                anItFace(aFace);
  for (; anItFace.More(); anItFace.Next())
    aWlist.Append(anItFace.Value());

  aFace.Free(true);
  NCollection_List<TopoDS_Shape>::Iterator anItl(aWlist);
  for (; anItl.More(); anItl.Next())
    aBB.Remove(aFace, anItl.Value());

  while (!aEseq.IsEmpty())
  {
    TopoDS_Wire aNewWire;
    aBB.MakeWire(aNewWire);
    TopoDS_Edge aCurEdge = TopoDS::Edge(aEseq(1));
    aBB.Add(aNewWire, aCurEdge);
    aEseq.Remove(1);
    TopoDS_Vertex aFirstVertex, aCurVertex;
    TopExp::Vertices(aCurEdge, aFirstVertex, aCurVertex, true); // with orientation
    while (!aCurVertex.IsSame(aFirstVertex))
    {
      TopoDS_Vertex aV1, aV2;
      int           ind;
      for (ind = 1; ind <= aEseq.Length(); ind++)
      {
        aCurEdge = TopoDS::Edge(aEseq(ind));
        TopExp::Vertices(aCurEdge, aV1, aV2, true); // with orientation
        if (aV1.IsSame(aCurVertex))
          break;
      }
      if (ind > aEseq.Length()) // error occurred: wire is not closed
        break;

      aBB.Add(aNewWire, aCurEdge);
      aEseq.Remove(ind);
      aCurVertex = aV2;
    }

    aBB.Add(aFace, aNewWire);
  }
}

//=======================================================================
// function : IsSolid
// purpose  : Checks if the shape is solid
//=======================================================================
bool IsSolid(const TopoDS_Shape& theS)
{
  TopExp_Explorer aExp(theS, TopAbs_SOLID);
  return aExp.More();
}

//=======================================================================
// function : AppendToList
// purpose  : Add to a list only unique elements
//=======================================================================
void AppendToList(NCollection_List<TopoDS_Shape>& theList, const TopoDS_Shape& theShape)
{
  NCollection_List<TopoDS_Shape>::Iterator aIt(theList);
  for (; aIt.More(); aIt.Next())
  {
    const TopoDS_Shape& aS = aIt.Value();
    if (aS.IsSame(theShape))
    {
      return;
    }
  }
  theList.Append(theShape);
}
