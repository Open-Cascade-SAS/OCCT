// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

/***********************************************************************

FONCTION :
----------
File OpenGl_telem_view.h :


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : xxx ; Creation.
02-07-96 : FMN ; Suppression WSWSHeight et WSWSWidth
Suppression de TelSetWSWindow
03-07-96 : FMN ; A une workstation correspond une vue.
Suppression de TelGetViewportAtLocation()
Suppression de TelPrintAllViews()
03-03-98 : FMN ; Suppression variable externe TglWhetherPerspective 

************************************************************************/

#ifndef  OPENGL_TELEM_VIEW_H
#define  OPENGL_TELEM_VIEW_H

#include <InterfaceGraphic_tgl_all.hxx>
#include <Handle_OpenGl_Display.hxx>

typedef  struct
{
  Tfloat    xmin;
  Tfloat    xmax;
  Tfloat    ymin;
  Tfloat    ymax;
} Tlimit;

typedef  struct
{
  Tfloat    xmin;
  Tfloat    xmax;
  Tfloat    ymin;
  Tfloat    ymax;
  Tfloat    zmin;
  Tfloat    zmax;
} Tlimit3;

typedef  enum
{
  TelParallel, TelPerspective
} TelProjType;

typedef  struct
{
  Tlimit      window;
  Tlimit3     viewport;
  TelProjType proj;
  Tfloat      prp[3];
  Tfloat      vpd;
  Tfloat      fpd;
  Tfloat      bpd;
} TEL_VIEW_MAPPING, *tel_view_mapping;

extern  void              /* vrp,     vpn,     vup,     error_ind, mat */
TelEvalViewOrientationMatrix(Tfloat*, Tfloat*, Tfloat*, Tfloat*, Tint*, Tmatrix3);

extern  void            /*         mapping, error_ind,  mat */
TelEvalViewMappingMatrix (const Handle(OpenGl_Display)& theGlDisplay, tel_view_mapping theMapping, Tint* theError, Tmatrix3 theMat);

#endif
