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


#include <HeaderSection_FileDescription.hxx>
#include <HeaderSection_FileName.hxx>
#include <HeaderSection_FileSchema.hxx>
#include <HeaderSection_HeaderRecognizer.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(HeaderSection_HeaderRecognizer,StepData_FileRecognizer)

static TCollection_AsciiString reco_FileName    ("FILE_NAME");
static TCollection_AsciiString reco_FileSchema ("FILE_SCHEMA");
static TCollection_AsciiString reco_FileDescription ("FILE_DESCRIPTION");



HeaderSection_HeaderRecognizer::HeaderSection_HeaderRecognizer ()
{ }

void HeaderSection_HeaderRecognizer::Eval
  (const TCollection_AsciiString& key)
{
  if (key.IsEqual(reco_FileName)) { 
    SetOK(new HeaderSection_FileName);
    return;
  }
  if (key.IsEqual(reco_FileSchema)) { 
    SetOK(new HeaderSection_FileSchema);
    return;
  }
  if (key.IsEqual(reco_FileDescription)) { 
    SetOK(new HeaderSection_FileDescription);
    return;
  } 
}
