/***********************************************************************

FONCTION :
----------
File OpenGl_execstruct :


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : xxx ; Creation.
11-03-96 : FMN ; Correction warning compilation
01-04-96 : CAL ; Integration MINSK portage WNT
22-04-96 : FMN ; Suppression prototype inutile.
30-06-97 : FMN ; Appel de la toolkit OpenGl_LightBox.
18-07-97 : FMN ; Meilleure gestion de la desactivation des lights
10-09-97 : FMN ; Meilleure gestion de la desactivation des lights
05-08-97 : PCT ; ajout descativation texture sur line etc...
19-08-97 : PCT ; ajout pourcentage de reflexion des objets avec textues environnante
22-12-97 : FMN ; Correction PRO10217 sur le mode Transient
13-03-98 : FMN ; Suppression variable externe TglUpdateView0
17-03-98 : FMN ; Ajout mode animation
- deplacement TelSetViewIndex vers OpenGl_execstruct.c.
- gestion du mode animation.
16-06-2000 : ATS,GG : G005 : Some conditions for execution of PARRAY  primitives

************************************************************************/
#ifdef DEBUG
#define TRACE
#define TRACE_EXEC
#endif

#define G003  /* EUG 16-09-99 G003 ZBufferAuto treatment 
*/

#define BUC60742  /* GG 18/09/00 Enable to use both animation and
graphic structure transformation
*/

#define G005  /* ATS,GG 11/01/00 Using arrays of primitives
*/

#define IMP051001 /* GG Enable/Disable polygon offset for
highlighted graphic structures
*/

/*----------------------------------------------------------------------*/
/*
* Includes
*/ 

#include <OpenGl_tgl_all.hxx>

#include <OpenGl_tgl_subrvis.hxx>

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <OpenGl_cmn_varargs.hxx>
#include <OpenGl_telem_attri.hxx>
#include <OpenGl_telem_filters.hxx>
#include <OpenGl_tsm.hxx>
#include <OpenGl_telem.hxx>
#include <OpenGl_LightBox.hxx>
#include <OpenGl_TextureBox.hxx>
#include <OpenGl_telem_view.hxx>
#include <OpenGl_telem_pick.hxx>
#include <OpenGl_telem_inquire.hxx>
#include <OpenGl_telem_util.hxx> /* for matcpy */

#include <OpenGl_tsm_ws.hxx>
#include <OpenGl_cmn_varargs.hxx>
#include <OpenGl_TextureBox.hxx>

#include <OpenGl_trsf_stack.hxx>

#ifdef G003
# include <OpenGl_degeneration.hxx>
#endif  /* G003 */
#ifdef G005
#include <InterfaceGraphic_PrimitiveArray.hxx>
#endif

/*----------------------------------------------------------------------*/
/*
* Variables externes
*/

extern tsm_trsf_stack cur_trsf_stack;
#ifdef G003
#define D_MODE( d )  ( int   )(   (  ( PDEGENERATION )( d.data.pdata )  ) -> mode        )
#define S_RATIO( d ) ( float )(   (  ( PDEGENERATION )( d.data.pdata )  ) -> skipRatio   )
extern GLboolean g_fAnimation;
extern GLboolean g_fDegeneration;
int       g_nDegenerateModel;
float     g_fSkipRatio;
#endif  /* G003 */

/*----------------------------------------------------------------------*/
/*
* Constantes
*/

static TEL_PACKED_NAME pn;

#define PUSHNAME( a )  pn.i = a, glPushName( pn.s[0] ), glPushName( pn.s[1] )
#define LOADNAME( a )  pn.i = a, glPopName(), glLoadName( pn.s[0] ), \
  glPushName( pn.s[1] )
#define POPNAME()      glPopName(), glPopName()

#define NO_TRACE_LIGHT 
#define NO_TRACE_EXEC 

/*----------------------------------------------------------------------*/
/*
* Prototypes fonctions
*/

static  TStatus  ExecuteStructurePick( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  ExecuteStructureDisplay( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  ExecuteStructureAdd( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  ExecuteStructurePrint( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  ExecuteStructureInquire( TSM_ELEM_DATA, Tint, cmn_key* );

/*----------------------------------------------------------------------*/
/*
* Variables locales
*/

static  TStatus  (*MtdTbl[])( TSM_ELEM_DATA, Tint, cmn_key* ) =
{
  ExecuteStructurePick,
    ExecuteStructureDisplay,
    ExecuteStructureAdd,
    0,             /* Delete : may be provided for deleting structure
                   network */
                   ExecuteStructurePrint,
                   ExecuteStructureInquire
};

/*----------------------------------------------------------------------*/

MtblPtr
TelExecuteStructureInitClass( TelType *el )
{
  *el = TelExecuteStructure;
  return MtdTbl;
}

/*----------------------------------------------------------------------*/

static  TStatus
ExecuteStructureAdd( TSM_ELEM_DATA d, Tint n, cmn_key *k )
{
  ((tsm_elem_data)(d.pdata))->ldata = k[0]->data.ldata;

  return TSuccess;
}

/*----------------------------------------------------------------------*/

static TStatus
ExecuteStructurePick( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  Tint              i, num;
  tsm_node          node;
  register Tint     display = 1;
  register Tint     pickfl = 0;
  register TelType  elem;
  Tint              vp;
  Tint              view = 0;
  CMN_KEY           key;

  if( TsmGetStructure( data.ldata, &num, &node ) == TFailure )
    return TFailure;

  if( !num )
    return TSuccess;

  PUSHNAME( data.ldata );  /* Struct id */
  PUSHNAME( -1 );          /* Pick id; dummy */
  PUSHNAME( -1 );          /* Elem no.; dummy */

  TsmPushAttri(); /* save previous graphics context */
  TglNamesetPush();
  if( TglFilterNameset( k[0]->id, InvisFilter ) == TSuccess )
    display = 0;
  if( TglFilterNameset( k[0]->id, PickFilter ) == TSuccess )
    pickfl = 1;
  key.id = TelViewIndex;
  TsmGetAttri( 1, &key );
  vp = key.data.ldata;
  if( TglVpBeingPicked == vp )
    view = 1;
  for( i = 0; i < num; i++, node = node->next )
  {
    elem = node->elem.el;
    switch( elem )
    {
    case TelAddNameset:
    case TelRemoveNameset:
      {
        TsmSendMessage( elem, PickTraverse, node->elem.data, 0 );
        if( TglFilterNameset( k[0]->id, InvisFilter ) == TSuccess )
          display = 0;
        else
          display = 1;
        if( TglFilterNameset( k[0]->id, PickFilter ) == TSuccess )
          pickfl = 1;
        else
          pickfl = 0;
        break;
      }
    case TelExecuteStructure:
      {
        LOADNAME( i+1 ); /* Elem no. */
        TsmSendMessage( elem, PickTraverse, node->elem.data, -1, k );
        break;
      }
    case TelPickId:
      {
        POPNAME();                        /* Elem no. */
        LOADNAME( node->elem.data.ldata );      /* Pick id */
        PUSHNAME( -1 );                   /* Elem no.; dummy */
        break;
      }
    case TelViewIndex:
      {
        vp = node->elem.data.ldata;
        if( vp == TglVpBeingPicked )
        {
          view = 1;
          key.id = TOn;
        }
        else
        {
          view = 0;
          key.id = TOff;
        }
        TsmSendMessage( elem, PickTraverse, node->elem.data, 1, &key );
        break;
      }
    case TelCurve:
    case TelPolygon:
    case TelPolygonSet:
    case TelPolygonHoles:
    case TelPolygonIndices:
    case TelQuadrangle:
    case TelPolyline:
    case TelMarker:
    case TelMarkerSet:
    case TelText:
    case TelTriangleMesh:
#ifdef G005
    case TelParray:
      /* do not send Pick message to */
      {                        /* primitives if they are not pickable */
        if( pickfl && view && display )
        {
          key.id = TOff; /* dummy key for highlight flag */
          LOADNAME( i+1 ); /* Elem no. */
          TsmSendMessage( elem, PickTraverse, node->elem.data, 1, &key );
        }
        break;
      }
#endif

    default:
      {
        TsmSendMessage( elem, PickTraverse, node->elem.data, 0 );
        break;
      }
    }
  }
  TglNamesetPop();
  TsmPopAttri(); /* restore previous graphics context */

  POPNAME();  /* Elem no. */
  POPNAME();  /* Pick id */
  POPNAME();  /* Struct id */

  return TSuccess;
}

/*----------------------------------------------------------------------*/

static  TStatus
ExecuteStructureDisplay( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  Tint              i, num;
  tsm_node          node;
  register Tint     display = 1;
  register Tint     highl = TOff;
  register TelType  elem;
  Tint              vp;
  CMN_KEY           key;
#ifdef G003
  static GLboolean s_fWire            = GL_FALSE;
  static Tint      s_depth            = 0;
  static Tint      save_texture_state = 0;
  static CMN_KEY   s_kLineWidth;
  static CMN_KEY   s_kPolyType;
  TStatus   retVal             = TSuccess;
#else
  int               save_texture_state;
#endif

  if( TsmGetStructure( data.ldata, &num, &node ) == TFailure )
#ifdef G003
  { retVal = TFailure; goto ret; }
#else
    return TFailure;
#endif

  if( !num )
#ifdef G003
    goto ret;
#else
    return TSuccess;
#endif

#ifdef G003
  ++s_depth;
#ifdef TRACE  
  printf(" .. ExecuteStructureDisplay.s_depth = %d\n",s_depth); 
#endif
#endif /*G003*/

  TsmPushAttri(); /* save previous graphics context */
  TglNamesetPush();
  if( TglFilterNameset( k[0]->id, InvisFilter ) == TSuccess )
    display = 0;
  if( TglFilterNameset( k[0]->id, HighlFilter ) == TSuccess )
    highl = TOn;
  key.id = TelViewIndex;
  TsmGetAttri( 1, &key );
#ifdef G003
  if ( s_depth == 1 ) {
    glPushAttrib ( GL_DEPTH_BUFFER_BIT );
    s_kLineWidth.id = TelPolylineWidth;
    s_kPolyType.id  = TelPolylineType;
#ifdef BUG
    TsmGetAttri ( 1, &s_kLineWidth, &s_kPolyType );
#else
    TsmGetAttri ( 2, &s_kLineWidth, &s_kPolyType );
#endif
    g_nDegenerateModel = 0;
    g_fSkipRatio       = 0.0F;
  }  /* end if */
#endif  /* G003 */

  vp = key.data.ldata;
  for( i = 0; i < num; i++, node = node->next )
  {
    elem = node->elem.el;
#ifdef TRACE_EXEC 
#include <OpenGl_traces.hxx>
#endif
    switch( elem )
    {
#ifdef G003
            case TelDegenerationMode:

      if ( g_fDegeneration )

        switch (  g_nDegenerateModel = D_MODE( node->elem )  ) 
      {

        case 0:

          break;

        default:

          glLineWidth ( 1.0 );
          glDisable   ( GL_LINE_STIPPLE );

        case 1:

          g_fSkipRatio = S_RATIO( node->elem );

      }  /* end switch */

    break;
#endif  /* G003 */
    case TelAddNameset:
    case TelRemoveNameset:
      {
      TsmSendMessage( elem, DisplayTraverse, node->elem.data, 0 );
      if( TglFilterNameset( k[0]->id, InvisFilter ) == TSuccess )
        display = 0;
      else
        display = 1;
      if( TglFilterNameset( k[0]->id, HighlFilter ) == TSuccess )
        highl = TOn;
      else
        highl = TOff;
      break;
      }
    case TelExecuteStructure:
      {
        tsm_trsf_stack new_trsf_stack = NULL;

        new_trsf_stack = (tsm_trsf_stack) malloc (sizeof (TSM_TRSF_STACK));
        cur_trsf_stack->next = new_trsf_stack;
        new_trsf_stack->prev = cur_trsf_stack;
        new_trsf_stack->next = NULL;
        matcpy(new_trsf_stack->LocalTran3, cur_trsf_stack->LocalTran3);
        cur_trsf_stack = new_trsf_stack;

#ifdef TRACE    
        printf(" .. ExecuteStructureDisplay.TelExecuteStructure.s_depth = %d\n",s_depth); 
#endif
#ifdef BUC60742   
        glMatrixMode (GL_MODELVIEW);
        glPushMatrix();
#endif
        TsmSendMessage( elem, DisplayTraverse, node->elem.data, -1, k );
#ifdef BUC60742   
        glMatrixMode (GL_MODELVIEW);
        glPopMatrix();
#endif
        cur_trsf_stack = new_trsf_stack->prev;
        cur_trsf_stack->next = NULL;
        free (new_trsf_stack);
        break;
      }
    case TelViewIndex:
      {
        break;
      } 

    case TelPolyline:
    case TelMarker:
    case TelMarkerSet:
    case TelText:
      {                        /* primitives if they are invisible */
        if( display )
        {
          LightOff();
#ifdef G003
          if ( g_fAnimation && !s_fWire && g_nDegenerateModel ) {
            glDisable ( GL_DEPTH_TEST );
#endif  /* G003 */
            save_texture_state = IsTextureEnabled();
            if( save_texture_state ) DisableTexture();
#ifdef G003
            s_fWire = GL_TRUE;
          }  /* end if */
#endif  /* G003 */

          key.id = highl;
          TsmSendMessage( elem, DisplayTraverse, node->elem.data, 1, &key );
          if (save_texture_state) EnableTexture();
        }
        break;
      }
    case TelCurve:
    case TelPolygon:
    case TelPolygonSet:
    case TelPolygonIndices:
    case TelQuadrangle:
    case TelTriangleMesh:
    case TelPolygonHoles:    /* do not send Display message to */
      {                        /* primitives if they are invisible */
        if( display )
        {
#ifdef G003
          if ( g_fAnimation && s_fWire && g_nDegenerateModel < 2 ) {
            if ( save_texture_state ) EnableTexture ();
            glEnable ( GL_DEPTH_TEST );
            s_fWire = GL_FALSE;
          }  /* end if */
#endif  /* G003 */
#ifdef IMP051001
          if( highl )
            call_subr_disable_polygon_offset();
#endif
          key.id = highl;
          TsmSendMessage( elem, DisplayTraverse, node->elem.data, 1, &key );
#ifdef IMP051001
          if( highl )
            call_subr_enable_polygon_offset();
#endif
        }
        break;
      }
#ifdef G005
    case TelParray:
      if( display ) {
        CALL_DEF_PARRAY *pa = (CALL_DEF_PARRAY *)node->elem.data.pdata;
        switch ( pa->type )
        {
        case TelPointsArrayType:
        case TelPolylinesArrayType:
        case TelSegmentsArrayType:
          LightOff();
#ifdef G003
          if ( g_fAnimation && !s_fWire && g_nDegenerateModel ) {
            glDisable ( GL_DEPTH_TEST );
#endif  /* G003 */
            save_texture_state = IsTextureEnabled();
            if( save_texture_state ) DisableTexture();
#ifdef G003
            s_fWire = GL_TRUE;
          }  /* end if */
#endif  /* G003 */

          key.id = highl;
          TsmSendMessage( elem, DisplayTraverse, node->elem.data, 1, &key );
          if (save_texture_state) EnableTexture();
          break;
        case TelPolygonsArrayType:
        case TelTrianglesArrayType:
        case TelQuadranglesArrayType:
        case TelTriangleStripsArrayType:
        case TelTriangleFansArrayType:
        case TelQuadrangleStripsArrayType:
#ifdef G003
          if ( g_fAnimation && s_fWire && g_nDegenerateModel < 2 ) {
            if ( save_texture_state ) EnableTexture ();
            glEnable ( GL_DEPTH_TEST );
            s_fWire = GL_FALSE;
          }  /* end if */
#endif  /* G003 */
          key.id = highl;
          TsmSendMessage( elem, DisplayTraverse, node->elem.data, 1, &key );
          break;
        default:
          break;
        }
      }
      break;
#endif  /*G005*/
      /* ABD 29/10/04  Transform Persistence of Presentation( pan, zoom, rotate ) */
    case TelTransformPersistence:
      {
        /*CALL_DEF_TRANSFORM_PERSISTENCE*/
        TEL_TRANSFORM_PERSISTENCE *tp 
          = (tel_transform_persistence)node->elem.data.pdata;
        if( tp->mode != 0 )
          TsmSendMessage (elem, DisplayTraverse, node->elem.data, 1, &key);
        break;
      }
      /*ABD 29/10/04  Transform Persistence of Presentation( pan, zoom, rotate ) */
    case TelUserdraw:
      {
        if( display )
        {
          key.id = highl;
          TsmSendMessage( elem, DisplayTraverse, node->elem.data, 1, &key );
        }
        break;
      }
    default:
      {
        TsmSendMessage( elem, DisplayTraverse, node->elem.data, 0 );
        break;
      }
    }
  }
  TglNamesetPop();
  TsmPopAttri(); /* restore previous graphics context; before update lights */
#ifdef G003
ret: 
  if ( s_depth == 1 ) {
    s_fWire = GL_FALSE;

    if ( g_nDegenerateModel > 1 ) {
      glLineWidth ( s_kLineWidth.data.fdata );
      if ( s_kPolyType.data.ldata != TEL_LS_SOLID ) glEnable ( GL_LINE_STIPPLE );
    }  /* end if */

    glPopAttrib ();
  }  /* end if */

  --s_depth;
  return retVal;
#else
  return TSuccess;
#endif
}

/*----------------------------------------------------------------------*/

static  TStatus
ExecuteStructurePrint( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  fprintf( stdout, "TelExecuteStructure. Value = %d\n", data.ldata);
  fprintf( stdout, "\n" );

  return TSuccess;
}

/*----------------------------------------------------------------------*/

static TStatus
ExecuteStructureInquire( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  Tint i;

  for( i = 0; i < n; i++ )
  {
    switch( k[i]->id )
    {
    case INQ_GET_SIZE_ID:
      {
        k[i]->data.ldata = sizeof( Tint );
        break;
      }
    case INQ_GET_CONTENT_ID:
      {
        TEL_INQ_CONTENT *c;
        Teldata         *w;

        c = (tel_inq_content)k[i]->data.pdata;
        w = c->data;
        c->act_size = 0;
        w->idata = data.ldata;
        break;
      }
    }
  }

  return TSuccess;
}

/*----------------------------------------------------------------------*/
