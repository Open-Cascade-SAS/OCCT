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

//#73 rln 10.03.99 S4135: "read.scale.unit" does not affect GlobalSection

#include <IGESData_BasicEditor.hxx>
#include <IGESData_ColorEntity.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_GeneralModule.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESModel.hxx>
#include <IGESData_LabelDisplayEntity.hxx>
#include <IGESData_LevelListEntity.hxx>
#include <IGESData_LineFontEntity.hxx>
#include <IGESData_Protocol.hxx>
#include <IGESData_SpecificModule.hxx>
#include <IGESData_TransfEntity.hxx>
#include <IGESData_ViewKindEntity.hxx>
#include <IGESGeom_Boundary.hxx>
#include <IGESGeom_CurveOnSurface.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Graph.hxx>
#include <Interface_Macros.hxx>
#include <Interface_Static.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <UnitsMethods.hxx>

IGESData_BasicEditor::IGESData_BasicEditor(const Handle(IGESData_Protocol)&  protocol)
{
  Init(protocol);
}

IGESData_BasicEditor::IGESData_BasicEditor(const Handle(IGESData_IGESModel)& model,
                                           const Handle(IGESData_Protocol)& protocol)
{
  Init(model, protocol);
}

IGESData_BasicEditor::IGESData_BasicEditor() {  }

void IGESData_BasicEditor::Init (const Handle(IGESData_Protocol)& protocol)
{
  theunit = Standard_False;
  theproto = protocol;
  themodel = GetCasted(IGESData_IGESModel,Interface_InterfaceModel::Template("iges"));
  theglib = Interface_GeneralLib (protocol);
  theslib = protocol;
}

void IGESData_BasicEditor::Init (const Handle(IGESData_IGESModel)& model, const Handle(IGESData_Protocol)& protocol)
{
  theunit = Standard_False;
  theproto = protocol;
  themodel = model;
  theglib = Interface_GeneralLib (protocol);
  theslib = protocol;
}

    Handle(IGESData_IGESModel)  IGESData_BasicEditor::Model () const
      {  return themodel;  }

// ####   Travaux sur le Header (GlobalSection)    ####

    Standard_Boolean IGESData_BasicEditor::SetUnitFlag
  (const Standard_Integer flag)
{
  if (themodel.IsNull()) return Standard_False;
  if (flag < 1 || flag > 11) return Standard_False;
  IGESData_GlobalSection GS = themodel->GlobalSection();
  Handle(TCollection_HAsciiString) name = GS.UnitName();
  Standard_CString nam = IGESData_BasicEditor::UnitFlagName (flag);
  if (nam[0] != '\0') name = new TCollection_HAsciiString (nam);
  GS.SetUnitFlag (flag);
  GS.SetUnitName (name);
  themodel->SetGlobalSection (GS);
  theunit = Standard_True;
  return Standard_True;
}

    Standard_Boolean IGESData_BasicEditor::SetUnitValue
  (const Standard_Real val)
{
  if (val <= 0.) return Standard_False;
  Standard_Real vmm = val * UnitsMethods::GetCasCadeLengthUnit(); //abv 20 Feb 00: adding cascade unit factor
  //#73 rln 10.03.99 S4135: "read.scale.unit" does not affect GlobalSection
  //if (Interface_Static::IVal("read.scale.unit") == 1) vmm = vmm * 1000.;
// vmm est exprime en MILLIMETRES
  if (vmm >=      25.       && vmm <=      26.      ) return SetUnitFlag(1);
  if (vmm >=       0.9      && vmm <=       1.1     ) return SetUnitFlag(2);
  if (vmm >=     300.       && vmm <=     310.      ) return SetUnitFlag(4);
  if (vmm >= 1600000.       && vmm <= 1620000.      ) return SetUnitFlag(5);
  if (vmm >=     990.       && vmm <=    1010.      ) return SetUnitFlag(6);
  if (vmm >=  990000.       && vmm <= 1010000.      ) return SetUnitFlag(7);
  if (vmm >=       0.025    && vmm <=       0.026   ) return SetUnitFlag(8);
  if (vmm >=       0.0009   && vmm <=       0.0011  ) return SetUnitFlag(9);
  if (vmm >=       9.       && vmm <=      11.      ) return SetUnitFlag(10);
  if (vmm >=       0.000025 && vmm <=       0.000026) return SetUnitFlag(11);
  return Standard_False;
}

//=======================================================================
//function : SetUnitName
//purpose  : 
//=======================================================================
Standard_Boolean IGESData_BasicEditor::SetUnitName (const Standard_CString name)
{
  if (themodel.IsNull()) return Standard_False;
  Standard_Integer flag = IGESData_BasicEditor::UnitNameFlag (name);
  IGESData_GlobalSection GS = themodel->GlobalSection();
  if (GS.UnitFlag() == 3) {
    char* nam = (char *)name;
    if (name[1] == 'H') nam = (char *)&name[2];
    GS.SetUnitName (new TCollection_HAsciiString(nam));
    themodel->SetGlobalSection (GS);
    return Standard_True;
  }
  if (flag > 0) return SetUnitFlag(flag);
  return (flag > 0);
}

    void  IGESData_BasicEditor::ApplyUnit (const Standard_Boolean enforce)
{
  if (themodel.IsNull()) return;
  if (!enforce && !theunit) return;
  IGESData_GlobalSection GS = themodel->GlobalSection();
  Standard_Real unit = GS.UnitValue();
  if (unit <= 0.) return;
  if (unit != 1.) {
    GS.SetMaxLineWeight (GS.MaxLineWeight () / unit);
    GS.SetResolution    (GS.Resolution    () / unit);
    GS.SetMaxCoord      (GS.MaxCoord      () / unit);
    themodel->SetGlobalSection (GS);
  }
  theunit = Standard_False;
}


// ####   Travaux globaux sur les entites    ####

    void  IGESData_BasicEditor::ComputeStatus ()
{
  if (themodel.IsNull()) return;
  Standard_Integer nb = themodel->NbEntities();
  if (nb == 0) return;
  TColStd_Array1OfInteger subs (0,nb); subs.Init(0); // gere Subordinate Status
  Interface_Graph G (themodel,theglib);    // gere & memorise UseFlag
  G.ResetStatus();

//  2 phases : d abord on fait un calcul d ensemble. Ensuite on applique
//             Tout le modele est traite, pas de jaloux

//  Chaque entite va donner une contribution sur ses descendents propres :
//  pour Subordinate (1 ou 2 cumulables), pour UseFlag (1 a 6 exclusifs)
//    (6 depuis IGES-5.2)

//  Pour Subordinate : Drawing et 402 (sauf p-e dimensioned geometry ?) donnent
//   Logical, le reste implique Physical (sur descendants directs propres)

//  Pour UseFlag, un peu plus complique :
//  D une part, les UseFlag se propagent aux descendants directs ou non
//  D autre part les cas sont plus compliques (et pas aussi clairs)

//  ATTENTION, on ne peut traiter que ce qui se deduit du graphe en s appuyant
//  sur les "IGES Type Number", on n a pas le droit ici d acceder a la
//  description specifique des differents types : traites par AutoCorrect.
//  Exemple : une courbe est 3D ou parametrique 2D(UV), non seulement selon son
//  ascendant, mais selon le role qu elle y joue (ex. pour CurveOnSurface :
//  CurveUV/Curve3D)
//  Traites actuellement (necessaires) :
//  1(Annotation), aussi 4(pour maillage). 5(ParamUV) traite par AutoCorrect

  Standard_Integer CN;
  Standard_Integer i; // svv Jan11 2000 : porting on DEC
  for (i = 1; i <= nb; i ++) {
//  Subordinate (sur directs en propre seulement)
    Handle(IGESData_IGESEntity) ent = themodel->Entity(i);
    Standard_Integer igt = ent->TypeNumber();
    Handle(Interface_GeneralModule) gmodule;
    if (theglib.Select (ent,gmodule,CN)) {
      Handle(IGESData_GeneralModule) gmod =
        Handle(IGESData_GeneralModule)::DownCast (gmodule);
      Interface_EntityIterator sh;
      gmod->OwnSharedCase (CN,ent,sh);
      for (sh.Start(); sh.More(); sh.Next()) {
	Standard_Integer nums = themodel->Number(sh.Value());
	if (igt == 402 || igt == 404) subs.SetValue (nums,subs.Value(nums) | 2);
	else subs.SetValue (nums,subs.Value(nums) | 1);
////	cout<<"ComputeStatus : nums = "<<nums<<" ->"<<subs.Value(nums)<<endl;
      }
    }
//  UseFlag (a propager)
    if (igt / 100 == 2) {
      G.GetFromEntity(ent,Standard_True,1);               // Annotation
      G.GetFromEntity(ent,Standard_False,ent->UseFlag());
    } else if (igt == 134 || igt == 116 || igt == 132) {
      Interface_EntityIterator sh = G.Sharings(ent);      // Maillage ...
      if (sh.NbEntities() > 0) G.GetFromEntity(ent,Standard_True,4);
//  UV : voir AutoCorrect des classes concernees (Boundary et CurveOnSurface)
/*
    } else if (ent->IsKind(STANDARD_TYPE(IGESGeom_CurveOnSurface))) {
      DeclareAndCast(IGESGeom_CurveOnSurface,cos,ent);    // Curve UV
      G.GetFromEntity (cos->CurveUV(),Standard_True,5);
    } else if (ent->IsKind(STANDARD_TYPE(IGESGeom_Boundary))) {
      DeclareAndCast(IGESGeom_Boundary,bnd,ent);          // Curve UV
      Standard_Integer nc = bnd->NbModelSpaceCurves();
      for (Standard_Integer ic = 1; ic <= nc; ic ++) {
	Standard_Integer nuv = bnd->NbParameterCurves(ic);
	for (Standard_Integer juv = 1; juv <= nuv; juv ++)
	  G.GetFromEntity(bnd->ParameterCurve(ic,juv),Standard_True,5);
      }
*/
    }
  }

//  A present, on va appliquer tout cela "de force"
//  Seule exception : des UseFlags non nuls deja en place sont laisses

  for (i = 1; i <= nb; i ++) {
    Handle(IGESData_IGESEntity) ent = themodel->Entity(i);
    Standard_Integer bl = ent->BlankStatus();
    Standard_Integer uf = ent->UseFlag();
    if (uf == 0) uf = G.Status(i);
    Standard_Integer hy = ent->HierarchyStatus();
////    cout<<" Ent.n0."<<i<<" Subord="<<subs.Value(i)<<" Use="<<uf<<endl;
    ent->InitStatus(bl,subs.Value(i),uf,hy);
  }
}


    Standard_Boolean  IGESData_BasicEditor::AutoCorrect
  (const Handle(IGESData_IGESEntity)& ent)
{
  if (themodel.IsNull()) return Standard_False;
  Handle(IGESData_IGESEntity) bof, subent;
  Handle(IGESData_LineFontEntity) linefont;
  Handle(IGESData_LevelListEntity) levelist;
  Handle(IGESData_ViewKindEntity) view;
  Handle(IGESData_TransfEntity) transf;
  Handle(IGESData_LabelDisplayEntity) labdisp;
  Handle(IGESData_ColorEntity) color;

  Standard_Boolean done = Standard_False;
  if (ent.IsNull()) return done;
//    Corrections dans l entete (entites presentes)
//    On ne verifie pas les items "Shared", presents de toute facon
//    Entete : traite par DirChecker pour les cas standard
/*
  linefont = ent->LineFont();
  if (!linefont.IsNull() && themodel->Number(linefont) == 0) {
    linefont.Nullify();
    ent->InitLineFont(linefont,0);
    done = Standard_True;
  }
  levelist = ent->LevelList();
  if (!levelist.IsNull() && themodel->Number(levelist) == 0) {
    levelist.Nullify();
    ent->InitLevel(levelist,0);
    done = Standard_True;
  }
  view = ent->View();
  if (!view.IsNull() && themodel->Number(view) == 0) {
    view.Nullify();
    ent->InitView(view);
    done = Standard_True;
  }
  transf = ent->Transf();
  if (!transf.IsNull() && themodel->Number(transf) == 0) {
    transf.Nullify();
    ent->InitTransf(transf);
    done = Standard_True;
  }
  labdisp = ent->LabelDisplay();
  if (!labdisp.IsNull() && themodel->Number(labdisp) == 0) {
    labdisp.Nullify();
    ent->InitMisc (ent->Structure(),labdisp,ent->LineWeightNumber());
    done = Standard_True;
  }
  color = ent->Color();
  if (!color.IsNull() && themodel->Number(color) == 0) {
    color.Nullify();
    ent->InitColor(color,0);
    done = Standard_True;
  }
*/

//    Corrections dans les Assocs (les Props restent attachees a l Entite)
  Interface_EntityIterator iter = ent->Associativities();
  for (iter.Start(); iter.More(); iter.Next()) {
    subent = GetCasted(IGESData_IGESEntity,iter.Value());
    if (!subent.IsNull() && themodel->Number(subent) == 0)
      {  subent->Dissociate(ent);      done = Standard_True;  }
  }

//    Corrections specifiques
  Standard_Integer CN;
  Handle(Interface_GeneralModule) gmodule;
  if (theglib.Select (ent,gmodule,CN)) {
    Handle(IGESData_GeneralModule) gmod =
      Handle(IGESData_GeneralModule)::DownCast (gmodule);
    IGESData_DirChecker DC = gmod->DirChecker(CN,ent);
    done |= DC.Correct(ent);
  }

  Handle(IGESData_SpecificModule) smod;
  if (theslib.Select (ent,smod,CN)) done |= smod->OwnCorrect (CN,ent);

  return done;
}


    Standard_Integer  IGESData_BasicEditor::AutoCorrectModel ()
{
  Standard_Integer res = 0;
  Standard_Integer nb = themodel->NbEntities();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    if (AutoCorrect (themodel->Entity(i))) res ++;
  }
  return res;
}



//=======================================================================
//function : UnitNameFlag
//purpose  : 
//=======================================================================
Standard_Integer IGESData_BasicEditor::UnitNameFlag  (const Standard_CString name)
{
  char* nam = (char *)&name[0];
  if (name[1] == 'H') {
    nam = (char *)&name[2];
  }
  if (!strcmp (nam,"INCH")) return 1;
  if (!strcmp (nam,"IN"))   return 1;
  if (!strcmp (nam,"MM"))   return 2;
  if (!strcmp (nam,"FT"))   return 4;
  if (!strcmp (nam,"MI"))   return 5;
  if (!strcmp (nam,"M"))    return 6;
  if (!strcmp (nam,"KM"))   return 7;
  if (!strcmp (nam,"MIL"))  return 8;
  if (!strcmp (nam,"UM"))   return 9;
  if (!strcmp (nam,"CM"))   return 10;
  if (!strcmp (nam,"UIN"))  return 11;
  return 0;
}

Standard_Real IGESData_BasicEditor::UnitFlagValue (const Standard_Integer flag)
{
  switch (flag) {
    case  1 : return 0.0254;
    case  2 : return 0.001;
    case  3 : return 1.;
    case  4 : return 0.3048;
    case  5 : return 1609.27;
    case  6 : return 1.;
    case  7 : return 1000.;
    case  8 : return 0.0000254;
    case  9 : return 0.000001;
    case 10 : return 0.01;
    case 11 : return 0.0000000254;
    default : break;
  }
  return 0.;
}

Standard_CString IGESData_BasicEditor::UnitFlagName (const Standard_Integer flag)
{
  Standard_CString name = "";
  switch (flag) {
    case  1 : name = "INCH";  break;
    case  2 : name = "MM";    break;
    case  4 : name = "FT";    break;
    case  5 : name = "MI";    break;
    case  6 : name = "M";     break;
    case  7 : name = "KM";    break;
    case  8 : name = "MIL";   break;
    case  9 : name = "UM";    break;
    case 10 : name = "CM";    break;
    case 11 : name = "UIN";   break;
    default : break;
  }
  return name;
}

Standard_CString IGESData_BasicEditor::IGESVersionName (const Standard_Integer flag)
{
  switch (flag) {
    case 1 : return "1.0";
    case 2 : return "ANSI Y14.26M-1981";
    case 3 : return "2.0";
    case 4 : return "3.0";
    case 5 : return "ANSI Y14.26M-1987";
    case 6 : return "4.0";
    case 7 : return "ANSI Y14.26M-1989";
    case 8 : return "5.0";
    case 9 : return "5.1";
    case 10: return "5.2";
    case 11: return "5.3";
    default : break;
  }
  return "";
}

Standard_Integer IGESData_BasicEditor::IGESVersionMax ()
      {  return 11;  }

Standard_CString IGESData_BasicEditor::DraftingName (const Standard_Integer flag)
{
  switch (flag) {
    case 0 : return "(None)";
    case 1 : return "ISO";
    case 2 : return "AFNOR";
    case 3 : return "ANSI";
    case 4 : return "BSI";
    case 5 : return "CSA";
    case 6 : return "DIN";
    case 7 : return "JIS";
    default : break;
  }
  return "";
}

Standard_Integer IGESData_BasicEditor::DraftingMax ()
      {  return 7;  }
