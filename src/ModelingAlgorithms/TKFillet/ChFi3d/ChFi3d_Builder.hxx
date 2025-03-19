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
#include <ChFiDS_Regularities.hxx>
#include <ChFiDS_SequenceOfSurfData.hxx>
#include <ChFiDS_StripeMap.hxx>
#include <ChFiDS_ElSpine.hxx>
#include <math_Vector.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_DataMapOfShapeListOfInteger.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
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

//! Root  class  for calculation of  surfaces (fillets,
//! chamfers)  destined  to smooth edges  of
//! a gap on a Shape and the reconstruction of  the   Shape.
class ChFi3d_Builder
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT virtual ~ChFi3d_Builder();

  Standard_EXPORT void SetParams(const Standard_Real Tang,
                                 const Standard_Real Tesp,
                                 const Standard_Real T2d,
                                 const Standard_Real TApp3d,
                                 const Standard_Real TolApp2d,
                                 const Standard_Real Fleche);

  Standard_EXPORT void SetContinuity(const GeomAbs_Shape InternalContinuity,
                                     const Standard_Real AngularTolerance);

  //! extracts from  the list the contour containing edge E.
  Standard_EXPORT void Remove(const TopoDS_Edge& E);

  //! gives the number of  the contour containing E or 0
  //! if E does  not  belong to  any  contour.
  Standard_EXPORT Standard_Integer Contains(const TopoDS_Edge& E) const;

  //! gives  the number of  the contour containing E or 0
  //! if E does  not  belong  to  any  contour.
  //! Sets in IndexInSpine the index of E in the contour if it's found
  Standard_EXPORT Standard_Integer Contains(const TopoDS_Edge& E,
                                            Standard_Integer&  IndexInSpine) const;

  //! gives the number of  disjoint contours on  which
  //! the  fillets  are  calculated
  Standard_EXPORT Standard_Integer NbElements() const;

  //! gives the n'th set  of edges (contour)
  //! if I >NbElements()
  Standard_EXPORT Handle(ChFiDS_Spine) Value(const Standard_Integer I) const;

  //! returns the length of  the contour of index IC.
  Standard_EXPORT Standard_Real Length(const Standard_Integer IC) const;

  //! returns the First vertex V of
  //! the contour of index IC.
  Standard_EXPORT TopoDS_Vertex FirstVertex(const Standard_Integer IC) const;

  //! returns the Last vertex V of
  //! the contour of index IC.
  Standard_EXPORT TopoDS_Vertex LastVertex(const Standard_Integer IC) const;

  //! returns the abscissa of the vertex V on
  //! the contour of index IC.
  Standard_EXPORT Standard_Real Abscissa(const Standard_Integer IC, const TopoDS_Vertex& V) const;

  //! returns the relative abscissa([0.,1.]) of the
  //! vertex V on the contour of index IC.
  Standard_EXPORT Standard_Real RelativeAbscissa(const Standard_Integer IC,
                                                 const TopoDS_Vertex&   V) const;

  //! returns true if the contour of index IC is closed
  //! an tangent.
  Standard_EXPORT Standard_Boolean ClosedAndTangent(const Standard_Integer IC) const;

  //! returns true if the contour of index IC is closed
  Standard_EXPORT Standard_Boolean Closed(const Standard_Integer IC) const;

  //! general calculation of geometry on all edges,
  //! topologic reconstruction.
  Standard_EXPORT void Compute();

  //! returns True if the computation  is  success
  Standard_EXPORT Standard_Boolean IsDone() const;

  //! if (Isdone()) makes the result.
  //! if (!Isdone())
  Standard_EXPORT TopoDS_Shape Shape() const;

  //! Advanced  function for the history
  Standard_EXPORT const TopTools_ListOfShape& Generated(const TopoDS_Shape& EouV);

  //! Returns the number of contours on  which the calculation
  //! has failed.
  Standard_EXPORT Standard_Integer NbFaultyContours() const;

  //! Returns the number of  I'th contour on  which the calculation
  //! has failed.
  Standard_EXPORT Standard_Integer FaultyContour(const Standard_Integer I) const;

  //! Returns the number of  surfaces calculated  on  the contour IC.
  Standard_EXPORT Standard_Integer NbComputedSurfaces(const Standard_Integer IC) const;

  //! Returns the IS'th surface calculated on  the contour IC.
  Standard_EXPORT Handle(Geom_Surface) ComputedSurface(const Standard_Integer IC,
                                                       const Standard_Integer IS) const;

  //! Returns the number of vertices on  which the calculation
  //! has failed.
  Standard_EXPORT Standard_Integer NbFaultyVertices() const;

  //! Returns the IV'th vertex on  which the calculation has failed.
  Standard_EXPORT TopoDS_Vertex FaultyVertex(const Standard_Integer IV) const;

  //! returns True if  a partial result has  been  calculated
  Standard_EXPORT Standard_Boolean HasResult() const;

  //! if (HasResult()) returns partial result
  //! if (!HasResult())
  Standard_EXPORT TopoDS_Shape BadShape() const;

  //! for the stripe IC ,indication on the cause
  //! of  failure WalkingFailure,TwistedSurface,Error, Ok
  Standard_EXPORT ChFiDS_ErrorStatus StripeStatus(const Standard_Integer IC) const;

  //! Reset all results of compute and returns the algorithm
  //! in the state of the last acquisition to enable modification of contours or areas.
  Standard_EXPORT void Reset();

  //! Returns the Builder of  topologic operations.
  Standard_EXPORT Handle(TopOpeBRepBuild_HBuilder) Builder() const;

  //! Method, implemented in the inheritants, calculates
  //! the elements of construction of the surface (fillet or
  //! chamfer).
  Standard_EXPORT Standard_Boolean SplitKPart(const Handle(ChFiDS_SurfData)&     Data,
                                              ChFiDS_SequenceOfSurfData&         SetData,
                                              const Handle(ChFiDS_Spine)&        Spine,
                                              const Standard_Integer             Iedge,
                                              const Handle(Adaptor3d_Surface)&   S1,
                                              const Handle(Adaptor3d_TopolTool)& I1,
                                              const Handle(Adaptor3d_Surface)&   S2,
                                              const Handle(Adaptor3d_TopolTool)& I2,
                                              Standard_Boolean&                  Intf,
                                              Standard_Boolean&                  Intl);

  Standard_EXPORT Standard_Boolean PerformTwoCornerbyInter(const Standard_Integer Index);

protected:
  Standard_EXPORT ChFi3d_Builder(const TopoDS_Shape& S, const Standard_Real Ta);

  Standard_EXPORT virtual void SimulKPart(const Handle(ChFiDS_SurfData)& SD) const = 0;

  Standard_EXPORT virtual Standard_Boolean SimulSurf(Handle(ChFiDS_SurfData)&           Data,
                                                     const Handle(ChFiDS_ElSpine)&      Guide,
                                                     const Handle(ChFiDS_Spine)&        Spine,
                                                     const Standard_Integer             Choix,
                                                     const Handle(BRepAdaptor_Surface)& S1,
                                                     const Handle(Adaptor3d_TopolTool)& I1,
                                                     const Handle(BRepAdaptor_Surface)& S2,
                                                     const Handle(Adaptor3d_TopolTool)& I2,
                                                     const Standard_Real                TolGuide,
                                                     Standard_Real&                     First,
                                                     Standard_Real&                     Last,
                                                     const Standard_Boolean             Inside,
                                                     const Standard_Boolean             Appro,
                                                     const Standard_Boolean             Forward,
                                                     const Standard_Boolean             RecOnS1,
                                                     const Standard_Boolean             RecOnS2,
                                                     const math_Vector&                 Soldep,
                                                     Standard_Integer&                  Intf,
                                                     Standard_Integer&                  Intl) = 0;

  Standard_EXPORT virtual void SimulSurf(Handle(ChFiDS_SurfData)&           Data,
                                         const Handle(ChFiDS_ElSpine)&      Guide,
                                         const Handle(ChFiDS_Spine)&        Spine,
                                         const Standard_Integer             Choix,
                                         const Handle(BRepAdaptor_Surface)& S1,
                                         const Handle(Adaptor3d_TopolTool)& I1,
                                         const Handle(BRepAdaptor_Curve2d)& PC1,
                                         const Handle(BRepAdaptor_Surface)& Sref1,
                                         const Handle(BRepAdaptor_Curve2d)& PCref1,
                                         Standard_Boolean&                  Decroch1,
                                         const Handle(BRepAdaptor_Surface)& S2,
                                         const Handle(Adaptor3d_TopolTool)& I2,
                                         const TopAbs_Orientation           Or2,
                                         const Standard_Real                Fleche,
                                         const Standard_Real                TolGuide,
                                         Standard_Real&                     First,
                                         Standard_Real&                     Last,
                                         const Standard_Boolean             Inside,
                                         const Standard_Boolean             Appro,
                                         const Standard_Boolean             Forward,
                                         const Standard_Boolean             RecP,
                                         const Standard_Boolean             RecS,
                                         const Standard_Boolean             RecRst,
                                         const math_Vector&                 Soldep);

  Standard_EXPORT virtual void SimulSurf(Handle(ChFiDS_SurfData)&           Data,
                                         const Handle(ChFiDS_ElSpine)&      Guide,
                                         const Handle(ChFiDS_Spine)&        Spine,
                                         const Standard_Integer             Choix,
                                         const Handle(BRepAdaptor_Surface)& S1,
                                         const Handle(Adaptor3d_TopolTool)& I1,
                                         const TopAbs_Orientation           Or1,
                                         const Handle(BRepAdaptor_Surface)& S2,
                                         const Handle(Adaptor3d_TopolTool)& I2,
                                         const Handle(BRepAdaptor_Curve2d)& PC2,
                                         const Handle(BRepAdaptor_Surface)& Sref2,
                                         const Handle(BRepAdaptor_Curve2d)& PCref2,
                                         Standard_Boolean&                  Decroch2,
                                         const Standard_Real                Fleche,
                                         const Standard_Real                TolGuide,
                                         Standard_Real&                     First,
                                         Standard_Real&                     Last,
                                         const Standard_Boolean             Inside,
                                         const Standard_Boolean             Appro,
                                         const Standard_Boolean             Forward,
                                         const Standard_Boolean             RecP,
                                         const Standard_Boolean             RecS,
                                         const Standard_Boolean             RecRst,
                                         const math_Vector&                 Soldep);

  Standard_EXPORT virtual void SimulSurf(Handle(ChFiDS_SurfData)&           Data,
                                         const Handle(ChFiDS_ElSpine)&      Guide,
                                         const Handle(ChFiDS_Spine)&        Spine,
                                         const Standard_Integer             Choix,
                                         const Handle(BRepAdaptor_Surface)& S1,
                                         const Handle(Adaptor3d_TopolTool)& I1,
                                         const Handle(BRepAdaptor_Curve2d)& PC1,
                                         const Handle(BRepAdaptor_Surface)& Sref1,
                                         const Handle(BRepAdaptor_Curve2d)& PCref1,
                                         Standard_Boolean&                  Decroch1,
                                         const TopAbs_Orientation           Or1,
                                         const Handle(BRepAdaptor_Surface)& S2,
                                         const Handle(Adaptor3d_TopolTool)& I2,
                                         const Handle(BRepAdaptor_Curve2d)& PC2,
                                         const Handle(BRepAdaptor_Surface)& Sref2,
                                         const Handle(BRepAdaptor_Curve2d)& PCref2,
                                         Standard_Boolean&                  Decroch2,
                                         const TopAbs_Orientation           Or2,
                                         const Standard_Real                Fleche,
                                         const Standard_Real                TolGuide,
                                         Standard_Real&                     First,
                                         Standard_Real&                     Last,
                                         const Standard_Boolean             Inside,
                                         const Standard_Boolean             Appro,
                                         const Standard_Boolean             Forward,
                                         const Standard_Boolean             RecP1,
                                         const Standard_Boolean             RecRst1,
                                         const Standard_Boolean             RecP2,
                                         const Standard_Boolean             RecRst2,
                                         const math_Vector&                 Soldep);

  Standard_EXPORT Standard_Boolean SimulData(Handle(ChFiDS_SurfData)&           Data,
                                             const Handle(ChFiDS_ElSpine)&      Guide,
                                             const Handle(ChFiDS_ElSpine)&      AdditionalGuide,
                                             Handle(BRepBlend_Line)&            Lin,
                                             const Handle(Adaptor3d_Surface)&   S1,
                                             const Handle(Adaptor3d_TopolTool)& I1,
                                             const Handle(Adaptor3d_Surface)&   S2,
                                             const Handle(Adaptor3d_TopolTool)& I2,
                                             Blend_Function&                    Func,
                                             Blend_FuncInv&                     FInv,
                                             const Standard_Real                PFirst,
                                             const Standard_Real                MaxStep,
                                             const Standard_Real                Fleche,
                                             const Standard_Real                TolGuide,
                                             Standard_Real&                     First,
                                             Standard_Real&                     Last,
                                             const Standard_Boolean             Inside,
                                             const Standard_Boolean             Appro,
                                             const Standard_Boolean             Forward,
                                             const math_Vector&                 Soldep,
                                             const Standard_Integer             NbSecMin,
                                             const Standard_Boolean RecOnS1 = Standard_False,
                                             const Standard_Boolean RecOnS2 = Standard_False);

  Standard_EXPORT Standard_Boolean SimulData(Handle(ChFiDS_SurfData)&           Data,
                                             const Handle(ChFiDS_ElSpine)&      HGuide,
                                             Handle(BRepBlend_Line)&            Lin,
                                             const Handle(Adaptor3d_Surface)&   S1,
                                             const Handle(Adaptor3d_TopolTool)& I1,
                                             const Handle(Adaptor3d_Surface)&   S2,
                                             const Handle(Adaptor2d_Curve2d)&   PC2,
                                             const Handle(Adaptor3d_TopolTool)& I2,
                                             Standard_Boolean&                  Decroch,
                                             Blend_SurfRstFunction&             Func,
                                             Blend_FuncInv&                     FInv,
                                             Blend_SurfPointFuncInv&            FInvP,
                                             Blend_SurfCurvFuncInv&             FInvC,
                                             const Standard_Real                PFirst,
                                             const Standard_Real                MaxStep,
                                             const Standard_Real                Fleche,
                                             const Standard_Real                TolGuide,
                                             Standard_Real&                     First,
                                             Standard_Real&                     Last,
                                             const math_Vector&                 Soldep,
                                             const Standard_Integer             NbSecMin,
                                             const Standard_Boolean             Inside,
                                             const Standard_Boolean             Appro,
                                             const Standard_Boolean             Forward,
                                             const Standard_Boolean             RecP,
                                             const Standard_Boolean             RecS,
                                             const Standard_Boolean             RecRst);

  Standard_EXPORT Standard_Boolean SimulData(Handle(ChFiDS_SurfData)&           Data,
                                             const Handle(ChFiDS_ElSpine)&      HGuide,
                                             Handle(BRepBlend_Line)&            Lin,
                                             const Handle(Adaptor3d_Surface)&   S1,
                                             const Handle(Adaptor2d_Curve2d)&   PC1,
                                             const Handle(Adaptor3d_TopolTool)& I1,
                                             Standard_Boolean&                  Decroch1,
                                             const Handle(Adaptor3d_Surface)&   S2,
                                             const Handle(Adaptor2d_Curve2d)&   PC2,
                                             const Handle(Adaptor3d_TopolTool)& I2,
                                             Standard_Boolean&                  Decroch2,
                                             Blend_RstRstFunction&              Func,
                                             Blend_SurfCurvFuncInv&             FInv1,
                                             Blend_CurvPointFuncInv&            FInvP1,
                                             Blend_SurfCurvFuncInv&             FInv2,
                                             Blend_CurvPointFuncInv&            FInvP2,
                                             const Standard_Real                PFirst,
                                             const Standard_Real                MaxStep,
                                             const Standard_Real                Fleche,
                                             const Standard_Real                TolGuide,
                                             Standard_Real&                     First,
                                             Standard_Real&                     Last,
                                             const math_Vector&                 Soldep,
                                             const Standard_Integer             NbSecMin,
                                             const Standard_Boolean             Inside,
                                             const Standard_Boolean             Appro,
                                             const Standard_Boolean             Forward,
                                             const Standard_Boolean             RecP1,
                                             const Standard_Boolean             RecRst1,
                                             const Standard_Boolean             RecP2,
                                             const Standard_Boolean             RecRst2);

  Standard_EXPORT virtual void SetRegul() = 0;

  Standard_EXPORT Standard_Boolean PerformElement(const Handle(ChFiDS_Spine)& CElement,
                                                  const Standard_Real         Offset,
                                                  const TopoDS_Face&          theFirstFace);

  Standard_EXPORT void PerformExtremity(const Handle(ChFiDS_Spine)& CElement);

  Standard_EXPORT void PerformSetOfSurf(Handle(ChFiDS_Stripe)& S,
                                        const Standard_Boolean Simul = Standard_False);

  Standard_EXPORT void PerformSetOfKPart(Handle(ChFiDS_Stripe)& S,
                                         const Standard_Boolean Simul = Standard_False);

  Standard_EXPORT void PerformSetOfKGen(Handle(ChFiDS_Stripe)& S,
                                        const Standard_Boolean Simul = Standard_False);

  Standard_EXPORT void Trunc(const Handle(ChFiDS_SurfData)&   SD,
                             const Handle(ChFiDS_Spine)&      Spine,
                             const Handle(Adaptor3d_Surface)& S1,
                             const Handle(Adaptor3d_Surface)& S2,
                             const Standard_Integer           iedge,
                             const Standard_Boolean           isfirst,
                             const Standard_Integer           cntlFiOnS);

  Standard_EXPORT void CallPerformSurf(Handle(ChFiDS_Stripe)&             Stripe,
                                       const Standard_Boolean             Simul,
                                       ChFiDS_SequenceOfSurfData&         SeqSD,
                                       Handle(ChFiDS_SurfData)&           SD,
                                       const Handle(ChFiDS_ElSpine)&      Guide,
                                       const Handle(ChFiDS_Spine)&        Spine,
                                       const Handle(BRepAdaptor_Surface)& HS1,
                                       const Handle(BRepAdaptor_Surface)& HS3,
                                       const gp_Pnt2d&                    P1,
                                       const gp_Pnt2d&                    P3,
                                       const Handle(Adaptor3d_TopolTool)& I1,
                                       const Handle(BRepAdaptor_Surface)& HS2,
                                       const Handle(BRepAdaptor_Surface)& HS4,
                                       const gp_Pnt2d&                    P2,
                                       const gp_Pnt2d&                    P4,
                                       const Handle(Adaptor3d_TopolTool)& I2,
                                       const Standard_Real                MaxStep,
                                       const Standard_Real                Fleche,
                                       const Standard_Real                TolGuide,
                                       Standard_Real&                     First,
                                       Standard_Real&                     Last,
                                       const Standard_Boolean             Inside,
                                       const Standard_Boolean             Appro,
                                       const Standard_Boolean             Forward,
                                       const Standard_Boolean             RecOnS1,
                                       const Standard_Boolean             RecOnS2,
                                       math_Vector&                       Soldep,
                                       Standard_Integer&                  Intf,
                                       Standard_Integer&                  Intl,
                                       Handle(BRepAdaptor_Surface)&       Surf1,
                                       Handle(BRepAdaptor_Surface)&       Surf2);

  //! Method, implemented in the inheritants, calculating
  //! elements of construction of the surface (fillet or
  //! chamfer).
  Standard_EXPORT virtual Standard_Boolean PerformSurf(ChFiDS_SequenceOfSurfData&         Data,
                                                       const Handle(ChFiDS_ElSpine)&      Guide,
                                                       const Handle(ChFiDS_Spine)&        Spine,
                                                       const Standard_Integer             Choix,
                                                       const Handle(BRepAdaptor_Surface)& S1,
                                                       const Handle(Adaptor3d_TopolTool)& I1,
                                                       const Handle(BRepAdaptor_Surface)& S2,
                                                       const Handle(Adaptor3d_TopolTool)& I2,
                                                       const Standard_Real                MaxStep,
                                                       const Standard_Real                Fleche,
                                                       const Standard_Real                TolGuide,
                                                       Standard_Real&                     First,
                                                       Standard_Real&                     Last,
                                                       const Standard_Boolean             Inside,
                                                       const Standard_Boolean             Appro,
                                                       const Standard_Boolean             Forward,
                                                       const Standard_Boolean             RecOnS1,
                                                       const Standard_Boolean             RecOnS2,
                                                       const math_Vector&                 Soldep,
                                                       Standard_Integer&                  Intf,
                                                       Standard_Integer&                  Intl) = 0;

  //! Method, implemented  in inheritants, calculates
  //! the elements of construction of  the surface (fillet
  //! or chamfer) contact edge/face.
  Standard_EXPORT virtual void PerformSurf(ChFiDS_SequenceOfSurfData&         Data,
                                           const Handle(ChFiDS_ElSpine)&      Guide,
                                           const Handle(ChFiDS_Spine)&        Spine,
                                           const Standard_Integer             Choix,
                                           const Handle(BRepAdaptor_Surface)& S1,
                                           const Handle(Adaptor3d_TopolTool)& I1,
                                           const Handle(BRepAdaptor_Curve2d)& PC1,
                                           const Handle(BRepAdaptor_Surface)& Sref1,
                                           const Handle(BRepAdaptor_Curve2d)& PCref1,
                                           Standard_Boolean&                  Decroch1,
                                           const Handle(BRepAdaptor_Surface)& S2,
                                           const Handle(Adaptor3d_TopolTool)& I2,
                                           const TopAbs_Orientation           Or2,
                                           const Standard_Real                MaxStep,
                                           const Standard_Real                Fleche,
                                           const Standard_Real                TolGuide,
                                           Standard_Real&                     First,
                                           Standard_Real&                     Last,
                                           const Standard_Boolean             Inside,
                                           const Standard_Boolean             Appro,
                                           const Standard_Boolean             Forward,
                                           const Standard_Boolean             RecP,
                                           const Standard_Boolean             RecS,
                                           const Standard_Boolean             RecRst,
                                           const math_Vector&                 Soldep);

  //! Method, implemented in  inheritants, calculates
  //! the elements of construction of  the surface (fillet
  //! or chamfer) contact edge/face.
  Standard_EXPORT virtual void PerformSurf(ChFiDS_SequenceOfSurfData&         Data,
                                           const Handle(ChFiDS_ElSpine)&      Guide,
                                           const Handle(ChFiDS_Spine)&        Spine,
                                           const Standard_Integer             Choix,
                                           const Handle(BRepAdaptor_Surface)& S1,
                                           const Handle(Adaptor3d_TopolTool)& I1,
                                           const TopAbs_Orientation           Or1,
                                           const Handle(BRepAdaptor_Surface)& S2,
                                           const Handle(Adaptor3d_TopolTool)& I2,
                                           const Handle(BRepAdaptor_Curve2d)& PC2,
                                           const Handle(BRepAdaptor_Surface)& Sref2,
                                           const Handle(BRepAdaptor_Curve2d)& PCref2,
                                           Standard_Boolean&                  Decroch2,
                                           const Standard_Real                MaxStep,
                                           const Standard_Real                Fleche,
                                           const Standard_Real                TolGuide,
                                           Standard_Real&                     First,
                                           Standard_Real&                     Last,
                                           const Standard_Boolean             Inside,
                                           const Standard_Boolean             Appro,
                                           const Standard_Boolean             Forward,
                                           const Standard_Boolean             RecP,
                                           const Standard_Boolean             RecS,
                                           const Standard_Boolean             RecRst,
                                           const math_Vector&                 Soldep);

  //! Method, implemented in inheritants, calculates
  //! the elements of construction of  the surface (fillet
  //! or chamfer) contact edge/edge.
  Standard_EXPORT virtual void PerformSurf(ChFiDS_SequenceOfSurfData&         Data,
                                           const Handle(ChFiDS_ElSpine)&      Guide,
                                           const Handle(ChFiDS_Spine)&        Spine,
                                           const Standard_Integer             Choix,
                                           const Handle(BRepAdaptor_Surface)& S1,
                                           const Handle(Adaptor3d_TopolTool)& I1,
                                           const Handle(BRepAdaptor_Curve2d)& PC1,
                                           const Handle(BRepAdaptor_Surface)& Sref1,
                                           const Handle(BRepAdaptor_Curve2d)& PCref1,
                                           Standard_Boolean&                  Decroch1,
                                           const TopAbs_Orientation           Or1,
                                           const Handle(BRepAdaptor_Surface)& S2,
                                           const Handle(Adaptor3d_TopolTool)& I2,
                                           const Handle(BRepAdaptor_Curve2d)& PC2,
                                           const Handle(BRepAdaptor_Surface)& Sref2,
                                           const Handle(BRepAdaptor_Curve2d)& PCref2,
                                           Standard_Boolean&                  Decroch2,
                                           const TopAbs_Orientation           Or2,
                                           const Standard_Real                MaxStep,
                                           const Standard_Real                Fleche,
                                           const Standard_Real                TolGuide,
                                           Standard_Real&                     First,
                                           Standard_Real&                     Last,
                                           const Standard_Boolean             Inside,
                                           const Standard_Boolean             Appro,
                                           const Standard_Boolean             Forward,
                                           const Standard_Boolean             RecP1,
                                           const Standard_Boolean             RecRst1,
                                           const Standard_Boolean             RecP2,
                                           const Standard_Boolean             RecRst2,
                                           const math_Vector&                 Soldep);

  Standard_EXPORT virtual void PerformTwoCorner(const Standard_Integer Index) = 0;

  Standard_EXPORT virtual void PerformThreeCorner(const Standard_Integer Index) = 0;

  Standard_EXPORT void PerformMoreThreeCorner(const Standard_Integer Index,
                                              const Standard_Integer nbcourb);

  Standard_EXPORT virtual void ExtentOneCorner(const TopoDS_Vertex&         V,
                                               const Handle(ChFiDS_Stripe)& S) = 0;

  Standard_EXPORT virtual void ExtentTwoCorner(const TopoDS_Vertex&       V,
                                               const ChFiDS_ListOfStripe& LS) = 0;

  Standard_EXPORT virtual void ExtentThreeCorner(const TopoDS_Vertex&       V,
                                                 const ChFiDS_ListOfStripe& LS) = 0;

  Standard_EXPORT virtual Standard_Boolean PerformFirstSection(
    const Handle(ChFiDS_Spine)&        S,
    const Handle(ChFiDS_ElSpine)&      HGuide,
    const Standard_Integer             Choix,
    Handle(BRepAdaptor_Surface)&       S1,
    Handle(BRepAdaptor_Surface)&       S2,
    const Handle(Adaptor3d_TopolTool)& I1,
    const Handle(Adaptor3d_TopolTool)& I2,
    const Standard_Real                Par,
    math_Vector&                       SolDep,
    TopAbs_State&                      Pos1,
    TopAbs_State&                      Pos2) const = 0;

  Standard_EXPORT Standard_Boolean SearchFace(const Handle(ChFiDS_Spine)& Sp,
                                              const ChFiDS_CommonPoint&   Pc,
                                              const TopoDS_Face&          FRef,
                                              TopoDS_Face&                FVoi) const;

  Standard_EXPORT Standard_Boolean StripeOrientations(const Handle(ChFiDS_Spine)& Sp,
                                                      TopAbs_Orientation&         Or1,
                                                      TopAbs_Orientation&         Or2,
                                                      Standard_Integer&           ChoixConge) const;

  //! Calculates  a Line of contact face/face.
  Standard_EXPORT Standard_Boolean ComputeData(Handle(ChFiDS_SurfData)&           Data,
                                               const Handle(ChFiDS_ElSpine)&      Guide,
                                               const Handle(ChFiDS_Spine)&        Spine,
                                               Handle(BRepBlend_Line)&            Lin,
                                               const Handle(Adaptor3d_Surface)&   S1,
                                               const Handle(Adaptor3d_TopolTool)& I1,
                                               const Handle(Adaptor3d_Surface)&   S2,
                                               const Handle(Adaptor3d_TopolTool)& I2,
                                               Blend_Function&                    Func,
                                               Blend_FuncInv&                     FInv,
                                               const Standard_Real                PFirst,
                                               const Standard_Real                MaxStep,
                                               const Standard_Real                Fleche,
                                               const Standard_Real                TolGuide,
                                               Standard_Real&                     First,
                                               Standard_Real&                     Last,
                                               const Standard_Boolean             Inside,
                                               const Standard_Boolean             Appro,
                                               const Standard_Boolean             Forward,
                                               const math_Vector&                 Soldep,
                                               Standard_Integer&                  Intf,
                                               Standard_Integer&                  Intl,
                                               Standard_Boolean&                  Gd1,
                                               Standard_Boolean&                  Gd2,
                                               Standard_Boolean&                  Gf1,
                                               Standard_Boolean&                  Gf2,
                                               const Standard_Boolean RecOnS1 = Standard_False,
                                               const Standard_Boolean RecOnS2 = Standard_False);

  //! Calculates a Line of contact edge/face.
  Standard_EXPORT Standard_Boolean ComputeData(Handle(ChFiDS_SurfData)&           Data,
                                               const Handle(ChFiDS_ElSpine)&      HGuide,
                                               Handle(BRepBlend_Line)&            Lin,
                                               const Handle(Adaptor3d_Surface)&   S1,
                                               const Handle(Adaptor3d_TopolTool)& I1,
                                               const Handle(Adaptor3d_Surface)&   S2,
                                               const Handle(Adaptor2d_Curve2d)&   PC2,
                                               const Handle(Adaptor3d_TopolTool)& I2,
                                               Standard_Boolean&                  Decroch,
                                               Blend_SurfRstFunction&             Func,
                                               Blend_FuncInv&                     FInv,
                                               Blend_SurfPointFuncInv&            FInvP,
                                               Blend_SurfCurvFuncInv&             FInvC,
                                               const Standard_Real                PFirst,
                                               const Standard_Real                MaxStep,
                                               const Standard_Real                Fleche,
                                               const Standard_Real                TolGuide,
                                               Standard_Real&                     First,
                                               Standard_Real&                     Last,
                                               const math_Vector&                 Soldep,
                                               const Standard_Boolean             Inside,
                                               const Standard_Boolean             Appro,
                                               const Standard_Boolean             Forward,
                                               const Standard_Boolean             RecP,
                                               const Standard_Boolean             RecS,
                                               const Standard_Boolean             RecRst);

  //! Calculates a Line of contact edge/edge.
  Standard_EXPORT Standard_Boolean ComputeData(Handle(ChFiDS_SurfData)&           Data,
                                               const Handle(ChFiDS_ElSpine)&      HGuide,
                                               Handle(BRepBlend_Line)&            Lin,
                                               const Handle(Adaptor3d_Surface)&   S1,
                                               const Handle(Adaptor2d_Curve2d)&   PC1,
                                               const Handle(Adaptor3d_TopolTool)& I1,
                                               Standard_Boolean&                  Decroch1,
                                               const Handle(Adaptor3d_Surface)&   S2,
                                               const Handle(Adaptor2d_Curve2d)&   PC2,
                                               const Handle(Adaptor3d_TopolTool)& I2,
                                               Standard_Boolean&                  Decroch2,
                                               Blend_RstRstFunction&              Func,
                                               Blend_SurfCurvFuncInv&             FInv1,
                                               Blend_CurvPointFuncInv&            FInvP1,
                                               Blend_SurfCurvFuncInv&             FInv2,
                                               Blend_CurvPointFuncInv&            FInvP2,
                                               const Standard_Real                PFirst,
                                               const Standard_Real                MaxStep,
                                               const Standard_Real                Fleche,
                                               const Standard_Real                TolGuide,
                                               Standard_Real&                     First,
                                               Standard_Real&                     Last,
                                               const math_Vector&                 Soldep,
                                               const Standard_Boolean             Inside,
                                               const Standard_Boolean             Appro,
                                               const Standard_Boolean             Forward,
                                               const Standard_Boolean             RecP1,
                                               const Standard_Boolean             RecRst1,
                                               const Standard_Boolean             RecP2,
                                               const Standard_Boolean             RecRst2);

  Standard_EXPORT Standard_Boolean CompleteData(Handle(ChFiDS_SurfData)&         Data,
                                                Blend_Function&                  Func,
                                                Handle(BRepBlend_Line)&          Lin,
                                                const Handle(Adaptor3d_Surface)& S1,
                                                const Handle(Adaptor3d_Surface)& S2,
                                                const TopAbs_Orientation         Or1,
                                                const Standard_Boolean           Gd1,
                                                const Standard_Boolean           Gd2,
                                                const Standard_Boolean           Gf1,
                                                const Standard_Boolean           Gf2,
                                                const Standard_Boolean Reversed = Standard_False);

  Standard_EXPORT Standard_Boolean CompleteData(Handle(ChFiDS_SurfData)&         Data,
                                                Blend_SurfRstFunction&           Func,
                                                Handle(BRepBlend_Line)&          Lin,
                                                const Handle(Adaptor3d_Surface)& S1,
                                                const Handle(Adaptor3d_Surface)& S2,
                                                const TopAbs_Orientation         Or,
                                                const Standard_Boolean           Reversed);

  Standard_EXPORT Standard_Boolean CompleteData(Handle(ChFiDS_SurfData)&         Data,
                                                Blend_RstRstFunction&            Func,
                                                Handle(BRepBlend_Line)&          Lin,
                                                const Handle(Adaptor3d_Surface)& S1,
                                                const Handle(Adaptor3d_Surface)& S2,
                                                const TopAbs_Orientation         Or);

  Standard_EXPORT Standard_Boolean StoreData(Handle(ChFiDS_SurfData)&         Data,
                                             const AppBlend_Approx&           Approx,
                                             const Handle(BRepBlend_Line)&    Lin,
                                             const Handle(Adaptor3d_Surface)& S1,
                                             const Handle(Adaptor3d_Surface)& S2,
                                             const TopAbs_Orientation         Or1,
                                             const Standard_Boolean           Gd1,
                                             const Standard_Boolean           Gd2,
                                             const Standard_Boolean           Gf1,
                                             const Standard_Boolean           Gf2,
                                             const Standard_Boolean Reversed = Standard_False);

  Standard_EXPORT Standard_Boolean CompleteData(Handle(ChFiDS_SurfData)&         Data,
                                                const Handle(Geom_Surface)&      Surfcoin,
                                                const Handle(Adaptor3d_Surface)& S1,
                                                const Handle(Geom2d_Curve)&      PC1,
                                                const Handle(Adaptor3d_Surface)& S2,
                                                const Handle(Geom2d_Curve)&      PC2,
                                                const TopAbs_Orientation         Or,
                                                const Standard_Boolean           On1,
                                                const Standard_Boolean           Gd1,
                                                const Standard_Boolean           Gd2,
                                                const Standard_Boolean           Gf1,
                                                const Standard_Boolean           Gf2);

  Standard_Real                        tolappangle;
  Standard_Real                        tolesp;
  Standard_Real                        tol2d;
  Standard_Real                        tolapp3d;
  Standard_Real                        tolapp2d;
  Standard_Real                        fleche;
  GeomAbs_Shape                        myConti;
  ChFiDS_Map                           myEFMap;
  ChFiDS_Map                           myESoMap;
  ChFiDS_Map                           myEShMap;
  ChFiDS_Map                           myVFMap;
  ChFiDS_Map                           myVEMap;
  Handle(TopOpeBRepDS_HDataStructure)  myDS;
  Handle(TopOpeBRepBuild_HBuilder)     myCoup;
  ChFiDS_ListOfStripe                  myListStripe;
  ChFiDS_StripeMap                     myVDataMap;
  ChFiDS_Regularities                  myRegul;
  ChFiDS_ListOfStripe                  badstripes;
  TopTools_ListOfShape                 badvertices;
  TopTools_DataMapOfShapeListOfInteger myEVIMap;
  TopTools_DataMapOfShapeShape         myEdgeFirstFace;
  Standard_Boolean                     done;
  Standard_Boolean                     hasresult;

private:
  Standard_EXPORT Standard_Boolean FaceTangency(const TopoDS_Edge&   E0,
                                                const TopoDS_Edge&   E1,
                                                const TopoDS_Vertex& V) const;

  Standard_EXPORT void PerformSetOfSurfOnElSpine(const Handle(ChFiDS_ElSpine)&     ES,
                                                 Handle(ChFiDS_Stripe)&            St,
                                                 Handle(BRepTopAdaptor_TopolTool)& It1,
                                                 Handle(BRepTopAdaptor_TopolTool)& It2,
                                                 const Standard_Boolean Simul = Standard_False);

  Standard_EXPORT void PerformFilletOnVertex(const Standard_Integer Index);

  Standard_EXPORT void PerformSingularCorner(const Standard_Integer Index);

  Standard_EXPORT void PerformOneCorner(const Standard_Integer Index,
                                        const Standard_Boolean PrepareOnSame = Standard_False);

  Standard_EXPORT void IntersectMoreCorner(const Standard_Integer Index);

  Standard_EXPORT void PerformMoreSurfdata(const Standard_Integer Index);

  Standard_EXPORT void PerformIntersectionAtEnd(const Standard_Integer Index);

  Standard_EXPORT void ExtentAnalyse();

  Standard_EXPORT Standard_Boolean FindFace(const TopoDS_Vertex&      V,
                                            const ChFiDS_CommonPoint& P1,
                                            const ChFiDS_CommonPoint& P2,
                                            TopoDS_Face&              Fv) const;

  Standard_EXPORT Standard_Boolean FindFace(const TopoDS_Vertex&      V,
                                            const ChFiDS_CommonPoint& P1,
                                            const ChFiDS_CommonPoint& P2,
                                            TopoDS_Face&              Fv,
                                            const TopoDS_Face&        Favoid) const;

  Standard_EXPORT Standard_Boolean MoreSurfdata(const Standard_Integer Index) const;

  Standard_EXPORT Standard_Boolean StartSol(const Handle(ChFiDS_Spine)&    Spine,
                                            Handle(BRepAdaptor_Surface)&   HS,
                                            gp_Pnt2d&                      P,
                                            Handle(BRepAdaptor_Curve2d)&   HC,
                                            Standard_Real&                 W,
                                            const Handle(ChFiDS_SurfData)& SD,
                                            const Standard_Boolean         isFirst,
                                            const Standard_Integer         OnS,
                                            Handle(BRepAdaptor_Surface)&   HSref,
                                            Handle(BRepAdaptor_Curve2d)&   HCref,
                                            Standard_Boolean&              RecP,
                                            Standard_Boolean&              RecS,
                                            Standard_Boolean&              RecRst,
                                            Standard_Boolean&              C1Obst,
                                            Handle(BRepAdaptor_Surface)&   HSbis,
                                            gp_Pnt2d&                      Pbis,
                                            const Standard_Boolean         Decroch,
                                            const TopoDS_Vertex&           Vref) const;

  Standard_EXPORT void StartSol(const Handle(ChFiDS_Stripe)&      S,
                                const Handle(ChFiDS_ElSpine)&     HGuide,
                                Handle(BRepAdaptor_Surface)&      HS1,
                                Handle(BRepAdaptor_Surface)&      HS2,
                                Handle(BRepTopAdaptor_TopolTool)& I1,
                                Handle(BRepTopAdaptor_TopolTool)& I2,
                                gp_Pnt2d&                         P1,
                                gp_Pnt2d&                         P2,
                                Standard_Real&                    First) const;

  Standard_EXPORT void ConexFaces(const Handle(ChFiDS_Spine)&  Sp,
                                  const Standard_Integer       IEdge,
                                  Handle(BRepAdaptor_Surface)& HS1,
                                  Handle(BRepAdaptor_Surface)& HS2) const;

  //! Assign to tolesp parameter minimal value of spine's tolesp if it is less
  //! than default initial value.
  Standard_EXPORT void UpdateTolesp();

  TopoDS_Shape         myShape;
  Standard_Real        angular;
  TopTools_ListOfShape myGenerated;
  TopoDS_Shape         myShapeResult;
  TopoDS_Shape         badShape;
};

#endif // _ChFi3d_Builder_HeaderFile
