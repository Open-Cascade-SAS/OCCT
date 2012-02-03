// File:	QADraw_Additional.cxx
// Created:	Tue Mar 12 17:39:57 2002
// Author:	QA Admin
//		<qa@umnox.nnov.matra-dtv.fr>


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
