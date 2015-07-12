// Created on: 1999-11-17
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

#ifndef _STEPConstruct_HeaderFile
#define _STEPConstruct_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
class StepRepr_RepresentationItem;
class Transfer_FinderProcess;
class TopoDS_Shape;
class TopLoc_Location;
class Transfer_TransientProcess;
class Transfer_Binder;
class StepShape_ShapeDefinitionRepresentation;
class StepShape_ContextDependentShapeRepresentation;
class STEPConstruct_Tool;
class STEPConstruct_UnitContext;
class STEPConstruct_Part;
class STEPConstruct_Assembly;
class STEPConstruct_Styles;
class STEPConstruct_ValidationProps;
class STEPConstruct_ExternRefs;
class STEPConstruct_AP203Context;
class STEPConstruct_ContextTool;
class STEPConstruct_PointHasher;


//! Defines tools for creation and investigation STEP constructs
//! used for representing various kinds of data, such as product and
//! assembly structure, unit contexts, associated information
//! The creation of these structures is made according to currently
//! active schema (AP203 or AP214 CD2 or DIS)
//! This is taken from parameter write.step.schema
class STEPConstruct 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Returns STEP entity of the (sub)type of RepresentationItem
  //! which is a result of the tranalation of the Shape, or Null if
  //! no result is recorded
  Standard_EXPORT static Handle(StepRepr_RepresentationItem) FindEntity (const Handle(Transfer_FinderProcess)& FinderProcess, const TopoDS_Shape& Shape);
  
  //! The same as above, but in the case if item not found, repeats
  //! search on the same shape without location. The Loc corresponds to the
  //! location with which result is found (either location of the Shape,
  //! or Null)
  Standard_EXPORT static Handle(StepRepr_RepresentationItem) FindEntity (const Handle(Transfer_FinderProcess)& FinderProcess, const TopoDS_Shape& Shape, TopLoc_Location& Loc);
  
  //! Returns Shape resulting from given STEP entity (Null if not mapped)
  Standard_EXPORT static TopoDS_Shape FindShape (const Handle(Transfer_TransientProcess)& TransientProcess, const Handle(StepRepr_RepresentationItem)& item);
  
  //! Find CDSR correcponding to the component in the specified assembly
  Standard_EXPORT static Standard_Boolean FindCDSR (const Handle(Transfer_Binder)& ComponentBinder, const Handle(StepShape_ShapeDefinitionRepresentation)& AssemblySDR, Handle(StepShape_ContextDependentShapeRepresentation)& ComponentCDSR);




protected:





private:




friend class STEPConstruct_Tool;
friend class STEPConstruct_UnitContext;
friend class STEPConstruct_Part;
friend class STEPConstruct_Assembly;
friend class STEPConstruct_Styles;
friend class STEPConstruct_ValidationProps;
friend class STEPConstruct_ExternRefs;
friend class STEPConstruct_AP203Context;
friend class STEPConstruct_ContextTool;
friend class STEPConstruct_PointHasher;

};







#endif // _STEPConstruct_HeaderFile
