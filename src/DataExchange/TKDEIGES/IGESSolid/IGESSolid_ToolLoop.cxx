// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
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

//--------------------------------------------------------------------
//--------------------------------------------------------------------
// pdn 20.04.99 STC22655 avoid of exceptions on empty loops

#include <IGESBasic_HArray1OfHArray1OfIGESEntity.hxx>
#include <IGESBasic_HArray1OfHArray1OfInteger.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_Dump.hxx>
#include <IGESData_IGESEntity.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESData_Status.hxx>
#include <IGESSolid_EdgeList.hxx>
#include <IGESSolid_Loop.hxx>
#include <IGESSolid_ToolLoop.hxx>
#include <IGESSolid_VertexList.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <MoniTool_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Msg.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_Integer.hxx>

#include <cstdio>

// MGE 03/08/98
//=================================================================================================

IGESSolid_ToolLoop::IGESSolid_ToolLoop() = default;

//=================================================================================================

void IGESSolid_ToolLoop::ReadOwnParams(const occ::handle<IGESSolid_Loop>&          ent,
                                       const occ::handle<IGESData_IGESReaderData>& IR,
                                       IGESData_ParamReader&                       PR) const
{
  // MGE 03/08/98

  bool                                  abool; // szv#4:S4163:12Mar99 `st` moved down
  int                                   nbedges = 0;
  int                                   i, j;
  int                                   anint;
  occ::handle<IGESData_IGESEntity>      anent;
  occ::handle<NCollection_HArray1<int>> tempTypes;
  occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>> tempEdges;
  occ::handle<NCollection_HArray1<int>>                              tempIndex;
  occ::handle<NCollection_HArray1<int>>                              tempOrientation;
  occ::handle<NCollection_HArray1<int>>                              nbParameterCurves;
  occ::handle<IGESBasic_HArray1OfHArray1OfInteger>                   isoparametricFlags;
  occ::handle<IGESBasic_HArray1OfHArray1OfIGESEntity>                tempCurves;
  IGESData_Status                                                    aStatus;

  // st = PR.ReadInteger(PR.Current(),Msg184, nbedges); //szv#4:S4163:12Mar99 moved in if
  // st = PR.ReadInteger(PR.Current(), "Number of edges", nbedges);
  bool sb = PR.ReadInteger(PR.Current(), nbedges);
  if (sb && (nbedges > 0))
  {

    Message_Msg Msg180("XSTEP_180");
    Message_Msg Msg190("XSTEP_190");

    tempTypes          = new NCollection_HArray1<int>(1, nbedges);
    tempEdges          = new NCollection_HArray1<occ::handle<IGESData_IGESEntity>>(1, nbedges);
    tempIndex          = new NCollection_HArray1<int>(1, nbedges);
    tempOrientation    = new NCollection_HArray1<int>(1, nbedges);
    nbParameterCurves  = new NCollection_HArray1<int>(1, nbedges);
    isoparametricFlags = new IGESBasic_HArray1OfHArray1OfInteger(1, nbedges);
    tempCurves         = new IGESBasic_HArray1OfHArray1OfIGESEntity(1, nbedges);

    for (i = 1; i <= nbedges; i++)
    {
      // st = PR.ReadInteger(PR.Current(), Msg190, anint); //szv#4:S4163:12Mar99 moved in if
      // st = PR.ReadInteger(PR.Current(), "Edge types", anint);
      if (PR.ReadInteger(PR.Current(), anint))
        tempTypes->SetValue(i, anint);
      else
        PR.SendFail(Msg190);

      if (!PR.ReadEntity(IR, PR.Current(), aStatus, anent))
      { // szv#4:S4163:12Mar99 `st=` not needed
        Message_Msg Msg193("XSTEP_193");
        switch (aStatus)
        {
          case IGESData_ReferenceError: {
            Message_Msg Msg216("IGES_216");
            Msg193.Arg(Msg216.Value());
            PR.SendFail(Msg193);
            break;
          }
          case IGESData_EntityError: {
            Message_Msg Msg217("IGES_217");
            Msg193.Arg(Msg217.Value());
            PR.SendFail(Msg193);
            break;
          }
          default: {
          }
        }
      }
      // st = PR.ReadEntity(IR, PR.Current(), "Edges", anent);
      // if (!st) {  }    // WARNING : Two possible Types : //szv#4:S4163:12Mar99 not needed
      if (!anent->IsKind(STANDARD_TYPE(IGESSolid_VertexList))
          && !anent->IsKind(STANDARD_TYPE(IGESSolid_EdgeList)))
        PR.SendFail(Msg190);
      else
        tempEdges->SetValue(i, anent);

      // st = PR.ReadInteger(PR.Current(), Msg191, anint); //szv#4:S4163:12Mar99 moved in if
      // st = PR.ReadInteger(PR.Current(), "List index", anint);
      if (PR.ReadInteger(PR.Current(), anint))
        tempIndex->SetValue(i, anint);
      else
      {
        Message_Msg Msg191("XSTEP_191");
        PR.SendFail(Msg191);
      }

      // st = PR.ReadBoolean(PR.Current(), Msg180, abool); //szv#4:S4163:12Mar99 moved in if
      // st = PR.ReadBoolean(PR.Current(), "Orientation flags", abool);
      if (PR.ReadBoolean(PR.Current(), Msg180, abool))
        tempOrientation->SetValue(i, (abool ? 1 : 0)); // bool;

      bool st = PR.ReadInteger(PR.Current(), anint);
      if (!st)
      {
        Message_Msg Msg192("XSTEP_192");
        PR.SendFail(Msg192);
      }
      // st = PR.ReadInteger(PR.Current(), "Number of parameter curves", anint);
      if (st && anint > 0)
      {
        Message_Msg Msg195("XSTEP_195");
        nbParameterCurves->SetValue(i, anint);
        occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>> tmpents =
          new NCollection_HArray1<occ::handle<IGESData_IGESEntity>>(1, anint);
        occ::handle<NCollection_HArray1<int>> tmpints = new NCollection_HArray1<int>(1, anint);
        for (j = 1; j <= anint; j++)
        {
          // st = PR.ReadBoolean(PR.Current(), Msg195, abool); //szv#4:S4163:12Mar99 moved in if
          // st = PR.ReadBoolean(PR.Current(), "Isoparametric flags", abool);
          if (PR.ReadBoolean(PR.Current(), Msg195, abool))
            tmpints->SetValue(j, (abool ? 1 : 0)); // bool;

          // st = PR.ReadEntity(IR, PR.Current(), Msg194, anent); //szv#4:S4163:12Mar99 moved in if
          // st = PR.ReadEntity(IR, PR.Current(), "Curves", anent);
          if (PR.ReadEntity(IR, PR.Current(), aStatus, anent))
            tmpents->SetValue(j, anent);
          else
          {
            Message_Msg Msg194("XSTEP_194");
            switch (aStatus)
            {
              case IGESData_ReferenceError: {
                Message_Msg Msg216("IGES_216");
                Msg194.Arg(Msg216.Value());
                PR.SendFail(Msg194);
                break;
              }
              case IGESData_EntityError: {
                Message_Msg Msg217("IGES_217");
                Msg194.Arg(Msg217.Value());
                PR.SendFail(Msg194);
                break;
              }
              default: {
              }
            }
          }
        }
        isoparametricFlags->SetValue(i, tmpints);
        tempCurves->SetValue(i, tmpents);
      }
      else
        nbParameterCurves->SetValue(i, anint);
    }
  }
  else
  {
    // pdn 20.04.99 STC22655 avoid of exceptions on empty loops
    Message_Msg Msg184("XSTEP_184");
    PR.SendFail(Msg184);
    return;
  }

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(), ent);
  ent->Init(tempTypes,
            tempEdges,
            tempIndex,
            tempOrientation,
            nbParameterCurves,
            isoparametricFlags,
            tempCurves);
}

//=================================================================================================

void IGESSolid_ToolLoop::WriteOwnParams(const occ::handle<IGESSolid_Loop>& ent,
                                        IGESData_IGESWriter&               IW) const
{
  int i, j;
  int length = ent->NbEdges();

  IW.Send(length);
  for (i = 1; i <= length; i++)
  {
    IW.Send(ent->EdgeType(i));
    IW.Send(ent->Edge(i));
    IW.Send(ent->ListIndex(i));
    IW.SendBoolean(ent->Orientation(i));
    IW.Send(ent->NbParameterCurves(i));
    for (j = 1; j <= ent->NbParameterCurves(i); j++)
    {
      IW.SendBoolean(ent->IsIsoparametric(i, j));
      IW.Send(ent->ParametricCurve(i, j));
    }
  }
}

//=================================================================================================

void IGESSolid_ToolLoop::OwnShared(const occ::handle<IGESSolid_Loop>& ent,
                                   Interface_EntityIterator&          iter) const
{
  int i, j;
  int length = ent->NbEdges();

  for (i = 1; i <= length; i++)
  {
    iter.GetOneItem(ent->Edge(i));
    for (j = 1; j <= ent->NbParameterCurves(i); j++)
      iter.GetOneItem(ent->ParametricCurve(i, j));
  }
}

//=================================================================================================

void IGESSolid_ToolLoop::OwnCopy(const occ::handle<IGESSolid_Loop>& another,
                                 const occ::handle<IGESSolid_Loop>& ent,
                                 Interface_CopyTool&                TC) const
{
  int nbedges = another->NbEdges();
  int i, j;
  int anint;

  occ::handle<NCollection_HArray1<int>> tempTypes = new NCollection_HArray1<int>(1, nbedges);
  occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>> tempEdges =
    new NCollection_HArray1<occ::handle<IGESData_IGESEntity>>(1, nbedges);
  occ::handle<NCollection_HArray1<int>> tempIndex       = new NCollection_HArray1<int>(1, nbedges);
  occ::handle<NCollection_HArray1<int>> tempOrientation = new NCollection_HArray1<int>(1, nbedges);
  occ::handle<NCollection_HArray1<int>> nbParameterCurves =
    new NCollection_HArray1<int>(1, nbedges);
  occ::handle<IGESBasic_HArray1OfHArray1OfInteger> isoparametricFlags =
    new IGESBasic_HArray1OfHArray1OfInteger(1, nbedges);
  occ::handle<IGESBasic_HArray1OfHArray1OfIGESEntity> tempCurves =
    new IGESBasic_HArray1OfHArray1OfIGESEntity(1, nbedges);

  for (i = 1; i <= nbedges; i++)
  {
    tempTypes->SetValue(i, another->EdgeType(i));

    DeclareAndCast(IGESData_IGESEntity, anent, TC.Transferred(another->Edge(i)));
    tempEdges->SetValue(i, anent);

    tempIndex->SetValue(i, another->ListIndex(i));

    tempOrientation->SetValue(i, (another->Orientation(i) ? 1 : 0));

    anint = another->NbParameterCurves(i);
    nbParameterCurves->SetValue(i, anint);

    occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>> tmpents;
    if (anint > 0)
      tmpents = new NCollection_HArray1<occ::handle<IGESData_IGESEntity>>(1, anint);
    occ::handle<NCollection_HArray1<int>> tmpints;
    if (anint > 0)
      tmpints = new NCollection_HArray1<int>(1, anint);
    for (j = 1; j <= anint; j++)
    {
      tmpints->SetValue(j, (another->IsIsoparametric(i, j) ? 1 : 0));

      DeclareAndCast(IGESData_IGESEntity, localent, TC.Transferred(another->ParametricCurve(i, j)));
      tmpents->SetValue(j, localent);
    }
    isoparametricFlags->SetValue(i, tmpints);
    tempCurves->SetValue(i, tmpents);
  }
  ent->Init(tempTypes,
            tempEdges,
            tempIndex,
            tempOrientation,
            nbParameterCurves,
            isoparametricFlags,
            tempCurves);
}

//=================================================================================================

IGESData_DirChecker IGESSolid_ToolLoop::DirChecker(
  const occ::handle<IGESSolid_Loop>& /* ent  */) const
{
  IGESData_DirChecker DC(508, 0, 1);

  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefVoid);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefVoid);

  DC.SubordinateStatusRequired(1);
  return DC;
}

//=================================================================================================

void IGESSolid_ToolLoop::OwnCheck(const occ::handle<IGESSolid_Loop>& ent,
                                  const Interface_ShareTool&,
                                  occ::handle<Interface_Check>& ach) const
{
  // MGE 03/08/98
  // Building of messages
  //========================================
  // Message_Msg Msg190("XSTEP_190");
  //========================================

  int upper = ent->NbEdges();
  for (int i = 1; i <= upper; i++)
    if (ent->EdgeType(i) != 0 && ent->EdgeType(i) != 1)
    {
      Message_Msg Msg190("XSTEP_190");
      ach->SendFail(Msg190);
    }
}

//=================================================================================================

void IGESSolid_ToolLoop::OwnDump(const occ::handle<IGESSolid_Loop>& ent,
                                 const IGESData_IGESDumper&         dumper,
                                 Standard_OStream&                  S,
                                 const int                          level) const
{
  int i, j;
  int nbedges  = ent->NbEdges();
  int sublevel = (level <= 4) ? 0 : 1;

  S << "IGESSolid_Loop\n"
    << "Edge types :\n"
    << "Edges      :\n"
    << "List index :\n"
    << "Orientation flags :\n"
    << "Parametric flags  : ";
  IGESData_DumpEntities(S, dumper, -level, 1, nbedges, ent->Edge);
  S << "\n";
  if (level > 4)
  {
    S << "[ ";
    for (i = 1; i <= nbedges; i++)
    {
      int nbc = ent->NbParameterCurves(i);
      S << "[" << i << "]:  "
        << "Edge type : " << ent->EdgeType(i) << "  "
        << "Edge : ";
      dumper.Dump(ent->Edge(i), S, sublevel);
      S << "  - Index : " << ent->ListIndex(i)
        << ", Orientation flag : " << (ent->Orientation(i) ? "Positive" : "Negative")
        << ", Number of parametric curves : " << nbc;
      if (nbc != 0)
      {
        if (level <= 5)
          S << "[ ask level > 5 for content ]";
        else
        {
          S << ":\n [ ";
          for (j = 1; j <= nbc; j++)
          {
            S << "[" << j << "]:  "
              << "Isoparametric flag : " << (ent->IsIsoparametric(i, j) ? "True" : "False") << "  "
              << "Parametric curve : ";
            dumper.Dump(ent->ParametricCurve(i, j), S, sublevel);
            S << "\n";
          }
          S << " ]";
        }
      }
      S << "\n";
    }
    S << " ]";
  }
  S << std::endl;
}
