// Timer.h: interface for the Timer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIMER_H__528FB454_797E_11D7_9B1B_000103C0F1F9__INCLUDED_)
#define AFX_TIMER_H__528FB454_797E_11D7_9B1B_000103C0F1F9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <OSD_Timer.hxx>

class Timer  
{
public:
	Standard_EXPORT Timer();
	Standard_EXPORT Timer(const char* filename);
	Standard_EXPORT ~Timer();
	Standard_EXPORT void Start();
	Standard_EXPORT void Stop();
	Standard_EXPORT void Continue();
	Standard_EXPORT void Reset();
  Standard_EXPORT float Seconds();
  Standard_EXPORT int Minutes();
	Standard_EXPORT void Print(char* label);

private:
	OSD_Timer myTimer;
	FILE* myWriter;
};

#endif // !defined(AFX_TIMER_H__528FB454_797E_11D7_9B1B_000103C0F1F9__INCLUDED_)
