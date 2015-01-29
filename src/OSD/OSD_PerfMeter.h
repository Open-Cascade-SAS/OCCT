/*
 Copyright (c) 1999-2014 OPEN CASCADE SAS

 This file is part of Open CASCADE Technology software library.

 This library is free software; you can redistribute it and/or modify it under
 the terms of the GNU Lesser General Public License version 2.1 as published
 by the Free Software Foundation, with special exception defined in the file
 OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
 distribution for complete text of the license and disclaimer of any warranty.

 Alternatively, this file may be used under the terms of Open CASCADE
 commercial license or contractual agreement.
*/

#ifndef _OSD_PERFMETER_H
#define _OSD_PERFMETER_H

/**
 * Macros for convenient and fast usage of meters.
 * Define PERF_ENABLE_METERS to make them available.
 */
#ifdef PERF_ENABLE_METERS

/**
 * @def PERF_START_METER(theMeterName)
 * Forces meter MeterName to begin to count by remembering the current data of timer.
 * Creates new meter if there is no such meter.
 */
#define PERF_START_METER(_m_name) { \
  static int __iMeter = -1; \
  if  (__iMeter >= 0)  perf_start_imeter (__iMeter); \
  else      __iMeter = perf_start_meter (_m_name);   \
}

/**
 * @def PERF_STOP_METER(theMeterName)
 * Forces meter MeterName to stop and cumulate the time elapsed since the start.
 */
#define PERF_STOP_METER(_m_name) { \
  static int __iMeter = -1; \
  if  (__iMeter >= 0)  perf_stop_imeter (__iMeter); \
  else      __iMeter = perf_stop_meter (_m_name); \
}

/**
 * @def PERF_TICK_METER(theMeterName)
 * Increments the counter of meter MeterName without changing its state with respect to measurement of time.
 * Creates new meter if there is no such meter.
 * It is useful to count the number of enters to a part of code without wasting a time to measure CPU time.
 */
#define PERF_TICK_METER(_m_name) { \
  static int __iMeter = -1; \
  if  (__iMeter >= 0)  perf_tick_imeter (__iMeter); \
  else      __iMeter = perf_tick_meter (_m_name); \
}

/**
 * @def PERF_CLOSE_METER(theMeterName)
 * Prints out and resets the given meter.
 */
#define PERF_CLOSE_METER(_m_name) perf_close_meter (_m_name);

/**
 * @def PERF_PRINT_ALL
 * Prints all existing meters which have been entered at least once and resets them.
 */
#define PERF_PRINT_ALL() { \
  perf_print_all_meters(1); \
}

#else
  #define PERF_TICK_METER(_m_name)
  #define PERF_START_METER(_m_name)
  #define PERF_STOP_METER(_m_name)
  #define PERF_CLOSE_METER(_m_name)
  #define PERF_PRINT_ALL()
#endif

/**
 * Creates new counter (if it is absent) identified by theMeterName and resets its cumulative value
 * @return meter global identifier if OK, -1 if alloc problem
 */
Standard_EXPORTEXTERNC int perf_init_meter (const char* const theMeterName);

/**
 * Forces meter theMeterName to begin to count by remembering the current data of timer.
 * Creates new meter if there is no such meter.
 * @return meter global identifier if OK, -1 if no such meter and cannot create a new one
 */
Standard_EXPORTEXTERNC int perf_start_meter (const char* const theMeterName);

/**
 * Forces meter with number theMeterId to begin count by remembering the current data of timer.
 * @return meter global identifier if OK, -1 if no such meter
 */
Standard_EXPORTEXTERNC int perf_start_imeter (const int theMeterId);

/**
 * Forces meter theMeterName to stop and cumulate the time elapsed since the start.
 * @return meter global identifier if OK, -1 if no such meter or it is has not been started
 */
Standard_EXPORTEXTERNC int perf_stop_meter (const char* const theMeterName);

/**
 * Forces meter with number theMeterId to stop and cumulate the time elapsed since the start.
 * @return meter global identifier if OK, -1 if no such meter or it is has not been started
 */
Standard_EXPORTEXTERNC int perf_stop_imeter (const int theMeterId);

/**
 * Increments the counter of meter theMeterName without changing its state with respect to measurement of time.
 * Creates new meter if there is no such meter.
 * @return meter global identifier if OK, -1 if no such meter and cannot create a new one
 */
Standard_EXPORTEXTERNC int perf_tick_meter (const char* const theMeterName);

/**
 * Increments the counter of meter theMeterId without changing its state with respect to measurement of time.
 * @return meter global identifier if OK, -1 if no such meter
 */
Standard_EXPORTEXTERNC int perf_tick_imeter (const int theMeterId);

/**
 * Tells the time cumulated by meter theMeterName and the number of enters to this meter.
 * @param theNbEnter [OUT] number of enters if the pointer != NULL
 * @param theSeconds [OUT] seconds if the pointer != NULL
 * @return meter global identifier if OK, -1 if no such meter
*/
Standard_EXPORTEXTERNC int perf_get_meter (const char* const theMeterName,
                                           int*              theNbEnter,
                                           double*           theSeconds);

/**
 * Prints on stdout the cumulated time and the number of enters for the specified meter.
 */
Standard_EXPORTEXTERNC void perf_close_meter (const char* const theMeterName);

/**
 * Prints on stdout the cumulated time and the number of enters for the specified meter.
 */
Standard_EXPORTEXTERNC void perf_close_imeter (const int theMeterId);

/**
 * Prints on stdout the cumulated time and the number of enters for each alive meter which have the number of enters > 0.
 * Resets all meters if reset is non-null.
 */
Standard_EXPORTEXTERNC void perf_print_all_meters (int reset);

/**
 * Prints to supplied string buffer the cumulated time and the number of enters 
 * for each alive meter with the number of enters > 0.
 * If buffer length is not sufficient, data of some meters may be lost.
 * It is recommended to reserve 256 bytes per meter, 25600 bytes should fit all.
 * Resets all meters.
 */
Standard_EXPORTEXTERNC void perf_sprint_all_meters (char *buffer, int length, int reset);

/**
 * Deletes all meters and frees memory.
 */
Standard_EXPORTEXTERNC void perf_destroy_all_meters (void);

/**
 * ATTENTION!!!
 * This func calls perf_print_all_meters() and perf_destroy_all_meters()
 * and is called automatically at the end of a program via system call atexit().
 */
Standard_EXPORTEXTERNC void perf_print_and_destroy (void);

#endif
