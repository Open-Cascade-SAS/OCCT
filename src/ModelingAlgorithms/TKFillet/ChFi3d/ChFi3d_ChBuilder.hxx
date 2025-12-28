// Created on: 1995-04-26
// Created by: Modelistation
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

#ifndef _ChFi3d_ChBuilder_HeaderFile
#define _ChFi3d_ChBuilder_HeaderFile

#include <BRepAdaptor_Surface.hxx>
#include <ChFi3d_Builder.hxx>
#include <ChFiDS_ChamfMethod.hxx>
#include <ChFiDS_ChamfMode.hxx>
#include <ChFiDS_CircSection.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <ChFiDS_Stripe.hxx>
#include <NCollection_List.hxx>
#include <ChFiDS_SurfData.hxx>
#include <NCollection_Sequence.hxx>
#include <ChFiDS_ElSpine.hxx>
#include <math_Vector.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopAbs_State.hxx>

class TopoDS_Shape;
class TopoDS_Edge;
class TopoDS_Face;
class ChFiDS_SurfData;
class ChFiDS_Spine;
class Adaptor3d_TopolTool;
class TopoDS_Vertex;
class ChFiDS_Stripe;

//! construction tool for 3D chamfers on edges (on a solid).
class ChFi3d_ChBuilder : public ChFi3d_Builder
{
public:
  DEFINE_STANDARD_ALLOC

  //! initializes the Builder with the Shape <S> for the
  //! computation of chamfers
  Standard_EXPORT ChFi3d_ChBuilder(const TopoDS_Shape& S, const double Ta = 1.0e-2);

  //! initializes a contour with the edge <E> as first
  //! (the next are found by propagation ).
  //! The two distances (parameters of the chamfer) must
  //! be set after.
  //! if the edge <E> has more than 2 adjacent faces
  Standard_EXPORT void Add(const TopoDS_Edge& E);

  //! initializes a new contour with the edge <E> as first
  //! (the next are found by propagation ), and the
  //! distance <Dis>
  //! if the edge <E> has more than 2 adjacent faces
  Standard_EXPORT void Add(const double Dis, const TopoDS_Edge& E);

  //! set the distance <Dis> of the fillet
  //! contour of index <IC> in the DS with <Dis> on <F>.
  //! if the face <F> is not one of common faces
  //! of an edge of the contour <IC>
  Standard_EXPORT void SetDist(const double    Dis,
                               const int IC,
                               const TopoDS_Face&     F);

  //! gives the distances <Dis> of the fillet
  //! contour of index <IC> in the DS
  Standard_EXPORT void GetDist(const int IC, double& Dis) const;

  //! initializes a new contour with the edge <E> as first
  //! (the next are found by propagation ), and the
  //! distance <Dis1> and <Dis2>
  //! if the edge <E> has more than 2 adjacent faces
  Standard_EXPORT void Add(const double Dis1,
                           const double Dis2,
                           const TopoDS_Edge&  E,
                           const TopoDS_Face&  F);

  //! set the distances <Dis1> and <Dis2> of the fillet
  //! contour of index <IC> in the DS with <Dis1> on <F>.
  //! if the face <F> is not one of common faces
  //! of an edge of the contour <IC>
  Standard_EXPORT void SetDists(const double    Dis1,
                                const double    Dis2,
                                const int IC,
                                const TopoDS_Face&     F);

  //! gives the distances <Dis1> and <Dis2> of the fillet
  //! contour of index <IC> in the DS
  Standard_EXPORT void Dists(const int IC,
                             double&         Dis1,
                             double&         Dis2) const;

  //! initializes a new contour with the edge <E> as first
  //! (the next are found by propagation ), and the
  //! distance <Dis1> and <Angle>
  //! if the edge <E> has more than 2 adjacent faces
  Standard_EXPORT void AddDA(const double Dis,
                             const double Angle,
                             const TopoDS_Edge&  E,
                             const TopoDS_Face&  F);

  //! set the distance <Dis> and <Angle> of the fillet
  //! contour of index <IC> in the DS with <Dis> on <F>.
  //! if the face <F> is not one of common faces
  //! of an edge of the contour <IC>
  Standard_EXPORT void SetDistAngle(const double    Dis,
                                    const double    Angle,
                                    const int IC,
                                    const TopoDS_Face&     F);

  //! gives the distances <Dis> and <Angle> of the fillet
  //! contour of index <IC> in the DS
  Standard_EXPORT void GetDistAngle(const int IC,
                                    double&         Dis,
                                    double&         Angle) const;

  //! set the mode of shamfer
  Standard_EXPORT void SetMode(const ChFiDS_ChamfMode theMode);

  //! renvoi la methode des chanfreins utilisee
  Standard_EXPORT ChFiDS_ChamfMethod IsChamfer(const int IC) const;

  //! returns the mode of chamfer used
  Standard_EXPORT ChFiDS_ChamfMode Mode() const;

  //! Reset tous rayons du contour IC.
  Standard_EXPORT void ResetContour(const int IC);

  Standard_EXPORT void Simulate(const int IC);

  Standard_EXPORT int NbSurf(const int IC) const;

  Standard_EXPORT occ::handle<NCollection_HArray1<ChFiDS_CircSection>> Sect(const int IC,
                                                 const int IS) const;

  Standard_EXPORT virtual void SimulSurf(occ::handle<ChFiDS_SurfData>&           Data,
                                         const occ::handle<ChFiDS_ElSpine>&      Guide,
                                         const occ::handle<ChFiDS_Spine>&        Spine,
                                         const int             Choix,
                                         const occ::handle<BRepAdaptor_Surface>& S1,
                                         const occ::handle<Adaptor3d_TopolTool>& I1,
                                         const occ::handle<BRepAdaptor_Curve2d>& PC1,
                                         const occ::handle<BRepAdaptor_Surface>& Sref1,
                                         const occ::handle<BRepAdaptor_Curve2d>& PCref1,
                                         bool&                  Decroch1,
                                         const occ::handle<BRepAdaptor_Surface>& S2,
                                         const occ::handle<Adaptor3d_TopolTool>& I2,
                                         const TopAbs_Orientation           Or2,
                                         const double                Fleche,
                                         const double                TolGuide,
                                         double&                     First,
                                         double&                     Last,
                                         const bool             Inside,
                                         const bool             Appro,
                                         const bool             Forward,
                                         const bool             RecP,
                                         const bool             RecS,
                                         const bool             RecRst,
                                         const math_Vector& Soldep) override;

  Standard_EXPORT virtual void SimulSurf(occ::handle<ChFiDS_SurfData>&           Data,
                                         const occ::handle<ChFiDS_ElSpine>&      Guide,
                                         const occ::handle<ChFiDS_Spine>&        Spine,
                                         const int             Choix,
                                         const occ::handle<BRepAdaptor_Surface>& S1,
                                         const occ::handle<Adaptor3d_TopolTool>& I1,
                                         const TopAbs_Orientation           Or1,
                                         const occ::handle<BRepAdaptor_Surface>& S2,
                                         const occ::handle<Adaptor3d_TopolTool>& I2,
                                         const occ::handle<BRepAdaptor_Curve2d>& PC2,
                                         const occ::handle<BRepAdaptor_Surface>& Sref2,
                                         const occ::handle<BRepAdaptor_Curve2d>& PCref2,
                                         bool&                  Decroch2,
                                         const double                Fleche,
                                         const double                TolGuide,
                                         double&                     First,
                                         double&                     Last,
                                         const bool             Inside,
                                         const bool             Appro,
                                         const bool             Forward,
                                         const bool             RecP,
                                         const bool             RecS,
                                         const bool             RecRst,
                                         const math_Vector& Soldep) override;

  Standard_EXPORT virtual void SimulSurf(occ::handle<ChFiDS_SurfData>&           Data,
                                         const occ::handle<ChFiDS_ElSpine>&      Guide,
                                         const occ::handle<ChFiDS_Spine>&        Spine,
                                         const int             Choix,
                                         const occ::handle<BRepAdaptor_Surface>& S1,
                                         const occ::handle<Adaptor3d_TopolTool>& I1,
                                         const occ::handle<BRepAdaptor_Curve2d>& PC1,
                                         const occ::handle<BRepAdaptor_Surface>& Sref1,
                                         const occ::handle<BRepAdaptor_Curve2d>& PCref1,
                                         bool&                  Decroch1,
                                         const TopAbs_Orientation           Or1,
                                         const occ::handle<BRepAdaptor_Surface>& S2,
                                         const occ::handle<Adaptor3d_TopolTool>& I2,
                                         const occ::handle<BRepAdaptor_Curve2d>& PC2,
                                         const occ::handle<BRepAdaptor_Surface>& Sref2,
                                         const occ::handle<BRepAdaptor_Curve2d>& PCref2,
                                         bool&                  Decroch2,
                                         const TopAbs_Orientation           Or2,
                                         const double                Fleche,
                                         const double                TolGuide,
                                         double&                     First,
                                         double&                     Last,
                                         const bool             Inside,
                                         const bool             Appro,
                                         const bool             Forward,
                                         const bool             RecP1,
                                         const bool             RecRst1,
                                         const bool             RecP2,
                                         const bool             RecRst2,
                                         const math_Vector& Soldep) override;

  //! Methode, implemented in inheritants, calculates
  //! the elements of construction of the surface (fillet
  //! or chamfer).
  Standard_EXPORT virtual bool PerformSurf(NCollection_Sequence<occ::handle<ChFiDS_SurfData>>&         Data,
                                                       const occ::handle<ChFiDS_ElSpine>&      Guide,
                                                       const occ::handle<ChFiDS_Spine>&        Spine,
                                                       const int             Choix,
                                                       const occ::handle<BRepAdaptor_Surface>& S1,
                                                       const occ::handle<Adaptor3d_TopolTool>& I1,
                                                       const occ::handle<BRepAdaptor_Surface>& S2,
                                                       const occ::handle<Adaptor3d_TopolTool>& I2,
                                                       const double                MaxStep,
                                                       const double                Fleche,
                                                       const double                TolGuide,
                                                       double&                     First,
                                                       double&                     Last,
                                                       const bool             Inside,
                                                       const bool             Appro,
                                                       const bool             Forward,
                                                       const bool             RecOnS1,
                                                       const bool             RecOnS2,
                                                       const math_Vector&                 Soldep,
                                                       int&                  Intf,
                                                       int& Intl) override;

  //! Method, implemented in the inheritants, calculates
  //! the elements of construction of the surface (fillet
  //! or chamfer) contact edge/face.
  Standard_EXPORT virtual void PerformSurf(NCollection_Sequence<occ::handle<ChFiDS_SurfData>>&         Data,
                                           const occ::handle<ChFiDS_ElSpine>&      Guide,
                                           const occ::handle<ChFiDS_Spine>&        Spine,
                                           const int             Choix,
                                           const occ::handle<BRepAdaptor_Surface>& S1,
                                           const occ::handle<Adaptor3d_TopolTool>& I1,
                                           const occ::handle<BRepAdaptor_Curve2d>& PC1,
                                           const occ::handle<BRepAdaptor_Surface>& Sref1,
                                           const occ::handle<BRepAdaptor_Curve2d>& PCref1,
                                           bool&                  Decroch1,
                                           const occ::handle<BRepAdaptor_Surface>& S2,
                                           const occ::handle<Adaptor3d_TopolTool>& I2,
                                           const TopAbs_Orientation           Or2,
                                           const double                MaxStep,
                                           const double                Fleche,
                                           const double                TolGuide,
                                           double&                     First,
                                           double&                     Last,
                                           const bool             Inside,
                                           const bool             Appro,
                                           const bool             Forward,
                                           const bool             RecP,
                                           const bool             RecS,
                                           const bool             RecRst,
                                           const math_Vector& Soldep) override;

  //! Method, implemented in inheritants, calculates
  //! the elements of construction of the surface (fillet
  //! or chamfer) contact edge/face.
  Standard_EXPORT virtual void PerformSurf(NCollection_Sequence<occ::handle<ChFiDS_SurfData>>&         Data,
                                           const occ::handle<ChFiDS_ElSpine>&      Guide,
                                           const occ::handle<ChFiDS_Spine>&        Spine,
                                           const int             Choix,
                                           const occ::handle<BRepAdaptor_Surface>& S1,
                                           const occ::handle<Adaptor3d_TopolTool>& I1,
                                           const TopAbs_Orientation           Or1,
                                           const occ::handle<BRepAdaptor_Surface>& S2,
                                           const occ::handle<Adaptor3d_TopolTool>& I2,
                                           const occ::handle<BRepAdaptor_Curve2d>& PC2,
                                           const occ::handle<BRepAdaptor_Surface>& Sref2,
                                           const occ::handle<BRepAdaptor_Curve2d>& PCref2,
                                           bool&                  Decroch2,
                                           const double                MaxStep,
                                           const double                Fleche,
                                           const double                TolGuide,
                                           double&                     First,
                                           double&                     Last,
                                           const bool             Inside,
                                           const bool             Appro,
                                           const bool             Forward,
                                           const bool             RecP,
                                           const bool             RecS,
                                           const bool             RecRst,
                                           const math_Vector& Soldep) override;

  //! Method, implemented in inheritants, calculates
  //! the elements of construction of the surface (fillet
  //! or chamfer) contact edge/edge.
  Standard_EXPORT virtual void PerformSurf(NCollection_Sequence<occ::handle<ChFiDS_SurfData>>&         Data,
                                           const occ::handle<ChFiDS_ElSpine>&      Guide,
                                           const occ::handle<ChFiDS_Spine>&        Spine,
                                           const int             Choix,
                                           const occ::handle<BRepAdaptor_Surface>& S1,
                                           const occ::handle<Adaptor3d_TopolTool>& I1,
                                           const occ::handle<BRepAdaptor_Curve2d>& PC1,
                                           const occ::handle<BRepAdaptor_Surface>& Sref1,
                                           const occ::handle<BRepAdaptor_Curve2d>& PCref1,
                                           bool&                  Decroch1,
                                           const TopAbs_Orientation           Or1,
                                           const occ::handle<BRepAdaptor_Surface>& S2,
                                           const occ::handle<Adaptor3d_TopolTool>& I2,
                                           const occ::handle<BRepAdaptor_Curve2d>& PC2,
                                           const occ::handle<BRepAdaptor_Surface>& Sref2,
                                           const occ::handle<BRepAdaptor_Curve2d>& PCref2,
                                           bool&                  Decroch2,
                                           const TopAbs_Orientation           Or2,
                                           const double                MaxStep,
                                           const double                Fleche,
                                           const double                TolGuide,
                                           double&                     First,
                                           double&                     Last,
                                           const bool             Inside,
                                           const bool             Appro,
                                           const bool             Forward,
                                           const bool             RecP1,
                                           const bool             RecRst1,
                                           const bool             RecP2,
                                           const bool             RecRst2,
                                           const math_Vector& Soldep) override;

protected:
  Standard_EXPORT void SimulKPart(const occ::handle<ChFiDS_SurfData>& SD) const override;

  Standard_EXPORT bool SimulSurf(occ::handle<ChFiDS_SurfData>&           Data,
                                             const occ::handle<ChFiDS_ElSpine>&      Guide,
                                             const occ::handle<ChFiDS_Spine>&        Spine,
                                             const int             Choix,
                                             const occ::handle<BRepAdaptor_Surface>& S1,
                                             const occ::handle<Adaptor3d_TopolTool>& I1,
                                             const occ::handle<BRepAdaptor_Surface>& S2,
                                             const occ::handle<Adaptor3d_TopolTool>& I2,
                                             const double                TolGuide,
                                             double&                     First,
                                             double&                     Last,
                                             const bool             Inside,
                                             const bool             Appro,
                                             const bool             Forward,
                                             const bool             RecOnS1,
                                             const bool             RecOnS2,
                                             const math_Vector&                 Soldep,
                                             int&                  Intf,
                                             int& Intl) override;

  Standard_EXPORT bool PerformFirstSection(const occ::handle<ChFiDS_Spine>&        S,
                                                       const occ::handle<ChFiDS_ElSpine>&      HGuide,
                                                       const int             Choix,
                                                       occ::handle<BRepAdaptor_Surface>&       S1,
                                                       occ::handle<BRepAdaptor_Surface>&       S2,
                                                       const occ::handle<Adaptor3d_TopolTool>& I1,
                                                       const occ::handle<Adaptor3d_TopolTool>& I2,
                                                       const double                Par,
                                                       math_Vector&                       SolDep,
                                                       TopAbs_State&                      Pos1,
                                                       TopAbs_State& Pos2) const override;

  //! computes the intersection of two chamfers on
  //! the vertex of index <Index> in myVDataMap.
  Standard_EXPORT void PerformTwoCorner(const int Index) override;

  //! computes the intersection of three chamfers on
  //! the vertex of index <Index> in myVDataMap.
  Standard_EXPORT void PerformThreeCorner(const int Index) override;

  //! extends the spine of the Stripe <S> at the
  //! extremity of the vertex <V>.
  Standard_EXPORT void ExtentOneCorner(const TopoDS_Vertex&         V,
                                       const occ::handle<ChFiDS_Stripe>& S) override;

  //! extends the spine of the 2 stripes of <LS> at the
  //! extremity of the vertex <V>
  Standard_EXPORT void ExtentTwoCorner(const TopoDS_Vertex&       V,
                                       const NCollection_List<occ::handle<ChFiDS_Stripe>>& LS) override;

  //! extends the spine of the 2 stripes of <LS> at the
  //! extremity of the vertex <V>
  Standard_EXPORT void ExtentThreeCorner(const TopoDS_Vertex&       V,
                                         const NCollection_List<occ::handle<ChFiDS_Stripe>>& LS) override;

  //! set the regularities
  Standard_EXPORT void SetRegul() override;

private:
  Standard_EXPORT void ConexFaces(const occ::handle<ChFiDS_Spine>& Sp,
                                  const int      IEdge,
                                  TopoDS_Face&                F1,
                                  TopoDS_Face&                F2) const;

  ChFiDS_ChamfMode myMode;
};

#endif // _ChFi3d_ChBuilder_HeaderFile
