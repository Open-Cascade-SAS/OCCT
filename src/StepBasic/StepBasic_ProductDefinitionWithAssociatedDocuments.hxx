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

#include <StepBasic_HArray1OfDocument.hxx>
#include <StepBasic_ProductDefinition.hxx>
#include <Standard_Integer.hxx>
class TCollection_HAsciiString;
class StepBasic_ProductDefinitionFormation;
class StepBasic_ProductDefinitionContext;
class StepBasic_Document;


class StepBasic_ProductDefinitionWithAssociatedDocuments;
DEFINE_STANDARD_HANDLE(StepBasic_ProductDefinitionWithAssociatedDocuments, StepBasic_ProductDefinition)


class StepBasic_ProductDefinitionWithAssociatedDocuments : public StepBasic_ProductDefinition
{

public:

  
  Standard_EXPORT StepBasic_ProductDefinitionWithAssociatedDocuments();
  
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aId, const Handle(TCollection_HAsciiString)& aDescription, const Handle(StepBasic_ProductDefinitionFormation)& aFormation, const Handle(StepBasic_ProductDefinitionContext)& aFrame, const Handle(StepBasic_HArray1OfDocument)& aDocIds);
  
  Standard_EXPORT Handle(StepBasic_HArray1OfDocument) DocIds() const;
  
  Standard_EXPORT void SetDocIds (const Handle(StepBasic_HArray1OfDocument)& DocIds);
  
  Standard_EXPORT Standard_Integer NbDocIds() const;
  
  Standard_EXPORT Handle(StepBasic_Document) DocIdsValue (const Standard_Integer num) const;
  
  Standard_EXPORT void SetDocIdsValue (const Standard_Integer num, const Handle(StepBasic_Document)& adoc);




  DEFINE_STANDARD_RTTIEXT(StepBasic_ProductDefinitionWithAssociatedDocuments,StepBasic_ProductDefinition)

protected:




private:


  Handle(StepBasic_HArray1OfDocument) theDocIds;


};







#endif // _StepBasic_ProductDefinitionWithAssociatedDocuments_HeaderFile
