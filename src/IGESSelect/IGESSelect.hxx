// Created on: 1994-05-31
// Created by: Christian CAILLET
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

#ifndef _IGESSelect_HeaderFile
#define _IGESSelect_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
class IGESData_IGESEntity;
class Interface_Graph;
class IGESSelect_EditHeader;
class IGESSelect_EditDirPart;
class IGESSelect_IGESTypeForm;
class IGESSelect_IGESName;
class IGESSelect_SignStatus;
class IGESSelect_SignLevelNumber;
class IGESSelect_SignColor;
class IGESSelect_CounterOfLevelNumber;
class IGESSelect_ViewSorter;
class IGESSelect_DispPerSingleView;
class IGESSelect_DispPerDrawing;
class IGESSelect_SelectVisibleStatus;
class IGESSelect_SelectSubordinate;
class IGESSelect_SelectLevelNumber;
class IGESSelect_SelectName;
class IGESSelect_SelectFromSingleView;
class IGESSelect_SelectFromDrawing;
class IGESSelect_SelectSingleViewFrom;
class IGESSelect_SelectDrawingFrom;
class IGESSelect_SelectBypassGroup;
class IGESSelect_SelectBypassSubfigure;
class IGESSelect_SelectBasicGeom;
class IGESSelect_SelectFaces;
class IGESSelect_SelectPCurves;
class IGESSelect_ModelModifier;
class IGESSelect_FileModifier;
class IGESSelect_FloatFormat;
class IGESSelect_AddFileComment;
class IGESSelect_UpdateFileName;
class IGESSelect_UpdateCreationDate;
class IGESSelect_UpdateLastChange;
class IGESSelect_SetVersion5;
class IGESSelect_SetGlobalParameter;
class IGESSelect_AutoCorrect;
class IGESSelect_ComputeStatus;
class IGESSelect_RebuildDrawings;
class IGESSelect_RebuildGroups;
class IGESSelect_AddGroup;
class IGESSelect_ChangeLevelNumber;
class IGESSelect_ChangeLevelList;
class IGESSelect_SplineToBSpline;
class IGESSelect_RemoveCurves;
class IGESSelect_SetLabel;
class IGESSelect_WorkLibrary;
class IGESSelect_Activator;
class IGESSelect_Dumper;


//! This package defines the library of the most used tools for
//! IGES Files : Selections & Modifiers specific to the IGES norm,
//! and the most needed converters
class IGESSelect 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Simply gives a prompt for a conversational action on standard
  //! input/output. Returns the status of a
  Standard_EXPORT static void Run();
  
  //! Gives a quick analysis of an IGES Entity in the context of a
  //! model (i.e. a File) described by a Graph.
  //! Returned values are :
  //! <sup> : the most meaningfull super entity, if any (else Null)
  //! <index> : meaningfull index relating to super entity, if any
  //! <returned> : a status which helps exploitation of <sup>, by
  //! giving a case
  //! (normally, types of <ent> and <sup> should suffice to
  //! known the case)
  Standard_EXPORT static Standard_Integer WhatIges (const Handle(IGESData_IGESEntity)& ent, const Interface_Graph& G, Handle(IGESData_IGESEntity)& sup, Standard_Integer& index);




protected:





private:




friend class IGESSelect_EditHeader;
friend class IGESSelect_EditDirPart;
friend class IGESSelect_IGESTypeForm;
friend class IGESSelect_IGESName;
friend class IGESSelect_SignStatus;
friend class IGESSelect_SignLevelNumber;
friend class IGESSelect_SignColor;
friend class IGESSelect_CounterOfLevelNumber;
friend class IGESSelect_ViewSorter;
friend class IGESSelect_DispPerSingleView;
friend class IGESSelect_DispPerDrawing;
friend class IGESSelect_SelectVisibleStatus;
friend class IGESSelect_SelectSubordinate;
friend class IGESSelect_SelectLevelNumber;
friend class IGESSelect_SelectName;
friend class IGESSelect_SelectFromSingleView;
friend class IGESSelect_SelectFromDrawing;
friend class IGESSelect_SelectSingleViewFrom;
friend class IGESSelect_SelectDrawingFrom;
friend class IGESSelect_SelectBypassGroup;
friend class IGESSelect_SelectBypassSubfigure;
friend class IGESSelect_SelectBasicGeom;
friend class IGESSelect_SelectFaces;
friend class IGESSelect_SelectPCurves;
friend class IGESSelect_ModelModifier;
friend class IGESSelect_FileModifier;
friend class IGESSelect_FloatFormat;
friend class IGESSelect_AddFileComment;
friend class IGESSelect_UpdateFileName;
friend class IGESSelect_UpdateCreationDate;
friend class IGESSelect_UpdateLastChange;
friend class IGESSelect_SetVersion5;
friend class IGESSelect_SetGlobalParameter;
friend class IGESSelect_AutoCorrect;
friend class IGESSelect_ComputeStatus;
friend class IGESSelect_RebuildDrawings;
friend class IGESSelect_RebuildGroups;
friend class IGESSelect_AddGroup;
friend class IGESSelect_ChangeLevelNumber;
friend class IGESSelect_ChangeLevelList;
friend class IGESSelect_SplineToBSpline;
friend class IGESSelect_RemoveCurves;
friend class IGESSelect_SetLabel;
friend class IGESSelect_WorkLibrary;
friend class IGESSelect_Activator;
friend class IGESSelect_Dumper;

};







#endif // _IGESSelect_HeaderFile
