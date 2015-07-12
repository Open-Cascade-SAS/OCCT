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

#ifndef _SWDRAW_ShapeFix_HeaderFile
#define _SWDRAW_ShapeFix_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Draw_Interpretor.hxx>


//! Contains commands to activate package ShapeFix
//! List of DRAW commands and corresponding functionalities:
//! edgesameparam - ShapeFix::SameParameter
//! settolerance  - ShapeFix_ShapeTolerance
//! stwire        - ShapeFix_Wire
//! reface        - ShapeFix_Face
//! repcurve      - ShapeFix_PCurves
class SWDRAW_ShapeFix 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Loads commands defined in ShapeFix
  Standard_EXPORT static void InitCommands (Draw_Interpretor& theCommands);




protected:





private:





};







#endif // _SWDRAW_ShapeFix_HeaderFile
