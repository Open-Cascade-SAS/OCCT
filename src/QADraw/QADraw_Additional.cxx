// Created on: 2002-03-12
// Created by: QA Admin
// Copyright (c) 2002-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



#include <QABugs.hxx>
#include <QADraw.hxx>
#include <QADNaming.hxx>
//#if defined(WOKC40)
//#include <QADBRepNaming.hxx>
//#else
#include <QANewDBRepNaming.hxx>
//#endif
#include <QANCollection.hxx>

void QADraw::AdditionalCommands(Draw_Interpretor& theCommands)
{
  QABugs::Commands(theCommands);

  QADNaming::AllCommands(theCommands);
//#if defined(WOKC40)
//  QADBRepNaming::AllCommands(theCommands);
//#else
  QANewDBRepNaming::AllCommands(theCommands);
//#endif
  QANCollection::Commands(theCommands);

  return;
}
