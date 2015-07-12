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


#include <Interface_Category.hxx>
#include <Interface_GeneralModule.hxx>
#include <Interface_GTool.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Protocol.hxx>
#include <Interface_ShareTool.hxx>
#include <Standard_Transient.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_SequenceOfAsciiString.hxx>

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
