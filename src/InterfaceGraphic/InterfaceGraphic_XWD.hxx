
#ifndef InterfaceGraphic_XWDHeader
#define InterfaceGraphic_XWDHeader

/* 
  
                     Copyright (C) 1991,1992,1993 by
  
                      MATRA DATAVISION, FRANCE
  
This software is furnished in accordance with the terms and conditions
of the contract and with the inclusion of the above copyright notice.
This software or any other copy thereof may not be provided or otherwise
be made available to any other person. No title to an ownership of the
software is hereby transferred.
  
At the termination of the contract, the software and all copies of this
software must be deleted.
  
Facility : CAS-CADE V1
  
*/ 

/*
 * InterfaceGraphic_SWAPTEST =	0 sur sun, sgi, hp
 *				1 sur ao1
 */

#define InterfaceGraphic_SWAPTEST (*(char*)&InterfaceGraphic_swaptest)
static unsigned long InterfaceGraphic_swaptest = 1;

#include <InterfaceGraphic_X11.hxx>

#include <X11/XWDFile.h>

#endif /* InterfaceGraphic_XWDHeader */
