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

#include "TOcaf_Application.h"

#include <TDF_Label.hxx>
#include <TFunction_DriverTable.hxx>
#include <TNaming_NamedShape.hxx>
#include <TPrsStd_AISPresentation.hxx>
#include <TPrsStd_AISViewer.hxx>

#include "TOcafFunction_BoxDriver.h"
#include "TOcafFunction_CylDriver.h"
#include "TOcafFunction_CutDriver.h"

//=======================================================================
//function : TOcaf_Application
//purpose  :
//=======================================================================
TOcaf_Application::TOcaf_Application()
{
  // Instantiate a TOcafFunction_BoxDriver and add it to the TFunction_DriverTable
  TFunction_DriverTable::Get()->AddDriver (TOcafFunction_BoxDriver::GetID(),
                                           new TOcafFunction_BoxDriver());

  // Instantiate a TOcafFunction_Cyl  Driver and add it to the TFunction_DriverTable
  TFunction_DriverTable::Get()->AddDriver (TOcafFunction_CylDriver::GetID(),
                                           new TOcafFunction_CylDriver());

  // Instantiate a TOcafFunction_CutDriver and add it to the TFunction_DriverTable
  Handle(TOcafFunction_CutDriver) myCutDriver = new TOcafFunction_CutDriver();
  TFunction_DriverTable::Get()->AddDriver (TOcafFunction_CutDriver::GetID(),
                                           new TOcafFunction_CutDriver());
}
