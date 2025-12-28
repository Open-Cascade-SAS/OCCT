// Created on: 1993-09-28
// Created by: Bruno DUMORTIER
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

#ifndef _GeomFill_AppSweep_HeaderFile
#define _GeomFill_AppSweep_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_HArray2.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_HArray2.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <NCollection_Sequence.hxx>
#include <Approx_ParametrizationType.hxx>
#include <GeomAbs_Shape.hxx>
#include <AppBlend_Approx.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Array1.hxx>
class StdFail_NotDone;
class Standard_DomainError;
class Standard_OutOfRange;
class GeomFill_SweepSectionGenerator;
class GeomFill_Line;

//! Approximate a sweep surface passing by all the
//! curves described in the SweepSectionGenerator.

class GeomFill_AppSweep : public AppBlend_Approx
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT GeomFill_AppSweep();

  Standard_EXPORT GeomFill_AppSweep(const int Degmin,
                                    const int Degmax,
                                    const double    Tol3d,
                                    const double    Tol2d,
                                    const int NbIt,
                                    const bool KnownParameters = false);

  Standard_EXPORT void Init(const int Degmin,
                            const int Degmax,
                            const double    Tol3d,
                            const double    Tol2d,
                            const int NbIt,
                            const bool KnownParameters = false);

  //! Define the type of parametrization used in the approximation
  Standard_EXPORT void SetParType(const Approx_ParametrizationType ParType);

  //! Define the Continuity used in the approximation
  Standard_EXPORT void SetContinuity(const GeomAbs_Shape C);

  //! define the Weights associed to the criterium used in
  //! the optimization.
  //!
  //! if Wi <= 0
  Standard_EXPORT void SetCriteriumWeight(const double W1,
                                          const double W2,
                                          const double W3);

  //! returns the type of parametrization used in the approximation
  Standard_EXPORT Approx_ParametrizationType ParType() const;

  //! returns the Continuity used in the approximation
  Standard_EXPORT GeomAbs_Shape Continuity() const;

  //! returns the Weights (as percent) associed to the criterium used in
  //! the optimization.
  Standard_EXPORT void CriteriumWeight(double& W1,
                                       double& W2,
                                       double& W3) const;

  Standard_EXPORT void Perform(const occ::handle<GeomFill_Line>&    Lin,
                               GeomFill_SweepSectionGenerator& SecGen,
                               const bool          SpApprox = false);

  Standard_EXPORT void PerformSmoothing(const occ::handle<GeomFill_Line>&    Lin,
                                        GeomFill_SweepSectionGenerator& SecGen);

  Standard_EXPORT void Perform(const occ::handle<GeomFill_Line>&    Lin,
                               GeomFill_SweepSectionGenerator& SecGen,
                               const int          NbMaxP);

  bool IsDone() const;

  Standard_EXPORT void SurfShape(int& UDegree,
                                 int& VDegree,
                                 int& NbUPoles,
                                 int& NbVPoles,
                                 int& NbUKnots,
                                 int& NbVKnots) const;

  Standard_EXPORT void Surface(NCollection_Array2<gp_Pnt>&      TPoles,
                               NCollection_Array2<double>&    TWeights,
                               NCollection_Array1<double>&    TUKnots,
                               NCollection_Array1<double>&    TVKnots,
                               NCollection_Array1<int>& TUMults,
                               NCollection_Array1<int>& TVMults) const;

  int UDegree() const;

  int VDegree() const;

  const NCollection_Array2<gp_Pnt>& SurfPoles() const;

  const NCollection_Array2<double>& SurfWeights() const;

  const NCollection_Array1<double>& SurfUKnots() const;

  const NCollection_Array1<double>& SurfVKnots() const;

  const NCollection_Array1<int>& SurfUMults() const;

  const NCollection_Array1<int>& SurfVMults() const;

  int NbCurves2d() const;

  Standard_EXPORT void Curves2dShape(int& Degree,
                                     int& NbPoles,
                                     int& NbKnots) const;

  Standard_EXPORT void Curve2d(const int   Index,
                               NCollection_Array1<gp_Pnt2d>&    TPoles,
                               NCollection_Array1<double>&    TKnots,
                               NCollection_Array1<int>& TMults) const;

  int Curves2dDegree() const;

  const NCollection_Array1<gp_Pnt2d>& Curve2dPoles(const int Index) const;

  const NCollection_Array1<double>& Curves2dKnots() const;

  const NCollection_Array1<int>& Curves2dMults() const;

  void TolReached(double& Tol3d, double& Tol2d) const;

  Standard_EXPORT double TolCurveOnSurf(const int Index) const;

private:
  Standard_EXPORT void InternalPerform(const occ::handle<GeomFill_Line>&    Lin,
                                       GeomFill_SweepSectionGenerator& SecGen,
                                       const bool          SpApprox,
                                       const bool          UseVariational);

  bool                 done;
  int                 dmin;
  int                 dmax;
  double                    tol3d;
  double                    tol2d;
  int                 nbit;
  int                 udeg;
  int                 vdeg;
  bool                 knownp;
  occ::handle<NCollection_HArray2<gp_Pnt>>      tabPoles;
  occ::handle<NCollection_HArray2<double>>    tabWeights;
  occ::handle<NCollection_HArray1<double>>    tabUKnots;
  occ::handle<NCollection_HArray1<double>>    tabVKnots;
  occ::handle<NCollection_HArray1<int>> tabUMults;
  occ::handle<NCollection_HArray1<int>> tabVMults;
  NCollection_Sequence<occ::handle<NCollection_HArray1<gp_Pnt2d>>>   seqPoles2d;
  double                    tol3dreached;
  double                    tol2dreached;
  Approx_ParametrizationType       paramtype;
  GeomAbs_Shape                    continuity;
  double                    critweights[3];
};

#define TheSectionGenerator GeomFill_SweepSectionGenerator
#define TheSectionGenerator_hxx <GeomFill_SweepSectionGenerator.hxx>
#define Handle_TheLine occ::handle<GeomFill_Line>
#define TheLine GeomFill_Line
#define TheLine_hxx <GeomFill_Line.hxx>
#define AppBlend_AppSurf GeomFill_AppSweep
#define AppBlend_AppSurf_hxx <GeomFill_AppSweep.hxx>

#include <AppBlend_AppSurf.lxx>

#undef TheSectionGenerator
#undef TheSectionGenerator_hxx
#undef Handle_TheLine
#undef TheLine
#undef TheLine_hxx
#undef AppBlend_AppSurf
#undef AppBlend_AppSurf_hxx

#endif // _GeomFill_AppSweep_HeaderFile
