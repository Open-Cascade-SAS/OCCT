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


#include <MoniTool_OptValue.hxx>
#include <MoniTool_Profile.hxx>
#include <Standard_Transient.hxx>

MoniTool_OptValue::MoniTool_OptValue (const Standard_CString opt)
    : theopt (opt)
{
// CKY:   NO CALL TO A VIRTUAL METHOD WITHIN THE CONSTRUCTOR
//   Explicite Load must be done after creation
}

    void  MoniTool_OptValue::Clear ()
      {  theval.Nullify();  }

    void  MoniTool_OptValue::SetValue
  (const Handle(MoniTool_Profile)& prof,
   const Standard_CString opt, const Standard_Boolean fast)
{
  if (prof.IsNull()) return;
  if (!opt || opt[0] == '\0') return;
  Handle(Standard_Transient) val;
  if (fast) prof->FastValue (opt,val);
  else prof->Value (opt,val);
  if (!val.IsNull()) theval = val;
}


    Standard_Boolean  MoniTool_OptValue::IsLoaded () const
      {  return (!theval.IsNull());  }

    Handle(MoniTool_Profile)  MoniTool_OptValue::Prof () const
{
  Handle(MoniTool_Profile) prof;
  return prof;
}

    void  MoniTool_OptValue::Load (const Standard_Boolean fast)
      {  SetValue (Prof(),theopt.ToCString(),fast);  }


    void  MoniTool_OptValue::Value (Handle(Standard_Transient)& val) const
      {  val = theval;  }

//=======================================================================
//function : Delete
//purpose  : 
//=======================================================================

void MoniTool_OptValue::Delete() 
{} 
