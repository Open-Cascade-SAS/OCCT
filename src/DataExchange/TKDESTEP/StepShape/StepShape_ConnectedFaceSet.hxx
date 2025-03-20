// Created on: 1995-12-01
// Created by: EXPRESS->CDL V0.2 Translator
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

#ifndef _StepShape_ConnectedFaceSet_HeaderFile
#define _StepShape_ConnectedFaceSet_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepShape_HArray1OfFace.hxx>
#include <StepShape_TopologicalRepresentationItem.hxx>
#include <Standard_Integer.hxx>
class TCollection_HAsciiString;
class StepShape_Face;

class StepShape_ConnectedFaceSet;
DEFINE_STANDARD_HANDLE(StepShape_ConnectedFaceSet, StepShape_TopologicalRepresentationItem)

class StepShape_ConnectedFaceSet : public StepShape_TopologicalRepresentationItem
{

public:
  //! Returns a ConnectedFaceSet
  Standard_EXPORT StepShape_ConnectedFaceSet();

  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& aName,
                            const Handle(StepShape_HArray1OfFace)&  aCfsFaces);

  Standard_EXPORT virtual void SetCfsFaces(const Handle(StepShape_HArray1OfFace)& aCfsFaces);

  Standard_EXPORT virtual Handle(StepShape_HArray1OfFace) CfsFaces() const;

  Standard_EXPORT virtual Handle(StepShape_Face) CfsFacesValue(const Standard_Integer num) const;

  Standard_EXPORT virtual Standard_Integer NbCfsFaces() const;

  DEFINE_STANDARD_RTTIEXT(StepShape_ConnectedFaceSet, StepShape_TopologicalRepresentationItem)

protected:
private:
  Handle(StepShape_HArray1OfFace) cfsFaces;
};

#endif // _StepShape_ConnectedFaceSet_HeaderFile
