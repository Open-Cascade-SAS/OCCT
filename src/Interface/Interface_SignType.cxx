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

#include <Interface_SignType.ixx>

TCollection_AsciiString  Interface_SignType::Text
  (const Handle(Standard_Transient)& ent,
   const Handle(Standard_Transient)& context) const
{
  TCollection_AsciiString atext;
  Handle(Interface_InterfaceModel) model =
    Handle(Interface_InterfaceModel)::DownCast(context);
  if (ent.IsNull() || model.IsNull()) return atext;
  atext.AssignCat (Value(ent,model));
  return atext;
}

    Standard_CString  Interface_SignType::ClassName
  (const Standard_CString typnam)
{
  char* tn =(char*) typnam;
  for (int i = 0; tn[i] != '\0'; i ++) {
    if (tn[i] == '_') return &tn[i+1];
  }
  return tn;
}
