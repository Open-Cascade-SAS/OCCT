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

#include <IFSelect_ParamEditor.ixx>
#include <Interface_Static.hxx>


IFSelect_ParamEditor::IFSelect_ParamEditor
  (const Standard_Integer nbmax, const Standard_CString label)
: IFSelect_Editor (nbmax) , thelabel (label)
{
  SetNbValues (0);
  if (thelabel.Length() == 0) thelabel.AssignCat ("Param Editor");
}

void  IFSelect_ParamEditor::AddValue
  (const Handle(Interface_TypedValue)& val, const Standard_CString shortname)
{
  SetNbValues (NbValues() + 1);
  SetValue (NbValues(), val, shortname);
}

void  IFSelect_ParamEditor::AddConstantText
  (const Standard_CString val, const Standard_CString shortname,
   const Standard_CString longname)
{
  Handle(Interface_TypedValue) tv = new Interface_TypedValue
    (longname[0] == '\0' ? shortname : longname);
  tv->SetCStringValue (val);
  SetNbValues (NbValues() + 1);
  SetValue (NbValues(), tv, shortname, IFSelect_EditRead);
}


TCollection_AsciiString  IFSelect_ParamEditor::Label () const
      {  return thelabel;  }

Standard_Boolean  IFSelect_ParamEditor::Recognize
  (const Handle(IFSelect_EditForm)& form) const
      {  return Standard_True;  }    // pas de contrainte

Handle(TCollection_HAsciiString)  IFSelect_ParamEditor::StringValue
  (const Handle(IFSelect_EditForm)& form,const Standard_Integer num) const
      {  return TypedValue(num)->HStringValue();  }


Standard_Boolean  IFSelect_ParamEditor::Load
  (const Handle(IFSelect_EditForm)& form,
   const Handle(Standard_Transient)& ent,
   const Handle(Interface_InterfaceModel)& model) const
{
  Standard_Integer i, nb = NbValues();
  for (i = 1; i <= nb; i ++) form->LoadValue (i,TypedValue(i)->HStringValue());

  return Standard_True;
}


Standard_Boolean  IFSelect_ParamEditor::Apply
  (const Handle(IFSelect_EditForm)& form,
   const Handle(Standard_Transient)& ent,
   const Handle(Interface_InterfaceModel)& model) const
{
  Standard_Integer i, nb = NbValues();
  for (i = 1; i <= nb; i ++)
    if (form->IsModified(i))
      TypedValue (i)->SetHStringValue (form->EditedValue(i));

  return Standard_True;
}

Handle(IFSelect_ParamEditor)  IFSelect_ParamEditor::StaticEditor
  (const Handle(TColStd_HSequenceOfHAsciiString)& list,
   const Standard_CString label)
{
  Handle(IFSelect_ParamEditor) editor;
  if (list.IsNull()) return editor;
  Standard_Integer i,nb = list->Length();
//  if (nb == 0) return editor;
  editor = new IFSelect_ParamEditor (nb+10,label);
  for (i = 1; i <= nb; i ++) {
    Handle(Interface_Static) val = Interface_Static::Static
      (list->Value(i)->ToCString());
    if (!val.IsNull()) editor->AddValue(val);
  }
  return editor;
}
