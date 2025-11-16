// Copyright (c) 2025 OPEN CASCADE SAS
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

#include <BRepPrimAPI_MakeBox.hxx>
#include <Standard_GUID.hxx>
#include <TDataStd_Name.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>
#include <XCAFDoc.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>

#include <gtest/gtest.h>

TEST(XCAFDoc_Test, OCC738_ShapeRefGUID)
{
  // Bug OCC738: The GUIDs in XCAFDoc have incorrect format
  // This test verifies that XCAFDoc::ShapeRefGUID() returns a valid GUID

  Standard_GUID aGUID;
  EXPECT_NO_THROW(aGUID = XCAFDoc::ShapeRefGUID());

  // Verify the GUID is not null (all zeros)
  Standard_GUID aNullGUID("00000000-0000-0000-0000-000000000000");
  EXPECT_NE(aGUID, aNullGUID) << "ShapeRefGUID should not be null";
}

TEST(XCAFDoc_Test, OCC738_AssemblyGUID)
{
  // Bug OCC738: The GUIDs in XCAFDoc have incorrect format
  // This test verifies that XCAFDoc::AssemblyGUID() returns a valid GUID

  Standard_GUID aGUID;
  EXPECT_NO_THROW(aGUID = XCAFDoc::AssemblyGUID());

  // Verify the GUID is not null (all zeros)
  Standard_GUID aNullGUID("00000000-0000-0000-0000-000000000000");
  EXPECT_NE(aGUID, aNullGUID) << "AssemblyGUID should not be null";
}

// RAII guard to save and restore XCAFDoc_ShapeTool AutoNaming state
class AutoNamingGuard
{
public:
  AutoNamingGuard()
      : mySavedValue(XCAFDoc_ShapeTool::AutoNaming())
  {
  }

  ~AutoNamingGuard() { XCAFDoc_ShapeTool::SetAutoNaming(mySavedValue); }

  // Disable copy and move
  AutoNamingGuard(const AutoNamingGuard&)            = delete;
  AutoNamingGuard& operator=(const AutoNamingGuard&) = delete;

private:
  Standard_Boolean mySavedValue;
};

TEST(XCAFDoc_Test, OCC23595_AutoNaming)
{
  // Bug OCC23595: XCAFDoc_ShapeTool extended with two methods -
  // SetAutoNaming() and AutoNaming()
  // This test verifies that the AutoNaming feature works correctly

  // RAII guard to automatically restore AutoNaming state on exit (exception-safe)
  AutoNamingGuard aGuard;

  // Create a new XmlXCAF document
  Handle(TDocStd_Application) anApp = new TDocStd_Application();
  Handle(TDocStd_Document)    aDoc;
  anApp->NewDocument("XmlXCAF", aDoc);
  ASSERT_FALSE(aDoc.IsNull());

  Handle(XCAFDoc_ShapeTool) aShTool = XCAFDoc_DocumentTool::ShapeTool(aDoc->Main());

  // Check default value of AutoNaming
  Standard_Boolean aValue = XCAFDoc_ShapeTool::AutoNaming();
  EXPECT_TRUE(aValue) << "AutoNaming should be true by default";

  // Test with AutoNaming enabled (true)
  XCAFDoc_ShapeTool::SetAutoNaming(Standard_True);
  TopoDS_Shape          aShape1 = BRepPrimAPI_MakeBox(100., 200., 300.).Shape();
  TDF_Label             aLabel1 = aShTool->AddShape(aShape1);
  Handle(TDataStd_Name) anAttr;
  EXPECT_TRUE(aLabel1.FindAttribute(TDataStd_Name::GetID(), anAttr))
    << "Shape should have a name attribute when AutoNaming is true";

  // Test with AutoNaming disabled (false)
  XCAFDoc_ShapeTool::SetAutoNaming(Standard_False);
  TopoDS_Shape aShape2 = BRepPrimAPI_MakeBox(300., 200., 100.).Shape();
  TDF_Label    aLabel2 = aShTool->AddShape(aShape2);
  EXPECT_FALSE(aLabel2.FindAttribute(TDataStd_Name::GetID(), anAttr))
    << "Shape should not have a name attribute when AutoNaming is false";

  // AutoNaming state will be automatically restored by the RAII guard
}
