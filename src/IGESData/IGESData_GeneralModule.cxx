#include <IGESData_GeneralModule.ixx>
#include <Interface_GeneralLib.hxx>
#include <IGESData.hxx>
#include <IGESData_Protocol.hxx>
#include <IGESData_IGESEntity.hxx>
#include <TCollection_HAsciiString.hxx>
#include <Interface_Macros.hxx>




    void  IGESData_GeneralModule::FillSharedCase
  (const Standard_Integer CN, const Handle(Standard_Transient)& ent,
   Interface_EntityIterator& iter) const 
{
  DeclareAndCast(IGESData_IGESEntity,anent,ent);
  if (anent.IsNull()) return;
//  .... Directory Part
  iter.AddItem (anent->DirFieldEntity (3));
  iter.AddItem (anent->DirFieldEntity (4));
  iter.AddItem (anent->DirFieldEntity (5));
  iter.AddItem (anent->DirFieldEntity (6));
  iter.AddItem (anent->DirFieldEntity (7));
  iter.AddItem (anent->DirFieldEntity (8));
  iter.AddItem (anent->DirFieldEntity (13));

//  .... Own Parameters
  OwnSharedCase (CN,anent,iter);

//  .... Properties
  Interface_EntityIterator assocs = anent->Properties();
  for (; assocs.More(); assocs.Next())  iter.AddItem (assocs.Value());
}

    void  IGESData_GeneralModule::ListImpliedCase
  (const Standard_Integer CN, const Handle(Standard_Transient)& ent,
   Interface_EntityIterator& iter) const
{
  DeclareAndCast(IGESData_IGESEntity,anent,ent);
  if (anent.IsNull()) return;
  OwnImpliedCase (CN,anent,iter);
  Interface_EntityIterator assocs = anent->Associativities();
  for (; assocs.More(); assocs.Next())  iter.AddItem (assocs.Value());
}


    void  IGESData_GeneralModule::OwnImpliedCase
  (const Standard_Integer , const Handle(IGESData_IGESEntity)& ,
   Interface_EntityIterator& ) const 
      {  }  // par defaut, rien  (redefinissable)



    void  IGESData_GeneralModule::CheckCase
  (const Standard_Integer CN, const Handle(Standard_Transient)& ent,
   const Interface_ShareTool& shares, Handle(Interface_Check)& ach) const
{
  DeclareAndCast(IGESData_IGESEntity,anent,ent);

  IGESData_DirChecker dc = DirChecker(CN,anent);
  dc.Check(ach,anent);
  OwnCheckCase (CN,anent,shares,ach);
}


    Standard_Boolean  IGESData_GeneralModule::CanCopy
  (const Standard_Integer CN, const Handle(Standard_Transient)& ent) const
      {  return Standard_True;  }

    void  IGESData_GeneralModule::CopyCase
  (const Standard_Integer CN, const Handle(Standard_Transient)& entfrom,
   const Handle(Standard_Transient)& entto, Interface_CopyTool& TC) const 
{
  DeclareAndCast(IGESData_IGESEntity,ento,entto);
  DeclareAndCast(IGESData_IGESEntity,enfr,entfrom);
////  ento->Clear();

// ...                Reprendre entete                ...
//?  ento->InitTypeAndForm (enfr->TypeNumber(), enfr->FormNumber());ShallowCopy
  
  if (enfr->DefLineFont() == IGESData_DefReference) ento->InitLineFont
    (GetCasted(IGESData_LineFontEntity,    TC.Transferred(enfr->LineFont())));
  else ento->InitLineFont (enfr->LineFont() , enfr->RankLineFont());

  if (enfr->DefLevel() == IGESData_DefSeveral) ento->InitLevel
    (GetCasted(IGESData_LevelListEntity,TC.Transferred(enfr->LevelList())),-1);
  else ento->InitLevel (enfr->LevelList() , enfr->Level());

  if (enfr->DefView() != IGESData_DefNone) ento->InitView
    (GetCasted(IGESData_ViewKindEntity,    TC.Transferred(enfr->View())));

  if (enfr->HasTransf()) ento->InitTransf
    (GetCasted(IGESData_TransfEntity,      TC.Transferred(enfr->Transf())));

  ento->InitStatus (enfr->BlankStatus() ,
		    enfr->SubordinateStatus() ,
		    enfr->UseFlag() ,
		    enfr->HierarchyStatus() );

  if (enfr->DefColor() == IGESData_DefReference) ento->InitColor
    (GetCasted(IGESData_ColorEntity,TC.Transferred(enfr->Color())));
  else ento->InitColor (enfr->Color() , enfr->RankColor());

  if (enfr->HasShortLabel()) ento->SetLabel
    (new TCollection_HAsciiString(enfr->ShortLabel()),
     enfr->SubScriptNumber());
  else ento->SetLabel (enfr->ShortLabel() , enfr->SubScriptNumber());

//  Directory Part : Miscellaneous
  Handle(IGESData_IGESEntity) Structure;
  if (enfr->HasStructure()) Structure =
    GetCasted(IGESData_IGESEntity,         TC.Transferred(enfr->Structure()));
  if (enfr->HasLabelDisplay()) ento->InitMisc
    (Structure,
     GetCasted(IGESData_LabelDisplayEntity,TC.Transferred(enfr->LabelDisplay())),
     enfr->LineWeightNumber());
  else ento->InitMisc
    (Structure , enfr->LabelDisplay() , enfr->LineWeightNumber());

//  LineWeightValue, Res1, Res2 : through action of ShallowCopy ?

  OwnCopyCase (CN,enfr,ento,TC);

//  ..  Properties
  if (enfr->NbProperties() != 0) {
    for (Interface_EntityIterator iter = enfr->Properties();
	 iter.More(); iter.Next()) {
      ento->AddProperty
	(GetCasted(IGESData_IGESEntity,TC.Transferred(iter.Value())));
    }
  }
}

    void  IGESData_GeneralModule::RenewImpliedCase
  (const Standard_Integer CN, const Handle(Standard_Transient)& entfrom,
   const Handle(Standard_Transient)& entto, const Interface_CopyTool& TC) const
{
  DeclareAndCast(IGESData_IGESEntity,ento,entto);
  DeclareAndCast(IGESData_IGESEntity,enfr,entfrom);

  OwnRenewCase (CN,enfr,ento,TC);

//  .. Associativities
  if (enfr->NbAssociativities() != 0) {
    for (Interface_EntityIterator iter = enfr->Associativities();
	 iter.More(); iter.Next()) {
      Handle(Standard_Transient) anent = iter.Value();
      Handle(Standard_Transient) newent;
      if (TC.Search(anent,newent)) ento->AddAssociativity
	(GetCasted(IGESData_IGESEntity,newent));
    }
  }
}

    void  IGESData_GeneralModule::OwnRenewCase
  (const Standard_Integer , const Handle(IGESData_IGESEntity)& ,
   const Handle(IGESData_IGESEntity)& , const Interface_CopyTool& ) const
     {  }

    void  IGESData_GeneralModule::WhenDeleteCase
  (const Standard_Integer CN, const Handle(Standard_Transient)& ent,
   const Standard_Boolean ) const    // dispatched : ignored
{
  DeclareAndCast(IGESData_IGESEntity,anent,ent);
  if (anent.IsNull()) return;
  anent->Clear();
  OwnDeleteCase (CN,anent);
}

    void  IGESData_GeneralModule::OwnDeleteCase
  (const Standard_Integer , const Handle(IGESData_IGESEntity)& ) const
      {  }  // par defaut, rien  (redefinissable)


    Handle(TCollection_HAsciiString)  IGESData_GeneralModule::Name
  (const Standard_Integer , const Handle(Standard_Transient)& ent,
   const Interface_ShareTool& ) const
{
  Handle(TCollection_HAsciiString) name;
  DeclareAndCast(IGESData_IGESEntity,anent,ent);
  if (anent.IsNull()) return name;
  name = anent->NameValue ();
  return name;
}
