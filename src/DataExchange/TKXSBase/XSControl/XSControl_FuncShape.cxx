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

#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#include <Geom_Geometry.hxx>
#include <IFSelect_Act.hxx>
#include <IFSelect_Functions.hxx>
#include <IFSelect_SessionPilot.hxx>
#include <MoniTool_Macros.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <ShapeExtend_Explorer.hxx>
#include <TCollection_AsciiString.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <Transfer_SimpleBinderOfTransient.hxx>
#include <Transfer_TransientListBinder.hxx>
#include <Transfer_TransientProcess.hxx>
#include <TransferBRep.hxx>
#include <TransferBRep_ShapeListBinder.hxx>
#include <TransferBRep_ShapeMapper.hxx>
#include <XSControl.hxx>
#include <XSControl_ConnectedShapes.hxx>
#include <XSControl_Controller.hxx>
#include <XSControl_FuncShape.hxx>
#include <XSControl_TransferReader.hxx>
#include <XSControl_Vars.hxx>
#include <XSControl_WorkSession.hxx>

#include <stdio.h>

// #include <TransferBRep_Analyzer.hxx>
//   ######################################################################
//   ####                                                              ####
//   ####                           COMMANDS                           ####
//   ####                                                              ####
//   ######################################################################
//=================================================================================================

static IFSelect_ReturnStatus XSControl_tpdraw(const occ::handle<IFSelect_SessionPilot>& pilot)
{
  int                         argc = pilot->NbWords();
  const char* const                   arg1 = pilot->Arg(1);
  const char* const                   arg2 = pilot->Arg(2);
  const char* const                   arg3 = pilot->Arg(3);
  const occ::handle<Transfer_TransientProcess>& TP =
    XSControl::Session(pilot)->TransferReader()->TransientProcess();
  Message_Messenger::StreamBuffer sout = Message::SendInfo();
  if (TP.IsNull())
  {
    sout << "No Transfer Read" << std::endl;
    return IFSelect_RetError;
  }
  //        ****    tpdraw        ****
  if (argc < 2)
  {
    sout << "Donner [mode facultatif : item ou root] , NUMERO , nom DRAW facultatif" << std::endl;
    sout << "  mode si present : item ou root, sinon n0 d entite modele" << std::endl;
    sout << "  INTEGER NUMBER : of entity, of transfer item or of transfer root\n"
         << "    or * to say all" << std::endl;
    return IFSelect_RetError;
  }
  int mode = 0, num = 0;
  if (arg1[0] == 'i')
    mode = 1;
  else if (arg1[0] == 'r')
    mode = 2;
  bool tout = false;
  if (mode == 0)
  {
    if (argc < 2)
    {
      sout << "Donner au moins un NUMERO ou *" << std::endl;
      return IFSelect_RetError;
    }
    if (arg1[0] == '*')
      tout = true;
    else
      num = IFSelect_Functions::GiveEntityNumber(XSControl::Session(pilot), arg1);
  }
  else
  {
    if (arg2[0] == '*')
      tout = true;
    else
      num = IFSelect_Functions::GiveEntityNumber(XSControl::Session(pilot), arg2);
  }

  int           nbvar = 0;
  occ::handle<Transfer_Binder>    binder;
  occ::handle<Standard_Transient> ent;
  TopoDS_Shape               sh;
  char                       nomvar[40];
  //  bool moderoot =  (pilot->Word(0).Value(3) == 'r');

  int                 n1, n2, i, max = 0, index = 0;
  occ::handle<Interface_InterfaceModel> model = TP->Model();
  if (model.IsNull())
  {
    if (mode == 0)
    {
      sout << "No model, specify n0 of transfer item" << std::endl;
      return IFSelect_RetError;
    }
  }
  if (mode == 0)
  {
    sout << "Model entity";
    max = model->NbEntities();
  }
  if (mode == 1)
  {
    sout << "Transfer item";
    max = TP->NbMapped();
  }
  if (mode == 2)
  {
    sout << "Transfer root";
    max = TP->NbRoots();
  }
  if (tout)
  {
    n1 = 1;
    n2 = max;
    sout << ", listing from 1 to " << max << std::endl;
  }
  else if (num <= 0 || num > max)
  {
    sout << " - Num=" << num << " hors limite (de 1 a " << max << ")" << std::endl;
    return IFSelect_RetError;
  }
  else
  {
    n1 = n2 = num;
    nbvar   = -1; // nbvar : 1st simple shape = no n0
    sout << ", n0 " << num << std::endl;
  }

  for (i = n1; i <= n2; i++)
  {
    if (mode == 0)
    {
      ent   = model->Value(i);
      num   = i;
      index = TP->MapIndex(ent);
    }
    else if (mode == 1)
    {
      ent = TP->Mapped(i);
      if (model.IsNull())
        num = 0;
      else
        num = model->Number(ent);
      index = i;
    }
    else if (mode == 2)
    {
      ent = TP->Root(i);
      if (model.IsNull())
        num = 0;
      else
        num = model->Number(ent);
      index = TP->MapIndex(ent);
    }

    if (index > 0)
      binder = TP->MapItem(index);
    if (binder.IsNull())
      index = 0;
    if (index == 0)
    {
      if (!tout)
        sout << "Entite n0 " << num << " : non repertoriee" << std::endl;
      continue;
    }
    if (!binder->HasResult())
    {
      if (!tout)
        sout << "Entity n0 " << num << " : no result" << std::endl;
      continue;
    }
    sh = TransferBRep::ShapeResult(binder);
    // DeclareAndCast(TransferBRep_ShapeBinder,shb,binder);
    if (!sh.IsNull())
    {
      // sh = shb->Result();
      nbvar++;
      if (sh.IsNull())
      {
        sout << " (no Shape recorded)" << std::endl;
        continue;
      }
      if (tout)
        sout << "[ " << i << " ]:";
      if (num == 0)
        sout << " pas dans le modele";
      else
        sout << " ent.n0 " << num;
      sout << ", item transfert n0 " << index;
      if (nbvar == 0)
      {
        if (argc > 3 && mode > 0)
          Sprintf(nomvar, "%s", arg3);
        else if (argc > 2 && mode == 0)
          Sprintf(nomvar, "%s", arg2);
        else
          Sprintf(nomvar, "tp_%d", i);
      }
      else
      {
        if (argc > 3 && mode > 0)
          Sprintf(nomvar, "%s_%d", arg3, nbvar);
        else if (argc > 2 && mode == 0)
          Sprintf(nomvar, "%s_%d", arg2, nbvar);
        else
          Sprintf(nomvar, "tp_%d", i);
      }
      sout << " -> 1 DRAW Shape: " << nomvar << std::endl;
      XSControl::Vars(pilot)->SetShape(nomvar, sh);
      continue;
    }
    DeclareAndCast(TransferBRep_ShapeListBinder, slb, binder);
    if (!slb.IsNull())
    {
      int nbs = slb->NbShapes();
      if (tout)
        sout << "[ " << i << " ]:";
      if (num == 0)
        sout << " pas dans le modele";
      else
        sout << " ent.n0 " << num;
      sout << ", item transfert n0 " << index;
      sout << " -> " << nbs << " DRAW Shapes :";
      for (int j = 1; j <= nbs; j++)
      {
        sh = slb->Shape(j);
        if (nbvar < 0)
          nbvar = 0;
        nbvar++;
        if (sh.IsNull())
        {
          sout << " (no Shape recorded)" << std::endl;
          continue;
        }
        if (argc > 3 && mode > 0)
          Sprintf(nomvar, "%s_%d", arg3, nbvar);
        else if (argc > 2 && mode == 0)
          Sprintf(nomvar, "%s_%d", arg2, nbvar);
        else
          Sprintf(nomvar, "tp_%d_%d", i, nbvar);
        sout << " " << nomvar;
        XSControl::Vars(pilot)->SetShape(nomvar, sh);
      }
      sout << std::endl;
      continue;
    }
    DeclareAndCast(Transfer_SimpleBinderOfTransient, trb, binder);
    if (!trb.IsNull())
    {
      occ::handle<Standard_Transient> resu = trb->Result();
      if (resu.IsNull())
      {
        sout << "Entity n0 " << num << " : no result" << std::endl;
        continue;
      }
      DeclareAndCast(Geom_Geometry, geom, resu);
      sout << "Entite n0 " << num << " : resultat " << resu->DynamicType()->Name();
      if (geom.IsNull())
      {
        sout << std::endl;
        continue;
      }
      nbvar++;
      if (nbvar == 0)
      {
        if (argc > 3 && mode > 0)
          Sprintf(nomvar, "%s", arg3);
        else if (argc > 2 && mode == 0)
          Sprintf(nomvar, "%s", arg2);
        else
          Sprintf(nomvar, "tp_%d", i);
      }
      else
      {
        if (argc > 3 && mode > 0)
          Sprintf(nomvar, "%s_%d", arg3, nbvar);
        else if (argc > 2 && mode == 0)
          Sprintf(nomvar, "%s_%d", arg2, nbvar);
        else
          Sprintf(nomvar, "tp_%d", i);
      }
      char* nomv = nomvar;
      XSControl::Vars(pilot)->Set(nomv, geom);
      sout << " -> DRAW Geom : " << nomvar << std::endl;
      continue;
    }

    if (sh.IsNull() && trb.IsNull())
      if (!tout)
        sout << "Entite n0 " << num << " : resultat pas une Shape mais " << binder->ResultTypeName()
             << std::endl;
  }

  if (sh.IsNull())
    sout << " (No Shape)" << std::endl;
  return IFSelect_RetDone;
}

//=================================================================================================

static IFSelect_ReturnStatus XSControl_tpcompound(const occ::handle<IFSelect_SessionPilot>& pilot)
{
  int                         argc = pilot->NbWords();
  const char* const                   arg1 = pilot->Arg(1);
  const occ::handle<Transfer_TransientProcess>& TP =
    XSControl::Session(pilot)->TransferReader()->TransientProcess();
  Message_Messenger::StreamBuffer sout = Message::SendInfo();
  if (TP.IsNull())
  {
    sout << "No Transfer Read" << std::endl;
    return IFSelect_RetError;
  }
  //        ****    tpcompound        ****
  if (argc < 2)
  {
    sout << "Give a NAME for the Compound  + optional givelist, else roots are taken" << std::endl;
    return IFSelect_RetError;
  }
  occ::handle<NCollection_HSequence<TopoDS_Shape>> list;
  if (argc == 2)
    list = TransferBRep::Shapes(TP);
  else
  {
    occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> lise =
      IFSelect_Functions::GiveList(pilot->Session(), pilot->CommandPart(2));
    if (lise.IsNull())
    {
      sout << "Not a valid entity list : " << pilot->CommandPart(2) << std::endl;
      return IFSelect_RetError;
    }
    list = TransferBRep::Shapes(TP, lise);
    sout << lise->Length() << " Entities, ";
  }
  if (list.IsNull())
  {
    sout << "No Shape listed" << std::endl;
    return IFSelect_RetError;
  }
  int nb = list->Length();
  sout << nb << " Shape(s) listed" << std::endl;
  TopoDS_Compound C;
  BRep_Builder    B;
  B.MakeCompound(C);
  for (int i = 1; i <= nb; i++)
    B.Add(C, list->Value(i));
  XSControl::Vars(pilot)->SetShape(arg1, C);
  return IFSelect_RetDone;
}

//=================================================================================================

static IFSelect_ReturnStatus XSControl_traccess(const occ::handle<IFSelect_SessionPilot>& pilot)
{
  int       argc = pilot->NbWords();
  const char* const arg1 = pilot->Arg(1);
  const char* const arg2 = pilot->Arg(2);
  //        ****    trdraw : TransferReader        **** 26
  //        ****    trsave : TransferReader        **** 27
  //        ****    trcomp  (comp -> DRAW)         **** 28
  //        ****    trscomp (comp -> save)         **** 29
  bool                        cascomp = (pilot->Word(0).Location(1, 'o', 1, 5) > 0);
  bool                        cassave = (pilot->Word(0).Location(1, 's', 1, 5) > 0);
  TCollection_AsciiString                 nomsh, noms;
  const occ::handle<XSControl_TransferReader>& TR   = XSControl::Session(pilot)->TransferReader();
  Message_Messenger::StreamBuffer         sout = Message::SendInfo();
  if (TR.IsNull())
  {
    sout << " manque init" << std::endl;
    return IFSelect_RetError;
  }
  const occ::handle<Interface_InterfaceModel>& mdl = TR->Model();
  if (mdl.IsNull())
  {
    sout << " modele absent" << std::endl;
    return IFSelect_RetError;
  }
  int num =
    (argc > 1 ? IFSelect_Functions::GiveEntityNumber(XSControl::Session(pilot), arg1) : 0);

  if (argc > 1)
    nomsh = arg1;
  else
    nomsh = cascomp ? "TREAD_COMP" : "TREAD_LIST";
  if (cassave)
    sout << " save shapes -> current directory" << std::endl;

  if (num == 0 || cascomp)
  {
    TopoDS_Compound C; // for compound case
    BRep_Builder    B;
    B.MakeCompound(C);

    const occ::handle<NCollection_HSequence<TopoDS_Shape>>& list = TR->ShapeResultList(true);
    sout << " TOUS RESULTATS par ShapeResultList, soit " << list->Length() << std::endl;
    for (int i = 1, nb = list->Length(); i <= nb; ++i)
    {
      noms = nomsh + "_" + i;
      if ((i % 1000) == 0)
        sout << "(" << i << ")" << std::endl;
      else if ((i % 100) == 0)
        sout << "*";
      else if ((i % 10) == 0)
        sout << "0";
      else
        sout << ".";
      if (list->Value(i).IsNull())
        continue;
      if (!cascomp && !cassave)
        XSControl::Vars(pilot)->SetShape(noms.ToCString(), list->Value(i));
      else if (!cascomp && cassave)
        BRepTools::Write(list->Value(i), noms.ToCString());
      else if (cascomp)
        B.Add(C, list->Value(i));
    }
    sout << std::endl;
    if (cascomp && !cassave)
      XSControl::Vars(pilot)->SetShape(nomsh.ToCString(), C);
    else if (cascomp && cassave)
      BRepTools::Write(C, nomsh.ToCString());
  }
  else
  {
    if (num < 1 || num > mdl->NbEntities())
    {
      sout << " incorrect:" << arg1 << std::endl;
      return IFSelect_RetError;
    }
    TopoDS_Shape sh = TR->ShapeResult(mdl->Value(num));
    if (sh.IsNull())
    {
      sout << " No result for " << arg1 << std::endl;
      return IFSelect_RetError;
    }
    if (argc > 2)
      nomsh = arg2;
    else
      nomsh = TCollection_AsciiString("TREAD_") + num;
    if (!cascomp && !cassave)
      XSControl::Vars(pilot)->SetShape(nomsh.ToCString(), sh);
    else if (!cascomp && cassave)
      BRepTools::Write(sh, nomsh.ToCString());
    else
      sout << "Option non comprise" << std::endl;
  }
  return IFSelect_RetDone;
}

//=================================================================================================

// PTV 23.08.2000 Added for checking where are an entity from.
static bool XSControl_IsEqualSubShape(const TopoDS_Shape& Shape,
                                                  TopoDS_Shape&       sh,
                                                  int    aLevel)
{
  if (sh.IsSame(Shape))
    return true;
  if (aLevel > 0)
  {
    TopoDS_Shape varShape;
    aLevel--;
    TopoDS_Iterator it(sh);
    for (; it.More(); it.Next())
    {
      varShape = it.Value();
      if (XSControl_IsEqualSubShape(Shape, varShape, aLevel))
        return true;
    }
  }
  return false;
}

//=================================================================================================

static IFSelect_ReturnStatus XSControl_fromshape(const occ::handle<IFSelect_SessionPilot>& pilot)
{
  int       argc = pilot->NbWords();
  const char* const arg1 = pilot->Arg(1);
  //        ****    fromshape (tread)         ****
  Message_Messenger::StreamBuffer sout = Message::SendInfo();
  if (argc < 2)
  {
    sout << "Give name of a DRAW Shape" << std::endl;
    return IFSelect_RetError;
  }
  const char*  a1    = (char*)arg1;
  TopoDS_Shape Shape = XSControl::Vars(pilot)->GetShape(a1);
  if (Shape.IsNull())
  {
    sout << "Not a DRAW Shape:" << arg1 << std::endl;
    return IFSelect_RetError;
  }
  bool yena   = false;
  int aLevel = 1;
  if (argc >= 3)
    aLevel = atoi(pilot->Arg(2));
  bool silent = false;
  if (aLevel < 0)
  {
    silent = true;
    aLevel = -aLevel;
  }

  //    IMPORT
  const occ::handle<XSControl_TransferReader>& TR = XSControl::Session(pilot)->TransferReader();
  if (TR.IsNull())
  {
  } // sout<<"No read transfer (import) recorded"<<std::endl;
  else
  {
    yena = true;
    if (!silent)
      sout << "Shape " << arg1 << " : ";
    int           modrec = 1;
    occ::handle<Standard_Transient> ent    = TR->EntityFromShapeResult(Shape, modrec);
    if (ent.IsNull())
    {
      modrec = -1;
      ent    = TR->EntityFromShapeResult(Shape, modrec);
    }
    if (ent.IsNull())
    {
      modrec                               = 2;
      occ::handle<Transfer_TransientProcess> TP = TR->TransientProcess();
      if (TP.IsNull())
      {
        if (silent)
          sout << "Shape " << arg1 << " : ";
        sout << "no map" << std::endl;
      }
      else
      {
        TopoDS_Shape    S0 = Shape;
        TopLoc_Location L;
        S0.Location(L);
        int i, nb = TP->NbMapped();
        if (!silent)
          sout << "searching in map among " << nb << " ...";
        for (i = 1; i <= nb; i++)
        {
          ent             = TP->Mapped(i);
          TopoDS_Shape sh = TransferBRep::ShapeResult(TP, ent);
          if (sh.IsNull())
          {
            ent.Nullify();
            continue;
          }
          if (XSControl_IsEqualSubShape(Shape, sh, aLevel))
            break;
          modrec = -2;
          sh.Location(L);
          if (XSControl_IsEqualSubShape(S0, sh, aLevel))
            break;
          ent.Nullify();
          modrec = 2;
        }
      }
    }
    if (!ent.IsNull())
    {
      if (silent)
        sout << "Shape " << arg1 << ": ";
      if (modrec < 0)
        sout << "(moved from origin) ";
      // else sout<<"(origin) ";
    }
    //  on affiche
    if (ent.IsNull())
    {
      if (!silent)
        sout << " unknown as imported";
      // skl 11.05.2004
      // if Shape is a compound try to make "fromshape" for its subshapes
      if (Shape.ShapeType() == TopAbs_COMPOUND)
      {
        sout << std::endl << "Subshapes imported from entities:";
        TopoDS_Iterator Iter(Shape);
        for (; Iter.More(); Iter.Next())
        {
          const TopoDS_Shape&        subsh     = Iter.Value();
          int           submodrec = 1;
          occ::handle<Standard_Transient> subent    = TR->EntityFromShapeResult(subsh, submodrec);
          if (subent.IsNull())
          {
            submodrec = -1;
            subent    = TR->EntityFromShapeResult(subsh, submodrec);
          }
          if (!subent.IsNull())
          {
            sout << "  " << XSControl::Session(pilot)->Model()->Number(subent);
          }
        }
      }
    }
    else
    {
      sout << "imported from entity ";
      XSControl::Session(pilot)->Model()->Print(ent, sout);
      if (silent)
        sout << " in file " << XSControl::Session(pilot)->LoadedFile() << std::endl;
    }
    if (!silent)
      sout << std::endl;
  }

  //   ET EN EXPORT ?
  const occ::handle<Transfer_FinderProcess>& FP =
    XSControl::Session(pilot)->TransferWriter()->FinderProcess();
  if (FP.IsNull())
  {
  }
  else
  {
    yena                           = true;
    occ::handle<Transfer_Finder>    fnd = TransferBRep::ShapeMapper(FP, Shape);
    occ::handle<Standard_Transient> ent;
    if (!fnd.IsNull())
      ent = FP->FindTransient(fnd);
    if (!ent.IsNull())
    {
      sout << "Shape " << arg1 << ": exported to entity ";
      XSControl::Session(pilot)->Model()->Print(ent, sout);
      if (silent)
        sout << " in file " << XSControl::Session(pilot)->LoadedFile();
      sout << std::endl;
    }
    // abv 31.08.00: treat case of split shape (several results)
    // it is supposed that results are of the same type and lie in one-level comp
    else
    {
      occ::handle<Transfer_Binder> bnd = FP->Find(fnd);
      if (!bnd.IsNull())
      {
        occ::handle<Transfer_TransientListBinder> TransientListBinder =
          // occ::down_cast<Transfer_TransientListBinder>( bnd->Next(true) ); //skl
          occ::down_cast<Transfer_TransientListBinder>(bnd); // skl
        if (!TransientListBinder.IsNull())
        {
          int i = 1, nb = TransientListBinder->NbTransients();
          if (nb > 0)
            sout << "Shape " << arg1 << ": exported to entities ";
          for (; i <= nb; i++)
          {
            XSControl::Session(pilot)->Model()->Print(TransientListBinder->Transient(i), sout);
            if (i < nb)
              sout << ", ";
          }
          if (nb > 0)
          {
            if (silent)
              sout << " in file " << XSControl::Session(pilot)->LoadedFile();
            sout << std::endl;
          }
        }
        /*	else {
              TopoDS_Shape comp = TransferBRep::ShapeResult(bnd);
              if ( ! comp.IsNull() && comp.ShapeType() < Shape.ShapeType() ) {
                bool start = true;
                for ( TopoDS_Iterator it(comp); it.More(); it.Next() ) {
                  occ::handle<Transfer_Finder> cfnd = TransferBRep::ShapeMapper (FP,it.Value());
                  if ( cfnd.IsNull() ) continue;
                  occ::handle<Standard_Transient> cent = FP->FindTransient (cfnd);
                  if ( cent.IsNull() ) continue;
                  if ( start )
                sout<<"Shape "<<arg1<<" : exported to entities ";
                  else sout << ", ";
                  start = false;
                  XSControl::Session(pilot)->Model()->Print(cent,sout);
                }
                if ( ! start ) sout<<std::endl;
              }
            }  */
      }
    }
  }
  if (!yena)
    sout << "No transfer (either import or export) recorded" << std::endl;

  return IFSelect_RetVoid;
}

//=================================================================================================

static IFSelect_ReturnStatus XSControl_trconnexentities(const occ::handle<IFSelect_SessionPilot>& pilot)
{
  int       argc = pilot->NbWords();
  const char* const arg1 = pilot->Arg(1);
  //        ****    connected entities (last transfer)         ****
  const occ::handle<XSControl_TransferReader>& TR = XSControl::Session(pilot)->TransferReader();
  occ::handle<Transfer_TransientProcess>       TP;
  if (!TR.IsNull())
    TP = TR->TransientProcess();
  Message_Messenger::StreamBuffer sout = Message::SendInfo();
  if (TP.IsNull())
  {
    sout << "no transfer map" << std::endl;
    return IFSelect_RetVoid;
  }
  if (argc < 2)
  {
    sout << "Give name of a DRAW Shape + optional shape type v-e-w-f(D)-s" << std::endl;
    return IFSelect_RetError;
  }
  const char*  a1    = (const char*)arg1;
  TopoDS_Shape Shape = XSControl::Vars(pilot)->GetShape(a1);
  if (Shape.IsNull())
  {
    sout << "Not a DRAW Shape:" << arg1 << std::endl;
    return IFSelect_RetError;
  }
  sout << "Shape " << arg1 << " : ";

  occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> list =
    XSControl_ConnectedShapes::AdjacentEntities(Shape, TP, TopAbs_FACE);
  int i, nb = list->Length();
  sout << nb << " Entities produced Connected Shapes :" << std::endl;
  const occ::handle<Interface_InterfaceModel>& model = XSControl::Session(pilot)->Model();
  sout << "(";
  for (i = 1; i <= nb; i++)
  {
    if (i > 1)
      sout << ",";
    sout << model->Number(list->Value(i));
  }
  sout << ")" << std::endl;
  return IFSelect_RetDone;
}

//=================================================================================================

static IFSelect_ReturnStatus XSControl_trimport(const occ::handle<IFSelect_SessionPilot>& pilot)
{
  //  FileName or . (for current)  VarName  GiveList (mandatory)
  //    GiveList : * for xst-transferrable-roots
  occ::handle<XSControl_WorkSession> WS = XSControl::Session(pilot);

  int                argc = pilot->NbWords();
  Message_Messenger::StreamBuffer sout = Message::SendInfo();
  if (argc < 4)
  {
    sout << "Give : filename or . for current model;  varname or . to take fileroot\n  GiveList, * "
            "for all transferable roots"
         << std::endl;
    return IFSelect_RetError;
  }
  const char* const arg1 = pilot->Arg(1);
  const char* const arg2 = pilot->Arg(2);
  const char* const arg3 = pilot->Arg(3);

  //  File Name and Variable (root) Name

  TCollection_AsciiString fnom, rnom;
  bool modfic = XSControl_FuncShape::FileAndVar(WS, arg1, arg2, "IMPORT", fnom, rnom);
  if (modfic)
    sout << " File to read : " << fnom << std::endl;
  else
    sout << " Model taken from the session : " << fnom << std::endl;
  sout << " -- Names of variables BREP-DRAW prefixed by : " << rnom << std::endl;

  //  keep the current command, because sub-commands will be called
  TCollection_AsciiString compart = pilot->CommandPart(3);

  //  Reading file if required

  if (modfic)
  {
    TCollection_AsciiString comload("xload ");
    comload.AssignCat(arg1);
    IFSelect_ReturnStatus status = pilot->Execute(comload);
    if (status != IFSelect_RetDone)
    {
      sout << "Abandon import" << std::endl;
      return status;
    }
  }
  else
  {
    sout << "Currently Loaded Model" << std::endl;
  }

  //  Selecting Entities

  occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> list;
  if (arg3[0] == '*' && arg3[1] == '\0')
  {
    list = WS->GiveList("xst-transferrable-roots");
    sout << "All Transferable Roots : ";
  }
  else
  {
    sout << "List given by " << compart.ToCString() << " : ";
    list = WS->GiveList(compart.ToCString());
  }
  if (list.IsNull())
  {
    sout << "No list defined. Abandon" << std::endl;
    return IFSelect_RetError;
  }
  int nbl = list->Length();
  sout << "Nb entities selected : " << nbl << std::endl;

  //  Starting Transfer

  WS->InitTransferReader(0);
  const occ::handle<XSControl_TransferReader>& TR = WS->TransferReader();
  if (TR.IsNull())
  {
    sout << " init not done or failed" << std::endl;
    return IFSelect_RetError;
  }

  TR->BeginTransfer();

  //  Transferring
  int nbt = TR->TransferList(list);
  sout << "Nb Entities Selected : " << nbl << " have given " << nbt << " results" << std::endl;

  //  Filling VARS. one compound (trimpcomp) or one shape per ent (trimport)
  bool iscomp = (pilot->Arg(0)[5] == 'c');
  int nbs    = 0;
  TopoDS_Shape     sh;
  TopoDS_Compound  C;
  BRep_Builder     B;
  B.MakeCompound(C);
  occ::handle<Interface_InterfaceModel> mdl = TR->Model();
  if (mdl.IsNull())
  {
    sout << " modele absent" << std::endl;
    return IFSelect_RetError;
  }
  for (int il = 1; il <= nbl; il++)
  {
    occ::handle<Standard_Transient> ent = list->Value(il);
    sh                             = TR->ShapeResult(ent);
    if (sh.IsNull())
      continue;
    nbs++;
    if (iscomp)
      B.Add(C, sh);
    else
    {
      char nomsh[50];
      Sprintf(nomsh, "%s_%d", rnom.ToCString(), nbs);
      XSControl::Vars(pilot)->SetShape(nomsh, sh);
    }
  }
  if (nbs == 0)
    sout << "No Shape produced" << std::endl;
  else if (nbs == 1)
  {
    sout << "One Shape produced, named " << rnom.ToCString() << std::endl;
    XSControl::Vars(pilot)->SetShape(rnom.ToCString(), sh);
  }
  else if (iscomp)
  {
    sout << "One compound made of " << nbs << " Shapes, named " << rnom.ToCString() << std::endl;
    XSControl::Vars(pilot)->SetShape(rnom.ToCString(), C);
  }
  else
  { // several individual shapes
    sout << nbs << " Shapes, named " << rnom.ToCString() << "_1 to " << rnom.ToCString() << "_"
         << nbs << std::endl;
  }

  return IFSelect_RetDone;
}

//=================================================================================================

static IFSelect_ReturnStatus XSControl_twrite(const occ::handle<IFSelect_SessionPilot>& pilot)
{
  int       argc = pilot->NbWords();
  const char* const arg1 = pilot->Arg(1);
  //        ****    twrite         ****
  Message_Messenger::StreamBuffer  sout = Message::SendInfo();
  occ::handle<XSControl_TransferWriter> TW   = XSControl::Session(pilot)->TransferWriter();
  if (argc < 2)
  {
    sout << " give draw shape name" << std::endl;
    return IFSelect_RetError;
  }
  sout << "Attention, on alimente le modele courant ..." << std::endl;

  // Shape
  for (int i = 1; i < argc; i++)
  {
    const char*  ai    = (const char*)pilot->Arg(i);
    TopoDS_Shape Shape = XSControl::Vars(pilot)->GetShape(ai);
    if (Shape.IsNull())
    {
      sout << "not a draw shape name:" << arg1 << std::endl;
      continue;
    }
    sout << "Pour Shape : " << ai;
    int stat = TW->TransferWriteShape(XSControl::Session(pilot)->Model(), Shape);
    sout << " Transfer Write Status = " << stat << std::endl;
  }
  pilot->Session()->ComputeGraph();
  // Transient ? (Geom) : ignore
  return IFSelect_RetDone;
}

//  ######################################################################
//  ####                    TIMERS                                    ####
//  ######################################################################

//  ######################################################################
//  ####                                                              ####
//  ####                    Initialising Commands                     ####
//  ####                                                              ####
//  ######################################################################

static int THE_XSControl_FuncShape_initactor = 0;

//=================================================================================================

void XSControl_FuncShape::Init()
{
  if (THE_XSControl_FuncShape_initactor)
  {
    return;
  }

  THE_XSControl_FuncShape_initactor = 1;

  IFSelect_Act::SetGroup("DE: General");

  IFSelect_Act::AddFunc(
    "tpdraw",
    "[mode:item or root]  num|*  [nomvar] Passes an ITEM to Shape Draw (Start or Result)",
    XSControl_tpdraw);

  IFSelect_Act::AddFunc("tpcompound",
                        "name:cstring [givelist] : -> compound with Shapes Root or from givelist",
                        XSControl_tpcompound);

  IFSelect_Act::AddFunc(
    "trdraw",
    "results ->DRAW : all;  or num [name] : from ent.num -> DRAW [name/tread_num]",
    XSControl_traccess);
  IFSelect_Act::AddFunc(
    "trsave",
    "results ->files : all;  or num [name] : from ent.num -> DRAW [name/tread_num]",
    XSControl_traccess);
  IFSelect_Act::AddFunc("trcomp",
                        "results -> 1 compound -> DRAW + name optional",
                        XSControl_traccess);
  IFSelect_Act::AddFunc("trscomp",
                        "results -> 1 compound -> file + name optional",
                        XSControl_traccess);

  IFSelect_Act::AddFunc("fromshape",
                        "shape [level=1]: imported/exported entity (when known)",
                        XSControl_fromshape);
  IFSelect_Act::AddFunc("trconnexent",
                        "name of draw shape : entities -> connected shapes (when known)",
                        XSControl_trconnexentities);

  IFSelect_Act::AddFunc("trimport",
                        "filename or .  varname  givelist  -> 1 shape per entity",
                        XSControl_trimport);
  IFSelect_Act::AddFunc("trimpcomp",
                        "filename or .  varname  givelist -> one xcompound",
                        XSControl_trimport);

  IFSelect_Act::AddFunc("twrite",
                        "shape : transfer write for this shape, AFTER newmodel !",
                        XSControl_twrite);

  // skl IFSelect_Act::AddFunc ("checkbrep","shapename or * [+ rootname for expurged and faulties [+
  // mode f-s]]",XSHAPE_checkbrep); skl IFSelect_Act::AddFunc ("dblist","option : clear  nb  index
  // set n1  data n1 n2",XSHAPE_dblist);
}

//  ######################################################################
//  ####                                                              ####
//  ####                      Additional Methods                      ####
//  ####                                                              ####
//  ######################################################################

//=================================================================================================

int XSControl_FuncShape::MoreShapes(const occ::handle<XSControl_WorkSession>& session,
                                                 occ::handle<NCollection_HSequence<TopoDS_Shape>>&   list,
                                                 const char* const               name)
{
  //  name = un nom -> Draw
  //  name = "*"    -> tous les transferts RACINES du TP
  //  name = "**"   -> tous les transferts du TP : VRAIMENT TOUS
  //  name = "."    -> reperage graphique (not yet impl)
  //  name = nom(n1-n2) with n1,n2 integers :  the variables of name  nomn1 to nomn2
  Message_Messenger::StreamBuffer sout = Message::SendInfo();
  if (list.IsNull())
    list = new NCollection_HSequence<TopoDS_Shape>();
  if (name[0] == '*' && (name[1] == '\0' || (name[1] == '*' && name[2] == '\0')))
  {
    const occ::handle<Transfer_TransientProcess>& TP = session->TransferReader()->TransientProcess();
    if (TP.IsNull())
    {
      sout << "last transfer : unknown" << std::endl;
      return 0;
    }
    occ::handle<NCollection_HSequence<TopoDS_Shape>> li = TransferBRep::Shapes(TP, (name[1] == '\0'));
    if (li.IsNull())
      return 0;
    list->Append(li);
    return li->Length();
  }
  int i, paro = 0, parf = 0, moins = 0, n1 = 0, n2 = 0;
  for (i = 0; name[i] != '\0'; i++)
  {
    if (name[i] == '(')
      paro = i;
    if (name[i] == '-')
      moins = i;
    if (name[i] == ')')
      parf = i;
  }
  if (paro && moins && parf)
  {
    n2 = atoi(&name[moins + 1]);
    n1 = atoi(&name[paro + 1]);
    if (n1 < 0)
      n1 += n2; // otherwise we have n1-n2
  }
  //  liste
  if (n1 <= n2 && n1 > 0)
  {
    char             nom[50], nomsh[60];
    int nbsh = 0;
    for (i = 0; i < paro; i++)
    {
      nom[i] = name[i];
    }
    nom[paro] = '\0';
    sout << "Shapes DRAW named : " << nom << n1 << " to " << nom << n2;
    for (i = n1; i <= n2; i++)
    {
      const char* nomshh = &nomsh[0];
      Sprintf(nomsh, "%s%d", nom, i);
      TopoDS_Shape Shape = session->Vars()->GetShape(nomshh);
      if (Shape.IsNull())
        continue;
      list->Append(Shape);
      nbsh++;
    }
    sout << "  -> taken " << nbsh << " Shapes" << std::endl;
    return nbsh;
  }
  const char*  a1    = (const char*)name;
  TopoDS_Shape Shape = session->Vars()->GetShape(a1);
  if (Shape.IsNull())
  {
    sout << "not a shape draw:" << a1 << std::endl;
    return 0;
  }
  list->Append(Shape);
  return 1;
}

//=================================================================================================

bool XSControl_FuncShape::FileAndVar(const occ::handle<XSControl_WorkSession>& session,
                                                 const char* const               file,
                                                 const char* const               var,
                                                 const char* const               def,
                                                 TCollection_AsciiString&             resfile,
                                                 TCollection_AsciiString&             resvar)
{
  bool iafic = true;
  resfile.Clear();
  resvar.Clear();
  if (file)
    if (file[0] == '\0' || (file[0] == '.' && file[1] == '\0'))
      iafic = false;
  if (!iafic)
    resfile.AssignCat(session->LoadedFile());
  else
    resfile.AssignCat(file);

  if (var && var[0] != '\0' && (var[0] != '.' || var[1] != '\0'))
    resvar.AssignCat(var);
  else if (resfile.Length() == 0)
    resvar.AssignCat(def);
  else
  {
    int nomdeb, nomfin;
    nomdeb = resfile.SearchFromEnd("/");
    if (nomdeb <= 0)
      nomdeb = resfile.SearchFromEnd("\\"); // for NT
    if (nomdeb < 0)
      nomdeb = 0;
    nomfin = resfile.SearchFromEnd(".");
    if (nomfin < nomdeb)
      nomfin = resfile.Length() + 1;
    resvar = resfile.SubString(nomdeb + 1, nomfin - 1);
  }
  return iafic;
}
