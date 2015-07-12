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

#ifndef _StepToTopoDS_TranslateFace_HeaderFile
#define _StepToTopoDS_TranslateFace_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepToTopoDS_TranslateFaceError.hxx>
#include <TopoDS_Shape.hxx>
#include <StepToTopoDS_Root.hxx>
class StdFail_NotDone;
class StepShape_FaceSurface;
class StepToTopoDS_Tool;
class StepToTopoDS_NMTool;
class TopoDS_Shape;



class StepToTopoDS_TranslateFace  : public StepToTopoDS_Root
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT StepToTopoDS_TranslateFace();
  
  Standard_EXPORT StepToTopoDS_TranslateFace(const Handle(StepShape_FaceSurface)& FS, StepToTopoDS_Tool& T, StepToTopoDS_NMTool& NMTool);
  
  Standard_EXPORT void Init (const Handle(StepShape_FaceSurface)& FS, StepToTopoDS_Tool& T, StepToTopoDS_NMTool& NMTool);
  
  Standard_EXPORT const TopoDS_Shape& Value() const;
  
  Standard_EXPORT StepToTopoDS_TranslateFaceError Error() const;




protected:





private:



  StepToTopoDS_TranslateFaceError myError;
  TopoDS_Shape myResult;


};







#endif // _StepToTopoDS_TranslateFace_HeaderFile
