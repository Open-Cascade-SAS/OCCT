/***********************************************************************

FONCTION :
----------
File OpenGl_subrvis :

TEST :
------

Le main TestOfDesktop.c permet de tester cette classe.


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : xxx ; Creation.
07-02-96 : FMN ; Suppression code inutile:
- TxglLink() et TxglUnlink()
19-03-96 : CAL ; Ajout de la fonction subr_resize.
appele par togl_ratio_window
21-03-96 : CAL ; Dans subr_doublebuffer et subr_open_ws
retrait des lignes qui donnaient un background
noir a la fenetre.
01-04-96 : CAL ; Integration MINSK portage WNT
02-07-96 : FMN ; Suppression WSWSHeight, WSWSWidth et WSType
17-07-96 : FMN ; Modification de call_subr_hlhsr().
18-07-96 : FMN ; Ajout appel TxglWinset() pour le Resize.
01-08-96 : FMN ; Ajout appel reaffichage dans le resize.
25-11-96 : FMN ; PRO6577: Ajout WSUpdateState pour call_subr_set_background.
29-01-97 : FMN ; Suppression de call_subr_doublebuffer().
05-02-97 : FMN ; Suppression variables inutiles.
12-02-97 : FMN ; Suppression de call_facilities_list.DepthCueing
14-02-97 : FMN ; Ajout WSUpdateState pour call_subr_transparency.
30-06-97 : FMN ; Appel de la toolkit OpenGl_LightBox.
02-07-97 : FMN ; Ajout init generale par contexte.
(GL_POLYGON_OFFSET_EXT,  glLightModelf,  glHint).
03-07-97 : FMN ; Test Headlight.
08-09-97 : CAL ; subr_open_ws retourne 0 si pb. (gere par togl_view)
07-10-97 : FMN ; Simplification WNT
03-10-97 : FMN ; Meilleure gestion des EXT,  ajout QueryExtension
17-11-97 : FMN ; Suppression redraw sur le resize.
05-12-97 : FMN ; Double gestion du glPolygonOffsetEXT
17-12-97 : FMN ; Probleme compilation SGI
03-03-98 : FMN ; Simplification gestion du glPolygonOffsetEXT 
15-03-98 : FMN ; Suppression variable WSNeedsClear
17-03-98 : FMN ; Ajout mode animation
08-07-98 : FMN ; Changement du nom de la primitive call_togl_erase_immediat_mode()
-> call_togl_erase_animation_mode.
22-10-98 : FMN ; Amelioration des perfs: glHint utilise GL_FASTEST.
14-12-98 : BGN; Developpement (S3989, Phase "triedre")
29-03-01 : GG; glPolygonOffsetEXT is no more available
since OGL 1.2
20-08-01 : GG  ; Add functions  call_subr_enable_polygon_offset ()
and call_subr_disable_polygon_offset ()

************************************************************************/

#ifdef WNT
#define BUC60579        /*GG_240999 Under WNT,resets the ws attributes for each
//                        created view-manager and not only for the first
*/
#endif

#define IMP190100 /*GG Reverse the front & back depth cueing planes
//      position.
*/

#define RIC120302 /*GG Use TxglSetWindow instead TxglCreateWindow
//      when a GLXContext is given
//      Add call_subr_displayCB function
*/

#define  BUC61044    /* 25/10/01 SAV ; added functionality to control gl depth testing
from higher API */
#define  BUC61045    /* 25/10/01 SAV ; added functionality to control gl lighting 
from higher API */

#define OCC1188         /*SAV added control of the background image*/

/*----------------------------------------------------------------------*/
/*
* Includes
*/

#include <stdio.h>

#include <OpenGl_tgl_all.hxx>


#include <InterfaceGraphic.hxx>
#include <InterfaceGraphic_Labels.hxx>
#include <InterfaceGraphic_Graphic3d.hxx>
#include <InterfaceGraphic_Visual3d.hxx>

#include <OpenGl_inquire.hxx>

#include <OpenGl_Extension.hxx> 

#include <OpenGl_cmn_varargs.hxx>
#include <OpenGl_tsm.hxx>
#include <OpenGl_tsm_ws.hxx>
#include <OpenGl_telem_view.hxx>
#include <OpenGl_telem_util.hxx>
#include <OpenGl_telem_pick.hxx>
#include <OpenGl_LightBox.hxx>
#include <OpenGl_telem_highlight.hxx>
#include <OpenGl_telem_depthcue.hxx>
#include <OpenGl_txgl.hxx>
#include <OpenGl_tgl_subrvis.hxx>
#include <OpenGl_tgl_pick.hxx>
#include <OpenGl_tgl_funcs.hxx>
#include <OpenGl_telem_filters.hxx>
#include <OpenGl_tgl_tox.hxx>
#include <OpenGl_tgl.hxx>
#include <OpenGl_animation.hxx>
#include <OpenGl_triedron.hxx>
#include <OpenGl_tgl_util.hxx>
#include <OpenGl_Memory.hxx>
#include <OpenGl_graduatedtrihedron.hxx>
#include <OpenGl_ResourceCleaner.hxx>
#include <OpenGl_ResourceTexture.hxx>

/*----------------------------------------------------------------------*/
/*
* Constantes
*/

static Tint   attri_init = 0;

/*----------------------------------------------------------------------*/

#ifndef WNT
static Bool WaitForNotify(Display *d, XEvent *e, char *arg) {
  return (e->type == MapNotify) && (e->xmap.window == (Window)arg);
}
#endif

/*----------------------------------------------------------------------*/

static int offsetStatus=0;
#ifdef GL_EXT_polygon_offset
static GLfloat offsetFactor=1.0,offsetUnits=0.0;
#elif GL_POLYGON_OFFSET_EXT
static GLfloat offsetFactor=0.5,offsetUnits=0.0005;
#endif
void call_subr_enable_polygon_offset( )
{
  if( offsetStatus < 0 ) return;

#ifdef GL_EXT_polygon_offset
  if( offsetStatus == 0 ) {
    if (QueryExtension("GL_EXT_polygon_offset")) {
      char svalue[64];
      if( call_util_osd_getenv ("CALL_OPENGL_POLYGON_OFFSET",
        svalue,sizeof(svalue)) ) {
          float v1,v2;
          int n = sscanf(svalue,"%f %f",&v1,&v2);
          if( n > 0 ) offsetFactor = v1;
          if( n > 1 ) offsetUnits = v2;
          printf(" $$$ CALL_OPENGL_POLYGON_OFFSET %f %f\n",
            offsetFactor,offsetUnits);
        }
        offsetStatus = 1;
    } else {
      offsetStatus = -1;
    } 
  }

  if( offsetStatus > 0 ) {      
#ifdef GL_POLYGON_OFFSET_FILL
    glPolygonOffset(offsetFactor,offsetUnits);
    glEnable(GL_POLYGON_OFFSET_FILL);
#elif GL_POLYGON_OFFSET_EXT
    glPolygonOffsetEXT(offsetFactor,offsetUnits);
    glEnable(GL_POLYGON_OFFSET_EXT);
#endif
  }
#else
  offsetStatus = -1;
#endif /* GL_EXT_polygon_offset */
}

void
call_subr_disable_polygon_offset( )
{
  if( offsetStatus > 0 ) {      
#ifdef GL_POLYGON_OFFSET_FILL
    glDisable(GL_POLYGON_OFFSET_FILL);
#elif GL_POLYGON_OFFSET_EXT
    glDisable(GL_POLYGON_OFFSET_EXT);
#endif
  }
}

/*----------------------------------------------------------------------*/

int
call_subr_open_ws( CALL_DEF_VIEW * aview )
{
  CMN_KEY_DATA   data;
  WINDOW         win;
  TEL_HIGHLIGHT  hrep = { TelHLForcedColour, {{ ( float )1.0, ( float )1.0, ( float )1.0 }} };
  TEL_DEPTHCUE   dcrep = { TelDCSuppressed, {( float )0.0, ( float )1.0}, {( float )1.0, ( float )0.0}, {{( float )0.0, ( float )0.0, ( float )0.0}}                            };
  Tint           width, height;
  Tfloat         bgcolr, bgcolg, bgcolb; /* background color */
#ifdef OCC1188
  TSM_BG_TEXTURE bgTexture;   
#endif

#ifndef WNT
  XEvent         event;
  Window         root;
  int            newx, newy;
  unsigned int   newwidth, newheight, newbw, newdepth;
#else
  RECT           cr;
#endif  /* WNT */

  width  = ( Tint )aview->DefWindow.dx;
  height = ( Tint )aview->DefWindow.dy;

  /* background color */
  bgcolr = aview->DefWindow.Background.r,
    bgcolg = aview->DefWindow.Background.g,
    bgcolb = aview->DefWindow.Background.b;

#ifdef RIC120302
  if( aview->GContext ) 
    win = TxglSetWindow( call_thedisplay, /*(HWND)*/(WINDOW)aview->DefWindow.XWindow,
    /*(HGLRC)*/(GLCONTEXT)aview->GContext );
  else
#endif /*RIC120302*/
  win = TxglCreateWindow( call_thedisplay, /*(HWND)*/(WINDOW)aview->DefWindow.XWindow,
  0, 0, width, height, 0, bgcolr, bgcolg, bgcolb );
#ifndef WNT
  if( win != aview->DefWindow.XWindow ) {
    XMapWindow( call_thedisplay, win );
    XIfEvent(call_thedisplay, &event, WaitForNotify, (char *) win);
  }
#endif  /* WNT */

  if( TxglWinset( call_thedisplay, (WINDOW) win ) == TFailure )
    return 0;
  else {
    TsmRegisterWsid( aview->WsId );
    data.ldata = (Tint) win;
    TsmSetWSAttri( aview->WsId, WSWindow, &data );
#ifndef WNT
    XGetGeometry (
      call_thedisplay, win, &root,
      &newx, &newy,&newwidth, &newheight, &newbw, &newdepth
      );
    width  = newwidth;
    height = newheight;
#else
    GetClientRect ( win, &cr );
    width  = cr.right;
    height = cr.bottom;
#endif  /* WNT */
#ifdef OCC1188
    bgTexture.texId = 0;
    data.pdata = &bgTexture;
    TsmSetWSAttri( aview->WsId, WSBgTexture, &data );
#endif
    data.ldata = width;
    TsmSetWSAttri( aview->WsId, WSWidth, &data );
    data.ldata = height;
    TsmSetWSAttri( aview->WsId, WSHeight, &data );
    data.ldata = -1;
    TsmSetWSAttri( aview->WsId, WSViewStid, &data );
    data.ldata = TOn;
    TsmSetWSAttri( aview->WsId, WSDbuff, &data );
    data.ldata = TNotDone;
    TsmSetWSAttri( aview->WsId, WSUpdateState, &data );
    data.ldata = TOff;
    TsmSetWSAttri( aview->WsId, WSTransparency, &data );
    TelSetHighlightRep( aview->WsId, 0, &hrep );
    TelSetDepthCueRep( aview->WsId, 0, &dcrep );
    TelInitWS( aview->WsId, width, height, bgcolr, bgcolg, bgcolb );

#ifdef BUC61044
    /* 24/10/01 : SAV ; setting OGL depth testing enabled by default */
    data.ldata = TOn;
    TsmSetWSAttri( aview->WsId, WSDepthTest, &data );
#endif
#ifdef BUC61045
    /* 24/10/01 : SAV ; setting flag to control GL_LIGHTING (used in triedron) */
    data.ldata = TOn;
    TsmSetWSAttri( aview->WsId, WSGLLight, &data );
#endif

#ifdef BUC60579 
    TsmInitAttributes();
#else
    if( !attri_init ) {
      TsmInitAttributes();
      attri_init = 1;
    }
#endif

    /* 
    * Init generale du contexte 
    */

    /* Eviter d'avoir les faces mal orientees en noir. */
    /* Pourrait etre utiliser pour detecter les problemes d'orientation */ 
    glLightModeli((GLenum)GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE); 

    /* Optimisation pour le Fog et l'antialiasing */
    glHint( GL_FOG_HINT, GL_FASTEST );    
    glHint( GL_POINT_SMOOTH_HINT, GL_FASTEST );    
    glHint( GL_LINE_SMOOTH_HINT, GL_FASTEST );    
    glHint( GL_POLYGON_SMOOTH_HINT, GL_FASTEST );  

    /* Polygon Offset */
    call_subr_enable_polygon_offset();

    return 1;

  } /* TxglWinset */
}

/*----------------------------------------------------------------------*/

void
call_subr_resize (CALL_DEF_VIEW * aview)
{
  CMN_KEY_DATA    data;
  WINDOW      win;
  Tint      width, height;
  Tfloat      bgcolr, bgcolg, bgcolb;   
  Tint      wswidth, wsheight;
  Tint      wsviewstid;
  /*    Tint      swap = 1;*/     /* swap buffers ? yes */

#ifndef WNT
  /*    XEvent      event;*/
  Window      root;
  int       newx, newy;
  unsigned int    newwidth, newheight, newbw, newdepth;
#else
  RECT      cr;
#endif  /* WNT */

#ifdef TRACE
  printf("call_subr_resize \n");
#endif
  /* Recuperation de la definition precedente de la ws */
  TsmGetWSAttri (aview->WsId, WSWidth, &data);
  wswidth = data.ldata;
  TsmGetWSAttri (aview->WsId, WSHeight, &data);
  wsheight = data.ldata;
  TsmGetWSAttri (aview->WsId, WSViewStid, &data);
  wsviewstid = data.ldata;

  /* S'il s'agit du togl_ratio_window initial alors on ne fait rien */
  if (wsviewstid == -1) return;

  /*
  * Si l'on resize une fenetre qui a la meme taille que la taille
  * precedente alors on ne fait rien
  */
  if ((wswidth == (Tint) aview->DefWindow.dx) &&
    (wsheight == (Tint) aview->DefWindow.dy)) return;

#ifndef WNT
  TsmGetWSAttri (aview->WsId, WSWindow, &data);
  win = data.ldata;

  XResizeWindow (call_thedisplay, win,
    (unsigned int) aview->DefWindow.dx,
    (unsigned int) aview->DefWindow.dy);

  XSync (call_thedisplay, False);

  /* Mettre a jour ses attributs */
  XGetGeometry (call_thedisplay, win, &root,
    &newx, &newy,&newwidth, &newheight, &newbw, &newdepth);
  width  = newwidth;
  height = newheight;

  /* background color */
  bgcolr = aview->DefWindow.Background.r,
    bgcolg = aview->DefWindow.Background.g,
    bgcolb = aview->DefWindow.Background.b;

  /* Rendre courant le Drawable et le Context */
  if (TxglWinset (call_thedisplay, (Window) win) == TSuccess) {

    data.ldata = width;
    TsmSetWSAttri (aview->WsId, WSWidth, &data);
    data.ldata = height;
    TsmSetWSAttri (aview->WsId, WSHeight, &data);
    TelInitWS (aview->WsId, width, height, bgcolr, bgcolg, bgcolb);
  }

#else

  /* On detruit l'ancienne sous-fenetre de la window graphique */
  TsmGetWSAttri (aview->WsId, WSWindow, &data);
  win = (WINDOW) data.ldata;
  /*
  * On recree une nouvelle sous-fenetre de la window graphique
  * avec la nouvelle dimension
  */
  /* background color */
  bgcolr = aview->DefWindow.Background.r,
    bgcolg = aview->DefWindow.Background.g,
    bgcolb = aview->DefWindow.Background.b;
  /* Rendre courant le Drawable et le Context */
  if (TxglWinset (call_thedisplay, win) == TSuccess) {
    //if (TxglWinset (call_thedisplay, GET_GL_CONTEXT()) == TSuccess) {
    /* Mettre a jour la sous-fenetre associee */
    /* Mettre a jour ses attributs */
    GetClientRect ( win, &cr );
    width  = cr.right;
    height = cr.bottom;
    data.ldata = width;
    TsmSetWSAttri (aview->WsId, WSWidth, &data);
    data.ldata = height;
    TsmSetWSAttri (aview->WsId, WSHeight, &data);
    TelInitWS (aview->WsId, width, height, bgcolr, bgcolg, bgcolb);
  }
#endif /* WNT */

  return;
}

/*----------------------------------------------------------------------*/

void
call_subr_antialiasing( CALL_DEF_VIEW * aview, Tint tag )
{
  CMN_KEY       k;
  Tint          fl = tag ? TOn : TOff;

  call_func_set_edit_mode( TEditReplace );
  call_func_open_struct( aview->ViewId );
  call_func_set_elem_ptr(0);
  call_func_set_elem_ptr_label(View_LABAliasing);
  call_func_offset_elem_ptr(1);
  k.data.ldata = fl;
  TsmAddToStructure( TelAntiAliasing, 1, &k );
  call_func_close_struct();
  return;
}

/*----------------------------------------------------------------------*/

void
call_subr_hlhsr( CALL_DEF_VIEW * aview, int tag )
{
  CMN_KEY_DATA  data;

  data.ldata = (tag ? TOn : TOff);
  TsmSetWSAttri( aview->WsId, WSZbuffer, &data );
  return;
}


/*----------------------------------------------------------------------*/

void
call_subr_depthcueing( CALL_DEF_VIEW * aview, Tint tag )
{
  TEL_DEPTHCUE  rep;
#   define  CALL_DEF_DEPTH_CUEING_INDEX   1

  if( !tag )
  {
    call_func_set_edit_mode(CALL_PHIGS_EDIT_REPLACE);
    call_func_open_struct(aview->ViewId);
    call_func_set_elem_ptr(0);
    call_func_set_elem_ptr_label(View_LABDepthCueing);
    call_func_offset_elem_ptr(1);
    call_func_appl_data(0);
    call_func_close_struct();
  }
  else
  {
    rep.mode      = CALL_PHIGS_ALLOWED;
    rep.planes[0] = aview->Context.DepthBackPlane;
    rep.planes[1] = aview->Context.DepthFrontPlane;
#ifdef TRACE
    printf(" $$$ DepthFrontPlane %f DepthBackPlane %f FrontPlaneDistance %f BackPlaneDistance %f\n",aview->Context.DepthFrontPlane,aview->Context.DepthBackPlane,aview->Mapping.FrontPlaneDistance,aview->Mapping.BackPlaneDistance);
#endif
    rep.planes[0] =
#ifdef IMP190100
      (aview->Context.DepthFrontPlane -
      aview->Mapping.BackPlaneDistance)   /
#else
      (aview->Mapping.FrontPlaneDistance -
      aview->Context.DepthFrontPlane)   /
#endif
      (aview->Mapping.FrontPlaneDistance -
      aview->Mapping.BackPlaneDistance);

    rep.planes[1] =
#ifdef IMP190100
      (aview->Context.DepthBackPlane -
      aview->Mapping.BackPlaneDistance)   /
#else
      (aview->Mapping.FrontPlaneDistance -
      aview->Context.DepthBackPlane)    /
#endif
      (aview->Mapping.FrontPlaneDistance -
      aview->Mapping.BackPlaneDistance);

    if(rep.planes[0] < 0.)
      rep.planes[0] = ( float )0.0;
    else if(rep.planes[0] > 1.)
      rep.planes[0] = ( float )1.0;
    if(rep.planes[1] < 0.)
      rep.planes[1] = ( float )0.0;
    else if(rep.planes[1] > 1.)
      rep.planes[1] = ( float )1.0;
#ifdef IMP190100
    if(rep.planes[1] > rep.planes[0])
    {
      rep.planes[1]  = ( float )0.0;
      rep.planes[0]  = ( float )1.0;
    }
#else
    if(rep.planes[0] > rep.planes[1])
    {
      rep.planes[0]  = ( float )0.0;
      rep.planes[1]  = ( float )1.0;
    }
#endif

#ifdef TRACE
    printf(" plane[0] %f plane[1] %f\n",rep.planes[0],rep.planes[1]);
#endif
    rep.scales[0] = ( float )1.;
    rep.scales[1] = ( float )0.;
    rep.col.rgb[0] = aview->DefWindow.Background.r;
    rep.col.rgb[1] = aview->DefWindow.Background.g;
    rep.col.rgb[2] = aview->DefWindow.Background.b;
    TelSetDepthCueRep(aview->WsId, CALL_DEF_DEPTH_CUEING_INDEX, &rep);

    call_func_set_edit_mode(CALL_PHIGS_EDIT_REPLACE);
    call_func_open_struct(aview->ViewId);
    call_func_set_elem_ptr(0);
    call_func_set_elem_ptr_label(View_LABDepthCueing);
    call_func_offset_elem_ptr(1);
    call_func_dcue_ind(CALL_DEF_DEPTH_CUEING_INDEX);
    call_func_close_struct();
  }
  return;
}

/*----------------------------------------------------------------------*/

TStatus
call_subr_get_view_index( Tint stid, Tint *ind )
{
  tsm_node    node;
  Tint        i, num;

  if( TsmGetStructure( stid, &num, &node ) == TSuccess )
  {
    for( i = 0; i < num; i++ )
    {
      if( node->elem.el == TelViewIndex )
      {
        *ind = node->elem.data.ldata;
        return TSuccess;
      }
    }
  }
  return TFailure;
}

/*----------------------------------------------------------------------*/

void
call_subr_pick( CALL_DEF_PICK *apick )
{
  Tint        depth;
  TPickOrder  order;
  TStatus     stat;

  static      int    *listid, *listelem, *listpickid;

  TEL_PICK_PATH    path_list[CALL_MAX_DEPTH_LENGTH];
  TEL_PICK_REPORT  rep;

  Tint  i;

  if( listid )
  {
    //cmn_freemem( listid );
    delete[] listid;
    //cmn_freemem( listelem );
    delete[] listelem;
    //cmn_freemem( listpickid );
    delete[] listpickid;
    listid = listelem = listpickid = 0;
  }

  order = apick->Context.order == 0 ? PTOP_FIRST : PBOTTOM_FIRST;

  depth = apick->Context.depth < CALL_MAX_DEPTH_LENGTH ?
    apick->Context.depth : CALL_MAX_DEPTH_LENGTH ;

  rep.pick_path = path_list;

  stat = TPick( apick->WsId, apick->x, apick->y, apick->Context.aperture,
    apick->Context.aperture, order, depth, &rep );

  if( stat == TSuccess )
  {
    apick->Pick.depth = rep.depth;

#ifdef TRACE
    printf( "\n\n\tPICK REPORT FOLLOWS" );
    printf( "\n\t*******************\n" );
    printf( "\ndepth = %d\torder = %s\n", rep.depth,
      order == PTOP_FIRST ? "TopFirst" : "BottomFirst" );
#endif

    //cmn_memreserve( listid, rep.depth, 0 );
    listid = new int[rep.depth];
    //cmn_memreserve( listelem, rep.depth, 0 );
    listelem = new int[rep.depth];
    //cmn_memreserve( listpickid, rep.depth, 0 );
    listpickid = new int[rep.depth];
    if( !listid || !listelem || !listpickid )
      return;
    apick->Pick.listid     = listid;
    apick->Pick.listelem   = listelem;
    apick->Pick.listpickid = listpickid;

    for( i = 0; i < rep.depth; i++ )
    {
      listelem[i]   = path_list[i].el_num;
      listpickid[i] = path_list[i].pick_id;
      listid[i]     = path_list[i].struct_id;
#ifdef TRACE
      printf( "\n\tSTRUCTURE ID [%d] = %ld", i, listid[i] );
      printf( "\n\tPICK ID      [%d] = %ld", i, listpickid[i] );
      printf( "\n\tELEMENT ID   [%d] = %ld", i, listelem[i] );
#endif
    }
#ifdef TRACE
    printf("\n");
#endif
  }

  return;
}


/*----------------------------------------------------------------------*/

void
call_subr_set_light_src_rep( CALL_DEF_LIGHT * alight )
{
  TEL_LIGHT    rep;

  switch( alight->LightType )
  {
  case 0 : /* TOLS_AMBIENT */
    rep.type = CALL_PHIGS_LIGHT_AMBIENT;
    rep.col.rgb[0] = alight->Color.r;
    rep.col.rgb[1] = alight->Color.g;
    rep.col.rgb[2] = alight->Color.b;
    break;

  case 1 : /* TOLS_DIRECTIONAL */
    rep.type = CALL_PHIGS_LIGHT_DIRECTIONAL;
    rep.col.rgb[0] = alight->Color.r;
    rep.col.rgb[1] = alight->Color.g;
    rep.col.rgb[2] = alight->Color.b;
    rep.dir[0] = alight->Direction.x;
    rep.dir[1] = alight->Direction.y;
    rep.dir[2] = alight->Direction.z;
    break;

  case 2 : /* TOLS_POSITIONAL */
    rep.type = CALL_PHIGS_LIGHT_POSITIONAL;
    rep.col.rgb[0] = alight->Color.r;
    rep.col.rgb[1] = alight->Color.g;
    rep.col.rgb[2] = alight->Color.b;
    rep.pos[0] = alight->Position.x;
    rep.pos[1] = alight->Position.y;
    rep.pos[2] = alight->Position.z;
    rep.atten[0] = alight->Attenuation[0];
    rep.atten[1] = alight->Attenuation[1];
    break;

  case 3 : /* TOLS_SPOT */
    rep.type = CALL_PHIGS_LIGHT_SPOT;
    rep.col.rgb[0] = alight->Color.r;
    rep.col.rgb[1] = alight->Color.g;
    rep.col.rgb[2] = alight->Color.b;
    rep.pos[0] = alight->Position.x;
    rep.pos[1] = alight->Position.y;
    rep.pos[2] = alight->Position.z;
    rep.dir[0] = alight->Direction.x;
    rep.dir[1] = alight->Direction.y;
    rep.dir[2] = alight->Direction.z;
    rep.shine = alight->Concentration;
    rep.atten[0] = alight->Attenuation[0];
    rep.atten[1] = alight->Attenuation[1];
    rep.angle = alight->Angle;
    break;
  }

  rep.HeadLight = alight->Headlight;
  AddLight( alight->WsId, alight->LightId, &rep );

  return;
}

/*----------------------------------------------------------------------*/

void
call_subr_set_background( Tint ws, tel_colour rgb )
{
  CMN_KEY_DATA  data;
  CMN_KEY_DATA  key;

  /* Mise a jour de l'update_mode */
  key.ldata = TNotDone;
  TsmSetWSAttri( ws, WSUpdateState, &key );

  data.pdata = rgb->rgb;
  TsmSetWSAttri( ws, WSBackground, &data );
  return;
}

/*----------------------------------------------------------------------*/

void
call_subr_set_gradient_background( Tint ws, Tint type, tel_colour color1, tel_colour color2 )
{
  CMN_KEY_DATA  data;
  CMN_KEY_DATA  key;
  TSM_BG_GRADIENT bgGradient;

  /* Mise a jour de l'update_mode */
  key.ldata = TNotDone;
  TsmSetWSAttri( ws, WSUpdateState, &key );

  bgGradient.type = (TSM_GradientType)type;
  bgGradient.color1 = *color1;
  bgGradient.color2 = *color2;
  data.pdata = &bgGradient;
  TsmSetWSAttri( ws, WSBgGradient, &data );
  return;
}


/*----------------------------------------------------------------------*/

void
call_subr_close_ws( CALL_DEF_VIEW * aview )
{
  CMN_KEY_DATA key;
  CMN_KEY_DATA textureKey;

  TsmGetWSAttri( aview->WsId, WSWindow, &key );

#ifdef OCC1188
  tsm_bg_texture texture;
  WINDOW aWnd = (WINDOW) key.ldata;

  TsmGetWSAttri( aview->WsId, WSBgTexture, &textureKey );
  texture = (tsm_bg_texture)textureKey.pdata;

  if ( texture != 0 && texture->texId != 0 )
  {
    OpenGl_ResourceCleaner* anCleaner = OpenGl_ResourceCleaner::GetInstance();

    // Delete the texture with ResourceCleaner; if it is not possible
    // do this directly with proper context selected
    if ( !anCleaner->AddResource( TxglGetContext(aWnd), 
                   new OpenGl_ResourceTexture(texture->texId) ) )
    {
      GLCONTEXT  cur_context  = GET_GL_CONTEXT(); 
      GLDRAWABLE cur_drawable = GET_GLDEV_CONTEXT();

      if ( TxglWinset( call_thedisplay, aWnd ) == TSuccess )
        glDeleteTextures( 1,(GLuint *)&(texture->texId) ); 

      // activate the previous context for this thread
      GL_MAKE_CURRENT( call_thedisplay, cur_drawable, cur_context );
    }
  }
  
#endif

#ifdef RIC120302
  if( !aview->GContext )
#endif
    TxglDestroyWindow( call_thedisplay, (WINDOW) key.ldata );

  TglDeleteFiltersForWS( aview->WsId );
  RemoveWksLight( aview->WsId );
  call_triedron_erase (aview); /* BGN 09-12-98 */
  call_graduatedtrihedron_erase( aview->WsId );
  TelDeleteViewsForWS( aview->WsId );
  TelDeleteHighlightsForWS( aview->WsId );
  TelDeleteDepthCuesForWS( aview->WsId );

  TsmUnregisterWsid( aview->WsId );
  return;
}

/*----------------------------------------------------------------------*/

void
call_subr_transparency (int wsid, int viewid, int tag)
{
  CMN_KEY_DATA  data;
  CMN_KEY_DATA  key;

  /* Est-il necessaire de faire de la transparence ? */
  TsmGetWSAttri( wsid, WSTransparency, &data );
  if (data.ldata != tag)
  {
    data.ldata = (tag ? TOn : TOff);
    TsmSetWSAttri( wsid, WSTransparency, &data );

    /* Mise a jour de l'update_mode */
    key.ldata = TNotDone;
    TsmSetWSAttri( wsid, WSUpdateState, &key );
    (void) call_togl_erase_animation_mode();
  }
}

#ifdef RIC120302
/*----------------------------------------------------------------------*/
int
call_subr_displayCB( CALL_DEF_VIEW * aview, int reason )
{
  int status = 0;
  if( aview->GDisplayCB ) {
    Aspect_GraphicCallbackStruct callData;
    CMN_KEY_DATA data;
    WINDOW win;

    TsmGetWSAttri (aview->WsId, WSWindow, &data);
    win = (WINDOW) data.ldata;

    callData.reason = reason;
    callData.display = call_thedisplay;
    callData.window = win;
    callData.wsID = aview->WsId;
    callData.viewID = aview->ViewId;
    callData.gcontext = TxglGetContext(win);

    status = (*aview->GDisplayCB)
      ( aview->DefWindow.XWindow, aview->GClientData, &callData );
  }

  return status;
}
#endif
