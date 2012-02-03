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
