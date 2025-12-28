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

#ifndef _StepRepr_RepresentationMap_HeaderFile
#define _StepRepr_RepresentationMap_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
class StepRepr_RepresentationItem;
class StepRepr_Representation;

class StepRepr_RepresentationMap : public Standard_Transient
{

public:
  //! Returns a RepresentationMap
  Standard_EXPORT StepRepr_RepresentationMap();

  Standard_EXPORT void Init(const occ::handle<StepRepr_RepresentationItem>& aMappingOrigin,
                            const occ::handle<StepRepr_Representation>&     aMappedRepresentation);

  Standard_EXPORT void SetMappingOrigin(
    const occ::handle<StepRepr_RepresentationItem>& aMappingOrigin);

  Standard_EXPORT occ::handle<StepRepr_RepresentationItem> MappingOrigin() const;

  Standard_EXPORT void SetMappedRepresentation(
    const occ::handle<StepRepr_Representation>& aMappedRepresentation);

  Standard_EXPORT occ::handle<StepRepr_Representation> MappedRepresentation() const;

  DEFINE_STANDARD_RTTIEXT(StepRepr_RepresentationMap, Standard_Transient)

private:
  occ::handle<StepRepr_RepresentationItem> mappingOrigin;
  occ::handle<StepRepr_Representation>     mappedRepresentation;
};

#endif // _StepRepr_RepresentationMap_HeaderFile
