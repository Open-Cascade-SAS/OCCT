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

#include <IFSelect_SignCategory.ixx>
#include <Interface_Category.hxx>


static Standard_CString nulsign = "";


    IFSelect_SignCategory::IFSelect_SignCategory ()
    : IFSelect_Signature ("Category")
{
  Interface_Category::Init();  // si pas deja fait
  Standard_Integer i, nb = Interface_Category::NbCategories();
  for (i = 1; i <= nb; i ++)  AddCase (Interface_Category::Name(i));
}


    Standard_CString IFSelect_SignCategory::Value
  (const Handle(Standard_Transient)& ent,
   const Handle(Interface_InterfaceModel)& model) const
{
  if (ent.IsNull() || model.IsNull()) return nulsign;
  return Interface_Category::Name(model->CategoryNumber (model->Number(ent)));
}
