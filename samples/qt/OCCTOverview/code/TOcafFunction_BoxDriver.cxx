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

#include "TOcafFunction_BoxDriver.h"

#include <BRepPrimAPI_MakeBox.hxx>
#include <Standard_GUID.hxx>
#include <TDataStd_Real.hxx>
#include <TNaming_Builder.hxx>

//=======================================================================
//function : GetID
//purpose  :
//=======================================================================
const Standard_GUID& TOcafFunction_BoxDriver::GetID()
{
  static const Standard_GUID anID("22D22E51-D69A-11d4-8F1A-0060B0EE18E8");
  return anID;
}

//=======================================================================
//function : Validate
//purpose  :
//=======================================================================
void TOcafFunction_BoxDriver::Validate(Handle(TFunction_Logbook)& log) const
{
  // We validate the object label ( Label() ), all the arguments and the results of the object:
  log->SetValid(Label(), Standard_True);
}

//=======================================================================
//function : MustExecute
//purpose  :
//=======================================================================
Standard_Boolean TOcafFunction_BoxDriver::MustExecute(const Handle(TFunction_Logbook)& log) const
{
  // If the object's label is modified:
  if (log->IsModified(Label())) return Standard_True;

  // Cut (in our simple case) has two arguments: The original shape, and the tool shape.
  // They are on the child labels of the box's label:
  // So, OriginalNShape  - is attached to the first  child label
  //     ToolNShape - is attached to the second child label.
  //
  // Let's check them:
  if (log->IsModified(Label().FindChild(1)))
  {
    return Standard_True; // width.
  }
  if (log->IsModified(Label().FindChild(2)))
  {
    return Standard_True; // length,
  }
  if (log->IsModified(Label().FindChild(3)))
  {
    return Standard_True; // width.
  }
  if (log->IsModified(Label().FindChild(4)))
  {
    return Standard_True; // length,
  }
  if (log->IsModified(Label().FindChild(5)))
  {
    return Standard_True; // width.
  }
  if (log->IsModified(Label().FindChild(6)))
  {
    return Standard_True; // length,
  }
  // if there are no any modifications concerned the box,
  // it's not necessary to recompute (to call the method Execute()):
  return Standard_False;
}

//=======================================================================
//function : Execute
//purpose  :
//=======================================================================
Standard_Integer TOcafFunction_BoxDriver::Execute(Handle(TFunction_Logbook)& /*log*/) const
{
  // Get the values of dimension and position attributes
  Handle(TDataStd_Real) TSR;
  Standard_Real x, y, z, l, h, w;
  if (!Label().FindChild(1).FindAttribute(TDataStd_Real::GetID(), TSR))
  {
    return 1;
  }
  l = TSR->Get();

  if (!Label().FindChild(2).FindAttribute(TDataStd_Real::GetID(), TSR))
  {
    return 1;
  }
  h = TSR->Get();

  if (!Label().FindChild(3).FindAttribute(TDataStd_Real::GetID(), TSR))
  {
    return 1;
  }
  w = TSR->Get();

  if (!Label().FindChild(4).FindAttribute(TDataStd_Real::GetID(), TSR))
  {
    return 1;
  }
  x = TSR->Get();

  if (!Label().FindChild(5).FindAttribute(TDataStd_Real::GetID(), TSR))
  {
    return 1;
  }
  y = TSR->Get();

  if (!Label().FindChild(6).FindAttribute(TDataStd_Real::GetID(), TSR))
  {
    return 1;
  }
  z = TSR->Get();

  // Build a box using the dimension and position attributes
  BRepPrimAPI_MakeBox mkBox(gp_Pnt(x, y, z), l, h, w);
  TopoDS_Shape ResultShape = mkBox.Shape();

  // Build a TNaming_NamedShape using built box
  TNaming_Builder B(Label());
  B.Generated(ResultShape);
  // That's all:
  // If there are no any mistakes we return 0:
  return 0;
}
