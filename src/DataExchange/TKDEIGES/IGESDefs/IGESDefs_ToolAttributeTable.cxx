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

#include <IGESData_DirChecker.hxx>
#include <IGESData_Dump.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESDefs_AttributeDef.hxx>
#include <IGESDefs_AttributeTable.hxx>
#include <IGESDefs_ToolAttributeTable.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <TCollection_HAsciiString.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <MoniTool_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <TCollection_HAsciiString.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_HArray2.hxx>

IGESDefs_ToolAttributeTable::IGESDefs_ToolAttributeTable() {}

void IGESDefs_ToolAttributeTable::ReadOwnParams(const occ::handle<IGESDefs_AttributeTable>& ent,
                                                const occ::handle<IGESData_IGESReaderData>& IR,
                                                IGESData_ParamReader&                  PR) const
{
  int nr = 1;
  int j;
  bool st = true;

  occ::handle<IGESDefs_AttributeDef>      ab = ent->Definition(); // formerly loaded
  occ::handle<NCollection_HArray2<occ::handle<Standard_Transient>>> list2;
  if (ab.IsNull())
  {
    PR.AddFail("No Attribute Definition as Structure");
    return;
  }
  int na = ab->NbAttributes();

  if (ent->FormNumber() == 1)
    st = PR.ReadInteger(PR.Current(), "No. of rows", nr);
  if (st)
    list2 = new NCollection_HArray2<occ::handle<Standard_Transient>>(1, na, 1, nr);

  //  AttributeDef repeated once (Form 0) or <nr> times (Form 1)
  for (int k = 1; k <= nr; k++)
  {
    for (int i = 1; i <= na; i++)
    {
      int avc   = ab->AttributeValueCount(i);
      int atype = ab->AttributeValueDataType(i);
      switch (atype)
      {
        case 0:
          for (j = 1; j <= avc; j++)
            PR.SetCurrentNumber(PR.CurrentNumber() + 1); // skip
          break;
        case 1: {
          occ::handle<NCollection_HArray1<int>> attrInt = new NCollection_HArray1<int>(1, avc);
          list2->SetValue(i, k, attrInt);
          int item;
          for (j = 1; j <= avc; j++)
          {
            // st = PR.ReadInteger(PR.Current(),"Value",item); //szv#4:S4163:12Mar99 moved in if
            if (PR.ReadInteger(PR.Current(), "Value", item))
              attrInt->SetValue(j, item);
          }
        }
        break;
        case 2: {
          occ::handle<NCollection_HArray1<double>> attrReal = new NCollection_HArray1<double>(1, avc);
          list2->SetValue(i, k, attrReal);
          double item;
          for (j = 1; j <= avc; j++)
          {
            // st = PR.ReadReal(PR.Current(),"Value",item); //szv#4:S4163:12Mar99 moved in if
            if (PR.ReadReal(PR.Current(), "Value", item))
              attrReal->SetValue(j, item);
          }
        }
        break;
        case 3: {
          occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>> attrStr =
            new NCollection_HArray1<occ::handle<TCollection_HAsciiString>>(1, avc);
          list2->SetValue(i, k, attrStr);
          occ::handle<TCollection_HAsciiString> item;
          for (j = 1; j <= avc; j++)
          {
            // st = PR.ReadText(PR.Current(),"Value",item); //szv#4:S4163:12Mar99 moved in if
            if (PR.ReadText(PR.Current(), "Value", item))
              attrStr->SetValue(j, item);
          }
        }
        break;
        case 4: {
          occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>> attrEnt = new NCollection_HArray1<occ::handle<IGESData_IGESEntity>>(1, avc);
          list2->SetValue(i, k, attrEnt);
          occ::handle<IGESData_IGESEntity> item;
          for (j = 1; j <= avc; j++)
          {
            // st = PR.ReadEntity(IR,PR.Current(),"Value",item); //szv#4:S4163:12Mar99 moved in if
            if (PR.ReadEntity(IR, PR.Current(), "Value", item))
              attrEnt->SetValue(j, item);
          }
        }
        break;
        case 5:
          for (j = 1; j <= avc; j++)
            PR.SetCurrentNumber(PR.CurrentNumber() + 1); // skip
          break;
        case 6: { // Here item takes value 0 or 1
          occ::handle<NCollection_HArray1<int>> attrInt = new NCollection_HArray1<int>(1, avc);
          list2->SetValue(i, k, attrInt);
          int item;
          for (j = 1; j <= avc; j++)
          {
            // st = PR.ReadInteger(PR.Current(),"Value",item); //szv#4:S4163:12Mar99 moved in if
            if (PR.ReadInteger(PR.Current(), "Value", item))
              attrInt->SetValue(j, item);
          }
        }
        break;
        default:
          break;
      }
    }
  }
  DirChecker(ent).CheckTypeAndForm(PR.CCheck(), ent);
  ent->Init(list2);
}

void IGESDefs_ToolAttributeTable::WriteOwnParams(const occ::handle<IGESDefs_AttributeTable>& ent,
                                                 IGESData_IGESWriter&                   IW) const
{
  occ::handle<IGESDefs_AttributeDef> ab = ent->Definition();

  int nr = ent->NbRows();
  int na = ent->NbAttributes();
  if (ent->FormNumber() == 1)
    IW.Send(nr);
  for (int k = 1; k <= nr; k++)
  {
    for (int i = 1; i <= na; i++)
    {
      int count = ab->AttributeValueCount(i);
      for (int j = 1; j <= count; j++)
      {
        switch (ab->AttributeValueDataType(i))
        {
          case 0:
            IW.SendVoid();
            break;
          case 1:
            IW.Send(ent->AttributeAsInteger(i, k, j));
            break;
          case 2:
            IW.Send(ent->AttributeAsReal(i, k, j));
            break;
          case 3:
            IW.Send(ent->AttributeAsString(i, k, j));
            break;
          case 4:
            IW.Send(ent->AttributeAsEntity(i, k, j));
            break;
          case 5:
            IW.SendVoid();
            break;
          case 6:
            IW.SendBoolean(ent->AttributeAsLogical(i, k, j));
            break;
          default:
            break;
        }
      }
    }
  }
}

void IGESDefs_ToolAttributeTable::OwnShared(const occ::handle<IGESDefs_AttributeTable>& ent,
                                            Interface_EntityIterator&              iter) const
{
  occ::handle<IGESDefs_AttributeDef> ab = ent->Definition();
  int              na = ent->NbAttributes();
  int              nr = ent->NbRows();
  for (int k = 1; k <= nr; k++)
  {
    for (int i = 1; i <= na; i++)
    {
      if (ab->AttributeValueDataType(i) != 4)
        continue;
      int avc = ab->AttributeValueCount(i);
      for (int j = 1; j <= avc; j++)
        iter.GetOneItem(ent->AttributeAsEntity(i, k, j));
    }
  }
}

void IGESDefs_ToolAttributeTable::OwnCopy(const occ::handle<IGESDefs_AttributeTable>& another,
                                          const occ::handle<IGESDefs_AttributeTable>& ent,
                                          Interface_CopyTool&                    TC) const
{
  int                   j     = 1;
  occ::handle<IGESDefs_AttributeDef>      ab    = another->Definition();
  int                   na    = another->NbAttributes();
  int                   nr    = another->NbRows();
  occ::handle<NCollection_HArray2<occ::handle<Standard_Transient>>> list2 = new NCollection_HArray2<occ::handle<Standard_Transient>>(1, na, 1, nr);
  for (int k = 1; k <= nr; k++)
  {
    for (int i = 1; i <= na; i++)
    {
      int avc   = ab->AttributeValueCount(i);
      int atype = ab->AttributeValueDataType(i);
      switch (atype)
      {
        case 0: ////    list2->SetValue(i,k,NULL);    by default
          break;
        case 1: {
          DeclareAndCast(NCollection_HArray1<int>, otherInt, another->AttributeList(i, k));
          occ::handle<NCollection_HArray1<int>> attrInt = new NCollection_HArray1<int>(1, avc);
          list2->SetValue(i, k, attrInt);
          for (j = 1; j <= avc; j++)
            attrInt->SetValue(j, otherInt->Value(j));
        }
        break;
        case 2: {
          DeclareAndCast(NCollection_HArray1<double>, otherReal, another->AttributeList(i, k));
          occ::handle<NCollection_HArray1<double>> attrReal = new NCollection_HArray1<double>(1, avc);
          list2->SetValue(i, k, attrReal);
          for (j = 1; j <= avc; j++)
            attrReal->SetValue(j, otherReal->Value(j));
        }
        break;
        case 3: {
          DeclareAndCast(NCollection_HArray1<occ::handle<TCollection_HAsciiString>>, otherStr, another->AttributeList(i, k));
          occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>> attrStr =
            new NCollection_HArray1<occ::handle<TCollection_HAsciiString>>(1, avc);
          list2->SetValue(i, k, attrStr);
          for (j = 1; j <= avc; j++)
            attrStr->SetValue(j, new TCollection_HAsciiString(otherStr->Value(j)));
        }
        break;
        case 4: {
          DeclareAndCast(NCollection_HArray1<occ::handle<IGESData_IGESEntity>>, otherEnt, another->AttributeList(i, k));
          occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>> attrEnt = new NCollection_HArray1<occ::handle<IGESData_IGESEntity>>(1, avc);
          list2->SetValue(i, k, attrEnt);
          for (j = 1; j <= avc; j++)
            attrEnt->SetValue(j,
                              GetCasted(IGESData_IGESEntity, TC.Transferred(otherEnt->Value(j))));
        }
        break;
        case 5: /////	      list2->SetValue(i,k,NULL);    by default
          break;
        case 6: { // Here item takes value 0 or 1
          DeclareAndCast(NCollection_HArray1<int>, otherInt, another->AttributeList(i, k));
          occ::handle<NCollection_HArray1<int>> attrInt = new NCollection_HArray1<int>(1, avc);
          list2->SetValue(i, k, attrInt);
          for (j = 1; j <= avc; j++)
            attrInt->SetValue(j, otherInt->Value(j));
        }
        break;
        default:
          break;
      }
    }
  }
  ent->Init(list2);
}

IGESData_DirChecker IGESDefs_ToolAttributeTable::DirChecker(
  const occ::handle<IGESDefs_AttributeTable>& /* ent */) const
{
  IGESData_DirChecker DC(422, 0, 1);
  DC.Structure(IGESData_DefReference);
  DC.GraphicsIgnored();
  DC.BlankStatusIgnored();
  DC.HierarchyStatusIgnored();
  return DC;
}

void IGESDefs_ToolAttributeTable::OwnCheck(const occ::handle<IGESDefs_AttributeTable>& ent,
                                           const Interface_ShareTool&,
                                           occ::handle<Interface_Check>& ach) const
{
  if (ent->Definition().IsNull())
  {
    if (ent->HasStructure())
      ach->AddFail("Structure in Directory Entry is not an Attribute Definition Table");
    else
      ach->AddFail("No Attribute Definition defined");
  }
  if (ent->FormNumber() == 0 && ent->NbRows() != 1)
    ach->AddFail("Form 0 with several Rows");
  if (ent->NbAttributes() != ent->Definition()->NbAttributes())
    ach->AddFail("Mismatch between Definition (Structure) and Content");
}

void IGESDefs_ToolAttributeTable::OwnDump(const occ::handle<IGESDefs_AttributeTable>& ent,
                                          const IGESData_IGESDumper&             dumper,
                                          Standard_OStream&                      S,
                                          const int                 level) const
{
  S << "IGESDefs_AttributeTable\n";

  occ::handle<IGESDefs_AttributeDef> ab = ent->Definition();

  int na = ent->NbAttributes();
  int nr = ent->NbRows();
  if (ent->FormNumber() == 1)
    S << "Number of Rows (i.e. complete sets of Attributes) : " << nr << "\n";
  else
    S << "One set of Attributes\n";
  S << "Number of defined Attributes : " << na << "\n";
  if (level <= 4)
    S << " [ structure : see Structure in Directory Entry; content : level > 4 ]\n";
  else
    for (int k = 1; k <= nr; k++)
    {
      for (int i = 1; i <= na; i++)
      {
        int avc = ab->AttributeValueCount(i);
        S << "[At.no." << i << " Row:" << k << "]";
        switch (ab->AttributeValueDataType(i))
        {
          case 0:
            S << "  (Void) ";
            break;
          case 1:
            S << "  Integer";
            break;
          case 2:
            S << "  Real   ";
            break;
          case 3:
            S << "  String ";
            break;
          case 4:
            S << "  Entity ";
            break;
          case 5:
            S << " (Not used)";
            break;
          case 6:
            S << "  Logical";
            break;
          default:
            break;
        }
        S << " :";
        for (int j = 1; j <= avc; j++)
        {
          S << "  ";
          switch (ab->AttributeValueDataType(i))
          {
            case 1:
              S << ent->AttributeAsInteger(i, k, j);
              break;
            case 2:
              S << ent->AttributeAsReal(i, k, j);
              break;
            case 3:
              IGESData_DumpString(S, ent->AttributeAsString(i, k, j));
              break;
            case 4:
              dumper.Dump(ent->AttributeAsEntity(i, k, j), S, level - 5);
              break;
            case 6:
              S << (ent->AttributeAsLogical(i, k, j) ? "True" : "False");
              break;
            default:
              break;
          }
        }
        S << "\n";
      }
    }
  S << std::endl;
}
