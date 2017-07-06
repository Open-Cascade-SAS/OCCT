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

#ifndef _StepBasic_DocumentUsageConstraint_HeaderFile
#define _StepBasic_DocumentUsageConstraint_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
class StepBasic_Document;
class TCollection_HAsciiString;


class StepBasic_DocumentUsageConstraint;
DEFINE_STANDARD_HANDLE(StepBasic_DocumentUsageConstraint, Standard_Transient)


class StepBasic_DocumentUsageConstraint : public Standard_Transient
{

public:

  
  Standard_EXPORT StepBasic_DocumentUsageConstraint();
  
  Standard_EXPORT void Init (const Handle(StepBasic_Document)& aSource, const Handle(TCollection_HAsciiString)& ase, const Handle(TCollection_HAsciiString)& asev);
  
  Standard_EXPORT Handle(StepBasic_Document) Source() const;
  
  Standard_EXPORT void SetSource (const Handle(StepBasic_Document)& aSource);
  
  Standard_EXPORT Handle(TCollection_HAsciiString) SubjectElement() const;
  
  Standard_EXPORT void SetSubjectElement (const Handle(TCollection_HAsciiString)& ase);
  
  Standard_EXPORT Handle(TCollection_HAsciiString) SubjectElementValue() const;
  
  Standard_EXPORT void SetSubjectElementValue (const Handle(TCollection_HAsciiString)& asev);




  DEFINE_STANDARD_RTTIEXT(StepBasic_DocumentUsageConstraint,Standard_Transient)

protected:




private:


  Handle(StepBasic_Document) theSource;
  Handle(TCollection_HAsciiString) theSE;
  Handle(TCollection_HAsciiString) theSEV;


};







#endif // _StepBasic_DocumentUsageConstraint_HeaderFile
