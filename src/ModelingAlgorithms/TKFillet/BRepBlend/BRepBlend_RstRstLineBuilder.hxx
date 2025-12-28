// Created on: 1997-01-23
// Created by: Laurent BOURESCHE
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _BRepBlend_RstRstLineBuilder_HeaderFile
#define _BRepBlend_RstRstLineBuilder_HeaderFile

#include <Adaptor3d_Surface.hxx>
#include <math_Vector.hxx>
#include <Blend_Point.hxx>
#include <Blend_DecrochStatus.hxx>
#include <Blend_Status.hxx>
#include <TopAbs_State.hxx>

class BRepBlend_Line;
class Adaptor3d_TopolTool;
class Blend_RstRstFunction;
class Blend_SurfCurvFuncInv;
class Blend_CurvPointFuncInv;
class Adaptor3d_HVertex;
class IntSurf_Transition;
class BRepBlend_Extremity;

//! This class processes the data resulting from
//! Blend_CSWalking but it takes in consideration the Surface
//! supporting the curve to detect the breakpoint.
//!
//! As a result, the criteria of distribution of
//! points on the line become more flexible because it
//! should calculate values approached
//! by an approximation of continued functions based on the
//! Blend_RstRstFunction.
//!
//! Thus this pseudo path necessitates 3 criteria of
//! regrouping:
//!
//! 1) exit of the domain of the curve
//!
//! 2) exit of the domain of the surface
//!
//! 3) stall as there is a solution of problem
//! surf/surf within the domain of the surface
//! of support of the restriction.
//!
//! Construction of a BRepBlend_Line between two pcurves
//! from an approached starting solution. The output
//! entries of this builder are of the same nature
//! as of a traditional walking, but the requirements
//! to the Line are not the same. If the determination of validity range is always
//! guaranteed, the criteria of correct repartition of sections
//! before smoothing are not respected. The resulting Line
//! is f(t) oriented.
class BRepBlend_RstRstLineBuilder
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT BRepBlend_RstRstLineBuilder(const occ::handle<Adaptor3d_Surface>&   Surf1,
                                              const occ::handle<Adaptor2d_Curve2d>&   Rst1,
                                              const occ::handle<Adaptor3d_TopolTool>& Domain1,
                                              const occ::handle<Adaptor3d_Surface>&   Surf2,
                                              const occ::handle<Adaptor2d_Curve2d>&   Rst2,
                                              const occ::handle<Adaptor3d_TopolTool>& Domain2);

  Standard_EXPORT void Perform(Blend_RstRstFunction&   Func,
                               Blend_SurfCurvFuncInv&  Finv1,
                               Blend_CurvPointFuncInv& FinvP1,
                               Blend_SurfCurvFuncInv&  Finv2,
                               Blend_CurvPointFuncInv& FinvP2,
                               const double     Pdep,
                               const double     Pmax,
                               const double     MaxStep,
                               const double     Tol3d,
                               const double     TolGuide,
                               const math_Vector&      Soldep,
                               const double     Fleche,
                               const bool  Appro = false);

  Standard_EXPORT bool PerformFirstSection(Blend_RstRstFunction&   Func,
                                                       Blend_SurfCurvFuncInv&  Finv1,
                                                       Blend_CurvPointFuncInv& FinvP1,
                                                       Blend_SurfCurvFuncInv&  Finv2,
                                                       Blend_CurvPointFuncInv& FinvP2,
                                                       const double     Pdep,
                                                       const double     Pmax,
                                                       const math_Vector&      Soldep,
                                                       const double     Tol3d,
                                                       const double     TolGuide,
                                                       const bool  RecRst1,
                                                       const bool  RecP1,
                                                       const bool  RecRst2,
                                                       const bool  RecP2,
                                                       double&          Psol,
                                                       math_Vector&            ParSol);

  Standard_EXPORT bool Complete(Blend_RstRstFunction&   Func,
                                            Blend_SurfCurvFuncInv&  Finv1,
                                            Blend_CurvPointFuncInv& FinvP1,
                                            Blend_SurfCurvFuncInv&  Finv2,
                                            Blend_CurvPointFuncInv& FinvP2,
                                            const double     Pmin);

  bool IsDone() const;

  const occ::handle<BRepBlend_Line>& Line() const;

  bool Decroch1Start() const;

  bool Decroch1End() const;

  bool Decroch2Start() const;

  bool Decroch2End() const;

private:
  Standard_EXPORT void InternalPerform(Blend_RstRstFunction&   Func,
                                       Blend_SurfCurvFuncInv&  Finv1,
                                       Blend_CurvPointFuncInv& FinvP1,
                                       Blend_SurfCurvFuncInv&  Finv2,
                                       Blend_CurvPointFuncInv& FinvP2,
                                       const double     Bound);

  Standard_EXPORT bool Recadre1(Blend_RstRstFunction&      Func,
                                            Blend_SurfCurvFuncInv&     Finv,
                                            math_Vector&               Solinv,
                                            bool&          IsVtx,
                                            occ::handle<Adaptor3d_HVertex>& Vtx);

  Standard_EXPORT bool Recadre2(Blend_RstRstFunction&      Func,
                                            Blend_SurfCurvFuncInv&     Finv,
                                            math_Vector&               Solinv,
                                            bool&          IsVtx,
                                            occ::handle<Adaptor3d_HVertex>& Vtx);

  Standard_EXPORT bool Recadre1(Blend_CurvPointFuncInv&    FinvP,
                                            math_Vector&               Solinv,
                                            bool&          IsVtx,
                                            occ::handle<Adaptor3d_HVertex>& Vtx);

  Standard_EXPORT bool Recadre2(Blend_CurvPointFuncInv&    FinvP,
                                            math_Vector&               Solinv,
                                            bool&          IsVtx,
                                            occ::handle<Adaptor3d_HVertex>& Vtx);

  Standard_EXPORT void Transition(const bool           OnFirst,
                                  const occ::handle<Adaptor2d_Curve2d>& Arc,
                                  const double              Param,
                                  IntSurf_Transition&              TLine,
                                  IntSurf_Transition&              TArc);

  Standard_EXPORT void MakeExtremity(BRepBlend_Extremity&             Extrem,
                                     const bool           OnFirst,
                                     const occ::handle<Adaptor2d_Curve2d>& Arc,
                                     const double              Param,
                                     const bool           IsVtx,
                                     const occ::handle<Adaptor3d_HVertex>& Vtx);

  Standard_EXPORT Blend_Status CheckDeflectionOnRst1(const Blend_Point& CurPoint);

  Standard_EXPORT Blend_Status CheckDeflectionOnRst2(const Blend_Point& CurPoint);

  Standard_EXPORT Blend_Status TestArret(Blend_RstRstFunction&  Func,
                                         const bool TestDeflection,
                                         const Blend_Status     State);

  Standard_EXPORT bool CheckInside(Blend_RstRstFunction& Func,
                                               TopAbs_State&         SituOnC1,
                                               TopAbs_State&         SituOnC2,
                                               Blend_DecrochStatus&  Decroch);

  bool            done;
  occ::handle<BRepBlend_Line>      line;
  math_Vector                 sol;
  occ::handle<Adaptor3d_Surface>   surf1;
  occ::handle<Adaptor3d_TopolTool> domain1;
  occ::handle<Adaptor3d_Surface>   surf2;
  occ::handle<Adaptor3d_TopolTool> domain2;
  occ::handle<Adaptor2d_Curve2d>   rst1;
  occ::handle<Adaptor2d_Curve2d>   rst2;
  double               tolpoint3d;
  double               tolgui;
  double               pasmax;
  double               fleche;
  double               param;
  Blend_Point                 previousP;
  bool            rebrou;
  bool            iscomplete;
  bool            comptra;
  double               sens;
  Blend_DecrochStatus         decrochdeb;
  Blend_DecrochStatus         decrochfin;
};

#include <BRepBlend_RstRstLineBuilder.lxx>

#endif // _BRepBlend_RstRstLineBuilder_HeaderFile
