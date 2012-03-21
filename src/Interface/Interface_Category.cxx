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

#include <Interface_Category.ixx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_SequenceOfAsciiString.hxx>
#include <Interface_GeneralModule.hxx>

static int init = 0;
static Standard_CString unspec = "unspecified";

static TColStd_SequenceOfAsciiString& thecats()
{
  static TColStd_SequenceOfAsciiString thecat;
  return thecat;
}




    Interface_Category::Interface_Category ()
    : thegtool (new Interface_GTool)    {  Init();  }

    Interface_Category::Interface_Category
  (const Handle(Interface_Protocol)& protocol)
    : thegtool (new Interface_GTool(protocol))    {  Init();  }

    Interface_Category::Interface_Category
  (const Handle(Interface_GTool)& gtool)
    : thegtool (gtool)    {  Init();  }

    void  Interface_Category::SetProtocol
  (const Handle(Interface_Protocol)& protocol)
      {  thegtool->SetProtocol(protocol);  }

    Standard_Integer Interface_Category::CatNum
  (const Handle(Standard_Transient)& ent, const Interface_ShareTool& shares)
{
  if (ent.IsNull()) return 0;
  Standard_Integer CN;
  Handle(Interface_GeneralModule) module;
  if (!thegtool->Select (ent,module,CN)) return 0;
  return module->CategoryNumber (CN,ent,shares);
}

    void  Interface_Category::ClearNums ()
      {  thenum.Nullify();  }

    void  Interface_Category::Compute
  (const Handle(Interface_InterfaceModel)& model,
   const Interface_ShareTool& shares)
{
  ClearNums();
  if (model.IsNull()) return;
  Standard_Integer CN, i, nb = model->NbEntities();
  thegtool->Reservate (nb);
  if (nb == 0) return;
  thenum = new TColStd_HArray1OfInteger (1,nb);  thenum->Init(0);
  for (i = 1; i <= nb; i ++) {
    Handle(Standard_Transient) ent = model->Value(i);
    if (ent.IsNull()) continue;
    Handle(Interface_GeneralModule) module;
    if (!thegtool->Select (ent,module,CN)) continue;
    thenum->SetValue (i,module->CategoryNumber (CN,ent,shares));
  }
}

Standard_Integer  Interface_Category::Num (const Standard_Integer nument) const
{
  if (thenum.IsNull()) return 0;
  if (nument < 1 || nument > thenum->Length()) return 0;
  return thenum->Value(nument);
}


//  ##########    LISTE DES CATEGORIES    ##########

    Standard_Integer  Interface_Category::AddCategory (const Standard_CString name)
{
  Standard_Integer num = Interface_Category::Number (name);
  if (num > 0) return num;
  thecats().Append (TCollection_AsciiString(name));
  return thecats().Length()+1;
}

    Standard_Integer  Interface_Category::NbCategories ()
      {  return thecats().Length();  }

    Standard_CString  Interface_Category::Name   (const Standard_Integer num)
{
  if (num < 0) return "";
  if (num < 1 || num > thecats().Length()) return unspec;
  return thecats().Value(num).ToCString();
}


    Standard_Integer  Interface_Category::Number (const Standard_CString name)
{
  Standard_Integer i, nb = thecats().Length();
  for (i = 1; i <= nb; i ++) {
    if (thecats().Value(i).IsEqual(name)) return i;
  }
  return 0;
}


    void Interface_Category::Init ()
{
  if (init) return;  init = 1;
  init = Interface_Category::AddCategory ("Shape");
  init = Interface_Category::AddCategory ("Drawing");
  init = Interface_Category::AddCategory ("Structure");
  init = Interface_Category::AddCategory ("Description");
  init = Interface_Category::AddCategory ("Auxiliary");
  init = Interface_Category::AddCategory ("Professional");
  init = Interface_Category::AddCategory ("FEA");
  init = Interface_Category::AddCategory ("Kinematics");
  init = Interface_Category::AddCategory ("Piping");
}
