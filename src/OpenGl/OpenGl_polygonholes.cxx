/***********************************************************************

FONCTION :
----------
File OpenGl_polygonholes :


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : xxx ; Creation.
11-03-96 : FMN ; Correction warning compilation
01-04-96 : CAL ; Integration MINSK portage WNT
09-08-96 : FMN ; Suppression appel glMatrixMode() avant glGetFloatv()
21-10-96 : FMN ; Suppression LMC_COLOR fait dans OpenGl_execstruct.c
23-12-97 : FMN ; Suppression TelSetFrontFaceAttri et TelSetBackFaceAttri 
Suppression TelBackInteriorStyle, TelBackInteriorStyleIndex
et TelBackInteriorShadingMethod
03-03-98 : FMN ; Suppression variable externe TglWhetherPerspective 
08-03-01 : GG  ; BUC60823 Avoid crash in the normal computation method
on confuse point.

************************************************************************/

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
#include <OpenGl_Memory.hxx>

#ifndef WNT
#if !defined(APIENTRY)
# define APIENTRY
#endif // APIENTRY
# define STATIC static
#else
# include <GL\GLU.H>
# define STATIC
typedef double GLUcoord;
#endif  /* WNT */


/*----------------------------------------------------------------------*/
/*
* Variables statiques
*/ 

static  TStatus  PolygonHolesDisplay( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  PolygonHolesAdd( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  PolygonHolesDelete( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  PolygonHolesPrint( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  PolygonHolesInquire( TSM_ELEM_DATA, Tint, cmn_key* );

#if !defined(GLU_VERSION_1_2)
#define GLUtesselator GLUtriangulatorObj
void gluTessBeginContour();
void gluTessBeginPolygon();
void gluTessEndPolygon();
void gluTessEndContour();
#endif


/*----------------------------------------------------------------------*/
/*
* Typedef
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

struct TEL_POLYGON_HOLES_DATA
{
  Tint       num_vertices;    /* Number of vertices */
  Tint       facet_flag;      /* TEL_FA_NONE or TEL_FA_NORMAL */
  Tint       vertex_flag;     /* TEL_VT_NONE or TEL_VT_NORMAL */
  Tint       shape_flag;      /* TEL_SHAPE_UNKNOWN or TEL_SHAPE_COMPLEX or
                              TEL_SHAPE_CONVEX  or TEL_SHAPE_CONCAVE */
  TEL_POINT  fnormal;         /* Facet normal */
  Tint       *edgevis;        /* edge visibility flags */
  tel_colour fcolour;         /* Facet colour */
  Tint       num_bounds;      /* number of bounds */
  Tint       *bounds;         /* Array of length num_bounds */
  tel_point  vertices;        /* Vertices */
  tel_colour vcolours;        /* Vertex colour values */
  tel_point  vnormals;        /* Vertex normals */
  DISPLAY_   *display;        /* FSXXX plus utilise ? */
  GLuint     triangle_dl1;  /* triangle display list 1 */
  GLuint     triangle_dl2;  /* triangle display list 2 */
  GLuint     triangle_dl3;  /* triangle display list 3 */
};
typedef TEL_POLYGON_HOLES_DATA* tel_polygon_holes_data;

static TEL_POLYGON_HOLES_DATA polygon_holes_defaults =
{
  0,                 /* num_vertices */
  TEL_FA_NONE,       /* facet_flag */
  TEL_VT_NONE,       /* vertex_flag */
  TEL_SHAPE_UNKNOWN, /* shape_flag */
  {{ ( float )0.0, ( float )0.0, ( float )0.0 }}, /* fnormal */
  0,                 /* edgevis */
  0,                 /* fcolour */
  0,                 /* num_bounds */
  0,                 /* bounds */
  0,                 /* vertices */
  0,                 /* vcolours */
  0,                 /* vnormals */
  0,                 /* display */
  0,       /* triangle_dl1 */
  0,       /* triangle_dl2 */
  0      /* triangle_dl3 */
};

static void bgntriangulate( tel_polygon_holes_data, void (APIENTRY*)() );
static void endtriangulate(void);
STATIC void APIENTRY out_vertex1( void* );
STATIC void APIENTRY out_vertex2( void* );
STATIC void APIENTRY out_vertex3( void* );
static void draw_tmesh( tel_polygon_holes_data, Tint );
static void draw_polygon_holes( tel_polygon_holes_data, Tint,
                               Tint, Tint );
static void draw_edges( tel_polygon_holes_data, Tmatrix3, tel_colour, Tint,
                       TelCullMode, Tint, Tint );

static  TStatus  (*MtdTbl[])( TSM_ELEM_DATA, Tint, cmn_key* ) =
{
  PolygonHolesDisplay,             /* PickTraverse */
    PolygonHolesDisplay,
    PolygonHolesAdd,
    PolygonHolesDelete,
    PolygonHolesPrint,
    PolygonHolesInquire
};


MtblPtr
TelPolygonHolesInitClass( TelType* el )
{
  *el = TelPolygonHoles;
  return MtdTbl;
}

static  TStatus
PolygonHolesAdd( TSM_ELEM_DATA d, Tint n, cmn_key *k )
{
  Tint                   i, j, m;
  tel_polygon_holes_data data;

  for( i = 0; i < n; i++ )
  {
    if( k[i]->id == NUM_FACETS_ID )
      break;
  }
  if( i == n )
    return TFailure;

  if( !(k[i]->data.ldata) )
    return TFailure;

  for( j = 0; j < n; j++ )
  {
    if( k[j]->id == BOUNDS_DATA_ID )
      break;
  }
  if( j == n )
    return TFailure;

  data = new TEL_POLYGON_HOLES_DATA();
  if( !data )
    return TFailure;

  /* load defaults */
  memcpy( data, &polygon_holes_defaults, sizeof(TEL_POLYGON_HOLES_DATA) );

  data->num_bounds = k[i]->data.ldata;
  data->bounds = new Tint[data->num_bounds];
  memcpy( data->bounds, k[j]->data.pdata, data->num_bounds*sizeof(Tint) );

  for( m = 0, data->num_vertices = 0; m < data->num_bounds; m++ )
    data->num_vertices += data->bounds[m];

  for( i = 0; i < n; i++ )
  {
    if( k[i]->id == VERTICES_ID )
      break;
  }
  if( i == n )
    return TFailure;

  if( !(k[i]->data.ldata) )
    return TFailure;

  data->vertices = new TEL_POINT[data->num_vertices];
  memcpy( data->vertices, k[i]->data.pdata, data->num_vertices*sizeof(TEL_POINT) );

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

    case EDGE_DATA_ID:
      data->edgevis = new Tint[data->num_vertices];
      memcpy( data->edgevis, k[i]->data.pdata, sizeof(Tint) );
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
    }
  }

  if( data->facet_flag != TEL_FA_NORMAL )
  {
#ifdef BUC60823
    TelGetPolygonNormal( data->vertices, NULL, 
      data->num_vertices, data->fnormal.xyz );
#else
    TelGetNormal( data->vertices[0].xyz, data->vertices[1].xyz,
      data->vertices[2].xyz, data->fnormal.xyz );
    vecnrm( data->fnormal.xyz );
#endif
  }
  ((tsm_elem_data)(d.pdata))->pdata = data;

  return TSuccess;
}



static  TStatus
PolygonHolesDisplay( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  Tint           front_face = 1;
  Tmatrix3       mat;
  CMN_KEY        k11, k12, k13, k14, k16, k17, k19, k111,
                 k112, k113, k117;

  Tint           fl=0;
  Tint           front_lighting_model, back_lighting_model;
  Tint           front_lighting_model_preserve, back_lighting_model_preserve;
  Tint           front_shading_method;
  Tint           interior_style;
  Tint           interior_index;
  TEL_COLOUR     interior_colour, back_interior_colour;
  TEL_COLOUR     point_colour, edge_colour;
  TelCullMode    face_culling_mode;
  Tint           face_distinguishing_mode;

  tel_polygon_holes_data d;

  k11.id          = TelFaceDistinguishingMode;
  k12.id          = TelInteriorReflectanceEquation;
  k13.id          = TelInteriorShadingMethod;
  k14.id          = TelBackInteriorReflectanceEquation;
  k16.id          = TelFaceCullingMode;
  k17.id          = TelInteriorStyle;
  k19.id          = TelInteriorStyleIndex;
  k111.id         = TelInteriorColour;
  k111.data.pdata = &interior_colour;
  k112.id         = TelBackInteriorColour;
  k112.data.pdata = &back_interior_colour;
  k113.id         = TelPolymarkerColour;
  k113.data.pdata = &point_colour;
  k117.id         = TelEdgeColour;
  k117.data.pdata = &edge_colour;

  TsmGetAttri( 11, &k11, &k12, &k13, &k14, &k16, &k17,
    &k19, &k111, &k112, &k113, &k117 );

  face_distinguishing_mode = k11.data.ldata;
  front_lighting_model     = k12.data.ldata;
  front_lighting_model_preserve = k12.data.ldata;
  front_shading_method     = k13.data.ldata;
  back_lighting_model      = k14.data.ldata;
  back_lighting_model_preserve = k14.data.ldata;
  face_culling_mode        = (TelCullMode)k16.data.ldata;
  interior_style           = k17.data.ldata;
  interior_index           = k19.data.ldata;

  if( k[0]->id == TOn )
  {                          /* Use highlight colours */
    TEL_HIGHLIGHT  hrep;
    CMN_KEY kkk, kkl;

    k11.id = TelHighlightIndex;
    TsmGetAttri( 1, &k11 );
    if( TelGetHighlightRep( TglActiveWs, k11.data.ldata, &hrep )
      == TSuccess )
    {
      if( hrep.type == TelHLForcedColour )
      {
        edge_colour = back_interior_colour = interior_colour = hrep.col;
        front_lighting_model = back_lighting_model = CALL_PHIGS_REFL_NONE;
        kkk.id = TelInteriorReflectanceEquation;
        kkk.data.ldata = CALL_PHIGS_REFL_NONE;
        kkl.id = TelBackInteriorReflectanceEquation;
        kkl.data.ldata = CALL_PHIGS_REFL_NONE;
        TsmSetAttri( 2, &kkk, &kkl );
        fl = 0;
      }
      else
      {
        edge_colour = hrep.col;
        k[0]->id = TOff;
      }
    }
    else
    {
      TelGetHighlightRep( TglActiveWs, 0, &hrep );
      if( hrep.type == TelHLForcedColour )
      {
        back_interior_colour = interior_colour = hrep.col;
        front_lighting_model = back_lighting_model = CALL_PHIGS_REFL_NONE;
        kkk.id = TelInteriorReflectanceEquation;
        kkk.data.ldata = CALL_PHIGS_REFL_NONE;
        kkl.id = TelBackInteriorReflectanceEquation;
        kkl.data.ldata = CALL_PHIGS_REFL_NONE;
        TsmSetAttri( 2, &kkk, &kkl );
        fl = 0;
      }
      else
        k[0]->id = TOff;
    }
  }

  d = (tel_polygon_holes_data)data.pdata;

  glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *) mat);

  if( face_distinguishing_mode == TOn )
  {
    if( ( face_culling_mode == TelCullBack && front_face ) ||
      ( face_culling_mode == TelCullNone && front_face )   )
    {
      glColor3fv( interior_colour.rgb );

      draw_polygon_holes(d, interior_style, front_lighting_model, k[0]->id);
    }
    else if( ( face_culling_mode == TelCullFront && !front_face ) ||
      ( face_culling_mode == TelCullNone  && !front_face ) )
    {
      /* CMN_KEY k; */
      glColor3fv( back_interior_colour.rgb );

      draw_polygon_holes( d, interior_style,
        back_lighting_model, k[0]->id);
    }
  }
  else
  {
    if( ( face_culling_mode == TelCullBack  && front_face  ) ||
      ( face_culling_mode == TelCullFront && !front_face ) ||
      ( face_culling_mode == TelCullNone )                    )
    {
      glColor3fv( interior_colour.rgb );

      draw_polygon_holes(d, interior_style, front_lighting_model, k[0]->id);
    }
  }


  /*A  remplacer par le call-back GLU_EDGE_FLAG , A TESTER */
  draw_edges( d, mat, &edge_colour, face_distinguishing_mode,
    face_culling_mode, interior_style, interior_style );

  if( fl )
  {
    CMN_KEY kkk, kkl;

    kkk.id = TelInteriorReflectanceEquation;
    kkk.data.ldata = front_lighting_model_preserve;
    kkl.id = TelBackInteriorReflectanceEquation;
    kkl.data.ldata = back_lighting_model_preserve;
    TsmSetAttri( 2, &kkk, &kkl );
  }

  return TSuccess;

}

static GLUtesselator *tripak;

static void
draw_polygon_holes( tel_polygon_holes_data d,
                   Tint style, Tint u, Tint hflag )
{
  long      i, j, m;

  GLdouble  xyz[3];

  tel_point  pvn;
  tel_colour pfc, pvc;

  pfc = d->fcolour;
  pvc = d->vcolours;
  pvn = d->vnormals;

  if( hflag )
  {
    pvc = pfc = 0;
  }

  if( pfc )
    glColor3fv( pfc->rgb );
  if( u != CALL_PHIGS_REFL_NONE )
    glNormal3fv( d->fnormal.xyz );

  if( d->display == 0 )
  {

    if( u == CALL_PHIGS_REFL_NONE )
    {
      if( pvc )
      {
        bgntriangulate( d, (void (APIENTRY*)())out_vertex2 );
      }
      else
      {
        bgntriangulate( d, (void (APIENTRY*)())out_vertex1 );
      }
    }
    else
    {
      if( pvn )
      {
        bgntriangulate( d, (void (APIENTRY*)())out_vertex3 );
      }
      else
      {
        bgntriangulate( d, (void (APIENTRY*)())out_vertex1 );
      }
    }
    gluTessBeginPolygon( tripak, NULL );
    for( i = 0, m = 0; i < d->num_bounds; i++ )
    {
      gluTessBeginContour( tripak );
      for( j = 0; j < d->bounds[i]; j++ )
      {
        xyz[0] = d->vertices[m].xyz[0];
        xyz[1] = d->vertices[m].xyz[1];
        xyz[2] = d->vertices[m].xyz[2];
#ifndef WNT
        gluTessVertex( tripak, xyz, (void *)m );
#else
        {
          GLdouble v[ 3 ];
          v[ 0 ] = d -> vertices[ m ].xyz[ 0 ];
          v[ 1 ] = d -> vertices[ m ].xyz[ 1 ];
          v[ 2 ] = d -> vertices[ m ].xyz[ 2 ];

          gluTessVertex (  tripak,  v, ( void* )m  );
        }
#endif  /* WNT */
        m++;
      }
      gluTessEndContour( tripak );
    }
    gluTessEndPolygon( tripak );
    endtriangulate();
  }
  else
  {
    if( u == CALL_PHIGS_REFL_NONE )
    {
      if( pvc )
        draw_tmesh( d, 2 );
      else
        draw_tmesh( d, 1 );
    }
    else
    {
      if( pvn )
        draw_tmesh( d, 3 );
      else
        draw_tmesh( d, 1 );
    }
  }


}

static void
draw_tmesh( tel_polygon_holes_data d, Tint v )
{
  Tint     i, j, k;
  DISPLAY_ *dis;
  SEQ_     *s;

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
          if ( s->tmesh_sequence[j] < (void *)0xffff ) {
            glVertex3fv( d->vertices[ (long)s->tmesh_sequence[ j ] ].xyz );
          }
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
            extra_vertex b = (extra_vertex)s->tmesh_sequence[j];
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
          if ( s->tmesh_sequence[j] < (void *)0xffff) {
            glNormal3fv( d->vnormals[ (long) s->tmesh_sequence[ j ] ].xyz );
            glVertex3fv( d->vertices[ (long) s->tmesh_sequence[ j ] ].xyz );
          } else {
            extra_vertex b = (extra_vertex)s->tmesh_sequence[j];
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


static  TStatus
PolygonHolesDelete( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  tel_polygon_holes_data p = (tel_polygon_holes_data)data.pdata;
  if( p->fcolour )
    //cmn_freemem( p->fcolour );
    delete p->fcolour;
  if( p->vertices )
    //cmn_freemem( p->vertices );
    delete[] p->vertices;
  if( p->vcolours )
    //cmn_freemem( p->vcolours );
    delete[] p->vcolours;
  if( p->vnormals )
    //cmn_freemem( p->vnormals );
    delete[] p->vnormals;
  if( p->bounds )
    //cmn_freemem( p->bounds );
    delete[] p->bounds;
  if( p->edgevis )
    //cmn_freemem( p->edgevis );
    delete[] p->edgevis;
  if( p->display )
  {
    Tint i, j;

    for( i = 0; i <  p->display->num_of_seq; i++ )
    {
      if( p->display->seq[i].tmesh_sequence ) {
        for ( j = 0; j < p->display->seq[i].ts_num ; j++ ) {
          if ( p->display->seq[i].tmesh_sequence[j]  >= (void *)0xffff )
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


static  TStatus
PolygonHolesPrint( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  Tint                   i;
  tel_polygon_holes_data p = (tel_polygon_holes_data)data.pdata;

  fprintf( stdout, "TelPolygonHoles. Number of bounds: %d\n", p->num_bounds );
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
  if( p->fcolour )
    fprintf( stdout, "\t\tFacet Colour : %g %g %g\n", p->fcolour->rgb[0],
    p->fcolour->rgb[1],
    p->fcolour->rgb[2] );
  else
    fprintf( stdout, "\n\t\tFacet Colour not specified\n" );

  if( p->edgevis )
  {
    fprintf( stdout, "\t\tEdge Visibility Data :\n" );
    for( i = 0; i < p->num_vertices; i++ )
      fprintf( stdout, "\t\t%d ", p->edgevis[i] );
    fprintf( stdout, "\n" );
  }

  if( p->bounds )
  {
    fprintf( stdout, "\t\tBounds array :\n" );
    for( i = 0; i < p->num_bounds; i++ )
      fprintf( stdout, "\t\tb[%d] %d \n", i, p->bounds[i] );
  }

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
  fprintf( stdout, "\n" );
  return TSuccess;
}


static TStatus
PolygonHolesInquire( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  Tint             i, j;
  Tint             size_reqd=0;
  TStatus          status = TSuccess;
  Tchar            *cur_ptr = 0;
  tel_polygon_holes_data d = (tel_polygon_holes_data)data.pdata;

  if( d->fcolour )
    size_reqd += sizeof( TEL_COLOUR );

  if( d->facet_flag == TEL_FA_NORMAL )
    size_reqd += sizeof( TEL_POINT );

  if( d->edgevis )
    size_reqd += ( d->num_vertices * sizeof( Tint ) );

  size_reqd += ( d->num_bounds * sizeof( Tint ) );

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
        w->fillareaset3data.num_bounds = d->num_bounds;
        w->fillareaset3data.shpflag = d->shape_flag;

        if( d->edgevis )
          w->fillareaset3data.edgflag = TOn;
        else
          w->fillareaset3data.edgflag = TOff;

        if( c->size >= size_reqd )
        {
          if( d->facet_flag == TEL_FA_NORMAL )
          {
            if( d->fcolour )
            {
              w->fillareaset3data.faflag = TEL_FAFLAG_COLOURNORMAL;
              w->fillareaset3data.gnormal = (tel_point)(c->buf);
              *(w->fillareaset3data.gnormal) = d->fnormal;
              w->fillareaset3data.facet_colour_val =
                (tel_colour)(c->buf + sizeof( TEL_POINT ) );
              *(w->fillareaset3data.facet_colour_val) = *(d->fcolour);
              cur_ptr = c->buf + sizeof( TEL_POINT ) +
                sizeof( TEL_COLOUR );
            }
            else
            {
              w->fillareaset3data.faflag = TEL_FAFLAG_NORMAL;
              w->fillareaset3data.facet_colour_val = 0;
              w->fillareaset3data.gnormal = (tel_point)(c->buf);
              *(w->fillareaset3data.gnormal) = d->fnormal;
              cur_ptr = c->buf + sizeof( TEL_POINT );
            }
          }
          else
          {
            w->fillareaset3data.gnormal = 0;
            if( d->fcolour )
            {
              w->fillareaset3data.faflag = TEL_FAFLAG_COLOUR;
              w->fillareaset3data.facet_colour_val = (tel_colour)(c->buf );
              *(w->fillareaset3data.facet_colour_val) = *(d->fcolour);
              cur_ptr = c->buf + sizeof( TEL_COLOUR );
            }
            else
            {
              w->fillareaset3data.faflag = TEL_FAFLAG_NONE;
              w->fillareaset3data.facet_colour_val = 0;
            }
          }

          w->fillareaset3data.points = (tel_point)cur_ptr;
          for( j = 0; j < d->num_vertices; j++ )
          {
            w->fillareaset3data.points[j] = d->vertices[j];
          }
          cur_ptr += ( d->num_vertices * sizeof( TEL_POINT ) );

          if( d->edgevis )
          {
            w->fillareaset3data.edgvis = (Tint *)(cur_ptr);
            memcpy( w->fillareaset3data.edgvis, d->edgevis,
              d->num_vertices*sizeof(Tint) );
            cur_ptr += (d->num_vertices * sizeof( Tint ) );
          }

          w->fillareaset3data.bounds = (Tint *)cur_ptr;
          memcpy( w->fillareaset3data.bounds, d->bounds,
            d->num_bounds*sizeof(Tint) );
          cur_ptr += ( d->num_bounds * sizeof( Tint ) );

          if( d->vertex_flag == TEL_VT_NORMAL )
          {
            if( d->vcolours )
            {
              w->fillareaset3data.vrtflag = TEL_VTFLAG_COLOURNORMAL;
              w->fillareaset3data.vnormals = (tel_point)(cur_ptr);
              for( j = 0; j < d->num_vertices; j++ )
              {
                w->fillareaset3data.vnormals[j] = d->vnormals[i];
              }
              cur_ptr += ( d->num_vertices * sizeof( TEL_POINT ) );

              w->fillareaset3data.colours = (tel_colour)(cur_ptr);

              for( j = 0; j < d->num_vertices; j++ )
              {
                w->fillareaset3data.colours[j] = d->vcolours[i];
              }
            }
            else
            {
              w->fillareaset3data.vrtflag = TEL_VTFLAG_NORMAL;
              w->fillareaset3data.colours = 0;
              w->fillareaset3data.vnormals = (tel_point)(cur_ptr);

              for( j = 0; j < d->num_vertices; j++ )
              {
                w->fillareaset3data.vnormals[j] = d->vnormals[i];
              }
            }
          }
          else
          {
            w->fillareaset3data.vnormals = 0;
            if( d->vcolours )
            {
              w->fillareaset3data.vrtflag = TEL_VTFLAG_COLOUR;
              w->fillareaset3data.colours = (tel_colour)(cur_ptr);
              for( j = 0; j < d->num_vertices; j++ )
              {
                w->fillareaset3data.colours[j] = d->vcolours[i];
              }
            }
            else
            {
              w->fillareaset3data.vrtflag = TEL_VTFLAG_NONE;
              w->fillareaset3data.colours = 0;
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



#define INCREMENT    64
static tel_polygon_holes_data DaTa;

STATIC void APIENTRY
out_bgntmesh_1 ( GLenum triangle_type )
{

  DISPLAY_ *dis;
  dis = DaTa->display;

  dis->num_of_seq++;
  if( dis->num_alloc < dis->num_of_seq )
  {
    dis->num_alloc += INCREMENT;

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
  dis->seq[ dis->num_of_seq - 1 ].triangle_type = triangle_type;
  glBegin(triangle_type);
}


STATIC void APIENTRY
out_vertex1( void *data )
{
  SEQ_ *s;

  s = &( DaTa->display->seq[ DaTa->display->num_of_seq - 1 ] );

  s->ts_num++;
  if( s->ts_alloc < s->ts_num )
  {
    s->ts_alloc += INCREMENT;

    if( s->tmesh_sequence == 0 )
    {
      s->tmesh_sequence = new void*[s->ts_alloc];
    }
    else
    {
#if defined(__SUNPRO_CC) && (__SUNPRO_CC <= 0x530)
      s->tmesh_sequence = (void**)realloc( s->tmesh_sequence,
        s->ts_alloc*sizeof(void*));
#else
      s->tmesh_sequence = cmn_resizemem<void*>( s->tmesh_sequence,
        s->ts_alloc);
#endif          
    }
  }
  s->tmesh_sequence[ s->ts_num - 1 ] = data;

  if ( data < (void *)0xffff ) {
    long int a = (long int)data;

    glVertex3fv( DaTa->vertices[a].xyz );
  }
  else {
    extra_vertex b = (extra_vertex)data;

    glVertex3fv(b->vert);
  }
}

STATIC void APIENTRY
out_vertex2( void *data )
{
  SEQ_ *s;

  s = &( DaTa->display->seq[ DaTa->display->num_of_seq - 1 ] );

  s->ts_num++;
  if( s->ts_alloc < s->ts_num )
  {
    s->ts_alloc += INCREMENT;

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

  if ( data < (void *)0xffff ){
    long int a = (long int)data;

    glColor3fv( DaTa->vcolours[a].rgb );
    glVertex3fv( DaTa->vertices[a].xyz );
  }
  else {
    extra_vertex b = (extra_vertex)data;

    glColor3fv(DaTa->vcolours[b->ind].rgb );
    glVertex3fv(b->vert);
  }
}

STATIC void APIENTRY
out_vertex3( void *data )
{
  SEQ_ *s;

  s = &( DaTa->display->seq[ DaTa->display->num_of_seq - 1 ] );

  s->ts_num++;
  if( s->ts_alloc < s->ts_num )
  {
    s->ts_alloc += INCREMENT;

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

  if ( data < (void *)0xffff ) {
    long int a = (long int)data;

    glNormal3fv( DaTa->vnormals[a].xyz );
    glVertex3fv( DaTa->vertices[a].xyz );
  }
  else {
    extra_vertex b = (extra_vertex) data;

    glNormal3fv( DaTa->vnormals[b->ind].xyz );
    glVertex3fv(b->vert);
  }

}

STATIC void APIENTRY
out_combine( GLdouble coords[3], int *data,
            GLfloat w[4], void **dataout )
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
#endif


}



STATIC void APIENTRY
out_endtmesh_1( void )
{
  glEnd();
}

STATIC void APIENTRY
out_error_1( GLenum error )
{
  printf( "%s\n", (char *) gluErrorString(error) );
}

static void
bgntriangulate( tel_polygon_holes_data d, void (APIENTRY*out_ver)() )
{
  DaTa = d;
  if( !DaTa->display )
    DaTa->display = new DISPLAY_();

  tripak = gluNewTess();
#if defined(linux) && !defined(NOGLUfuncptr)
  gluTessCallback(tripak, GLU_TESS_BEGIN, (_GLUfuncptr)(out_bgntmesh_1));
  gluTessCallback(tripak, GLU_TESS_VERTEX, out_ver);
  gluTessCallback(tripak, GLU_TESS_END, out_endtmesh_1);
  gluTessCallback(tripak, GLU_TESS_ERROR, (_GLUfuncptr)(out_error_1));
  gluTessCallback(tripak, GLU_TESS_COMBINE, (_GLUfuncptr)(out_combine));
#else 
  gluTessCallback(tripak, GLU_TESS_BEGIN, (void (APIENTRY*)())out_bgntmesh_1);
  gluTessCallback(tripak, GLU_TESS_VERTEX, (void (APIENTRY*)())out_ver);
  gluTessCallback(tripak, GLU_TESS_END, (void (APIENTRY*)())out_endtmesh_1);
  gluTessCallback(tripak, GLU_TESS_ERROR, (void (APIENTRY*)())out_error_1);
  gluTessCallback(tripak, GLU_TESS_COMBINE, (void (APIENTRY*)())out_combine);
#endif

}

static void
endtriangulate(void)
{
  DaTa = 0;
  gluDeleteTess(tripak);
}

static void
draw_edges( tel_polygon_holes_data p, Tmatrix3 mat, tel_colour edge_colour,
           Tint face_distinguishing_mode, TelCullMode face_culling_mode,
           Tint interior_style, Tint back_interior_style )
{
  CMN_KEY k, k1, k2, k3, k4;
  Tint    i, j, a, clos;
  Tint    edge_type, line_type_preserve;
  Tfloat  edge_width, line_width_preserve;
  /* Following pointers have been provided for performance improvement */
  Tint       *bounds;
  tel_point  pv;

  /* OCC11904 -- Temporarily disable environment mapping */
  glPushAttrib(GL_ENABLE_BIT);
  glDisable(GL_TEXTURE_1D);
  glDisable(GL_TEXTURE_2D);


  bounds = p->bounds;
  pv  = p->vertices;

  k.id = TelEdgeFlag;
  TsmGetAttri( 1, &k );

  if( !p->edgevis )
    if( k.data.ldata == TOff )
    {
      glPopAttrib();
      return;
    }

    k1.id  = TelPolylineWidth;
    k2.id  = TelPolylineType;
    k3.id  = TelEdgeType;
    k4.id  = TelEdgeWidth;

    TsmGetAttri( 4, &k1, &k2, &k3, &k4 );

    line_width_preserve = k1.data.fdata;
    line_type_preserve  = k2.data.ldata;
    edge_type           = k3.data.ldata;
    edge_width          = k4.data.fdata;

    if( p->edgevis == 0 )
    {
      if( face_distinguishing_mode == TOff )
      {
        if( interior_style == TSM_SOLID || interior_style == TSM_HATCH )
        {
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
          if( face_culling_mode == TelCullBack )
          {
            for( i = 0, a = 0; i < p->num_bounds; i++ )
            {
              glBegin(GL_LINE_STRIP);
              for( clos=a, j = 0; j < bounds[i]; j++ )
              {
                glVertex3fv( pv[ a++ ].xyz );
              }
              glVertex3fv( pv[ clos ].xyz );
              glEnd();
            }
          }
          else if( face_culling_mode == TelCullFront )
          {
            for( i = 0, a = 0; i < p->num_bounds; i++ )
            {
              glBegin(GL_LINE_STRIP);
              for( clos=a, j = 0; j < bounds[i]; j++ )
              {
                glVertex3fv( pv[ a++ ].xyz );
              }
              glVertex3fv( pv[ clos ].xyz );
              glEnd();
            }
          }
          else
          {
            for( i = 0, a = 0; i < p->num_bounds; i++ )
            {
              glBegin(GL_LINE_STRIP);
              for( clos=a, j = 0; j < bounds[i]; j++ )
              {
                glVertex3fv( pv[ a++ ].xyz );
              }
              glVertex3fv( pv[ clos ].xyz );
              glEnd();
            }
          }
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
        }
      }
      else
      {
        if( ( interior_style == TSM_SOLID      ||
          interior_style == TSM_HATCH         ) ||
          ( back_interior_style == TSM_SOLID ||
          back_interior_style == TSM_HATCH    ) )
        {
          if( face_culling_mode == TelCullBack )
          {
            if( interior_style == TSM_SOLID || interior_style == TSM_HATCH )
            {
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
              for( i = 0, a = 0; i < p->num_bounds; i++ )
              {
                glBegin(GL_LINE_STRIP);
                for( clos=a, j = 0; j < bounds[i]; j++ )
                {
                  glVertex3fv( pv[ a++ ].xyz );
                }
                glVertex3fv( pv[ clos ].xyz );
                glEnd();
              }
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
            }
          }
          else if( face_culling_mode == TelCullFront )
          {
            if( back_interior_style == TSM_SOLID ||
              back_interior_style == TSM_HATCH )
            {
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
              for( i = 0, a = 0; i < p->num_bounds; i++ )
              {
                glBegin(GL_LINE_STRIP);
                for( clos=a, j = 0; j < bounds[i]; j++ )
                {
                  glVertex3fv( pv[ a++ ].xyz );
                }
                glVertex3fv( pv[ clos ].xyz );
                glEnd();
              }
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
            }
          }
          else
          {
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
            for( i = 0, a = 0; i < p->num_bounds; i++ )
            {
              glBegin(GL_LINE_STRIP);
              for( clos=a, j = 0; j < bounds[i]; j++ )
              {
                glVertex3fv( pv[ a++ ].xyz );
              }
              glVertex3fv( pv[ clos ].xyz );
              glEnd();
            }
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
          }
        }
      }
    }
    else
    {
      if( face_distinguishing_mode == TOff )
      {
        if( interior_style == TSM_SOLID || interior_style == TSM_HATCH )
        {
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
          if( face_culling_mode == TelCullBack )
          {
            glBegin(GL_LINES);
            for( i = 0, j = 0; i < p->num_bounds; i++ )
            {
              for( a = 0; a < bounds[i]-1; a++ )
              {
                if( p->edgevis[j+a] )
                {
                  glVertex3fv( pv[j+a].xyz );
                  glVertex3fv( pv[j+a+1].xyz );
                }
              }
              if( p->edgevis[j+a] )
              {
                glVertex3fv( pv[j+a].xyz );
                glVertex3fv( pv[j].xyz );
              }
              j += p->bounds[i];
            }
            glEnd();
          }
          else if( face_culling_mode == TelCullFront )
          {
            glBegin(GL_LINES);
            for( i = 0, j = 0; i < p->num_bounds; i++ )
            {
              for( a = 0; a < bounds[i]-1; a++ )
              {
                if( p->edgevis[j+a] )
                {
                  glVertex3fv( pv[j+a].xyz );
                  glVertex3fv( pv[j+a+1].xyz );
                }
              }
              if( p->edgevis[j+a] )
              {
                glVertex3fv( pv[j+a].xyz );
                glVertex3fv( pv[j].xyz );
              }
              j += p->bounds[i];
            }
            glEnd();
          }
          else
          {
            glBegin(GL_LINES);
            for( i = 0, j = 0; i < p->num_bounds; i++ )
            {
              for( a = 0; a < bounds[i]-1; a++ )
              {
                if( p->edgevis[j+a] )
                {
                  glVertex3fv( pv[j+a].xyz );
                  glVertex3fv( pv[j+a+1].xyz );
                }
              }
              if( p->edgevis[j+a] )
              {
                glVertex3fv( pv[j+a].xyz );
                glVertex3fv( pv[j].xyz );
              }
              j += p->bounds[i];
            }
            glEnd();
          }
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
        }
      }
      else
      {
        if( ( interior_style == TSM_SOLID      ||
          interior_style == TSM_HATCH         ) ||
          ( back_interior_style == TSM_SOLID ||
          back_interior_style == TSM_HATCH    ) )
        {
          if( face_culling_mode == TelCullBack )
          {
            if( interior_style == TSM_SOLID || interior_style == TSM_HATCH )
            {
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
              glBegin(GL_LINES);
              for( i = 0, j = 0; i < p->num_bounds; i++ )
              {
                for( a = 0; a < bounds[i]-1; a++ )
                {
                  if( p->edgevis[j+a] )
                  {
                    glVertex3fv( pv[j+a].xyz );
                    glVertex3fv( pv[j+a+1].xyz );
                  }
                }
                if( p->edgevis[j+a] )
                {
                  glVertex3fv( pv[j+a].xyz );
                  glVertex3fv( pv[j].xyz );
                }
                j += p->bounds[i];
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
            }
          }
          else if( face_culling_mode == TelCullFront )
          {
            if( back_interior_style == TSM_SOLID ||
              back_interior_style == TSM_HATCH )
            {
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
              glBegin(GL_LINES);
              for( i = 0, j = 0; i < p->num_bounds; i++ )
              {
                for( a = 0; a < bounds[i]-1; a++ )
                {
                  if( p->edgevis[j+a] )
                  {
                    glVertex3fv( pv[j+a].xyz );
                    glVertex3fv( pv[j+a+1].xyz );
                  }
                }
                if( p->edgevis[j+a] )
                {
                  glVertex3fv( pv[j+a].xyz );
                  glVertex3fv( pv[j].xyz );
                }
                j += p->bounds[i];
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
            }
          }
          else
          {
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
            for( i = 0, j = 0; i < p->num_bounds; i++ )
            {
              glBegin(GL_LINES);
              for( a = 0; a < bounds[i]-1; a++ )
              {
                if( p->edgevis[j+a] )
                {
                  glVertex3fv( pv[j+a].xyz );
                  glVertex3fv( pv[j+a+1].xyz );
                }
              }
              if( p->edgevis[j+a] )
              {
                glVertex3fv( pv[j+a].xyz );
                glVertex3fv( pv[j].xyz );
              }
              glEnd();
              j += p->bounds[i];
            }

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
          }
        }
      }
    }
    glPopAttrib();
}
