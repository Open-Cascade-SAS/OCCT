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
#include <Standard_Boolean.hxx>
class StdFail_NotDone;
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
class Transfer_ActorOfTransientProcess;
class TopoDS_Shape;



class StepToTopoDS_Builder  : public StepToTopoDS_Root
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT StepToTopoDS_Builder();
  
  Standard_EXPORT StepToTopoDS_Builder(const Handle(StepShape_ManifoldSolidBrep)& S, const Handle(Transfer_TransientProcess)& TP);
  
  Standard_EXPORT StepToTopoDS_Builder(const Handle(StepShape_BrepWithVoids)& S, const Handle(Transfer_TransientProcess)& TP);
  
  Standard_EXPORT StepToTopoDS_Builder(const Handle(StepShape_FacetedBrep)& S, const Handle(Transfer_TransientProcess)& TP);
  
  Standard_EXPORT StepToTopoDS_Builder(const Handle(StepShape_FacetedBrepAndBrepWithVoids)& S, const Handle(Transfer_TransientProcess)& TP);
  
  Standard_EXPORT StepToTopoDS_Builder(const Handle(StepShape_ShellBasedSurfaceModel)& S, const Handle(Transfer_TransientProcess)& TP, StepToTopoDS_NMTool& NMTool);
  
  Standard_EXPORT StepToTopoDS_Builder(const Handle(StepShape_GeometricSet)& S, const Handle(Transfer_TransientProcess)& TP);
  
  Standard_EXPORT void Init (const Handle(StepShape_ManifoldSolidBrep)& S, const Handle(Transfer_TransientProcess)& TP);
  
  Standard_EXPORT void Init (const Handle(StepShape_BrepWithVoids)& S, const Handle(Transfer_TransientProcess)& TP);
  
  Standard_EXPORT void Init (const Handle(StepShape_FacetedBrep)& S, const Handle(Transfer_TransientProcess)& TP);
  
  Standard_EXPORT void Init (const Handle(StepShape_FacetedBrepAndBrepWithVoids)& S, const Handle(Transfer_TransientProcess)& TP);
  
  Standard_EXPORT void Init (const Handle(StepShape_ShellBasedSurfaceModel)& S, const Handle(Transfer_TransientProcess)& TP, StepToTopoDS_NMTool& NMTool);
  
  Standard_EXPORT void Init (const Handle(StepShape_EdgeBasedWireframeModel)& S, const Handle(Transfer_TransientProcess)& TP);
  
  Standard_EXPORT void Init (const Handle(StepShape_FaceBasedSurfaceModel)& S, const Handle(Transfer_TransientProcess)& TP);
  
  Standard_EXPORT void Init (const Handle(StepShape_GeometricSet)& S, const Handle(Transfer_TransientProcess)& TP, const Handle(Transfer_ActorOfTransientProcess)& RA = NULL, const Standard_Boolean isManifold = Standard_False);
  
  Standard_EXPORT const TopoDS_Shape& Value() const;
  
  Standard_EXPORT StepToTopoDS_BuilderError Error() const;




protected:





private:



  StepToTopoDS_BuilderError myError;
  TopoDS_Shape myResult;


};







#endif // _StepToTopoDS_Builder_HeaderFile
