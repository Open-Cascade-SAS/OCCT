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

#include <Interface_GTool.ixx>

Interface_GTool::Interface_GTool  ()    {  }

    Interface_GTool::Interface_GTool
  (const Handle(Interface_Protocol)& proto, const Standard_Integer nb)
    : theproto (proto) , thelib (proto)
      {  if (nb > 0)  {  thentnum.ReSize(nb);  thentmod.ReSize(nb);  }  }


    void  Interface_GTool::SetSignType (const Handle(Interface_SignType)& sign)
      {  thesign = sign;  }

    Handle(Interface_SignType)  Interface_GTool::SignType () const
      {  return thesign;  }

    Standard_CString  Interface_GTool::SignValue
  (const Handle(Standard_Transient)& ent,
   const Handle(Interface_InterfaceModel)& model) const
{
  if (ent.IsNull()) return "";
  if (thesign.IsNull()) return Interface_SignType::ClassName(ent->DynamicType()->Name());
  return thesign->Value (ent,model);
}

    Standard_CString  Interface_GTool::SignName () const
{
  if (thesign.IsNull()) return "Class Name";
  return thesign->Name();
}


    void  Interface_GTool::SetProtocol
  (const Handle(Interface_Protocol)& proto, const Standard_Boolean enforce)
{
  if (proto == theproto && !enforce) return;
  theproto = proto;
  thelib.Clear();
  thelib.AddProtocol (proto);
}

    Handle(Interface_Protocol)  Interface_GTool::Protocol () const
      {  return theproto;  }

    Interface_GeneralLib&  Interface_GTool::Lib ()
      {  return thelib;  }

    void  Interface_GTool::Reservate
  (const Standard_Integer nb, const Standard_Boolean enforce)
{
  Standard_Integer n = thentnum.NbBuckets();
  if (n < nb && !enforce) return;
  thentnum.ReSize (nb);  thentmod.ReSize (nb);
}

    void  Interface_GTool::ClearEntities ()
      {  thentnum.Clear();  thentmod.Clear();  }


//=======================================================================
//function : Select
//purpose  : 
//=======================================================================

Standard_Boolean  Interface_GTool::Select (const Handle(Standard_Transient)& ent,
                                           Handle(Interface_GeneralModule)& gmod,
                                           Standard_Integer& CN,
                                           const Standard_Boolean enforce)
{
  const Handle(Standard_Type)& aType = ent->DynamicType();
  Standard_Integer num = thentmod.FindIndex(aType);// (ent);
  if (num == 0 || enforce) {
    if (thelib.Select (ent,gmod,CN)) {
      num = thentmod.Add (aType,gmod);
      thentnum.Bind (aType,CN);
      return Standard_True;
    }
    return Standard_False;
  }
  gmod = Handle(Interface_GeneralModule)::DownCast (thentmod.FindFromKey(aType));
  CN   = thentnum.Find (aType);
  return Standard_True;
}
