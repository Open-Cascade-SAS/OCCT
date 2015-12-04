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


#include <Standard_Type.hxx>
#include <StepGeom_TrimmingMember.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepGeom_TrimmingMember,StepData_SelectReal)

StepGeom_TrimmingMember::StepGeom_TrimmingMember  ()    {  }

    Standard_Boolean  StepGeom_TrimmingMember::HasName () const
      {  return Standard_True;  }

    Standard_CString  StepGeom_TrimmingMember::Name () const
      {  return "PARAMETER_VALUE";  }

    Standard_Boolean  StepGeom_TrimmingMember::SetName (const Standard_CString /*name*/)
      {  return Standard_True;  }
