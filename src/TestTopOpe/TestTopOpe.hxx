// Created on: 1996-10-21
// Created by: Jean Yves LEBEY
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _TestTopOpe_HeaderFile
#define _TestTopOpe_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Draw_Interpretor.hxx>
class TopOpeBRepBuild_HBuilder;
class TopOpeBRepDS_HDataStructure;
class TopoDS_Shape;



class TestTopOpe 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Defines all Top. Ope. test commands
  Standard_EXPORT static void AllCommands (Draw_Interpretor& I);
  
  Standard_EXPORT static void TOPOCommands (Draw_Interpretor& I);
  
  Standard_EXPORT static void BOOPCommands (Draw_Interpretor& I);
  
  //! commands on a HDS involved in topological operations
  Standard_EXPORT static void HDSCommands (Draw_Interpretor& I);
  
  //! Defines the HBuilder on which BOOPCommands will operate.
  Standard_EXPORT static void CurrentHB (const Handle(TopOpeBRepBuild_HBuilder)& HDS);
  
  //! Defines the HDS on which HDSCommands/BOOPCommands will operate.
  Standard_EXPORT static void CurrentDS (const Handle(TopOpeBRepDS_HDataStructure)& HDS);
  
  //! Defines current shapes of current topological operation
  Standard_EXPORT static void Shapes (const TopoDS_Shape& S1, const TopoDS_Shape& S2);
  
  Standard_EXPORT static void MesureCommands (Draw_Interpretor& I);
  
  Standard_EXPORT static void CORCommands (Draw_Interpretor& I);
  
  Standard_EXPORT static void DSACommands (Draw_Interpretor& I);
  
  Standard_EXPORT static void OtherCommands (Draw_Interpretor& I);
  
  //! Loads all Draw commands of TKDrawDEB. Used for plugin.
  Standard_EXPORT static void Factory (Draw_Interpretor& theDI);




protected:





private:





};







#endif // _TestTopOpe_HeaderFile
