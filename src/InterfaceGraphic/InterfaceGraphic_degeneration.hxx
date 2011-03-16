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
  IMPLEMENT_MEMORY_OPERATORS
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
  IMPLEMENT_MEMORY_OPERATORS  
};
typedef _ds_internal  DS_INTERNAL;
typedef _ds_internal* PDS_INTERNAL;

#endif  /* __INTERFACEGRAPHIC_DEGENERATION_H */
