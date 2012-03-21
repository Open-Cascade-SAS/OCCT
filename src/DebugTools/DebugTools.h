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

#ifndef _DEBUGTOOLS_H
#define _DEBUGTOOLS_H

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
Standard_EXPORT int	perf_tick_meter         (const char * const MeterName);
Standard_EXPORT int	perf_tick_imeter        (const int  iMeter);
Standard_EXPORT int	perf_start_meter        (const char * const MeterName);
Standard_EXPORT int	perf_start_imeter       (const int  iMeter);
Standard_EXPORT int	perf_stop_meter         (const char * const MeterName);
Standard_EXPORT int	perf_stop_imeter        (const int  iMeter);
Standard_EXPORT int	perf_get_meter          (const char * const MeterName,
                                                 int        * nb_enter,
                                                 double	    * seconds);
Standard_EXPORT void    perf_close_meter        (const char * const MeterName);
Standard_EXPORT void	perf_print_all_meters   (void);
Standard_EXPORT void	perf_destroy_all_meters (void);

#ifdef __cplusplus
}
#endif

#ifdef PERF_ENABLE_METERS
#define PERF_TICK_METER(_m_name) {              \
  static int __iMeter = 0;                      \
  if  (__iMeter)  perf_tick_imeter (__iMeter);  \
  else __iMeter = perf_tick_meter (_m_name);    \
}

#define PERF_START_METER(_m_name) {             \
  static int __iMeter = 0;                      \
  if  (__iMeter)  perf_start_imeter (__iMeter); \
  else __iMeter = perf_start_meter (_m_name);   \
}

#define PERF_STOP_METER(_m_name) {              \
  static int __iMeter = 0;                      \
  if  (__iMeter)  perf_stop_imeter (__iMeter);  \
  else __iMeter = perf_stop_meter (_m_name);    \
}

#define PERF_CLOSE_METER(_m_name) perf_close_meter (_m_name);

#define PERF_PRINT_ALL {                        \
  perf_print_all_meters();                      \
}

#define PERF_PRINT_ALL_METERS {                 \
  perf_print_all_meters();                      \
  perf_destroy_all_meters();                    \
}

#else
#define PERF_TICK_METER(_m_name)
#define PERF_START_METER(_m_name)
#define PERF_STOP_METER(_m_name)
#define PERF_CLOSE_METER(_m_name)
#define PERF_PRINT_ALL
#define PERF_PRINT_ALL_METERS
#endif

#endif
