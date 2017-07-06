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


#include <MoniTool_DataMapIteratorOfDataMapOfTimer.hxx>
#include <MoniTool_DataMapOfTimer.hxx>
#include <MoniTool_Timer.hxx>
#include <MoniTool_TimerSentry.hxx>
#include <OSD_Timer.hxx>
#include <Standard_Type.hxx>

#include <stdio.h>
IMPLEMENT_STANDARD_RTTIEXT(MoniTool_Timer,Standard_Transient)

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================
void MoniTool_Timer::Dump(Standard_OStream &ostr)
{
  Standard_Integer hours, minutes;
  Standard_Real seconds, CPUtime, user, system;
    
  myTimer.Show(seconds,minutes,hours,CPUtime);
  myTimer.OSD_Chronometer::Show(user,system);
      
  Standard_Real elapsed = seconds + minutes*60 + hours*3600;
  
  char buff[1024];
  Sprintf ( buff, "Elapsed:%6.1f sec, CPU User:%9.4f sec, CPU Sys:%9.4f sec, hits: %d",
                   elapsed, user, system, myCount );
    
  ostr << buff << endl;
}

//=======================================================================
//function : Dictionary
//purpose  : Return DataMapOfTimer
//=======================================================================

MoniTool_DataMapOfTimer& MoniTool_Timer::Dictionary () 
{
  static MoniTool_DataMapOfTimer dic;
  return dic;
}

//=======================================================================
//function : Timer
//purpose  : Return handle for timer from map
//=======================================================================

Handle(MoniTool_Timer) MoniTool_Timer::Timer(const Standard_CString name) 
{
//  AmendAccess();
  MoniTool_DataMapOfTimer &dic = Dictionary();
  if ( dic.IsBound(name) )
    return dic.Find(name);
  Handle(MoniTool_Timer) MT = new MoniTool_Timer;
  MT->Timer().Reset();
  dic.Bind(name,MT);
  return MT;
}

//=======================================================================
//function : ClearTimers
//purpose  : Clears all the map of timers
//=======================================================================

void MoniTool_Timer::ClearTimers() 
{
  Dictionary().Clear();
}

//=======================================================================
//function : DumpTimers
//purpose  : Shows all timer from Dictionary
//=======================================================================

void MoniTool_Timer::DumpTimers (Standard_OStream &ostr) 
{
  MoniTool_DataMapOfTimer &dic = Dictionary();
  MoniTool_DataMapIteratorOfDataMapOfTimer iter(dic);

  Standard_Integer NbTimers = dic.Extent();

  ostr << "DUMP OF TIMERS:" << endl;
  Standard_CString *keys = new Standard_CString[NbTimers];
  Standard_Integer i=0;
  for( ; iter.More() && i < NbTimers; iter.Next()) {
    keys[i++] = iter.Key();
  }
  for(i=0; i < NbTimers; i++) {
    Standard_Integer ntmp = 0;
    Standard_CString stmp = 0;
    for(Standard_Integer j=0; j < NbTimers; j++) {
      if ( keys[j] && ( ! stmp || strcmp(stmp,keys[j]) > 0 ) ) {
        ntmp=j;
        stmp=keys[j];
      }
    }
    //Handle(MoniTool_Timer) MT = iter.Value();    
    char buff[1024];
    Sprintf ( buff, "%-20s\t", stmp );
    ostr << "TIMER: " << buff;
    //iter.Value()->Dump ( ostr );
    Timer(stmp)->Dump(ostr);
    keys[ntmp]=0;
    if ( Timer(stmp)->IsRunning() ) cerr << "Warning: timer " << stmp << " is running" << endl;
  }
  delete[] keys;
}

//=======================================================================
//function : ComputeAmendments
//purpose  : 
//=======================================================================

static Standard_Real amAccess=0., amInternal=0., amExternal=0., amError=0.;

void MoniTool_Timer::ComputeAmendments ()
{
  const Standard_Integer NBTESTS = 100000;

  Standard_Integer i;

  Handle(MoniTool_Timer) MT0 = MoniTool_Timer::Timer("_mt_amend_0_");
  Handle(MoniTool_Timer) MT1 = MoniTool_Timer::Timer("_mt_amend_1_");
  Handle(MoniTool_Timer) MT2 = MoniTool_Timer::Timer("_mt_amend_2_");
  Handle(MoniTool_Timer) MT3 = MoniTool_Timer::Timer("_mt_amend_3_");
  MT0->Reset();
  MT1->Reset();
  MT2->Reset();
  MT3->Reset();
  MoniTool_Timer::Timer("_mt_amend_t1_")->Reset();
  MoniTool_Timer::Timer("_mt_amend_t2_")->Reset();
  MoniTool_Timer::Timer("_mt_amend_t3_")->Reset();
  
  // reference test
  MT0->Start();
  for ( i=1; i <= NBTESTS; i++ ) {
    for ( int k=1; k <= 100; k++ ) Sqrt ( i+k );
  }
  MT0->Stop();
  
  // test for direct access
  Handle(MoniTool_Timer) MT = MoniTool_Timer::Timer("_mt_amend_t1_");
  MT1->Start();
  for ( i=1; i <= NBTESTS; i++ ) {
    MT->Start();
    for ( int k=1; k <= 100; k++ ) Sqrt ( i+k );
    MT->Stop();
  }
  MT1->Stop();

  // test for using Sentry
  MT2->Start();
  for ( i=1; i <= NBTESTS; i++ ) {
    MoniTool_TimerSentry TS ("_mt_amend_t2_");
    for ( int k=1; k <= 100; k++ ) Sqrt ( i+k );
  }
  MT2->Stop();

  // test for access by name
  MT3->Start();
  for ( i=1; i <= NBTESTS; i++ ) {
    MoniTool_Timer::Start("_mt_amend_t3_");
    for ( int k=1; k <= 100; k++ ) Sqrt ( i+k );
    MoniTool_Timer::Stop("_mt_amend_t3_");
  }
  MT3->Stop();
  
  // analyze results
  Standard_Real cpu0, cpu1, cpu2, cpu3, cput1, cput2, cput3;
  cpu0 = MoniTool_Timer::Timer("_mt_amend_0_")->CPU();
  cpu1 = MoniTool_Timer::Timer("_mt_amend_1_")->CPU();
  cput1 = MT->CPU();
  cpu2 = MoniTool_Timer::Timer("_mt_amend_2_")->CPU();
  cput2 = MoniTool_Timer::Timer("_mt_amend_t2_")->CPU();
  cpu3 = MoniTool_Timer::Timer("_mt_amend_3_")->CPU();
  cput3 = MoniTool_Timer::Timer("_mt_amend_t3_")->CPU();
  
  amExternal += ( cpu1  - cpu0 ) / NBTESTS;
  amInternal += ( cput1 - cpu0 ) / NBTESTS;
  amAccess   += ( 0.5 * ( cpu3 - cpu1 ) ) / NBTESTS;
  amError     = Abs ( cpu1 + cpu3 - 2 * cpu2 ) / NBTESTS;
  
  cout << "CPU 0: " << cpu0 << endl;
  cout << "CPU 1: " << cpu1 << " INTERNAL: " << cput1 << endl;
  cout << "CPU 2: " << cpu2 << " INTERNAL: " << cput2 << endl;
  cout << "CPU 3: " << cpu3 << " INTERNAL: " << cput3 << endl;
  cout << "Access: " << amAccess << ", External: " << amExternal << 
          ", Internal: " << amInternal << ", Error: " << amError << endl;
  
}
  
//=======================================================================
//function : GetAmendments
//purpose  : 
//=======================================================================

void MoniTool_Timer::GetAmendments (Standard_Real &access, 
                                    Standard_Real &internal, 
                                    Standard_Real &external, 
                                    Standard_Real &error10)
{
  access   = amAccess;
  internal = amInternal;
  external = amExternal;
  error10  = amError;
}

//=======================================================================
//function : AmendAccess
//purpose  : 
//=======================================================================

static Handle(MoniTool_Timer) myActive;
     
void MoniTool_Timer::AmendAccess ()
{
  Standard_Real amend = amAccess;
  for ( Handle(MoniTool_Timer) act = myActive; ! act.IsNull(); act = act->myNext )
    act->myAmend += amend;
}

void MoniTool_Timer::AmendStart ()
{
  Standard_Real amend = amExternal;
  for ( Handle(MoniTool_Timer) act = myActive; ! act.IsNull(); act = act->myNext )
    act->myAmend += amend;
  myAmend += amInternal;

  // add to list
  if ( ! myActive.IsNull() ) {
    myActive->myPrev = this;
    myNext = myActive;
  }
  myActive = this;
}

void MoniTool_Timer::AmendStop ()
{ 
  Handle (MoniTool_Timer) thisActive(this);
  if ( myActive == thisActive )  myActive = myNext;
//    if ( myActive == this )  myActive = myNext;
 
  if ( ! myPrev.IsNull() ) myPrev->myNext = myNext;
  if ( ! myNext.IsNull() ) myNext->myPrev = myPrev;
 
  myNext = myPrev = 0;
}
