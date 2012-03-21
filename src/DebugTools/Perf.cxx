// Copyright (c) 1999-2012 OPEN CASCADE SAS
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

/*======================================================================
File    :       Perf.c
Purpose :       Set of functions to measure the CPU user time
Author  :       Michael SAZONOV <msv@nnov.matra-dtv.fr>
Created :       10/08/2000
History :	25/09/2001 : AGV : (const char *) in prototypes;
				   search in table using hashvalue
                09/11/2001 : AGV : Add functions perf_*_imeter for performance
                                   Add function perf_tick_meter
======================================================================*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <DebugTools.h>

#ifndef WNT
#include <unistd.h>
#endif

/* Function times() is more precise than clock() because it does not take into
   account the system time and the time of child processes                   */
#include <time.h>
#ifdef WNT
#define STRICT
#include <windows.h>
typedef __int64 PERF_TIME;
#define PICK_TIME(_utime) {     \
  FILETIME t1, t2, ktime;       \
  GetThreadTimes (GetCurrentThread(), &t1, &t2, &ktime, (FILETIME *)&(_utime));\
}
#define GET_SECONDS(_utime) (((double)(_utime))/10000000.)
#else
#include <sys/times.h>
typedef clock_t PERF_TIME;
#define PICK_TIME(_utime) {     \
  struct tms tmbuf;             \
  times (&tmbuf);               \
  (_utime) = tmbuf.tms_utime;   \
}

#ifndef CLK_TCK
#define CLK_TCK 100             /* as SunOS */
#endif

#define GET_SECONDS(_utime) (((double)(_utime))/CLK_TCK)
#endif
/*======================================================================
        DEFINITIONS
======================================================================*/

typedef struct {
  char*         name;           /* identifier */
  unsigned int	hash;		/* hash value */
  PERF_TIME     cumul_time;     /* cumulative time */
  PERF_TIME     start_time;     /* to store start time */
  int           nb_enter;       /* number of enters */
} t_TimeCounter;

#define MAX_METERS 100

static t_TimeCounter MeterTable[MAX_METERS];
static int nb_meters = 0;

static t_TimeCounter*	find_meter	 (const char * const MeterName);
static t_TimeCounter*	_perf_init_meter (const char * const MeterName,
					  const int    doFind);
static unsigned int	hash_value	 (const char * const aString);

/*======================================================================
Function :      perf_init_meter
Purpose  :      Creates new counter (if it is absent) identified by
                MeterName and resets its cumulative value
Returns  :      iMeter if OK, 0 if alloc problem
======================================================================*/
int perf_init_meter (const char * const MeterName)
{
  t_TimeCounter* ptc = _perf_init_meter (MeterName, ~0);
  return (ptc ? (ptc-MeterTable)+1 : 0);
}

/*======================================================================
Function :      perf_tick_meter
Purpose  :      Increments the counter of meter MeterName without changing
                its state with respect to measurement of time.
		creates new meter if there is no such meter
Returns  :      iMeter if OK, 0 if no such meter and cannot create a new one
======================================================================*/
int perf_tick_meter (const char * const MeterName)
{
  t_TimeCounter* ptc = find_meter (MeterName);

  if (!ptc) {
    /* create new meter */
    ptc = _perf_init_meter (MeterName, 0);
  }

  if (ptc) {
    ptc -> nb_enter ++;
    return (ptc-MeterTable) + 1;
  }
  return 0;
}

/*======================================================================
Function :      perf_tick_imeter
Purpose  :      Increments the counter of meter iMeter without changing
                its state with respect to measurement of time.
Returns  :      iMeter if OK, 0 if no such meter
======================================================================*/
int perf_tick_imeter (const int iMeter)
{
  if (iMeter > 0 && iMeter <= nb_meters) {
    MeterTable[iMeter-1].nb_enter ++;
    return iMeter;
  }
  return 0;
}

/*======================================================================
Function :      perf_start_meter
Purpose  :      Forces meter MeterName to begin to count by remembering
                the current data of timer;
		creates new meter if there is no such meter
Returns  :      iMeter if OK, 0 if no such meter and cannot create a new one
======================================================================*/
int perf_start_meter (const char * const MeterName)
{
  t_TimeCounter* ptc = find_meter (MeterName);

  if (!ptc) {
    /* create new meter */
    ptc = _perf_init_meter (MeterName, 0);
  }

  if (ptc) {
    PICK_TIME (ptc->start_time)
    return (ptc - MeterTable) + 1;
  }

  return 0;
}

/*======================================================================
Function :      perf_start_imeter
Purpose  :      Forces meter with number iMeter to begin count by remembering
                the current data of timer;
		the meter must be previously created
Returns  :      iMeter if OK, 0 if no such meter
======================================================================*/
int perf_start_imeter (const int iMeter)
{
  if (iMeter > 0 && iMeter <= nb_meters) {
    t_TimeCounter * const ptc = &MeterTable[iMeter-1];
    PICK_TIME (ptc->start_time)
    return iMeter;
  }
  return 0;
}

/*======================================================================
Function :      perf_stop_meter
Purpose  :      Forces meter MeterName to stop and cumulate time elapsed
                since start
Returns  :      1 if OK, 0 if no such meter or it is has not been started
======================================================================*/
int perf_stop_meter (const char * const MeterName)
{
  t_TimeCounter* ptc = find_meter (MeterName);

  if (ptc && ptc->start_time) {
    PERF_TIME utime;
    PICK_TIME (utime)
    ptc->cumul_time += utime - ptc->start_time;
    ptc->start_time = 0;
    ptc->nb_enter++;
    return (ptc-MeterTable) + 1;
  }

  return 0;
}

/*======================================================================
Function :      perf_stop_imeter
Purpose  :      Forces meter with number iMeter to stop and cumulate the time
                elapsed since the start
Returns  :      iMeter if OK, 0 if no such meter
======================================================================*/
int perf_stop_imeter (const int iMeter)
{
  if (iMeter > 0 && iMeter <= nb_meters) {
    t_TimeCounter * const ptc = &MeterTable[iMeter-1];

    if (ptc->start_time) {
      PERF_TIME utime;
      PICK_TIME (utime)
      ptc->cumul_time += utime - ptc->start_time;
      ptc->start_time = 0;
      ptc->nb_enter++;
      return iMeter;
    }
  }
  return 0;
}

/*======================================================================
Function :      perf_get_meter
Purpose  :      Tells the time cumulated by meter MeterName and the number
                of enters to this meter
Output   :      *nb_enter, *seconds if the pointers != NULL
Returns  :      iMeter if OK, 0 if no such meter
======================================================================*/
int perf_get_meter (const char  * const MeterName,
                    int         * nb_enter,
                    double      * seconds)
{
  t_TimeCounter* ptc = find_meter (MeterName);

  if (!ptc) return 0;

  if (nb_enter) *nb_enter = ptc->nb_enter;
  if (seconds) *seconds = GET_SECONDS(ptc->cumul_time);
  return (ptc-MeterTable) + 1;
}

/*======================================================================
Function :      perf_print_all_meters
Purpose  :      Prints on stdout the cumulated time and the number of
                enters for each meter in MeterTable;
                resets all meters
Output   :      none
Returns  :      none
======================================================================*/
void perf_print_all_meters (void)
{
  int i;

  if (!nb_meters) return;

  printf ("     Perf meter results                  :   enters   seconds  enters/sec\n");

  for (i=0; i<nb_meters; i++) {
    t_TimeCounter* ptc = &MeterTable[i];

    if (ptc && ptc->nb_enter) {
      double secs = GET_SECONDS(ptc->cumul_time);

      if (ptc->start_time)
        printf ("Warning : meter %s has not been stopped\n", ptc->name);

      printf ("%-40s : %7d %8.2f %10.2f\n",
              ptc->name, ptc->nb_enter, secs,
              (secs>0. ? ptc->nb_enter / secs : 0.));

      ptc->cumul_time = 0;
      ptc->start_time = 0;
      ptc->nb_enter   = 0;
    }
  }
}

/*======================================================================
Function :      perf_close_meter
Purpose  :      Prints out a meter and resets it
Returns  :      none
======================================================================*/
void perf_close_meter (const char * const MeterName)
{
  t_TimeCounter* ptc = find_meter (MeterName);
  if (ptc && ptc->nb_enter) {
    if (ptc->start_time)
      printf ("  ===> Warning : meter %s has not been stopped\n", ptc->name);
    printf ("  ===> [%s] : %d enters, %9.3f seconds\n",
            ptc->name, ptc->nb_enter, GET_SECONDS(ptc->cumul_time));
    ptc->cumul_time = 0;
    ptc->start_time = 0;
    ptc->nb_enter   = 0;
  }
}

/*======================================================================
Function :      perf_destroy_all_meters
Purpose  :      Deletes all meters and frees memory
Returns  :      none
======================================================================*/
void perf_destroy_all_meters (void)
{
  int i;
  for (i=0; i<nb_meters; i++)
    free (MeterTable[i].name);
  nb_meters = 0;
}

/* agv - non portable: #pragma fini (perf_print_and_destroy) */

void perf_print_and_destroy (void)
{
  perf_print_all_meters ();
  perf_destroy_all_meters ();
}

/*======================================================================
Function :      _perf_init_meter
Purpose  :      Creates new counter (if it is absent) identified by
                MeterName and resets its cumulative value
Returns  :      the pointer if OK, 0 if alloc problem
Remarks  :      For internal use in this module
======================================================================*/
static t_TimeCounter* _perf_init_meter (const char * const MeterName,
                                        const int  doFind)
{
  static int hasbeencalled = 0;
  t_TimeCounter* ptc = 0;
  if (doFind)
    ptc = find_meter (MeterName);

  if (!ptc) {
    if (nb_meters >= MAX_METERS) return 0;
    ptc = &MeterTable[nb_meters];

    ptc -> name = strdup (MeterName);
    if (!ptc -> name)
      return 0;

    ptc -> hash = hash_value (MeterName);
    nb_meters++;
  }

  ptc->cumul_time = 0;
  ptc->start_time = 0;
  ptc->nb_enter   = 0;
  if (hasbeencalled == 0) {
    /*atexit (perf_print_and_destroy);*/
    hasbeencalled = ~0;
  }
  return ptc;
}

/*======================================================================
Function :      find_meter
Purpose  :      Finds the meter MeterName in the MeterTable
Returns  :      Pointer to meter object
Remarks  :      For internal use in this module
======================================================================*/
static t_TimeCounter* find_meter (const char * const MeterName)
{
  int i;
  const unsigned int aHash = hash_value (MeterName);
  for (i=0; i<nb_meters; i++)
    if (MeterTable[i].hash == aHash)
      if (!strcmp (MeterTable[i].name, MeterName)) return &MeterTable[i];
  return NULL;
}

static const unsigned int wCRC16a[16] =
{
    0000000,    0140301,    0140601,    0000500,
    0141401,    0001700,    0001200,    0141101,
    0143001,    0003300,    0003600,    0143501,
    0002400,    0142701,    0142201,    0002100,
};

static const unsigned int wCRC16b[16] =
{
    0000000,    0146001,    0154001,    0012000,
    0170001,    0036000,    0024000,    0162001,
    0120001,    0066000,    0074000,    0132001,
    0050000,    0116001,    0104001,    0043000,
};

/*======================================================================
Function :      hash_value
Returns  :      the hash value of the string
Remarks  :      For internal use in this module
======================================================================*/
static unsigned int hash_value (const char * const aString)
{
  int i;
  unsigned int aCRC = 0;
  const int aLen = strlen (aString);
  const unsigned char * aPtr = (const unsigned char *) aString;
  for (i = aLen; i > 0; i--) {
    const unsigned int  bTmp = aCRC ^ (const unsigned int) (* aPtr++);
    aCRC = ((aCRC >> 8) ^ wCRC16a[bTmp & 0x0F]) ^ wCRC16b[(bTmp >> 4) & 0x0F];
  }
  return aCRC;
}
