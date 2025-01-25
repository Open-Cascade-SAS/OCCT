// Created:	Wed Nov  3 14:39:28 1999
// Author:	Andrey BETENEV
// Copyright (c) 1999-2020 OPEN CASCADE SAS
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

#include <Express.hxx>

#include <Express_Schema.hxx>
#include <OSD_Process.hxx>
#include <OSD_Environment.hxx>
#include <Quantity_Date.hxx>
#include <TCollection_AsciiString.hxx>

//=================================================================================================

Handle(Express_Schema)& Express::Schema()
{
  static Handle(Express_Schema) aSchema;
  return aSchema;
}

//=======================================================================
// function : WriteFileStamp
// purpose  : Write header of HXX or CXX file
//=======================================================================

void Express::WriteFileStamp(Standard_OStream& theOS)
{
  static const char* EC_VERSION = "2.0";

  OSD_Process             aProcess;
  Quantity_Date           aCurTime = aProcess.SystemDate();
  OSD_Environment         anEnv("EXPTOCAS_TIME");
  TCollection_AsciiString aTimeString = anEnv.Value();
  if (aTimeString.IsEmpty())
  {
    aTimeString += aCurTime.Year();
    aTimeString += "-";
    aTimeString += aCurTime.Month();
    aTimeString += "-";
    aTimeString += aCurTime.Day();
  }

  theOS << "// Created on : " << aTimeString
        << "\n"
           "// Created by: "
        << aProcess.UserName()
        << "\n"
           "// Generator: ExpToCasExe (EXPRESS -> CASCADE/XSTEP Translator) V"
        << EC_VERSION
        << "\n"
           "// Copyright (c) Open CASCADE "
        << aCurTime.Year()
        << "\n"
           "//\n"
           "// This file is part of Open CASCADE Technology software library.\n"
           "//\n"
           "// This library is free software; you can redistribute it and/or modify it under\n"
           "// the terms of the GNU Lesser General Public License version 2.1 as published\n"
           "// by the Free Software Foundation, with special exception defined in the file\n"
           "// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT\n"
           "// distribution for complete text of the license and disclaimer of any warranty.\n"
           "//\n"
           "// Alternatively, this file may be used under the terms of Open CASCADE\n"
           "// commercial license or contractual agreement.\n"
           "\n";
}

//=================================================================================================

void Express::WriteMethodStamp(Standard_OStream& theOS, const TCollection_AsciiString& theName)
{
  theOS << "\n"
           "//=======================================================================\n"
           "// function : "
        << theName
        << "\n"
           "// purpose  :\n"
           "//=======================================================================\n"
           "\n";
}

//=================================================================================================

TCollection_AsciiString Express::ToStepName(const TCollection_AsciiString& theName)
{
  TCollection_AsciiString aStepName(theName);
  for (Standard_Integer i = 2; i <= aStepName.Length(); i++)
  {
    if (isupper(aStepName.Value(i)))
    {
      aStepName.Insert(i++, '_');
    }
  }
  aStepName.LowerCase();

  return aStepName;
}

//=================================================================================================

TCollection_AsciiString Express::EnumPrefix(const TCollection_AsciiString& theName)
{
  TCollection_AsciiString aStepName;
  for (Standard_Integer i = 1; i <= theName.Length(); i++)
  {
    if (isupper(theName.Value(i)))
    {
      aStepName += theName.Value(i);
    }
  }
  aStepName.LowerCase();

  return aStepName;
}
