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
