// File:        QANCollection_PerfTest.hxx
// Created:     Wed May 15 12:38:26 2002
// Author:      Alexander KARTOMIN (akm)
//              <a-kartomin@opencascade.com>
//
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
