// Created on: 1999-03-09
// Created by: data exchange team
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

#ifndef _SWDRAW_ShapeUpgrade_HeaderFile
#define _SWDRAW_ShapeUpgrade_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Draw_Interpretor.hxx>


//! Contains commands to activate package ShapeUpgrade
//! List of DRAW commands and corresponding functionalities:
//! DT_ShapeDivide         - ShapeUpgrade_ShapeDivide
//! DT_PlaneDividedFace    - ShapeUpgrade_PlaneDividedFace
//! DT_PlaneGridShell      - ShapeUpgrade_PlaneGridShell
//! DT_PlaneFaceCommon     - ShapeUpgrade_PlaneFaceCommon
//! DT_Split2dCurve        - ShapeUpgrade_Split2dCurve
//! DT_SplitCurve          - ShapeUpgrade_SplitCurve
//! DT_SplitSurface        - ShapeUpgrade_SplitSurface
//! DT_SupportModification - ShapeUpgrade_DataMapOfShapeSurface
//! DT_Debug               - ShapeUpgrade::SetDebug
//! shellsolid             - ShapeAnalysis_Shell/ShapeUpgrade_ShellSewing
class SWDRAW_ShapeUpgrade 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Loads commands defined in ShapeUpgrade
  Standard_EXPORT static void InitCommands (Draw_Interpretor& theCommands);




protected:





private:





};







#endif // _SWDRAW_ShapeUpgrade_HeaderFile
