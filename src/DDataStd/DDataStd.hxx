// Created on: 1997-03-27
// Created by: Denis PASCAL
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _DDataStd_HeaderFile
#define _DDataStd_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Draw_Interpretor.hxx>
#include <Standard_OStream.hxx>
class TDataXtd_Constraint;


//! commands for Standard Attributes.
//! =================================
class DDataStd 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Used to browse tree nodes.
  //! commands
  //! ========
  //! command to set and get modeling attributes
  Standard_EXPORT static void AllCommands (Draw_Interpretor& I);
  
  //! to set and get NamedShape
  Standard_EXPORT static void NamedShapeCommands (Draw_Interpretor& I);
  
  //! to set and get Integer, Real,  Reference, Geometry
  Standard_EXPORT static void BasicCommands (Draw_Interpretor& I);
  
  //! to set and get Datum attributes
  Standard_EXPORT static void DatumCommands (Draw_Interpretor& I);
  
  //! to set and get Constraint and Constraint  attributes
  Standard_EXPORT static void ConstraintCommands (Draw_Interpretor& I);
  
  //! to set and get Objects attributes
  Standard_EXPORT static void ObjectCommands (Draw_Interpretor& I);
  
  //! to display standard attributes
  Standard_EXPORT static void DrawDisplayCommands (Draw_Interpretor& I);
  
  //! to set and get Name attribute
  Standard_EXPORT static void NameCommands (Draw_Interpretor& I);
  
  //! to build, edit and browse an explicit tree of labels
  //! package methods
  //! ===============
  Standard_EXPORT static void TreeCommands (Draw_Interpretor& I);
  
  Standard_EXPORT static void DumpConstraint (const Handle(TDataXtd_Constraint)& C, Standard_OStream& S);




protected:





private:




friend class DDataStd_DrawPresentation;
friend class DDataStd_DrawDriver;
friend class DDataStd_TreeBrowser;

};







#endif // _DDataStd_HeaderFile
