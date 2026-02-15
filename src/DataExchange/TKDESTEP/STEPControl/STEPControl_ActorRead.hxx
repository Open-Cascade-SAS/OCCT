// Created on: 1994-12-21
// Created by: Dieter THIEMANN
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _STEPControl_ActorRead_HeaderFile
#define _STEPControl_ActorRead_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepData_Factors.hxx>
#include <StepToTopoDS_NMTool.hxx>
#include <Transfer_ActorOfTransientProcess.hxx>
#include <Standard_Integer.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <Message_ProgressRange.hxx>
#include <Interface_InterfaceModel.hxx>

class StepRepr_Representation;
class Standard_Transient;
class Transfer_Binder;
class Transfer_TransientProcess;
class StepGeom_Axis2Placement3d;
class gp_Trsf;
class StepRepr_RepresentationRelationship;
class TransferBRep_ShapeBinder;
class StepBasic_ProductDefinition;
class StepRepr_NextAssemblyUsageOccurrence;
class StepShape_ShapeRepresentation;
class StepShape_ContextDependentShapeRepresentation;
class StepRepr_ShapeRepresentationRelationship;
class StepGeom_GeometricRepresentationItem;
class StepRepr_MappedItem;
class StepShape_FaceSurface;
class TopoDS_Shell;
class TopoDS_Compound;
class StepRepr_ConstructiveGeometryRepresentationRelationship;
class StepRepr_MechanicalDesignAndDraughtingRelationship;
class StepData_StepModel;

//! This class performs the transfer of an Entity from
//! AP214 and AP203, either Geometric or Topologic.
//!
//! I.E. for each type of Entity, it invokes the appropriate Tool
//! then returns the Binder which contains the Result
class STEPControl_ActorRead : public Transfer_ActorOfTransientProcess
{

public:
  Standard_EXPORT STEPControl_ActorRead(const occ::handle<Interface_InterfaceModel>& theModel);

  Standard_EXPORT bool Recognize(const occ::handle<Standard_Transient>& start) override;

  Standard_EXPORT occ::handle<Transfer_Binder> Transfer(
    const occ::handle<Standard_Transient>&        start,
    const occ::handle<Transfer_TransientProcess>& TP,
    const Message_ProgressRange&                  theProgress = Message_ProgressRange()) override;

  //! theUseTrsf - special flag for using Axis2Placement from ShapeRepresentation for transform root
  //! shape
  Standard_EXPORT occ::handle<Transfer_Binder> TransferShape(
    const occ::handle<Standard_Transient>&        start,
    const occ::handle<Transfer_TransientProcess>& TP,
    const StepData_Factors&                       theLocalFactors = StepData_Factors(),
    const bool                                    isManifold      = true,
    const bool                                    theUseTrsf      = false,
    const Message_ProgressRange&                  theProgress     = Message_ProgressRange());

  //! set units and tolerances context by given ShapeRepresentation
  Standard_EXPORT void PrepareUnits(const occ::handle<StepRepr_Representation>&   rep,
                                    const occ::handle<Transfer_TransientProcess>& TP,
                                    StepData_Factors&                             theLocalFactors);

  //! reset units and tolerances context to default
  //! (mm, radians, read.precision.val, etc.)
  Standard_EXPORT void ResetUnits(occ::handle<StepData_StepModel>& theModel,
                                  StepData_Factors&                theLocalFactors);

  //! Set model
  Standard_EXPORT void SetModel(const occ::handle<Interface_InterfaceModel>& theModel);

  //! Computes transformation defined by two axis placements (in MAPPED_ITEM
  //! or ITEM_DEFINED_TRANSFORMATION) taking into account their
  //! representation contexts (i.e. units, which may be different)
  //! Returns True if transformation is computed and is not an identity.
  Standard_EXPORT bool ComputeTransformation(
    const occ::handle<StepGeom_Axis2Placement3d>& Origin,
    const occ::handle<StepGeom_Axis2Placement3d>& Target,
    const occ::handle<StepRepr_Representation>&   OrigContext,
    const occ::handle<StepRepr_Representation>&   TargContext,
    const occ::handle<Transfer_TransientProcess>& TP,
    gp_Trsf&                                      Trsf,
    const StepData_Factors&                       theLocalFactors = StepData_Factors());

  //! Computes transformation defined by given
  //! REPRESENTATION_RELATIONSHIP_WITH_TRANSFORMATION
  Standard_EXPORT bool ComputeSRRWT(const occ::handle<StepRepr_RepresentationRelationship>& SRR,
                                    const occ::handle<Transfer_TransientProcess>&           TP,
                                    gp_Trsf&                                                Trsf,
                                    const StepData_Factors& theLocalFactors = StepData_Factors());

  //! Heals the collected during transferring shapes.
  //! Activated only in case if corresponding DE parameter is selected.
  //! If post-healing is not enabled, healing performed in TransferShape method.
  //! Can be called after transferring shapes.
  //! Automatically clean shapes to heal after healing.
  //! @param theTP - transient process to use for healing
  //! @param theFirstIndex - index of first shape to merge history
  Standard_EXPORT void PostHealing(const Handle(Transfer_TransientProcess)& theTP,
                                   const Standard_Integer                   theFirstIndex);

  DEFINE_STANDARD_RTTIEXT(STEPControl_ActorRead, Transfer_ActorOfTransientProcess)

protected:
  //! Transfers product definition entity
  //! theUseTrsf - special flag for using Axis2Placement from ShapeRepresentation for transform root
  //! shape
  Standard_EXPORT occ::handle<TransferBRep_ShapeBinder> TransferEntity(
    const occ::handle<StepBasic_ProductDefinition>& PD,
    const occ::handle<Transfer_TransientProcess>&   TP,
    const StepData_Factors&                         theLocalFactors = StepData_Factors(),
    const bool                                      theUseTrsf      = false,
    const Message_ProgressRange&                    theProgress     = Message_ProgressRange());

  //! Transfers next assembly usage occurrence entity
  Standard_EXPORT occ::handle<TransferBRep_ShapeBinder> TransferEntity(
    const occ::handle<StepRepr_NextAssemblyUsageOccurrence>& NAUO,
    const occ::handle<Transfer_TransientProcess>&            TP,
    const StepData_Factors&                                  theLocalFactors = StepData_Factors(),
    const Message_ProgressRange&                             theProgress = Message_ProgressRange());

  //! Transfers shape representation entity
  //! theUseTrsf - special flag for using Axis2Placement from ShapeRepresentation for transform root
  //! shape
  Standard_EXPORT occ::handle<TransferBRep_ShapeBinder> TransferEntity(
    const occ::handle<StepShape_ShapeRepresentation>& sr,
    const occ::handle<Transfer_TransientProcess>&     TP,
    const StepData_Factors&                           theLocalFactors,
    bool&                                             isBound,
    const bool                                        theUseTrsf  = false,
    const Message_ProgressRange&                      theProgress = Message_ProgressRange());

  //! Transfers context dependent shape representation entity
  Standard_EXPORT occ::handle<TransferBRep_ShapeBinder> TransferEntity(
    const occ::handle<StepShape_ContextDependentShapeRepresentation>& CDSR,
    const occ::handle<Transfer_TransientProcess>&                     TP,
    const StepData_Factors&      theLocalFactors = StepData_Factors(),
    const Message_ProgressRange& theProgress     = Message_ProgressRange());

  //! Transfers shape representation relationship entity
  //! theUseTrsf - special flag for using Axis2Placement from ShapeRepresentation for transform root
  //! shape
  Standard_EXPORT occ::handle<TransferBRep_ShapeBinder> TransferEntity(
    const occ::handle<StepRepr_ShapeRepresentationRelationship>& und,
    const occ::handle<Transfer_TransientProcess>&                TP,
    const StepData_Factors&                                      theLocalFactors,
    const int                                                    nbrep      = 0,
    const bool                                                   theUseTrsf = false,
    const Message_ProgressRange& theProgress = Message_ProgressRange());

  //! Transfers geometric representation item entity such as ManifoldSolidBRep ,...etc
  Standard_EXPORT occ::handle<TransferBRep_ShapeBinder> TransferEntity(
    const occ::handle<StepGeom_GeometricRepresentationItem>& git,
    const occ::handle<Transfer_TransientProcess>&            TP,
    const StepData_Factors&                                  theLocalFactors,
    const bool                                               isManifold,
    const Message_ProgressRange&                             theProgress);

  //! Transfers mapped item
  Standard_EXPORT occ::handle<TransferBRep_ShapeBinder> TransferEntity(
    const occ::handle<StepRepr_MappedItem>&       mapit,
    const occ::handle<Transfer_TransientProcess>& TP,
    const StepData_Factors&                       theLocalFactors,
    const Message_ProgressRange&                  theProgress);

  //! Transfers FaceSurface entity
  Standard_EXPORT occ::handle<TransferBRep_ShapeBinder> TransferEntity(
    const occ::handle<StepShape_FaceSurface>&     fs,
    const occ::handle<Transfer_TransientProcess>& TP,
    const StepData_Factors&                       theLocalFactors,
    const Message_ProgressRange&                  theProgress);

  occ::handle<TransferBRep_ShapeBinder> TransferEntity(
    const occ::handle<StepRepr_ConstructiveGeometryRepresentationRelationship>& theCGRR,
    const occ::handle<Transfer_TransientProcess>&                               theTP,
    const StepData_Factors& theLocalFactors = StepData_Factors());

  //! Transfers MechanicalDesignAndDraughtingRelationship entity
  occ::handle<TransferBRep_ShapeBinder> TransferEntity(
    const occ::handle<StepRepr_MechanicalDesignAndDraughtingRelationship>& theMDADR,
    const occ::handle<Transfer_TransientProcess>&                          theTP,
    const StepData_Factors&                                                theLocalFactors,
    const Message_ProgressRange&                                           theProgress);

  //! Translates file by old way when CDSR are roots.
  //! Acts only if "read.step.product_mode" is equal Off.
  Standard_EXPORT occ::handle<TransferBRep_ShapeBinder> OldWay(
    const occ::handle<Standard_Transient>&        start,
    const occ::handle<Transfer_TransientProcess>& TP,
    const Message_ProgressRange&                  theProgress);

private:
  Standard_EXPORT TopoDS_Shell closeIDEASShell(const TopoDS_Shell&                   shell,
                                               const NCollection_List<TopoDS_Shape>& closingShells);

  Standard_EXPORT void computeIDEASClosings(
    const TopoDS_Compound&                               comp,
    NCollection_IndexedDataMap<TopoDS_Shape,
                               NCollection_List<TopoDS_Shape>,
                               TopTools_ShapeMapHasher>& shellClosingMap);

  Standard_EXPORT TopoDS_Shape
    TransferRelatedSRR(const occ::handle<Transfer_TransientProcess>&     theTP,
                       const occ::handle<StepShape_ShapeRepresentation>& theRep,
                       const bool                                        theUseTrsf,
                       const bool                                        theReadConstructiveGeomRR,
                       const StepData_Factors&                           theLocalFactors,
                       TopoDS_Compound&                                  theCund,
                       Message_ProgressScope&                            thePS);

private:
  StepToTopoDS_NMTool                  myNMTool;
  Standard_Real                        myPrecision;
  Standard_Real                        myMaxTol;
  Handle(StepRepr_Representation)      mySRContext;
  Handle(Interface_InterfaceModel)     myModel;
  NCollection_IndexedMap<TopoDS_Shape> myShapesToHeal;
};

#endif // _STEPControl_ActorRead_HeaderFile
