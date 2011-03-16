// File:	CDF_Timer.cxx
// Created:	Fri Jul 17 08:08:27 1998
// Author:	Jean-Louis Frenkel
//		<rmi@frilox.paris1.matra-dtv.fr>


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
