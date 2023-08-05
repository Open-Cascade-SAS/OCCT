// Created on: 1999-06-10
// Created by: Vladislav ROMASHKO
// Copyright (c) 1999 Matra Datavision
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

#ifndef TFunction_DataMapOfLabelListOfLabel_HeaderFile
#define TFunction_DataMapOfLabelListOfLabel_HeaderFile

#include <TDF_LabelList.hxx>
#include <NCollection_DataMap.hxx>

typedef NCollection_DataMap<TDF_Label,TDF_LabelList> TFunction_DataMapOfLabelListOfLabel;
typedef NCollection_DataMap<TDF_Label,TDF_LabelList>::Iterator TFunction_DataMapIteratorOfDataMapOfLabelListOfLabel;


#endif
