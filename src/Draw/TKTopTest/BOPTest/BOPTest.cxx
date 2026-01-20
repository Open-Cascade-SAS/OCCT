// Created on: 2000-05-18
// Created by: Peter KURNEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#include <BOPTest.hxx>
#include <BRepTest.hxx>
#include <DBRep.hxx>
#include <Draw.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_PluginMacro.hxx>
#include <GeometryTest.hxx>
#include <GeomliteTest.hxx>
#include <HLRTest.hxx>
#include <NCollection_Map.hxx>
#include <MeshTest.hxx>
#include <Message_Msg.hxx>
#include <SWDRAW.hxx>

#include <BOPAlgo_Alerts.hxx>
#include <BOPTest_Objects.hxx>

//=================================================================================================

void BOPTest::AllCommands(Draw_Interpretor& theCommands)
{
  static bool done = false;
  if (done)
    return;
  done = true;
  //
  BOPTest::BOPCommands(theCommands);
  BOPTest::CheckCommands(theCommands);
  BOPTest::LowCommands(theCommands);
  BOPTest::TolerCommands(theCommands);
  BOPTest::ObjCommands(theCommands);
  BOPTest::PartitionCommands(theCommands);
  BOPTest::APICommands(theCommands);
  BOPTest::OptionCommands(theCommands);
  BOPTest::DebugCommands(theCommands);
  BOPTest::CellsCommands(theCommands);
  BOPTest::UtilityCommands(theCommands);
  BOPTest::RemoveFeaturesCommands(theCommands);
  BOPTest::PeriodicityCommands(theCommands);
  BOPTest::MkConnectedCommands(theCommands);
}

//=================================================================================================

void BOPTest::Factory(Draw_Interpretor& theCommands)
{
  static bool FactoryDone = false;
  if (FactoryDone)
    return;

  FactoryDone = true;

  DBRep::BasicCommands(theCommands);
  GeomliteTest::AllCommands(theCommands);
  GeometryTest::AllCommands(theCommands);
  BRepTest::AllCommands(theCommands);
  MeshTest::Commands(theCommands);
  HLRTest::Commands(theCommands);
  BOPTest::AllCommands(theCommands);
  SWDRAW::Init(theCommands);
}
// Declare entry point PLUGINFACTORY
DPLUGIN(BOPTest)

//=================================================================================================

void BOPTest::ReportAlerts(const occ::handle<Message_Report>& theReport)
{
  // first report warnings, then errors
  Message_Gravity            anAlertTypes[2] = {Message_Warning, Message_Fail};
  TCollection_ExtendedString aMsgType[2]     = {"Warning: ", "Error: "};
  for (int iGravity = 0; iGravity < 2; iGravity++)
  {
    // report shapes for the same type of alert together
    NCollection_Map<occ::handle<Standard_Transient>> aPassedTypes;
    const NCollection_List<occ::handle<Message_Alert>>& aList = theReport->GetAlerts(anAlertTypes[iGravity]);
    for (NCollection_List<occ::handle<Message_Alert>>::Iterator aIt(aList); aIt.More(); aIt.Next())
    {
      // check that this type of warnings has not yet been processed
      const occ::handle<Standard_Type>& aType = aIt.Value()->DynamicType();
      if (!aPassedTypes.Add(aType))
        continue;

      // get alert message
      Message_Msg                aMsg(aIt.Value()->GetMessageKey());
      TCollection_ExtendedString aText = aMsgType[iGravity] + aMsg.Get();

      // collect all shapes if any attached to this alert
      if (BOPTest_Objects::DrawWarnShapes())
      {
        TCollection_AsciiString aShapeList;
        int        aNbShapes = 0;
        for (NCollection_List<occ::handle<Message_Alert>>::Iterator aIt2(aIt); aIt2.More(); aIt2.Next())
        {
          occ::handle<TopoDS_AlertWithShape> aShapeAlert =
            occ::down_cast<TopoDS_AlertWithShape>(aIt2.Value());

          if (!aShapeAlert.IsNull() && (aType == aShapeAlert->DynamicType())
              && !aShapeAlert->GetShape().IsNull())
          {
            //
            char aName[80];
            Sprintf(aName, "%ss_%d_%d", (iGravity ? "e" : "w"), aPassedTypes.Extent(), ++aNbShapes);
            DBRep::Set(aName, aShapeAlert->GetShape());
            //
            aShapeList += " ";
            aShapeList += aName;
          }
        }
        aText += (aNbShapes ? ": " : "(no shapes attached)");
        aText += aShapeList;
      }

      // output message with list of shapes
      Draw_Interpretor& aDrawInterpretor = Draw::GetInterpretor();
      aDrawInterpretor << aText << "\n";
    }
  }
}

//=================================================================================================

BOPAlgo_Operation BOPTest::GetOperationType(const char* const theOp)
{
  TCollection_AsciiString anOp(theOp);
  anOp.LowerCase();

  if (anOp.IsIntegerValue())
  {
    // Check if the given value satisfies the enumeration.
    int iOp = anOp.IntegerValue();
    if (iOp >= 0 && iOp <= 4)
    {
      return static_cast<BOPAlgo_Operation>(iOp);
    }
    return BOPAlgo_UNKNOWN;
  }

  // Check for the meaningful symbolic operation parameter
  if (anOp == "common")
  {
    return BOPAlgo_COMMON;
  }
  else if (anOp == "fuse")
  {
    return BOPAlgo_FUSE;
  }
  else if (anOp == "cut")
  {
    return BOPAlgo_CUT;
  }
  else if (anOp == "tuc" || anOp == "cut21")
  {
    return BOPAlgo_CUT21;
  }
  else if (anOp == "section")
  {
    return BOPAlgo_SECTION;
  }

  return BOPAlgo_UNKNOWN;
}
