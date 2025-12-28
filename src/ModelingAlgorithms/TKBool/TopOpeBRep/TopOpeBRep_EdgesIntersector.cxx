// Created on: 1994-10-07
// Created by: Jean Yves LEBEY
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

#ifdef DRAW
static void CurveToString(const GeomAbs_CurveType t, TCollection_AsciiString& N)
{
  switch (t)
  {
    case GeomAbs_Line:
      N = "LINE";
      break;
    case GeomAbs_Circle:
      N = "CIRCLE";
      break;
    case GeomAbs_Ellipse:
      N = "ELLIPSE";
      break;
    case GeomAbs_Hyperbola:
      N = "HYPERBOLA";
      break;
    case GeomAbs_Parabola:
      N = "PARABOLA";
      break;
    case GeomAbs_BezierCurve:
      N = "BEZIER";
      break;
    case GeomAbs_BSplineCurve:
      N = "BSPLINE";
      break;
    case GeomAbs_OffsetCurve:
      N = "OFFSET";
      break;
    case GeomAbs_OtherCurve:
      N = "OTHER";
      break;
    default:
      N = "UNKNOWN";
      break;
  }
}
#endif

#include <Bnd_Box.hxx>
#include <BRep_Tool.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <GeomTools_Curve2dSet.hxx>
#include <GeomTools_CurveSet.hxx>
#include <GeomTools_SurfaceSet.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <Precision.hxx>
#include <Standard_Failure.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRep_EdgesIntersector.hxx>
#include <TopOpeBRep_Point2d.hxx>
#include <TopOpeBRepDS_Transition.hxx>
#include <TopOpeBRepTool_2d.hxx>
#include <TopOpeBRepTool_CurveTool.hxx>
#include <TopOpeBRepTool_GEOMETRY.hxx>
#include <TopOpeBRepTool_PROJECT.hxx>
#include <TopOpeBRepTool_TOPOLOGY.hxx>
#include <TopOpeBRepTool_ShapeTool.hxx>
#include <TopOpeBRepTool_tol.hxx>
#include <TopOpeBRepTool_TOOL.hxx>

#ifdef OCCT_DEBUG
extern bool TopOpeBRepTool_GettraceNYI();
extern bool TopOpeBRepTool_GettraceKRO();
extern bool TopOpeBRep_GettracePROEDG();
extern bool TopOpeBRep_GetcontextTOL0();
extern bool TopOpeBRep_GetcontextNOFEI();
extern bool TopOpeBRep_GettraceFITOL();
extern bool TopOpeBRep_GettraceEEFF();
extern void debeeff();
  #include <TopOpeBRepTool_KRO.hxx>
Standard_EXPORT TOPKRO KRO_DSFILLER_INTEE("intersection edge/edge");
#endif

// la surface de reference peut etre celle de la 1ere ou la 2eme face
// de l'appel de SetFaces. Ces deux faces sont "SameDomain".
// Leurs normales geometriques sont SurfacesSameOriented()
// Leurs normales topologiques sont FacesSameOriented()
// cas type 1 :
//    face1 FORWARD, normale geometrique Ng1 en +Z
//    face2 REVERSED, normale geometrique Ng2 en -Z
// ==> SurfaceSameOriented = 0, FacesSameOriented = 1

//=================================================================================================

TopOpeBRep_EdgesIntersector::TopOpeBRep_EdgesIntersector()
{
  mySurface1              = new BRepAdaptor_Surface();
  mySurface2              = new BRepAdaptor_Surface();
  mySurfacesSameOriented  = false;
  myFacesSameOriented     = false;
  myTol1                  = 0.; // Precision::PConfusion();
  myTol2                  = 0.; // Precision::PIntersection();
  myDimension             = 2;
  myTolForced             = false;
  myf1surf1F_sameoriented = true;
  myf2surf1F_sameoriented = true;

  myNbSegments = 0;
  myHasSegment = false;
  SetSameDomain(false);

  myNbPoints     = 0;
  myTrueNbPoints = 0;
  myPointIndex   = 0;
  myip2d = mynp2d = 0;
  myselectkeep    = true;
}

TopOpeBRep_EdgesIntersector::~TopOpeBRep_EdgesIntersector() = default;

//=================================================================================================

void TopOpeBRep_EdgesIntersector::SetFaces(const TopoDS_Shape& F1, const TopoDS_Shape& F2)
{
  Bnd_Box B1, B2;
  SetFaces(F1, F2, B1, B2);
}

//=================================================================================================

void TopOpeBRep_EdgesIntersector::SetFaces(const TopoDS_Shape& F1,
                                           const TopoDS_Shape& F2,
                                           const Bnd_Box&      B1,
                                           const Bnd_Box&      B2)
{
  bool computerestriction = false;

  bool so11               = true;
  bool so21               = true;
  myf1surf1F_sameoriented = so11;
  myf2surf1F_sameoriented = so21;
  mySurfacesSameOriented  = true;
  myFacesSameOriented     = true;

  myFace1                 = TopoDS::Face(F1);
  BRepAdaptor_Surface& S1 = *mySurface1;
  S1.Initialize(myFace1, computerestriction);
  mySurfaceType1 = S1.GetType();

  myFace2                 = TopoDS::Face(F2);
  BRepAdaptor_Surface& S2 = *mySurface2;
  S2.Initialize(myFace2, computerestriction);
  mySurfaceType2 = S2.GetType();

  TopoDS_Face face1forward = myFace1;
  face1forward.Orientation(TopAbs_FORWARD);

  so11                    = TopOpeBRepTool_ShapeTool::FacesSameOriented(face1forward, myFace1);
  myf1surf1F_sameoriented = so11;

  so21                    = TopOpeBRepTool_ShapeTool::FacesSameOriented(face1forward, myFace2);
  myf2surf1F_sameoriented = so21;

  mySurfacesSameOriented = TopOpeBRepTool_ShapeTool::SurfacesSameOriented(S1, S2);
  myFacesSameOriented    = TopOpeBRepTool_ShapeTool::FacesSameOriented(myFace1, myFace2);

  if (!myTolForced)
  {
    FTOL_FaceTolerances2d(B1, B2, myFace1, myFace2, S1, S2, myTol1, myTol2);
    myTol1 = (myTol1 > 1.e-4) ? 1.e-4 : myTol1;
    myTol2 = (myTol2 > 1.e-4) ? 1.e-4 : myTol2;
  }

#ifdef OCCT_DEBUG
  int DEBi = 0;
  if (DEBi)
  {
    std::cout << "TopOpeBRep_EdgesIntersector::SetFaces : ";
    std::cout << "f1 ";
    TopAbs::Print(myFace1.Orientation(), std::cout);
    std::cout << " / f1F : ";
    if (so11)
      std::cout << "sameoriented";
    else
      std::cout << "difforiented";
    std::cout << std::endl;
    std::cout << "  ";
    std::cout << "f2 ";
    TopAbs::Print(myFace2.Orientation(), std::cout);
    std::cout << " / f1F : ";
    if (so21)
      std::cout << "sameoriented";
    else
      std::cout << "difforiented";
    std::cout << std::endl;
  }
#endif
}

//=================================================================================================

void TopOpeBRep_EdgesIntersector::ForceTolerances(const double Tol1, const double Tol2)
{
  myTol1      = Tol1;
  myTol2      = Tol2;
  myTolForced = true;
}

#include <IntRes2d_Transition.hxx>

static bool TransitionEqualAndExtremity(const IntRes2d_Transition& T1,
                                        const IntRes2d_Transition& T2)
{
  if (T1.PositionOnCurve() == IntRes2d_Head || T1.PositionOnCurve() == IntRes2d_End)
  {
    if (T1.PositionOnCurve() == T2.PositionOnCurve())
    {
      if (T1.TransitionType() == T2.TransitionType())
      {
        if (T1.TransitionType() == IntRes2d_Touch)
        {
          if (T1.IsTangent() == T2.IsTangent())
          {
            if (T1.Situation() == T2.Situation())
            {
              if (T1.IsOpposite() == T2.IsOpposite())
              {
                return (true);
              }
            }
          }
        }
        else
        {
          return (true);
        }
      }
    }
  }
  return (false);
}

//  Modified by Sergey KHROMOV - Fri Jan 11 14:49:48 2002 Begin
static bool IsTangentSegment(const IntRes2d_IntersectionPoint& P1,
                             const IntRes2d_IntersectionPoint& P2,
                             const Geom2dAdaptor_Curve&        aC1,
                             const Geom2dAdaptor_Curve&        aC2,
                             const double                      aTolConf)
{
  const gp_Pnt2d&            aP2d1   = P1.Value();
  const gp_Pnt2d&            aP2d2   = P2.Value();
  const IntRes2d_Transition& aTrans1 = P1.TransitionOfFirst();
  const IntRes2d_Transition& aTrans2 = P2.TransitionOfFirst();

  if (aTrans1.TransitionType() == IntRes2d_Touch || aTrans2.TransitionType() == IntRes2d_Touch)
  {
    double aSqrDistPP = aP2d1.SquareDistance(aP2d2);

    if (aSqrDistPP <= aTolConf)
    {
      double aParDist1 = std::abs(P1.ParamOnFirst() - P2.ParamOnFirst());
      double aParDist2 = std::abs(P1.ParamOnSecond() - P2.ParamOnSecond());
      double aResol1   = aC1.Resolution(aTolConf);
      double aResol2   = aC2.Resolution(aTolConf);

      if (aParDist1 * aParDist1 <= aResol1 && aParDist2 * aParDist2 <= aResol2)
        return true;
    }
  }

  return false;
}
//  Modified by Sergey KHROMOV - Fri Jan 11 14:49:49 2002 End

//------------------------------------------------------------------------
bool EdgesIntersector_checkT1D(const TopoDS_Edge&       E1,
                               const TopoDS_Edge&       E2,
                               const TopoDS_Vertex&     vG,
                               TopOpeBRepDS_Transition& newT)
//------------------------------------------------------------------------
// E1 sdm E2, interferes with E2 at vertex vG
// vG is vertex of E2, but not vertex of E1
// purpose : get newT / attached to E1, I1d=(newT(E2),G,E2)
{
#define FIRST (1)
#define LAST (2)
#define CLOSING (3)

  newT.Set(TopAbs_UNKNOWN, TopAbs_UNKNOWN);
  int ovine = FUN_tool_orientVinE(vG, E2);
  if (ovine == 0)
  {
    return false;
  }
  else if (ovine == CLOSING)
  {
    newT.Set(TopAbs_INTERNAL);
    return true;
  }

  bool first = (ovine == FIRST);
  bool last  = (ovine == LAST);

  TopOpeBRepDS_Config C   = TopOpeBRepDS_SAMEORIENTED;
  bool                sso = TopOpeBRepTool_ShapeTool::ShapesSameOriented(E1, E2);
  if (!sso)
    C = TopOpeBRepDS_DIFFORIENTED;

  bool               SO = (C == TopOpeBRepDS_SAMEORIENTED);
  bool               DO = (C == TopOpeBRepDS_DIFFORIENTED);
  TopAbs_Orientation o1 = E1.Orientation();
  if (o1 == TopAbs_REVERSED)
  {
    SO = !SO;
    DO = !DO;
  } // relative to E1 FORWARD

  bool reversed = (SO && first) || (DO && last);
  bool forward  = (SO && last) || (DO && first);
  if (reversed)
    newT.Set(TopAbs_REVERSED);
  if (forward)
    newT.Set(TopAbs_FORWARD);
  return (reversed || forward);
} // EdgesIntersector_checkT1D

// modified by NIZNHY-PKV Fri Nov  5 12:27:07 1999 from
#include <BRepAdaptor_Surface.hxx>

// modified by NIZNHY-PKV Fri Nov  5 12:27:10 1999 to
//=================================================================================================

void TopOpeBRep_EdgesIntersector::Perform(const TopoDS_Shape& E1,
                                          const TopoDS_Shape& E2,
                                          const bool          ReduceSegment)
{
  mysp2d.Clear();
  myip2d = 1;
  mynp2d = 0;

  myEdge1 = TopoDS::Edge(E1);
  myEdge2 = TopoDS::Edge(E2);

  double                    first, last, tole, tolpc;
  gp_Pnt2d                  pfirst, plast;
  occ::handle<Geom2d_Curve> PC1;
  // modified by NIZNHY-PKV Thu Nov  4 16:08:05 1999 f

  BRepAdaptor_Surface aSurface1(myFace1), aSurface2(myFace2);
  GeomAbs_SurfaceType aSurfaceType1 = aSurface1.GetType(), aSurfaceType2 = aSurface2.GetType();

  if (aSurfaceType1 == GeomAbs_Sphere && aSurfaceType2 == GeomAbs_Sphere)
  {
    PC1 = FC2D_MakeCurveOnSurface(myEdge1, myFace1, first, last, tolpc, true);
  }
  else
  {
    PC1 = FC2D_CurveOnSurface(myEdge1, myFace1, first, last, tolpc);
  }
  // modified by NIZNHY-PKV Thu Nov  4 15:44:13 1999 to

  if (PC1.IsNull())
    throw Standard_Failure("EdgesIntersector::Perform : no 2d curve");

  myCurve1.Load(PC1);
  BRep_Tool::UVPoints(myEdge1, myFace1, pfirst, plast);
  tole = BRep_Tool::Tolerance(myEdge1);
  myDomain1.SetValues(pfirst, first, tole, plast, last, tole);

#ifdef OCCT_DEBUG
  bool trc = false;
  if (trc)
  {
    std::cout << "ed1 on fa1 : {pfirst=(" << pfirst.X() << " " << pfirst.Y() << "), first=" << first
              << "\n";
    std::cout << "              plast =(" << plast.X() << " " << plast.Y() << "),last=" << last
              << "}" << std::endl;
  }
#endif

  bool                            memesfaces  = myFace1.IsSame(myFace2);
  bool                            memesupport = false;
  TopLoc_Location                 L1, L2;
  const occ::handle<Geom_Surface> S1 = BRep_Tool::Surface(myFace1, L1);
  const occ::handle<Geom_Surface> S2 = BRep_Tool::Surface(myFace2, L2);
  if (S1 == S2 && L1 == L2)
    memesupport = true;

  if (mySurfaceType1 == GeomAbs_Plane || memesfaces || memesupport)
  {
    occ::handle<Geom2d_Curve> PC2 = FC2D_CurveOnSurface(myEdge2, myFace1, first, last, tolpc);
    myCurve2.Load(PC2);
    BRep_Tool::UVPoints(myEdge2, myFace1, pfirst, plast);
    tole = BRep_Tool::Tolerance(myEdge2);
    myDomain2.SetValues(pfirst, first, tole, plast, last, tole);

#ifdef OCCT_DEBUG
    if (trc)
    {
      std::cout << "ed2 on fa1 : {pfirst=(" << pfirst.X() << " " << pfirst.Y()
                << "), first=" << first << "\n";
      std::cout << "              plast =(" << plast.X() << " " << plast.Y() << "),last=" << last
                << "}" << std::endl;
    }
#endif
  }

  else
  {

    occ::handle<Geom2d_Curve> PC2on1;
    occ::handle<Geom_Curve>   NC;
    bool                      dgE2 = BRep_Tool::Degenerated(myEdge2);
    if (dgE2)
    { // xpu210998 : cto900Q3
      TopExp_Explorer      exv(myEdge2, TopAbs_VERTEX);
      const TopoDS_Vertex& v2  = TopoDS::Vertex(exv.Current());
      gp_Pnt               pt2 = BRep_Tool::Pnt(v2);
      gp_Pnt2d             uv2;
      double               d;
      bool                 ok = FUN_tool_projPonF(pt2, myFace1, uv2, d);
      if (!ok)
        return; // nyiRaise

      occ::handle<Geom_Surface> aSurf1 = BRep_Tool::Surface(myFace1);
      bool                      apex   = FUN_tool_onapex(uv2, aSurf1);
      if (apex)
      {
        TopoDS_Vertex vf, vl;
        TopExp::Vertices(myEdge1, vf, vl);
        gp_Pnt ptf = BRep_Tool::Pnt(vf);
        double df  = pt2.Distance(ptf);

        double tolf = BRep_Tool::Tolerance(vf);

        bool          onf = (df < tolf);
        TopoDS_Vertex v1  = onf ? vf : vl;
        NCollection_IndexedDataMap<TopoDS_Shape,
                                   NCollection_List<TopoDS_Shape>,
                                   TopTools_ShapeMapHasher>
          mapVE;
        TopExp::MapShapesAndAncestors(myFace1, TopAbs_VERTEX, TopAbs_EDGE, mapVE);
        const NCollection_List<TopoDS_Shape>&    Edsanc = mapVE.FindFromKey(v1);
        NCollection_List<TopoDS_Shape>::Iterator it(Edsanc);
        for (; it.More(); it.Next())
        {
          const TopoDS_Edge& ee   = TopoDS::Edge(it.Value());
          bool               dgee = BRep_Tool::Degenerated(ee);
          if (!dgee)
            continue;
          //	  double f,l;
          PC2on1 = BRep_Tool::CurveOnSurface(ee, myFace1, first, last);
        }
      }
      else
      {
      } // NYIxpu210998
    } // dgE2
    else
    {
      // project curve of edge 2 on surface of face 1
      TopLoc_Location         loc;
      occ::handle<Geom_Curve> C = BRep_Tool::Curve(myEdge2, loc, first, last);
      NC                        = occ::down_cast<Geom_Curve>(C->Transformed(loc.Transformation()));
      double tolreached2d;

      // modified by NIZNHY-PKV Fri Nov  5 12:29:13 1999 from
      if (aSurfaceType1 == GeomAbs_Sphere && aSurfaceType2 == GeomAbs_Sphere)
      {
        PC2on1 = FC2D_MakeCurveOnSurface(myEdge2, myFace1, first, last, tolpc, true);
      }
      else
      {
        PC2on1 = TopOpeBRepTool_CurveTool::MakePCurveOnFace(myFace1, NC, tolreached2d);
      }
      // modified by NIZNHY-PKV Thu Nov  4 14:52:25 1999 t
    }

    if (!PC2on1.IsNull())
    {
      myCurve2.Load(PC2on1);
      tole = BRep_Tool::Tolerance(myEdge2);
      PC2on1->D0(first, pfirst);
      PC2on1->D0(last, plast);
      myDomain2.SetValues(pfirst, first, tole, plast, last, tole);
#ifdef OCCT_DEBUG
      if (TopOpeBRep_GettracePROEDG())
      {
        std::cout << "------------ projection de curve" << std::endl;
        std::cout << "--- Curve : " << std::endl;
        GeomTools_CurveSet::PrintCurve(NC, std::cout);
        std::cout << "--- nouvelle PCurve : " << std::endl;
        GeomTools_Curve2dSet::PrintCurve2d(PC2on1, std::cout);
        occ::handle<Geom_Surface> aS1 = BRep_Tool::Surface(myFace1);
        std::cout << "--- sur surface : " << std::endl;
        GeomTools_SurfaceSet::PrintSurface(aS1, std::cout);
        std::cout << std::endl;
      }
#endif
    }
    else
      return;
  }

  // compute the intersection
#ifdef OCCT_DEBUG
  if (TopOpeBRepTool_GettraceKRO())
    KRO_DSFILLER_INTEE.Start();
#endif

  double tol1 = myTol1, tol2 = myTol2;
  // Wrong !!!
  /*  if ( !myTolForced ) {
      if ( t1 != t2 ) {
        //#ifdef OCCT_DEBUG // JYL 28/09/98 : temporaire
        //if ( TopOpeBRep_GetcontextTOL0() ) { // JYL 28/09/98 : temporaire
        tol1 = 0.; // JYL 28/09/98 : temporaire
        tol2 = 0.; // JYL 28/09/98 : temporaire
        //} // JYL 28/09/98 : temporaire
        //#endif // JYL 28/09/98 : temporaire
      }
    }
  */

#ifdef OCCT_DEBUG
  if (TopOpeBRep_GettraceFITOL())
  {
    std::cout << "EdgesIntersector : Perform";
  #ifdef DRAW
    GeomAbs_CurveType       t1 = myCurve1.GetType();
    GeomAbs_CurveType       t2 = myCurve2.GetType();
    TCollection_AsciiString s1;
    CurveToString(t1, s1);
    std::cout << " " << s1;
    TCollection_AsciiString s2;
    CurveToString(t2, s2);
    std::cout << " " << s2;
  #endif
    std::cout << std::endl;
    std::cout << "                   tol1 = " << tol1 << std::endl;
    std::cout << "                   tol2 = " << tol2 << std::endl;
  }
#endif

  myIntersector.Perform(myCurve1, myDomain1, myCurve2, myDomain2, tol1, tol2);

  int nbp = myIntersector.NbPoints();
  int nbs = myIntersector.NbSegments();

  mylpnt.Clear();
  mylseg.Clear();
  //  for (int p=1; p<=nbp; p++) mylpnt.Append(myIntersector.Point(p));
  int p;
  for (p = 1; p <= nbp; p++)
    mylpnt.Append(myIntersector.Point(p));
  for (int s = 1; s <= nbs; s++)
    mylseg.Append(myIntersector.Segment(s));

  bool filter = true;
#ifdef OCCT_DEBUG
  bool nofilter = TopOpeBRep_GetcontextNOFEI();
  if (nofilter)
    filter = false;
#endif

  //-- Filter :
  if (filter)
  {
    bool fin;
    do
    {
      fin = true;
      for (p = 1; p < nbp && fin; p++)
      {
        const IntRes2d_IntersectionPoint& P1 = mylpnt.Value(p);
        const IntRes2d_IntersectionPoint& P2 = mylpnt.Value(p + 1);
        if (TransitionEqualAndExtremity(P1.TransitionOfFirst(), P2.TransitionOfFirst())
            || TransitionEqualAndExtremity(P1.TransitionOfSecond(), P2.TransitionOfSecond()))
        {
#ifdef OCCT_DEBUG
          bool TRC = true;
          if (TRC)
            std::cout << "\n Egalite de transitions \n" << std::endl;
#endif
          fin = false;
          mylpnt.Remove(p);
          nbp--;
        }
        //  Modified by Sergey KHROMOV - Fri Jan 11 10:31:38 2002 Begin
        else if (IsTangentSegment(P1, P2, myCurve1, myCurve2, std::max(tol1, tol2)))
        {
          const IntRes2d_Transition& aTrans = P2.TransitionOfFirst();

          fin = false;
          if (aTrans.TransitionType() == IntRes2d_Touch)
            mylpnt.Remove(p);
          else
            mylpnt.Remove(p + 1);
          nbp--;
        }
        //  Modified by Sergey KHROMOV - Fri Jan 11 10:31:39 2002 End
      }
    } while (fin == false);
  }
  //-- End filter

  myNbSegments = mylseg.Length();
  myHasSegment = (myNbSegments != 0);
  ComputeSameDomain();

  myNbPoints     = mylpnt.Length();
  myTrueNbPoints = myNbPoints + 2 * myNbSegments;
  myPointIndex   = 0;

#ifdef OCCT_DEBUG
  if (TopOpeBRep_GettraceEEFF())
    debeeff();
#endif

  MakePoints2d();
  if (ReduceSegment)
    ReduceSegments();

  // xpu010998 : cto900J1, e5 sdm e13, IntPatch (Touch,Inside) ->
  //             faulty INTERNAL transition at G=v9 :
  // xpu281098 : cto019D2, e3 sdm e9, faulty EXTERNAL transition
  bool esd = SameDomain();
  for (InitPoint(); MorePoint(); NextPoint())
  {
    TopOpeBRep_Point2d& P2D       = mysp2d(myip2d);
    bool                isvertex1 = P2D.IsVertex(1);
    bool                isvertex2 = P2D.IsVertex(2);
    bool                isvertex  = isvertex1 || isvertex2;

    if (isvertex && esd)
    {
      TopOpeBRepDS_Transition& T1 = P2D.ChangeTransition(1);
      TopOpeBRepDS_Transition& T2 = P2D.ChangeTransition(2);

      bool isvertex12 = isvertex1 && isvertex2;
      bool isvertex22 = isvertex2 && !isvertex12;
      bool isvertex11 = isvertex1 && !isvertex12;

      bool T1INT = (T1.Orientation(TopAbs_IN) == TopAbs_INTERNAL);

      if (T1INT && isvertex2 && !isvertex1)
      {
        const TopoDS_Vertex&    V2 = P2D.Vertex(2);
        TopOpeBRepDS_Transition newT;
        bool                    computed = ::EdgesIntersector_checkT1D(myEdge1, myEdge2, V2, newT);
        if (computed)
          T1.Set(newT.Orientation(TopAbs_IN));
      }

      bool T2INT   = (T2.Orientation(TopAbs_IN) == TopAbs_INTERNAL);
      bool T2EXT   = (T2.Orientation(TopAbs_IN) == TopAbs_EXTERNAL);
      bool INTEXT2 = T2INT || T2EXT;
      if (INTEXT2 && isvertex1 && !isvertex2)
      {
        const TopoDS_Vertex&    V1 = P2D.Vertex(1);
        TopOpeBRepDS_Transition newT;
        bool                    computed = ::EdgesIntersector_checkT1D(myEdge2, myEdge1, V1, newT);
        if (computed)
          T2.Set(newT.Orientation(TopAbs_IN));
      }

      // xpu121098 : cto900I7 (e12on,vG14)
      TopoDS_Vertex vcl2;
      bool          clE2 = TopOpeBRepTool_TOOL::ClosedE(myEdge2, vcl2);
      bool          nT1  = (!T1INT && clE2 && isvertex22 && vcl2.IsSame(P2D.Vertex(2)));
      if (nT1)
        T1.Set(TopAbs_INTERNAL);
      TopoDS_Vertex vcl1;
      bool          clE1 = TopOpeBRepTool_TOOL::ClosedE(myEdge1, vcl1);
      bool          nT2  = (!T2INT && clE1 && isvertex11 && vcl1.IsSame(P2D.Vertex(1)));
      if (nT2)
        T2.Set(TopAbs_INTERNAL);

    } // (isvertex && esd)
  } // MorePoint

#ifdef OCCT_DEBUG
  if (TopOpeBRepTool_GettraceKRO())
    KRO_DSFILLER_INTEE.Stop();
#endif
} // Perform

//=================================================================================================

void TopOpeBRep_EdgesIntersector::Dimension(const int Dim)
{
  if (Dim == 1 || Dim == 2)
  {
    myDimension = Dim;
  }
}

//=================================================================================================

int TopOpeBRep_EdgesIntersector::Dimension() const
{
  return myDimension;
}

//=================================================================================================

bool TopOpeBRep_EdgesIntersector::ComputeSameDomain()
{
  const Geom2dAdaptor_Curve& C1 = Curve(1);
  const Geom2dAdaptor_Curve& C2 = Curve(2);
  GeomAbs_CurveType          t1 = C1.GetType();
  GeomAbs_CurveType          t2 = C2.GetType();

  if (!myHasSegment)
    return SetSameDomain(false);

  bool tt = (t1 == t2);
  if (!tt)
    return SetSameDomain(false);

  if (t1 == GeomAbs_Line)
    return SetSameDomain(true);

  if (t1 != GeomAbs_Circle)
  {
#ifdef OCCT_DEBUG
    if (TopOpeBRepTool_GettraceNYI())
      std::cout << "TopOpeBRep_EdgesIntersector : EdgesSameDomain on NYI curve type" << std::endl;
#endif
    return SetSameDomain(false);
  }

  gp_Circ2d c1 = C1.Circle();
  gp_Circ2d c2 = C2.Circle();
  double    r1 = c1.Radius();
  double    r2 = c2.Radius();
  //  bool rr = (r1 == r2);
  // clang-format off
  bool rr = (std::abs(r1-r2) < Precision::Confusion()); //xpu281098 (cto019D2) tolerance a revoir
  if (!rr) return SetSameDomain(false);

  const gp_Pnt2d& p1 = c1.Location();
  const gp_Pnt2d& p2 = c2.Location();

  const BRepAdaptor_Surface& BAS1 = Surface(1);
  double u1,v1; p1.Coord(u1,v1); gp_Pnt P1 = BAS1.Value(u1,v1);
  double u2,v2; p2.Coord(u2,v2); gp_Pnt P2 = BAS1.Value(u2,v2);// recall myCurve2=C2d(myEdge2,myFace1);
  // clang-format on
  double dpp  = P1.Distance(P2);
  double tol1 = BRep_Tool::Tolerance(TopoDS::Edge(Edge(1)));
  double tol2 = BRep_Tool::Tolerance(TopoDS::Edge(Edge(2)));
  double tol  = tol1 + tol2;
  bool   esd  = (dpp <= tol);
  if (esd)
    return SetSameDomain(true);

  return SetSameDomain(false);
} // ComputeSameDomain

//=================================================================================================

bool TopOpeBRep_EdgesIntersector::SetSameDomain(const bool B)
{
  mySameDomain = B;
  return B;
}

//=================================================================================================

void TopOpeBRep_EdgesIntersector::MakePoints2d()
{
  mysp2d.Clear();
  TopAbs_Orientation E1ori = myEdge1.Orientation();
  TopAbs_Orientation E2ori = myEdge2.Orientation();
  for (InitPoint1(); MorePoint1(); NextPoint1())
  {
    const IntRes2d_IntersectionPoint& IP = Point1();
    TopOpeBRep_Point2d                p2d;
    p2d.SetPint(IP);
    p2d.SetTransition(1, Transition1(1, E2ori));
    p2d.SetTransition(2, Transition1(2, E1ori));
    p2d.SetParameter(1, Parameter1(1));
    p2d.SetParameter(2, Parameter1(2));
    bool isv1 = IsVertex1(1);
    p2d.SetIsVertex(1, isv1);
    if (isv1)
      p2d.SetVertex(1, TopoDS::Vertex(Vertex1(1)));
    bool isv2 = IsVertex1(2);
    p2d.SetIsVertex(2, isv2);
    if (isv2)
      p2d.SetVertex(2, TopoDS::Vertex(Vertex1(2)));
    p2d.SetIsPointOfSegment(IsPointOfSegment1());
    p2d.SetSegmentAncestors(0, 0);
    p2d.SetStatus(Status1());
    p2d.SetValue(Value1());
    p2d.SetValue2d(IP.Value());
    p2d.SetTolerance(ToleranceMax());
    p2d.SetEdgesConfig(EdgesConfig1());
    p2d.SetIndex(Index1());
    mysp2d.Append(p2d);
  }
  myip2d = 1;
  mynp2d = mysp2d.Length();
}

//=================================================================================================

bool TopOpeBRep_EdgesIntersector::ReduceSegment(TopOpeBRep_Point2d& psa,
                                                TopOpeBRep_Point2d& psb,
                                                TopOpeBRep_Point2d& Pn) const
{
  bool reduced = false;
  int  ixpsa   = psa.Index();
  int  ixpsb   = psb.Index();

  bool                           pospsa = psa.IsPointOfSegment();
  TopOpeBRep_P2Dstatus           stspsa = psa.Status();
  double                         tpsa1  = psa.Parameter(1);
  double                         tpsa2  = psa.Parameter(2);
  const TopOpeBRepDS_Transition& Tpsa1  = psa.Transition(1);
  const TopOpeBRepDS_Transition& Tpsa2  = psa.Transition(2);

  bool                           pospsb = psb.IsPointOfSegment();
  TopOpeBRep_P2Dstatus           stspsb = psb.Status();
  double                         tpsb1  = psb.Parameter(1);
  double                         tpsb2  = psb.Parameter(2);
  const TopOpeBRepDS_Transition& Tpsb1  = psb.Transition(1);
  const TopOpeBRepDS_Transition& Tpsb2  = psb.Transition(2);

  bool conda = (pospsa && (stspsa == TopOpeBRep_P2DSGF));
  bool condb = (pospsb && (stspsb == TopOpeBRep_P2DSGL));
  bool cond  = (conda && condb);

  if (cond)
  {
    reduced = true;

    double tm1 = (tpsa1 + tpsb1) / 2.;
    Pn.SetParameter(1, tm1);
    double tm2 = (tpsa2 + tpsb2) / 2.;
    Pn.SetParameter(2, tm2);

    TopOpeBRepDS_Transition Tn1;
    Tn1.Before(Tpsa1.Before(), Tpsa1.ShapeBefore());
    Tn1.After(Tpsb1.After(), Tpsb1.ShapeAfter());
    Pn.SetTransition(1, Tn1);
    TopOpeBRepDS_Transition Tn2;
    Tn2.Before(Tpsa2.Before(), Tpsa2.ShapeBefore());
    Tn2.After(Tpsb2.After(), Tpsb2.ShapeAfter());
    Pn.SetTransition(2, Tn2);

    const gp_Pnt& P3Dpsa = psa.Value();
    const gp_Pnt& P3Dpsb = psb.Value();
    gp_Pnt        P3Dn((P3Dpsa.X() + P3Dpsb.X()) / 2,
                (P3Dpsa.Y() + P3Dpsb.Y()) / 2,
                (P3Dpsa.Z() + P3Dpsb.Z()) / 2);
    Pn.SetValue(P3Dn);
    const gp_Pnt2d& P2Dpsa = psa.Value2d();
    const gp_Pnt2d& P2Dpsb = psb.Value2d();
    gp_Pnt2d        P2Dn((P2Dpsa.X() + P2Dpsb.X()) / 2, (P2Dpsa.Y() + P2Dpsb.Y()) / 2);
    Pn.SetValue2d(P2Dn);

    double tolpsa = psa.Tolerance();
    double tolpsb = psb.Tolerance();
    double toln   = (tolpsa + tolpsb) * 1.5;
    Pn.SetTolerance(toln);

    Pn.SetIsPointOfSegment(false);
    Pn.SetSegmentAncestors(ixpsa, ixpsb);
    psa.SetKeep(false);
    psb.SetKeep(false);

    TopOpeBRepDS_Config cpsa = psa.EdgesConfig();

    Pn.SetEdgesConfig(cpsa);

    bool isvpsa1 = psa.IsVertex(1);
    if (isvpsa1)
      Pn.SetVertex(1, psa.Vertex(1));
    bool isvpsa2 = psa.IsVertex(2);
    if (isvpsa2)
      Pn.SetVertex(2, psa.Vertex(2));
    bool isvpsb1 = psb.IsVertex(1);
    if (isvpsb1)
      Pn.SetVertex(1, psb.Vertex(1));
    bool isvpsb2 = psb.IsVertex(2);
    if (isvpsb2)
      Pn.SetVertex(2, psb.Vertex(2));
  }

  return reduced;
} // ReduceSegment

//=================================================================================================

void TopOpeBRep_EdgesIntersector::ReduceSegments()
{
  bool condredu = (myHasSegment && !mySameDomain);
  if (!condredu)
    return;

  int ip = 1;
  int np = mynp2d;
  while (ip < np)
  {
    TopOpeBRep_Point2d& psa = mysp2d(ip);
    TopOpeBRep_Point2d& psb = mysp2d(ip + 1);
    TopOpeBRep_Point2d  pn;
    bool                reduced = ReduceSegment(psa, psb, pn);
    if (reduced)
    {
      pn.SetIndex(++mynp2d);
      mysp2d.Append(pn);
    }
    ip++;
  }

  mylseg.Clear();
  myNbSegments   = mylseg.Length();
  myHasSegment   = (myNbSegments != 0);
  myTrueNbPoints = myNbPoints + 2 * myNbSegments;

} // ReduceSegments

//=================================================================================================

bool TopOpeBRep_EdgesIntersector::IsEmpty()
{
  return (mynp2d == 0);
}

//=================================================================================================

bool TopOpeBRep_EdgesIntersector::HasSegment() const
{
  return myHasSegment;
}

//=================================================================================================

bool TopOpeBRep_EdgesIntersector::SameDomain() const
{
  return mySameDomain;
}

//=================================================================================================

const TopoDS_Shape& TopOpeBRep_EdgesIntersector::Edge(const int Index) const
{
  if (Index == 1)
    return myEdge1;
  else if (Index == 2)
    return myEdge2;
  else
    throw Standard_Failure("TopOpeBRep_EdgesIntersector::Edge");
}

//=================================================================================================

const Geom2dAdaptor_Curve& TopOpeBRep_EdgesIntersector::Curve(const int Index) const
{
  if (Index == 1)
    return myCurve1;
  else if (Index == 2)
    return myCurve2;
  else
    throw Standard_Failure("TopOpeBRep_EdgesIntersector::Curve");
}

//=================================================================================================

const TopoDS_Shape& TopOpeBRep_EdgesIntersector::Face(const int Index) const
{
  if (Index == 1)
    return myFace1;
  else if (Index == 2)
    return myFace2;
  else
    throw Standard_Failure("TopOpeBRep_EdgesIntersector::Face");
}

//=================================================================================================

const BRepAdaptor_Surface& TopOpeBRep_EdgesIntersector::Surface(const int Index) const
{
  if (Index == 1)
    return *mySurface1;
  else if (Index == 2)
    return *mySurface2;
  else
    throw Standard_Failure("TopOpeBRep_EdgesIntersector::Surface");
}

//=================================================================================================

bool TopOpeBRep_EdgesIntersector::SurfacesSameOriented() const
{
  return mySurfacesSameOriented;
}

//=================================================================================================

bool TopOpeBRep_EdgesIntersector::FacesSameOriented() const
{
  return myFacesSameOriented;
}

//=================================================================================================

void TopOpeBRep_EdgesIntersector::InitPoint(const bool selectkeep)
{
  myselectkeep = selectkeep;
  myip2d       = 1;
  mynp2d       = mysp2d.Length();
  Find();
}

//=================================================================================================

bool TopOpeBRep_EdgesIntersector::MorePoint() const
{
  bool b = (myip2d <= mynp2d);
  return b;
}

//=================================================================================================

void TopOpeBRep_EdgesIntersector::NextPoint()
{
  myip2d++;
  Find();
}

//=================================================================================================

void TopOpeBRep_EdgesIntersector::Find()
{
  while (myip2d <= mynp2d)
  {
    if (myselectkeep)
    {
      bool kf = mysp2d(myip2d).Keep();
      if (kf)
        break;
      else
        myip2d++;
    }
    else
    {
      break;
    }
  }
}

//=================================================================================================

const NCollection_Sequence<TopOpeBRep_Point2d>& TopOpeBRep_EdgesIntersector::Points() const
{
  return mysp2d;
}

//=================================================================================================

const TopOpeBRep_Point2d& TopOpeBRep_EdgesIntersector::Point() const
{
  return mysp2d(myip2d);
}

//=================================================================================================

const TopOpeBRep_Point2d& TopOpeBRep_EdgesIntersector::Point(const int I) const
{
  if (I < 1 || I > mysp2d.Length())
    throw Standard_Failure("TopOpeBRep_EdgesIntersector::Point(I)");
  return mysp2d(I);
}

//=================================================================================================

double TopOpeBRep_EdgesIntersector::ToleranceMax() const
{
  double tol = std::max(myTol1, myTol2);
  return tol;
}

//=================================================================================================

void TopOpeBRep_EdgesIntersector::Tolerances(double& tol1, double& tol2) const
{
  tol1 = myTol1;
  tol2 = myTol2;
}

//=================================================================================================

int TopOpeBRep_EdgesIntersector::NbPoints() const
{
  return myNbPoints;
}

//=================================================================================================

int TopOpeBRep_EdgesIntersector::NbSegments() const
{
  return myNbSegments;
}

//=================================================================================================

#ifndef OCCT_DEBUG
void TopOpeBRep_EdgesIntersector::Dump(const TCollection_AsciiString&, const int, const int)
{
#else
void TopOpeBRep_EdgesIntersector::Dump(const TCollection_AsciiString& str,
                                       const int                      E1index,
                                       const int                      E2index)
{
  InitPoint();
  if (!MorePoint())
    return;
  std::cout << std::endl << "---- " << str << " ---- E/E : " << NbPoints() << " p , ";
  std::cout << NbSegments() << " s : " << myTrueNbPoints << " true points" << std::endl;
  std::cout << "E1 = " << E1index << " ";
  TopAbs::Print(Edge(1).Orientation(), std::cout) << ", ";
  std::cout << "E2 = " << E2index << " ";
  TopAbs::Print(Edge(2).Orientation(), std::cout) << "  ";
  std::cout << "hs=" << HasSegment() << " hsd=" << SameDomain() << std::endl;

  for (InitPoint(); MorePoint(); NextPoint())
  {
    const TopOpeBRep_Point2d P2d = Point();
    P2d.Dump(E1index, E2index);
    if (P2d.Status() == TopOpeBRep_P2DNEW)
    {
      int ip1, ip2;
      P2d.SegmentAncestors(ip1, ip2);
      double d1 = Point(ip1).Value().Distance(Point(ip2).Value());
      double d2 = d1 + Point(ip1).Tolerance() / 2. + Point(ip2).Tolerance() / 2.;
      std::cout << "ancestor segment : d P" << ip1 << ",P" << ip2 << " = " << d1 << std::endl;
      std::cout << "     t1/2 + d + t2/2 P" << ip1 << ",P" << ip2 << " = " << d2 << std::endl;
    }
  }
  std::cout << std::endl;
#endif
}
