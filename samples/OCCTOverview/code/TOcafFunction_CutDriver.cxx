// Copyright (c) 2020 OPEN CASCADE SAS
//
// This file is part of the examples of the Open CASCADE Technology software library.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE

#include "TOcafFunction_CutDriver.h"

#include <TNaming_NamedShape.hxx>
#include <TNaming_Builder.hxx>

#include <BRepAlgoAPI_Cut.hxx>
#include <Standard_GUID.hxx>
#include <TCollection_AsciiString.hxx>
#include <TDF_Tool.hxx>
#include <TDF_Reference.hxx>
#include <TFunction_Logbook.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QMessageBox>
#include <QApplication>
#include <QObject>
#include <Standard_WarningsRestore.hxx>

//=======================================================================
//function : GetID
//purpose  :
//=======================================================================
const Standard_GUID& TOcafFunction_CutDriver::GetID()
{
  static const Standard_GUID anID("22D22E52-D69A-11d4-8F1A-0060B0EE18E8");
  return anID;
}

//=======================================================================
//function : Validate
//purpose  :
//=======================================================================
void TOcafFunction_CutDriver::Validate (Handle(TFunction_Logbook)& log) const
{
  // We validate the object label ( Label() ), all the arguments and the results of the object:
  log->SetValid(Label(), Standard_True);
}

//=======================================================================
//function : MustExecute
//purpose  :
//=======================================================================
Standard_Boolean TOcafFunction_CutDriver::MustExecute(const Handle(TFunction_Logbook)& log) const
{
  // If the object's label is modified:
  if (log->IsModified(Label())) return Standard_True;

  // Cut (in our simple case) has two arguments: The original shape, and the tool shape.
  // They are on the child labels of the cut's label:
  // So, OriginalNShape  - is attached to the first  child label
  //     ToolNShape - is attached to the second child label,
  //     .
  // Let's check them:
  Handle(TDF_Reference) OriginalRef;
  //TDF_Label aLabel = Label().FindChild(1);
/*
  BOOL f = Label().IsNull();
  int a = Label().NbChildren();
*/
  TCollection_AsciiString aEntry;
  TDF_Tool::Entry(Label(), aEntry);
  std::cout << "Entry: " << aEntry.ToCString() << std::endl;
  Label().FindChild(1).FindAttribute(TDF_Reference::GetID(), OriginalRef);
  if (log->IsModified(OriginalRef->Get()))   return Standard_True; // Original shape.

  Handle(TDF_Reference) ToolRef;
  Label().FindChild(2).FindAttribute(TDF_Reference::GetID(), ToolRef);
  if (log->IsModified(ToolRef->Get()))   return Standard_True; // Tool shape.

  // if there are no any modifications concerned the cut,
  // it's not necessary to recompute (to call the method Execute()):
  return Standard_False;
}

//=======================================================================
//function : Execute
//purpose  :
//=======================================================================
Standard_Integer TOcafFunction_CutDriver::Execute(Handle(TFunction_Logbook)& /*log*/) const
{
  // Let's get the arguments (OriginalNShape, ToolNShape of the object):

  // First, we have to retrieve the TDF_Reference attributes to obtain
  // the root labels of the OriginalNShape and the ToolNShape:
  Handle(TDF_Reference)  OriginalRef, ToolRef;
  if (!Label().FindChild(1).FindAttribute(TDF_Reference::GetID(), OriginalRef))
  {
    return 1;
  }
  TDF_Label OriginalLab = OriginalRef->Get();
  if (!Label().FindChild(2).FindAttribute(TDF_Reference::GetID(), ToolRef))
  {
    return 1;
  }
  TDF_Label ToolLab = ToolRef->Get();

  // Get the TNaming_NamedShape attributes of these labels
  Handle(TNaming_NamedShape) OriginalNShape, ToolNShape;
  if (!(OriginalLab.FindAttribute(TNaming_NamedShape::GetID(), OriginalNShape)))
  {
    throw Standard_Failure("TOcaf_Commands::CutObjects");
  }
  if (!(ToolLab.FindAttribute(TNaming_NamedShape::GetID(), ToolNShape)))
  {
    throw Standard_Failure("TOcaf_Commands::CutObjects");
  }

  // Now, let's get the TopoDS_Shape of these TNaming_NamedShape:
  TopoDS_Shape OriginalShape = OriginalNShape->Get();
  TopoDS_Shape ToolShape = ToolNShape->Get();

  // STEP 2:
    // Let's call for algorithm computing a cut operation:
  BRepAlgoAPI_Cut mkCut(OriginalShape, ToolShape);
  // Let's check if the Cut has been successful:
  if (!mkCut.IsDone())
  {
    QMessageBox::critical(qApp->activeWindow(),
      QObject::tr("Cut Function Driver"),
      QObject::tr("Cut not done."));
    return 2;
  }
  TopoDS_Shape ResultShape = mkCut.Shape();

  // Build a TNaming_NamedShape using built cut
  TNaming_Builder B(Label());
  B.Modify(OriginalShape, ResultShape);
  // That's all:
    // If there are no any mistakes we return 0:
  return 0;
}
