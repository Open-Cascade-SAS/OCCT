// Created on: 2004-03-05
// Created by: Mikhail KUZMITCHEV
// Copyright (c) 2004-2014 OPEN CASCADE SAS
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

#ifndef QANCollection_DoubleMapOfRealInteger_HeaderFile
#define QANCollection_DoubleMapOfRealInteger_HeaderFile

#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_MapRealHasher.hxx>
#include <TColStd_MapIntegerHasher.hxx>
#include <NCollection_DoubleMap.hxx>

typedef NCollection_DoubleMap<Standard_Real,Standard_Integer,TColStd_MapRealHasher,TColStd_MapIntegerHasher> QANCollection_DoubleMapOfRealInteger;
typedef NCollection_DoubleMap<Standard_Real,Standard_Integer,TColStd_MapRealHasher,TColStd_MapIntegerHasher>::Iterator QANCollection_DoubleMapIteratorOfDoubleMapOfRealInteger;


#endif
