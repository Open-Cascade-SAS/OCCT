// Created on: 1995-03-15
// Created by: Christian CAILLET
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

#ifndef _XSDRAWIGES_HeaderFile
#define _XSDRAWIGES_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Draw_Interpretor.hxx>


//! XSDRAW for IGES : commands IGESSelect, Controller, transfer
class XSDRAWIGES 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Inits IGESSelect commands, for DRAW
  Standard_EXPORT static void InitSelect();
  
  //! Inits IGESToBRep for DRAW
  Standard_EXPORT static void InitToBRep (Draw_Interpretor& theCommands);
  
  //! Inits BRepToIGES for DRAW
  Standard_EXPORT static void InitFromBRep (Draw_Interpretor& theCommands);




protected:





private:





};







#endif // _XSDRAWIGES_HeaderFile
