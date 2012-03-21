// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef __INTERFACEGRAPHIC_DEGENERATION_H
# define __INTERFACEGRAPHIC_DEGENERATION_H

/*
Created by EUG

16/06/2000 : ATS : G005 : Copied from OpenGl to InterfaceGraphic to support required 
for InterfaceGraphic_Parray.hxx definitions
*/

#include <InterfaceGraphic_telem.hxx>

struct _degeneration 
{
  int   mode;
  float skipRatio;
  DEFINE_STANDARD_ALLOC
};
typedef _degeneration  DEGENERATION;
typedef _degeneration* PDEGENERATION;

struct _ds_internal 
{

  unsigned int        list;
  unsigned int        dlist;
  int                 degMode;
  int                 model;
  float               skipRatio;
  unsigned char*      bDraw;
  DEFINE_STANDARD_ALLOC
};
typedef _ds_internal  DS_INTERNAL;
typedef _ds_internal* PDS_INTERNAL;

#endif  /* __INTERFACEGRAPHIC_DEGENERATION_H */
