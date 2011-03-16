/***********************************************************************

FONCTION :
----------
File OpenGl_polygon :


REMARQUES:
---------- 

Le culling et le backfacing ne marchent pas.

HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : xxx ; Creation.
11-03-96 : FMN ; Correction warning compilation
01-04-96 : CAL ; Integration MINSK portage WNT
28-06-96 : GG  ; Auto checking of the polygon facet normal with
the order of the points (OPTIM)
09-08-96 : FMN ; Suppression appel glMatrixMode() avant glGetFloatv()
28-02-97 : FMN ; Suppression OpenGl_telem_light.h
18-07-97 : FMN ; Ajout desactivation des lights suivant front_lighting_model
21-07-97 : FMN ; Amelioration des performances OPTIMISATION_FMN
- suppression calcul inutile sur les front faces
27-08-97 : FMN ; Correction affichage des edges
On n'affiche que les edges dans le mode IS_EMPTY
10-09-97 : FMN ; Amelioration des perfs liees aux lights.
15-09-97 : PCT ; Ajout coordonnees textures
24-09-97 : FMN ; Suppression OPTIMISATION_FMN.
08-12-97 : FMN ; Suppression appel TsmGetAttri inutile.
31-12-97 : FMN ; Simplification pour le highlight
15-01-98 : FMN ; Ajout Hidden line
18-06-98 : CAL ; Correction PRO14340. Patch K4198 et K4199
08-03-01 : GG  ; BUC60823 Avoid crash in the normal computation method
on confuse point.

************************************************************************/

#define G003  /* EUG 22-09-99 Degeneration management
*/

/*----------------------------------------------------------------------*/
/*
* Includes
*/ 

#include <OpenGl_tgl_all.hxx>

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <OpenGl_cmn_varargs.hxx>
#include <OpenGl_Extension.hxx>
#include <OpenGl_telem_attri.hxx>
#include <OpenGl_tsm.hxx>
#include <OpenGl_telem.hxx>
#include <OpenGl_telem_util.hxx>
#include <OpenGl_telem_highlight.hxx>
#include <OpenGl_telem_inquire.hxx>
#include <OpenGl_telem_view.hxx>
#include <OpenGl_tgl_funcs.hxx>
#include <OpenGl_LightBox.hxx>
#include <OpenGl_TextureBox.hxx>
#include <OpenGl_Memory.hxx>



/*----------------------------------------------------------------------*/
/*
* Constantes
*/ 

#ifndef WNT
# define CALLBACK
#if !defined(APIENTRY)
# define APIENTRY
#endif // APIENTRY
# define STATIC static
#else
# include <GL\GLU.H>
#define STATIC
typedef double GLUcoord;
#endif  /* WNT */

#ifdef G003
extern int g_nDegenerateModel;
#endif  /* G003 */

/*----------------------------------------------------------------------*/
/*
* Fonctions statiques
*/ 

static TStatus  PolygonDisplay( TSM_ELEM_DATA, Tint, cmn_key* );
static TStatus  PolygonAdd( TSM_ELEM_DATA, Tint, cmn_key* );
static TStatus  PolygonDelete( TSM_ELEM_DATA, Tint, cmn_key* );
static TStatus  PolygonPrint( TSM_ELEM_DATA, Tint, cmn_key* );
static TStatus  PolygonInquire( TSM_ELEM_DATA, Tint, cmn_key* );

/*----------------------------------------------------------------------*/
/*
* Variables externes
*/

extern  Tint  ForbidSetTextureMapping; /* currently defined in tsm/tsm.c */

/*----------------------------------------------------------------------*/
/*
* Definition de types
*/ 

struct EXTRA_VERTEX
{
  GLfloat vert[3];
  int ind;
  IMPLEMENT_MEMORY_OPERATORS
};
typedef EXTRA_VERTEX* extra_vertex;

struct SEQ_
{
  Tint ts_num, ts_alloc;
  void **tmesh_sequence;
  GLenum triangle_type; /* FSXXX OPTI */
  IMPLEMENT_MEMORY_OPERATORS
};

struct DISPLAY_
{
  Tint num_of_seq;
  Tint num_alloc;
  SEQ_ *seq;
  IMPLEMENT_MEMORY_OPERATORS
};

struct TEL_POLYGON_DATA
{
  Tint       num_vertices;  /* Number of vertices */
  Tint       facet_flag;  /* TEL_FA_NONE or TEL_FA_NORMAL */
  Tint       vertex_flag; /* TEL_VT_NONE or TEL_VT_NORMAL */
  Tint       shape_flag;  /* TEL_SHAPE_UNKNOWN or TEL_SHAPE_COMPLEX or
                          TEL_SHAPE_CONVEX  or TEL_SHAPE_CONCAVE */
  GLboolean  reverse_order; /* GL_TRUE if polygon vertex must be display in reverse order */
  TEL_POINT  fnormal;   /* Facet normal */
  tel_colour fcolour;   /* Facet colour */
  tel_point  vertices;    /* Vertices */
  tel_colour vcolours;    /* Vertex colour values */
  tel_point  vnormals;    /* Vertex normals */
  tel_texture_coord vtexturecoord; /* Texture Coordinates */
  DISPLAY_   *display;    /* FSXXX OPTI */
  GLuint     triangle_dl1;     /* triangle display list 1 */
  GLuint     triangle_dl2;     /* triangle display list 2 */
  GLuint     triangle_dl3;     /* triangle display list 3 */
  IMPLEMENT_MEMORY_OPERATORS
};
typedef TEL_POLYGON_DATA *tel_polygon_data;

static TEL_POLYGON_DATA polygon_defaults =
{
  0,                 /* num_vertices */
  TEL_FA_NONE,       /* facet_flag */
  TEL_VT_NONE,       /* vertex_flag */
  TEL_SHAPE_UNKNOWN, /* shape_flag */
  GL_FALSE,        /* reverse_order */
  {{ ( float )0.0, ( float )0.0, ( float )0.0 }}, /* fnormal */
  0,                 /* fcolour */
  0,                 /* vertices */
  0,                 /* vcolours */
  0,                 /* vnormals */
  0,                 /* vtexturecoord */
  0,                 /* display */
  0,                 /* triangle_dl1 */
  0,                 /* triangle_dl2 */
  0                  /* triangle_dl3 */
};


static void bgntriangulate( tel_polygon_data, void (APIENTRY*)() );
static void endtriangulate(void);
static void draw_tmesh( tel_polygon_data, Tint );
static void draw_polygon( tel_polygon_data, Tint, Tint );
static void draw_polygon_concav( tel_polygon_data, Tint, Tint );

#if !defined(GLU_VERSION_1_2)
#define GLUtesselator GLUtriangulatorObj
void gluTessBeginContour();
void gluTessBeginPolygon();
void gluTessEndPolygon();
void gluTessEndContour();
#endif

static  TStatus  (*MtdTbl[])( TSM_ELEM_DATA, Tint, cmn_key* ) =
{
  PolygonDisplay,             /* PickTraverse */
  PolygonDisplay,
  PolygonAdd,
  PolygonDelete,
  PolygonPrint,
  PolygonInquire
};

static void draw_edges( tel_polygon_data, tel_colour, Tint);

/*----------------------------------------------------------------------*/

MtblPtr
TelPolygonInitClass( TelType* el )
{
  *el = TelPolygon;
  return MtdTbl;
}
/*----------------------------------------------------------------------*/

static  TStatus
PolygonAdd( TSM_ELEM_DATA d, Tint n, cmn_key *k )
{
  Tint             i, j;
  tel_polygon_data data;

  for( i = 0; i < n; i++ )
  {
    if( k[i]->id == NUM_VERTICES_ID )
      break;
  }
  if( i == n )
    return TFailure;

  if( !(k[i]->data.ldata) )
    return TFailure;

  for( j = 0; j < n; j++ )
  {
    if( k[j]->id == VERTICES_ID )
      break;
  }
  if( j == n )
    return TFailure;

  data = new TEL_POLYGON_DATA();
  if( !data )
    return TFailure;

  /* load defaults */
  memcpy( data, &polygon_defaults, sizeof(TEL_POLYGON_DATA) );

  data->num_vertices = k[i]->data.ldata;
  data->vertices = new TEL_POINT[data->num_vertices];
  memcpy( data->vertices, k[j]->data.pdata, data->num_vertices*sizeof(TEL_POINT) );

  for( i = 0; i < n; i++ )
  {
    switch( k[i]->id )
    {
    case FNORMALS_ID:
      data->facet_flag = TEL_FA_NORMAL;
      memcpy( &data->fnormal, k[i]->data.pdata, sizeof(TEL_POINT) );
      vecnrm( data->fnormal.xyz );
      break;

    case FACET_COLOUR_VALS_ID:
      data->fcolour = new TEL_COLOUR();
      memcpy( data->fcolour, k[i]->data.pdata, sizeof(TEL_COLOUR) );
      break;

    case VERTEX_COLOUR_VALS_ID:
      data->vcolours = new TEL_COLOUR[data->num_vertices];
      memcpy( data->vcolours, k[i]->data.pdata, data->num_vertices*sizeof(TEL_COLOUR) );
      break;

    case VNORMALS_ID:
      data->vertex_flag = TEL_VT_NORMAL;
      data->vnormals = new TEL_POINT[data->num_vertices];
      memcpy( data->vnormals, k[i]->data.pdata, data->num_vertices*sizeof(TEL_POINT) );
      for( j = 0; j < data->num_vertices; j++ )
        vecnrm( data->vnormals[j].xyz );
      break;

    case SHAPE_FLAG_ID:
      data->shape_flag = k[i]->data.ldata;
      break;

    case VTEXTURECOORD_ID:
      data->vtexturecoord = new TEL_TEXTURE_COORD[data->num_vertices];
      memcpy( data->vtexturecoord, k[i]->data.pdata, data->num_vertices*sizeof(TEL_TEXTURE_COORD) );
      break;
    }
  }

  if( data->facet_flag != TEL_FA_NORMAL ) {
#ifdef BUC60823
    TelGetPolygonNormal( data->vertices, NULL, 
      data->num_vertices, data->fnormal.xyz );
#else
    TelGetNormal( data->vertices[0].xyz, data->vertices[1].xyz,
      data->vertices[2].xyz, data->fnormal.xyz );
    vecnrm( data->fnormal.xyz );
#endif
  }
  else 
  {
    TEL_POINT  fnormal;         /* Facet normal */
    Tfloat sens;
#ifdef BUC60823
    TelGetPolygonNormal( data->vertices, NULL, 
      data->num_vertices, fnormal.xyz );
#else
    TelGetNormal( data->vertices[0].xyz, data->vertices[1].xyz,
      data->vertices[2].xyz, fnormal.xyz );
#endif
    sens = fnormal.xyz[0]*data->fnormal.xyz[0] +
      fnormal.xyz[1]*data->fnormal.xyz[1] +
      fnormal.xyz[2]*data->fnormal.xyz[2];
    if( sens < 0. ) data->reverse_order = GL_TRUE;
  }
  ((tsm_elem_data)(d.pdata))->pdata = data;

  /* s'il n'est pas convexe, le polygone est triangule de la meme maniere
  que les polygones a trous */
  /* Pourrait etre place en tete de routine pour optimisation si je
  savais recuperer le shape_flag a partir de d sans reconstruire data (!) */

  /* FSXXX
  if( data->shape_flag != TEL_SHAPE_CONVEX )
  return(PolygonHolesAdd( d, n, k ));
  */

  return TSuccess;
}
/*----------------------------------------------------------------------*/

static  TStatus
PolygonDisplay( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  /*   Tint           front_face = 1;*/
  CMN_KEY        k11, k12, k17, k111, k114;

  Tint           front_lighting_model;
  Tint           interior_style;
  TEL_COLOUR     interior_colour;
  TEL_COLOUR     edge_colour;

  tel_polygon_data d;

  k12.id          = TelInteriorReflectanceEquation;
  k17.id          = TelInteriorStyle;
  k111.id         = TelInteriorColour;
  k111.data.pdata = &interior_colour;
  k114.id         = TelEdgeColour;
  k114.data.pdata = &edge_colour;

  TsmGetAttri( 4, &k12, &k17, &k111, &k114);

  front_lighting_model     = k12.data.ldata;
  interior_style           = k17.data.ldata;

#ifdef PRINT
  printf("PolygonDisplay \n"); 
#endif

  /* 
  * Use highlight colours 
  */

  if( k[0]->id == TOn )
  {                         
    TEL_HIGHLIGHT  hrep;

    k11.id = TelHighlightIndex;
    TsmGetAttri( 1, &k11 );
    if( TelGetHighlightRep( TglActiveWs, k11.data.ldata, &hrep ) == TSuccess )
    {
      if( hrep.type == TelHLForcedColour )
      {
        interior_colour = hrep.col;
        edge_colour = hrep.col;
        front_lighting_model = CALL_PHIGS_REFL_NONE;
      }
      else
        k[0]->id = TOff;
    }
    else
    {
      TelGetHighlightRep( TglActiveWs, 0, &hrep );
      if( hrep.type == TelHLForcedColour )
      {
        interior_colour = hrep.col;
        edge_colour = hrep.col;
        front_lighting_model = CALL_PHIGS_REFL_NONE;
      }
      else
        k[0]->id = TOff;
    }
  }

  d = (tel_polygon_data)data.pdata;
#ifdef G003

  if( interior_style != TSM_EMPTY && g_nDegenerateModel < 2 ) 
#else 
  if( interior_style != TSM_EMPTY ) 
#endif  /*G003*/
  {          
    if (front_lighting_model == CALL_PHIGS_REFL_NONE)
      LightOff();
    else LightOn();

    glColor3fv( interior_colour.rgb );

    if( d->shape_flag != TEL_SHAPE_CONVEX )
      draw_polygon_concav( d, front_lighting_model, k[0]->id );
    else
      draw_polygon( d, front_lighting_model, k[0]->id );
  }

#ifdef G003
  /* OCC11904 -- Temporarily disable environment mapping */
  glPushAttrib(GL_ENABLE_BIT);
  glDisable(GL_TEXTURE_1D);
  glDisable(GL_TEXTURE_2D);

  switch ( g_nDegenerateModel ) {

default:
case 0:  /* no degeneration        */
  draw_edges ( d, &edge_colour, interior_style );
  break;
case 2:  /* wireframe degeneration */

#ifdef xG003  /* GG260100 use interior color instead edge color */
  draw_edges ( d, &interior_colour, interior_style );
#else
  draw_edges ( d, &edge_colour, interior_style );
#endif
  break;

case 3:  /* marker degeneration */

  break;

  }  /* end switch */

  glPopAttrib(); /* skt: GL_ENABLE_BIT*/
#else
  draw_edges( d, &edge_colour, interior_style );
  /*skt: It is not correct to pop attributes here*/
  /*glPopAttrib();*/
#endif  /*G003*/
  return TSuccess;

}
/*----------------------------------------------------------------------*/

static void
draw_polygon( tel_polygon_data d, Tint front_lighting_model, Tint hflag )
{
  Tint      i;

  tel_point ptr;
  tel_point  pvn;
  tel_colour pfc, pvc;
  tel_texture_coord pvt;

  pfc = d->fcolour;
  pvc = d->vcolours;
  pvn = d->vnormals;
  pvt = d->vtexturecoord;

  if( hflag ) pvc = pfc = NULL;

  ptr = d->vertices;
  if( pfc )
    glColor3fv( pfc->rgb );
  if( front_lighting_model != CALL_PHIGS_REFL_NONE )
    glNormal3fv( d->fnormal.xyz );

  if( d->reverse_order ) glFrontFace( GL_CW );

  if (d->num_vertices == 3) glBegin(GL_TRIANGLES);
  else if(d->num_vertices == 4) glBegin(GL_QUADS);
  else glBegin(GL_POLYGON);
  if( front_lighting_model == CALL_PHIGS_REFL_NONE )
  {
    if( pvc )
    {
      for( i=0; i<d->num_vertices; i++, ptr++ )
      {
        glColor3fv( pvc[i].rgb );
        glVertex3fv( ptr->xyz );
      }
    }
    else
    {
      for( i=0; i<d->num_vertices; i++, ptr++ )
      {
        glVertex3fv( ptr->xyz );
      }
    }
  } 
  else
  {
    if( pvn )
    {
      if (pvt && !ForbidSetTextureMapping)
        for( i=0; i<d->num_vertices; i++, ptr++ )
        {
          glNormal3fv( pvn[i].xyz );
          glTexCoord2fv( pvt[i].xy );
          glVertex3fv( ptr->xyz );
        }
      else
        for( i=0; i<d->num_vertices; i++, ptr++ )
        {
          glNormal3fv( pvn[i].xyz );
          glVertex3fv( ptr->xyz );
        }      
    }
    else
    {
      for( i=0; i<d->num_vertices; i++, ptr++ )
      {
        glVertex3fv( ptr->xyz );
      }
    }
  }
  glEnd();
  if( d->reverse_order ) glFrontFace( GL_CCW );

}

/*----------------------------------------------------------------------*/

/* JWR - allow varying the size */

#define INCREMENT    8

static int seq_increment = INCREMENT;

tel_polygon_data DaTa;
static GLUtesselator *tripak = 0;

STATIC void APIENTRY
out_bgntmesh( GLenum triangle_type )
{
  DISPLAY_ *dis;
  dis = DaTa->display;

  dis->num_of_seq++;
  if( dis->num_alloc < dis->num_of_seq )
  {
    dis->num_alloc += seq_increment;

    if( dis->seq == 0 )
    {
      dis->seq = new SEQ_[dis->num_alloc];
    }
    else
    {
#if defined(__SUNPRO_CC) && (__SUNPRO_CC <= 0x530)
      dis->seq = (SEQ_*)realloc( dis->seq, dis->num_alloc*sizeof(SEQ_) );
#else
      dis->seq = cmn_resizemem<SEQ_>( dis->seq, dis->num_alloc );
#endif
    }
  }
  dis->seq[ dis->num_of_seq - 1 ].ts_num = 0;
  dis->seq[ dis->num_of_seq - 1 ].ts_alloc = 0;
  dis->seq[ dis->num_of_seq - 1 ].tmesh_sequence = 0;

#ifdef JWR_DEC_TRIFAN_BUG
  dis->seq[ dis->num_of_seq - 1 ].triangle_type = GL_POLYGON;
  glBegin(GL_POLYGON);
#else
  dis->seq[ dis->num_of_seq - 1 ].triangle_type = triangle_type;
  glBegin(triangle_type);
#endif
}

/*----------------------------------------------------------------------*/

STATIC void APIENTRY
out_vert1( void *data )
{
  SEQ_ *s;

  s  = &( DaTa->display->seq[ DaTa->display->num_of_seq - 1 ] );

  s->ts_num++;
  if( s->ts_alloc < s->ts_num )
  {
    s->ts_alloc += seq_increment;

    if( s->tmesh_sequence == 0 )
    {
      s->tmesh_sequence = new void*[s->ts_alloc];
    }
    else
    {
#if defined(__SUNPRO_CC) && (__SUNPRO_CC <= 0x530)
      s->tmesh_sequence = (void**)realloc( s->tmesh_sequence, s->ts_alloc*sizeof(void*));
#else
      s->tmesh_sequence = cmn_resizemem<void*>( s->tmesh_sequence, s->ts_alloc);
#endif
    }
  }
  s->tmesh_sequence[ s->ts_num - 1 ] = data;


  if ( data < (void *)0xffff ) {
    long a = (long)data;

    glVertex3fv( DaTa->vertices[a].xyz );
  }
  else {
    extra_vertex b = (extra_vertex) data;

#ifdef TRACE
    printf("combine1 %e   %e   %e %d\n",
      b->vert[0], b->vert[1], b->vert[2], (b->ind));
#endif
    glVertex3fv( b->vert );
  }

}

/*----------------------------------------------------------------------*/

STATIC void APIENTRY
out_vert2( void *data )
{
  SEQ_ *s;

  s = &( DaTa->display->seq[ DaTa->display->num_of_seq - 1 ] );

  s->ts_num++;
  if( s->ts_alloc < s->ts_num )
  {
    s->ts_alloc += seq_increment;

    if( s->tmesh_sequence == 0 )
    {
      s->tmesh_sequence = new void*[s->ts_alloc];
    }
    else
    {
#if defined(__SUNPRO_CC) && (__SUNPRO_CC <= 0x530)
      s->tmesh_sequence = (void**)( s->tmesh_sequence,
        s->ts_alloc*sizeof(void*) );
#else
      s->tmesh_sequence = cmn_resizemem<void*>( s->tmesh_sequence,
        s->ts_alloc );
#endif
    }
  }
  s->tmesh_sequence[ s->ts_num - 1 ] = data;

  if ( data < (void *)0xffff ) {
    long a = (long)data;

    glColor3fv( DaTa->vcolours[a].rgb );
    glVertex3fv(  DaTa->vertices[a].xyz );
  }
  else {
    extra_vertex b = (extra_vertex) data;

    printf("combine2 %e   %e   %e %d\n",b->vert[0], b->vert[1], b->vert[2], (b->ind));
    glColor3fv( DaTa->vcolours[(b->ind)].rgb );
    glVertex3fv( b->vert );
  }

}

/*----------------------------------------------------------------------*/

STATIC void APIENTRY
out_vert3( void *data )
{
  SEQ_ *s;

  s = &( DaTa->display->seq[ DaTa->display->num_of_seq - 1 ] );

  s->ts_num++;
  if( s->ts_alloc < s->ts_num )
  {
    s->ts_alloc += seq_increment;

    if( s->tmesh_sequence == 0 )
    {
      s->tmesh_sequence = new void*[s->ts_alloc];
    }
    else
    {
#if defined(__SUNPRO_CC) && (__SUNPRO_CC <= 0x530)
      s->tmesh_sequence = (void**)realloc( s->tmesh_sequence,
        s->ts_alloc*sizeof(void*) );
#else
      s->tmesh_sequence = cmn_resizemem<void*>( s->tmesh_sequence,
        s->ts_alloc );
#endif
    }
  }
  s->tmesh_sequence[ s->ts_num - 1 ] = data;

  if ( data <= (void *)0xffff ) {
    long a = (long)data;

    glNormal3fv(  DaTa->vnormals[a].xyz );
    glVertex3fv(  DaTa->vertices[a].xyz);
  }
  else {
    extra_vertex b = (extra_vertex) data;

    printf("combine3 %e   %e   %e  %d\n", b->vert[0], b->vert[1], b->vert[2], (b->ind));
    glNormal3fv(  DaTa->vnormals[(b->ind)].xyz );
    glVertex3fv( b->vert );
  }

}

/*----------------------------------------------------------------------*/

STATIC void APIENTRY
mycombine( GLdouble coords[3], int *data, GLfloat w[4],  void **dataout)
{
  extra_vertex  new_vertex;

  new_vertex =  (extra_vertex) malloc(sizeof(EXTRA_VERTEX));

  new_vertex->vert[0] = ( float )coords[0];
  new_vertex->vert[1] = ( float )coords[1];
  new_vertex->vert[2] = ( float )coords[2];
  new_vertex->ind =   *data;
  *dataout = new_vertex;
#ifdef TRACE
  printf("mycombine3 %e   %e   %e\n",
    new_vertex->vert[0],
    new_vertex->vert[1],
    new_vertex->vert[2],
    new_vertex->ind);
  printf("weights: %f %f %f %f\n",
    w[0], w[1], w[2], w[3]);
#endif
}

/*----------------------------------------------------------------------*/

STATIC void APIENTRY
out_endtmesh( void )
{
  glEnd();
}

/*----------------------------------------------------------------------*/

STATIC void APIENTRY
out_error( GLenum error )
{
  printf( "POLYGON : %s\n", (char *) gluErrorString(error) );
}

/*----------------------------------------------------------------------*/

static void
bgntriangulate(tel_polygon_data d, void ( APIENTRY * out_ver)() )
{
  DaTa = d;
  if( !DaTa->display )
    DaTa->display = new DISPLAY_();

  tripak = gluNewTess();

#if defined(linux) && !defined(NOGLUfuncptr)
  gluTessCallback( tripak, GLU_TESS_BEGIN, (_GLUfuncptr)(out_bgntmesh) );
  gluTessCallback( tripak, GLU_TESS_VERTEX, out_ver );
  gluTessCallback( tripak, GLU_TESS_END, out_endtmesh );
  gluTessCallback( tripak, GLU_TESS_ERROR, (_GLUfuncptr)(out_error) );
  gluTessCallback( tripak, GLU_TESS_COMBINE, (_GLUfuncptr)(mycombine) );
#else 
  gluTessCallback( tripak, GLU_TESS_BEGIN, (void (APIENTRY*)())out_bgntmesh );
  gluTessCallback( tripak, GLU_TESS_VERTEX, (void (APIENTRY*)())out_ver );
  gluTessCallback( tripak, GLU_TESS_END, (void (APIENTRY*)())out_endtmesh );
  gluTessCallback( tripak, GLU_TESS_ERROR, (void (APIENTRY*)())out_error );
  gluTessCallback( tripak, GLU_TESS_COMBINE, (void (APIENTRY*)())mycombine );
#endif
}

/*----------------------------------------------------------------------*/

static void
endtriangulate()
{
  DaTa = 0;
  gluDeleteTess(tripak);
}

/*----------------------------------------------------------------------*/

static void
draw_polygon_concav( tel_polygon_data d, Tint front_lighting_model, Tint hflag )
{
  long       i;

  tel_point  pvn;
  tel_point  ptr;
  tel_colour pfc, pvc;
  GLdouble  xyz[3];

  pfc = d->fcolour;
  pvc = d->vcolours;
  pvn = d->vnormals;

  if( hflag )
  {
    pvc = pfc = NULL;
  }

  ptr = d->vertices;
  DaTa = d;

  if( pfc )
    glColor3fv( pfc->rgb );
  if( front_lighting_model != CALL_PHIGS_REFL_NONE )
    glNormal3fv( d->fnormal.xyz );

  if( d->reverse_order ) glFrontFace( GL_CW );

  if( d->display == 0 )
  {

    if( front_lighting_model == CALL_PHIGS_REFL_NONE )
    {
      if( pvc )
      {
        bgntriangulate(d, (void (APIENTRY*)())out_vert2);
      }
      else
      {
        bgntriangulate(d, (void (APIENTRY*)())out_vert1);
      }
    }
    else
    {
      if( pvn )
      {
        bgntriangulate(d, (void (APIENTRY*)())out_vert3);
      }
      else
      {
        bgntriangulate(d, (void (APIENTRY*)())out_vert1);
      }
    }
    gluTessBeginPolygon( tripak, NULL );
    gluTessBeginContour( tripak);

    for( i=0; i<d->num_vertices; i++, ptr++ )
    {
      xyz[0] = ptr->xyz[0];
      xyz[1] = ptr->xyz[1];
      xyz[2] = ptr->xyz[2];    
#ifndef WNT
      gluTessVertex( tripak, xyz,(void * ) i );
#else
      {
        double v[ 3 ] = {ptr -> xyz[ 0 ], ptr -> xyz[ 1 ], ptr -> xyz[ 2 ]};
        gluTessVertex (  tripak,  v, ( void* )i  );
      }
#endif  /* WNT */
    }
    gluTessEndContour( tripak);
    gluTessEndPolygon( tripak );
    endtriangulate();
  } 
  else 
  {
    if( front_lighting_model == CALL_PHIGS_REFL_NONE )
    {
      if( pvc ) draw_tmesh( d, 2 );
      else draw_tmesh( d, 1 );
    }
    else
    {
      if( pvn ) draw_tmesh( d, 3 );
      else draw_tmesh( d, 1 );
    }
  }

  if( d->reverse_order ) glFrontFace( GL_CCW );
}

/*----------------------------------------------------------------------*/

static void
draw_tmesh( tel_polygon_data d, Tint v )
{
  Tint      i, j, k;
  DISPLAY_ *dis;
  SEQ_     *s;
  extra_vertex b;

  dis = d->display;
  for( i = 0; i < dis->num_of_seq; i++ )
  {
    s = &(dis->seq[i]);

    glBegin(s->triangle_type);
    switch( v )
    {
    case 1:
      {
        for( j = 0, k = 0; j < s->ts_num; j++ )
        {
          if ( s->tmesh_sequence[j] < (void *)0xffff )
            glVertex3fv( d->vertices[ (long)s->tmesh_sequence[ j ] ].xyz );
          else {
            extra_vertex b = (extra_vertex)s->tmesh_sequence[j];
            glVertex3fv( b->vert );
          }

        }
        break;
      }
    case 2:
      {
        for( j = 0, k = 0; j < s->ts_num; j++ )
        {
          if ( s->tmesh_sequence[j] < (void *)0xffff ) {
            glColor3fv( d->vcolours[ (long) s->tmesh_sequence[ j ] ].rgb );
            glVertex3fv( d->vertices[ (long) s->tmesh_sequence[ j ] ].xyz );
          } else {
            b = (extra_vertex) s->tmesh_sequence[j];
            glColor3fv( d->vcolours[(b->ind)].rgb);
            glVertex3fv( b->vert );
          }
        }
        break;
      }
    case 3:
      {
        for( j = 0, k = 0; j < s->ts_num; j++ )
        {
          if ( s->tmesh_sequence[j] < (void *)0xffff ) {
            glNormal3fv( d->vnormals[ (long) s->tmesh_sequence[ j ] ].xyz);
            glVertex3fv( d->vertices[ (long) s->tmesh_sequence[ j ] ].xyz);
          } else {
            b = (extra_vertex) s->tmesh_sequence[j];
            glNormal3fv( d->vnormals[(b->ind)].xyz);
            glVertex3fv( b->vert );
          }
        }
        break;
      }
    }
    glEnd();
  }
}

/*----------------------------------------------------------------------*/

static  TStatus
PolygonDelete( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  tel_polygon_data p = (tel_polygon_data)data.pdata;
  if( p->fcolour )  //cmn_freemem( p->fcolour );
    delete p->fcolour;
  if( p->vertices )// cmn_freemem( p->vertices );
    delete[] p->vertices;
  if( p->vcolours ) //cmn_freemem( p->vcolours );
    delete[] p->vcolours;
  if( p->vnormals ) //cmn_freemem( p->vnormals );
    delete[] p->vnormals;
  if (p->vtexturecoord) //cmn_freemem( p->vtexturecoord);
    delete p->vtexturecoord;

  if(p->display)
  {
    Tint i, j;

    for( i = 0; i <  p->display->num_of_seq; i++ )
    {
      if(p->display->seq[i].tmesh_sequence) {
        for ( j = 0; j < p->display->seq[i].ts_num ; j++ ) {
          if ( p->display->seq[i].tmesh_sequence[j] >= (void *)0xffff )
            free(p->display->seq[i].tmesh_sequence[j]);
        }
      }
      delete[] p->display->seq[i].tmesh_sequence;
    }
    delete[] p->display->seq;
    delete p->display;
  }
  if (data.pdata)
    delete data.pdata;
  return TSuccess;
}

/*----------------------------------------------------------------------*/

static  TStatus
PolygonPrint( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  Tint             i;
  tel_polygon_data p = (tel_polygon_data)data.pdata;

  fprintf( stdout, "TelPolygon. Number of points: %d\n", p->num_vertices );
  switch( p->shape_flag )
  {
  case TEL_SHAPE_UNKNOWN:
    fprintf( stdout, "\t\tShape Flag : UNKNOWN\n" );
    break;

  case TEL_SHAPE_COMPLEX:
    fprintf( stdout, "\t\tShape Flag : COMPLEX\n" );
    break;

  case TEL_SHAPE_CONCAVE:
    fprintf( stdout, "\t\tShape Flag : CONCAVE\n" );
    break;

  case TEL_SHAPE_CONVEX:
    fprintf( stdout, "\t\tShape Flag : CONVEX\n" );
    break;

  }
  switch( p->facet_flag )
  {
  case TEL_FA_NONE:
    if( p->fcolour )
      fprintf( stdout, "\t\tFacet Flag : COLOUR\n" );
    else
      fprintf( stdout, "\t\tFacet Flag : NONE\n" );
    break;

  case TEL_FA_NORMAL:
    if( p->fcolour )
      fprintf( stdout, "\t\tFacet Flag : COLOURNORMAL\n" );
    else
      fprintf( stdout, "\t\tFacet Flag : NORMAL\n" );
    break;
  }
  switch( p->vertex_flag )
  {
  case TEL_VT_NONE:
    if( p->vcolours )
      fprintf( stdout, "\t\tVertex Flag : COLOUR\n" );
    else
      fprintf( stdout, "\t\tVertex Flag : NONE\n" );
    break;

  case TEL_VT_NORMAL:
    if( p->vcolours )
      fprintf( stdout, "\t\tVertex Flag : COLOURNORMAL\n" );
    else
      fprintf( stdout, "\t\tVertex Flag : NORMAL\n" );
    break;
  }
  fprintf( stdout, "\t\tFacet Normal : %g %g %g\n", p->fnormal.xyz[0],
    p->fnormal.xyz[1],
    p->fnormal.xyz[2] );
  fprintf( stdout, "\t\tReverse order : %d\n", p->reverse_order);

  if( p->fcolour )
    fprintf( stdout, "\t\tFacet Colour : %g %g %g\n", p->fcolour->rgb[0],
    p->fcolour->rgb[1],
    p->fcolour->rgb[2] );
  else
    fprintf( stdout, "\n\t\tFacet Colour not specified\n" );

  if( p->vertices )
  {
    fprintf( stdout, "\n\t\tVertices : " );
    for( i = 0; i < p->num_vertices; i++ )
      fprintf( stdout, "\n\t\t v[%d] = %g %g %g", i,
      p->vertices[i].xyz[0],
      p->vertices[i].xyz[1],
      p->vertices[i].xyz[2] );
  }

  fprintf( stdout, "\n" );
  if( p->vcolours )
  {
    fprintf( stdout, "\n\t\tVertex Colours : " );
    for( i = 0; i < p->num_vertices; i++ )
      fprintf( stdout, "\n\t\t v[%d] = %g %g %g", i,
      p->vcolours[i].rgb[0],
      p->vcolours[i].rgb[1],
      p->vcolours[i].rgb[2] );
  }
  else
    fprintf( stdout, "\n\t\tVertex Colours not specified\n" );

  if( p->vnormals )
  {
    fprintf( stdout, "\n\t\tVertex Normals : " );
    for( i = 0; i < p->num_vertices; i++ )
      fprintf( stdout, "\n\t\t v[%d] = %g %g %g", i,
      p->vnormals[i].xyz[0],
      p->vnormals[i].xyz[1],
      p->vnormals[i].xyz[2] );
  }
  else
    fprintf( stdout, "\n\t\tVertex Normals not specified\n" );

  if (p->vtexturecoord)
  {
    fprintf(stdout, "\n\t\tTexture Coordinates : ");
    for (i=0; i<p->num_vertices; i++)
      fprintf(stdout, "\n\t\t v[%d] = %g %g", i,
      p->vtexturecoord[i].xy[0],
      p->vtexturecoord[i].xy[1]);
  }
  else
    fprintf( stdout, "\n\t\tTexture Coordinates not specified\n");

  fprintf( stdout, "\n" );

  return TSuccess;
}

/*----------------------------------------------------------------------*/

static TStatus
PolygonInquire( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  Tint             i, j;
  tel_polygon_data d;
  Tint             size_reqd=0;
  TStatus          status = TSuccess;
  Tchar            *cur_ptr = 0;

  d = (tel_polygon_data)data.pdata;

  if( d->fcolour )
    size_reqd += sizeof( TEL_COLOUR );

  if( d->facet_flag == TEL_FA_NORMAL )
    size_reqd += sizeof( TEL_POINT );

  size_reqd += ( d->num_vertices * sizeof( TEL_POINT ) );

  if( d->vcolours )
    size_reqd += ( d->num_vertices * sizeof( TEL_COLOUR ) );

  if( d->vertex_flag == TEL_VT_NORMAL )
    size_reqd += ( d->num_vertices * sizeof( TEL_POINT ) );

  for( i = 0; i < n; i++ )
  {
    switch( k[i]->id )
    {
    case INQ_GET_SIZE_ID:
      {
        k[i]->data.ldata = size_reqd;
        break;
      }

    case INQ_GET_CONTENT_ID:
      {
        TEL_INQ_CONTENT *c;
        Teldata         *w;

        c = (tel_inq_content)k[i]->data.pdata;
        c->act_size = size_reqd;
        w = c->data;

        cur_ptr = c->buf;
        w->fillarea3data.num_points = d->num_vertices;
        w->fillarea3data.shpflag = d->shape_flag;

        if( c->size >= size_reqd )
        {
          if( d->facet_flag == TEL_FA_NORMAL )
          {
            if( d->fcolour )
            {
              w->fillarea3data.faflag = TEL_FAFLAG_COLOURNORMAL;
              w->fillarea3data.gnormal = (tel_point)(c->buf);
              *(w->fillarea3data.gnormal) = d->fnormal;
              w->fillarea3data.facet_colour_val =
                (tel_colour)(c->buf + sizeof( TEL_POINT ) );
              *(w->fillarea3data.facet_colour_val) = *(d->fcolour);
              cur_ptr = c->buf + sizeof( TEL_POINT ) +
                sizeof( TEL_COLOUR );
            }
            else
            {
              w->fillarea3data.faflag = TEL_FAFLAG_NORMAL;
              w->fillarea3data.facet_colour_val = 0;
              w->fillarea3data.gnormal = (tel_point)(c->buf);
              *(w->fillarea3data.gnormal) = d->fnormal;
              cur_ptr = c->buf + sizeof( TEL_POINT );
            }
          }
          else
          {
            w->fillarea3data.gnormal = 0;
            if( d->fcolour )
            {
              w->fillarea3data.faflag = TEL_FAFLAG_COLOUR;
              w->fillarea3data.facet_colour_val = (tel_colour)(c->buf );
              *(w->fillarea3data.facet_colour_val) = *(d->fcolour);
              cur_ptr = c->buf + sizeof( TEL_COLOUR );
            }
            else
            {
              w->fillarea3data.faflag = TEL_FAFLAG_NONE;
              w->fillarea3data.facet_colour_val = 0;
            }
          }

          w->fillarea3data.points = (tel_point)cur_ptr;
          for( j = 0; j < d->num_vertices; j++ )
          {
            w->fillarea3data.points[j] = d->vertices[j];
          }
          cur_ptr += ( d->num_vertices * sizeof( TEL_POINT ) );

          if( d->vertex_flag == TEL_VT_NORMAL )
          {
            if( d->vcolours )
            {
              w->fillarea3data.vrtflag = TEL_VTFLAG_COLOURNORMAL;
              w->fillarea3data.vnormals = (tel_point)(cur_ptr);
              for( j = 0; j < d->num_vertices; j++ )
              {
                w->fillarea3data.vnormals[j] = d->vnormals[i];
              }
              cur_ptr += ( d->num_vertices * sizeof( TEL_POINT ) );

              w->fillarea3data.colours = (tel_colour)(cur_ptr);

              for( j = 0; j < d->num_vertices; j++ )
              {
                w->fillarea3data.colours[j] = d->vcolours[i];
              }
            }
            else
            {
              w->fillarea3data.vrtflag = TEL_VTFLAG_NORMAL;
              w->fillarea3data.colours = 0;
              w->fillarea3data.vnormals = (tel_point)(cur_ptr);

              for( j = 0; j < d->num_vertices; j++ )
              {
                w->fillarea3data.vnormals[j] = d->vnormals[i];
              }
            }
          }
          else
          {
            w->fillarea3data.vnormals = 0;
            if( d->vcolours )
            {
              w->fillarea3data.vrtflag = TEL_VTFLAG_COLOUR;
              w->fillarea3data.colours = (tel_colour)(cur_ptr);
              for( j = 0; j < d->num_vertices; j++ )
              {
                w->fillarea3data.colours[j] = d->vcolours[i];
              }
            }
            else
            {
              w->fillarea3data.vrtflag = TEL_VTFLAG_NONE;
              w->fillarea3data.colours = 0;
            }
          }

          status = TSuccess;
        }
        else
          status = TFailure;
        break;
      }
    }
  }
  return status;
}
/*----------------------------------------------------------------------*/

static void
draw_edges( tel_polygon_data p, tel_colour edge_colour, Tint interior_style )
{
  CMN_KEY k, k1, k2, k3, k4;
  tel_point  ptr;
  Tint    i;
  Tint    edge_type, line_type_preserve;
  Tfloat  edge_width, line_width_preserve;
  GLboolean texture_on;

  if (interior_style != TSM_HIDDENLINE)
  {
    k.id = TelEdgeFlag;
    TsmGetAttri( 1, &k );
    if( k.data.ldata == TOff )return;
  } 

  ptr = p->vertices;

  LightOff();
  texture_on = IsTextureEnabled();
  if (texture_on) DisableTexture();

  k1.id  = TelPolylineWidth;
  k2.id  = TelPolylineType;
  k3.id  = TelEdgeType;
  k4.id  = TelEdgeWidth;

  TsmGetAttri( 4, &k1, &k2, &k3, &k4 );

  line_width_preserve = k1.data.fdata;
  line_type_preserve  = k2.data.ldata;
  edge_type           = k3.data.ldata;
  edge_width          = k4.data.fdata;

  if( line_width_preserve != edge_width )
  {
    k.id = TelPolylineWidth;
    k.data.fdata = edge_width;
    TsmSetAttri( 1, &k );
  }
  if( line_type_preserve != edge_type )
  {
    k.id = TelPolylineType;
    k.data.ldata = edge_type;
    TsmSetAttri( 1, &k );
  }

  glColor3fv( edge_colour->rgb );

  glBegin(GL_LINE_LOOP);
  for( i=0; i<p->num_vertices; i++, ptr++ )
  {
    glVertex3fv( ptr->xyz );
  }
  glEnd();

  if( line_width_preserve != edge_width )
  {
    k.id = TelPolylineWidth;
    k.data.fdata = line_width_preserve;
    TsmSetAttri( 1, &k );
  }
  if( line_type_preserve != edge_type )
  {
    k.id = TelPolylineType;
    k.data.ldata = line_type_preserve;
    TsmSetAttri( 1, &k );
  }

  if (texture_on) EnableTexture();
}
/*----------------------------------------------------------------------*/
