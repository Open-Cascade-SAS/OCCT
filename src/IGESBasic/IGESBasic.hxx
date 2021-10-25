// Created on: 1993-01-09
// Created by: CKY / Contract Toubro-Larsen (Arun MENON)
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _IGESBasic_HeaderFile
#define _IGESBasic_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

class IGESBasic_Protocol;


//! This package represents basic entities from IGES
class IGESBasic 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Prepares dynqmic data (Protocol, Modules) for this package
  Standard_EXPORT static void Init();
  
  //! Returns the Protocol for this Package
  Standard_EXPORT static Handle(IGESBasic_Protocol) Protocol();




protected:





private:




friend class IGESBasic_SubfigureDef;
friend class IGESBasic_Group;
friend class IGESBasic_GroupWithoutBackP;
friend class IGESBasic_SingleParent;
friend class IGESBasic_ExternalRefFileIndex;
friend class IGESBasic_OrderedGroup;
friend class IGESBasic_OrderedGroupWithoutBackP;
friend class IGESBasic_Hierarchy;
friend class IGESBasic_ExternalReferenceFile;
friend class IGESBasic_Name;
friend class IGESBasic_AssocGroupType;
friend class IGESBasic_SingularSubfigure;
friend class IGESBasic_ExternalRefFileName;
friend class IGESBasic_ExternalRefFile;
friend class IGESBasic_ExternalRefName;
friend class IGESBasic_ExternalRefLibName;
friend class IGESBasic_ToolSubfigureDef;
friend class IGESBasic_ToolGroup;
friend class IGESBasic_ToolGroupWithoutBackP;
friend class IGESBasic_ToolSingleParent;
friend class IGESBasic_ToolExternalRefFileIndex;
friend class IGESBasic_ToolOrderedGroup;
friend class IGESBasic_ToolOrderedGroupWithoutBackP;
friend class IGESBasic_ToolHierarchy;
friend class IGESBasic_ToolExternalReferenceFile;
friend class IGESBasic_ToolName;
friend class IGESBasic_ToolAssocGroupType;
friend class IGESBasic_ToolSingularSubfigure;
friend class IGESBasic_ToolExternalRefFileName;
friend class IGESBasic_ToolExternalRefFile;
friend class IGESBasic_ToolExternalRefName;
friend class IGESBasic_ToolExternalRefLibName;
friend class IGESBasic_Protocol;
friend class IGESBasic_ReadWriteModule;
friend class IGESBasic_GeneralModule;
friend class IGESBasic_SpecificModule;
friend class IGESBasic_HArray1OfHArray1OfInteger;
friend class IGESBasic_HArray1OfHArray1OfReal;
friend class IGESBasic_HArray1OfHArray1OfXY;
friend class IGESBasic_HArray1OfHArray1OfXYZ;
friend class IGESBasic_HArray1OfHArray1OfIGESEntity;

};







#endif // _IGESBasic_HeaderFile
