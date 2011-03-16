// File:	QANCollection.cxx
// Created:	Fri Mar  5 09:08:10 2004
// Author:	Mikhail KUZMITCHEV
//		<mkv@russox>


#include <QANCollection.hxx>
#include <Draw_Interpretor.hxx>

#include <gp_Pnt.hxx>
//#include <QANCollection_Common.hxx>

void QANCollection::Commands(Draw_Interpretor& theCommands) {
  QANCollection::Commands1(theCommands);
  QANCollection::Commands2(theCommands);
  QANCollection::Commands3(theCommands);
  return;
}
