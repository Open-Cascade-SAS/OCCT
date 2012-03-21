/*
 Copyright (c) 1999-2012 OPEN CASCADE SAS

 The content of this file is subject to the Open CASCADE Technology Public
 License Version 6.5 (the "License"). You may not use the content of this file
 except in compliance with the License. Please obtain a copy of the License
 at http://www.opencascade.org and read it completely before using this file.

 The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
 main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.

 The Original Code and all software distributed under the License is
 distributed on an "AS IS" basis, without warranty of any kind, and the
 Initial Developer hereby disclaims all such warranties, including without
 limitation, any warranties of merchantability, fitness for a particular
 purpose or non-infringement. Please see the License for the specific terms
 and conditions governing the rights and limitations under the License.

*/

#ifndef _OSD_PERFMETER_H
#define _OSD_PERFMETER_H

/*
  Macros for convenient and fast usage of meters.
  Define PERF_ENABLE_METERS to make them available.
*/

#ifdef PERF_ENABLE_METERS

/* PERF_START_METER
   Forces meter MeterName to begin to count by remembering
   the current data of timer.
   Creates new meter if there is no such meter
*/
#define PERF_START_METER(_m_name) {                  \
  static int __iMeter = -1;                          \
  if  (__iMeter >= 0)  perf_start_imeter (__iMeter); \
  else      __iMeter = perf_start_meter (_m_name);   \
}

/* PERF_STOP_METER
   Forces meter MeterName to stop and cumulate the time elapsed
   since the start
*/
#define PERF_STOP_METER(_m_name) {                   \
  static int __iMeter = -1;                          \
  if  (__iMeter >= 0)  perf_stop_imeter (__iMeter);  \
  else      __iMeter = perf_stop_meter (_m_name);    \
}

/* PERF_TICK_METER
   Increments the counter of meter MeterName without changing
   its state with respect to measurement of time.
   Creates new meter if there is no such meter.
   It is useful to count the number of enters to a part of code
   without wasting a time to measure CPU time.
*/
#define PERF_TICK_METER(_m_name) {                   \
  static int __iMeter = -1;                          \
  if  (__iMeter >= 0)  perf_tick_imeter (__iMeter);  \
  else      __iMeter = perf_tick_meter (_m_name);    \
}

/* PERF_CLOSE_METER
   Prints out and resets the given meter
*/
#define PERF_CLOSE_METER(_m_name) perf_close_meter (_m_name);

/* PERF_PRINT_ALL
   Prints all existing meters which have been entered at least once
   and resets them
*/
#define PERF_PRINT_ALL {                              \
  perf_print_all_meters();                            \
}


#else
#define PERF_TICK_METER(_m_name)
#define PERF_START_METER(_m_name)
#define PERF_STOP_METER(_m_name)
#define PERF_CLOSE_METER(_m_name)
#define PERF_PRINT_ALL
#endif

#ifndef Standard_EXPORT
#ifdef WNT
#define Standard_EXPORT __declspec( dllexport )
#else
#define Standard_EXPORT extern
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

Standard_EXPORT int     perf_init_meter         (const char * const MeterName);
/* Creates new counter (if it is absent) identified by
   MeterName and resets its cumulative value
   Returns  : iMeter if OK, -1 if alloc problem
*/

Standard_EXPORT int     perf_start_meter        (const char * const MeterName);
/* Forces meter MeterName to begin to count by remembering
   the current data of timer.
   Creates new meter if there is no such meter
   Returns  : iMeter if OK, -1 if no such meter and cannot create a new one
*/

Standard_EXPORT int     perf_start_imeter       (const int  iMeter);
/* Forces meter with number iMeter to begin count by remembering
   the current data of timer.
   Returns  : iMeter if OK, -1 if no such meter
*/

Standard_EXPORT int     perf_stop_meter         (const char * const MeterName);
/* Forces meter MeterName to stop and cumulate the time elapsed since the start
   Returns  : iMeter if OK, -1 if no such meter or it is has not been started
*/

Standard_EXPORT int     perf_stop_imeter        (const int  iMeter);
/* Forces meter with number iMeter to stop and cumulate the time
   elapsed since the start.
   Returns  : iMeter if OK, -1 if no such meter or it is has not been started
*/

Standard_EXPORT int     perf_tick_meter         (const char * const MeterName);
/* Increments the counter of meter MeterName without changing
   its state with respect to measurement of time.
   Creates new meter if there is no such meter
   Returns  : iMeter if OK, -1 if no such meter and cannot create a new one
*/

Standard_EXPORT int     perf_tick_imeter        (const int  iMeter);
/* Increments the counter of meter iMeter without changing
   its state with respect to measurement of time.
   Returns  : iMeter if OK, -1 if no such meter
*/

Standard_EXPORT int     perf_get_meter          (const char * const MeterName,
                                                 int        * nb_enter,
                                                 double     * seconds);
/* Tells the time cumulated by meter MeterName and the number
   of enters to this meter
   Output   :      *nb_enter, *seconds if the pointers != NULL
   Returns  :      iMeter if OK, -1 if no such meter
*/

Standard_EXPORT void    perf_close_meter        (const char * const MeterName);
/* Prints on stdout the cumulated time and the number of enters
   for the specified meter
*/

Standard_EXPORT void    perf_close_imeter       (const int iMeter);
/* Prints on stdout the cumulated time and the number of enters
   for the specified meter
*/

Standard_EXPORT void    perf_print_all_meters   (void);
/* Prints on stdout the cumulated time and the number of
   enters for each alive meter which have the number of enters > 0.
   Resets all meters
*/

Standard_EXPORT void    perf_destroy_all_meters (void);
/* Deletes all meters and frees memory
*/

extern          void    perf_print_and_destroy (void);
/* ATTENTION !!!
   This func calls both perf_print_all_meters() and perf_destroy_all_meters()
   and is called automatically at the end of a program
   via system call atexit()
*/

#ifdef __cplusplus
}
#endif

#endif
