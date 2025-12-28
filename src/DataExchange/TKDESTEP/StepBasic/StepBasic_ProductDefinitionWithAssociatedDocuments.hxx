// Created on: 1998-06-30
// Created by: Christian CAILLET
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

#ifndef _StepBasic_ProductDefinitionWithAssociatedDocuments_HeaderFile
#define _StepBasic_ProductDefinitionWithAssociatedDocuments_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepBasic_Document.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepBasic_ProductDefinition.hxx>
#include <Standard_Integer.hxx>
class TCollection_HAsciiString;
class StepBasic_ProductDefinitionFormation;
class StepBasic_ProductDefinitionContext;
class StepBasic_Document;

class StepBasic_ProductDefinitionWithAssociatedDocuments : public StepBasic_ProductDefinition
{

public:
  Standard_EXPORT StepBasic_ProductDefinitionWithAssociatedDocuments();

  Standard_EXPORT void Init(
    const occ::handle<TCollection_HAsciiString>&                             aId,
    const occ::handle<TCollection_HAsciiString>&                             aDescription,
    const occ::handle<StepBasic_ProductDefinitionFormation>&                 aFormation,
    const occ::handle<StepBasic_ProductDefinitionContext>&                   aFrame,
    const occ::handle<NCollection_HArray1<occ::handle<StepBasic_Document>>>& aDocIds);

  Standard_EXPORT occ::handle<NCollection_HArray1<occ::handle<StepBasic_Document>>> DocIds() const;

  Standard_EXPORT void SetDocIds(
    const occ::handle<NCollection_HArray1<occ::handle<StepBasic_Document>>>& DocIds);

  Standard_EXPORT int NbDocIds() const;

  Standard_EXPORT occ::handle<StepBasic_Document> DocIdsValue(const int num) const;

  Standard_EXPORT void SetDocIdsValue(const int num, const occ::handle<StepBasic_Document>& adoc);

  DEFINE_STANDARD_RTTIEXT(StepBasic_ProductDefinitionWithAssociatedDocuments,
                          StepBasic_ProductDefinition)

private:
  occ::handle<NCollection_HArray1<occ::handle<StepBasic_Document>>> theDocIds;
};

#endif // _StepBasic_ProductDefinitionWithAssociatedDocuments_HeaderFile
