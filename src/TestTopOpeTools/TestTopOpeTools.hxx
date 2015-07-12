// Created on: 1994-04-26
// Created by: Jean Yves LEBEY
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

#ifndef _TestTopOpeTools_HeaderFile
#define _TestTopOpeTools_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Draw_Interpretor.hxx>
class TestTopOpeTools_Mesure;



//! Provide Trace control on packages involved in
//! topological operations kernel, from Draw command interpretor.
//!
//! They may be used by users of topological operation kernel, such as :
//!
//! * topological operation performer,
//! * hidden line removal performer,
//! * fillet, chamfer performer
//!
//! Trace  control   consists  in  management  of
//! control functions,  activating/desactivating  execution  of
//! instructions considered as purely PASSIVE code,
//! performing dumps, prints, and drawing of internal objects
//! dealed by some topological operation packages.
//!
//! All of the Trace controls in top.ope. kernel
//! are enclosed by the C conditional compilation statements :
//! #ifdef OCCT_DEBUG  ...  #endif
//!
//! The "Traced" packages of topological operation kernel are :
//! - TopBuild
//! - TopOpeBRepTool
//! - TopOpeBRepDS
//!
//! How to use the Trace :
//! ----------------------
//!
//! In a Test.cxx program using  the Draw command interpretor, dealing
//! a set of commands theCommands (Draw_CommandManager theCommands)  :
//!
//! TestTopOpeTools::TraceCommands();
//!
//! Compile your Test.cxx, run and then, under the command manager prompt :
//!
//! Trace : prints the list of the Trace flags available on top.ope. kernel
//! Trace <flag> : activates Trace code of <flag>
//! Trace <flag> <1 | 0> : activates/desactivates Trace code of <flag>
//! Trace <1 | 0> : activates/desactivates all Trace code of top.ope. kernel
//!
//! How to add Traces :
//! -------------------
//!
//! It it possible to add your own "Traced" portions of code in your code.
//! In your test program, simply add :
//!
//! #include <TestTopOpeTools_AddTrace.hxx>
//!
//! and see the file TestTopOpeTools_Trace.hxx for explanations.
class TestTopOpeTools 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Defines all topological operation test commands
  Standard_EXPORT static void AllCommands (Draw_Interpretor& I);
  
  //! Defines the dump commands on
  //! topological operation packages.
  Standard_EXPORT static void TraceCommands (Draw_Interpretor& I);
  
  //! Defines auxiliary commands
  Standard_EXPORT static void OtherCommands (Draw_Interpretor& I);




protected:





private:




friend class TestTopOpeTools_Mesure;

};







#endif // _TestTopOpeTools_HeaderFile
