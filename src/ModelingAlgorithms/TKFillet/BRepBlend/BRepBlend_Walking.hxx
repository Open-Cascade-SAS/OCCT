// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2024 OPEN CASCADE SAS
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

#ifndef _BRepBlend_Walking_HeaderFile
#define _BRepBlend_Walking_HeaderFile

#include <Adaptor3d_Surface.hxx>
#include <BRepBlend_SequenceOfPointOnRst.hxx>
#include <Blend_Point.hxx>
#include <Blend_SequenceOfPoint.hxx>
#include <Blend_Status.hxx>
#include <ChFiDS_ElSpine.hxx>
#include <StdFail_NotDone.hxx>
#include <TopAbs_State.hxx>
#include <math_Vector.hxx>

class BRepBlend_Line;
class Adaptor3d_TopolTool;
class StdFail_NotDone;
class Adaptor3d_HVertex;
class BRepBlend_HCurve2dTool;
class Adaptor3d_HSurfaceTool;
class BRepBlend_HCurveTool;
class BRepBlend_BlendTool;
class BRepBlend_PointOnRst;
class BRepBlend_Extremity;
class Blend_Point;
class Blend_Function;
class Blend_FuncInv;
class gp_Pnt;
class gp_Pnt2d;
class IntSurf_Transition;

class BRepBlend_Walking
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT BRepBlend_Walking(const Handle(Adaptor3d_Surface)&   Surf1,
                                    const Handle(Adaptor3d_Surface)&   Surf2,
                                    const Handle(Adaptor3d_TopolTool)& Domain1,
                                    const Handle(Adaptor3d_TopolTool)& Domain2,
                                    const Handle(ChFiDS_ElSpine)&      HGuide);

  //! To define different domains for control and clipping.
  Standard_EXPORT void SetDomainsToRecadre(const Handle(Adaptor3d_TopolTool)& RecDomain1,
                                           const Handle(Adaptor3d_TopolTool)& RecDomain2);

  //! To define singular points computed before walking.
  Standard_EXPORT void AddSingularPoint(const Blend_Point& P);

  Standard_EXPORT void Perform(Blend_Function&        F,
                               Blend_FuncInv&         FInv,
                               const Standard_Real    Pdep,
                               const Standard_Real    Pmax,
                               const Standard_Real    MaxStep,
                               const Standard_Real    Tol3d,
                               const Standard_Real    TolGuide,
                               const math_Vector&     Soldep,
                               const Standard_Real    Fleche,
                               const Standard_Boolean Appro = Standard_False);

  Standard_EXPORT Standard_Boolean PerformFirstSection(Blend_Function&     F,
                                                       const Standard_Real Pdep,
                                                       math_Vector&        ParDep,
                                                       const Standard_Real Tol3d,
                                                       const Standard_Real TolGuide,
                                                       TopAbs_State&       Pos1,
                                                       TopAbs_State&       Pos2);

  Standard_EXPORT Standard_Boolean PerformFirstSection(Blend_Function&        F,
                                                       Blend_FuncInv&         FInv,
                                                       const Standard_Real    Pdep,
                                                       const Standard_Real    Pmax,
                                                       const math_Vector&     ParDep,
                                                       const Standard_Real    Tol3d,
                                                       const Standard_Real    TolGuide,
                                                       const Standard_Boolean RecOnS1,
                                                       const Standard_Boolean RecOnS2,
                                                       Standard_Real&         Psol,
                                                       math_Vector&           ParSol);

  Standard_EXPORT Standard_Boolean Continu(Blend_Function&     F,
                                           Blend_FuncInv&      FInv,
                                           const Standard_Real P);

  Standard_EXPORT Standard_Boolean Continu(Blend_Function&        F,
                                           Blend_FuncInv&         FInv,
                                           const Standard_Real    P,
                                           const Standard_Boolean OnS1);

  Standard_EXPORT Standard_Boolean Complete(Blend_Function&     F,
                                            Blend_FuncInv&      FInv,
                                            const Standard_Real Pmin);

  Standard_EXPORT void ClassificationOnS1(const Standard_Boolean C);

  Standard_EXPORT void ClassificationOnS2(const Standard_Boolean C);

  Standard_EXPORT void Check2d(const Standard_Boolean C);

  Standard_EXPORT void Check(const Standard_Boolean C);

  inline Standard_Boolean TwistOnS1() const { return twistflag1; }

  inline Standard_Boolean TwistOnS2() const { return twistflag2; }

  inline Standard_Boolean IsDone() const { return done; }

  inline const Handle(BRepBlend_Line)& Line() const
  {
    if (!done)
    {
      throw StdFail_NotDone();
    }
    return line;
  }

private:
  Standard_EXPORT void InternalPerform(Blend_Function&     F,
                                       Blend_FuncInv&      FInv,
                                       const Standard_Real Bound);

  Standard_EXPORT Standard_Boolean CorrectExtremityOnOneRst(const Standard_Integer IndexOfRst,
                                                            const Standard_Real    theU,
                                                            const Standard_Real    theV,
                                                            const Standard_Real    theParam,
                                                            const gp_Pnt&          thePntOnRst,
                                                            Standard_Real&         NewU,
                                                            Standard_Real&         NewV,
                                                            gp_Pnt&                NewPoint,
                                                            Standard_Real&         NewParam) const;

  Standard_EXPORT Standard_Integer ArcToRecadre(const Standard_Boolean OnFirst,
                                                const math_Vector&     Sol,
                                                const Standard_Integer PrevIndex,
                                                gp_Pnt2d&              lpt2d,
                                                gp_Pnt2d&              pt2d,
                                                Standard_Real&         ponarc);

  Standard_EXPORT Standard_Boolean Recadre(Blend_FuncInv&             FInv,
                                           const Standard_Boolean     OnFirst,
                                           const math_Vector&         Sol,
                                           math_Vector&               Solrst,
                                           Standard_Integer&          Indexsol,
                                           Standard_Boolean&          IsVtx,
                                           Handle(Adaptor3d_HVertex)& Vtx,
                                           const Standard_Real        Extrap = 0.0);

  Standard_EXPORT void Transition(const Standard_Boolean           OnFirst,
                                  const Handle(Adaptor2d_Curve2d)& A,
                                  const Standard_Real              Param,
                                  IntSurf_Transition&              TLine,
                                  IntSurf_Transition&              TArc);

  Standard_EXPORT void MakeExtremity(BRepBlend_Extremity&             Extrem,
                                     const Standard_Boolean           OnFirst,
                                     const Standard_Integer           Index,
                                     const Standard_Real              Param,
                                     const Standard_Boolean           IsVtx,
                                     const Handle(Adaptor3d_HVertex)& Vtx);

  Standard_EXPORT void MakeSingularExtremity(BRepBlend_Extremity&             Extrem,
                                             const Standard_Boolean           OnFirst,
                                             const Handle(Adaptor3d_HVertex)& Vtx);

  Standard_EXPORT Blend_Status CheckDeflection(const Standard_Boolean OnFirst,
                                               const Blend_Point&     CurPoint);

  Standard_EXPORT Blend_Status TestArret(Blend_Function&        F,
                                         const Blend_Status     State,
                                         const Standard_Boolean TestDeflection = Standard_True,
                                         const Standard_Boolean TestSolution   = Standard_True,
                                         const Standard_Boolean TestLengthStep = Standard_False);

  Blend_Point                 previousP;
  Handle(BRepBlend_Line)      line;
  math_Vector                 sol;
  Blend_SequenceOfPoint       jalons;
  Handle(Adaptor3d_Surface)   surf1;
  Handle(Adaptor3d_Surface)   surf2;
  Handle(Adaptor3d_TopolTool) domain1;
  Handle(Adaptor3d_TopolTool) domain2;
  Handle(Adaptor3d_TopolTool) recdomain1;
  Handle(Adaptor3d_TopolTool) recdomain2;
  Handle(ChFiDS_ElSpine)      hguide;
  Standard_Boolean            ToCorrectOnRst1;
  Standard_Boolean            ToCorrectOnRst2;
  Standard_Real               CorrectedParam;
  Standard_Real               tolpoint3d;
  Standard_Real               tolgui;
  Standard_Real               pasmax;
  Standard_Real               fleche;
  Standard_Real               param;
  Standard_Real               sens;
  Standard_Boolean            done;
  Standard_Boolean            rebrou;
  Standard_Boolean            iscomplete;
  Standard_Boolean            comptra;
  Standard_Boolean            clasonS1;
  Standard_Boolean            clasonS2;
  Standard_Boolean            check2d;
  Standard_Boolean            check;
  Standard_Boolean            twistflag1;
  Standard_Boolean            twistflag2;
};

#endif // _BRepBlend_Walking_HeaderFile
