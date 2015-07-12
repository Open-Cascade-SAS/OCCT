// Created on: 1993-01-09
// Created by: CKY / Contract Toubro-Larsen ( Deepak PRABHU )
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

#ifndef _IGESDefs_HeaderFile
#define _IGESDefs_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

class IGESDefs_Protocol;
class IGESDefs_AssociativityDef;
class IGESDefs_MacroDef;
class IGESDefs_UnitsData;
class IGESDefs_AttributeDef;
class IGESDefs_TabularData;
class IGESDefs_GenericData;
class IGESDefs_AttributeTable;
class IGESDefs_ToolAssociativityDef;
class IGESDefs_ToolMacroDef;
class IGESDefs_ToolUnitsData;
class IGESDefs_ToolAttributeDef;
class IGESDefs_ToolTabularData;
class IGESDefs_ToolGenericData;
class IGESDefs_ToolAttributeTable;
class IGESDefs_Protocol;
class IGESDefs_ReadWriteModule;
class IGESDefs_GeneralModule;
class IGESDefs_SpecificModule;
class IGESDefs_HArray1OfHArray1OfTextDisplayTemplate;


//! To embody general definitions of Entities
//! (Parameters, Tables ...)
class IGESDefs 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Prepares dynamic data (Protocol, Modules) for this package
  Standard_EXPORT static void Init();
  
  //! Returns the Protocol for this Package
  Standard_EXPORT static Handle(IGESDefs_Protocol) Protocol();




protected:





private:




friend class IGESDefs_AssociativityDef;
friend class IGESDefs_MacroDef;
friend class IGESDefs_UnitsData;
friend class IGESDefs_AttributeDef;
friend class IGESDefs_TabularData;
friend class IGESDefs_GenericData;
friend class IGESDefs_AttributeTable;
friend class IGESDefs_ToolAssociativityDef;
friend class IGESDefs_ToolMacroDef;
friend class IGESDefs_ToolUnitsData;
friend class IGESDefs_ToolAttributeDef;
friend class IGESDefs_ToolTabularData;
friend class IGESDefs_ToolGenericData;
friend class IGESDefs_ToolAttributeTable;
friend class IGESDefs_Protocol;
friend class IGESDefs_ReadWriteModule;
friend class IGESDefs_GeneralModule;
friend class IGESDefs_SpecificModule;
friend class IGESDefs_HArray1OfHArray1OfTextDisplayTemplate;

};







#endif // _IGESDefs_HeaderFile
