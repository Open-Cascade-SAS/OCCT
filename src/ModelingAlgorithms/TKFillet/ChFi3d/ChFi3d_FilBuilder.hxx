// Created on: 1995-04-25
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

#ifndef _ChFi3d_FilBuilder_HeaderFile
#define _ChFi3d_FilBuilder_HeaderFile

#include <BRepAdaptor_Surface.hxx>
#include <BlendFunc_SectionShape.hxx>
#include <ChFi3d_Builder.hxx>
#include <ChFi3d_FilletShape.hxx>
#include <ChFiDS_ListOfStripe.hxx>
#include <ChFiDS_ElSpine.hxx>
#include <ChFiDS_SecHArray1.hxx>
#include <ChFiDS_SequenceOfSurfData.hxx>
#include <math_Vector.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopAbs_State.hxx>

class Adaptor3d_TopolTool;
class BRepBlend_Line;
class gp_XY;
class ChFiDS_SurfData;
class ChFiDS_Spine;
class ChFiDS_Stripe;
class Law_Function;
class TopoDS_Edge;
class TopoDS_Shape;
class TopoDS_Vertex;

//! Tool  of  construction of  fillets 3d on  edges (on a solid).
class ChFi3d_FilBuilder : public ChFi3d_Builder
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT ChFi3d_FilBuilder(const TopoDS_Shape&      S,
                                    const ChFi3d_FilletShape FShape = ChFi3d_Rational,
                                    const Standard_Real      Ta     = 1.0e-2);

  //! Sets the type of fillet surface.
  Standard_EXPORT void SetFilletShape(const ChFi3d_FilletShape FShape);

  //! Returns the type of fillet surface.
  Standard_EXPORT ChFi3d_FilletShape GetFilletShape() const;

  //! initialisation of  a contour with the first edge
  //! (the following are found  by propagation).
  //! Attention, you  need  to start  with  SetRadius.
  Standard_EXPORT void Add(const TopoDS_Edge& E);

  //! initialisation of the constant vector the corresponding  1st  edge.
  Standard_EXPORT void Add(const Standard_Real Radius, const TopoDS_Edge& E);

  //! Set the radius of the contour of index IC.
  Standard_EXPORT void SetRadius(const Handle(Law_Function)& C,
                                 const Standard_Integer      IC,
                                 const Standard_Integer      IinC);

  //! Returns true the contour is flagged as edge constant.
  Standard_EXPORT Standard_Boolean IsConstant(const Standard_Integer IC);

  //! Returns the vector if the contour is flagged as edge
  //! constant.
  Standard_EXPORT Standard_Real Radius(const Standard_Integer IC);

  //! Reset all vectors of contour IC.
  Standard_EXPORT void ResetContour(const Standard_Integer IC);

  //! Set a constant on edge E of  the contour of
  //! index IC. Since  then  E is flagged as constant.
  Standard_EXPORT void SetRadius(const Standard_Real    Radius,
                                 const Standard_Integer IC,
                                 const TopoDS_Edge&     E);

  //! Extracts the flag constant and the vector of edge E.
  Standard_EXPORT void UnSet(const Standard_Integer IC, const TopoDS_Edge& E);

  //! Set a vector on vertex  V of  the contour of index IC.
  Standard_EXPORT void SetRadius(const Standard_Real    Radius,
                                 const Standard_Integer IC,
                                 const TopoDS_Vertex&   V);

  //! Extracts the vector of  the vertex V.
  Standard_EXPORT void UnSet(const Standard_Integer IC, const TopoDS_Vertex& V);

  //! Set  a vertex on the point of parametre U in the edge IinC
  //! of  the contour of index IC
  Standard_EXPORT void SetRadius(const gp_XY&           UandR,
                                 const Standard_Integer IC,
                                 const Standard_Integer IinC);

  //! Returns true E is flagged as edge constant.
  Standard_EXPORT Standard_Boolean IsConstant(const Standard_Integer IC, const TopoDS_Edge& E);

  //! Returns the vector if E is flagged as edge constant.
  Standard_EXPORT Standard_Real Radius(const Standard_Integer IC, const TopoDS_Edge& E);

  //! Returns in First and Last  les extremities of  the
  //! part of variable  vector framing E, returns
  //! False  if  E is flagged as edge constant.
  Standard_EXPORT Standard_Boolean GetBounds(const Standard_Integer IC,
                                             const TopoDS_Edge&     E,
                                             Standard_Real&         First,
                                             Standard_Real&         Last);

  //! Returns the rule of  elementary  evolution of  the
  //! part to  variable vector framing E, returns a
  //! rule zero if E is flagged as edge constant.
  Standard_EXPORT Handle(Law_Function) GetLaw(const Standard_Integer IC, const TopoDS_Edge& E);

  //! Sets the rule of elementary evolution of  the
  //! part to variable  vector framing E.
  Standard_EXPORT void SetLaw(const Standard_Integer      IC,
                              const TopoDS_Edge&          E,
                              const Handle(Law_Function)& L);

  Standard_EXPORT void Simulate(const Standard_Integer IC);

  Standard_EXPORT Standard_Integer NbSurf(const Standard_Integer IC) const;

  Standard_EXPORT Handle(ChFiDS_SecHArray1) Sect(const Standard_Integer IC,
                                                 const Standard_Integer IS) const;

protected:
  Standard_EXPORT void SimulKPart(const Handle(ChFiDS_SurfData)& SD) const Standard_OVERRIDE;

  Standard_EXPORT Standard_Boolean SimulSurf(Handle(ChFiDS_SurfData)&           Data,
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
                                             Standard_Integer& Intl) Standard_OVERRIDE;

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
                                         const math_Vector& Soldep) Standard_OVERRIDE;

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
                                         const math_Vector& Soldep) Standard_OVERRIDE;

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
                                         const math_Vector& Soldep) Standard_OVERRIDE;

  Standard_EXPORT Standard_Boolean PerformFirstSection(const Handle(ChFiDS_Spine)&        S,
                                                       const Handle(ChFiDS_ElSpine)&      HGuide,
                                                       const Standard_Integer             Choix,
                                                       Handle(BRepAdaptor_Surface)&       S1,
                                                       Handle(BRepAdaptor_Surface)&       S2,
                                                       const Handle(Adaptor3d_TopolTool)& I1,
                                                       const Handle(Adaptor3d_TopolTool)& I2,
                                                       const Standard_Real                Par,
                                                       math_Vector&                       SolDep,
                                                       TopAbs_State&                      Pos1,
                                                       TopAbs_State& Pos2) const Standard_OVERRIDE;

  //! Method calculates the elements of construction of  the
  //! fillet (constant or evolutive).
  Standard_EXPORT Standard_Boolean PerformSurf(ChFiDS_SequenceOfSurfData&         SeqData,
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
                                               Standard_Integer& Intl) Standard_OVERRIDE;

  Standard_EXPORT virtual void PerformSurf(ChFiDS_SequenceOfSurfData&         SeqData,
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
                                           const math_Vector& Soldep) Standard_OVERRIDE;

  Standard_EXPORT virtual void PerformSurf(ChFiDS_SequenceOfSurfData&         SeqData,
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
                                           const math_Vector& Soldep) Standard_OVERRIDE;

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
                                           const math_Vector& Soldep) Standard_OVERRIDE;

  //! Method to split an singular SurfData  in  several  non
  //! singular  SurfData..
  Standard_EXPORT void SplitSurf(ChFiDS_SequenceOfSurfData&    SeqData,
                                 const Handle(BRepBlend_Line)& line);

  Standard_EXPORT void PerformTwoCorner(const Standard_Integer Index) Standard_OVERRIDE;

  Standard_EXPORT void PerformThreeCorner(const Standard_Integer Index) Standard_OVERRIDE;

  Standard_EXPORT void ExtentOneCorner(const TopoDS_Vertex&         V,
                                       const Handle(ChFiDS_Stripe)& S) Standard_OVERRIDE;

  Standard_EXPORT void ExtentTwoCorner(const TopoDS_Vertex&       V,
                                       const ChFiDS_ListOfStripe& LS) Standard_OVERRIDE;

  Standard_EXPORT void ExtentThreeCorner(const TopoDS_Vertex&       V,
                                         const ChFiDS_ListOfStripe& LS) Standard_OVERRIDE;

  Standard_EXPORT void SetRegul() Standard_OVERRIDE;

private:
  BlendFunc_SectionShape myShape;
};

#endif // _ChFi3d_FilBuilder_HeaderFile
