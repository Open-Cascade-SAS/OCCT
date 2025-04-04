// Created on: 1993-06-23
// Created by: Jean Yves LEBEY
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _TopOpeBRepDS_CurveData_HeaderFile
#define _TopOpeBRepDS_CurveData_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopOpeBRepDS_Curve.hxx>
#include <TopOpeBRepDS_GeometryData.hxx>

class TopOpeBRepDS_CurveData : public TopOpeBRepDS_GeometryData
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT TopOpeBRepDS_CurveData();

  Standard_EXPORT TopOpeBRepDS_CurveData(const TopOpeBRepDS_Curve& C);

  friend class TopOpeBRepDS_DataStructure;

protected:
private:
  TopOpeBRepDS_Curve myCurve;
};

#endif // _TopOpeBRepDS_CurveData_HeaderFile
