// Created on: 2000-04-09
// Created by: Sergey MOZOKHIN
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

#ifndef STEPCAFControl_DataMapOfLabelExternFile_HeaderFile
#define STEPCAFControl_DataMapOfLabelExternFile_HeaderFile

#include <TDF_Label.hxx>
#include <STEPCAFControl_ExternFile.hxx>
#include <NCollection_DataMap.hxx>

typedef NCollection_DataMap<TDF_Label, Handle(STEPCAFControl_ExternFile)>
  STEPCAFControl_DataMapOfLabelExternFile;
typedef NCollection_DataMap<TDF_Label, Handle(STEPCAFControl_ExternFile)>::Iterator
  STEPCAFControl_DataMapIteratorOfDataMapOfLabelExternFile;

#endif
