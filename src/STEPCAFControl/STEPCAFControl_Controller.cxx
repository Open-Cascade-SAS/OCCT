// Created on: 2000-10-05
// Created by: Andrey BETENEV
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


#include <Interface_Static.hxx>
#include <Standard_Type.hxx>
#include <STEPCAFControl_ActorWrite.hxx>
#include <STEPCAFControl_Controller.hxx>
#include <XSAlgo.hxx>

IMPLEMENT_STANDARD_RTTIEXT(STEPCAFControl_Controller,STEPControl_Controller)

//=======================================================================
//function : STEPCAFControl_Controller
//purpose  : 
//=======================================================================
STEPCAFControl_Controller::STEPCAFControl_Controller ()
{
  Handle(STEPCAFControl_ActorWrite) ActWrite = new STEPCAFControl_ActorWrite;
  myAdaptorWrite = ActWrite;
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

Standard_Boolean STEPCAFControl_Controller::Init ()
{
  static Standard_Boolean inic = Standard_False;
  if (inic) return Standard_True;
  inic = Standard_True;
  // self-registering
  Handle(STEPCAFControl_Controller) STEPCTL = new STEPCAFControl_Controller;
  // do XSAlgo::Init, cause it does not called before.
  XSAlgo::Init();
  // do something to avoid warnings...
  STEPCTL->AutoRecord();

  //-----------------------------------------------------------
  // Few variables for advanced control of translation process
  //-----------------------------------------------------------

  // Indicates whether to write sub-shape names to 'Name' attributes of
  // STEP Representation Items
  Interface_Static::Init   ("stepcaf", "write.stepcaf.subshapes.name", 'e', "");
  Interface_Static::Init   ("stepcaf", "write.stepcaf.subshapes.name", '&', "enum 0");
  Interface_Static::Init   ("stepcaf", "write.stepcaf.subshapes.name", '&', "eval Off"); // 0
  Interface_Static::Init   ("stepcaf", "write.stepcaf.subshapes.name", '&', "eval On");  // 1
  Interface_Static::SetIVal("write.stepcaf.subshapes.name", 0); // Disabled by default

  // Indicates whether to read sub-shape names from 'Name' attributes of
  // STEP Representation Items
  Interface_Static::Init   ("stepcaf", "read.stepcaf.subshapes.name", 'e', "");
  Interface_Static::Init   ("stepcaf", "read.stepcaf.subshapes.name", '&', "enum 0");
  Interface_Static::Init   ("stepcaf", "read.stepcaf.subshapes.name", '&', "eval Off"); // 0
  Interface_Static::Init   ("stepcaf", "read.stepcaf.subshapes.name", '&', "eval On");  // 1
  Interface_Static::SetIVal("read.stepcaf.subshapes.name", 0); // Disabled by default

  // STEP file encoding for names translation
  // Note: the numbers should be consistent with Resource_FormatType enumeration
  Interface_Static::Init ("step", "read.stepcaf.codepage", 'e', "");
  Interface_Static::Init ("step", "read.stepcaf.codepage", '&', "enum 0");
  Interface_Static::Init ("step", "read.stepcaf.codepage", '&', "eval SJIS");         // Resource_FormatType_SJIS
  Interface_Static::Init ("step", "read.stepcaf.codepage", '&', "eval EUC");          // Resource_FormatType_EUC
  Interface_Static::Init ("step", "read.stepcaf.codepage", '&', "eval ANSI");         // Resource_FormatType_ANSI
  Interface_Static::Init ("step", "read.stepcaf.codepage", '&', "eval GB");           // Resource_FormatType_GB
  Interface_Static::Init ("step", "read.stepcaf.codepage", '&', "eval UTF8");         // Resource_FormatType_UTF8
  Interface_Static::Init ("step", "read.stepcaf.codepage", '&', "eval SystemLocale"); // Resource_FormatType_SystemLocale
  Interface_Static::Init ("step", "read.stepcaf.codepage", '&', "eval CP1250");       // Resource_FormatType_CP1250
  Interface_Static::Init ("step", "read.stepcaf.codepage", '&', "eval CP1251");       // Resource_FormatType_CP1251
  Interface_Static::Init ("step", "read.stepcaf.codepage", '&', "eval CP1252");       // Resource_FormatType_CP1252
  Interface_Static::Init ("step", "read.stepcaf.codepage", '&', "eval CP1253");       // Resource_FormatType_CP1253
  Interface_Static::Init ("step", "read.stepcaf.codepage", '&', "eval CP1254");       // Resource_FormatType_CP1254
  Interface_Static::Init ("step", "read.stepcaf.codepage", '&', "eval CP1255");       // Resource_FormatType_CP1255
  Interface_Static::Init ("step", "read.stepcaf.codepage", '&', "eval CP1256");       // Resource_FormatType_CP1256
  Interface_Static::Init ("step", "read.stepcaf.codepage", '&', "eval CP1257");       // Resource_FormatType_CP1257
  Interface_Static::Init ("step", "read.stepcaf.codepage", '&', "eval CP1258");       // Resource_FormatType_CP1258
  Interface_Static::SetCVal ("read.stepcaf.codepage", "UTF8");

  return Standard_True;
}
