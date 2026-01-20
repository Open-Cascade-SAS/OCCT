// Created on: 1996-01-18
// Created by: Frederic MAUPAS
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _STEPConstruct_UnitContext_HeaderFile
#define _STEPConstruct_UnitContext_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_CString.hxx>
#include <StepData_Factors.hxx>
#include <StepBasic_SiPrefix.hxx>
class StepData_StepModel;
class StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx;
class StepRepr_GlobalUnitAssignedContext;
class StepBasic_NamedUnit;
class StepRepr_GlobalUncertaintyAssignedContext;
class StepBasic_SiUnit;

//! Tool for creation (encoding) and decoding (for writing and reading
//! accordingly) context defining units and tolerances (uncerntanties)
class STEPConstruct_UnitContext
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates empty tool
  Standard_EXPORT STEPConstruct_UnitContext();

  //! Creates new context (units are MM and radians,
  //! uncertainty equal to Tol3d)
  Standard_EXPORT void Init(const double               Tol3d,
                            const occ::handle<StepData_StepModel>& theModel,
                            const StepData_Factors&           theLocalFactors = StepData_Factors());

  //! Returns True if Init was called successfully
  Standard_EXPORT bool IsDone() const;

  //! Returns context (or Null if not done)
  Standard_EXPORT occ::handle<StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx> Value()
    const;

  //! Computes the length, plane angle and solid angle conversion factor.
  //! Returns a status, 0 if OK
  Standard_EXPORT int
    ComputeFactors(const occ::handle<StepRepr_GlobalUnitAssignedContext>& aContext,
                   const StepData_Factors& theLocalFactors = StepData_Factors());

  Standard_EXPORT int
    ComputeFactors(const occ::handle<StepBasic_NamedUnit>& aUnit,
                   const StepData_Factors&            theLocalFactors = StepData_Factors());

  //! Computes the uncertainty value (for length)
  Standard_EXPORT int
    ComputeTolerance(const occ::handle<StepRepr_GlobalUncertaintyAssignedContext>& aContext);

  //! Returns the lengthFactor
  Standard_EXPORT double LengthFactor() const;

  //! Returns the planeAngleFactor
  Standard_EXPORT double PlaneAngleFactor() const;

  //! Returns the solidAngleFactor
  Standard_EXPORT double SolidAngleFactor() const;

  //! Returns the Uncertainty value (for length)
  //! It has been converted with LengthFactor
  Standard_EXPORT double Uncertainty() const;

  //! Returns the areaFactor
  Standard_EXPORT double AreaFactor() const;

  //! Returns the volumeFactor
  Standard_EXPORT double VolumeFactor() const;

  //! Tells if a Uncertainty (for length) is recorded
  Standard_EXPORT bool HasUncertainty() const;

  //! Returns true if ComputeFactors has calculated
  //! a LengthFactor
  Standard_EXPORT bool LengthDone() const;

  //! Returns true if ComputeFactors has calculated
  //! a PlaneAngleFactor
  Standard_EXPORT bool PlaneAngleDone() const;

  //! Returns true if ComputeFactors has calculated
  //! a SolidAngleFactor
  Standard_EXPORT bool SolidAngleDone() const;

  //! Returns true if areaFactor is computed
  Standard_EXPORT bool AreaDone() const;

  //! Returns true if volumeFactor is computed
  Standard_EXPORT bool VolumeDone() const;

  //! Returns a message for a given status (0 - empty)
  //! This message can then be added as warning for transfer
  Standard_EXPORT const char* StatusMessage(const int status) const;

  //! Convert SI prefix defined by enumeration to corresponding
  //! real factor (e.g. 1e6 for mega)
  Standard_EXPORT static double ConvertSiPrefix(const StepBasic_SiPrefix aPrefix);

private:
  //! Fills numerical equivalent of SiUnitName (in SI value)
  //! Returns False if name SiUnit Name not recognized
  Standard_EXPORT bool SiUnitNameFactor(const occ::handle<StepBasic_SiUnit>& aSiUnit,
                                                    double&                  val) const;

  bool                                                         done;
  occ::handle<StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx> GRC;
  double                                                            lengthFactor;
  double                                                            planeAngleFactor;
  double                                                            solidAngleFactor;
  double                                                            theUncertainty;
  double                                                            areaFactor;
  double                                                            volumeFactor;
  bool                                                         areaDone;
  bool                                                         volumeDone;
  bool                                                         lengthDone;
  bool                                                         planeAngleDone;
  bool                                                         solidAngleDone;
  bool                                                         hasUncertainty;
};

#endif // _STEPConstruct_UnitContext_HeaderFile
