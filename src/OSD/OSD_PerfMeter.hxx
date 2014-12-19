// Created on: 2002-04-03
// Created by: Michael SAZONOV
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#ifndef OSD_PerfMeter_HeaderFile
#define OSD_PerfMeter_HeaderFile

#include <OSD_PerfMeter.h>

//! This class enables measuring the CPU time between two points of code execution, regardless of the scope of these points of code.
//! A meter is identified by its name (string). So multiple objects in various places of user code may point to the same meter.
//! The results will be printed on stdout upon finish of the program.
//! For details see OSD_PerfMeter.h
class OSD_PerfMeter
{

public:

  //! Constructs a void meter (to further call Init and Start)
  OSD_PerfMeter() : myIMeter(-1) {}

  //! Constructs and starts (if autoStart is true) the named meter
  OSD_PerfMeter (const char* theMeter,
                 const bool  theToAutoStart = true)
  : myIMeter (perf_get_meter (theMeter, 0, 0))
  {
    if (myIMeter < 0) myIMeter = perf_init_meter (theMeter);
    if (theToAutoStart) Start();
  }

  //! Prepares the named meter
  void Init (const char* theMeter)
  {
    myIMeter = perf_get_meter (theMeter, 0, 0);
    if (myIMeter < 0) myIMeter = perf_init_meter (theMeter);
  }

  //! Starts the meter
  void Start() const { perf_start_imeter(myIMeter); }

  //! Stops the meter
  void Stop() const { perf_stop_imeter(myIMeter); }

  //! Increments the counter w/o time measurement
  void Tick() const { perf_tick_imeter(myIMeter); }

  //! Outputs the meter data and resets it to initial state
  void Flush() const { perf_close_imeter(myIMeter); }

  //! Assures stopping upon destruction
  ~OSD_PerfMeter() { if (myIMeter >= 0) Stop(); }

private:

  OSD_PerfMeter(const OSD_PerfMeter&);
  OSD_PerfMeter& operator= (const OSD_PerfMeter&);

protected:

  int myIMeter;

};

#endif // OSD_PerfMeter_HeaderFile
