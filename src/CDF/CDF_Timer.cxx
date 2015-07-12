// Created on: 1998-07-17
// Created by: Jean-Louis Frenkel
// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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


#include <CDF_Timer.hxx>

#include <stdlib.h>
CDF_Timer::CDF_Timer() {
  myTimer.Start();
}


void CDF_Timer::ShowAndRestart(const Standard_CString aMessage) {
  if(MustShow()) {
    Show(aMessage);
    myTimer.Reset();
    myTimer.Start();
  }
}
  
void CDF_Timer::ShowAndStop(const Standard_CString aMessage) {
  if(MustShow()) {
    Show(aMessage);
    myTimer.Stop();
  }
}
 
void CDF_Timer::Show(const Standard_CString aMessage) {
  Standard_Integer minutes,hours; Standard_Real seconds,CPUtime;
  myTimer.Show(seconds,minutes,hours,CPUtime);
  cout << aMessage << hours << "h " << minutes << "' " << seconds << "'' (cpu: " << CPUtime << ")" << endl;
}
Standard_Boolean CDF_Timer::MustShow() {
  static Standard_Boolean theMustShow=getenv("STORETIMER") != NULL;
  return theMustShow;
}
