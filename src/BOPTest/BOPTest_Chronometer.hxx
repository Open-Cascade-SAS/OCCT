// Created by: Peter KURNEV
// Copyright (c) 2010-2014 OPEN CASCADE SAS
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

#ifndef  BOPTest_Chronometer_HeaderFile
#define  BOPTest_Chronometer_HeaderFile
//
#include <OSD_Timer.hxx>
//=======================================================================
//class    : BOPTest_Chronometer
//purpose  : 
//=======================================================================
class BOPTest_Chronometer {
 public:
  BOPTest_Chronometer() {
  }
  //
  ~BOPTest_Chronometer() {
  }
  //
  void Start() {
    myChronometer.Reset();
    myChronometer.Start();
  }
  //
  void Stop() {
    myChronometer.Stop();
    myTime=myChronometer.ElapsedTime();
  }
  //
  double Time() const{
    return myTime;
  };
  //
 protected:
  OSD_Timer myChronometer;
  double myTime;
};

#endif
