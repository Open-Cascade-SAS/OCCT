// Created on: 1991-12-06
// Created by: Remi LEQUETTE
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _HLRTest_HeaderFile
#define _HLRTest_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Draw_Interpretor.hxx>
class HLRAlgo_Projector;
class TopoDS_Shape;
class HLRTopoBRep_OutLiner;


//! This package   is  a test  of  the    Hidden Lines
//! algorithms instantiated on the BRep Data Structure
//! and using the Draw package to display the results.
class HLRTest 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Draw Variable Outliner to test
  //! Set a Projector Variable
  Standard_EXPORT static void Set (const Standard_CString Name, const HLRAlgo_Projector& P);
  
  //! Get a projector variable
  //! Returns false if the variable is not a projector
  Standard_EXPORT static Standard_Boolean GetProjector (Standard_CString& Name, HLRAlgo_Projector& P);
  
  //! Set a OutLiner Variable
  Standard_EXPORT static void Set (const Standard_CString Name, const TopoDS_Shape& S);
  
  //! Get a outliner variable
  //! Returns a null handle if the variable is not a outliner
  Standard_EXPORT static Handle(HLRTopoBRep_OutLiner) GetOutLiner (Standard_CString& Name);
  
  //! Defines commands to test the Hidden Line Removal
  Standard_EXPORT static void Commands (Draw_Interpretor& I);

};

#endif // _HLRTest_HeaderFile
