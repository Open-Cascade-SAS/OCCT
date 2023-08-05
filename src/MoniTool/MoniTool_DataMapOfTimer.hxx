// Created on: 1998-04-01
// Created by: Christian CAILLET
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef MoniTool_DataMapOfTimer_HeaderFile
#define MoniTool_DataMapOfTimer_HeaderFile

#include <Standard_CString.hxx>
#include <Standard_CStringHasher.hxx>
#include <NCollection_DataMap.hxx>

class MoniTool_Timer;
typedef NCollection_DataMap<Standard_CString,Handle(MoniTool_Timer), Standard_CStringHasher> MoniTool_DataMapOfTimer;
typedef NCollection_DataMap<Standard_CString,Handle(MoniTool_Timer), Standard_CStringHasher>::Iterator MoniTool_DataMapIteratorOfDataMapOfTimer;


#endif
