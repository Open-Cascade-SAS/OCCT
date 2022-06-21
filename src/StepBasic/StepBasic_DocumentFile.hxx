// Created on: 2000-05-11
// Created by: data exchange team
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#ifndef _StepBasic_DocumentFile_HeaderFile
#define _StepBasic_DocumentFile_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepBasic_Document.hxx>
class StepBasic_CharacterizedObject;
class TCollection_HAsciiString;
class StepBasic_DocumentType;


class StepBasic_DocumentFile;
DEFINE_STANDARD_HANDLE(StepBasic_DocumentFile, StepBasic_Document)

//! Representation of STEP entity DocumentFile
class StepBasic_DocumentFile : public StepBasic_Document
{

public:

  
  //! Empty constructor
  Standard_EXPORT StepBasic_DocumentFile();
  
  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aDocument_Id, const Handle(TCollection_HAsciiString)& aDocument_Name, const Standard_Boolean hasDocument_Description, const Handle(TCollection_HAsciiString)& aDocument_Description, const Handle(StepBasic_DocumentType)& aDocument_Kind, const Handle(TCollection_HAsciiString)& aCharacterizedObject_Name, const Standard_Boolean hasCharacterizedObject_Description, const Handle(TCollection_HAsciiString)& aCharacterizedObject_Description);
  
  //! Returns data for supertype CharacterizedObject
  Standard_EXPORT Handle(StepBasic_CharacterizedObject) CharacterizedObject() const;
  
  //! Set data for supertype CharacterizedObject
  Standard_EXPORT void SetCharacterizedObject (const Handle(StepBasic_CharacterizedObject)& CharacterizedObject);




  DEFINE_STANDARD_RTTIEXT(StepBasic_DocumentFile,StepBasic_Document)

protected:




private:


  Handle(StepBasic_CharacterizedObject) theCharacterizedObject;


};







#endif // _StepBasic_DocumentFile_HeaderFile
