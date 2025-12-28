// Created on: 1998-07-07
// Created by: Christian CAILLET
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _STEPConstruct_Assembly_HeaderFile
#define _STEPConstruct_Assembly_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

class StepShape_ShapeDefinitionRepresentation;
class StepShape_ShapeRepresentation;
class StepGeom_Axis2Placement3d;
class StepGeom_CartesianTransformationOperator3d;
class StepRepr_NextAssemblyUsageOccurrence;
class StepShape_ContextDependentShapeRepresentation;
class Interface_Graph;

//! This operator creates and checks an item of an assembly, from its
//! basic data : a ShapeRepresentation, a Location ...
//!
//! Three ways of coding such item from a ShapeRepresentation :
//! - do nothing : i.e. information for assembly are ignored
//! - create a MappedItem
//! - create a RepresentationRelationship (WithTransformation)
class STEPConstruct_Assembly
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT STEPConstruct_Assembly();

  //! Initialises with starting values
  //! Ax0 : origin axis (typically, standard XYZ)
  //! Loc : location to which place the item
  //! Makes a MappedItem
  //! Resulting Value is returned by ItemValue
  Standard_EXPORT void Init(const occ::handle<StepShape_ShapeDefinitionRepresentation>& aSR,
                            const occ::handle<StepShape_ShapeDefinitionRepresentation>& SDR0,
                            const occ::handle<StepGeom_Axis2Placement3d>&               Ax0,
                            const occ::handle<StepGeom_Axis2Placement3d>&               Loc);

  //! Initialises with starting values
  //! theTrsfOp : local transformation to apply, may have scaling factor
  //! Makes a MappedItem
  //! Resulting Value is returned by ItemValue
  Standard_EXPORT void Init(const occ::handle<StepShape_ShapeDefinitionRepresentation>&    theSR,
                            const occ::handle<StepShape_ShapeDefinitionRepresentation>&    theSDR0,
                            const occ::handle<StepGeom_CartesianTransformationOperator3d>& theTrsfOp);

  //! Make a (ShapeRepresentationRelationship,...WithTransformation)
  //! Resulting Value is returned by ItemValue
  Standard_EXPORT void MakeRelationship();

  //! Returns the Value
  //! If no Make... has been called, returns the starting SR
  Standard_EXPORT occ::handle<Standard_Transient> ItemValue() const;

  //! Returns the location of the item, computed from starting aLoc
  Standard_EXPORT occ::handle<StepGeom_Axis2Placement3d> ItemLocation() const;

  //! Returns NAUO object describing the assembly link
  Standard_EXPORT occ::handle<StepRepr_NextAssemblyUsageOccurrence> GetNAUO() const;

  //! Checks whether SRR's definition of assembly and component contradicts
  //! with NAUO definition or not, according to model schema (AP214 or AP203)
  Standard_EXPORT static bool CheckSRRReversesNAUO(
    const Interface_Graph&                                       theGraph,
    const occ::handle<StepShape_ContextDependentShapeRepresentation>& CDSR);

private:
  occ::handle<StepShape_ShapeDefinitionRepresentation>    thesdr;
  occ::handle<StepShape_ShapeDefinitionRepresentation>    thesdr0;
  occ::handle<StepShape_ShapeRepresentation>              thesr;
  occ::handle<StepShape_ShapeRepresentation>              thesr0;
  occ::handle<Standard_Transient>                         theval;
  occ::handle<StepGeom_Axis2Placement3d>                  theloc;
  occ::handle<StepGeom_Axis2Placement3d>                  theax0;
  occ::handle<StepGeom_CartesianTransformationOperator3d> myTrsfOp;
  bool                                               myIsCartesianTrsf;
};

#endif // _STEPConstruct_Assembly_HeaderFile
