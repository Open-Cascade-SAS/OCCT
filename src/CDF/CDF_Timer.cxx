// Created on: 1998-07-17
// Created by: Jean-Louis Frenkel
// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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



#include <CDF_Timer.ixx>
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
