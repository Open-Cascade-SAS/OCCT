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

#ifndef _TopOpeBRepDS_FIR_HeaderFile
#define _TopOpeBRepDS_FIR_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopOpeBRepDS_DataMapOfShapeListOfShapeOn1State.hxx>
#include <Standard_Integer.hxx>
class TopOpeBRepDS_HDataStructure;

//! FaceInterferenceReducer
class TopOpeBRepDS_FIR
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT TopOpeBRepDS_FIR(const Handle(TopOpeBRepDS_HDataStructure)& HDS);

  Standard_EXPORT void ProcessFaceInterferences(
    const TopOpeBRepDS_DataMapOfShapeListOfShapeOn1State& M);

  Standard_EXPORT void ProcessFaceInterferences(
    const Standard_Integer                                I,
    const TopOpeBRepDS_DataMapOfShapeListOfShapeOn1State& M);

protected:
private:
  Handle(TopOpeBRepDS_HDataStructure) myHDS;
};

#endif // _TopOpeBRepDS_FIR_HeaderFile
