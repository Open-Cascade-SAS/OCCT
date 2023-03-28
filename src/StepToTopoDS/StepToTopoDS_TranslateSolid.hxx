// Copyright (c) 2022 OPEN CASCADE SAS
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

#ifndef _StepToTopoDS_TranslateSolid_HeaderFile
#define _StepToTopoDS_TranslateSolid_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepToTopoDS_TranslateSolidError.hxx>
#include <TopoDS_Shape.hxx>
#include <StepToTopoDS_Root.hxx>
#include <Message_ProgressRange.hxx>

class StepShape_ConnectedFaceSet;
class StepVisual_TessellatedSolid;
class StepToTopoDS_Tool;
class StepToTopoDS_NMTool;
class Transfer_TransientProcess;

class StepToTopoDS_TranslateSolid : public StepToTopoDS_Root
{
public:

  DEFINE_STANDARD_ALLOC


  Standard_EXPORT StepToTopoDS_TranslateSolid();

  Standard_EXPORT void Init(const Handle(StepVisual_TessellatedSolid)& theTSo,
                            const Handle(Transfer_TransientProcess)& theTP,
                            StepToTopoDS_Tool& theTool,
                            StepToTopoDS_NMTool& theNMTool,
                            const Standard_Boolean theReadTessellatedWhenNoBRepOnly,
                            Standard_Boolean& theHasGeom,
                            const StepData_Factors& theLocalFactors,
                            const Message_ProgressRange& theProgress = Message_ProgressRange());

  Standard_EXPORT const TopoDS_Shape& Value() const;

  Standard_EXPORT StepToTopoDS_TranslateSolidError Error() const;

protected:

private:

  StepToTopoDS_TranslateSolidError myError;
  TopoDS_Shape myResult;

};

#endif // _StepToTopoDS_TranslateSolid_HeaderFile
