// Created on: 1994-03-24
// Created by: Isabelle GRIGNON
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

#ifndef ChFi3d_Builder_0_HeaderFile
#define ChFi3d_Builder_0_HeaderFile

#include <TopOpeBRepDS_SurfaceCurveInterference.hxx>
#include <TopOpeBRepDS_CurvePointInterference.hxx>
#include <TopOpeBRepDS_DataStructure.hxx>
#include <BRepBlend_Extremity.hxx>
#include <ChFiDS_Stripe.hxx>
#include <ChFiDS_SurfData.hxx>
#include <ChFiDS_Spine.hxx>
#include <ChFiDS_ElSpine.hxx>
#include <ChFiDS_CommonPoint.hxx>
#include <ChFiDS_Regul.hxx>
#include <NCollection_List.hxx>
#include <ChFiDS_FaceInterference.hxx>
#include <ChFiDS_Map.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <IntSurf_TypeTrans.hxx>
#include <GeomFill_BoundWithSurf.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom_Surface.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_Circle.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <Adaptor3d_Curve.hxx>
#include <Adaptor3d_Surface.hxx>
#include <Bnd_Box.hxx>
#include <GeomAbs_Shape.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_Array1.hxx>
#ifdef OCCT_DEBUG
  #include <OSD_Chronometer.hxx>
extern OSD_Chronometer simul, elspine, chemine;
#endif

double ChFi3d_InPeriod(const double U,
                              const double UFirst,
                              const double ULast,
                              const double Eps);

void ChFi3d_Boite(const gp_Pnt2d& p1,
                  const gp_Pnt2d& p2,
                  double&  mu,
                  double&  Mu,
                  double&  mv,
                  double&  Mv);

void ChFi3d_Boite(const gp_Pnt2d& p1,
                  const gp_Pnt2d& p2,
                  const gp_Pnt2d& p3,
                  const gp_Pnt2d& p4,
                  double&  Du,
                  double&  Dv,
                  double&  mu,
                  double&  Mu,
                  double&  mv,
                  double&  Mv);

void ChFi3d_SetPointTolerance(TopOpeBRepDS_DataStructure& DStr,
                              const Bnd_Box&              box,
                              const int      IP);

void ChFi3d_EnlargeBox(const occ::handle<Geom_Curve>& C,
                       const double       wd,
                       const double       wf,
                       Bnd_Box&                  box1,
                       Bnd_Box&                  box2);

void ChFi3d_EnlargeBox(const occ::handle<Adaptor3d_Surface>& S,
                       const occ::handle<Geom2d_Curve>&      PC,
                       const double              wd,
                       const double              wf,
                       Bnd_Box&                         box1,
                       Bnd_Box&                         box2);

void ChFi3d_EnlargeBox(const TopoDS_Edge&          E,
                       const NCollection_List<TopoDS_Shape>& LF,
                       const double         w,
                       Bnd_Box&                    box);

void ChFi3d_EnlargeBox(TopOpeBRepDS_DataStructure&    DStr,
                       const occ::handle<ChFiDS_Stripe>&   st,
                       const occ::handle<ChFiDS_SurfData>& sd,
                       Bnd_Box&                       b1,
                       Bnd_Box&                       b2,
                       const bool         isfirst);

GeomAbs_Shape ChFi3d_evalconti(const TopoDS_Edge& E, const TopoDS_Face& F1, const TopoDS_Face& F2);

void ChFi3d_conexfaces(const TopoDS_Edge& E,
                       TopoDS_Face&       F1,
                       TopoDS_Face&       F2,
                       const ChFiDS_Map&  EFMap);

ChFiDS_State ChFi3d_EdgeState(TopoDS_Edge* E, const ChFiDS_Map& EFMap);

bool ChFi3d_KParticular(const occ::handle<ChFiDS_Spine>& Spine,
                                    const int      IE,
                                    const BRepAdaptor_Surface&  S1,
                                    const BRepAdaptor_Surface&  S2);

void ChFi3d_BoundFac(BRepAdaptor_Surface&   S,
                     const double    umin,
                     const double    umax,
                     const double    vmin,
                     const double    vmax,
                     const bool checknaturalbounds = true);

void ChFi3d_BoundSrf(GeomAdaptor_Surface&   S,
                     const double    umin,
                     const double    umax,
                     const double    vmin,
                     const double    vmax,
                     const bool checknaturalbounds = true);

bool ChFi3d_InterPlaneEdge(const occ::handle<Adaptor3d_Surface>& Plan,
                                       const occ::handle<Adaptor3d_Curve>&   C,
                                       double&                   W,
                                       const bool           Sens,
                                       const double              tolc);

void ChFi3d_ExtrSpineCarac(const TopOpeBRepDS_DataStructure& DStr,
                           const occ::handle<ChFiDS_Stripe>&      cd,
                           const int            i,
                           const double               p,
                           const int            jf,
                           const int            sens,
                           gp_Pnt&                           P,
                           gp_Vec&                           V,
                           double&                    R);

occ::handle<Geom_Circle> ChFi3d_CircularSpine(double&      WFirst,
                                         double&      WLast,
                                         const gp_Pnt&       Pdeb,
                                         const gp_Vec&       Vdeb,
                                         const gp_Pnt&       Pfin,
                                         const gp_Vec&       Vfin,
                                         const double rad);

occ::handle<Geom_BezierCurve> ChFi3d_Spine(const gp_Pnt&       pd,
                                      gp_Vec&             vd,
                                      const gp_Pnt&       pf,
                                      gp_Vec&             vf,
                                      const double R);

occ::handle<GeomFill_Boundary> ChFi3d_mkbound(const occ::handle<Adaptor3d_Surface>& Fac,
                                         occ::handle<Geom2d_Curve>&            curv,
                                         const int           sens1,
                                         const gp_Pnt2d&                  pfac1,
                                         const gp_Vec2d&                  vfac1,
                                         const int           sens2,
                                         const gp_Pnt2d&                  pfac2,
                                         const gp_Vec2d&                  vfac2,
                                         const double              t3d,
                                         const double              ta);

occ::handle<GeomFill_Boundary> ChFi3d_mkbound(const occ::handle<Adaptor3d_Surface>& Surf,
                                         occ::handle<Geom2d_Curve>&            curv,
                                         const int           sens1,
                                         const gp_Pnt2d&                  p1,
                                         gp_Vec&                          v1,
                                         const int           sens2,
                                         const gp_Pnt2d&                  p2,
                                         gp_Vec&                          v2,
                                         const double              t3d,
                                         const double              ta);

occ::handle<GeomFill_Boundary> ChFi3d_mkbound(const occ::handle<Geom_Surface>& s,
                                         const gp_Pnt2d&             p1,
                                         const gp_Pnt2d&             p2,
                                         const double         t3d,
                                         const double         ta,
                                         const bool isfreeboundary = false);

occ::handle<GeomFill_Boundary> ChFi3d_mkbound(const occ::handle<Adaptor3d_Surface>& HS,
                                         const gp_Pnt2d&                  p1,
                                         const gp_Pnt2d&                  p2,
                                         const double              t3d,
                                         const double              ta,
                                         const bool isfreeboundary = false);

occ::handle<GeomFill_Boundary> ChFi3d_mkbound(const occ::handle<Adaptor3d_Surface>& HS,
                                         const occ::handle<Geom2d_Curve>&      curv,
                                         const double              t3d,
                                         const double              ta,
                                         const bool isfreeboundary = false);

occ::handle<GeomFill_Boundary> ChFi3d_mkbound(const occ::handle<Adaptor3d_Surface>& Fac,
                                         occ::handle<Geom2d_Curve>&            curv,
                                         const gp_Pnt2d&                  p1,
                                         const gp_Pnt2d&                  p2,
                                         const double              t3d,
                                         const double              ta,
                                         const bool isfreeboundary = false);

void ChFi3d_Coefficient(const gp_Vec&  V3d,
                        const gp_Vec&  D1u,
                        const gp_Vec&  D1v,
                        double& DU,
                        double& DV);

occ::handle<Geom2d_Curve> ChFi3d_BuildPCurve(const gp_Pnt2d&        p1,
                                        gp_Dir2d&              d1,
                                        const gp_Pnt2d&        p2,
                                        gp_Dir2d&              d2,
                                        const bool redresse = true);

occ::handle<Geom2d_Curve> ChFi3d_BuildPCurve(const occ::handle<Adaptor3d_Surface>& Surf,
                                        const gp_Pnt2d&                  p1,
                                        const gp_Vec&                    v1,
                                        const gp_Pnt2d&                  p2,
                                        const gp_Vec&                    v2,
                                        const bool           redresse = false);

occ::handle<Geom2d_Curve> ChFi3d_BuildPCurve(const occ::handle<Adaptor3d_Surface>& Surf,
                                        const gp_Pnt2d&                  p1,
                                        const gp_Vec2d&                  v1,
                                        const gp_Pnt2d&                  p2,
                                        const gp_Vec2d&                  v2,
                                        const bool           redresse = false);

bool ChFi3d_CheckSameParameter(const occ::handle<Adaptor3d_Curve>&   C3d,
                                           occ::handle<Geom2d_Curve>&            Pcurv,
                                           const occ::handle<Adaptor3d_Surface>& S,
                                           const double              tol3d,
                                           double&                   tolreached);

bool ChFi3d_SameParameter(const occ::handle<Adaptor3d_Curve>&   C3d,
                                      occ::handle<Geom2d_Curve>&            Pcurv,
                                      const occ::handle<Adaptor3d_Surface>& S,
                                      const double              tol3d,
                                      double&                   tolreached);

bool ChFi3d_SameParameter(const occ::handle<Geom_Curve>&   C3d,
                                      occ::handle<Geom2d_Curve>&       Pcurv,
                                      const occ::handle<Geom_Surface>& S,
                                      const double         Pardeb,
                                      const double         Parfin,
                                      const double         tol3d,
                                      double&              tolreached);

void ChFi3d_ComputePCurv(const occ::handle<Geom_Curve>&   C3d,
                         const gp_Pnt2d&             UV1,
                         const gp_Pnt2d&             UV2,
                         occ::handle<Geom2d_Curve>&       Pcurv,
                         const occ::handle<Geom_Surface>& S,
                         const double         Pardeb,
                         const double         Parfin,
                         const double         tol3d,
                         double&              tolreached,
                         const bool      reverse = false);

void ChFi3d_ComputePCurv(const occ::handle<Adaptor3d_Curve>&   C3d,
                         const gp_Pnt2d&                  UV1,
                         const gp_Pnt2d&                  UV2,
                         occ::handle<Geom2d_Curve>&            Pcurv,
                         const occ::handle<Adaptor3d_Surface>& S,
                         const double              Pardeb,
                         const double              Parfin,
                         const double              tol3d,
                         double&                   tolreached,
                         const bool           reverse = false);

void ChFi3d_ComputePCurv(const gp_Pnt2d&        UV1,
                         const gp_Pnt2d&        UV2,
                         occ::handle<Geom2d_Curve>&  Pcurv,
                         const double    Pardeb,
                         const double    Parfin,
                         const bool reverse = false);

bool ChFi3d_IntTraces(const occ::handle<ChFiDS_SurfData>& fd1,
                                  const double            pref1,
                                  double&                 p1,
                                  const int         jf1,
                                  const int         sens1,
                                  const occ::handle<ChFiDS_SurfData>& fd2,
                                  const double            pref2,
                                  double&                 p2,
                                  const int         jf2,
                                  const int         sens2,
                                  const gp_Pnt2d&                RefP2d,
                                  const bool         Check2dDistance = false,
                                  const bool         enlarge         = false);

bool ChFi3d_IsInFront(TopOpeBRepDS_DataStructure&  DStr,
                                  const occ::handle<ChFiDS_Stripe>& cd1,
                                  const occ::handle<ChFiDS_Stripe>& cd2,
                                  const int       i1,
                                  const int       i2,
                                  const int       sens1,
                                  const int       sens2,
                                  double&               p1,
                                  double&               p2,
                                  TopoDS_Face&                 face,
                                  bool&            sameside,
                                  int&            jf1,
                                  int&            jf2,
                                  bool&            visavis,
                                  const TopoDS_Vertex&         Vtx,
                                  const bool       Check2dDistance = false,
                                  const bool       enlarge         = false);

void ChFi3d_ProjectPCurv(const occ::handle<Adaptor3d_Curve>&   HCg,
                         const occ::handle<Adaptor3d_Surface>& HSg,
                         occ::handle<Geom2d_Curve>&            Pcurv,
                         const double              tol3d,
                         double&                   tolreached);

void ChFi3d_ReparamPcurv(const double   Uf,
                         const double   Ul,
                         occ::handle<Geom2d_Curve>& Pcurv);

void ChFi3d_ComputeArete(const ChFiDS_CommonPoint&   P1,
                         const gp_Pnt2d&             UV1,
                         const ChFiDS_CommonPoint&   P2,
                         const gp_Pnt2d&             UV2,
                         const occ::handle<Geom_Surface>& Surf,
                         occ::handle<Geom_Curve>&         C3d,
                         occ::handle<Geom2d_Curve>&       Pcurv,
                         double&              Pardeb,
                         double&              Parfin,
                         const double         tol3d,
                         const double         tol2d,
                         double&              tolreached,
                         const int      IFlag);

occ::handle<TopOpeBRepDS_SurfaceCurveInterference> ChFi3d_FilCurveInDS(const int      Icurv,
                                                                  const int      Isurf,
                                                                  const occ::handle<Geom2d_Curve>& Pcurv,
                                                                  const TopAbs_Orientation    Et);

TopAbs_Orientation ChFi3d_TrsfTrans(const IntSurf_TypeTrans T1);

Standard_EXPORT void ChFi3d_FilCommonPoint(const BRepBlend_Extremity& SP,
                                           const IntSurf_TypeTrans    TransLine,
                                           const bool     Start,
                                           ChFiDS_CommonPoint&        CP,
                                           const double        Tol);

int ChFi3d_SolidIndex(const occ::handle<ChFiDS_Spine>& sp,
                                   TopOpeBRepDS_DataStructure& DStr,
                                   ChFiDS_Map&                 MapESo,
                                   ChFiDS_Map&                 MapESh);

int ChFi3d_IndexPointInDS(const ChFiDS_CommonPoint&   P1,
                                       TopOpeBRepDS_DataStructure& DStr);

occ::handle<TopOpeBRepDS_CurvePointInterference> ChFi3d_FilPointInDS(
  const TopAbs_Orientation Et,
  const int   Ic,
  const int   Ip,
  const double      Par,
  const bool   IsVertex = false);

occ::handle<TopOpeBRepDS_CurvePointInterference> ChFi3d_FilVertexInDS(const TopAbs_Orientation Et,
                                                                 const int   Ic,
                                                                 const int   Ip,
                                                                 const double      Par);

void ChFi3d_FilDS(const int       SolidIndex,
                  const occ::handle<ChFiDS_Stripe>& CorDat,
                  TopOpeBRepDS_DataStructure&  DStr,
                  NCollection_List<ChFiDS_Regul>&         reglist,
                  const double          tol3d,
                  const double          tol2d);

void ChFi3d_StripeEdgeInter(const occ::handle<ChFiDS_Stripe>& theStripe1,
                            const occ::handle<ChFiDS_Stripe>& theStripe2,
                            TopOpeBRepDS_DataStructure&  DStr,
                            const double          tol2d);

int ChFi3d_IndexOfSurfData(const TopoDS_Vertex&         V1,
                                        const occ::handle<ChFiDS_Stripe>& CD,
                                        int&            sens);

TopoDS_Edge ChFi3d_EdgeFromV1(const TopoDS_Vertex&         V1,
                              const occ::handle<ChFiDS_Stripe>& CD,
                              int&            sens);

double ChFi3d_ConvTol2dToTol3d(const occ::handle<Adaptor3d_Surface>& S,
                                      const double              tol2d);

bool ChFi3d_ComputeCurves(const occ::handle<Adaptor3d_Surface>& S1,
                                      const occ::handle<Adaptor3d_Surface>& S2,
                                      const NCollection_Array1<double>&      Pardeb,
                                      const NCollection_Array1<double>&      Parfin,
                                      occ::handle<Geom_Curve>&              C3d,
                                      occ::handle<Geom2d_Curve>&            Pc1,
                                      occ::handle<Geom2d_Curve>&            Pc2,
                                      const double              tol3d,
                                      const double              tol2d,
                                      double&                   tolreached,
                                      const bool           wholeCurv = true);

bool ChFi3d_IntCS(const occ::handle<Adaptor3d_Surface>& S,
                              const occ::handle<Adaptor3d_Curve>&   C,
                              gp_Pnt2d&                        p2dS,
                              double&                   wc);

void ChFi3d_ComputesIntPC(const ChFiDS_FaceInterference&     Fi1,
                          const ChFiDS_FaceInterference&     Fi2,
                          const occ::handle<GeomAdaptor_Surface>& HS1,
                          const occ::handle<GeomAdaptor_Surface>& HS2,
                          double&                     UInt1,
                          double&                     UInt2);

void ChFi3d_ComputesIntPC(const ChFiDS_FaceInterference&     Fi1,
                          const ChFiDS_FaceInterference&     Fi2,
                          const occ::handle<GeomAdaptor_Surface>& HS1,
                          const occ::handle<GeomAdaptor_Surface>& HS2,
                          double&                     UInt1,
                          double&                     UInt2,
                          gp_Pnt&                            P);

occ::handle<GeomAdaptor_Surface> ChFi3d_BoundSurf(TopOpeBRepDS_DataStructure&    DStr,
                                             const occ::handle<ChFiDS_SurfData>& Fd1,
                                             const int&        IFaCo1,
                                             const int&        IFaArc1);

int ChFi3d_SearchPivot(int*   s,
                                    double       u[3][3],
                                    const double t);

bool ChFi3d_SearchFD(TopOpeBRepDS_DataStructure&  DStr,
                                 const occ::handle<ChFiDS_Stripe>& cd1,
                                 const occ::handle<ChFiDS_Stripe>& cd2,
                                 const int       sens1,
                                 const int       sens2,
                                 int&            i1,
                                 int&            i2,
                                 double&               p1,
                                 double&               p2,
                                 const int       ind1,
                                 const int       ind2,
                                 TopoDS_Face&                 face,
                                 bool&            sameside,
                                 int&            jf1,
                                 int&            jf2);

void ChFi3d_Parameters(const occ::handle<Geom_Surface>& S,
                       const gp_Pnt&               p3d,
                       double&              u,
                       double&              v);

void ChFi3d_TrimCurve(const occ::handle<Geom_Curve>&  gc,
                      const gp_Pnt&              FirstP,
                      const gp_Pnt&              LastP,
                      occ::handle<Geom_TrimmedCurve>& gtc);

Standard_EXPORT void ChFi3d_PerformElSpine(occ::handle<ChFiDS_ElSpine>& HES,
                                           occ::handle<ChFiDS_Spine>&   Spine,
                                           const GeomAbs_Shape     continuity,
                                           const double     tol,
                                           const bool  IsOffset = false);

TopoDS_Face ChFi3d_EnlargeFace(const occ::handle<ChFiDS_Spine>&        Spine,
                               const occ::handle<BRepAdaptor_Surface>& HS,
                               const double                Tol);

void ChFi3d_cherche_face1(const NCollection_List<TopoDS_Shape>& map, const TopoDS_Face& F1, TopoDS_Face& F);

void ChFi3d_cherche_element(const TopoDS_Vertex& V,
                            const TopoDS_Edge&   E1,
                            const TopoDS_Face&   F1,
                            TopoDS_Edge&         E,
                            TopoDS_Vertex&       Vtx);

double ChFi3d_EvalTolReached(const occ::handle<Adaptor3d_Surface>& S1,
                                    const occ::handle<Geom2d_Curve>&      pc1,
                                    const occ::handle<Adaptor3d_Surface>& S2,
                                    const occ::handle<Geom2d_Curve>&      pc2,
                                    const occ::handle<Geom_Curve>&        C);

void ChFi3d_cherche_edge(const TopoDS_Vertex&          V,
                         const NCollection_Array1<TopoDS_Shape>& E1,
                         const TopoDS_Face&            F1,
                         TopoDS_Edge&                  E,
                         TopoDS_Vertex&                Vtx);

int ChFi3d_nbface(const NCollection_List<TopoDS_Shape>& mapVF);

void ChFi3d_edge_common_faces(const NCollection_List<TopoDS_Shape>& mapEF, TopoDS_Face& F1, TopoDS_Face& F2);

double ChFi3d_AngleEdge(const TopoDS_Vertex& Vtx,
                               const TopoDS_Edge&   E1,
                               const TopoDS_Edge&   E2);

void ChFi3d_ChercheBordsLibres(const ChFiDS_Map&    myVEMap,
                               const TopoDS_Vertex& V1,
                               bool&    bordlibre,
                               TopoDS_Edge&         edgelibre1,
                               TopoDS_Edge&         edgelibre2);

int ChFi3d_NbNotDegeneratedEdges(const TopoDS_Vertex& Vtx, const ChFiDS_Map& VEMap);
int ChFi3d_NumberOfEdges(const TopoDS_Vertex& Vtx, const ChFiDS_Map& VEMap);

int ChFi3d_NumberOfSharpEdges(const TopoDS_Vertex& Vtx,
                                           const ChFiDS_Map&    VEMap,
                                           const ChFiDS_Map&    EFmap);

void ChFi3d_cherche_vertex(const TopoDS_Edge& E1,
                           const TopoDS_Edge& E2,
                           TopoDS_Vertex&     vertex,
                           bool&  trouve);

void ChFi3d_Couture(const TopoDS_Face& F, bool& couture, TopoDS_Edge& edgecouture);

void ChFi3d_CoutureOnVertex(const TopoDS_Face&   F,
                            const TopoDS_Vertex& V,
                            bool&    couture,
                            TopoDS_Edge&         edgecouture);

bool ChFi3d_IsPseudoSeam(const TopoDS_Edge& E, const TopoDS_Face& F);

occ::handle<Geom_BSplineCurve> ChFi3d_ApproxByC2(const occ::handle<Geom_Curve>& C);

bool ChFi3d_IsSmooth(const occ::handle<Geom_Curve>& C);

#endif
