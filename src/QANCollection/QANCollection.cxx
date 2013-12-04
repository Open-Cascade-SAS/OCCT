// Created on: 2004-03-05
// Created by: Mikhail KUZMITCHEV
// Copyright (c) 2004-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <QANCollection.hxx>
#include <Draw_Interpretor.hxx>

#include <gp_Pnt.hxx>
//#include <QANCollection_Common.hxx>

void QANCollection::Commands(Draw_Interpretor& theCommands) {
  QANCollection::Commands1(theCommands);
  QANCollection::Commands2(theCommands);
  QANCollection::Commands3(theCommands);
  QANCollection::Commands4(theCommands);
  return;
}
