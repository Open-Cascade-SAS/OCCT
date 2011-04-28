/***********************************************************************

FONCTION :
----------
file OpenGl_togl_begin_immediat_mode.c :


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : CAL ; Creation.
08-03-96 : FMN ; - Ajout include manquant 
01-04-96 : CAL ; Integration MINSK portage WNT
15-04-96 : CAL ; Integration travail PIXMAP de Jim ROTH
07-05-96 : CAL ; Gestion du minmax calcule en C++
voir call_togl_set_minmax
Debug temporaire pour ROB, TelRobSetRGB ()
12-06-96 : CAL ; Recuperation des attributs lors d'un draw_structure
22-07-96 :  GG ; Faire fonctionner correctement le highlight
des polygones en mode immediat
11-09-96 : FMN ; Ajout appel a TxglWinset dans call_togl_begin_immediat_mode
04-11-96 : CAL : Retrait du mode simule, ajout du higlight
dans togl_draw_structure
06-11-96 : CAL : Retrait du test sur doublebuffer PRO5949
16-12-96 : CAL : Ajout d'un glFinish () dans end_immediat_mode
21-01-97 : CAL : Gestion du flag <partial> differente pour
eviter les comparaisons avec shortrealfirst ou last
22-04-97 : FMN ; Correction faute i a la place de j dans util_transform_pt
22-04-97 : FMN ; Correction appel util_transform_pt dans togl_set_minmax
24-04-97 : FMN ; (PRO4063) Ajout displaylist pour le mode transient
25-07-97 : CAL ; Portage WNT (Bool, False, True)
30-07-97 : FMN ; Meilleure gestion de la desactivation des lights
10-09-97 : FMN ; Meilleure gestion de la desactivation des lights
07-10-97 : FMN ; Simplification WNT
Ajout glFlush dans call_togl_clear_immediat_mode()
26-11-97 : FMN ; Ajout mode Ajout et xor
Meilleure gestion display list pour le RetainMode
19-12-97 : FMN ; Oublie return dans le call_togl_begin_ajout_mode()
22-12-97 : FMN ; Suppression DEBUG_TEMPO_FOR_ROB
Correction PRO10217
Ajout affichage cadre TelCopyBuffer
Ajout LightOff sur le redraw.
02-02-98 : CAL ; Correction du bug sur les transformations de structure
tracees en mode transient. (PRO10217) (PRO12283)
05-02-98 : CAL ; Perfs : mise en place d'un flag identity pour savoir
si la matrice de transformation est l'identite.
08-04-98 : FMN ; Mailleur traitement du reaffichage d'une vue. Remplacement 
de TsmDisplayStructure() par call_func_redraw_all_structs(). 
08-07-98 : FMN ; PRO14399: Desactivation animationFlag quand on debute
le mode immediat (Transient et Ajout). 
30-09-98 : CAL ; Optimisation pour eviter de charger inutilement
les matrices de la vue.
27-11-98 : CAL ; S4062. Ajout des layers.
02.14.100 : JR : Warnings on WNT truncations from double to float

IMP190100 : GG
//                      -> Preserve current attributes because
//                      attributes can be changed between
//                      begin_immediate_mode() end end_immediate_mode()
//                      -> Optimize transformations computation.
//          -> Optimize REDRAW in immediat mode.
//  19/09/00  -> Avoid to raise in set_linetype() & set_linewith()
//        due to a bad call to TsmPutAttri() function.

BUC60726  GG_040900 Clear the last temporary detected in any case

BUC60900  GG_180501 Unhilight entity more earlier
when clear() is call instead at the next begin().

************************************************************************/


#define BUC60863  /*GG_100401 After any view update, made identical 
//      the front and back buffer to avoid ghost drawing.
//      Minimize flicking.
*/

#define IMP150501       /*GG_150501 CADPAK_V2 Enable/Disable Zbuffer  
NOTE that the previous and unused "double-buffer"
arg is changed to "zbuffer" and enable/disable
to use the OpenGl zbuffer capabilities during immediat
drawing 
*/

#define IMP260601 /*GG Enable correct backing store between 2 different views.
*/

#define IMP051001 /*GG Activates the polygon offset for polygon 
//      selection fill mode.
*/

/*----------------------------------------------------------------------*/

#define RIC120302       /* GG Enable to use the application display
//                      callback at end of traversal
*/

/*
* Includes
*/

#include <OpenGl_tgl_all.hxx>
#ifdef IMP051001
#include <OpenGl_Extension.hxx>
#endif
#include <math.h>
#include <GL/gl.h>
#include <stdio.h>

#include <InterfaceGraphic_Labels.hxx>
#include <InterfaceGraphic_Graphic3d.hxx>
#include <InterfaceGraphic_Visual3d.hxx>

#include <OpenGl_tsm_ws.hxx>
#include <OpenGl_txgl.hxx>
#include <OpenGl_tgl_tox.hxx>
#include <OpenGl_tgl_funcs.hxx>
#include <OpenGl_telem_filters.hxx>
#include <OpenGl_telem.hxx>
#include <OpenGl_telem_util.hxx>
#include <OpenGl_telem_view.hxx>
#include <OpenGl_LightBox.hxx>
#include <OpenGl_animation.hxx>
#include <OpenGl_telem_attri.hxx>
#include <OpenGl_tgl.hxx>
#include <OpenGl_tgl_subrvis.hxx>

int call_util_osd_getenv( char * , char * , int ) ;

/*----------------------------------------------------------------------*/
/* 
* Constantes
*/

#define xCOPY
#define xTRACE 0

#if TRACE > 0
#include <OpenGl_traces.h>
#endif


/*----------------------------------------------------------------------*/
/* 
* Variables externes
*/

/*----------------------------------------------------------------------*/
/*
* Prototypes fonctions
*/

/*----------------------------------------------------------------------*/
/*
* Prototypes Private functions
*/

static void call_util_transform_pt (float *x, float *y, float *z);
static int call_util_is_identity ();
static void call_util_transpose_mat (float tmat[16], float mat[4][4]);

/*----------------------------------------------------------------------*/
/* 
* Variables statiques
*/

static int openglNumberOfPoints = 0;
static GLint openglViewId = 0;
static GLint openglWsId = 0;

static int identity = 1;

static int partial = -1;  /* -1 init, 0 complete, 1 partielle */

static float xm, ym, zm, XM, YM, ZM;

static float openglMat[4][4] = {
  {1., 0., 0., 0.},
  {0., 1., 0., 0.},
  {0., 0., 1., 0.},
  {0., 0., 0., 1.},
};

static GLboolean transientOpen = GL_FALSE;

/*----------------------------------------------------------------------*/
/*  Mode Ajout              */
/*----------------------------------------------------------------------*/

int EXPORT call_togl_begin_ajout_mode (
                                       CALL_DEF_VIEW *aview
                                       )
{
  CMN_KEY_DATA data;

  TsmGetWSAttri (aview->WsId, WSWindow, &data);
#ifdef IMP260601
  if (TxglWinset (call_thedisplay, (WINDOW) data.ldata) != TSuccess) return (0);
#else
  if (TxglWinset (call_thedisplay, (WINDOW) data.ldata) == TSuccess)
#endif

    if (aview->ViewId == -1) return (0);

  openglWsId = aview->WsId;
  openglViewId = aview->ViewId;

  /* mise en place des matrices de projection et de mapping */
  /* TelSetViewIndex (aview->WsId, aview->ViewId); */

#ifdef TRACE
  printf ("call_togl_begin_ajout_mode %d %d\n", openglViewId, openglWsId);
#endif

  TelMakeFrontAndBackBufCurrent (aview->WsId);

  TsmPushAttri();

  return (1);
}

/*----------------------------------------------------------------------*/
void EXPORT call_togl_end_ajout_mode ()
{

#ifdef TRACE
  printf ("call_togl_end_ajout_mode %d %d\n", openglViewId, openglWsId);
#endif

  if (openglViewId != 0) {
    TelMakeBackBufCurrent (openglWsId);
  }

  openglViewId = 0;
  openglWsId = 0;

  identity = 1;

  /* FMN necessaire pour l'affichage sur WNT */
  glFlush();

  TsmPopAttri();

}


/*----------------------------------------------------------------------*/
/*  Mode Transient              */
/*----------------------------------------------------------------------*/

void EXPORT call_togl_clear_immediat_mode (
  CALL_DEF_VIEW *aview, Tint aFlush
  )
{
  CMN_KEY_DATA   data;
  GLuint listIndex = 0;
  GLuint retainmode = 0;

  if( transientOpen ) {
    call_togl_end_immediat_mode(0);
  } 

#ifdef IMP260601
  TsmGetWSAttri (aview->WsId, WSWindow, &data);
  if (TxglWinset (call_thedisplay, (WINDOW) data.ldata) != TSuccess) return;
#endif
  TsmGetWSAttri( aview->WsId, WSRetainMode, &data );
  retainmode = data.ldata;

#ifdef TRACE
  printf("call_togl_clear_immediat_mode. xm %f, ym %f, zm %f, XM %f, YM %f, ZM %f, partial %d retainmode %d\n",xm, ym, zm, XM, YM, ZM, partial, retainmode);
#endif

  if (  TelBackBufferRestored () == TOff ) {
    call_togl_erase_animation_mode();
    call_func_redraw_all_structs_begin (aview->WsId);
    if( aview->ptrUnderLayer)
      call_togl_redraw_layer2d (aview, (CALL_DEF_LAYER *)aview->ptrUnderLayer);
    call_func_redraw_all_structs_proc (aview->WsId);
    if( aview->ptrOverLayer)
      call_togl_redraw_layer2d (aview, (CALL_DEF_LAYER *)aview->ptrOverLayer);
#ifdef RIC120302
    call_subr_displayCB(aview,OCC_REDRAW_WINDOW);
#endif
    call_func_redraw_all_structs_end (aview->WsId, aFlush);
    // After a redraw,
    // Made the back identical to the front buffer.
    // Always perform full copy (partial update optimization is useless on mordern hardware)!
    if (retainmode)
      TelCopyBuffers (aview->WsId, GL_FRONT, GL_BACK, xm, ym, zm, XM, YM, ZM, 0);
#ifdef TRACE
    printf(" $$$ REDRAW\n");
#endif
    TelSetBackBufferRestored (TOn);
  } else if( partial >= 0 ) {
    // Restore pixels from the back buffer.
    // Always perform full copy (partial update optimization is useless on mordern hardware)!
    TelCopyBuffers (aview->WsId, GL_BACK, GL_FRONT, xm, ym, zm, XM, YM, ZM, 0);
  }

  TsmGetWSAttri (aview->WsId, WSTransient, &data);
  listIndex = (GLuint) data.ldata;
  if( listIndex != 0 ) {  /* Clear current list contents */
    glNewList( listIndex, GL_COMPILE_AND_EXECUTE);   
    glEndList();
  }
  partial = -1;
  XM = YM = ZM = (float ) shortrealfirst ();
  xm = ym = zm = (float ) shortreallast ();
}

/*----------------------------------------------------------------------*/

void call_togl_redraw_immediat_mode(
                                    CALL_DEF_VIEW * aview
                                    )
{
  CMN_KEY_DATA   data;
  GLuint listIndex = 0;
  int retainmode = 0;
#ifndef IMP150501
  GLboolean flag_zbuffer = GL_FALSE;
#endif

  TsmGetWSAttri( aview->WsId, WSRetainMode, &data );
  retainmode = data.ldata;

  TsmGetWSAttri (aview->WsId, WSTransient, &data);
  listIndex = (GLuint) data.ldata;

  if (retainmode != 0) {
    if (listIndex != 0) {
      TelMakeFrontBufCurrent (aview->WsId);

      LightOff();
      /* mise en place des matrices de projection et de mapping */
      /* TelSetViewIndex (aview->WsId, aview->ViewId);  */

#ifdef TRACE
      printf("call_togl_redraw_immediat_mode::call displaylist(%d) \n",
        listIndex);
#endif
#ifdef IMP150501
      glCallList(listIndex); 
#else
      flag_zbuffer = glIsEnabled(GL_DEPTH_TEST);
      if (flag_zbuffer) glDisable(GL_DEPTH_TEST);
      glCallList(listIndex); 
      if (flag_zbuffer) glEnable(GL_DEPTH_TEST); 
#endif
      /* FMN necessaire pour l'affichage sur WNT */
      glFlush();

      TelMakeBackBufCurrent (aview->WsId);
    } 
  }
}


/*----------------------------------------------------------------------*/

int EXPORT
call_togl_begin_immediat_mode
(
 CALL_DEF_VIEW * aview,
 CALL_DEF_LAYER * anunderlayer,
 CALL_DEF_LAYER * anoverlayer,
#ifdef IMP150501
 int zbuffer, 
#else
 int doublebuffer,
#endif
 int retainmode
 )
{
  CMN_KEY_DATA data;
  GLuint listIndex = 0;

  if (aview->ViewId == -1) return (0);

  TsmGetWSAttri (aview->WsId, WSWindow, &data);
#ifdef IMP260601
  if (TxglWinset (call_thedisplay, (WINDOW) data.ldata) != TSuccess) return (0);
#else
  if (TxglWinset (call_thedisplay, (WINDOW) data.ldata) == TSuccess)
#endif

    openglWsId = aview->WsId;
  openglViewId = aview->ViewId;

  data.ldata = retainmode;
  TsmSetWSAttri( aview->WsId, WSRetainMode, &data );

#ifdef TRACE
  printf ("call_togl_begin_immediat_mode %d %d RetainMode %d zbuffer %d partial %d\n", openglViewId, openglWsId, retainmode, zbuffer, partial);
#endif

  call_togl_clear_immediat_mode(aview,1);

  TelMakeFrontBufCurrent (aview->WsId);

  TsmPushAttri();

#ifdef COPY
  if (partial == 1) {
    glColor3f (1., 0., 0.);
    glBegin(GL_LINE_LOOP);
    glVertex3f (xm, ym, zm);
    glVertex3f (XM, ym, zm);
    glVertex3f (XM, YM, zm);
    glVertex3f (xm, YM, zm);
    glEnd();
  }
#endif

  /* 
  * RetainMode
  */
  if( retainmode ) {
    TsmGetWSAttri (aview->WsId, WSTransient, &data);
    listIndex = (GLuint) data.ldata;

    if (listIndex == 0) {
      listIndex = glGenLists(1);
#ifdef TRACE
      printf("call_togl_begin_immediat_mode::init displaylist() %d \n", listIndex);
#endif
      data.ldata = listIndex;
      TsmSetWSAttri( aview->WsId, WSTransient, &data );
    }
    if (listIndex == 0) return(0);    
#ifdef TRACE
    printf("call_togl_begin_immediat_mode::glNewList() %d \n", listIndex);
#endif
    glNewList(listIndex, GL_COMPILE_AND_EXECUTE);   
    transientOpen = GL_TRUE;
  }

#ifdef IMP150501
  if( zbuffer ) {
    glEnable(GL_DEPTH_TEST);
  } else glDisable(GL_DEPTH_TEST);
#endif

  return (1);
}


/*----------------------------------------------------------------------*/
/*
call_togl_end_immediat_mode (synchronize)
GLboolean synchronize

End of immediate graphic primitives in the view.
*/

void EXPORT
call_togl_end_immediat_mode
(
 int synchronize
 )
{

#ifdef TRACE
  printf ("call_togl_end_immediat_mode %d %d %d\n", 
    synchronize, openglViewId, openglWsId);
#endif

#ifndef BUC60863  /* Don't modify the current minmax !!*/
  /* Mise ajout MinMax */
  if ((xm > shortreallast ()) ||
    (ym > shortreallast ()) ||
    (zm > shortreallast ()) ||
    (XM > shortreallast ()) ||
    (YM > shortreallast ()) ||
    (ZM > shortreallast ()) ||
    (xm < shortrealfirst ()) ||
    (ym < shortrealfirst ()) ||
    (zm < shortrealfirst ()) ||
    (XM < shortrealfirst ()) ||
    (YM < shortrealfirst ()) ||
    (ZM < shortrealfirst ())) {
      XM = YM = ZM = (float ) shortreallast ();
      xm = ym = zm = (float ) shortrealfirst ();
      partial = 0;
    }
  else {
    if (partial != 0) partial = 1;
  }
#endif  /*BUC60863*/

  if (openglViewId != 0) {
#ifdef COPY
    if (partial == 1) {
      glColor3f (0., 1., 0.);
      glBegin(GL_LINE_LOOP);
      glVertex3f (xm, ym, zm);
      glVertex3f (XM, ym, zm);
      glVertex3f (XM, YM, zm);
      glVertex3f (xm, YM, zm);
      glEnd();
    }
#endif

    if (transientOpen) {
      glEndList();
#ifdef TRACE
      printf("--->glEndList() \n");
#endif
      transientOpen = GL_FALSE;
    }

    TelMakeBackBufCurrent (openglWsId);     
  }

  /*
  * Ajout CAL : pour voir quelque chose
  * avant le prochain begin_immediat_mode
  */
  glFinish ();

  openglViewId = 0;
  openglWsId = 0;

  identity = 1;

  TsmPopAttri();

}

/*----------------------------------------------------------------------*/
/*
call_togl_transform (amatrix, mode)
float amatrix[4][4]
GLboolean mode

Set the local transformation for All primitives
REPLACE the existante if mode is True or POSTCONCATENATE if GL_FALSE
*/
void EXPORT
call_togl_transform ( float amatrix[4][4], int mode )
{
  int i, j;
  if (mode || identity) {
    for (i=0; i<4; i++)
      for (j=0; j<4; j++)
        openglMat[i][j] = amatrix[i][j];
  } else
    TelMultiplymat3 (openglMat, openglMat, amatrix);

  identity = call_util_is_identity ();
}


/*
call_togl_begin_polyline ()

Sets the graphic library ready to receive a polyline.
*/

void EXPORT
call_togl_begin_polyline ()
{
  openglNumberOfPoints  = 0;
  LightOff();
  glBegin (GL_LINE_STRIP);
}


/*----------------------------------------------------------------------*/
/*
call_togl_end_polyline ()

End of the polyline.
*/

void EXPORT
call_togl_end_polyline ()
{

#ifdef TRACE
  printf ("call_togl_end_polyline %d %d\n", openglViewId, openglWsId);
#endif

  if (openglViewId != 0) glEnd ();
}


/*----------------------------------------------------------------------*/
/*
call_togl_draw (x, y, z)
float x;
float y;
float z;

Defines a new point in the current primitive.
*/

void EXPORT
call_togl_draw
(
 float x,
 float y,
 float z
 )
{

#ifdef TRACE
  printf ("call_togl_draw %d %d\n", openglViewId, openglWsId);
#endif

  if (openglViewId != 0) {
    openglNumberOfPoints ++;
    call_util_transform_pt (&x, &y, &z);
    if (x > XM) XM = x;
    if (y > YM) YM = y;
    if (z > ZM) ZM = z;
    if (x < xm) xm = x;
    if (y < ym) ym = y;
    if (z < zm) zm = z;
    glVertex3f (x, y, z);
    partial = 1;
  }
}


/*----------------------------------------------------------------------*/
/*
call_togl_move (x, y, z)
float x;
float y;
float z;

Defines a new point in the current primitive.
*/

void EXPORT
call_togl_move
(
 float x,
 float y,
 float z
 )
{

#ifdef TRACE
  printf ("call_togl_move %d %d\n", openglViewId, openglWsId);
#endif

  if (openglViewId != 0) {
    if (openglNumberOfPoints != 0) {
      call_togl_end_polyline ();
      call_togl_begin_polyline ();
    }

    openglNumberOfPoints ++;
    call_util_transform_pt (&x, &y, &z);
    if (x > XM) XM = x;
    if (y > YM) YM = y;
    if (z > ZM) ZM = z;
    if (x < xm) xm = x;
    if (y < ym) ym = y;
    if (z < zm) zm = z;
    glVertex3f (x, y, z);
    partial = 1;
  }
}


/*----------------------------------------------------------------------*/
/*
call_togl_set_linecolor (r, g, b)
float r;
float g;
float b;

Sets a new color in the current primitive.
*/

void EXPORT
call_togl_set_linecolor
(
 float r,
 float g,
 float b
 )
{

#ifdef TRACE
  printf ("call_togl_set_linecolor %d %d\n", openglViewId, openglWsId);
#endif

  if (openglViewId != 0) {
    TEL_COLOUR color;
    CMN_KEY key;
    key.id = TelPolylineColour;
    color.rgb[0] = r; color.rgb[1] = g; color.rgb[2] = b;
    key.data.pdata = &color;
    TsmSetAttri( 1, &key );
  }
}


/*----------------------------------------------------------------------*/
/*
call_togl_set_linetype (type)
long type;

Sets a new line type in the current primitive.
*/

void EXPORT
call_togl_set_linetype
(
 long type
 )
{
#ifdef TRACE
  printf ("call_togl_set_linetype %d %d\n", openglViewId, openglWsId);
#endif

  if (openglViewId != 0) {
    CMN_KEY key;
    key.id = TelPolylineType;
    key.data.ldata = type;
    TsmSetAttri( 1, &key);
  }
}


/*----------------------------------------------------------------------*/
/*
call_togl_set_linewidth (width)
float width;

Sets a new line width in the current primitive.
*/

void EXPORT
call_togl_set_linewidth
(
 float width
 )
{

#ifdef TRACE
  printf ("call_togl_set_linewidth %d %d\n", openglViewId, openglWsId);
#endif

  if (openglViewId != 0) {
    CMN_KEY key;
    key.id = TelPolylineWidth;
    key.data.fdata = width;
    TsmSetAttri( 1, &key);
  }
}


/*----------------------------------------------------------------------*/
/*
call_togl_draw_structure (aStructureId)
int aStructureId

Draw a structure in the transient view space
*/

void EXPORT
call_togl_draw_structure
(
 int aStructureId
 )
{
  Tint    i, num;
  tsm_node node;
  register Tint istrsfed = 0;
  register Tint display = 1;
  register Tint highl = TOff;
  register TelType  telem;
  CMN_KEY   key;

  float mat16[16];
  GLint mode1;
  GLint mode2;

#ifdef TRACE
  printf ("call_togl_draw_structure %d %d %d\n", openglViewId, openglWsId);
#endif

  if (openglViewId == 0) return;

  if (TsmGetStructure (aStructureId, &num, &node) == TFailure || !num) return;

  /*transform_persistence_end();*/

  TsmPushAttriLight (); 
  TsmPushAttri();
  TglNamesetPush();

  /* mise en place de la matrice de transformation du trace transient */
  if (! identity) {
    call_util_transpose_mat (mat16, openglMat);
    glGetIntegerv (GL_MATRIX_MODE, &mode1);
    glMatrixMode (GL_MODELVIEW);
    glPushMatrix ();
    glScalef (1., 1., 1.);
    glMultMatrixf (mat16);
  }

  for (i = 0; i < num; i++, node = node->next) {
    telem = node->elem.el;
#if TRACE > 0
    TelTrace(telem,node->elem.data.ldata);
#endif
    switch (telem) {
      case TelAddNameset:
      case TelRemoveNameset: {
        TsmSendMessage (telem, DisplayTraverse, node->elem.data, 0);

        if( TglFilterNameset( openglWsId, InvisFilter ) == TSuccess )
          display = 0;

        if( TglFilterNameset( openglWsId, HighlFilter ) == TSuccess )
          highl = TOn;

        break;
                             }

      case TelCurve:
      case TelMarker:
      case TelMarkerSet:
      case TelPolyline:
      case TelText: {
        /* do not send Display message to */
        /* primitives if they are invisible */
        if (display) 
        {
          LightOff();
#ifdef IMP150501
          glDepthMask(GL_FALSE);
#endif
          key.id = highl;
          TsmSendMessage (telem, DisplayTraverse, node->elem.data, 1, &key);
        }
                    }
                    break;

      case TelPolygon:
      case TelPolygonSet:
      case TelPolygonHoles:
      case TelPolygonIndices:
      case TelQuadrangle:
      case TelParray:  
      case TelTriangleMesh: {
        /* do not send Display message to */
        /* primitives if they are invisible */
        if (display) 
        {
          /* LightOn(); */
#ifdef IMP150501
          glDepthMask(GL_FALSE);
#endif
#ifdef IMP051001
          if( highl ) {
            call_subr_disable_polygon_offset();
          }
#endif
          key.id = highl;
          TsmSendMessage (telem, DisplayTraverse, node->elem.data, 1, &key);
#ifdef IMP051001
          if( highl ) {
            call_subr_enable_polygon_offset();
          }
#endif
        }
                            }
                            break;

      case TelLocalTran3: {
        tel_matrix3_data  d = ((tel_matrix3_data)(node->elem.data.pdata));
        istrsfed = 1;
        call_util_transpose_mat (mat16, d->mat);
        glGetIntegerv (GL_MATRIX_MODE, &mode2);
        glMatrixMode (GL_MODELVIEW);
        glPushMatrix ();
        glScalef (1., 1., 1.);
        glMultMatrixf (mat16);
                          }
                          break;
                          /* ABD 29/10/04  Transform Persistence of Presentation( pan, zoom, rotate ) */
      case TelTransformPersistence:
        {
          key.id = openglWsId;
          TsmSendMessage (telem, DisplayTraverse, node->elem.data, 1, &key);
          break;
        }
        /*ABD 29/10/04  Transform Persistence of Presentation( pan, zoom, rotate ) */
      default: {
        key.id = openglWsId;
        TsmSendMessage (telem, DisplayTraverse, node->elem.data, 0, &key);
               }
    }
  }

  TglNamesetPop();
  TsmPopAttri();
  TsmPopAttriLight (); 

  if (istrsfed) {
    glPopMatrix ();
    glMatrixMode (mode2);
  }
  if (! identity) {
    glPopMatrix ();
    glMatrixMode (mode1);
  }

  return;
}

/*----------------------------------------------------------------------*/
/*
call_togl_set_minmax (x1, y1, z1, x2, y2, z2)
float x1;
float y1;
float z1;
float x2;
float y2;
float z2;

Give the boundary box of the transient graphic
*/

void EXPORT
call_togl_set_minmax (
                      float x1,
                      float y1,
                      float z1,
                      float x2,
                      float y2,
                      float z2
                      )
{
#ifdef TRACE
  printf("call_togl_set_minmax. x1 %f, y1 %f, z1 %f, x2 %f, y2 %f, z2 %f\n",x1, y1, z1, x2, y2, z2);
#endif
  if ((x1 > shortreallast ()) ||
    (y1 > shortreallast ()) ||
    (z1 > shortreallast ()) ||
    (x2 > shortreallast ()) ||
    (y2 > shortreallast ()) ||
    (z2 > shortreallast ()) ||
    (x1 < shortrealfirst ()) ||
    (y1 < shortrealfirst ()) ||
    (z1 < shortrealfirst ()) ||
    (x2 < shortrealfirst ()) ||
    (y2 < shortrealfirst ()) ||
    (z2 < shortrealfirst ())) {
      XM = YM = ZM = (float ) shortreallast ();
      xm = ym = zm = (float ) shortrealfirst ();
      partial = 0;
    }
  else {
    call_util_transform_pt (&x1, &y1, &z1);
    call_util_transform_pt (&x2, &y2, &z2);
    if (x1 > XM) XM = x1;
    if (x1 < xm) xm = x1;
    if (y1 > YM) YM = y1;
    if (y1 < ym) ym = y1;
    if (z1 > ZM) ZM = z1;
    if (z1 < zm) zm = z1;

    if (x2 > XM) XM = x2;
    if (x2 < xm) xm = x2;
    if (y2 > YM) YM = y2;
    if (y2 < ym) ym = y2;
    if (z2 > ZM) ZM = z2;
    if (z2 < zm) zm = z2;
    if (partial != 0) partial = 1;
  }
}

/*----------------------------------------------------------------------*/
/*
* Private functions
*/

/*----------------------------------------------------------------------*/
/*
call_util_transform_pt (x, y, z)
float *x;
float *y;
float *z;

Transform the point pt
*/

static void call_util_transform_pt
(
 float *x,
 float *y,
 float *z
 )
{
  int i, j;
  float sum = 0.;
  float tpt[4], pt[4];

  if (! identity) {
    pt[0] = *x, pt[1] = *y, pt[2] = *z, pt[3] = 1.0;

    for (i = 0; i < 4; i++) {
      for (j = 0, sum = 0.0; j < 4; j++)
        sum += openglMat[i][j] * pt[j];
      tpt[i] = sum;
    }

    *x = tpt[0], *y = tpt[1], *z = tpt[2];
  }
}


/*----------------------------------------------------------------------*/
/*
int call_util_is_identity ()

Returns 1 if openglMat is the identity
*/

static int call_util_is_identity
(
 )
{
  int i, j;
  int res = 1;

  for (i = 0; res && i < 4; i++)
    for (j = 0; res && j < 4; j++)
      if (i == j) res = (openglMat[i][j] == 1.);
      else        res = (openglMat[i][j] == 0.);

#ifdef TRACE
      printf ("La matrice suivante :\n");
      for (i = 0; i < 4; i++) {
        printf ("\t");
        for (j = 0; j < 4; j++)
          printf ("%f ", openglMat[i][j]);
        printf ("\n");
      }
      if (res)
        printf ("est la matrice identite\n");
      else
        printf ("n'est pas la matrice identite\n");
#endif

      return (res);
}


/*----------------------------------------------------------------------*/
/*
void call_util_transpose_mat (tmat, mat)
float tmat[16];
float mat[4][4];

Transpose mat and returns tmat.
*/

static void call_util_transpose_mat (float tmat[16], float mat[4][4]) {
  int i, j;

  for (i=0; i<4; i++)
    for (j=0; j<4; j++)
      tmat[j*4+i] = mat[i][j];

#ifdef TRACE
  printf ("Transposee :\n");
  for (i = 0; i < 4; i++) {
    printf ("\t");
    for (j = 0; j < 4; j++)
      printf ("%f ", tmat[i*4+j]);
    printf ("\n");
  }
#endif
}
