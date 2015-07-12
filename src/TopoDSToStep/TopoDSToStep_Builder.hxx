// Created on: 1994-11-25
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

#ifndef _TopoDSToStep_Builder_HeaderFile
#define _TopoDSToStep_Builder_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDSToStep_BuilderError.hxx>
#include <TopoDSToStep_Root.hxx>
class StepShape_TopologicalRepresentationItem;
class StdFail_NotDone;
class TopoDS_Shape;
class TopoDSToStep_Tool;
class Transfer_FinderProcess;


//! This builder Class provides services to build
//! a ProSTEP Shape model from a Cas.Cad BRep.
class TopoDSToStep_Builder  : public TopoDSToStep_Root
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT TopoDSToStep_Builder();
  
  Standard_EXPORT TopoDSToStep_Builder(const TopoDS_Shape& S, TopoDSToStep_Tool& T, const Handle(Transfer_FinderProcess)& FP);
  
  Standard_EXPORT void Init (const TopoDS_Shape& S, TopoDSToStep_Tool& T, const Handle(Transfer_FinderProcess)& FP);
  
  Standard_EXPORT TopoDSToStep_BuilderError Error() const;
  
  Standard_EXPORT const Handle(StepShape_TopologicalRepresentationItem)& Value() const;




protected:





private:



  Handle(StepShape_TopologicalRepresentationItem) myResult;
  TopoDSToStep_BuilderError myError;


};







#endif // _TopoDSToStep_Builder_HeaderFile
