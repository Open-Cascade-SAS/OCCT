// Created on: 1994-12-16
// Created by: Frederic MAUPAS
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

#ifndef _StepToTopoDS_Builder_HeaderFile
#define _StepToTopoDS_Builder_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepToTopoDS_BuilderError.hxx>
#include <TopoDS_Shape.hxx>
#include <StepToTopoDS_Root.hxx>
#include <Message_ProgressRange.hxx>

class StepShape_ManifoldSolidBrep;
class Transfer_TransientProcess;
class StepShape_BrepWithVoids;
class StepShape_FacetedBrep;
class StepShape_FacetedBrepAndBrepWithVoids;
class StepShape_ShellBasedSurfaceModel;
class StepToTopoDS_NMTool;
class StepShape_GeometricSet;
class StepShape_EdgeBasedWireframeModel;
class StepShape_FaceBasedSurfaceModel;
class StepVisual_TessellatedFace;
class StepVisual_TessellatedShell;
class StepVisual_TessellatedSolid;
class StepVisual_TessellatedSurfaceSet;
class Transfer_ActorOfTransientProcess;

class StepToTopoDS_Builder : public StepToTopoDS_Root
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT StepToTopoDS_Builder();

  Standard_EXPORT void Init(const occ::handle<StepShape_ManifoldSolidBrep>& theManifoldSolid,
                            const occ::handle<Transfer_TransientProcess>&   theTP,
                            const StepData_Factors&      theLocalFactors = StepData_Factors(),
                            const Message_ProgressRange& theProgress     = Message_ProgressRange());

  Standard_EXPORT void Init(const occ::handle<StepShape_BrepWithVoids>&   theBRepWithVoids,
                            const occ::handle<Transfer_TransientProcess>& theTP,
                            const StepData_Factors&      theLocalFactors = StepData_Factors(),
                            const Message_ProgressRange& theProgress     = Message_ProgressRange());

  Standard_EXPORT void Init(const occ::handle<StepShape_FacetedBrep>&     theFB,
                            const occ::handle<Transfer_TransientProcess>& theTP,
                            const StepData_Factors&      theLocalFactors = StepData_Factors(),
                            const Message_ProgressRange& theProgress     = Message_ProgressRange());

  Standard_EXPORT void Init(const occ::handle<StepShape_FacetedBrepAndBrepWithVoids>& theFBABWV,
                            const occ::handle<Transfer_TransientProcess>&             theTP,
                            const StepData_Factors&      theLocalFactors = StepData_Factors(),
                            const Message_ProgressRange& theProgress     = Message_ProgressRange());

  Standard_EXPORT void Init(const occ::handle<StepShape_ShellBasedSurfaceModel>& S,
                            const occ::handle<Transfer_TransientProcess>&        TP,
                            StepToTopoDS_NMTool&                                 NMTool,
                            const StepData_Factors&      theLocalFactors = StepData_Factors(),
                            const Message_ProgressRange& theProgress     = Message_ProgressRange());

  Standard_EXPORT void Init(const occ::handle<StepShape_EdgeBasedWireframeModel>& S,
                            const occ::handle<Transfer_TransientProcess>&         TP,
                            const StepData_Factors& theLocalFactors = StepData_Factors());

  Standard_EXPORT void Init(const occ::handle<StepShape_FaceBasedSurfaceModel>& S,
                            const occ::handle<Transfer_TransientProcess>&       TP,
                            const StepData_Factors& theLocalFactors = StepData_Factors());

  Standard_EXPORT void Init(const occ::handle<StepShape_GeometricSet>&    S,
                            const occ::handle<Transfer_TransientProcess>& TP,
                            const StepData_Factors& theLocalFactors = StepData_Factors(),
                            const occ::handle<Transfer_ActorOfTransientProcess>& RA         = NULL,
                            const bool                                           isManifold = false,
                            const Message_ProgressRange& theProgress = Message_ProgressRange());

  Standard_EXPORT void Init(const occ::handle<StepVisual_TessellatedSolid>& theTSo,
                            const occ::handle<Transfer_TransientProcess>&   theTP,
                            const bool                   theReadTessellatedWhenNoBRepOnly,
                            bool&                        theHasGeom,
                            const StepData_Factors&      theLocalFactors = StepData_Factors(),
                            const Message_ProgressRange& theProgress     = Message_ProgressRange());

  Standard_EXPORT void Init(const occ::handle<StepVisual_TessellatedShell>& theTSh,
                            const occ::handle<Transfer_TransientProcess>&   theTP,
                            const bool                   theReadTessellatedWhenNoBRepOnly,
                            bool&                        theHasGeom,
                            const StepData_Factors&      theLocalFactors = StepData_Factors(),
                            const Message_ProgressRange& theProgress     = Message_ProgressRange());

  Standard_EXPORT void Init(const occ::handle<StepVisual_TessellatedFace>& theTF,
                            const occ::handle<Transfer_TransientProcess>&  theTP,
                            const bool              theReadTessellatedWhenNoBRepOnly,
                            bool&                   theHasGeom,
                            const StepData_Factors& theLocalFactors = StepData_Factors());

  Standard_EXPORT void Init(const occ::handle<StepVisual_TessellatedSurfaceSet>& theTSS,
                            const occ::handle<Transfer_TransientProcess>&        theTP,
                            bool&                                                theHasGeom,
                            const StepData_Factors& theLocalFactors = StepData_Factors());

  Standard_EXPORT const TopoDS_Shape& Value() const;

  Standard_EXPORT StepToTopoDS_BuilderError Error() const;

private:
  StepToTopoDS_BuilderError myError;
  TopoDS_Shape              myResult;
};

#endif // _StepToTopoDS_Builder_HeaderFile
