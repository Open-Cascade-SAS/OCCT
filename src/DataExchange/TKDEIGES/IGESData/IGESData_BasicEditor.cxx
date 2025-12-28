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

// #73 rln 10.03.99 S4135: "read.scale.unit" does not affect GlobalSection

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
#include <Interface_EntityIterator.hxx>
#include <Interface_Graph.hxx>
#include <MoniTool_Macros.hxx>
#include <TCollection_HAsciiString.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>

IGESData_BasicEditor::IGESData_BasicEditor(const occ::handle<IGESData_Protocol>& protocol)
{
  Init(protocol);
}

IGESData_BasicEditor::IGESData_BasicEditor(const occ::handle<IGESData_IGESModel>& model,
                                           const occ::handle<IGESData_Protocol>&  protocol)
{
  Init(model, protocol);
}

IGESData_BasicEditor::IGESData_BasicEditor() {}

void IGESData_BasicEditor::Init(const occ::handle<IGESData_Protocol>& protocol)
{
  theunit  = false;
  theproto = protocol;
  themodel = GetCasted(IGESData_IGESModel, Interface_InterfaceModel::Template("iges"));
  theglib  = Interface_GeneralLib(protocol);
  theslib  = protocol;
}

void IGESData_BasicEditor::Init(const occ::handle<IGESData_IGESModel>& model,
                                const occ::handle<IGESData_Protocol>&  protocol)
{
  theunit  = false;
  theproto = protocol;
  themodel = model;
  theglib  = Interface_GeneralLib(protocol);
  theslib  = protocol;
}

occ::handle<IGESData_IGESModel> IGESData_BasicEditor::Model() const
{
  return themodel;
}

// ####   Work on the Header (GlobalSection)    ####

bool IGESData_BasicEditor::SetUnitFlag(const int flag)
{
  if (themodel.IsNull())
    return false;
  if (flag < 1 || flag > 11)
    return false;
  IGESData_GlobalSection                GS   = themodel->GlobalSection();
  occ::handle<TCollection_HAsciiString> name = GS.UnitName();
  const char*                           nam  = IGESData_BasicEditor::UnitFlagName(flag);
  if (nam[0] != '\0')
    name = new TCollection_HAsciiString(nam);
  GS.SetUnitFlag(flag);
  GS.SetUnitName(name);
  themodel->SetGlobalSection(GS);
  theunit = true;
  return true;
}

bool IGESData_BasicEditor::SetUnitValue(const double val)
{
  if (val <= 0.)
    return false;
  double vmm = val * themodel->GlobalSection().CascadeUnit();
  // #73 rln 10.03.99 S4135: "read.scale.unit" does not affect GlobalSection
  // if (Interface_Static::IVal("read.scale.unit") == 1) vmm = vmm * 1000.;
  // vmm is expressed in MILLIMETERS
  int aFlag = GetFlagByValue(vmm);
  return (aFlag > 0) ? SetUnitFlag(aFlag) : false;
}

//=================================================================================================

int IGESData_BasicEditor::GetFlagByValue(const double theValue)
{
  if (theValue >= 25. && theValue <= 26.)
    return 1;
  if (theValue >= 0.9 && theValue <= 1.1)
    return 2;
  if (theValue >= 300. && theValue <= 310.)
    return 4;
  if (theValue >= 1600000. && theValue <= 1620000.)
    return 5;
  if (theValue >= 990. && theValue <= 1010.)
    return 6;
  if (theValue >= 990000. && theValue <= 1010000.)
    return 7;
  if (theValue >= 0.025 && theValue <= 0.026)
    return 8;
  if (theValue >= 0.0009 && theValue <= 0.0011)
    return 9;
  if (theValue >= 9. && theValue <= 11.)
    return 10;
  if (theValue >= 0.000025 && theValue <= 0.000026)
    return 11;
  return 0;
}

//=================================================================================================

bool IGESData_BasicEditor::SetUnitName(const char* name)
{
  if (themodel.IsNull())
    return false;
  int                    flag = IGESData_BasicEditor::UnitNameFlag(name);
  IGESData_GlobalSection GS   = themodel->GlobalSection();
  if (GS.UnitFlag() == 3)
  {
    char* nam = (char*)name;
    if (name[1] == 'H')
      nam = (char*)&name[2];
    GS.SetUnitName(new TCollection_HAsciiString(nam));
    themodel->SetGlobalSection(GS);
    return true;
  }
  if (flag > 0)
    return SetUnitFlag(flag);
  return (flag > 0);
}

void IGESData_BasicEditor::ApplyUnit(const bool enforce)
{
  if (themodel.IsNull())
    return;
  if (!enforce && !theunit)
    return;
  IGESData_GlobalSection GS   = themodel->GlobalSection();
  double                 unit = GS.UnitValue();
  if (unit <= 0.)
    return;
  if (unit != 1.)
  {
    GS.SetMaxLineWeight(GS.MaxLineWeight() / unit);
    GS.SetResolution(GS.Resolution() / unit);
    GS.SetMaxCoord(GS.MaxCoord() / unit);
    themodel->SetGlobalSection(GS);
  }
  theunit = false;
}

// ####   Global work on entities    ####

void IGESData_BasicEditor::ComputeStatus()
{
  if (themodel.IsNull())
    return;
  int nb = themodel->NbEntities();
  if (nb == 0)
    return;
  NCollection_Array1<int> subs(0, nb);
  subs.Init(0);                         // gere Subordinate Status
  Interface_Graph G(themodel, theglib); // gere & memorise UseFlag
  G.ResetStatus();

  //  2 phases : first we do an overall calculation. Then we apply
  //             The whole model is processed, no favorites

  //  Each entity will give a contribution on its own descendants :
  //  for Subordinate (1 or 2 cumulative), for UseFlag (1 to 6 exclusive)
  //    (6 since IGES-5.2)

  //  For Subordinate : Drawing and 402 (except maybe dimensioned geometry ?) give
  //   Logical, the rest implies Physical (on direct descendants)

  //  For UseFlag, a bit more complicated :
  //  On one hand, UseFlags propagate to direct descendants or not
  //  On the other hand cases are more complicated (and not as clear)

  //  WARNING, we can only process what can be deduced from the graph by relying
  //  on "IGES Type Number", we don't have the right here to access the
  //  specific description of different types : handled by AutoCorrect.
  //  Example : a curve is 3D or parametric 2D(UV), not only according to its
  //  ancestor, but according to the role it plays there (ex. for CurveOnSurface :
  //  CurveUV/Curve3D)
  //  Currently handled (necessary) :
  //  1(Annotation), also 4(for meshing). 5(ParamUV) handled by AutoCorrect

  int CN;
  int i; // svv Jan11 2000 : porting on DEC
  for (i = 1; i <= nb; i++)
  {
    //  Subordinate (on direct descendants only)
    occ::handle<IGESData_IGESEntity>     ent = themodel->Entity(i);
    int                                  igt = ent->TypeNumber();
    occ::handle<Interface_GeneralModule> gmodule;
    if (theglib.Select(ent, gmodule, CN))
    {
      occ::handle<IGESData_GeneralModule> gmod = occ::down_cast<IGESData_GeneralModule>(gmodule);
      Interface_EntityIterator            sh;
      gmod->OwnSharedCase(CN, ent, sh);
      for (sh.Start(); sh.More(); sh.Next())
      {
        int nums = themodel->Number(sh.Value());
        if (igt == 402 || igt == 404)
          subs.SetValue(nums, subs.Value(nums) | 2);
        else
          subs.SetValue(nums, subs.Value(nums) | 1);
        ////	std::cout<<"ComputeStatus : nums = "<<nums<<" ->"<<subs.Value(nums)<<std::endl;
      }
    }
    //  UseFlag (a propager)
    if (igt / 100 == 2)
    {
      G.GetFromEntity(ent, true, 1); // Annotation
      G.GetFromEntity(ent, false, ent->UseFlag());
    }
    else if (igt == 134 || igt == 116 || igt == 132)
    {
      Interface_EntityIterator sh = G.Sharings(ent); // Maillage ...
      if (sh.NbEntities() > 0)
        G.GetFromEntity(ent, true, 4);
      //  UV : see AutoCorrect of concerned classes (Boundary and CurveOnSurface)
      /*
          } else if (ent->IsKind(STANDARD_TYPE(IGESGeom_CurveOnSurface))) {
            DeclareAndCast(IGESGeom_CurveOnSurface,cos,ent);    // Curve UV
            G.GetFromEntity (cos->CurveUV(),true,5);
          } else if (ent->IsKind(STANDARD_TYPE(IGESGeom_Boundary))) {
            DeclareAndCast(IGESGeom_Boundary,bnd,ent);          // Curve UV
            int nc = bnd->NbModelSpaceCurves();
            for (int ic = 1; ic <= nc; ic ++) {
          int nuv = bnd->NbParameterCurves(ic);
          for (int juv = 1; juv <= nuv; juv ++)
            G.GetFromEntity(bnd->ParameterCurve(ic,juv),true,5);
            }
      */
    }
  }

  //  Now, we will apply all this "by force"
  //  Only exception: non-zero UseFlags already in place are left

  for (i = 1; i <= nb; i++)
  {
    occ::handle<IGESData_IGESEntity> ent = themodel->Entity(i);
    int                              bl  = ent->BlankStatus();
    int                              uf  = ent->UseFlag();
    if (uf == 0)
      uf = G.Status(i);
    int hy = ent->HierarchyStatus();
    ////    std::cout<<" Ent.n0."<<i<<" Subord="<<subs.Value(i)<<" Use="<<uf<<std::endl;
    ent->InitStatus(bl, subs.Value(i), uf, hy);
  }
}

bool IGESData_BasicEditor::AutoCorrect(const occ::handle<IGESData_IGESEntity>& ent)
{
  if (themodel.IsNull())
    return false;
  occ::handle<IGESData_IGESEntity>         bof, subent;
  occ::handle<IGESData_LineFontEntity>     linefont;
  occ::handle<IGESData_LevelListEntity>    levelist;
  occ::handle<IGESData_ViewKindEntity>     view;
  occ::handle<IGESData_TransfEntity>       transf;
  occ::handle<IGESData_LabelDisplayEntity> labdisp;
  occ::handle<IGESData_ColorEntity>        color;

  bool done = false;
  if (ent.IsNull())
    return done;
  //    Corrections in the header (present entities)
  //    We don't check "Shared" items, present anyway
  //    Header : handled by DirChecker for standard cases
  /*
    linefont = ent->LineFont();
    if (!linefont.IsNull() && themodel->Number(linefont) == 0) {
      linefont.Nullify();
      ent->InitLineFont(linefont,0);
      done = true;
    }
    levelist = ent->LevelList();
    if (!levelist.IsNull() && themodel->Number(levelist) == 0) {
      levelist.Nullify();
      ent->InitLevel(levelist,0);
      done = true;
    }
    view = ent->View();
    if (!view.IsNull() && themodel->Number(view) == 0) {
      view.Nullify();
      ent->InitView(view);
      done = true;
    }
    transf = ent->Transf();
    if (!transf.IsNull() && themodel->Number(transf) == 0) {
      transf.Nullify();
      ent->InitTransf(transf);
      done = true;
    }
    labdisp = ent->LabelDisplay();
    if (!labdisp.IsNull() && themodel->Number(labdisp) == 0) {
      labdisp.Nullify();
      ent->InitMisc (ent->Structure(),labdisp,ent->LineWeightNumber());
      done = true;
    }
    color = ent->Color();
    if (!color.IsNull() && themodel->Number(color) == 0) {
      color.Nullify();
      ent->InitColor(color,0);
      done = true;
    }
  */

  //    Corrections in Assocs (Props remain attached to the Entity)
  Interface_EntityIterator iter = ent->Associativities();
  for (iter.Start(); iter.More(); iter.Next())
  {
    subent = GetCasted(IGESData_IGESEntity, iter.Value());
    if (!subent.IsNull() && themodel->Number(subent) == 0)
    {
      subent->Dissociate(ent);
      done = true;
    }
  }

  //    Corrections specifiques
  int                                  CN;
  occ::handle<Interface_GeneralModule> gmodule;
  if (theglib.Select(ent, gmodule, CN))
  {
    occ::handle<IGESData_GeneralModule> gmod = occ::down_cast<IGESData_GeneralModule>(gmodule);
    IGESData_DirChecker                 DC   = gmod->DirChecker(CN, ent);
    done |= DC.Correct(ent);
  }

  occ::handle<IGESData_SpecificModule> smod;
  if (theslib.Select(ent, smod, CN))
    done |= smod->OwnCorrect(CN, ent);

  return done;
}

int IGESData_BasicEditor::AutoCorrectModel()
{
  int res = 0;
  int nb  = themodel->NbEntities();
  for (int i = 1; i <= nb; i++)
  {
    if (AutoCorrect(themodel->Entity(i)))
      res++;
  }
  return res;
}

//=================================================================================================

int IGESData_BasicEditor::UnitNameFlag(const char* name)
{
  char* nam = (char*)&name[0];
  if (name[1] == 'H')
  {
    nam = (char*)&name[2];
  }
  if (!strcmp(nam, "INCH"))
    return 1;
  if (!strcmp(nam, "IN"))
    return 1;
  if (!strcmp(nam, "MM"))
    return 2;
  if (!strcmp(nam, "FT"))
    return 4;
  if (!strcmp(nam, "MI"))
    return 5;
  if (!strcmp(nam, "M"))
    return 6;
  if (!strcmp(nam, "KM"))
    return 7;
  if (!strcmp(nam, "MIL"))
    return 8;
  if (!strcmp(nam, "UM"))
    return 9;
  if (!strcmp(nam, "CM"))
    return 10;
  if (!strcmp(nam, "UIN"))
    return 11;
  return 0;
}

double IGESData_BasicEditor::UnitFlagValue(const int flag)
{
  switch (flag)
  {
    case 1:
      return 25.4; // inch
    case 2:
      return 1.; // millimeter
    case 3:
      return 1.;
    case 4:
      return 304.8; // foot
    case 5:
      return 1609344.; // mile
    case 6:
      return 1000.; // meter
    case 7:
      return 1000000.; // kilometer
    case 8:
      return 0.0254; // mil (0.001 inch)
    case 9:
      return 0.001; // micron
    case 10:
      return 10.; // centimeter
    case 11:
      return 0.0000254; // microinch
    default:
      return 0.;
  }
}

const char* IGESData_BasicEditor::UnitFlagName(const int flag)
{
  const char* name = "";
  switch (flag)
  {
    case 1:
      name = "INCH";
      break;
    case 2:
      name = "MM";
      break;
    case 4:
      name = "FT";
      break;
    case 5:
      name = "MI";
      break;
    case 6:
      name = "M";
      break;
    case 7:
      name = "KM";
      break;
    case 8:
      name = "MIL";
      break;
    case 9:
      name = "UM";
      break;
    case 10:
      name = "CM";
      break;
    case 11:
      name = "UIN";
      break;
    default:
      break;
  }
  return name;
}

const char* IGESData_BasicEditor::IGESVersionName(const int flag)
{
  switch (flag)
  {
    case 1:
      return "1.0";
    case 2:
      return "ANSI Y14.26M-1981";
    case 3:
      return "2.0";
    case 4:
      return "3.0";
    case 5:
      return "ANSI Y14.26M-1987";
    case 6:
      return "4.0";
    case 7:
      return "ANSI Y14.26M-1989";
    case 8:
      return "5.0";
    case 9:
      return "5.1";
    case 10:
      return "5.2";
    case 11:
      return "5.3";
    default:
      break;
  }
  return "";
}

int IGESData_BasicEditor::IGESVersionMax()
{
  return 11;
}

const char* IGESData_BasicEditor::DraftingName(const int flag)
{
  switch (flag)
  {
    case 0:
      return "(None)";
    case 1:
      return "ISO";
    case 2:
      return "AFNOR";
    case 3:
      return "ANSI";
    case 4:
      return "BSI";
    case 5:
      return "CSA";
    case 6:
      return "DIN";
    case 7:
      return "JIS";
    default:
      break;
  }
  return "";
}

int IGESData_BasicEditor::DraftingMax()
{
  return 7;
}
