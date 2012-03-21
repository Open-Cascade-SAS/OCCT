// Copyright (c) 1997-1999 Matra Datavision
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
