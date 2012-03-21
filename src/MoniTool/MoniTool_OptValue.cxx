// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <MoniTool_OptValue.ixx>

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
