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
#include <ChFiDS_Regularities.hxx>
#include <ChFiDS_FaceInterference.hxx>
#include <ChFiDS_Map.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopTools_ListOfShape.hxx>
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
#include <TColStd_Array1OfReal.hxx>
#include <TopTools_Array1OfShape.hxx>
#ifdef OCCT_DEBUG
#include <OSD_Chronometer.hxx>
extern OSD_Chronometer simul,elspine,chemine;
#endif

Standard_Real ChFi3d_InPeriod(const Standard_Real U, 
			      const Standard_Real UFirst, 
			      const Standard_Real ULast,
			      const Standard_Real Eps);

void ChFi3d_Boite(const gp_Pnt2d& p1,const gp_Pnt2d& p2,
		  Standard_Real& mu,Standard_Real& Mu,
		  Standard_Real& mv,Standard_Real& Mv);

void ChFi3d_Boite(const gp_Pnt2d& p1,const gp_Pnt2d& p2,
		  const gp_Pnt2d& p3,const gp_Pnt2d& p4,
		  Standard_Real& Du,Standard_Real& Dv,
		  Standard_Real& mu,Standard_Real& Mu,
		  Standard_Real& mv,Standard_Real& Mv);

void ChFi3d_SetPointTolerance(TopOpeBRepDS_DataStructure& DStr,
			      const Bnd_Box&              box,
			      const Standard_Integer      IP);

void ChFi3d_EnlargeBox(const Handle(Geom_Curve)& C,
		       const Standard_Real       wd,
		       const Standard_Real       wf,
		       Bnd_Box&                  box1,
		       Bnd_Box&                  box2);

void ChFi3d_EnlargeBox(const Handle(Adaptor3d_Surface)& S,
		       const Handle(Geom2d_Curve)&     PC,
		       const Standard_Real             wd,
		       const Standard_Real             wf,
		       Bnd_Box&                        box1,
		       Bnd_Box&                        box2);

void ChFi3d_EnlargeBox(const TopoDS_Edge&           E,
		       const TopTools_ListOfShape&  LF,
		       const Standard_Real          w,
		       Bnd_Box&                     box);

void ChFi3d_EnlargeBox(TopOpeBRepDS_DataStructure&    DStr,
		       const Handle(ChFiDS_Stripe)&   st, 
		       const Handle(ChFiDS_SurfData)& sd,
		       Bnd_Box&                       b1,
		       Bnd_Box&                       b2,
		       const Standard_Boolean         isfirst);

GeomAbs_Shape ChFi3d_evalconti(const TopoDS_Edge& E,
			       const TopoDS_Face& F1,
			       const TopoDS_Face& F2);

void ChFi3d_conexfaces(const TopoDS_Edge& E,
		       TopoDS_Face&       F1,
		       TopoDS_Face&       F2,
		       const ChFiDS_Map&  EFMap);

ChFiDS_State ChFi3d_EdgeState(TopoDS_Edge* E,
			      const ChFiDS_Map&  EFMap);

Standard_Boolean ChFi3d_KParticular
(const Handle(ChFiDS_Spine)& Spine,
 const Standard_Integer      IE,
 const BRepAdaptor_Surface&  S1,
 const BRepAdaptor_Surface&  S2);
 
void ChFi3d_BoundFac(BRepAdaptor_Surface& S,
		     const Standard_Real  umin,
		     const Standard_Real  umax,
		     const Standard_Real  vmin,
		     const Standard_Real  vmax,
		     const Standard_Boolean checknaturalbounds = Standard_True);
		      
void ChFi3d_BoundSrf(GeomAdaptor_Surface& S,
		     const Standard_Real  umin,
		     const Standard_Real  umax,
		     const Standard_Real  vmin,
		     const Standard_Real  vmax,
		     const Standard_Boolean checknaturalbounds = Standard_True);
		      
Standard_Boolean  ChFi3d_InterPlaneEdge (const Handle(Adaptor3d_Surface)& Plan,
					 const Handle(Adaptor3d_Curve)&   C,
					 Standard_Real&            W,
					 const Standard_Boolean    Sens,
					 const Standard_Real tolc);
					 
void ChFi3d_ExtrSpineCarac(const TopOpeBRepDS_DataStructure& DStr,
			   const Handle(ChFiDS_Stripe)&      cd,
			   const Standard_Integer            i,
			   const Standard_Real               p,
			   const Standard_Integer            jf,
			   const Standard_Integer            sens,
			   gp_Pnt&                           P,
			   gp_Vec&                           V,
			   Standard_Real&                    R);
			   
Handle(Geom_Circle) ChFi3d_CircularSpine(Standard_Real&      WFirst,
                                         Standard_Real&      WLast,
                                         const gp_Pnt&       Pdeb,
                                         const gp_Vec&       Vdeb,
                                         const gp_Pnt&       Pfin,
                                         const gp_Vec&       Vfin,
                                         const Standard_Real rad);

Handle(Geom_BezierCurve) ChFi3d_Spine(const gp_Pnt&       pd,
				      gp_Vec&             vd,
				      const gp_Pnt&       pf,
				      gp_Vec&             vf,
				      const Standard_Real R);

Handle(GeomFill_Boundary) ChFi3d_mkbound
(const Handle(Adaptor3d_Surface)& Fac,
 Handle(Geom2d_Curve)& curv, 
 const Standard_Integer sens1,
 const gp_Pnt2d& pfac1,
 const gp_Vec2d& vfac1,
 const Standard_Integer sens2,
 const gp_Pnt2d& pfac2,
 const gp_Vec2d& vfac2,
 const Standard_Real t3d,
 const Standard_Real ta);

Handle(GeomFill_Boundary) ChFi3d_mkbound
(const Handle(Adaptor3d_Surface)& Surf,
 Handle(Geom2d_Curve)& curv,
 const Standard_Integer sens1,
 const gp_Pnt2d& p1,
 gp_Vec&   v1,
 const Standard_Integer sens2,
 const gp_Pnt2d& p2,
 gp_Vec& v2,
 const Standard_Real t3d,
 const Standard_Real ta);

Handle(GeomFill_Boundary) ChFi3d_mkbound
(const Handle(Geom_Surface)& s,
 const gp_Pnt2d& p1,
 const gp_Pnt2d& p2,
 const Standard_Real t3d,
 const Standard_Real ta,
 const Standard_Boolean isfreeboundary = Standard_False);

Handle(GeomFill_Boundary) ChFi3d_mkbound
(const Handle(Adaptor3d_Surface)& HS,
 const gp_Pnt2d& p1,
 const gp_Pnt2d& p2,
 const Standard_Real t3d,
 const Standard_Real ta,
 const Standard_Boolean isfreeboundary = Standard_False);

Handle(GeomFill_Boundary) ChFi3d_mkbound
(const Handle(Adaptor3d_Surface)& HS,
 const Handle(Geom2d_Curve)& curv,
 const Standard_Real t3d,
 const Standard_Real ta,
 const Standard_Boolean isfreeboundary = Standard_False);

Handle(GeomFill_Boundary) ChFi3d_mkbound
(const Handle(Adaptor3d_Surface)& Fac,
 Handle(Geom2d_Curve)& curv, 
 const gp_Pnt2d& p1,
 const gp_Pnt2d& p2,
 const Standard_Real t3d,
 const Standard_Real ta,
 const Standard_Boolean isfreeboundary = Standard_False);

void ChFi3d_Coefficient(const gp_Vec& V3d,
			const gp_Vec& D1u,
			const gp_Vec& D1v,
			Standard_Real& DU,
			Standard_Real& DV); 

Handle(Geom2d_Curve) ChFi3d_BuildPCurve
(const gp_Pnt2d&        p1,
 gp_Dir2d&              d1,
 const gp_Pnt2d&        p2,
 gp_Dir2d&              d2,
 const Standard_Boolean redresse = Standard_True);

Handle(Geom2d_Curve) ChFi3d_BuildPCurve
(const Handle(Adaptor3d_Surface)& Surf,
 const gp_Pnt2d&                 p1,
 const gp_Vec&                   v1,
 const gp_Pnt2d&                 p2,
 const gp_Vec&                   v2,
 const Standard_Boolean redresse = Standard_False);

Handle(Geom2d_Curve) ChFi3d_BuildPCurve
(const Handle(Adaptor3d_Surface)& Surf,
 const gp_Pnt2d&                 p1,
 const gp_Vec2d&                 v1,
 const gp_Pnt2d&                 p2,
 const gp_Vec2d&                 v2,
 const Standard_Boolean redresse = Standard_False);

Standard_Boolean ChFi3d_CheckSameParameter 
(const Handle(Adaptor3d_Curve)&   C3d,
 Handle(Geom2d_Curve)&           Pcurv,
 const Handle(Adaptor3d_Surface)& S,
 const Standard_Real             tol3d,
 Standard_Real&                  tolreached);

Standard_Boolean ChFi3d_SameParameter(const Handle(Adaptor3d_Curve)&   C3d,
				      Handle(Geom2d_Curve)&           Pcurv,
				      const Handle(Adaptor3d_Surface)& S,
				      const Standard_Real             tol3d,
				      Standard_Real&                  tolreached);

Standard_Boolean ChFi3d_SameParameter(const Handle(Geom_Curve)&   C3d,
				      Handle(Geom2d_Curve)&       Pcurv,
				      const Handle(Geom_Surface)& S,
				      const Standard_Real         Pardeb,
				      const Standard_Real         Parfin,
				      const Standard_Real         tol3d,
				      Standard_Real&              tolreached);

void ChFi3d_ComputePCurv(const Handle(Geom_Curve)&   C3d,
			 const gp_Pnt2d&             UV1,
			 const gp_Pnt2d&             UV2,
			 Handle(Geom2d_Curve)&       Pcurv,
			 const Handle(Geom_Surface)& S,
			 const Standard_Real         Pardeb,
			 const Standard_Real         Parfin,
			 const Standard_Real         tol3d,
			 Standard_Real&              tolreached,
			 const Standard_Boolean      reverse = Standard_False);

void ChFi3d_ComputePCurv(const Handle(Adaptor3d_Curve)&   C3d,
			 const gp_Pnt2d&                 UV1,
			 const gp_Pnt2d&                 UV2,
			 Handle(Geom2d_Curve)&           Pcurv,
			 const Handle(Adaptor3d_Surface)& S,
			 const Standard_Real             Pardeb,
			 const Standard_Real             Parfin,
			 const Standard_Real             tol3d,
			 Standard_Real&                  tolreached,
			 const Standard_Boolean          reverse = Standard_False);

void ChFi3d_ComputePCurv(const gp_Pnt2d& UV1,
			 const gp_Pnt2d& UV2,
			 Handle(Geom2d_Curve)& Pcurv,
			 const Standard_Real Pardeb,
			 const Standard_Real Parfin,
			 const Standard_Boolean reverse = Standard_False);

Standard_Boolean ChFi3d_IntTraces(const Handle(ChFiDS_SurfData)& fd1,
				  const Standard_Real            pref1,
				  Standard_Real&                 p1,
				  const Standard_Integer         jf1,
				  const Standard_Integer         sens1,
				  const Handle(ChFiDS_SurfData)& fd2,
				  const Standard_Real            pref2,
				  Standard_Real&                 p2,
				  const Standard_Integer         jf2,
				  const Standard_Integer         sens2,
				  const gp_Pnt2d&                RefP2d,
				  const Standard_Boolean         Check2dDistance = Standard_False,
				  const Standard_Boolean         enlarge = Standard_False);

Standard_Boolean ChFi3d_IsInFront(TopOpeBRepDS_DataStructure& DStr,
				  const Handle(ChFiDS_Stripe)& cd1, 
				  const Handle(ChFiDS_Stripe)& cd2,
				  const Standard_Integer i1,
				  const Standard_Integer i2,
				  const Standard_Integer sens1,
				  const Standard_Integer sens2,
				  Standard_Real& p1,
				  Standard_Real& p2,
				  TopoDS_Face& face,
				  Standard_Boolean& sameside,
				  Standard_Integer& jf1,
				  Standard_Integer& jf2,
				  Standard_Boolean& visavis,
				  const TopoDS_Vertex& Vtx,
				  const Standard_Boolean Check2dDistance = Standard_False,
				  const Standard_Boolean enlarge = Standard_False);

void ChFi3d_ProjectPCurv(const Handle(Adaptor3d_Curve)&   HCg, 
			 const Handle(Adaptor3d_Surface)& HSg, 
			 Handle(Geom2d_Curve)&           Pcurv,
			 const Standard_Real             tol3d,
			 Standard_Real&                  tolreached) ;

void ChFi3d_ReparamPcurv(const Standard_Real   Uf, 
			 const Standard_Real   Ul,
			 Handle(Geom2d_Curve)& Pcurv) ;

void  ChFi3d_ComputeArete(const ChFiDS_CommonPoint&   P1,
			  const gp_Pnt2d&             UV1,
			  const ChFiDS_CommonPoint&   P2,
			  const gp_Pnt2d&             UV2,
			  const Handle(Geom_Surface)& Surf,
			  Handle(Geom_Curve)&         C3d,
			  Handle(Geom2d_Curve)&       Pcurv,
			  Standard_Real&              Pardeb,
			  Standard_Real&              Parfin,
			  const Standard_Real         tol3d,
			  const Standard_Real         tol2d,
			  Standard_Real&              tolreached,
			  const Standard_Integer      IFlag);
			  
Handle(TopOpeBRepDS_SurfaceCurveInterference)  
     ChFi3d_FilCurveInDS(const Standard_Integer Icurv,
			 const Standard_Integer Isurf,
			 const Handle(Geom2d_Curve)& Pcurv,
			 const TopAbs_Orientation Et);

TopAbs_Orientation ChFi3d_TrsfTrans(const IntSurf_TypeTrans T1);

Standard_EXPORT void ChFi3d_FilCommonPoint(const BRepBlend_Extremity& SP,
					   const IntSurf_TypeTrans TransLine,
					   const Standard_Boolean Start,
					   ChFiDS_CommonPoint& CP,
					   const Standard_Real Tol);

			 
Standard_Integer ChFi3d_SolidIndex(const Handle(ChFiDS_Spine)&  sp,
				   TopOpeBRepDS_DataStructure&  DStr,
				   ChFiDS_Map&                  MapESo,
				   ChFiDS_Map&                  MapESh);

Standard_Integer  ChFi3d_IndexPointInDS(const ChFiDS_CommonPoint& P1,
					TopOpeBRepDS_DataStructure& DStr);

Handle(TopOpeBRepDS_CurvePointInterference) ChFi3d_FilPointInDS
(const TopAbs_Orientation Et,
 const Standard_Integer Ic,
 const Standard_Integer Ip,
 const Standard_Real Par,
 const Standard_Boolean IsVertex = Standard_False);

Handle(TopOpeBRepDS_CurvePointInterference) ChFi3d_FilVertexInDS
(const TopAbs_Orientation Et,
 const Standard_Integer Ic,
 const Standard_Integer Ip,
 const Standard_Real Par);

void  ChFi3d_FilDS(const Standard_Integer       SolidIndex,
		   const Handle(ChFiDS_Stripe)& CorDat,
		   TopOpeBRepDS_DataStructure&  DStr,
		   ChFiDS_Regularities&         reglist,
		   const Standard_Real          tol3d,
		   const Standard_Real          tol2d);
		   

void ChFi3d_StripeEdgeInter (const Handle(ChFiDS_Stripe)& theStripe1,
			     const Handle(ChFiDS_Stripe)& theStripe2,
			     TopOpeBRepDS_DataStructure&  DStr,
			     const Standard_Real          tol2d);

Standard_Integer ChFi3d_IndexOfSurfData(const TopoDS_Vertex& V1,
					const Handle(ChFiDS_Stripe)& CD,
					Standard_Integer& sens);

TopoDS_Edge ChFi3d_EdgeFromV1(const TopoDS_Vertex& V1,
			      const Handle(ChFiDS_Stripe)& CD,
			      Standard_Integer& sens);

Standard_Real ChFi3d_ConvTol2dToTol3d(const Handle(Adaptor3d_Surface)& S,
				      const Standard_Real             tol2d);

Standard_Boolean  ChFi3d_ComputeCurves(const Handle(Adaptor3d_Surface)&   S1,
				       const Handle(Adaptor3d_Surface)&   S2,
				       const TColStd_Array1OfReal& Pardeb,
				       const TColStd_Array1OfReal& Parfin,
				       Handle(Geom_Curve)&         C3d,
				       Handle(Geom2d_Curve)&       Pc1,
				       Handle(Geom2d_Curve)&       Pc2,
				       const Standard_Real         tol3d,
				       const Standard_Real         tol2d,
				       Standard_Real&              tolreached,
				       const Standard_Boolean      wholeCurv
				        = Standard_True);

Standard_Boolean ChFi3d_IntCS(const Handle(Adaptor3d_Surface)& S,
			      const Handle(Adaptor3d_Curve)& C,
			      gp_Pnt2d& p2dS,
			      Standard_Real& wc);

void ChFi3d_ComputesIntPC (const ChFiDS_FaceInterference&      Fi1,
			   const ChFiDS_FaceInterference&      Fi2,
			   const Handle(GeomAdaptor_Surface)& HS1,
			   const Handle(GeomAdaptor_Surface)& HS2,
			   Standard_Real&                      UInt1, 
			   Standard_Real&                      UInt2);

void ChFi3d_ComputesIntPC (const ChFiDS_FaceInterference&      Fi1,
			   const ChFiDS_FaceInterference&      Fi2,
			   const Handle(GeomAdaptor_Surface)& HS1,
			   const Handle(GeomAdaptor_Surface)& HS2,
			   Standard_Real&                      UInt1, 
			   Standard_Real&                      UInt2,
			   gp_Pnt&                             P);

Handle(GeomAdaptor_Surface) ChFi3d_BoundSurf(TopOpeBRepDS_DataStructure& DStr,
					      const Handle(ChFiDS_SurfData)& Fd1,
					      const Standard_Integer&        IFaCo1,
					      const Standard_Integer&        IFaArc1);

Standard_Integer ChFi3d_SearchPivot(Standard_Integer* s,
				    Standard_Real u[3][3],
				    const Standard_Real t);

Standard_Boolean ChFi3d_SearchFD(TopOpeBRepDS_DataStructure& DStr,
				 const Handle(ChFiDS_Stripe)& cd1, 
				 const Handle(ChFiDS_Stripe)& cd2,
				 const Standard_Integer sens1,
				 const Standard_Integer sens2,
				 Standard_Integer& i1,
				 Standard_Integer& i2,
				 Standard_Real& p1,
				 Standard_Real& p2,
				 const Standard_Integer ind1,
				 const Standard_Integer ind2,
				 TopoDS_Face& face,
				 Standard_Boolean& sameside,
				 Standard_Integer& jf1,
				 Standard_Integer& jf2);


void ChFi3d_Parameters(const Handle(Geom_Surface)& S,
		       const gp_Pnt& p3d,
		       Standard_Real& u,
		       Standard_Real& v);

void ChFi3d_TrimCurve(const Handle(Geom_Curve)& gc,
		      const gp_Pnt& FirstP,
		      const gp_Pnt& LastP,
		      Handle(Geom_TrimmedCurve)& gtc);

Standard_EXPORT void ChFi3d_PerformElSpine(Handle(ChFiDS_ElSpine)& HES,
					   Handle(ChFiDS_Spine)&    Spine,
					   const GeomAbs_Shape      continuity,
					   const Standard_Real      tol,
                                           const Standard_Boolean   IsOffset = Standard_False);

TopoDS_Face ChFi3d_EnlargeFace(const Handle(ChFiDS_Spine)& Spine,
			       const Handle(BRepAdaptor_Surface)&  HS,
			       const Standard_Real         Tol );


void ChFi3d_cherche_face1 (const TopTools_ListOfShape & map,
                    const TopoDS_Face & F1,
                          TopoDS_Face &  F);  

void ChFi3d_cherche_element( const TopoDS_Vertex & V,
                      const TopoDS_Edge & E1,
                      const TopoDS_Face & F1,
                      TopoDS_Edge & E , 
                      TopoDS_Vertex  & Vtx );

Standard_Real ChFi3d_EvalTolReached(const Handle(Adaptor3d_Surface)& S1,
				    const Handle(Geom2d_Curve)&     pc1,
				    const Handle(Adaptor3d_Surface)& S2,
				    const Handle(Geom2d_Curve)&     pc2,
				    const Handle(Geom_Curve)&       C);

void ChFi3d_cherche_edge( const TopoDS_Vertex & V,
                      const  TopTools_Array1OfShape  & E1,
                      const TopoDS_Face & F1,
                      TopoDS_Edge & E , 
                      TopoDS_Vertex  & Vtx );

Standard_Integer  ChFi3d_nbface (const TopTools_ListOfShape & mapVF );

void ChFi3d_edge_common_faces (const TopTools_ListOfShape & mapEF,
                               TopoDS_Face & F1,
                               TopoDS_Face &  F2); 


Standard_Real ChFi3d_AngleEdge (const TopoDS_Vertex & Vtx,
                                  const TopoDS_Edge&  E1,
                                  const TopoDS_Edge &  E2);

void ChFi3d_ChercheBordsLibres(const  ChFiDS_Map & myVEMap,
                                const TopoDS_Vertex & V1,
                                Standard_Boolean & bordlibre,
                                TopoDS_Edge & edgelibre1,
                                TopoDS_Edge & edgelibre2);

Standard_Integer ChFi3d_NbNotDegeneratedEdges (const TopoDS_Vertex& Vtx,
				      const ChFiDS_Map& VEMap);
Standard_Integer ChFi3d_NumberOfEdges(const TopoDS_Vertex& Vtx,
				      const ChFiDS_Map& VEMap);

Standard_Integer ChFi3d_NumberOfSharpEdges(const TopoDS_Vertex& Vtx,
                                           const ChFiDS_Map& VEMap,
                                           const ChFiDS_Map& EFmap);

void ChFi3d_cherche_vertex (const TopoDS_Edge & E1,
			    const TopoDS_Edge & E2,
			    TopoDS_Vertex & vertex,
			    Standard_Boolean & trouve);

void ChFi3d_Couture( const TopoDS_Face & F,
                     Standard_Boolean & couture,
                     TopoDS_Edge & edgecouture);

void ChFi3d_CoutureOnVertex( const TopoDS_Face & F,
			     const TopoDS_Vertex & V,
			     Standard_Boolean & couture,
			     TopoDS_Edge & edgecouture);

Standard_Boolean ChFi3d_IsPseudoSeam( const TopoDS_Edge& E,
				      const TopoDS_Face& F );

Handle(Geom_BSplineCurve) ChFi3d_ApproxByC2( const Handle(Geom_Curve)& C );

Standard_Boolean ChFi3d_IsSmooth( const Handle(Geom_Curve)& C );

#endif
