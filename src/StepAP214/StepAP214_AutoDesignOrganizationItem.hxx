// Created on: 1998-08-04
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

#ifndef _StepAP214_AutoDesignOrganizationItem_HeaderFile
#define _StepAP214_AutoDesignOrganizationItem_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepAP214_AutoDesignGeneralOrgItem.hxx>
#include <Standard_Integer.hxx>
class Standard_Transient;
class StepBasic_Document;
class StepBasic_PhysicallyModeledProductDefinition;



class StepAP214_AutoDesignOrganizationItem  : public StepAP214_AutoDesignGeneralOrgItem
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT StepAP214_AutoDesignOrganizationItem();
  
  Standard_EXPORT virtual Standard_Integer CaseNum (const Handle(Standard_Transient)& ent) const Standard_OVERRIDE;
  
  Standard_EXPORT Handle(StepBasic_Document) Document() const;
  
  Standard_EXPORT Handle(StepBasic_PhysicallyModeledProductDefinition) PhysicallyModeledProductDefinition() const;




protected:





private:





};







#endif // _StepAP214_AutoDesignOrganizationItem_HeaderFile
