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

#ifndef __INCLUDE_LIBRARY_BUF_CMD_RSP_SIZ_H
# define __INCLUDE_LIBRARY_BUF_CMD_RSP_SIZ_H

#ifdef CSF1
#define BUF_CMD_SIZ_WRD 1000
#define BUF_RSP_SIZ_WRD 1000

#else
#define BUF_CMD_SIZ_WRD 640
#define BUF_RSP_SIZ_WRD 640
#endif

#define BUF_CMD_SIZ_BYT BUF_CMD_SIZ_WRD*4
#define BUF_RSP_SIZ_BYT BUF_RSP_SIZ_WRD*4

#endif  /* __INCLUDE_LIBRARY_BUF_CMD_RSP_SIZ_H */
