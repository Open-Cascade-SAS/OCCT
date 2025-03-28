// Copyright (c) 2025 OPEN CASCADE SAS
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

#ifndef _MergeSTEPEntities_Axis2Placement3dProcessor_HeaderFile
#define _MergeSTEPEntities_Axis2Placement3dProcessor_HeaderFile

#include <MergeSTEPEntities_EntityProcessor.hxx>
#include <MergeSTEPEntities_Axis2Placement3dHasher.hxx>

#include <StepGeom_Axis2Placement3d.hxx>

//! Processor for merging StepGeom_Axis2Placement3d entities.
//! This processor merges axis placements with the same location, axis, and ref direction.
class MergeSTEPEntities_Axis2Placement3dProcessor
    : public MergeSTEPEntities_EntityProcessor<StepGeom_Axis2Placement3d,
                                               MergeSTEPEntities_Axis2Placement3dHasher>
{
public:
  //! Constructor. Stores the work session and registers replacer functions.
  //! @param theWS the work session.
  Standard_EXPORT MergeSTEPEntities_Axis2Placement3dProcessor(
    const Handle(XSControl_WorkSession)& theWS);

private:
  //! Replaces the old axis placement with the new one in the StepGeom_Plane entity.
  //! @param theOldEntity the old axis placement.
  //! @param theNewEntity the new axis placement to replace the old one.
  //! @param theSharing the StepGeom_Plane entity to update.
  //! @return true if the axis placement was replaced, false otherwise.
  static bool replacePlane(const Handle(StepGeom_Axis2Placement3d)& theOldEntity,
                           const Handle(StepGeom_Axis2Placement3d)& theNewEntity,
                           Handle(Standard_Transient)               theSharing);

  //! Replaces the old axis placement with the new one in the StepRepr_ItemDefinedTransformation
  //! entity.
  //! @param theOldEntity the old axis placement.
  //! @param theNewEntity the new axis placement to replace the old one.
  //! @param theSharing the StepRepr_ItemDefinedTransformation entity to update.
  //! @return true if the axis placement was replaced, false otherwise.
  static bool replaceItemDefinedTransformation(
    const Handle(StepGeom_Axis2Placement3d)& theOldEntity,
    const Handle(StepGeom_Axis2Placement3d)& theNewEntity,
    Handle(Standard_Transient)               theSharing);

  //! Replaces the old axis placement with the new one in the StepGeom_CylindricalSurface entity.
  //! @param theOldEntity the old axis placement.
  //! @param theNewEntity the new axis placement to replace the old one.
  //! @param theSharing the StepGeom_CylindricalSurface entity to update.
  //! @return true if the axis placement was replaced, false otherwise.
  static bool replaceCylindricalSurface(const Handle(StepGeom_Axis2Placement3d)& theOldEntity,
                                        const Handle(StepGeom_Axis2Placement3d)& theNewEntity,
                                        Handle(Standard_Transient)               theSharing);

  //! Replaces the old axis placement with the new one in the StepShape_ShapeRepresentation entity.
  //! @param theOldEntity the old axis placement.
  //! @param theNewEntity the new axis placement to replace the old one.
  //! @param theSharing the StepShape_ShapeRepresentation entity to update.
  //! @return true if the axis placement was replaced, false otherwise.
  static bool replaceShapeRepresentation(const Handle(StepGeom_Axis2Placement3d)& theOldEntity,
                                         const Handle(StepGeom_Axis2Placement3d)& theNewEntity,
                                         Handle(Standard_Transient)               theSharing);

  //! Replaces the old axis placement with the new one in the
  //! StepRepr_ConstructiveGeometryRepresentation entity.
  //! @param theOldEntity the old axis placement.
  //! @param theNewEntity the new axis placement to replace the old one.
  //! @param theSharing the StepRepr_ConstructiveGeometryRepresentation entity to update.
  //! @return true if the axis placement was replaced, false otherwise.
  static bool replaceConstructiveGeometryRepresentation(
    const Handle(StepGeom_Axis2Placement3d)& theOldEntity,
    const Handle(StepGeom_Axis2Placement3d)& theNewEntity,
    Handle(Standard_Transient)               theSharing);

  //! Replaces the old axis placement with the new one in the StepGeom_Circle entity.
  //! @param theOldEntity the old axis placement.
  //! @param theNewEntity the new axis placement to replace the old one.
  //! @param theSharing the StepGeom_Circle entity to update.
  //! @return true if the axis placement was replaced, false otherwise.
  static bool replaceCircle(const Handle(StepGeom_Axis2Placement3d)& theOldEntity,
                            const Handle(StepGeom_Axis2Placement3d)& theNewEntity,
                            Handle(Standard_Transient)               theSharing);

  //! Replaces the old axis placement with the new one in the StepGeom_PresentationLayerAssignment
  //! @param theOldEntity the old axis placement.
  //! @param theNewEntity the new axis placement to replace the old one.
  //! @param theSharing the StepGeom_PresentationLayerAssignment entity to update.
  //! @return true if the axis placement was replaced, false otherwise.
  static bool replacePresentationLayerAssignment(
    const Handle(StepGeom_Axis2Placement3d)& theOldEntity,
    const Handle(StepGeom_Axis2Placement3d)& theNewEntity,
    Handle(Standard_Transient)               theSharing);

  //! Replaces the old axis placement with the new one in the StepVisual_StyledItem entity.
  //! @param theOldEntity the old axis placement.
  //! @param theNewEntity the new axis placement to replace the old one.
  //! @param theSharing the StepVisual_StyledItem entity to update.
  //! @return true if the axis placement was replaced, false otherwise.
  static bool replaceStyledItem(const Handle(StepGeom_Axis2Placement3d)& theOldEntity,
                                const Handle(StepGeom_Axis2Placement3d)& theNewEntity,
                                Handle(Standard_Transient)               theSharing);

  //! Replaces the old axis placement with the new one in the StepGeom_Ellipse entity.
  //! @param theOldEntity the old axis placement.
  //! @param theNewEntity the new axis placement to replace the old one.
  //! @param theSharing the StepGeom_Ellipse entity to update.
  //! @return true if the axis placement was replaced, false otherwise.
  static bool replaceEllipse(const Handle(StepGeom_Axis2Placement3d)& theOldEntity,
                             const Handle(StepGeom_Axis2Placement3d)& theNewEntity,
                             Handle(Standard_Transient)               theSharing);

  //! Replaces the old axis placement with the new one in the StepGeom_ConicalSurface entity.
  //! @param theOldEntity the old axis placement.
  //! @param theNewEntity the new axis placement to replace the old one.
  //! @param theSharing the StepGeom_ConicalSurface entity to update.
  //! @return true if the axis placement was replaced, false otherwise.
  static bool replaceConicalSurface(const Handle(StepGeom_Axis2Placement3d)& theOldEntity,
                                    const Handle(StepGeom_Axis2Placement3d)& theNewEntity,
                                    Handle(Standard_Transient)               theSharing);

  //! Replaces the old axis placement with the new one in the StepGeom_ToroidalSurface entity.
  //! @param theOldEntity the old axis placement.
  //! @param theNewEntity the new axis placement to replace the old one.
  //! @param theSharing the StepGeom_ToroidalSurface entity to update.
  //! @return true if the axis placement was replaced, false otherwise.
  static bool replaceToroidalSurface(const Handle(StepGeom_Axis2Placement3d)& theOldEntity,
                                     const Handle(StepGeom_Axis2Placement3d)& theNewEntity,
                                     Handle(Standard_Transient)               theSharing);

  //! Replaces the old axis placement with the new one in the
  //! StepShape_AdvancedBrepShapeRepresentation entity.
  //! @param theOldEntity the old axis placement.
  //! @param theNewEntity the new axis placement to replace the old one.
  //! @param theSharing the StepShape_AdvancedBrepShapeRepresentation entity to update.
  //! @return true if the axis placement was replaced, false otherwise.
  static bool replaceAdvancedBrepShapeRepresentation(
    const Handle(StepGeom_Axis2Placement3d)& theOldEntity,
    const Handle(StepGeom_Axis2Placement3d)& theNewEntity,
    Handle(Standard_Transient)               theSharing);

  //! Replaces the old axis placement with the new one in the StepGeom_SphericalSurface entity.
  //! @param theOldEntity the old axis placement.
  //! @param theNewEntity the new axis placement to replace the old one.
  //! @param theSharing the StepGeom_SphericalSurface entity to update.
  //! @return true if the axis placement was replaced, false otherwise.
  static bool replaceSphericalSurface(const Handle(StepGeom_Axis2Placement3d)& theOldEntity,
                                      const Handle(StepGeom_Axis2Placement3d)& theNewEntity,
                                      Handle(Standard_Transient)               theSharing);
};

#endif // _MergeSTEPEntities_DirectionProcessor_HeaderFile
