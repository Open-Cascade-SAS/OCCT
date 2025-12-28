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

#ifndef _BRepBlend_SurfRstLineBuilder_HeaderFile
#define _BRepBlend_SurfRstLineBuilder_HeaderFile

#include <Adaptor3d_Surface.hxx>
#include <math_Vector.hxx>
#include <Blend_Point.hxx>
#include <Blend_Status.hxx>
#include <TopAbs_State.hxx>

class BRepBlend_Line;
class Adaptor3d_TopolTool;
class Blend_SurfRstFunction;
class Blend_FuncInv;
class Blend_SurfPointFuncInv;
class Blend_SurfCurvFuncInv;
class gp_Pnt2d;
class Adaptor3d_HVertex;
class IntSurf_Transition;
class BRepBlend_Extremity;

//! This class processes data resulting from
//! Blend_CSWalking taking in consideration the Surface
//! supporting the curve to detect the breakpoint.
//!
//! The criteria of distribution of points on the line are detailed
//! because it is to be used in the calculatuon of values approached
//! by an approximation of functions continued basing on
//! Blend_SurfRstFunction.
//!
//! Thus this pseudo path necessitates 3 criteria of regrouping:
//!
//! 1) exit of the domain of the curve
//!
//! 2) exit of the domain of the surface
//!
//! 3) stall as there is a solution to the problem
//! surf/surf within the domain of the surface
//! of support of the restriction.
//!
//! Construction of a BRepBlend_Line between a surface and
//! a pcurve on surface from an approached
//! starting solution. The output entries of this builder
//! are of the same nature as of the traditional walking
//! but the requirements on the Line are not the same
//! If the determination of validity range is always
//! guaranteed, the criteria of correct repartition of sections
//! before smoothing are not respected. The resulting Line
//! is f(t) oriented.
class BRepBlend_SurfRstLineBuilder
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT BRepBlend_SurfRstLineBuilder(const occ::handle<Adaptor3d_Surface>&   Surf1,
                                               const occ::handle<Adaptor3d_TopolTool>& Domain1,
                                               const occ::handle<Adaptor3d_Surface>&   Surf2,
                                               const occ::handle<Adaptor2d_Curve2d>&   Rst,
                                               const occ::handle<Adaptor3d_TopolTool>& Domain2);

  Standard_EXPORT void Perform(Blend_SurfRstFunction&  Func,
                               Blend_FuncInv&          Finv,
                               Blend_SurfPointFuncInv& FinvP,
                               Blend_SurfCurvFuncInv&  FinvC,
                               const double     Pdep,
                               const double     Pmax,
                               const double     MaxStep,
                               const double     Tol3d,
                               const double     Tol2d,
                               const double     TolGuide,
                               const math_Vector&      Soldep,
                               const double     Fleche,
                               const bool  Appro = false);

  Standard_EXPORT bool PerformFirstSection(Blend_SurfRstFunction&  Func,
                                                       Blend_FuncInv&          Finv,
                                                       Blend_SurfPointFuncInv& FinvP,
                                                       Blend_SurfCurvFuncInv&  FinvC,
                                                       const double     Pdep,
                                                       const double     Pmax,
                                                       const math_Vector&      Soldep,
                                                       const double     Tol3d,
                                                       const double     Tol2d,
                                                       const double     TolGuide,
                                                       const bool  RecRst,
                                                       const bool  RecP,
                                                       const bool  RecS,
                                                       double&          Psol,
                                                       math_Vector&            ParSol);

  Standard_EXPORT bool Complete(Blend_SurfRstFunction&  Func,
                                            Blend_FuncInv&          Finv,
                                            Blend_SurfPointFuncInv& FinvP,
                                            Blend_SurfCurvFuncInv&  FinvC,
                                            const double     Pmin);

  Standard_EXPORT int ArcToRecadre(const math_Vector&     Sol,
                                                const int PrevIndex,
                                                gp_Pnt2d&              pt2d,
                                                gp_Pnt2d&              lastpt2d,
                                                double&         ponarc);

  bool IsDone() const;

  const occ::handle<BRepBlend_Line>& Line() const;

  bool DecrochStart() const;

  bool DecrochEnd() const;

private:
  Standard_EXPORT void InternalPerform(Blend_SurfRstFunction&  Func,
                                       Blend_FuncInv&          Finv,
                                       Blend_SurfPointFuncInv& FinvP,
                                       Blend_SurfCurvFuncInv&  FinvC,
                                       const double     Bound);

  Standard_EXPORT bool Recadre(Blend_SurfCurvFuncInv&     FinvC,
                                           math_Vector&               Solinv,
                                           occ::handle<Adaptor2d_Curve2d>& Arc,
                                           bool&          IsVtx,
                                           occ::handle<Adaptor3d_HVertex>& Vtx);

  Standard_EXPORT bool Recadre(Blend_SurfRstFunction&     Func,
                                           Blend_FuncInv&             Finv,
                                           math_Vector&               Solinv,
                                           bool&          IsVtx,
                                           occ::handle<Adaptor3d_HVertex>& Vtx);

  Standard_EXPORT bool Recadre(Blend_SurfPointFuncInv&    FinvP,
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

  Standard_EXPORT Blend_Status CheckDeflectionOnSurf(const Blend_Point& CurPoint);

  Standard_EXPORT Blend_Status CheckDeflectionOnRst(const Blend_Point& CurPoint);

  Standard_EXPORT Blend_Status TestArret(Blend_SurfRstFunction& Func,
                                         const bool TestDeflection,
                                         const Blend_Status     State);

  Standard_EXPORT bool CheckInside(Blend_SurfRstFunction& Func,
                                               TopAbs_State&          SituOnC,
                                               TopAbs_State&          SituOnS,
                                               bool&      Decroch);

  bool            done;
  occ::handle<BRepBlend_Line>      line;
  math_Vector                 sol;
  occ::handle<Adaptor3d_Surface>   surf1;
  occ::handle<Adaptor3d_TopolTool> domain1;
  occ::handle<Adaptor3d_Surface>   surf2;
  occ::handle<Adaptor2d_Curve2d>   rst;
  occ::handle<Adaptor3d_TopolTool> domain2;
  double               tolpoint3d;
  double               tolpoint2d;
  double               tolgui;
  double               pasmax;
  double               fleche;
  double               param;
  Blend_Point                 previousP;
  bool            rebrou;
  bool            iscomplete;
  bool            comptra;
  double               sens;
  bool            decrochdeb;
  bool            decrochfin;
};

#include <BRepBlend_SurfRstLineBuilder.lxx>

#endif // _BRepBlend_SurfRstLineBuilder_HeaderFile
