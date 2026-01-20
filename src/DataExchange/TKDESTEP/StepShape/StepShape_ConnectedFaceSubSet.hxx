// Created on: 2002-01-04
// Created by: data exchange team
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

#ifndef _StepShape_ConnectedFaceSubSet_HeaderFile
#define _StepShape_ConnectedFaceSubSet_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepShape_ConnectedFaceSet.hxx>
#include <StepShape_Face.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
class TCollection_HAsciiString;

//! Representation of STEP entity ConnectedFaceSubSet
class StepShape_ConnectedFaceSubSet : public StepShape_ConnectedFaceSet
{

public:
  //! Empty constructor
  Standard_EXPORT StepShape_ConnectedFaceSubSet();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(const occ::handle<TCollection_HAsciiString>&   aRepresentationItem_Name,
                            const occ::handle<NCollection_HArray1<occ::handle<StepShape_Face>>>&    aConnectedFaceSet_CfsFaces,
                            const occ::handle<StepShape_ConnectedFaceSet>& aParentFaceSet);

  //! Returns field ParentFaceSet
  Standard_EXPORT occ::handle<StepShape_ConnectedFaceSet> ParentFaceSet() const;

  //! Set field ParentFaceSet
  Standard_EXPORT void SetParentFaceSet(const occ::handle<StepShape_ConnectedFaceSet>& ParentFaceSet);

  DEFINE_STANDARD_RTTIEXT(StepShape_ConnectedFaceSubSet, StepShape_ConnectedFaceSet)

private:
  occ::handle<StepShape_ConnectedFaceSet> theParentFaceSet;
};

#endif // _StepShape_ConnectedFaceSubSet_HeaderFile
