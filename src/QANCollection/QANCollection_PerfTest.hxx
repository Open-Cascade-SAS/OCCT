// Created on: 2002-05-15
// Created by: Alexander KARTOMIN (akm)
// Copyright (c) 2002-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

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
