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

#include <IFSelect_CheckCounter.ixx>
#include <Standard_Transient.hxx>
#include <Interface_Check.hxx>
#include <stdio.h>


//=======================================================================
//function : IFSelect_CheckCounter
//purpose  : 
//=======================================================================

IFSelect_CheckCounter::IFSelect_CheckCounter(const Standard_Boolean withlist)
     : IFSelect_SignatureList (withlist)
{
  SetName("Check");
}


//=======================================================================
//function : SetSignature
//purpose  : 
//=======================================================================

void IFSelect_CheckCounter::SetSignature(const Handle(MoniTool_SignText)& sign)
{
  thesign = sign;
}


//=======================================================================
//function : Signature
//purpose  : 
//=======================================================================

Handle(MoniTool_SignText) IFSelect_CheckCounter::Signature () const
{
 return thesign;
}


//=======================================================================
//function : Analyse
//purpose  : 
//=======================================================================

void  IFSelect_CheckCounter::Analyse(const Interface_CheckIterator& list,
                                     const Handle(Interface_InterfaceModel)& model,
                                     const Standard_Boolean original,
                                     const Standard_Boolean failsonly)
{
  Standard_Integer i,nb,num, nbe = (model.IsNull() ? 0 : model->NbEntities());
  char mess[300];
  sprintf (mess,"Check %s",list.Name());
  SetName (mess);
  for (list.Start(); list.More(); list.Next()) {
    num = list.Number();
    Handle(Standard_Transient) ent;
    const Handle(Interface_Check) check = list.Value();
    ent = check->Entity();
    if (ent.IsNull() && num > 0 && num <= nbe) ent = model->Value(num);
    nb = check->NbFails();
    Standard_CString tystr = NULL;
    if (!ent.IsNull()) {
      if (!thesign.IsNull()) tystr = thesign->Text (ent,model).ToCString();
      else if (!model.IsNull()) tystr = model->TypeName (ent);
      else tystr =
	Interface_InterfaceModel::ClassName(ent->DynamicType()->Name());
    }
    for (i = 1; i <= nb; i ++) {
      if (ent.IsNull())  sprintf(mess,"F: %s",check->CFail(i,original));
      else sprintf(mess,"F:%s: %s",tystr,check->CFail(i,original));
      Add (ent,mess);
    }
    nb = 0;
    if (!failsonly) nb = check->NbWarnings();
    for (i = 1; i <= nb; i ++) {
      if (ent.IsNull())  sprintf(mess,"W: %s",check->CWarning(i,original));
      else sprintf(mess,"W:%s: %s",tystr,check->CWarning(i,original));
      Add (ent,mess);
    }
  }
}
