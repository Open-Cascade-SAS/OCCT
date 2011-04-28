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

#include <OpenGl_telem.hxx>
#include <OpenGl_tgl.hxx>
#include <NCollection_List.hxx>

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


typedef  struct
{
  Tmatrix3    orientation_matrix;
  Tmatrix3    mapping_matrix;
  Tlimit3     clip_limit;
  Tint        clip_xy;
  Tint        clip_back;
  Tint        clip_front;
  Tint        shield_indicator;
  TEL_COLOUR  shield_colour;
  Tint        border_indicator;
  TEL_COLOUR  border_colour;
  Tint        active_status;
  NCollection_List<CALL_DEF_PLANE> clipping_planes;
  struct
  {
    Tfloat  vrp[3];
    Tfloat  vpn[3];
    Tfloat  vup[3];
    TEL_VIEW_MAPPING  map;
    Tfloat  scaleFactors[3];
  } extra;
} TEL_VIEW_REP, *tel_view_rep;


typedef  enum
{
  UVN2XYZ, XYZ2UVN
} TelCoordType;

extern  Tint  TglUpdateView0; /* defined in telem/view */

extern  void              /* vrp,     vpn,     vup,     error_ind, mat */
TelEvalViewOrientationMatrix(Tfloat*, Tfloat*, Tfloat*, Tfloat*, Tint*, Tmatrix3);

extern  void            /*         mapping, error_ind,  mat */
TelEvalViewMappingMatrix( tel_view_mapping, Tint*, Tmatrix3 );

extern  void            /*         mapping, error_ind,  mat,    cx,     cy */
TelEvalViewMappingMatrixPick(tel_view_mapping, Tint*, Tmatrix3, Tfloat, Tfloat);

extern  TStatus         /* ws,  view_id, view_representation */
TelSetViewRepresentation( Tint, Tint, tel_view_rep );

extern  TStatus /* ws, view_id,      type,     uvn,     xyz */
TelConvertCoord( Tint, Tint, TelCoordType, Tfloat*, Tfloat* );

extern  TStatus TelSetViewIndex( Tint, Tint );  /* ws, view_id */
extern  TStatus TelSetViewProjection( Tint, Tint );  /* ws, view_id */
extern  TStatus         /* ws,  view_id, view_representation */
TelGetViewRepresentation( Tint, Tint, tel_view_rep );
extern TStatus TelDeleteViewsForWS( Tint wsid );


extern  void    TelClearViews( Tint ); /* ws */
extern  void    TelClearBackground( Tint ); /* ws */

extern TStatus TelPrintViewRepresentation( Tint, Tint ); /* ws, view */
extern TStatus Tel2Dto3D( Tint ws, Tint vid, Tint x, Tint y,
                         Tfloat *x1, Tfloat *y1, Tfloat *z1,
                         Tfloat *x2, Tfloat *y2, Tfloat *z2 );
/* x is from bottom */
/* y is from top */

#endif
