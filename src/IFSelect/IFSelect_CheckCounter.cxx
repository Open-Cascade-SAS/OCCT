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
