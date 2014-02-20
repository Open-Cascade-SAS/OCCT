// Created on: 2002-05-15
// Created by: Alexander KARTOMIN (akm)
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

// Purpose:     To test the performance of NCollection classes

#ifndef QANCollection_PerfTest_HeaderFile
#define QANCollection_PerfTest_HeaderFile

#include <gp_Pnt.hxx>
#include <QANCollection_Common3.hxx>

// ===================== Test methods for each type ===========================
// TCollection classes compared are the containers with
//     ValueType  -  gp_Pnt
//     Key1Type   -  Standard_Real
//     Key2Type   -  Standard_Integer

#include <QANCollection_PerfArrays.hxx>
#include <QANCollection_PerfLists.hxx>
#include <QANCollection_PerfMaps.hxx>

#endif
