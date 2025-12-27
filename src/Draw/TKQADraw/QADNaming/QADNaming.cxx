// Created on: 1997-01-09
// Created by: VAUTHIER Jean-Claude & Fricaud Yves
// Copyright (c) 1997-1999 Matra Datavision
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

#include <BRep_Tool.hxx>
#include <DBRep.hxx>
#include <DDF.hxx>
#include <gp_Pnt.hxx>
#include <QADNaming.hxx>
#include <TCollection_AsciiString.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_Data.hxx>
#include <TDF_Label.hxx>
#include <TDF_Tool.hxx>
#include <TNaming_Iterator.hxx>
#include <TNaming_Tool.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>

#include <stdio.h>

//=================================================================================================

void QADNaming::GetShape(const char*  LabelName,
                         const occ::handle<TDF_Data>& DF,
                         NCollection_List<TopoDS_Shape>&   L)
{
  L.Clear();
  TDF_Label        Label;
  bool Found = DDF::AddLabel(DF, LabelName, Label);
  if (Found)
  {
    TNaming_Iterator it(Label, DF->Transaction());
    for (; it.More(); it.Next())
    {
      L.Append(it.NewShape());
    }
  }
}

//=================================================================================================

void QADNaming_BuildMap(NCollection_Map<TDF_Label>& Updated, const TDF_Label& Lab)
{
  TDF_ChildIterator it(Lab);
  for (; it.More(); it.Next())
  {
    Updated.Add(it.Value());
    QADNaming_BuildMap(Updated, it.Value());
  }
}

//=================================================================================================

TopoDS_Shape QADNaming::CurrentShape(const char* LabelName, const occ::handle<TDF_Data>& DF)
{
  TopoDS_Shape     S;
  TDF_Label        Label;
  bool Found = DDF::AddLabel(DF, LabelName, Label);
  if (!Found)
  {
    std::cout << "no labels" << std::endl;
    return S;
  }
  if (Found)
  {
    occ::handle<TNaming_NamedShape> NS;
    Label.FindAttribute(TNaming_NamedShape::GetID(), NS);
    S = TNaming_Tool::CurrentShape(NS);
    if (S.IsNull())
      std::cout << "current shape from " << LabelName << " is deleted" << std::endl;
    return S;
  }
  return S;
}

//=================================================================================================

TCollection_AsciiString QADNaming::GetEntry(const TopoDS_Shape&     Shape,
                                            const occ::handle<TDF_Data>& DF,
                                            int&       theStatus)
{
  theStatus = 0;
  // occ::handle<TNaming_UsedShapes> US;
  // DF->Root().FindAttribute(TNaming_UsedShapes::GetID(),US);

  if (!TNaming_Tool::HasLabel(DF->Root(), Shape))
  {
    return TCollection_AsciiString();
  }
  int        Transdef;
  TDF_Label               Lab = TNaming_Tool::Label(DF->Root(), Shape, Transdef);
  TCollection_AsciiString entry;
  TDF_Tool::Entry(Lab, entry);
  // Update theStatus;
  TNaming_Iterator it(Lab, DF->Transaction());
  for (; it.More(); it.Next())
  {
    theStatus++;
    if (theStatus == 2)
      break;
  }
  return entry;
}

//=================================================================================================

bool QADNaming::Entry(void* const theArguments, TDF_Label& theLabel)
{
  const char**     arg = (const char**)theArguments;
  occ::handle<TDF_Data> DF;
  if (!DDF::GetDF(arg[1], DF))
  {
    std::cout << "Wrong df" << std::endl;
    return false;
  }
  DDF::AddLabel(DF, arg[2], theLabel);
  if (!theLabel.IsNull())
    return true;
  std::cout << "Wrong entry" << std::endl;
  return false;
}

//=======================================================================
// function : IsSameShapes
// purpose  : IsSameShapes DrawShape1 DrawShape2
//=======================================================================
static int QADNaming_IsSameShapes(Draw_Interpretor& di,
                                               int  nb,
                                               const char**      arg)
{
  if (nb == 3)
  {
    TopoDS_Shape aShape1 = DBRep::Get(arg[1]);
    TopoDS_Shape aShape2 = DBRep::Get(arg[2]);
    if (aShape1.IsNull() || aShape2.IsNull())
      return 0;
    if (aShape1.IsSame(aShape2))
      di << "1";
    return 0;
  }
  return 1;
}

//=======================================================================
// function : CenterOfShape
// purpose  : CenterOfShape DrawShape
//=======================================================================
static int QADNaming_CenterOfShape(Draw_Interpretor& di,
                                                int  nb,
                                                const char**      arg)
{
  if (nb == 2)
  {
    TopoDS_Shape aShape = DBRep::Get(arg[1]);
    if (aShape.IsNull())
      return 1;
    double   x = 0, y = 0, z = 0;
    float           all = 0;
    TopExp_Explorer anExp(aShape, TopAbs_VERTEX);
    for (; anExp.More(); anExp.Next(), all++)
    {
      TopoDS_Vertex aVertex = TopoDS::Vertex(anExp.Current());
      gp_Pnt        aPoint  = BRep_Tool::Pnt(aVertex);
      x += aPoint.X();
      y += aPoint.Y();
      z += aPoint.Z();
    }
    if (all != 0)
    {
      char s[100];
      Sprintf(s, "%f %f %f", (float)x / all, (float)y / all, (float)z / all);
      di << s;
      return 0;
    }
  }
  return 1;
}

//=================================================================================================

void QADNaming::AllCommands(Draw_Interpretor& theCommands)
{
  static bool done = false;
  if (done)
    return;
  done = true;

  QADNaming::BasicCommands(theCommands);
  QADNaming::BuilderCommands(theCommands);
  QADNaming::IteratorsCommands(theCommands);
  QADNaming::ToolsCommands(theCommands);
  QADNaming::SelectionCommands(theCommands);

  // auxiliary command IsSameShapes
  theCommands.Add("IsSameShapes",
                  "IsSameShapes DrawShape1 DrawShape2",
                  __FILE__,
                  QADNaming_IsSameShapes,
                  "Auxiliary methods");
  theCommands.Add("CenterOfShape",
                  "CenterOfShape DrawShape",
                  __FILE__,
                  QADNaming_CenterOfShape,
                  "Auxiliary methods");
  // define the TCL variable Draw_NamingData
  const char* com = "set Draw_NamingData 1";
  theCommands.Eval(com);
}
