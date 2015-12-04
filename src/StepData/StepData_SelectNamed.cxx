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
#include <StepData_Field.hxx>
#include <StepData_SelectNamed.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepData_SelectNamed,StepData_SelectMember)

//  Definitions reprises de Field :
#define KindInteger 1
#define KindBoolean 2
#define KindLogical 3
#define KindEnum    4
#define KindReal    5
#define KindString  6

StepData_SelectNamed::StepData_SelectNamed  ()
    {  theval.Clear();  thename.Clear();  }


    Standard_Boolean  StepData_SelectNamed::HasName () const
      {  return (thename.Length() > 0);  }

    Standard_CString  StepData_SelectNamed::Name () const
      {  return thename.ToCString();  }

    Standard_Boolean  StepData_SelectNamed::SetName (const Standard_CString name)
      {  thename.Clear();  thename.AssignCat (name);  return Standard_True;  }

    const StepData_Field&  StepData_SelectNamed::Field () const
      {  return theval;  }

    StepData_Field&  StepData_SelectNamed::CField ()
      {  return theval;  }


    Standard_Integer  StepData_SelectNamed::Kind () const
      {  return theval.Kind();  }

    void  StepData_SelectNamed::SetKind  (const Standard_Integer kind)
      {  theval.Clear (kind);  }

    Standard_Integer  StepData_SelectNamed::Int  () const
      {  return theval.Int();  }

    void  StepData_SelectNamed::SetInt (const Standard_Integer val)
      {  theval.SetInt (val);  }

    Standard_Real  StepData_SelectNamed::Real () const
      {  return theval.Real();  }

    void  StepData_SelectNamed::SetReal (const Standard_Real val)
      {  theval.SetReal (val);  }

    Standard_CString  StepData_SelectNamed::String  () const
      {  return theval.String();  }

    void  StepData_SelectNamed::SetString (const Standard_CString val)
      {  theval.SetString (val);  }
