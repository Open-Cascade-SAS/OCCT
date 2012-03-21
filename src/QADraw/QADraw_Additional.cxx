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



#include <QADraw.hxx>

#include <QABRGM.hxx>
#include <QACADCAM.hxx>
#include <QAMitutoyoUK.hxx>
#include <QAMitutoyoUS.hxx>
#include <QAOCC.hxx>
#include <QARina.hxx>
#include <QARoutelous.hxx>
#include <QASamtech.hxx>
#include <QATelco.hxx>
#include <QAUsinor.hxx>

#include <QADNaming.hxx>
//#if defined(WOKC40)
//#include <QADBRepNaming.hxx>
//#else
#include <QANewDBRepNaming.hxx>
//#endif

#include <QARicardo.hxx>
#include <QAYasaki.hxx>
#include <QAAlcatel.hxx>
#include <QAInsynchro.hxx>
#include <QANIC.hxx>
#include <QABUC.hxx>
#include <QAAMINO.hxx>
#include <QAMARTEC.hxx>
#include <QAQuickPen.hxx>
#if defined(WOKC40)
#include <QAViewer2dTest.hxx>
#endif
#include <QANCollection.hxx>
#include <QACEADRT.hxx>

void QADraw::AdditionalCommands(Draw_Interpretor& theCommands)
{
  QABRGM::Commands(theCommands);
  QACADCAM::Commands(theCommands);
  QAMitutoyoUK::Commands(theCommands);
  QAMitutoyoUS::Commands(theCommands);
  QAOCC::Commands(theCommands);
  QARina::Commands(theCommands);
  QARoutelous::Commands(theCommands);
  QASamtech::Commands(theCommands);
  QATelco::Commands(theCommands);
  QAUsinor::Commands(theCommands);

  QADNaming::AllCommands(theCommands);
//#if defined(WOKC40)
//  QADBRepNaming::AllCommands(theCommands);
//#else
  QANewDBRepNaming::AllCommands(theCommands);
//#endif

  QARicardo::Commands(theCommands);
  QAYasaki::Commands(theCommands);
  QAAlcatel::Commands(theCommands);
  QAInsynchro::Commands(theCommands);
  QANIC::Commands(theCommands);
  QABUC::Commands(theCommands);
  QAAMINO::Commands(theCommands);
  QAMARTEC::Commands(theCommands);
  QAQuickPen::Commands(theCommands);
#if defined(WOKC40)
  QAViewer2dTest::Commands(theCommands);
#endif
  QANCollection::Commands(theCommands);
  QACEADRT::Commands(theCommands);

  return;
}
