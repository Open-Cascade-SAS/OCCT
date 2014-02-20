// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _Aspect_Units_HeaderFile
#define _Aspect_Units_HeaderFile

/*
       Since Cas.cade version 1.5 ,the default unit LENGTH is MILLIMETER.
#define METER *1.
#define CENTIMETER *0.01
#define TOCENTIMETER(v) (v)*100.
#define FROMCENTIMETER(v) (v)/100.
#define MILLIMETER *0.001
#define TOMILLIMETER(v) (v)*1000.
#define FROMMILLIMETER(v) (v)/1000.
*/

#define METER *1000.
#define CENTIMETER *10.
#define TOCENTIMETER(v) (v)/10.
#define FROMCENTIMETER(v) (v)*10.
#define MILLIMETER *1.
#define TOMILLIMETER(v) v
#define FROMMILLIMETER(v) v

#endif
