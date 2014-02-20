/*
 Copyright (c) 1998-1999 Matra Datavision
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

#ifdef UNX
#define DEBUG           0   /* false */
#elif defined(WNT)
#define DEBUG           0   /* false */
#else
#define DEBUG           0   /* false */
#endif

#if DEBUG                    /* AVEC DEBUG */

#ifdef CSF1
#define DO_MALLOC 0          /* True : Pas de malloc(libc) */
#define RPC_TRACE 0          /* True : Trace dans Serpc */
#define NTD_TRACE 0
#define OSD_TRACE 0
#define ACS_TRACE 0
#define FRONT_TRACE 0

#else                        /* Euclid */
#ifdef UNX
#define TKBD_TRACE 0         /* false */
#define NTD_TRACE 0
#define OSD_TRACE 0
#define RPC_TRACE 0
#define ACS_TRACE 0
#else
#define TKBD_TRACE 0         /* false */
#define NTD_TRACE 0
#define OSD_TRACE 0
#define RPC_TRACE 0
#define ACS_TRACE 0
#endif

#endif

#else                        /* Pas debug */
#ifdef CSF1
#define DO_MALLOC 0          /* True : Pas de malloc(libc) */
#define RPC_TRACE 0
#define NTD_TRACE 0
#define OSD_TRACE 0
#define ACS_TRACE 0

#else                        /* Euclid sans debug */
#define TKBD_TRACE 0
#define RPC_TRACE 0
#define NTD_TRACE 0
#define OSD_TRACE 0
#define ACS_TRACE 0
#endif

#endif

