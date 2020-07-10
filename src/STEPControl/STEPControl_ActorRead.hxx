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

#include <StepToTopoDS_NMTool.hxx>
#include <Standard_Real.hxx>
#include <Transfer_ActorOfTransientProcess.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <Message_ProgressRange.hxx>

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


class STEPControl_ActorRead;
DEFINE_STANDARD_HANDLE(STEPControl_ActorRead, Transfer_ActorOfTransientProcess)

//! This class performs the transfer of an Entity from
//! AP214 and AP203, either Geometric or Topologic.
//!
//! I.E. for each type of Entity, it invokes the appropriate Tool
//! then returns the Binder which contains the Result
class STEPControl_ActorRead : public Transfer_ActorOfTransientProcess
{

public:

  
  Standard_EXPORT STEPControl_ActorRead();
  
  Standard_EXPORT virtual Standard_Boolean Recognize (const Handle(Standard_Transient)& start) Standard_OVERRIDE;
  
  Standard_EXPORT virtual Handle(Transfer_Binder) Transfer
                   (const Handle(Standard_Transient)& start,
                    const Handle(Transfer_TransientProcess)& TP,
                    const Message_ProgressRange& theProgress = Message_ProgressRange()) Standard_OVERRIDE;
  
  //! theUseTrsf - special flag for using Axis2Placement from ShapeRepresentation for transform root shape
  Standard_EXPORT Handle(Transfer_Binder) TransferShape (
      const Handle(Standard_Transient)& start,
      const Handle(Transfer_TransientProcess)& TP,
      const Standard_Boolean isManifold = Standard_True,
      const Standard_Boolean theUseTrsf = Standard_False,
      const Message_ProgressRange& theProgress = Message_ProgressRange());
  
  //! set units and tolerances context by given ShapeRepresentation
  Standard_EXPORT void PrepareUnits (const Handle(StepRepr_Representation)& rep, const Handle(Transfer_TransientProcess)& TP);
  
  //! reset units and tolerances context to default
  //! (mm, radians, read.precision.val, etc.)
  Standard_EXPORT void ResetUnits();
  
  //! Computes transformation defined by two axis placements (in MAPPED_ITEM
  //! or ITEM_DEFINED_TRANSFORMATION) taking into account their
  //! representation contexts (i.e. units, which may be different)
  //! Returns True if transformation is computed and is not an identity.
  Standard_EXPORT Standard_Boolean ComputeTransformation (const Handle(StepGeom_Axis2Placement3d)& Origin, const Handle(StepGeom_Axis2Placement3d)& Target, const Handle(StepRepr_Representation)& OrigContext, const Handle(StepRepr_Representation)& TargContext, const Handle(Transfer_TransientProcess)& TP, gp_Trsf& Trsf);
  
  //! Computes transformation defined by given
  //! REPRESENTATION_RELATIONSHIP_WITH_TRANSFORMATION
  Standard_EXPORT Standard_Boolean ComputeSRRWT (const Handle(StepRepr_RepresentationRelationship)& SRR, const Handle(Transfer_TransientProcess)& TP, gp_Trsf& Trsf);




  DEFINE_STANDARD_RTTIEXT(STEPControl_ActorRead,Transfer_ActorOfTransientProcess)

protected:

  
  //! Transfers product definition entity
  //! theUseTrsf - special flag for using Axis2Placement from ShapeRepresentation for transform root shape
    Standard_EXPORT Handle(TransferBRep_ShapeBinder) TransferEntity (
      const Handle(StepBasic_ProductDefinition)& PD,
      const Handle(Transfer_TransientProcess)& TP,
      const Standard_Boolean theUseTrsf = Standard_False,
      const Message_ProgressRange& theProgress = Message_ProgressRange());
  
  //! Transfers next assembly usage occurence entity
  Standard_EXPORT Handle(TransferBRep_ShapeBinder) TransferEntity
                   (const Handle(StepRepr_NextAssemblyUsageOccurrence)& NAUO,
                    const Handle(Transfer_TransientProcess)& TP,
                    const Message_ProgressRange& theProgress = Message_ProgressRange());
  
  //! Transfers shape representation entity
  //! theUseTrsf - special flag for using Axis2Placement from ShapeRepresentation for transform root shape
  Standard_EXPORT Handle(TransferBRep_ShapeBinder) TransferEntity (
      const Handle(StepShape_ShapeRepresentation)& sr,
      const Handle(Transfer_TransientProcess)& TP,
      Standard_Boolean& isBound,
      const Standard_Boolean theUseTrsf = Standard_False,
      const Message_ProgressRange& theProgress = Message_ProgressRange());
  
  //! Transfers context dependent shape representation entity
  Standard_EXPORT Handle(TransferBRep_ShapeBinder) TransferEntity
                   (const Handle(StepShape_ContextDependentShapeRepresentation)& CDSR,
                    const Handle(Transfer_TransientProcess)& TP,
                    const Message_ProgressRange& theProgress = Message_ProgressRange());
  
  //! Transfers  shape representation relationship entity
  //! theUseTrsf - special flag for using Axis2Placement from ShapeRepresentation for transform root shape
  Standard_EXPORT Handle(TransferBRep_ShapeBinder) TransferEntity (
      const Handle(StepRepr_ShapeRepresentationRelationship)& und,
      const Handle(Transfer_TransientProcess)& TP,
      const Standard_Integer nbrep = 0,
      const Standard_Boolean theUseTrsf = Standard_False,
      const Message_ProgressRange& theProgress = Message_ProgressRange());
  
  //! Transfers  geometric representation item entity such as ManifoldSolidBRep ,...etc
  Standard_EXPORT Handle(TransferBRep_ShapeBinder) TransferEntity
                   (const Handle(StepGeom_GeometricRepresentationItem)& git,
                    const Handle(Transfer_TransientProcess)& TP,
                    const Standard_Boolean isManifold,
                    const Message_ProgressRange& theProgress);
  
  //! Transfers  mapped item
  Standard_EXPORT Handle(TransferBRep_ShapeBinder) TransferEntity
                   (const Handle(StepRepr_MappedItem)& mapit,
                    const Handle(Transfer_TransientProcess)& TP,
                    const Message_ProgressRange& theProgress);
  
  //! Transfers  FaceSurface entity
  Standard_EXPORT Handle(TransferBRep_ShapeBinder) TransferEntity
                   (const Handle(StepShape_FaceSurface)& fs,
                    const Handle(Transfer_TransientProcess)& TP,
                    const Message_ProgressRange& theProgress);

  Handle(TransferBRep_ShapeBinder) TransferEntity( const Handle(StepRepr_ConstructiveGeometryRepresentationRelationship)& theCGRR,
    const Handle(Transfer_TransientProcess)& theTP);
  
  //! Tranlates file by old way when CDSR are roots . Acts only if "read.step.product_mode" is equal Off.
  Standard_EXPORT Handle(TransferBRep_ShapeBinder) OldWay
                   (const Handle(Standard_Transient)& start,
                    const Handle(Transfer_TransientProcess)& TP,
                    const Message_ProgressRange& theProgress);



private:

  
  Standard_EXPORT TopoDS_Shell closeIDEASShell (const TopoDS_Shell& shell, const TopTools_ListOfShape& closingShells);
  
  Standard_EXPORT void computeIDEASClosings (const TopoDS_Compound& comp, TopTools_IndexedDataMapOfShapeListOfShape& shellClosingMap);

  StepToTopoDS_NMTool myNMTool;
  Standard_Real myPrecision;
  Standard_Real myMaxTol;
  Handle(StepRepr_Representation) mySRContext;


};







#endif // _STEPControl_ActorRead_HeaderFile
