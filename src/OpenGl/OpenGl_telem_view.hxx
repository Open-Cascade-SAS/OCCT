// Copyright (c) 1995-1999 Matra Datavision
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
TelEvalViewMappingMatrix( tel_view_mapping, Tint*, Tmatrix3 );

#endif
