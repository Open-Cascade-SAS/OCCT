// Created on: 1992-04-06
// Created by: Christian CAILLET
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _IGESData_HeaderFile
#define _IGESData_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

class IGESData_Protocol;


//! basic description of an IGES Interface
class IGESData 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Prepares General dynamic data used for IGESData specifically :
  //! Protocol and Modules, which treat UndefinedEntity
  Standard_EXPORT static void Init();
  
  //! Returns a Protocol from IGESData (avoids to create it)
  Standard_EXPORT static Handle(IGESData_Protocol) Protocol();




protected:





private:




friend class IGESData_Protocol;
friend class IGESData_IGESModel;
friend class IGESData_IGESEntity;
friend class IGESData_LineFontEntity;
friend class IGESData_LevelListEntity;
friend class IGESData_TransfEntity;
friend class IGESData_ViewKindEntity;
friend class IGESData_LabelDisplayEntity;
friend class IGESData_ColorEntity;
friend class IGESData_NameEntity;
friend class IGESData_SingleParentEntity;
friend class IGESData_UndefinedEntity;
friend class IGESData_FreeFormatEntity;
friend class IGESData_GlobalSection;
friend class IGESData_DefSwitch;
friend class IGESData_DirChecker;
friend class IGESData_IGESReaderData;
friend class IGESData_IGESReaderTool;
friend class IGESData_ParamReader;
friend class IGESData_ParamCursor;
friend class IGESData_DirPart;
friend class IGESData_IGESType;
friend class IGESData_FileRecognizer;
friend class IGESData_IGESWriter;
friend class IGESData_IGESDumper;
friend class IGESData_BasicEditor;
friend class IGESData_ToolLocation;
friend class IGESData_GeneralModule;
friend class IGESData_DefaultGeneral;
friend class IGESData_ReadWriteModule;
friend class IGESData_SpecificModule;
friend class IGESData_DefaultSpecific;
friend class IGESData_FileProtocol;
friend class IGESData_WriterLib;
friend class IGESData_SpecificLib;
friend class IGESData_GlobalNodeOfWriterLib;
friend class IGESData_NodeOfWriterLib;
friend class IGESData_GlobalNodeOfSpecificLib;
friend class IGESData_NodeOfSpecificLib;

};







#endif // _IGESData_HeaderFile
