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


#include <IFSelect_CheckCounter.hxx>
#include <Interface_Check.hxx>
#include <Interface_CheckIterator.hxx>
#include <Interface_InterfaceModel.hxx>
#include <MoniTool_SignText.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>

#include <stdio.h>
IMPLEMENT_STANDARD_RTTIEXT(IFSelect_CheckCounter,IFSelect_SignatureList)

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
