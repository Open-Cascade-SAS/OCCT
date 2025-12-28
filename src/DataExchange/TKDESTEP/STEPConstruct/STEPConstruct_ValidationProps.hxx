// Created on: 1999-09-08
// Created by: Andrey BETENEV
// Copyright (c) 1999 Matra Datavision
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

#ifndef _STEPConstruct_ValidationProps_HeaderFile
#define _STEPConstruct_ValidationProps_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepBasic_Unit.hxx>
#include <STEPConstruct_Tool.hxx>
#include <Standard_CString.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_Sequence.hxx>
class StepBasic_ProductDefinition;
class XSControl_WorkSession;
class TopoDS_Shape;
class StepRepr_RepresentationItem;
class StepRepr_CharacterizedDefinition;
class StepRepr_RepresentationContext;
class gp_Pnt;
class StepRepr_NextAssemblyUsageOccurrence;
class StepRepr_PropertyDefinition;

//! This class provides tools for access (write and read)
//! the validation properties on shapes in the STEP file.
//! These are surface area, solid volume and centroid.
class STEPConstruct_ValidationProps : public STEPConstruct_Tool
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates an empty tool
  Standard_EXPORT STEPConstruct_ValidationProps();

  //! Creates a tool and loads it with worksession
  Standard_EXPORT STEPConstruct_ValidationProps(const occ::handle<XSControl_WorkSession>& WS);

  //! Load worksession; returns True if succeeded
  Standard_EXPORT bool Init(const occ::handle<XSControl_WorkSession>& WS);

  //! General method for adding (writing) a validation property
  //! for shape which should be already mapped on writing itself.
  //! It uses FindTarget() to find target STEP entity
  //! resulting from given shape, and associated context
  //! Returns True if success, False in case of fail
  Standard_EXPORT bool AddProp(const TopoDS_Shape&                        Shape,
                                           const occ::handle<StepRepr_RepresentationItem>& Prop,
                                           const char*                     Descr,
                                           const bool instance = false);

  //! General method for adding (writing) a validation property
  //! for shape which should be already mapped on writing itself.
  //! It takes target and Context entities which correspond to shape
  //! Returns True if success, False in case of fail
  Standard_EXPORT bool AddProp(const StepRepr_CharacterizedDefinition&       target,
                                           const occ::handle<StepRepr_RepresentationContext>& Context,
                                           const occ::handle<StepRepr_RepresentationItem>&    Prop,
                                           const char*                        Descr);

  //! Adds surface area property for given shape (already mapped).
  //! Returns True if success, False in case of fail
  Standard_EXPORT bool AddArea(const TopoDS_Shape& Shape, const double Area);

  //! Adds volume property for given shape (already mapped).
  //! Returns True if success, False in case of fail
  Standard_EXPORT bool AddVolume(const TopoDS_Shape& Shape, const double Vol);

  //! Adds centroid property for given shape (already mapped).
  //! Returns True if success, False in case of fail
  //! If instance is True, then centroid is assigned to
  //! an instance of component in assembly
  Standard_EXPORT bool AddCentroid(const TopoDS_Shape&    Shape,
                                               const gp_Pnt&          Pnt,
                                               const bool instance = false);

  //! Finds target STEP entity to which validation props should
  //! be assigned, and corresponding context, starting from shape
  //! Returns True if success, False in case of fail
  Standard_EXPORT bool FindTarget(const TopoDS_Shape&                     S,
                                              StepRepr_CharacterizedDefinition&       target,
                                              occ::handle<StepRepr_RepresentationContext>& Context,
                                              const bool instance = false);

  //! Searches for entities of the type PropertyDefinitionRepresentation
  //! in the model and fills the sequence by them
  Standard_EXPORT bool LoadProps(NCollection_Sequence<occ::handle<Standard_Transient>>& seq) const;

  //! Returns CDSR associated with given PpD or NULL if not found
  //! (when, try GetPropSDR)
  Standard_EXPORT occ::handle<StepRepr_NextAssemblyUsageOccurrence> GetPropNAUO(
    const occ::handle<StepRepr_PropertyDefinition>& PD) const;

  //! Returns SDR associated with given PpD or NULL if not found
  //! (when, try GetPropCDSR)
  Standard_EXPORT occ::handle<StepBasic_ProductDefinition> GetPropPD(
    const occ::handle<StepRepr_PropertyDefinition>& PD) const;

  //! Returns Shape associated with given SDR or Null Shape
  //! if not found
  Standard_EXPORT TopoDS_Shape
    GetPropShape(const occ::handle<StepBasic_ProductDefinition>& ProdDef) const;

  //! Returns Shape associated with given PpD or Null Shape
  //! if not found
  Standard_EXPORT TopoDS_Shape GetPropShape(const occ::handle<StepRepr_PropertyDefinition>& PD) const;

  //! Returns value of Real-Valued property (Area or Volume)
  //! If Property is neither Area nor Volume, returns False
  //! Else returns True and isArea indicates whether property
  //! is area or volume
  Standard_EXPORT bool
    GetPropReal(const occ::handle<StepRepr_RepresentationItem>& item,
                double&                             Val,
                bool&                          isArea,
                const StepData_Factors& theLocalFactors = StepData_Factors()) const;

  //! Returns value of Centroid property (or False if it is not)
  Standard_EXPORT bool
    GetPropPnt(const occ::handle<StepRepr_RepresentationItem>&    item,
               const occ::handle<StepRepr_RepresentationContext>& Context,
               gp_Pnt&                                       Pnt,
               const StepData_Factors& theLocalFactors = StepData_Factors()) const;

  //! Sets current assembly shape SDR (for FindCDSR calls)
  Standard_EXPORT void SetAssemblyShape(const TopoDS_Shape& shape);

private:
  StepBasic_Unit                      areaUnit;
  StepBasic_Unit                      volUnit;
  occ::handle<StepBasic_ProductDefinition> myAssemblyPD;
};

#endif // _STEPConstruct_ValidationProps_HeaderFile
