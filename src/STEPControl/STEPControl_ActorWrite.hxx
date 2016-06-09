// Created on: 1995-01-31
// Created by: Dieter THIEMANN
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _STEPControl_ActorWrite_HeaderFile
#define _STEPControl_ActorWrite_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <STEPConstruct_ContextTool.hxx>
#include <Transfer_ActorOfFinderProcess.hxx>
#include <Standard_Boolean.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <STEPControl_StepModelType.hxx>
class Transfer_Finder;
class Transfer_Binder;
class Transfer_FinderProcess;
class StepShape_ShapeDefinitionRepresentation;
class StepGeom_Axis2Placement3d;
class TopoDS_Shape;
class StepShape_NonManifoldSurfaceShapeRepresentation;


class STEPControl_ActorWrite;
DEFINE_STANDARD_HANDLE(STEPControl_ActorWrite, Transfer_ActorOfFinderProcess)

//! This class performs the transfer of a Shape from TopoDS
//! to AP203 or AP214 (CD2 or DIS)
class STEPControl_ActorWrite : public Transfer_ActorOfFinderProcess
{

public:

  
  Standard_EXPORT STEPControl_ActorWrite();
  
  Standard_EXPORT virtual Standard_Boolean Recognize (const Handle(Transfer_Finder)& start) Standard_OVERRIDE;
  
  Standard_EXPORT virtual Handle(Transfer_Binder) Transfer (const Handle(Transfer_Finder)& start, const Handle(Transfer_FinderProcess)& FP) Standard_OVERRIDE;
  
  Standard_EXPORT Handle(Transfer_Binder) TransferSubShape (const Handle(Transfer_Finder)& start, const Handle(StepShape_ShapeDefinitionRepresentation)& SDR, Handle(StepGeom_Axis2Placement3d)& AX1, const Handle(Transfer_FinderProcess)& FP, const Handle(TopTools_HSequenceOfShape)& shapeGroup = NULL, const Standard_Boolean isManifold = Standard_True);
  
  Standard_EXPORT Handle(Transfer_Binder) TransferShape (const Handle(Transfer_Finder)& start, const Handle(StepShape_ShapeDefinitionRepresentation)& SDR, const Handle(Transfer_FinderProcess)& FP, const Handle(TopTools_HSequenceOfShape)& shapeGroup = NULL, const Standard_Boolean isManifold = Standard_True);
  
  Standard_EXPORT Handle(Transfer_Binder) TransferCompound (const Handle(Transfer_Finder)& start, const Handle(StepShape_ShapeDefinitionRepresentation)& SDR, const Handle(Transfer_FinderProcess)& FP);
  
  Standard_EXPORT void SetMode (const STEPControl_StepModelType M);
  
  Standard_EXPORT STEPControl_StepModelType Mode() const;
  
  Standard_EXPORT void SetGroupMode (const Standard_Integer mode);
  
  Standard_EXPORT Standard_Integer GroupMode() const;
  
  Standard_EXPORT void SetTolerance (const Standard_Real Tol);
  
  //! Customizable method to check whether shape S should
  //! be written as assembly or not
  //! Default implementation uses flag GroupMode and analyses
  //! the shape itself
  //! NOTE: this method can modify shape
  Standard_EXPORT virtual Standard_Boolean IsAssembly (TopoDS_Shape& S) const;




  DEFINE_STANDARD_RTTIEXT(STEPControl_ActorWrite,Transfer_ActorOfFinderProcess)

protected:




private:

  
  //! Non-manifold shapes are stored in NMSSR group
  //! (NON_MANIFOLD_SURFACE_SHAPE_REPRESENTATION).
  //! Use this method to get the corresponding NMSSR (or
  //! to create a new one if doesn't exist yet)
  //! (ssv; 13.11.2010)
  Standard_EXPORT Handle(StepShape_NonManifoldSurfaceShapeRepresentation) getNMSSRForGroup (const Handle(TopTools_HSequenceOfShape)& shapeGroup, const Handle(Transfer_FinderProcess)& FP, Standard_Boolean& isNMSSRCreated) const;
  
  //! bind already written shared faces to STEP entity for non-manifold
  Standard_EXPORT void mergeInfoForNM(const Handle(Transfer_FinderProcess)& theFP, const Handle(Standard_Transient) &theInfo) const;

  Standard_Integer mygroup;
  Standard_Real mytoler;
  STEPConstruct_ContextTool myContext;


};







#endif // _STEPControl_ActorWrite_HeaderFile
