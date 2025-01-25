// Created on: 1993-10-29
// Created by: Christophe MARION
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

#ifndef _HLRAlgo_PolyShellData_HeaderFile
#define _HLRAlgo_PolyShellData_HeaderFile

#include <Standard_Type.hxx>
#include <Standard_Transient.hxx>
#include <HLRAlgo_ListOfBPoint.hxx>
#include <HLRAlgo_PolyData.hxx>

class HLRAlgo_PolyShellData;
DEFINE_STANDARD_HANDLE(HLRAlgo_PolyShellData, Standard_Transient)

//! All the PolyData of a Shell
class HLRAlgo_PolyShellData : public Standard_Transient
{

public:
  struct ShellIndices
  {
    Standard_Integer Min, Max;
  };

  Standard_EXPORT HLRAlgo_PolyShellData(const Standard_Integer nbFace);

  Standard_EXPORT void UpdateGlobalMinMax(HLRAlgo_PolyData::Box& theBox);

  Standard_EXPORT void UpdateHiding(const Standard_Integer nbHiding);

  Standard_Boolean Hiding() const { return !myHPolHi.IsEmpty(); }

  NCollection_Array1<Handle(HLRAlgo_PolyData)>& PolyData() { return myPolyg; }

  NCollection_Array1<Handle(HLRAlgo_PolyData)>& HidingPolyData() { return myHPolHi; }

  HLRAlgo_ListOfBPoint& Edges() { return mySegList; }

  ShellIndices& Indices() { return myIndices; }

  DEFINE_STANDARD_RTTIEXT(HLRAlgo_PolyShellData, Standard_Transient)

private:
  ShellIndices                                 myIndices;
  NCollection_Array1<Handle(HLRAlgo_PolyData)> myPolyg;
  NCollection_Array1<Handle(HLRAlgo_PolyData)> myHPolHi;
  HLRAlgo_ListOfBPoint                         mySegList;
};

#endif // _HLRAlgo_PolyShellData_HeaderFile
