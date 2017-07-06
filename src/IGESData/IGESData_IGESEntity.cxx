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


#include <gp_GTrsf.hxx>
#include <IGESData_ColorEntity.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_GeneralModule.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESReaderTool.hxx>
#include <IGESData_IGESType.hxx>
#include <IGESData_LabelDisplayEntity.hxx>
#include <IGESData_LevelListEntity.hxx>
#include <IGESData_LineFontEntity.hxx>
#include <IGESData_NameEntity.hxx>
#include <IGESData_ReadWriteModule.hxx>
#include <IGESData_SingleParentEntity.hxx>
#include <IGESData_TransfEntity.hxx>
#include <IGESData_ViewKindEntity.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_EntityList.hxx>
#include <Interface_InterfaceError.hxx>
#include <Interface_Macros.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_PCharacter.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>

#include <stdio.h>
IMPLEMENT_STANDARD_RTTIEXT(IGESData_IGESEntity,Standard_Transient)

#define ThisEntity  Handle(IGESData_IGESEntity)::DownCast(This())

namespace
{
  static const Standard_Integer IGESFlagAssocs  = 131072;
  static const Standard_Integer IGESFlagProps   = 262144;
  static const Standard_Integer IGESFourStatus  = 65535;
  static const Standard_Integer IGESStatusField = 15;
  static const Standard_Integer IGESShiftSubord = 4;
  static const Standard_Integer IGESShiftUse    = 8;
  static const Standard_Integer IGESShiftHier   = 12;
}

//=======================================================================
//function : IGESData_IGESEntity
//purpose  : Default constructor.
//=======================================================================
IGESData_IGESEntity::IGESData_IGESEntity()
: theType       (0),
  theForm       (0),
  theDefLevel   (0),
  theStatusNum  (0),
  theLWeightNum (0),
  theLWeightVal (0.0),
  theSubScriptN (0)
{
  theRes1[0] = theRes2[0] = '\0';
}

//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================
void IGESData_IGESEntity::Clear ()
{
//  Handle et DefSwitch
  theStructure.Nullify();
  theDefLineFont.SetVoid();  theLineFont.Nullify();
  theDefColor.SetVoid();     theColor.Nullify();
  theDefLevel   = 0;         theLevelList.Nullify();
  theView.Nullify();         theTransf.Nullify();      theLabDisplay.Nullify();
  theSubScriptN = 0;         theShortLabel.Nullify();

//  Autres Valeurs, Listes
  theType       = theForm       = 0;
  theStatusNum  = theLWeightNum = 0;
  theLWeightVal = 0.;
//  theRes1[0]    = theRes2[0]    = '\0';
  theProps.Clear(); theAssocs.Clear();
}


//  #########################################################################
//  ....                Definition IGES : Directory Entry                ....

    IGESData_IGESType IGESData_IGESEntity::IGESType () const
      {  return IGESData_IGESType(theType,theForm);  }

    Standard_Integer IGESData_IGESEntity::TypeNumber () const
      {  return theType;  }

    Standard_Integer IGESData_IGESEntity::FormNumber () const
      {  return theForm;  }


    Handle(IGESData_IGESEntity)  IGESData_IGESEntity::DirFieldEntity
  (const Standard_Integer num) const
{
  Handle(IGESData_IGESEntity) ent;
  if (num ==  3) ent = theStructure;
  if (num ==  4) ent = theLineFont;
  if (num ==  5) ent = theLevelList;
  if (num ==  6) ent = theView;
  if (num ==  7) ent = theTransf;
  if (num ==  8) ent = theLabDisplay;
  if (num == 13) ent = theColor;
  return ent;
}

    Standard_Boolean IGESData_IGESEntity::HasStructure () const
      {  return (!theStructure.IsNull());  }

    Handle(IGESData_IGESEntity) IGESData_IGESEntity::Structure () const
      {  return theStructure;  }

    IGESData_DefType IGESData_IGESEntity::DefLineFont () const
      {  return theDefLineFont.DefType();  }

    Standard_Integer IGESData_IGESEntity::RankLineFont () const
      {  return theDefLineFont.Value();  }

    Handle(IGESData_LineFontEntity) IGESData_IGESEntity::LineFont () const
      {  return GetCasted(IGESData_LineFontEntity,theLineFont);  }

    IGESData_DefList IGESData_IGESEntity::DefLevel () const
{
  if (theDefLevel >  0) return IGESData_DefOne;
  if (theDefLevel <  0) return IGESData_DefSeveral;
  return IGESData_DefNone;
}

    Standard_Integer IGESData_IGESEntity::Level () const
      {  return theDefLevel;  }

    Handle(IGESData_LevelListEntity) IGESData_IGESEntity::LevelList () const
      {  return GetCasted(IGESData_LevelListEntity,theLevelList);  }


    IGESData_DefList IGESData_IGESEntity::DefView () const
{
  if (View().IsNull())    return IGESData_DefNone;
  if (View()->IsSingle()) return IGESData_DefOne;
  else                    return IGESData_DefSeveral;
}

    Handle(IGESData_ViewKindEntity) IGESData_IGESEntity::View () const
      {  return GetCasted(IGESData_ViewKindEntity,theView);  }

    Handle(IGESData_ViewKindEntity) IGESData_IGESEntity::SingleView () const
{
  Handle(IGESData_ViewKindEntity) nulvue;
  if (DefView() != IGESData_DefOne) return nulvue;
  return View();
}

    Handle(IGESData_ViewKindEntity) IGESData_IGESEntity::ViewList () const
{
  Handle(IGESData_ViewKindEntity) nulvue;
  if (DefView() != IGESData_DefSeveral) return nulvue;
  return View();
}


    Standard_Boolean IGESData_IGESEntity::HasTransf () const
      {  return (!theTransf.IsNull());  }

    Handle(IGESData_TransfEntity) IGESData_IGESEntity::Transf () const
      {  return GetCasted(IGESData_TransfEntity,theTransf);  }


    Standard_Boolean IGESData_IGESEntity::HasLabelDisplay () const
      {  return (!theLabDisplay.IsNull());  }

    Handle(IGESData_LabelDisplayEntity) IGESData_IGESEntity::LabelDisplay
      () const
      {  return GetCasted(IGESData_LabelDisplayEntity,theLabDisplay);  }

// Status : un Integer pour BlankStatus,SubrodinateStatus,USeFlag,HierarchySt.
// Decoupage : 4 bits chacun (BlankStatus tout a droite, etc)

    Standard_Integer IGESData_IGESEntity::BlankStatus () const
      {  return (theStatusNum & IGESStatusField);  }

    Standard_Integer IGESData_IGESEntity::SubordinateStatus () const
      {  return ((theStatusNum >> IGESShiftSubord) & IGESStatusField);  }

    Standard_Integer IGESData_IGESEntity::UseFlag () const
      {  return ((theStatusNum >> IGESShiftUse) & IGESStatusField);  }

    Standard_Integer IGESData_IGESEntity::HierarchyStatus () const
      {  return ((theStatusNum >> IGESShiftHier) & IGESStatusField);  }

    Standard_Integer IGESData_IGESEntity::LineWeightNumber () const
      {  return theLWeightNum;  }

    Standard_Real IGESData_IGESEntity::LineWeight () const
      {  return theLWeightVal;  }


    IGESData_DefType IGESData_IGESEntity::DefColor () const
      {  return theDefColor.DefType();  }

    Standard_Integer IGESData_IGESEntity::RankColor () const
      {  return theDefColor.Value();  }

    Handle(IGESData_ColorEntity) IGESData_IGESEntity::Color () const
      {  return GetCasted(IGESData_ColorEntity,theColor);  }


//=======================================================================
//function : CResValues
//purpose  : 
//=======================================================================
Standard_Boolean IGESData_IGESEntity::CResValues (const Standard_CString res1, 
						  const Standard_CString res2) const
{
  Standard_Boolean res = Standard_False;
  Standard_PCharacter pres1, pres2;
  //
  pres1=(Standard_PCharacter)res1;
  pres2=(Standard_PCharacter)res2;
  //
  for (Standard_Integer i = 0; i < 8; i ++) {
    pres1[i] = theRes1[i]; 
    pres2[i] = theRes2[i];
    if (theRes1[i] > ' ' || theRes2[i] > ' ') {
      res = Standard_True;
    }
  }
  pres1[8] = '\0'; 
  pres2[8] = '\0';
  //
  return res;
}

    Standard_Boolean IGESData_IGESEntity::HasShortLabel () const
      {  return (!theShortLabel.IsNull());  }

    Handle(TCollection_HAsciiString) IGESData_IGESEntity::ShortLabel () const
      {  return theShortLabel;  }

    Standard_Boolean IGESData_IGESEntity::HasSubScriptNumber () const
      {  return (theSubScriptN >= 0);  }  // =0 nul mais defini, <0 absent

    Standard_Integer IGESData_IGESEntity::SubScriptNumber () const
{
  if (theSubScriptN < 0) return 0;
  return theSubScriptN;
}

//  ....                (Re)Initialisation du Directory                 ....

    void  IGESData_IGESEntity::InitTypeAndForm
  (const Standard_Integer typenum, const Standard_Integer formnum)
      {  theType = typenum;  theForm = formnum;  }

    void  IGESData_IGESEntity::InitDirFieldEntity
  (const Standard_Integer num, const Handle(IGESData_IGESEntity)& ent)
{
  if (num ==  3) theStructure  = ent;
  if (num ==  4) theLineFont   = ent;
  if (num ==  5) theLevelList  = ent;
  if (num ==  6) theView       = ent;
  if (num ==  7) theTransf     = ent;
  if (num ==  8) theLabDisplay = ent;
  if (num == 13) theColor      = ent;
}

    void  IGESData_IGESEntity::InitTransf
  (const Handle(IGESData_TransfEntity)& ent)
{  theTransf = ent;  }

    void  IGESData_IGESEntity::InitView
  (const Handle(IGESData_ViewKindEntity)& ent)
{  theView = ent;  }

    void  IGESData_IGESEntity::InitLineFont
  (const Handle(IGESData_LineFontEntity)& ent, const Standard_Integer rank)
{  theDefLineFont.SetRank((ent.IsNull() ? rank : -1));  theLineFont = ent;  }

    void  IGESData_IGESEntity::InitLevel
  (const Handle(IGESData_LevelListEntity)& ent, const Standard_Integer val)
{  theLevelList = ent;  theDefLevel = (ent.IsNull() ? val : -1);  }

    void  IGESData_IGESEntity::InitColor
  (const Handle(IGESData_ColorEntity)& ent, const Standard_Integer rank)
{  theDefColor.SetRank((ent.IsNull() ? rank : -1));  theColor = ent;  }

    void  IGESData_IGESEntity::InitStatus
  (const Standard_Integer blank,   const Standard_Integer subordinate,
   const Standard_Integer useflag, const Standard_Integer hierarchy)
{
  theStatusNum  = (theStatusNum & (!IGESFourStatus));
  theStatusNum += (blank       & IGESStatusField) |
                ((subordinate  & IGESStatusField) << IGESShiftSubord) |
                ((useflag      & IGESStatusField) << IGESShiftUse) |
                ((hierarchy    & IGESStatusField) << IGESShiftHier);
}

    void  IGESData_IGESEntity::SetLabel
  (const Handle(TCollection_HAsciiString)& label, const Standard_Integer sub)
{  theShortLabel = label;  theSubScriptN = sub;  }

    void  IGESData_IGESEntity::InitMisc
  (const Handle(IGESData_IGESEntity)& str,
   const Handle(IGESData_LabelDisplayEntity)& lab,
   const Standard_Integer weightnum)
{
  theStructure = str;  theLabDisplay = lab;
  if (theLWeightNum != 0)  theLWeightVal *= (weightnum/theLWeightNum);
  else if (weightnum == 0) theLWeightVal  = 0;
  theLWeightNum = weightnum;
}

//  ....                  Notions derivees importantes                  ....


//  SingleParent : ici on ne traite que l Associativity SingleParent
//  Pour considerer le partage implicite, il faut remonter au Modele ...

    Standard_Boolean IGESData_IGESEntity::HasOneParent () const
{
  return (NbTypedProperties(STANDARD_TYPE(IGESData_SingleParentEntity)) == 1);
}

    Handle(IGESData_IGESEntity) IGESData_IGESEntity::UniqueParent () const
{
  if (NbTypedProperties(STANDARD_TYPE(IGESData_SingleParentEntity)) != 1)
    throw Interface_InterfaceError("IGESEntity : UniqueParent");
  else {
    DeclareAndCast(IGESData_SingleParentEntity,PP,
		   TypedProperty(STANDARD_TYPE(IGESData_SingleParentEntity)));
    return PP->SingleParent();
  }
}


    gp_GTrsf IGESData_IGESEntity::Location () const
{
  //szv#4:S4163:12Mar99 unreachcble eliminated
  //if (!HasTransf()) return gp_GTrsf();    // Identite
  //else return Transf()->Value();          // c-a-d Compoound
  if (!HasTransf()) return gp_GTrsf();    // Identite
  Handle(IGESData_TransfEntity) trsf = Transf(); 
  return (trsf.IsNull())? gp_GTrsf() : trsf->Value(); 
}

    gp_GTrsf IGESData_IGESEntity::VectorLocation () const
{
  if (!HasTransf()) return gp_GTrsf();    // Identite
//    Prendre Location et anuler TranslationPart
  gp_GTrsf loca = Transf()->Value();      // c-a-d Compoound
  loca.SetTranslationPart (gp_XYZ(0.,0.,0.));
  return loca;
}

    gp_GTrsf IGESData_IGESEntity::CompoundLocation () const
{
  gp_GTrsf loca = Location();
  if (!HasOneParent()) return loca;
  gp_GTrsf locp = UniqueParent()->CompoundLocation();
  loca.PreMultiply(locp);
  return loca;
}


    Standard_Boolean IGESData_IGESEntity::HasName () const
{
  if (HasShortLabel()) return Standard_True;
  return (NbTypedProperties(STANDARD_TYPE(IGESData_NameEntity)) == 1);
}

Handle(TCollection_HAsciiString) IGESData_IGESEntity::NameValue () const
{
  Handle(TCollection_HAsciiString) nom;  // au depart vide
  //   Question : concatene-t-on le SubScript ?  Oui, forme label(subscript)
  Standard_Integer nbname = NbTypedProperties(STANDARD_TYPE(IGESData_NameEntity));
  if (nbname == 0) {
    if (!HasShortLabel()) return nom;
    if (theSubScriptN < 0) return theShortLabel;
    char lenom[50];
    sprintf (lenom,"%s(%d)",theShortLabel->ToCString(),theSubScriptN);
    nom = new TCollection_HAsciiString (lenom);
  }
  else if (nbname > 0) {
    DeclareAndCast(IGESData_NameEntity,name,
		   TypedProperty(STANDARD_TYPE(IGESData_NameEntity), 1));
    nom = name->Value();
  }

  return nom;
}


//  ....            Listes d'infos Optionnelles (Assocs,Props)            ....

    Standard_Boolean IGESData_IGESEntity::ArePresentAssociativities () const
{
  if (!theAssocs.IsEmpty()) return Standard_True;
  return (theStatusNum & IGESFlagAssocs) != 0;
}

    Standard_Integer IGESData_IGESEntity::NbAssociativities () const
{
  if (theAssocs.IsEmpty()) return 0;
  return theAssocs.NbEntities();
}

    Interface_EntityIterator IGESData_IGESEntity::Associativities () const
{
  Interface_EntityIterator iter;
  theAssocs.FillIterator(iter);
  return iter;
}

    Standard_Integer IGESData_IGESEntity::NbTypedAssociativities
  (const Handle(Standard_Type)& atype) const
      {  return theAssocs.NbTypedEntities(atype);  }

    Handle(IGESData_IGESEntity) IGESData_IGESEntity::TypedAssociativity
  (const Handle(Standard_Type)& atype) const
{  return GetCasted(IGESData_IGESEntity,theAssocs.TypedEntity(atype));  }

    void IGESData_IGESEntity::AddAssociativity
  (const Handle(IGESData_IGESEntity)& ent)
      {  theAssocs.Append(ent);  }

    void IGESData_IGESEntity::RemoveAssociativity
  (const Handle(IGESData_IGESEntity)& ent)
      {  theAssocs.Remove(ent);  }

    void IGESData_IGESEntity::LoadAssociativities
  (const Interface_EntityList& list)
      {  theAssocs = list;  theStatusNum = (theStatusNum | IGESFlagAssocs);  }

    void IGESData_IGESEntity::ClearAssociativities ()
      {  theAssocs.Clear();  }

    void IGESData_IGESEntity::Associate
  (const Handle(IGESData_IGESEntity)& ent) const
      {  if (!ent.IsNull()) ent->AddAssociativity(ThisEntity);  }

    void IGESData_IGESEntity::Dissociate
  (const Handle(IGESData_IGESEntity)& ent) const
      {  if (!ent.IsNull()) ent->RemoveAssociativity(ThisEntity);  }


    Standard_Boolean IGESData_IGESEntity::ArePresentProperties () const
{
  if (!theProps.IsEmpty()) return Standard_True;
  return (theStatusNum & IGESFlagProps) != 0;
}

    Standard_Integer IGESData_IGESEntity::NbProperties () const
{
  if (theProps.IsEmpty()) return 0;
  return theProps.NbEntities();
}

    Interface_EntityIterator IGESData_IGESEntity::Properties () const
{
  Interface_EntityIterator iter;
  theProps.FillIterator(iter);
  return iter;
}

    Standard_Integer IGESData_IGESEntity::NbTypedProperties
  (const Handle(Standard_Type)& atype) const
      {  return theProps.NbTypedEntities(atype);  }

    Handle(IGESData_IGESEntity) IGESData_IGESEntity::TypedProperty
  (const Handle(Standard_Type)& atype, const Standard_Integer anum) const
{  return GetCasted(IGESData_IGESEntity,theProps.TypedEntity(atype, anum));  }

    void IGESData_IGESEntity::AddProperty
  (const Handle(IGESData_IGESEntity)& ent)
      {  theProps.Append(ent);  }

    void IGESData_IGESEntity::RemoveProperty
  (const Handle(IGESData_IGESEntity)& ent)
      {  theProps.Remove(ent);  }

    void IGESData_IGESEntity::LoadProperties
  (const Interface_EntityList& list)
      {  theProps = list;    theStatusNum = (theStatusNum | IGESFlagProps);  }

    void IGESData_IGESEntity::ClearProperties ()
      {  theProps.Clear();  }

// ....                     Actions liees au Transfert                     ....

    void IGESData_IGESEntity::SetLineWeight
      (const Standard_Real defw, const Standard_Real maxw,
       const Standard_Integer gradw)
{
  if (theLWeightNum == 0) theLWeightVal = defw;
  else if (gradw == 1) theLWeightVal = maxw*theLWeightNum;
  else theLWeightVal = (maxw*theLWeightNum)/gradw;
}
