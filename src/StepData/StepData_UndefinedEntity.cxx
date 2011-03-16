#include <StepData_UndefinedEntity.ixx>
#include <Interface_ParamType.hxx>
#include <TCollection_AsciiString.hxx>
#include <Interface_Macros.hxx>



    StepData_UndefinedEntity::StepData_UndefinedEntity ()
      {  thecont = new Interface_UndefinedContent;  thesub = Standard_False;  }

    StepData_UndefinedEntity::StepData_UndefinedEntity
  (const Standard_Boolean issub)
      { thesub = issub;  thecont = new Interface_UndefinedContent;  }

    Handle(Interface_UndefinedContent)
      StepData_UndefinedEntity::UndefinedContent () const
      {  return thecont;  }

    Standard_Boolean StepData_UndefinedEntity::IsSub () const
      {  return thesub;  }

    Standard_Boolean StepData_UndefinedEntity::IsComplex () const
      {  return (!thenext.IsNull());  }

    Handle(StepData_UndefinedEntity) StepData_UndefinedEntity::Next () const
      {  return thenext;  }

    Standard_CString StepData_UndefinedEntity::StepType () const
      {  if (thetype.IsNull()) return "";  return thetype->ToCString();  }


void StepData_UndefinedEntity::ReadRecord(const Handle(StepData_StepReaderData)& SR,
                                          const Standard_Integer num,
                                          Handle(Interface_Check)& ach)
{
  thetype = new TCollection_HAsciiString(SR->RecordType(num));
  Standard_Integer nb = SR->NbParams(num);

  thecont->Reservate (nb,4);
  for (Standard_Integer i = 1; i <= nb; i ++) {
    Handle(Standard_Transient) anent;
    Handle(TCollection_HAsciiString) hval;
    Standard_CString val = SR->ParamCValue(num,i);
    Interface_ParamType partyp = SR->ParamType(num,i);
    Standard_Integer nume = 0;
    if (partyp == Interface_ParamIdent) {
      nume = SR->ParamNumber(num,i);
      if (nume <= 0) {
	ach->AddFail("A reference to another entity is unresolved");
	partyp = Interface_ParamVoid;
      }
      else {
	anent = SR->BoundEntity(nume);
      }
    }
    else if (partyp == Interface_ParamSub) {
      nume = SR->ParamNumber(num,i);
      Handle(StepData_UndefinedEntity) und = new StepData_UndefinedEntity (Standard_True);
      anent = und;
      und->ReadRecord(SR,nume,ach);
    }
    else if (partyp == Interface_ParamText) {
      //    Return integre a supprimer silya
      Standard_Integer lval = strlen(val);  Standard_Integer mval = -1;
      for (Standard_Integer j = 0; j < lval; j ++) {
	if (val[j] == '\n') { mval = i; break; }
      }
      if (mval > 0) {
	nume = -1;
	hval = new TCollection_HAsciiString (val);
	hval->RemoveAll('\n');
      }
    }
    if (nume == 0) hval = new TCollection_HAsciiString (val);
    if (nume >  0) thecont->AddEntity(partyp,anent);
    else           thecont->AddLiteral (partyp,hval);
  }
  Standard_Integer nextyp = SR->NextForComplex(num);
  if (nextyp == 0) return;
  thenext = new StepData_UndefinedEntity;
  thenext->ReadRecord(SR,nextyp,ach);
}


void StepData_UndefinedEntity::WriteParams (StepData_StepWriter& SW) const
{
  if (!IsSub()) SW.StartEntity(TCollection_AsciiString(StepType()));
  Standard_Integer nb = thecont->NbParams();
  Handle(Standard_Transient) anent;
  for (Standard_Integer i = 1; i <= nb; i ++) {
    Interface_ParamType partyp = thecont->ParamType(i);
    if (partyp == Interface_ParamSub) {
      DeclareAndCast(StepData_UndefinedEntity,und,thecont->ParamEntity(i));
      und->StepType(); //svv #2
      if (und->IsSub()) SW.OpenTypedSub (und->StepType());
      und->WriteParams(SW);
      if (und->IsSub()) SW.CloseSub();
    }
    else if (partyp == Interface_ParamIdent) {
      anent = thecont->ParamEntity(i);
      SW.Send(anent);
    }
    else SW.SendString (thecont->ParamValue(i)->ToCString());
  }
//  if (IsSub()) return;
//  SW.NewLine(Standard_True);
  if (thenext.IsNull()) return;
  thenext->WriteParams(SW);
}

    void  StepData_UndefinedEntity::GetFromAnother
  (const Handle(StepData_UndefinedEntity)& another,
   Interface_CopyTool& TC)
{
//  DeclareAndCast(StepData_UndefinedEntity,another,other);
  thetype = new TCollection_HAsciiString (another->StepType());
  thecont = new Interface_UndefinedContent;
  thecont->GetFromAnother(another->UndefinedContent(),TC);

  thesub = another->IsSub();
  if (another->IsComplex()) thenext =
    GetCasted(StepData_UndefinedEntity,TC.Transferred(another->Next()));
  else thenext.Nullify();
}


    void  StepData_UndefinedEntity::FillShared
  (Interface_EntityIterator& list) const
{
  Standard_Integer i, nb = thecont->NbParams();
  for (i = 1; i <= nb; i ++) {
    Interface_ParamType ptype = thecont->ParamType(i);
    if (ptype == Interface_ParamSub) {
      DeclareAndCast(StepData_UndefinedEntity,subent,thecont->ParamEntity(i));
      subent->FillShared (list);
    } else if (ptype == Interface_ParamIdent) {
      list.AddItem(thecont->ParamEntity(i));
    }
  }
  if (!thenext.IsNull()) thenext->FillShared (list);
}
