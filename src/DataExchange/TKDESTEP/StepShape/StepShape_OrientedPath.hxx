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

#ifndef _StepShape_OrientedPath_HeaderFile
#define _StepShape_OrientedPath_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepShape_Path.hxx>
#include <StepShape_OrientedEdge.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Standard_Integer.hxx>
class StepShape_EdgeLoop;
class TCollection_HAsciiString;
class StepShape_OrientedEdge;

class StepShape_OrientedPath : public StepShape_Path
{

public:
  //! Returns a OrientedPath
  Standard_EXPORT StepShape_OrientedPath();

  Standard_EXPORT void Init(const occ::handle<TCollection_HAsciiString>& aName,
                            const occ::handle<StepShape_EdgeLoop>&       aPathElement,
                            const bool                                   aOrientation);

  Standard_EXPORT void SetPathElement(const occ::handle<StepShape_EdgeLoop>& aPathElement);

  Standard_EXPORT occ::handle<StepShape_EdgeLoop> PathElement() const;

  Standard_EXPORT void SetOrientation(const bool aOrientation);

  Standard_EXPORT bool Orientation() const;

  Standard_EXPORT void SetEdgeList(
    const occ::handle<NCollection_HArray1<occ::handle<StepShape_OrientedEdge>>>& aEdgeList)
    override;

  Standard_EXPORT occ::handle<NCollection_HArray1<occ::handle<StepShape_OrientedEdge>>>
    EdgeList() const override;

  Standard_EXPORT occ::handle<StepShape_OrientedEdge> EdgeListValue(
    const int num) const override;

  Standard_EXPORT int NbEdgeList() const override;

  DEFINE_STANDARD_RTTIEXT(StepShape_OrientedPath, StepShape_Path)

private:
  occ::handle<StepShape_EdgeLoop> pathElement;
  bool                            orientation;
};

#endif // _StepShape_OrientedPath_HeaderFile
