// Created on: 1994-06-27
// Created by: Frederic MAUPAS
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

#ifndef _HeaderSection_HeaderRecognizer_HeaderFile
#define _HeaderSection_HeaderRecognizer_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepData_FileRecognizer.hxx>
class TCollection_AsciiString;


class HeaderSection_HeaderRecognizer;
DEFINE_STANDARD_HANDLE(HeaderSection_HeaderRecognizer, StepData_FileRecognizer)

//! Recognizes STEP Standard Header Entities
//! (FileName, FileDescription, FileSchema)
class HeaderSection_HeaderRecognizer : public StepData_FileRecognizer
{

public:

  
  Standard_EXPORT HeaderSection_HeaderRecognizer();




  DEFINE_STANDARD_RTTIEXT(HeaderSection_HeaderRecognizer,StepData_FileRecognizer)

protected:

  
  //! Recognizes data types of Header STEP Standard
  Standard_EXPORT void Eval (const TCollection_AsciiString& key) Standard_OVERRIDE;



private:




};







#endif // _HeaderSection_HeaderRecognizer_HeaderFile
