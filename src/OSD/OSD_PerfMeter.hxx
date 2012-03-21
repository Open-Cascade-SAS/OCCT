// Created on: 2002-04-03
// Created by: Michael SAZONOV
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


#ifndef OSD_PerfMeter_HeaderFile
#define OSD_PerfMeter_HeaderFile

#include <OSD_PerfMeter.h>

//  This  class enables measuring  the CPU  time between  two points  of code
//  execution,  regardless of the scope of these points of code.   A meter is
//  identified by its name (string  of chars). So multiple objects in various
//  places of  user code may  point to the  same meter.  The results  will be
//  printed  on  stdout  upon  finish   of  the  program.   For  details  see
//  OSD_PerfMeter.h

class OSD_PerfMeter {

 public:
  // ---------- PUBLIC METHODS ----------

  OSD_PerfMeter () : myIMeter(-1) {}
  // constructs a void meter (to further call Init and Start)

  OSD_PerfMeter (const char* meter, const unsigned autoStart = 1)
    : myIMeter(perf_get_meter(meter,0,0)) {
      if (myIMeter < 0) myIMeter = perf_init_meter(meter);
      if (autoStart) Start();
    }
  // constructs and starts (if autoStart is true) the named meter

  void Init (const char* meter) {
    myIMeter = perf_get_meter(meter,0,0);
    if (myIMeter < 0) myIMeter = perf_init_meter(meter);
  }
  // prepares the named meter

  void Start    () const { perf_start_imeter(myIMeter); }
  // starts the meter

  void Stop     () const { perf_stop_imeter(myIMeter); }
  // stops the meter

  void Tick     () const { perf_tick_imeter(myIMeter); }
  // increments the counter w/o time measurement

  void Flush    () const { perf_close_imeter(myIMeter); }
  // outputs the meter data and resets it to initial state

  virtual ~OSD_PerfMeter () { if (myIMeter >= 0) Stop(); }
  // assures stopping upon destruction

 protected:
  // ---------- PROTECTED FIELDS ----------

  int myIMeter;
};

#endif
