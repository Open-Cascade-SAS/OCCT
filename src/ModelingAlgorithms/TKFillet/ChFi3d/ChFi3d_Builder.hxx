// Created on: 1993-11-09
// Created by: Laurent BOURESCHE
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

#ifndef _ChFi3d_Builder_HeaderFile
#define _ChFi3d_Builder_HeaderFile

#include <BRepAdaptor_Curve2d.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <GeomAbs_Shape.hxx>
#include <ChFiDS_ErrorStatus.hxx>
#include <ChFiDS_Map.hxx>
#include <ChFiDS_Regul.hxx>
#include <NCollection_List.hxx>
#include <ChFiDS_SurfData.hxx>
#include <NCollection_Sequence.hxx>
#include <ChFiDS_StripeMap.hxx>
#include <ChFiDS_ElSpine.hxx>
#include <math_Vector.hxx>
#include <TopoDS_Shape.hxx>
#include <Standard_Integer.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopAbs_State.hxx>

class TopOpeBRepDS_HDataStructure;
class TopOpeBRepBuild_HBuilder;
class TopoDS_Edge;
class ChFiDS_Spine;
class TopoDS_Vertex;
class Geom_Surface;
class ChFiDS_SurfData;
class Adaptor3d_TopolTool;
class BRepBlend_Line;
class Blend_Function;
class Blend_FuncInv;
class Blend_SurfRstFunction;
class Blend_SurfPointFuncInv;
class Blend_SurfCurvFuncInv;
class Blend_RstRstFunction;
class Blend_CurvPointFuncInv;
class ChFiDS_Stripe;
class BRepTopAdaptor_TopolTool;
class gp_Pnt2d;
class ChFiDS_CommonPoint;
class TopoDS_Face;
class AppBlend_Approx;
class Geom2d_Curve;

//! Root class for calculation of surfaces (fillets,
//! chamfers) destined to smooth edges of
//! a gap on a Shape and the reconstruction of the Shape.
class ChFi3d_Builder
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT virtual ~ChFi3d_Builder();

  Standard_EXPORT void SetParams(const double Tang,
                                 const double Tesp,
                                 const double T2d,
                                 const double TApp3d,
                                 const double TolApp2d,
                                 const double Fleche);

  Standard_EXPORT void SetContinuity(const GeomAbs_Shape InternalContinuity,
                                     const double        AngularTolerance);

  //! extracts from the list the contour containing edge E.
  Standard_EXPORT void Remove(const TopoDS_Edge& E);

  //! gives the number of the contour containing E or 0
  //! if E does not belong to any contour.
  Standard_EXPORT int Contains(const TopoDS_Edge& E) const;

  //! gives the number of the contour containing E or 0
  //! if E does not belong to any contour.
  //! Sets in IndexInSpine the index of E in the contour if it's found
  Standard_EXPORT int Contains(const TopoDS_Edge& E, int& IndexInSpine) const;

  //! gives the number of disjoint contours on which
  //! the fillets are calculated
  Standard_EXPORT int NbElements() const;

  //! gives the n'th set of edges (contour)
  //! if I >NbElements()
  Standard_EXPORT occ::handle<ChFiDS_Spine> Value(const int I) const;

  //! returns the length of the contour of index IC.
  Standard_EXPORT double Length(const int IC) const;

  //! returns the First vertex V of
  //! the contour of index IC.
  Standard_EXPORT TopoDS_Vertex FirstVertex(const int IC) const;

  //! returns the Last vertex V of
  //! the contour of index IC.
  Standard_EXPORT TopoDS_Vertex LastVertex(const int IC) const;

  //! returns the abscissa of the vertex V on
  //! the contour of index IC.
  Standard_EXPORT double Abscissa(const int IC, const TopoDS_Vertex& V) const;

  //! returns the relative abscissa([0.,1.]) of the
  //! vertex V on the contour of index IC.
  Standard_EXPORT double RelativeAbscissa(const int IC, const TopoDS_Vertex& V) const;

  //! returns true if the contour of index IC is closed
  //! an tangent.
  Standard_EXPORT bool ClosedAndTangent(const int IC) const;

  //! returns true if the contour of index IC is closed
  Standard_EXPORT bool Closed(const int IC) const;

  //! general calculation of geometry on all edges,
  //! topologic reconstruction.
  Standard_EXPORT void Compute();

  //! returns True if the computation is success
  Standard_EXPORT bool IsDone() const;

  //! if (Isdone()) makes the result.
  //! if (!Isdone())
  Standard_EXPORT TopoDS_Shape Shape() const;

  //! Advanced function for the history
  Standard_EXPORT const NCollection_List<TopoDS_Shape>& Generated(const TopoDS_Shape& EouV);

  //! Returns the number of contours on which the calculation
  //! has failed.
  Standard_EXPORT int NbFaultyContours() const;

  //! Returns the number of I'th contour on which the calculation
  //! has failed.
  Standard_EXPORT int FaultyContour(const int I) const;

  //! Returns the number of surfaces calculated on the contour IC.
  Standard_EXPORT int NbComputedSurfaces(const int IC) const;

  //! Returns the IS'th surface calculated on the contour IC.
  Standard_EXPORT occ::handle<Geom_Surface> ComputedSurface(const int IC, const int IS) const;

  //! Returns the number of vertices on which the calculation
  //! has failed.
  Standard_EXPORT int NbFaultyVertices() const;

  //! Returns the IV'th vertex on which the calculation has failed.
  Standard_EXPORT TopoDS_Vertex FaultyVertex(const int IV) const;

  //! returns True if a partial result has been calculated
  Standard_EXPORT bool HasResult() const;

  //! if (HasResult()) returns partial result
  //! if (!HasResult())
  Standard_EXPORT TopoDS_Shape BadShape() const;

  //! for the stripe IC ,indication on the cause
  //! of failure WalkingFailure,TwistedSurface,Error, Ok
  Standard_EXPORT ChFiDS_ErrorStatus StripeStatus(const int IC) const;

  //! Reset all results of compute and returns the algorithm
  //! in the state of the last acquisition to enable modification of contours or areas.
  Standard_EXPORT void Reset();

  //! Returns the Builder of topologic operations.
  Standard_EXPORT occ::handle<TopOpeBRepBuild_HBuilder> Builder() const;

  //! Method, implemented in the inheritants, calculates
  //! the elements of construction of the surface (fillet or
  //! chamfer).
  Standard_EXPORT bool SplitKPart(const occ::handle<ChFiDS_SurfData>&                 Data,
                                  NCollection_Sequence<occ::handle<ChFiDS_SurfData>>& SetData,
                                  const occ::handle<ChFiDS_Spine>&                    Spine,
                                  const int                                           Iedge,
                                  const occ::handle<Adaptor3d_Surface>&               S1,
                                  const occ::handle<Adaptor3d_TopolTool>&             I1,
                                  const occ::handle<Adaptor3d_Surface>&               S2,
                                  const occ::handle<Adaptor3d_TopolTool>&             I2,
                                  bool&                                               Intf,
                                  bool&                                               Intl);

  Standard_EXPORT bool PerformTwoCornerbyInter(const int Index);

protected:
  Standard_EXPORT ChFi3d_Builder(const TopoDS_Shape& S, const double Ta);

  Standard_EXPORT virtual void SimulKPart(const occ::handle<ChFiDS_SurfData>& SD) const = 0;

  Standard_EXPORT virtual bool SimulSurf(occ::handle<ChFiDS_SurfData>&           Data,
                                         const occ::handle<ChFiDS_ElSpine>&      Guide,
                                         const occ::handle<ChFiDS_Spine>&        Spine,
                                         const int                               Choix,
                                         const occ::handle<BRepAdaptor_Surface>& S1,
                                         const occ::handle<Adaptor3d_TopolTool>& I1,
                                         const occ::handle<BRepAdaptor_Surface>& S2,
                                         const occ::handle<Adaptor3d_TopolTool>& I2,
                                         const double                            TolGuide,
                                         double&                                 First,
                                         double&                                 Last,
                                         const bool                              Inside,
                                         const bool                              Appro,
                                         const bool                              Forward,
                                         const bool                              RecOnS1,
                                         const bool                              RecOnS2,
                                         const math_Vector&                      Soldep,
                                         int&                                    Intf,
                                         int&                                    Intl) = 0;

  Standard_EXPORT virtual void SimulSurf(occ::handle<ChFiDS_SurfData>&           Data,
                                         const occ::handle<ChFiDS_ElSpine>&      Guide,
                                         const occ::handle<ChFiDS_Spine>&        Spine,
                                         const int                               Choix,
                                         const occ::handle<BRepAdaptor_Surface>& S1,
                                         const occ::handle<Adaptor3d_TopolTool>& I1,
                                         const occ::handle<BRepAdaptor_Curve2d>& PC1,
                                         const occ::handle<BRepAdaptor_Surface>& Sref1,
                                         const occ::handle<BRepAdaptor_Curve2d>& PCref1,
                                         bool&                                   Decroch1,
                                         const occ::handle<BRepAdaptor_Surface>& S2,
                                         const occ::handle<Adaptor3d_TopolTool>& I2,
                                         const TopAbs_Orientation                Or2,
                                         const double                            Fleche,
                                         const double                            TolGuide,
                                         double&                                 First,
                                         double&                                 Last,
                                         const bool                              Inside,
                                         const bool                              Appro,
                                         const bool                              Forward,
                                         const bool                              RecP,
                                         const bool                              RecS,
                                         const bool                              RecRst,
                                         const math_Vector&                      Soldep);

  Standard_EXPORT virtual void SimulSurf(occ::handle<ChFiDS_SurfData>&           Data,
                                         const occ::handle<ChFiDS_ElSpine>&      Guide,
                                         const occ::handle<ChFiDS_Spine>&        Spine,
                                         const int                               Choix,
                                         const occ::handle<BRepAdaptor_Surface>& S1,
                                         const occ::handle<Adaptor3d_TopolTool>& I1,
                                         const TopAbs_Orientation                Or1,
                                         const occ::handle<BRepAdaptor_Surface>& S2,
                                         const occ::handle<Adaptor3d_TopolTool>& I2,
                                         const occ::handle<BRepAdaptor_Curve2d>& PC2,
                                         const occ::handle<BRepAdaptor_Surface>& Sref2,
                                         const occ::handle<BRepAdaptor_Curve2d>& PCref2,
                                         bool&                                   Decroch2,
                                         const double                            Fleche,
                                         const double                            TolGuide,
                                         double&                                 First,
                                         double&                                 Last,
                                         const bool                              Inside,
                                         const bool                              Appro,
                                         const bool                              Forward,
                                         const bool                              RecP,
                                         const bool                              RecS,
                                         const bool                              RecRst,
                                         const math_Vector&                      Soldep);

  Standard_EXPORT virtual void SimulSurf(occ::handle<ChFiDS_SurfData>&           Data,
                                         const occ::handle<ChFiDS_ElSpine>&      Guide,
                                         const occ::handle<ChFiDS_Spine>&        Spine,
                                         const int                               Choix,
                                         const occ::handle<BRepAdaptor_Surface>& S1,
                                         const occ::handle<Adaptor3d_TopolTool>& I1,
                                         const occ::handle<BRepAdaptor_Curve2d>& PC1,
                                         const occ::handle<BRepAdaptor_Surface>& Sref1,
                                         const occ::handle<BRepAdaptor_Curve2d>& PCref1,
                                         bool&                                   Decroch1,
                                         const TopAbs_Orientation                Or1,
                                         const occ::handle<BRepAdaptor_Surface>& S2,
                                         const occ::handle<Adaptor3d_TopolTool>& I2,
                                         const occ::handle<BRepAdaptor_Curve2d>& PC2,
                                         const occ::handle<BRepAdaptor_Surface>& Sref2,
                                         const occ::handle<BRepAdaptor_Curve2d>& PCref2,
                                         bool&                                   Decroch2,
                                         const TopAbs_Orientation                Or2,
                                         const double                            Fleche,
                                         const double                            TolGuide,
                                         double&                                 First,
                                         double&                                 Last,
                                         const bool                              Inside,
                                         const bool                              Appro,
                                         const bool                              Forward,
                                         const bool                              RecP1,
                                         const bool                              RecRst1,
                                         const bool                              RecP2,
                                         const bool                              RecRst2,
                                         const math_Vector&                      Soldep);

  Standard_EXPORT bool SimulData(occ::handle<ChFiDS_SurfData>&           Data,
                                 const occ::handle<ChFiDS_ElSpine>&      Guide,
                                 const occ::handle<ChFiDS_ElSpine>&      AdditionalGuide,
                                 occ::handle<BRepBlend_Line>&            Lin,
                                 const occ::handle<Adaptor3d_Surface>&   S1,
                                 const occ::handle<Adaptor3d_TopolTool>& I1,
                                 const occ::handle<Adaptor3d_Surface>&   S2,
                                 const occ::handle<Adaptor3d_TopolTool>& I2,
                                 Blend_Function&                         Func,
                                 Blend_FuncInv&                          FInv,
                                 const double                            PFirst,
                                 const double                            MaxStep,
                                 const double                            Fleche,
                                 const double                            TolGuide,
                                 double&                                 First,
                                 double&                                 Last,
                                 const bool                              Inside,
                                 const bool                              Appro,
                                 const bool                              Forward,
                                 const math_Vector&                      Soldep,
                                 const int                               NbSecMin,
                                 const bool                              RecOnS1 = false,
                                 const bool                              RecOnS2 = false);

  Standard_EXPORT bool SimulData(occ::handle<ChFiDS_SurfData>&           Data,
                                 const occ::handle<ChFiDS_ElSpine>&      HGuide,
                                 occ::handle<BRepBlend_Line>&            Lin,
                                 const occ::handle<Adaptor3d_Surface>&   S1,
                                 const occ::handle<Adaptor3d_TopolTool>& I1,
                                 const occ::handle<Adaptor3d_Surface>&   S2,
                                 const occ::handle<Adaptor2d_Curve2d>&   PC2,
                                 const occ::handle<Adaptor3d_TopolTool>& I2,
                                 bool&                                   Decroch,
                                 Blend_SurfRstFunction&                  Func,
                                 Blend_FuncInv&                          FInv,
                                 Blend_SurfPointFuncInv&                 FInvP,
                                 Blend_SurfCurvFuncInv&                  FInvC,
                                 const double                            PFirst,
                                 const double                            MaxStep,
                                 const double                            Fleche,
                                 const double                            TolGuide,
                                 double&                                 First,
                                 double&                                 Last,
                                 const math_Vector&                      Soldep,
                                 const int                               NbSecMin,
                                 const bool                              Inside,
                                 const bool                              Appro,
                                 const bool                              Forward,
                                 const bool                              RecP,
                                 const bool                              RecS,
                                 const bool                              RecRst);

  Standard_EXPORT bool SimulData(occ::handle<ChFiDS_SurfData>&           Data,
                                 const occ::handle<ChFiDS_ElSpine>&      HGuide,
                                 occ::handle<BRepBlend_Line>&            Lin,
                                 const occ::handle<Adaptor3d_Surface>&   S1,
                                 const occ::handle<Adaptor2d_Curve2d>&   PC1,
                                 const occ::handle<Adaptor3d_TopolTool>& I1,
                                 bool&                                   Decroch1,
                                 const occ::handle<Adaptor3d_Surface>&   S2,
                                 const occ::handle<Adaptor2d_Curve2d>&   PC2,
                                 const occ::handle<Adaptor3d_TopolTool>& I2,
                                 bool&                                   Decroch2,
                                 Blend_RstRstFunction&                   Func,
                                 Blend_SurfCurvFuncInv&                  FInv1,
                                 Blend_CurvPointFuncInv&                 FInvP1,
                                 Blend_SurfCurvFuncInv&                  FInv2,
                                 Blend_CurvPointFuncInv&                 FInvP2,
                                 const double                            PFirst,
                                 const double                            MaxStep,
                                 const double                            Fleche,
                                 const double                            TolGuide,
                                 double&                                 First,
                                 double&                                 Last,
                                 const math_Vector&                      Soldep,
                                 const int                               NbSecMin,
                                 const bool                              Inside,
                                 const bool                              Appro,
                                 const bool                              Forward,
                                 const bool                              RecP1,
                                 const bool                              RecRst1,
                                 const bool                              RecP2,
                                 const bool                              RecRst2);

  Standard_EXPORT virtual void SetRegul() = 0;

  Standard_EXPORT bool PerformElement(const occ::handle<ChFiDS_Spine>& CElement,
                                      const double                     Offset,
                                      const TopoDS_Face&               theFirstFace);

  Standard_EXPORT void PerformExtremity(const occ::handle<ChFiDS_Spine>& CElement);

  Standard_EXPORT void PerformSetOfSurf(occ::handle<ChFiDS_Stripe>& S, const bool Simul = false);

  Standard_EXPORT void PerformSetOfKPart(occ::handle<ChFiDS_Stripe>& S, const bool Simul = false);

  Standard_EXPORT void PerformSetOfKGen(occ::handle<ChFiDS_Stripe>& S, const bool Simul = false);

  Standard_EXPORT void Trunc(const occ::handle<ChFiDS_SurfData>&   SD,
                             const occ::handle<ChFiDS_Spine>&      Spine,
                             const occ::handle<Adaptor3d_Surface>& S1,
                             const occ::handle<Adaptor3d_Surface>& S2,
                             const int                             iedge,
                             const bool                            isfirst,
                             const int                             cntlFiOnS);

  Standard_EXPORT void CallPerformSurf(occ::handle<ChFiDS_Stripe>&                         Stripe,
                                       const bool                                          Simul,
                                       NCollection_Sequence<occ::handle<ChFiDS_SurfData>>& SeqSD,
                                       occ::handle<ChFiDS_SurfData>&                       SD,
                                       const occ::handle<ChFiDS_ElSpine>&                  Guide,
                                       const occ::handle<ChFiDS_Spine>&                    Spine,
                                       const occ::handle<BRepAdaptor_Surface>&             HS1,
                                       const occ::handle<BRepAdaptor_Surface>&             HS3,
                                       const gp_Pnt2d&                                     P1,
                                       const gp_Pnt2d&                                     P3,
                                       const occ::handle<Adaptor3d_TopolTool>&             I1,
                                       const occ::handle<BRepAdaptor_Surface>&             HS2,
                                       const occ::handle<BRepAdaptor_Surface>&             HS4,
                                       const gp_Pnt2d&                                     P2,
                                       const gp_Pnt2d&                                     P4,
                                       const occ::handle<Adaptor3d_TopolTool>&             I2,
                                       const double                                        MaxStep,
                                       const double                                        Fleche,
                                       const double                                        TolGuide,
                                       double&                                             First,
                                       double&                                             Last,
                                       const bool                                          Inside,
                                       const bool                                          Appro,
                                       const bool                                          Forward,
                                       const bool                                          RecOnS1,
                                       const bool                                          RecOnS2,
                                       math_Vector&                                        Soldep,
                                       int&                                                Intf,
                                       int&                                                Intl,
                                       occ::handle<BRepAdaptor_Surface>&                   Surf1,
                                       occ::handle<BRepAdaptor_Surface>&                   Surf2);

  //! Method, implemented in the inheritants, calculating
  //! elements of construction of the surface (fillet or
  //! chamfer).
  Standard_EXPORT virtual bool PerformSurf(NCollection_Sequence<occ::handle<ChFiDS_SurfData>>& Data,
                                           const occ::handle<ChFiDS_ElSpine>&      Guide,
                                           const occ::handle<ChFiDS_Spine>&        Spine,
                                           const int                               Choix,
                                           const occ::handle<BRepAdaptor_Surface>& S1,
                                           const occ::handle<Adaptor3d_TopolTool>& I1,
                                           const occ::handle<BRepAdaptor_Surface>& S2,
                                           const occ::handle<Adaptor3d_TopolTool>& I2,
                                           const double                            MaxStep,
                                           const double                            Fleche,
                                           const double                            TolGuide,
                                           double&                                 First,
                                           double&                                 Last,
                                           const bool                              Inside,
                                           const bool                              Appro,
                                           const bool                              Forward,
                                           const bool                              RecOnS1,
                                           const bool                              RecOnS2,
                                           const math_Vector&                      Soldep,
                                           int&                                    Intf,
                                           int&                                    Intl) = 0;

  //! Method, implemented in inheritants, calculates
  //! the elements of construction of the surface (fillet
  //! or chamfer) contact edge/face.
  Standard_EXPORT virtual void PerformSurf(NCollection_Sequence<occ::handle<ChFiDS_SurfData>>& Data,
                                           const occ::handle<ChFiDS_ElSpine>&      Guide,
                                           const occ::handle<ChFiDS_Spine>&        Spine,
                                           const int                               Choix,
                                           const occ::handle<BRepAdaptor_Surface>& S1,
                                           const occ::handle<Adaptor3d_TopolTool>& I1,
                                           const occ::handle<BRepAdaptor_Curve2d>& PC1,
                                           const occ::handle<BRepAdaptor_Surface>& Sref1,
                                           const occ::handle<BRepAdaptor_Curve2d>& PCref1,
                                           bool&                                   Decroch1,
                                           const occ::handle<BRepAdaptor_Surface>& S2,
                                           const occ::handle<Adaptor3d_TopolTool>& I2,
                                           const TopAbs_Orientation                Or2,
                                           const double                            MaxStep,
                                           const double                            Fleche,
                                           const double                            TolGuide,
                                           double&                                 First,
                                           double&                                 Last,
                                           const bool                              Inside,
                                           const bool                              Appro,
                                           const bool                              Forward,
                                           const bool                              RecP,
                                           const bool                              RecS,
                                           const bool                              RecRst,
                                           const math_Vector&                      Soldep);

  //! Method, implemented in inheritants, calculates
  //! the elements of construction of the surface (fillet
  //! or chamfer) contact edge/face.
  Standard_EXPORT virtual void PerformSurf(NCollection_Sequence<occ::handle<ChFiDS_SurfData>>& Data,
                                           const occ::handle<ChFiDS_ElSpine>&      Guide,
                                           const occ::handle<ChFiDS_Spine>&        Spine,
                                           const int                               Choix,
                                           const occ::handle<BRepAdaptor_Surface>& S1,
                                           const occ::handle<Adaptor3d_TopolTool>& I1,
                                           const TopAbs_Orientation                Or1,
                                           const occ::handle<BRepAdaptor_Surface>& S2,
                                           const occ::handle<Adaptor3d_TopolTool>& I2,
                                           const occ::handle<BRepAdaptor_Curve2d>& PC2,
                                           const occ::handle<BRepAdaptor_Surface>& Sref2,
                                           const occ::handle<BRepAdaptor_Curve2d>& PCref2,
                                           bool&                                   Decroch2,
                                           const double                            MaxStep,
                                           const double                            Fleche,
                                           const double                            TolGuide,
                                           double&                                 First,
                                           double&                                 Last,
                                           const bool                              Inside,
                                           const bool                              Appro,
                                           const bool                              Forward,
                                           const bool                              RecP,
                                           const bool                              RecS,
                                           const bool                              RecRst,
                                           const math_Vector&                      Soldep);

  //! Method, implemented in inheritants, calculates
  //! the elements of construction of the surface (fillet
  //! or chamfer) contact edge/edge.
  Standard_EXPORT virtual void PerformSurf(NCollection_Sequence<occ::handle<ChFiDS_SurfData>>& Data,
                                           const occ::handle<ChFiDS_ElSpine>&      Guide,
                                           const occ::handle<ChFiDS_Spine>&        Spine,
                                           const int                               Choix,
                                           const occ::handle<BRepAdaptor_Surface>& S1,
                                           const occ::handle<Adaptor3d_TopolTool>& I1,
                                           const occ::handle<BRepAdaptor_Curve2d>& PC1,
                                           const occ::handle<BRepAdaptor_Surface>& Sref1,
                                           const occ::handle<BRepAdaptor_Curve2d>& PCref1,
                                           bool&                                   Decroch1,
                                           const TopAbs_Orientation                Or1,
                                           const occ::handle<BRepAdaptor_Surface>& S2,
                                           const occ::handle<Adaptor3d_TopolTool>& I2,
                                           const occ::handle<BRepAdaptor_Curve2d>& PC2,
                                           const occ::handle<BRepAdaptor_Surface>& Sref2,
                                           const occ::handle<BRepAdaptor_Curve2d>& PCref2,
                                           bool&                                   Decroch2,
                                           const TopAbs_Orientation                Or2,
                                           const double                            MaxStep,
                                           const double                            Fleche,
                                           const double                            TolGuide,
                                           double&                                 First,
                                           double&                                 Last,
                                           const bool                              Inside,
                                           const bool                              Appro,
                                           const bool                              Forward,
                                           const bool                              RecP1,
                                           const bool                              RecRst1,
                                           const bool                              RecP2,
                                           const bool                              RecRst2,
                                           const math_Vector&                      Soldep);

  Standard_EXPORT virtual void PerformTwoCorner(const int Index) = 0;

  Standard_EXPORT virtual void PerformThreeCorner(const int Index) = 0;

  Standard_EXPORT void PerformMoreThreeCorner(const int Index, const int nbcourb);

  Standard_EXPORT virtual void ExtentOneCorner(const TopoDS_Vertex&              V,
                                               const occ::handle<ChFiDS_Stripe>& S) = 0;

  Standard_EXPORT virtual void ExtentTwoCorner(
    const TopoDS_Vertex&                                V,
    const NCollection_List<occ::handle<ChFiDS_Stripe>>& LS) = 0;

  Standard_EXPORT virtual void ExtentThreeCorner(
    const TopoDS_Vertex&                                V,
    const NCollection_List<occ::handle<ChFiDS_Stripe>>& LS) = 0;

  Standard_EXPORT virtual bool PerformFirstSection(const occ::handle<ChFiDS_Spine>&        S,
                                                   const occ::handle<ChFiDS_ElSpine>&      HGuide,
                                                   const int                               Choix,
                                                   occ::handle<BRepAdaptor_Surface>&       S1,
                                                   occ::handle<BRepAdaptor_Surface>&       S2,
                                                   const occ::handle<Adaptor3d_TopolTool>& I1,
                                                   const occ::handle<Adaptor3d_TopolTool>& I2,
                                                   const double                            Par,
                                                   math_Vector&                            SolDep,
                                                   TopAbs_State&                           Pos1,
                                                   TopAbs_State& Pos2) const = 0;

  Standard_EXPORT bool SearchFace(const occ::handle<ChFiDS_Spine>& Sp,
                                  const ChFiDS_CommonPoint&        Pc,
                                  const TopoDS_Face&               FRef,
                                  TopoDS_Face&                     FVoi) const;

  Standard_EXPORT bool StripeOrientations(const occ::handle<ChFiDS_Spine>& Sp,
                                          TopAbs_Orientation&              Or1,
                                          TopAbs_Orientation&              Or2,
                                          int&                             ChoixConge) const;

  //! Calculates a Line of contact face/face.
  Standard_EXPORT bool ComputeData(occ::handle<ChFiDS_SurfData>&           Data,
                                   const occ::handle<ChFiDS_ElSpine>&      Guide,
                                   const occ::handle<ChFiDS_Spine>&        Spine,
                                   occ::handle<BRepBlend_Line>&            Lin,
                                   const occ::handle<Adaptor3d_Surface>&   S1,
                                   const occ::handle<Adaptor3d_TopolTool>& I1,
                                   const occ::handle<Adaptor3d_Surface>&   S2,
                                   const occ::handle<Adaptor3d_TopolTool>& I2,
                                   Blend_Function&                         Func,
                                   Blend_FuncInv&                          FInv,
                                   const double                            PFirst,
                                   const double                            MaxStep,
                                   const double                            Fleche,
                                   const double                            TolGuide,
                                   double&                                 First,
                                   double&                                 Last,
                                   const bool                              Inside,
                                   const bool                              Appro,
                                   const bool                              Forward,
                                   const math_Vector&                      Soldep,
                                   int&                                    Intf,
                                   int&                                    Intl,
                                   bool&                                   Gd1,
                                   bool&                                   Gd2,
                                   bool&                                   Gf1,
                                   bool&                                   Gf2,
                                   const bool                              RecOnS1 = false,
                                   const bool                              RecOnS2 = false);

  //! Calculates a Line of contact edge/face.
  Standard_EXPORT bool ComputeData(occ::handle<ChFiDS_SurfData>&           Data,
                                   const occ::handle<ChFiDS_ElSpine>&      HGuide,
                                   occ::handle<BRepBlend_Line>&            Lin,
                                   const occ::handle<Adaptor3d_Surface>&   S1,
                                   const occ::handle<Adaptor3d_TopolTool>& I1,
                                   const occ::handle<Adaptor3d_Surface>&   S2,
                                   const occ::handle<Adaptor2d_Curve2d>&   PC2,
                                   const occ::handle<Adaptor3d_TopolTool>& I2,
                                   bool&                                   Decroch,
                                   Blend_SurfRstFunction&                  Func,
                                   Blend_FuncInv&                          FInv,
                                   Blend_SurfPointFuncInv&                 FInvP,
                                   Blend_SurfCurvFuncInv&                  FInvC,
                                   const double                            PFirst,
                                   const double                            MaxStep,
                                   const double                            Fleche,
                                   const double                            TolGuide,
                                   double&                                 First,
                                   double&                                 Last,
                                   const math_Vector&                      Soldep,
                                   const bool                              Inside,
                                   const bool                              Appro,
                                   const bool                              Forward,
                                   const bool                              RecP,
                                   const bool                              RecS,
                                   const bool                              RecRst);

  //! Calculates a Line of contact edge/edge.
  Standard_EXPORT bool ComputeData(occ::handle<ChFiDS_SurfData>&           Data,
                                   const occ::handle<ChFiDS_ElSpine>&      HGuide,
                                   occ::handle<BRepBlend_Line>&            Lin,
                                   const occ::handle<Adaptor3d_Surface>&   S1,
                                   const occ::handle<Adaptor2d_Curve2d>&   PC1,
                                   const occ::handle<Adaptor3d_TopolTool>& I1,
                                   bool&                                   Decroch1,
                                   const occ::handle<Adaptor3d_Surface>&   S2,
                                   const occ::handle<Adaptor2d_Curve2d>&   PC2,
                                   const occ::handle<Adaptor3d_TopolTool>& I2,
                                   bool&                                   Decroch2,
                                   Blend_RstRstFunction&                   Func,
                                   Blend_SurfCurvFuncInv&                  FInv1,
                                   Blend_CurvPointFuncInv&                 FInvP1,
                                   Blend_SurfCurvFuncInv&                  FInv2,
                                   Blend_CurvPointFuncInv&                 FInvP2,
                                   const double                            PFirst,
                                   const double                            MaxStep,
                                   const double                            Fleche,
                                   const double                            TolGuide,
                                   double&                                 First,
                                   double&                                 Last,
                                   const math_Vector&                      Soldep,
                                   const bool                              Inside,
                                   const bool                              Appro,
                                   const bool                              Forward,
                                   const bool                              RecP1,
                                   const bool                              RecRst1,
                                   const bool                              RecP2,
                                   const bool                              RecRst2);

  Standard_EXPORT bool CompleteData(occ::handle<ChFiDS_SurfData>&         Data,
                                    Blend_Function&                       Func,
                                    occ::handle<BRepBlend_Line>&          Lin,
                                    const occ::handle<Adaptor3d_Surface>& S1,
                                    const occ::handle<Adaptor3d_Surface>& S2,
                                    const TopAbs_Orientation              Or1,
                                    const bool                            Gd1,
                                    const bool                            Gd2,
                                    const bool                            Gf1,
                                    const bool                            Gf2,
                                    const bool                            Reversed = false);

  Standard_EXPORT bool CompleteData(occ::handle<ChFiDS_SurfData>&         Data,
                                    Blend_SurfRstFunction&                Func,
                                    occ::handle<BRepBlend_Line>&          Lin,
                                    const occ::handle<Adaptor3d_Surface>& S1,
                                    const occ::handle<Adaptor3d_Surface>& S2,
                                    const TopAbs_Orientation              Or,
                                    const bool                            Reversed);

  Standard_EXPORT bool CompleteData(occ::handle<ChFiDS_SurfData>&         Data,
                                    Blend_RstRstFunction&                 Func,
                                    occ::handle<BRepBlend_Line>&          Lin,
                                    const occ::handle<Adaptor3d_Surface>& S1,
                                    const occ::handle<Adaptor3d_Surface>& S2,
                                    const TopAbs_Orientation              Or);

  Standard_EXPORT bool StoreData(occ::handle<ChFiDS_SurfData>&         Data,
                                 const AppBlend_Approx&                Approx,
                                 const occ::handle<BRepBlend_Line>&    Lin,
                                 const occ::handle<Adaptor3d_Surface>& S1,
                                 const occ::handle<Adaptor3d_Surface>& S2,
                                 const TopAbs_Orientation              Or1,
                                 const bool                            Gd1,
                                 const bool                            Gd2,
                                 const bool                            Gf1,
                                 const bool                            Gf2,
                                 const bool                            Reversed = false);

  Standard_EXPORT bool CompleteData(occ::handle<ChFiDS_SurfData>&         Data,
                                    const occ::handle<Geom_Surface>&      Surfcoin,
                                    const occ::handle<Adaptor3d_Surface>& S1,
                                    const occ::handle<Geom2d_Curve>&      PC1,
                                    const occ::handle<Adaptor3d_Surface>& S2,
                                    const occ::handle<Geom2d_Curve>&      PC2,
                                    const TopAbs_Orientation              Or,
                                    const bool                            On1,
                                    const bool                            Gd1,
                                    const bool                            Gd2,
                                    const bool                            Gf1,
                                    const bool                            Gf2);

  double                                                                            tolappangle;
  double                                                                            tolesp;
  double                                                                            tol2d;
  double                                                                            tolapp3d;
  double                                                                            tolapp2d;
  double                                                                            fleche;
  GeomAbs_Shape                                                                     myConti;
  ChFiDS_Map                                                                        myEFMap;
  ChFiDS_Map                                                                        myESoMap;
  ChFiDS_Map                                                                        myEShMap;
  ChFiDS_Map                                                                        myVFMap;
  ChFiDS_Map                                                                        myVEMap;
  occ::handle<TopOpeBRepDS_HDataStructure>                                          myDS;
  occ::handle<TopOpeBRepBuild_HBuilder>                                             myCoup;
  NCollection_List<occ::handle<ChFiDS_Stripe>>                                      myListStripe;
  ChFiDS_StripeMap                                                                  myVDataMap;
  NCollection_List<ChFiDS_Regul>                                                    myRegul;
  NCollection_List<occ::handle<ChFiDS_Stripe>>                                      badstripes;
  NCollection_List<TopoDS_Shape>                                                    badvertices;
  NCollection_DataMap<TopoDS_Shape, NCollection_List<int>, TopTools_ShapeMapHasher> myEVIMap;
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>          myEdgeFirstFace;
  bool                                                                              done;
  bool                                                                              hasresult;

private:
  Standard_EXPORT bool FaceTangency(const TopoDS_Edge&   E0,
                                    const TopoDS_Edge&   E1,
                                    const TopoDS_Vertex& V) const;

  Standard_EXPORT void PerformSetOfSurfOnElSpine(const occ::handle<ChFiDS_ElSpine>&     ES,
                                                 occ::handle<ChFiDS_Stripe>&            St,
                                                 occ::handle<BRepTopAdaptor_TopolTool>& It1,
                                                 occ::handle<BRepTopAdaptor_TopolTool>& It2,
                                                 const bool Simul = false);

  Standard_EXPORT void PerformFilletOnVertex(const int Index);

  Standard_EXPORT void PerformSingularCorner(const int Index);

  Standard_EXPORT void PerformOneCorner(const int Index, const bool PrepareOnSame = false);

  Standard_EXPORT void IntersectMoreCorner(const int Index);

  Standard_EXPORT void PerformMoreSurfdata(const int Index);

  Standard_EXPORT void PerformIntersectionAtEnd(const int Index);

  Standard_EXPORT void ExtentAnalyse();

  Standard_EXPORT bool FindFace(const TopoDS_Vertex&      V,
                                const ChFiDS_CommonPoint& P1,
                                const ChFiDS_CommonPoint& P2,
                                TopoDS_Face&              Fv) const;

  Standard_EXPORT bool FindFace(const TopoDS_Vertex&      V,
                                const ChFiDS_CommonPoint& P1,
                                const ChFiDS_CommonPoint& P2,
                                TopoDS_Face&              Fv,
                                const TopoDS_Face&        Favoid) const;

  Standard_EXPORT bool MoreSurfdata(const int Index) const;

  Standard_EXPORT bool StartSol(const occ::handle<ChFiDS_Spine>&    Spine,
                                occ::handle<BRepAdaptor_Surface>&   HS,
                                gp_Pnt2d&                           P,
                                occ::handle<BRepAdaptor_Curve2d>&   HC,
                                double&                             W,
                                const occ::handle<ChFiDS_SurfData>& SD,
                                const bool                          isFirst,
                                const int                           OnS,
                                occ::handle<BRepAdaptor_Surface>&   HSref,
                                occ::handle<BRepAdaptor_Curve2d>&   HCref,
                                bool&                               RecP,
                                bool&                               RecS,
                                bool&                               RecRst,
                                bool&                               C1Obst,
                                occ::handle<BRepAdaptor_Surface>&   HSbis,
                                gp_Pnt2d&                           Pbis,
                                const bool                          Decroch,
                                const TopoDS_Vertex&                Vref) const;

  Standard_EXPORT void StartSol(const occ::handle<ChFiDS_Stripe>&      S,
                                const occ::handle<ChFiDS_ElSpine>&     HGuide,
                                occ::handle<BRepAdaptor_Surface>&      HS1,
                                occ::handle<BRepAdaptor_Surface>&      HS2,
                                occ::handle<BRepTopAdaptor_TopolTool>& I1,
                                occ::handle<BRepTopAdaptor_TopolTool>& I2,
                                gp_Pnt2d&                              P1,
                                gp_Pnt2d&                              P2,
                                double&                                First) const;

  Standard_EXPORT void ConexFaces(const occ::handle<ChFiDS_Spine>&  Sp,
                                  const int                         IEdge,
                                  occ::handle<BRepAdaptor_Surface>& HS1,
                                  occ::handle<BRepAdaptor_Surface>& HS2) const;

  //! Assign to tolesp parameter minimal value of spine's tolesp if it is less
  //! than default initial value.
  Standard_EXPORT void UpdateTolesp();

  TopoDS_Shape                   myShape;
  double                         angular;
  NCollection_List<TopoDS_Shape> myGenerated;
  TopoDS_Shape                   myShapeResult;
  TopoDS_Shape                   badShape;
};

#endif // _ChFi3d_Builder_HeaderFile
