// Created on: 1997-04-02
// Created by: Administrateur Atelier XSTEP
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _StepData_EDescr_HeaderFile
#define _StepData_EDescr_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
class StepData_Described;

class StepData_EDescr;
DEFINE_STANDARD_HANDLE(StepData_EDescr, Standard_Transient)

//! This class is intended to describe the authorized form for an
//! entity, either Simple or Plex
class StepData_EDescr : public Standard_Transient
{

public:
  //! Tells if a ESDescr matches a step type : exact or super type
  Standard_EXPORT virtual Standard_Boolean Matches(const Standard_CString steptype) const = 0;

  //! Tells if a EDescr is complex (ECDescr) or simple (ESDescr)
  Standard_EXPORT virtual Standard_Boolean IsComplex() const = 0;

  //! Creates a described entity (i.e. a simple one)
  Standard_EXPORT virtual Handle(StepData_Described) NewEntity() const = 0;

  DEFINE_STANDARD_RTTIEXT(StepData_EDescr, Standard_Transient)

protected:
private:
};

#endif // _StepData_EDescr_HeaderFile
