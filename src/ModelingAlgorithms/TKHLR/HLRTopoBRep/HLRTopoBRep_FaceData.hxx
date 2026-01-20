// Created on: 1995-03-27
// Created by: Christophe MARION
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _HLRTopoBRep_FaceData_HeaderFile
#define _HLRTopoBRep_FaceData_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>

//! Contains the 3 ListOfShape of a Face
//! (Internal OutLines, OutLines on restriction and IsoLines).
class HLRTopoBRep_FaceData
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT HLRTopoBRep_FaceData();

  const NCollection_List<TopoDS_Shape>& FaceIntL() const;

  const NCollection_List<TopoDS_Shape>& FaceOutL() const;

  const NCollection_List<TopoDS_Shape>& FaceIsoL() const;

  NCollection_List<TopoDS_Shape>& AddIntL();

  NCollection_List<TopoDS_Shape>& AddOutL();

  NCollection_List<TopoDS_Shape>& AddIsoL();

private:
  NCollection_List<TopoDS_Shape> myIntL;
  NCollection_List<TopoDS_Shape> myOutL;
  NCollection_List<TopoDS_Shape> myIsoL;
};

#include <HLRTopoBRep_FaceData.lxx>

#endif // _HLRTopoBRep_FaceData_HeaderFile
