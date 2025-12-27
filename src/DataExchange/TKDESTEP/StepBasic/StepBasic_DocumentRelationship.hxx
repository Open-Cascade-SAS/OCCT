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

#ifndef _StepBasic_DocumentRelationship_HeaderFile
#define _StepBasic_DocumentRelationship_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
class TCollection_HAsciiString;
class StepBasic_Document;

class StepBasic_DocumentRelationship : public Standard_Transient
{

public:
  Standard_EXPORT StepBasic_DocumentRelationship();

  Standard_EXPORT void Init(const occ::handle<TCollection_HAsciiString>& aName,
                            const occ::handle<TCollection_HAsciiString>& aDescription,
                            const occ::handle<StepBasic_Document>&       aRelating,
                            const occ::handle<StepBasic_Document>&       aRelated);

  Standard_EXPORT occ::handle<TCollection_HAsciiString> Name() const;

  Standard_EXPORT void SetName(const occ::handle<TCollection_HAsciiString>& aName);

  Standard_EXPORT occ::handle<TCollection_HAsciiString> Description() const;

  Standard_EXPORT void SetDescription(const occ::handle<TCollection_HAsciiString>& aDescription);

  Standard_EXPORT occ::handle<StepBasic_Document> RelatingDocument() const;

  Standard_EXPORT void SetRelatingDocument(const occ::handle<StepBasic_Document>& aRelating);

  Standard_EXPORT occ::handle<StepBasic_Document> RelatedDocument() const;

  Standard_EXPORT void SetRelatedDocument(const occ::handle<StepBasic_Document>& aRelated);

  DEFINE_STANDARD_RTTIEXT(StepBasic_DocumentRelationship, Standard_Transient)

private:
  occ::handle<TCollection_HAsciiString> theName;
  occ::handle<TCollection_HAsciiString> theDescription;
  occ::handle<StepBasic_Document>       theRelating;
  occ::handle<StepBasic_Document>       theRelated;
};

#endif // _StepBasic_DocumentRelationship_HeaderFile
