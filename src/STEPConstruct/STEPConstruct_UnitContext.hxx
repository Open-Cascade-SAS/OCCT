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

#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <Standard_CString.hxx>
#include <StepBasic_SiPrefix.hxx>
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
  Standard_EXPORT void Init (const Standard_Real Tol3d);
  
  //! Returns True if Init was called successfully
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  //! Returns context (or Null if not done)
  Standard_EXPORT Handle(StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx) Value() const;
  
  //! Computes the length, plane angle and solid angle conversion
  //! factor  .  Returns a status, 0 if OK
  Standard_EXPORT Standard_Integer ComputeFactors (const Handle(StepRepr_GlobalUnitAssignedContext)& aContext);
  
  Standard_EXPORT Standard_Integer ComputeFactors (const Handle(StepBasic_NamedUnit)& aUnit);
  
  //! Computes the uncertainty value (for length)
  Standard_EXPORT Standard_Integer ComputeTolerance (const Handle(StepRepr_GlobalUncertaintyAssignedContext)& aContext);
  
  //! Returns the lengthFactor
  Standard_EXPORT Standard_Real LengthFactor() const;
  
  //! Returns the planeAngleFactor
  Standard_EXPORT Standard_Real PlaneAngleFactor() const;
  
  //! Returns the solidAngleFactor
  Standard_EXPORT Standard_Real SolidAngleFactor() const;
  
  //! Returns the Uncertainty value (for length)
  //! It has been converted  with LengthFactor
  Standard_EXPORT Standard_Real Uncertainty() const;
  
  //! Returns the areaFactor
  Standard_EXPORT Standard_Real AreaFactor() const;
  
  //! Returns the volumeFactor
  Standard_EXPORT Standard_Real VolumeFactor() const;
  
  //! Tells if a Uncertainty (for length) is recorded
  Standard_EXPORT Standard_Boolean HasUncertainty() const;
  
  //! Returns true if ComputeFactors has calculated
  //! a LengthFactor
  Standard_EXPORT Standard_Boolean LengthDone() const;
  
  //! Returns true if ComputeFactors has calculated
  //! a PlaneAngleFactor
  Standard_EXPORT Standard_Boolean PlaneAngleDone() const;
  
  //! Returns true if ComputeFactors has calculated
  //! a SolidAngleFactor
  Standard_EXPORT Standard_Boolean SolidAngleDone() const;
  
  //! Returns true if areaFactor is computed
  Standard_EXPORT Standard_Boolean AreaDone() const;
  
  //! Returns true if volumeFactor is computed
  Standard_EXPORT Standard_Boolean VolumeDone() const;
  
  //! Returns a message for a given status (0 - empty)
  //! This message can then be added as warning for transfer
  Standard_EXPORT Standard_CString StatusMessage (const Standard_Integer status) const;
  
  //! Convert SI prefix defined by enumertaion to corresponding
  //! real factor (e.g. 1e6 for mega)
  Standard_EXPORT static Standard_Real ConvertSiPrefix (const StepBasic_SiPrefix aPrefix);




protected:





private:

  
  //! Fills numerical equivalent of SiUnitName (in SI value)
  //! Returns False if name SiUnit Name not recognized
  Standard_EXPORT Standard_Boolean SiUnitNameFactor (const Handle(StepBasic_SiUnit)& aSiUnit, Standard_Real& val) const;


  Standard_Boolean done;
  Handle(StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx) GRC;
  Standard_Real lengthFactor;
  Standard_Real planeAngleFactor;
  Standard_Real solidAngleFactor;
  Standard_Real theUncertainty;
  Standard_Real areaFactor;
  Standard_Real volumeFactor;
  Standard_Boolean areaDone;
  Standard_Boolean volumeDone;
  Standard_Boolean lengthDone;
  Standard_Boolean planeAngleDone;
  Standard_Boolean solidAngleDone;
  Standard_Boolean hasUncertainty;


};







#endif // _STEPConstruct_UnitContext_HeaderFile
