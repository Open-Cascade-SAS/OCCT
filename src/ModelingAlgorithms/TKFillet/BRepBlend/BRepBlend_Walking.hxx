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
#include <BRepBlend_PointOnRst.hxx>
#include <NCollection_Sequence.hxx>
#include <Blend_Point.hxx>
#include <Blend_Point.hxx>
#include <NCollection_Sequence.hxx>
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

  Standard_EXPORT BRepBlend_Walking(const occ::handle<Adaptor3d_Surface>&   Surf1,
                                    const occ::handle<Adaptor3d_Surface>&   Surf2,
                                    const occ::handle<Adaptor3d_TopolTool>& Domain1,
                                    const occ::handle<Adaptor3d_TopolTool>& Domain2,
                                    const occ::handle<ChFiDS_ElSpine>&      HGuide);

  //! To define different domains for control and clipping.
  Standard_EXPORT void SetDomainsToRecadre(const occ::handle<Adaptor3d_TopolTool>& RecDomain1,
                                           const occ::handle<Adaptor3d_TopolTool>& RecDomain2);

  //! To define singular points computed before walking.
  Standard_EXPORT void AddSingularPoint(const Blend_Point& P);

  Standard_EXPORT void Perform(Blend_Function&        F,
                               Blend_FuncInv&         FInv,
                               const double    Pdep,
                               const double    Pmax,
                               const double    MaxStep,
                               const double    Tol3d,
                               const double    TolGuide,
                               const math_Vector&     Soldep,
                               const double    Fleche,
                               const bool Appro = false);

  Standard_EXPORT bool PerformFirstSection(Blend_Function&     F,
                                                       const double Pdep,
                                                       math_Vector&        ParDep,
                                                       const double Tol3d,
                                                       const double TolGuide,
                                                       TopAbs_State&       Pos1,
                                                       TopAbs_State&       Pos2);

  Standard_EXPORT bool PerformFirstSection(Blend_Function&        F,
                                                       Blend_FuncInv&         FInv,
                                                       const double    Pdep,
                                                       const double    Pmax,
                                                       const math_Vector&     ParDep,
                                                       const double    Tol3d,
                                                       const double    TolGuide,
                                                       const bool RecOnS1,
                                                       const bool RecOnS2,
                                                       double&         Psol,
                                                       math_Vector&           ParSol);

  Standard_EXPORT bool Continu(Blend_Function&     F,
                                           Blend_FuncInv&      FInv,
                                           const double P);

  Standard_EXPORT bool Continu(Blend_Function&        F,
                                           Blend_FuncInv&         FInv,
                                           const double    P,
                                           const bool OnS1);

  Standard_EXPORT bool Complete(Blend_Function&     F,
                                            Blend_FuncInv&      FInv,
                                            const double Pmin);

  Standard_EXPORT void ClassificationOnS1(const bool C);

  Standard_EXPORT void ClassificationOnS2(const bool C);

  Standard_EXPORT void Check2d(const bool C);

  Standard_EXPORT void Check(const bool C);

  inline bool TwistOnS1() const { return twistflag1; }

  inline bool TwistOnS2() const { return twistflag2; }

  inline bool IsDone() const { return done; }

  inline const occ::handle<BRepBlend_Line>& Line() const
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
                                       const double Bound);

  Standard_EXPORT bool CorrectExtremityOnOneRst(const int IndexOfRst,
                                                            const double    theU,
                                                            const double    theV,
                                                            const double    theParam,
                                                            const gp_Pnt&          thePntOnRst,
                                                            double&         NewU,
                                                            double&         NewV,
                                                            gp_Pnt&                NewPoint,
                                                            double&         NewParam) const;

  Standard_EXPORT int ArcToRecadre(const bool OnFirst,
                                                const math_Vector&     Sol,
                                                const int PrevIndex,
                                                gp_Pnt2d&              lpt2d,
                                                gp_Pnt2d&              pt2d,
                                                double&         ponarc);

  Standard_EXPORT bool Recadre(Blend_FuncInv&             FInv,
                                           const bool     OnFirst,
                                           const math_Vector&         Sol,
                                           math_Vector&               Solrst,
                                           int&          Indexsol,
                                           bool&          IsVtx,
                                           occ::handle<Adaptor3d_HVertex>& Vtx,
                                           const double        Extrap = 0.0);

  Standard_EXPORT void Transition(const bool           OnFirst,
                                  const occ::handle<Adaptor2d_Curve2d>& A,
                                  const double              Param,
                                  IntSurf_Transition&              TLine,
                                  IntSurf_Transition&              TArc);

  Standard_EXPORT void MakeExtremity(BRepBlend_Extremity&             Extrem,
                                     const bool           OnFirst,
                                     const int           Index,
                                     const double              Param,
                                     const bool           IsVtx,
                                     const occ::handle<Adaptor3d_HVertex>& Vtx);

  Standard_EXPORT void MakeSingularExtremity(BRepBlend_Extremity&             Extrem,
                                             const bool           OnFirst,
                                             const occ::handle<Adaptor3d_HVertex>& Vtx);

  Standard_EXPORT Blend_Status CheckDeflection(const bool OnFirst,
                                               const Blend_Point&     CurPoint);

  Standard_EXPORT Blend_Status TestArret(Blend_Function&        F,
                                         const Blend_Status     State,
                                         const bool TestDeflection = true,
                                         const bool TestSolution   = true,
                                         const bool TestLengthStep = false);

  Blend_Point                 previousP;
  occ::handle<BRepBlend_Line>      line;
  math_Vector                 sol;
  NCollection_Sequence<Blend_Point>       jalons;
  occ::handle<Adaptor3d_Surface>   surf1;
  occ::handle<Adaptor3d_Surface>   surf2;
  occ::handle<Adaptor3d_TopolTool> domain1;
  occ::handle<Adaptor3d_TopolTool> domain2;
  occ::handle<Adaptor3d_TopolTool> recdomain1;
  occ::handle<Adaptor3d_TopolTool> recdomain2;
  occ::handle<ChFiDS_ElSpine>      hguide;
  bool            ToCorrectOnRst1;
  bool            ToCorrectOnRst2;
  double               CorrectedParam;
  double               tolpoint3d;
  double               tolgui;
  double               pasmax;
  double               fleche;
  double               param;
  double               sens;
  bool            done;
  bool            rebrou;
  bool            iscomplete;
  bool            comptra;
  bool            clasonS1;
  bool            clasonS2;
  bool            check2d;
  bool            check;
  bool            twistflag1;
  bool            twistflag2;
};

#endif // _BRepBlend_Walking_HeaderFile
