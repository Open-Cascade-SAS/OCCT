/***********************************************************************

FONCTION :
----------
File OpenGl_indexpolygons :


REMARQUES:
---------- 

Le culling et le backfacing ne marchent pas.


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : xxx ; Creation.
11-03-96 : FMN ; Correction warning compilation
01-04-96 : CAL ; Integration MINSK portage WNT
09-08-96 : FMN ; Suppression appel glMatrixMode() avant glGetFloatv()
21-10-96 : FMN ; Suppression LMC_COLOR fait dans OpenGl_execstruct.c
30-06-97 : FMN ; Suppression OpenGl_telem_light.h
18-07-97 : FMN ; Ajout desactivation des lights suivant front_lighting_model
21-07-97 : FMN ; Amelioration des performances OPTIMISATION_FMN
- suppression calcul inutile sur les front faces
- utilisation de GL_TRIANGLES et GL_QUADS
27-08-97 : FMN ; Correction affichage des edges
On n'affiche que les edges dans le mode IS_EMPTY
27-08-97 : FMN ; Correction affichage des edges visibility [PRO9859]
10-09-97 : FMN ; Amelioration des perfs liees aux lights.
15-09-97 : PCT ; Ajout coordonnees textures
24-09-97 : FMN ; Suppression OPTIMISATION_FMN.
08-12-97 : FMN ; Suppression appel TsmGetAttri inutile.
31-12-97 : FMN ; Simplification pour le highlight
15-01-98 : FMN ; Ajout Hidden line
08-03-01 : GG  ; BUC60823 Avoid crash in the normal computation method
on confuse point.

************************************************************************/

#define xPRINT

#define G003  /* EUG 07-10-99 Degeneration mode support 
*/

#define G004  /* VKH 25-01-00 View dump management
Disable animation during dump
*/

#define BUC60876  /* GG 5/4/2001 Disable local display list
//      when animation is not required
*/

#define OCC749          /* SAV 19/09/02 added processing of colored vertices */
#define OCC7824         /* ASL 26/01/05 transparency of polygon with colors assigned to vertices */

/*----------------------------------------------------------------------*/
/*
* Includes
*/ 

#include <OpenGl_tgl_all.hxx>

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <OpenGl_cmn_varargs.hxx>
//#include <OpenGl_cmn_memory.hxx>
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

#ifdef G003
# include <float.h>
# define DEF_DS_INTERNAL
# include <OpenGl_degeneration.hxx>
# ifdef G004
extern GLboolean g_fBitmap;
# endif /* G004 */
#endif  /* G003 */

static long s_Rand = 1L;
# define OPENGL_RAND() (  ( unsigned )( s_Rand = s_Rand * 214013L + 2531011L ))

/*----------------------------------------------------------------------*/
/*
* Constantes
*/ 

/*----------------------------------------------------------------------*/
/*
* Prototypes
*/ 

static  TStatus  PolygonIndicesDisplay( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  PolygonIndicesAdd( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  PolygonIndicesDelete( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  PolygonIndicesPrint( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  PolygonIndicesInquire( TSM_ELEM_DATA, Tint, cmn_key* );

/*static  GLboolean   lighting_mode;*/

struct TEL_INDEXPOLY_DATA
{
  Tint       num_vertices;   /* Number of vertices */
  Tint       num_bounds;     /* Number of bounds */
  Tint       facet_flag;     /* TEL_FA_NONE or TEL_FA_NORMAL */
  Tint       vertex_flag;    /* TEL_VT_NONE or TEL_VT_NORMAL */
  Tint       shape_flag;     /* TEL_SHAPE_UNKNOWN or TEL_SHAPE_COMPLEX or
                             TEL_SHAPE_CONVEX  or TEL_SHAPE_CONCAVE */
  Tint       *edge_vis;      /* Edge visibility indicators for each edge */
  Tint       *bounds;        /* Bounds array */
  Tint       *indices;       /* Connectivity array */
  tel_point  fnormals;       /* Facet normals */
  tel_colour fcolours;       /* Facet colour values */
  tel_point  vertices;       /* Vertices */
  tel_colour vcolours;       /* Vertex colour values */
  tel_point  vnormals;       /* Vertex normals */
  tel_texture_coord vtexturecoord; /* Texture Coordinates */
  Tint       edge_count;     /* Internal field */
#ifdef G003
  DS_INTERNAL d;
#endif  /* G003 */
  IMPLEMENT_MEMORY_OPERATORS
};
typedef TEL_INDEXPOLY_DATA* tel_indexpoly_data;

static TEL_INDEXPOLY_DATA indexpoly_defaults =
{
  0,                    /* num_vertices */
  0,                    /* num_bounds */
  TEL_FA_NONE,          /* facet_flag */
  TEL_VT_NONE,          /* vertex_flag */
  TEL_SHAPE_UNKNOWN,    /* shape_flag */
  0,                    /* edge_vis */
  0,                    /* bounds */
  0,                    /* indices */
  0,                    /* fnormal */
  0,                    /* fcolour */
  0,                    /* vertices */
  0,                    /* vcolours */
  0,                    /* vnormals */
  0,                    /* vtexturecoord */
  0                     /* edge_count */
#ifdef G003
  , { 0, 0, 0, -1, 0.0F, NULL }
#endif  /* G003 */
};

static void draw_indexpoly(
                           tel_indexpoly_data,
                           Tint,          /* highlight_flag */
                           Tint,          /* front_lighting_model,  */
                           Tint,          /* interior_style,  */
                           tel_colour     /* edge_colour, */
#ifdef OCC749
                           , tel_surf_prop
#endif
                           );

#ifdef G003
static void draw_edges                 ( tel_indexpoly_data, tel_colour, Tint, Tint );
static void draw_degenerates_as_points ( tel_indexpoly_data                         );
static void draw_degenerates_as_bboxs  ( tel_indexpoly_data                         );
void set_drawable_items         ( GLboolean*, int                            );
#else
static void draw_edges( tel_indexpoly_data,tel_colour, Tint);
#endif

static  TStatus  (*MtdTbl[])( TSM_ELEM_DATA, Tint, cmn_key* ) =
{
  PolygonIndicesDisplay,             /* PickTraverse */
    PolygonIndicesDisplay,
    PolygonIndicesAdd,
    PolygonIndicesDelete,
    PolygonIndicesPrint,
    PolygonIndicesInquire
};

/*----------------------------------------------------------------------*/
/*
* Variables externes
*/

extern  Tint  ForbidSetTextureMapping; /* currently defined in tsm/tsm.c */
extern int   g_nDegenerateModel;

#ifdef G003
extern float g_fSkipRatio;
extern GLboolean g_fAnimation;
#endif  /* G003 */

/*----------------------------------------------------------------------*/

MtblPtr
TelPolygonIndicesInitClass( TelType* el )
{
  *el = TelPolygonIndices;
  return MtdTbl;
}

/*----------------------------------------------------------------------*/

static  TStatus
PolygonIndicesAdd( TSM_ELEM_DATA d, Tint n, cmn_key *k )
{
  Tint             i, j, a, b, edge_count=0;
  tel_indexpoly_data data;

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

  for( a = 0; a < n; a++ )
  {
    if( k[a]->id == NUM_FACETS_ID )
      break;
  }
  if( a == n )
    return TFailure;

  //cmn_memreserve( data, 1, 1 );
  data = new TEL_INDEXPOLY_DATA();
  if( !data )
    return TFailure;

  /* load defaults */
  //cmn_memcpy<TEL_INDEXPOLY_DATA>( data, &indexpoly_defaults, 1 );
  memcpy( data, &indexpoly_defaults, sizeof(TEL_INDEXPOLY_DATA) );

  data->num_vertices = k[i]->data.ldata;
  //cmn_memreserve( data->vertices, data->num_vertices, 0 );
  data->vertices = new TEL_POINT[data->num_vertices];  
  //cmn_memcpy<TEL_POINT>( data->vertices, k[j]->data.pdata, data->num_vertices );
  memcpy( data->vertices, k[j]->data.pdata, data->num_vertices*sizeof(TEL_POINT) );
  data->num_bounds = k[a]->data.ldata;

  for( b = 0; b < n; b++ )
  {
    if( k[b]->id == BOUNDS_DATA_ID )
      break;
  }
  if( b == n )
  {
    //cmn_freemem( data->vertices );
    delete[] data->vertices;
    return TFailure;
  }
  //cmn_memreserve( data->bounds, data->num_bounds, 0 );
  data->bounds = new Tint[data->num_bounds];
  //cmn_memcpy<Tint>( data->bounds, k[b]->data.pdata, data->num_bounds );
  memcpy( data->bounds, k[b]->data.pdata, data->num_bounds*sizeof(Tint) );

  for( b = 0; b < data->num_bounds; b++ )
    edge_count += data->bounds[b];

  data->edge_count = edge_count;

  for( i = 0; i < n; i++ )
  {
    switch( k[i]->id )
    {
    case EDGE_DATA_ID:
       data->edge_vis = new Tint[edge_count];
      memcpy( data->edge_vis, k[i]->data.pdata, edge_count*sizeof(Tint) );
      break;

    case CONNECTIVITY_ID:
      data->indices = new Tint[edge_count];
      memcpy( data->indices, k[i]->data.pdata, edge_count*sizeof(Tint) );
      break;

    case FNORMALS_ID:
      data->facet_flag = TEL_FA_NORMAL;
       data->fnormals = new TEL_POINT[data->num_bounds];
      memcpy( data->fnormals, k[i]->data.pdata, data->num_bounds*sizeof(TEL_POINT) );
      for( a = 0; a < data->num_bounds; a++ )
        vecnrm( data->fnormals[a].xyz );
      break;

    case FACET_COLOUR_VALS_ID:
      data->fcolours = new TEL_COLOUR[data->num_bounds];
      memcpy( data->fcolours, k[i]->data.pdata, data->num_bounds*sizeof(TEL_COLOUR) );
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

  if( data->facet_flag != TEL_FA_NORMAL )
  {
    data->fnormals = new TEL_POINT[data->num_bounds];
    for( i = 0, a = 0; i < data->num_bounds; i++ ) {
#ifdef BUC60823
      TelGetPolygonNormal( data->vertices, &data->indices[a],
        data->bounds[i], data->fnormals[i].xyz );
#else
      TelGetNormal( data->vertices[data->indices[a]].xyz,
        data->vertices[data->indices[a+1]].xyz,
        data->vertices[data->indices[a+j]].xyz,
        data->fnormals[i].xyz );
      vecnrm(data->fnormals[i].xyz);
#endif
      a += data->bounds[i];
    }
  }
#ifdef G003
  data->d.bDraw = new unsigned char[data->num_bounds];
#endif  /* G003 */
  ((tsm_elem_data)(d.pdata))->pdata = data;

  return TSuccess;
}

/*----------------------------------------------------------------------*/

static  TStatus
PolygonIndicesDisplay( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  CMN_KEY       k11, k12, k17, k111, k114;
#ifdef OCC749
  CMN_KEY       k117;
#endif

  Tint           front_lighting_model;
  Tint           interior_style;
  TEL_COLOUR     interior_colour;
  TEL_COLOUR     edge_colour;
#ifdef OCC749
  TEL_SURF_PROP  prop;
#endif

  tel_indexpoly_data d;

  k12.id          = TelInteriorReflectanceEquation;
  k17.id          = TelInteriorStyle;
  k111.id         = TelInteriorColour;
  k111.data.pdata = &interior_colour;
  k114.id         = TelEdgeColour;
  k114.data.pdata = &edge_colour;
#ifdef OCC749
  k117.id         = TelSurfaceAreaProperties;
  k117.data.pdata = &prop;
#endif

#ifdef OCC749
  TsmGetAttri( 5, &k12, &k17, &k111, &k114, &k117);
#else
  TsmGetAttri( 4, &k12, &k17, &k111, &k114);
#endif

  front_lighting_model     = k12.data.ldata;
  interior_style           = k17.data.ldata;

#ifdef PRINT
  printf("PolygonIndicesDisplay \n"); 
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
        edge_colour = interior_colour = hrep.col;
        front_lighting_model = CALL_PHIGS_REFL_NONE;
      }
      else if( hrep.type == TelHLColour )
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
        edge_colour = interior_colour = hrep.col;
        front_lighting_model = CALL_PHIGS_REFL_NONE;
      }
      else if( hrep.type == TelHLColour )
      {
        edge_colour = hrep.col;
        k[0]->id = TOff;
      }
    }
  }

  d = (tel_indexpoly_data)data.pdata;

  glColor3fv( interior_colour.rgb );
#ifdef PRINT
  printf("PolygonIndicesDisplay.interior_colour %f,%f,%f \n",
    interior_colour.rgb[0],interior_colour.rgb[1],interior_colour.rgb[2]); 
#endif

  draw_indexpoly( d, k[0]->id,
    front_lighting_model,
    interior_style,
    &edge_colour
#ifdef OCC749
    , &prop
#endif
    );

  return TSuccess;

}

/*----------------------------------------------------------------------*/

static void
draw_indexpoly( tel_indexpoly_data p, Tint hflag,
               Tint front_lighting_model,
               Tint interior_style,
               tel_colour edge_colour
#ifdef OCC749
               , tel_surf_prop prop
#endif
               )
{
  Tint i, j, k, a, newList = 0;
  Tint  lighting_model;

  /* Following pointers have been provided for performance improvement */
  Tint       *ind;
  tel_point  pfn, pvn, pv;
  tel_colour pvc, pfc;
  tel_texture_coord pvt;

  ind = p->indices;
  pfn = p->fnormals;
  pvn = p->vnormals;
  pvc = p->vcolours;
  pfc = p->fcolours;
  pv  = p->vertices;
  pvt = p->vtexturecoord;
#ifdef G003
  if ( g_nDegenerateModel < 2 && interior_style != TSM_EMPTY ) 
#else
  if ( interior_style != TSM_EMPTY ) 
#endif  /* G003 */
  {      
    if( hflag == TOn )
    {
      pvc = 0;
      pfc = 0;
    }

    if ( interior_style == TSM_HIDDENLINE)
    {
      pvc = 0;
      pfc = 0;
    }

    if (front_lighting_model == CALL_PHIGS_REFL_NONE)
      LightOff();
    else LightOn();

    lighting_model = front_lighting_model;

#ifdef G003
#ifdef BUC60876
    if ( !g_fAnimation ) 
      goto doDraw; /* Disable object display list out of animation */
#endif
#ifdef G004
    if ( g_fBitmap ) 
      goto doDraw;  /* dipsplay lists are NOT shared between */
    /*  window's context and bitmap's one    */
#endif /* G004 */

    if ( p -> d.model != lighting_model || !p -> d.list ||
      p -> d.model == -1 ||( g_nDegenerateModel && p -> d.skipRatio != g_fSkipRatio )) {

        p -> d.skipRatio = g_fSkipRatio;
        p -> d.model     = lighting_model;
        p -> d.degMode   = g_nDegenerateModel;
#endif  /* G003 */
#ifdef G003
        if ( g_fSkipRatio == 0.0 ) {
          if ( !p -> d.list ) p -> d.list = glGenLists ( 1 );

          glNewList ( p -> d.list, GL_COMPILE_AND_EXECUTE );
          newList = 1;
doDraw:
#endif  /* G003 */

          if ( lighting_model == CALL_PHIGS_REFL_NONE ) {
            if ( p -> bounds[ 0 ] == 3 )
              glBegin ( GL_TRIANGLES );
            else if ( p -> bounds[ 0 ] == 4 )
              glBegin ( GL_QUADS );
            else glBegin ( GL_POLYGON );

            if ( pvc ) {
              for ( i = 0, j = 0, a = 0; i < p -> num_bounds; ++i ) {
                a = j + p -> bounds[ i ];
                for ( ; j < a; ++j ) {
                  glColor3fv  ( pvc[  ind[ j ]  ].rgb  );
                  glVertex3fv ( pv[   ind[ j ]  ].xyz  );
                }  /* end for ( j . . . ) */
              }  /* end for ( i . . . ) */
            } else if ( pfc ) {
              for ( i = 0, j = 0, a = 0; i < p -> num_bounds; ++i ) {
                a = j + p -> bounds[ i ];
                glColor3fv ( pfc[ i ].rgb );
                for ( ; j < a; ++j ) glVertex3fv ( pv[  ind[ j ]  ].xyz );
              }  /* end for */
            } else {
              for ( i = 0, j = 0, a = 0; i < p -> num_bounds; ++i ) {
                a = j + p -> bounds[ i ];
                for ( ; j < a; ++j ) glVertex3fv ( pv[  ind[ j ]  ].xyz  );
              }  /* end for */
            }  /* end else */
            glEnd ();
          } else {   /* lighting_model != TelLModelNone */
            if ( p -> bounds[ 0 ] == 3 )
              glBegin ( GL_TRIANGLES );
            else if ( p -> bounds[ 0 ] == 4 )
              glBegin ( GL_QUADS );
            else glBegin ( GL_POLYGON );

#ifdef OCC749
            for ( i = a = 0; i < p -> num_bounds; ++i ) {
              j = a; a += p -> bounds[ i ];
              if( pfn ) glNormal3fv ( pfn[ i ].xyz );
              if( pfc && !prop->isphysic ) {
                GLfloat diff[4], ambi[4], emsv[4], r, g, b;

#ifdef OCC7824
                ambi[3] = diff[3] = emsv[3] = prop->trans;
#else
                ambi[3] = 1.0f;  diff[3] = 1.0f;  emsv[3] = 1.0f;
#endif

                r = pfc[ i ].rgb[0];  g = pfc[ i ].rgb[1];  b = pfc[ i ].rgb[2];

                if( prop->isamb ) {
                  ambi[0] = prop->amb * r;
                  ambi[1] = prop->amb * g;
                  ambi[2] = prop->amb * b;
                  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambi);
                }
                if( prop->isdiff ) {
                  diff[0] = prop->diff * r;
                  diff[1] = prop->diff * g;
                  diff[2] = prop->diff * b;
                  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff);
                }
                if( prop->isemsv ) {
                  emsv[0] = prop->emsv * r;
                  emsv[1] = prop->emsv * g;
                  emsv[2] = prop->emsv * b;
                  glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emsv);
                }
              }
              for ( ; j < a; ++j ) {
                k = ind[ j ];
                if( pvn ) glNormal3fv ( pvn[ k ].xyz );
                if( pvc && !prop->isphysic ) {
                  GLfloat diff[4], ambi[4], emsv[4], r, g, b;

#ifdef OCC7824
                  ambi[3] = diff[3] = emsv[3] = prop->trans;
#else
                  ambi[3] = 1.0f;  diff[3] = 1.0f;  emsv[3] = 1.0f;
#endif

                  r = pvc[ k ].rgb[0];  g = pvc[ k ].rgb[1];  b = pvc[ k ].rgb[2];

                  if( prop->isamb ) {
                    ambi[0] = prop->amb * r;
                    ambi[1] = prop->amb * g;
                    ambi[2] = prop->amb * b;
                    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambi);
                  }
                  if( prop->isdiff ) {
                    diff[0] = prop->diff * r;
                    diff[1] = prop->diff * g;
                    diff[2] = prop->diff * b;
                    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff);
                  }
                  if( prop->isemsv ) {
                    emsv[0] = prop->emsv * r;
                    emsv[1] = prop->emsv * g;
                    emsv[2] = prop->emsv * b;
                    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emsv);
                  }
                }
                if( pvt && !ForbidSetTextureMapping ) glTexCoord2fv ( pvt[ k ].xy  );
                glVertex3fv ( pv[ k ].xyz );
              }  /* end for ( j . . . ) */
            }  /* end for ( i . . . ) */
            glEnd ();  
#else
            if ( pvn ) {
              if ( pvt && !ForbidSetTextureMapping )
                for ( i = 0, j = 0, a = 0; i < p -> num_bounds; ++i ) {
                  a = j + p -> bounds[ i ];
                  for ( ; j < a; ++j ) {
                    glNormal3fv   ( pvn[  ind[ j ]  ].xyz );
                    glTexCoord2fv ( pvt[  ind[ j ]  ].xy  );
                    glVertex3fv   ( pv[   ind[ j ]  ].xyz );
                  }  /* end for ( j . . . ) */
                }  /* end for ( i . . . ) */
              else
                for ( i = 0, j = 0, a = 0; i < p -> num_bounds; ++i ) {
                  a = j + p -> bounds[ i ];
                  for ( ; j < a; ++j ) {
                    glNormal3fv ( pvn[  ind[ j ]  ].xyz  );
                    glVertex3fv ( pv[   ind[ j ]  ].xyz  );
                  }  /* end for ( j . . . ) */
                }  /* end for ( i . . . ) */
            } else { /* !pvn */
              for ( i = 0, j = 0, a = 0; i < p -> num_bounds; ++i ) {
                a = j + p -> bounds[ i ];
                glNormal3fv ( pfn[ i ].xyz );
                for ( ; j < a; ++j ) glVertex3fv ( pv[  ind[ j ]  ].xyz  );
              }  /* end for */
            }  /* end else */
            glEnd ();  
#endif /* OCC749 */

          }  /* end else */
#ifdef G003
        } else if ( g_fSkipRatio != 1.0 ) {
          if ( !p -> d.dlist ) p -> d.dlist = glGenLists ( 1 );
          glNewList ( p -> d.dlist, GL_COMPILE_AND_EXECUTE );
          newList = 1;
          set_drawable_items ( p -> d.bDraw, p -> num_bounds );
          if ( lighting_model == CALL_PHIGS_REFL_NONE ) {
            if ( p -> bounds[ 0 ] == 3 )
              glBegin ( GL_TRIANGLES );
            else if ( p -> bounds[ 0 ] == 4 )
              glBegin ( GL_QUADS );
            else glBegin ( GL_POLYGON );

            if ( pvc ) {
              for ( i = 0, j = 0, a = 0; i < p -> num_bounds; ++i ) {
                a = j + p -> bounds[ i ];
                if ( p -> d.bDraw[ i ] )
                  for ( ; j < a; ++j ) {
                    glColor3fv  ( pvc[  ind[ j ]  ].rgb  );
                    glVertex3fv ( pv[   ind[ j ]  ].xyz  );
                  }  /* end for ( j . . . ) */
                else j = a;
              }  /* end for ( i . . . ) */
            } else if ( pfc ) {
              for ( i = 0, j = 0, a = 0; i < p -> num_bounds; ++i ) {
                a = j + p -> bounds[ i ];
                if ( p -> d.bDraw[ i ] ) {
                  glColor3fv ( pfc[ i ].rgb );
                  for ( ; j < a; ++j ) glVertex3fv ( pv[  ind[ j ]  ].xyz );
                } else j = a;
              }  /* end for */
            } else {
              for ( i = 0, j = 0, a = 0; i < p -> num_bounds; ++i ) {
                a = j + p -> bounds[ i ];
                if ( p -> d.bDraw[ i ] )
                  for ( ; j < a; ++j ) glVertex3fv ( pv[  ind[ j ]  ].xyz  );
                else j = a;
              }  /* end for */
            }  /* end else */
            glEnd ();
          } else {   /* lighting_model != TelLModelNone */
            if ( p -> bounds[ 0 ] == 3 )
              glBegin ( GL_TRIANGLES );
            else if ( p -> bounds[ 0 ] == 4 )
              glBegin ( GL_QUADS );
            else glBegin ( GL_POLYGON );

            if ( pvn ) {
              if ( pvt && !ForbidSetTextureMapping )
                for ( i = 0, j = 0, a = 0; i < p -> num_bounds; ++i ) {
                  a = j + p->bounds[ i ];
                  if ( p -> d.bDraw[ i ] )
                    for ( ; j < a; ++j ) {
                      glNormal3fv   ( pvn[  ind[ j ]  ].xyz );
                      glTexCoord2fv ( pvt[  ind[ j ]  ].xy  );
                      glVertex3fv   ( pv[   ind[ j ]  ].xyz );
                    }  /* end for ( j . . . ) */
                  else j = a;
                }  /* end for ( i . . . ) */
              else
                for ( i = 0, j = 0, a = 0; i < p -> num_bounds; ++i ) {
                  a = j + p -> bounds[ i ];
                  if ( p -> d.bDraw[ i ] )
                    for ( ; j < a; ++j ) {
                      glNormal3fv ( pvn[  ind[ j ]  ].xyz  );
                      glVertex3fv ( pv[   ind[ j ]  ].xyz  );
                    }  /* end for ( j . . . ) */
                  else j = a;
                }  /* end for ( i . . . ) */
            } else {  /* !pvn */
              for ( i = 0, j = 0, a = 0; i < p -> num_bounds; ++i ) {
                a = j + p -> bounds[ i ];
                if ( p -> d.bDraw[ i ] ) {
                  glNormal3fv ( pfn[ i ].xyz );
                  for ( ; j < a; ++j ) glVertex3fv ( pv[  ind[ j ]  ].xyz  );
                } else j = a;
              }  /* end for */
            }  /* end else */
            glEnd ();  
          }  /* end else */
        } else {
          if ( !p -> d.dlist ) p -> d.dlist = glGenLists ( 1 );
          glNewList ( p -> d.dlist, GL_COMPILE_AND_EXECUTE );
          newList = 1;
        }  /* end else */
#endif  /* G003 */
#ifdef G003
        if ( newList ) glEndList ();
        if ( g_nDegenerateModel ) return;
      } else {
        glCallList ( g_fSkipRatio == 0.0 ? p -> d.list : p -> d.dlist );
        if ( g_nDegenerateModel ) return;
      }  /* end else */
#endif  /* G003 */
  }
#ifdef G003
  i = 0;

  switch ( g_nDegenerateModel ) {

default:
  break;

case 2:  /* XXX_TDM_WIREFRAME */
  i = 1;
  break;

case 3:  /* XXX_TDM_MARKER */
  draw_degenerates_as_points ( p );
  return;

case 4:  /* XXX_TDM_BBOX */
  draw_degenerates_as_bboxs ( p );
  return;

  }  /* end switch */

  draw_edges ( p, edge_colour, interior_style, i );
#else
  draw_edges( p, edge_colour, interior_style );
#endif
}

/*----------------------------------------------------------------------*/

static  TStatus
PolygonIndicesDelete( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  tel_indexpoly_data p;

  p = (tel_indexpoly_data)data.pdata;
  if( p->edge_vis )
    //cmn_freemem( p->edge_vis );
    delete[] p->edge_vis;
  if( p->bounds )
    //cmn_freemem( p->bounds );
    delete[] p->bounds;
  if( p->indices )
    //cmn_freemem( p->indices );
    delete[] p->indices;
  if( p->fcolours )
    //cmn_freemem( p->fcolours );
    delete[] p->fcolours;
  if( p->fnormals )
    //cmn_freemem( p->fnormals );
    delete[] p->fnormals;
  if( p->vertices )
    //cmn_freemem( p->vertices );
    delete[] p->vertices;
  if( p->vcolours )
    //cmn_freemem( p->vcolours );
    delete[] p->vcolours;
  if( p->vnormals )
    //cmn_freemem( p->vnormals );
    delete[] p->vnormals;
  if (p->vtexturecoord)
    //cmn_freemem( p->vtexturecoord );
    delete[] p->vtexturecoord;
#ifdef G003
  if (  GET_GL_CONTEXT() != NULL  ) {

    if ( p -> d.list ) glDeleteLists ( p ->  d.list, 1 );
    if ( p -> d.dlist ) glDeleteLists ( p -> d.dlist, 1 );

  }  /* end if */

  if ( p -> d.bDraw )
    //cmn_freemem ( p -> d.bDraw );
    delete[] p -> d.bDraw;
#endif  /* G003 */

  //cmn_freemem( data.pdata );
  if (data.pdata)
    delete data.pdata;
  return TSuccess;
}

/*----------------------------------------------------------------------*/

static  TStatus
PolygonIndicesPrint( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  Tint             i;
  tel_indexpoly_data p;

  p = (tel_indexpoly_data)data.pdata;

  fprintf( stdout, "TelPolygonIndices. Number of Vertices: %d\n", p->num_vertices );
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
    if( p->fcolours )
      fprintf( stdout, "\t\tFacet Flag : COLOUR\n" );
    else
      fprintf( stdout, "\t\tFacet Flag : NONE\n" );
    break;

  case TEL_FA_NORMAL:
    if( p->fcolours )
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
  if( p->edge_vis )
  {
    fprintf( stdout, "\t\tEdge Visibility Data :\n" );
    for( i = 0; i < p->edge_count; i++ )
      fprintf( stdout, "\t\t%d ", p->edge_vis[i] );
    fprintf( stdout, "\n" );
  }
  if( p->bounds )
  {
    fprintf( stdout, "\t\tBounds array :\n" );
    for( i = 0; i < p->num_bounds; i++ )
      fprintf( stdout, "\t\tb[%d] %d \n", i, p->bounds[i] );
  }
  if( p->indices )
  {
    fprintf( stdout, "\t\tConnectivity array :\n" );
    for( i = 0; i < p->edge_count; i++ )
      fprintf( stdout, "\t\tI[%d] %d \n", i, p->indices[i] );
  }
  if( p->fnormals )
  {
    fprintf( stdout, "\n\t\tFacet Normals : " );
    for( i = 0; i < p->num_bounds; i++ )
      fprintf( stdout, "\n\t\t v[%d] = %g %g %g", i,
      p->fnormals[i].xyz[0],
      p->fnormals[i].xyz[1],
      p->fnormals[i].xyz[2] );
    fprintf( stdout, "\n" );
  }
  else
    fprintf( stdout, "\n\t\tFacet Normals not specified\n" );

  if( p->fcolours )
  {
    fprintf( stdout, "\n\t\tFacet Colours : " );
    for( i = 0; i < p->num_bounds; i++ )
      fprintf( stdout, "\n\t\t v[%d] = %g %g %g", i,
      p->fcolours[i].rgb[0],
      p->fcolours[i].rgb[1],
      p->fcolours[i].rgb[2] );    }
  else
    fprintf( stdout, "\n\t\tFacet Colours not specified\n" );

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
PolygonIndicesInquire( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  Tint                 i, j;
  tel_indexpoly_data   d;
  Tint                 size_reqd=0;
  TStatus              status = TSuccess;
  Tchar                *cur_ptr = 0;

  d = (tel_indexpoly_data)data.pdata;

  if( d->edge_vis )
    size_reqd += ( d->edge_count * sizeof( Tint ) );

  size_reqd += ( d->num_bounds * sizeof( Tint ) ); /* bounds */
  size_reqd += ( d->edge_count * sizeof( Tint ) ); /* connectivity */

  if( d->fcolours )
    size_reqd += ( d->num_bounds * sizeof( TEL_COLOUR ) );

  if( d->facet_flag == TEL_FA_NORMAL )
    size_reqd += ( d->num_bounds * sizeof( TEL_POINT ) );

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
        w->indexedpolygons3data.shpflag = d->shape_flag;
        w->indexedpolygons3data.num_bounds = d->num_bounds;
        w->indexedpolygons3data.num_vertices = d->num_vertices;
        if( d->edge_vis )
          w->indexedpolygons3data.edgflag = TOn;
        else
          w->indexedpolygons3data.edgflag = TOff;

        if( c->size >= size_reqd )
        {
          if( d->facet_flag == TEL_FA_NORMAL )
          {
            if( d->fcolours )
            {
              w->indexedpolygons3data.fctflag = TEL_FAFLAG_COLOURNORMAL;
              w->indexedpolygons3data.gnormals = (tel_point)(c->buf);
              for( j = 0; j < d->num_bounds; j++ )
              {
                w->indexedpolygons3data.gnormals[j] = d->fnormals[j];
              }
              cur_ptr += ( d->num_bounds * sizeof( TEL_POINT ) );

              w->indexedpolygons3data.facet_colour_vals =
                (tel_colour)(cur_ptr);
              for( j = 0; j < d->num_bounds; j++ )
              {
                w->indexedpolygons3data.facet_colour_vals[j] =
                  d->fcolours[j];
              }
              cur_ptr += ( d->num_bounds * sizeof( TEL_COLOUR ) );
            }
            else
            {
              w->indexedpolygons3data.fctflag = TEL_FAFLAG_NORMAL;
              w->indexedpolygons3data.facet_colour_vals = 0;
              w->indexedpolygons3data.gnormals = (tel_point)(c->buf);
              for( j = 0; j < d->num_bounds; j++ )
              {
                w->indexedpolygons3data.gnormals[j] = d->fnormals[j];
              }
              cur_ptr += ( d->num_bounds * sizeof( TEL_POINT ) );
            }
          }
          else
          {
            w->indexedpolygons3data.gnormals = 0;
            if( d->fcolours )
            {
              w->indexedpolygons3data.fctflag = TEL_FAFLAG_COLOUR;
              w->indexedpolygons3data.facet_colour_vals =
                (tel_colour)(c->buf );
              for( j = 0; j < d->num_bounds; j++ )
              {
                w->indexedpolygons3data.facet_colour_vals[j] =
                  d->fcolours[j];
              }
              cur_ptr += ( d->num_bounds * sizeof( TEL_COLOUR ) );
            }
            else
            {
              w->indexedpolygons3data.fctflag = TEL_FAFLAG_NONE;
              w->indexedpolygons3data.facet_colour_vals = 0;
            }
          }

          if( d->edge_vis )
          {
            w->indexedpolygons3data.edgvis = (Tint *)(cur_ptr);
            //             cmn_memcpy<Tint>( w->indexedpolygons3data.edgvis,
            //                               d->edge_vis,
            //                               d->edge_count );
            memcpy( w->indexedpolygons3data.edgvis, d->edge_vis, d->edge_count*sizeof(Tint) );
            cur_ptr += (d->edge_count * sizeof( Tint ) );
          }

          w->indexedpolygons3data.points = (tel_point)cur_ptr;
          for( j = 0; j < d->num_vertices; j++ )
          {
            w->indexedpolygons3data.points[j] = d->vertices[j];
          }
          cur_ptr += ( d->num_vertices * sizeof( TEL_POINT ) );

          w->indexedpolygons3data.bounds = (Tint *)cur_ptr;
          //           cmn_memcpy<Tint>( w->indexedpolygons3data.bounds,
          //                             d->bounds,
          //                             d->num_bounds );
          memcpy( w->indexedpolygons3data.bounds, d->bounds, d->num_bounds*sizeof(Tint) );
          cur_ptr += ( d->num_bounds * sizeof( Tint ) );

          w->indexedpolygons3data.indices = (Tint *)cur_ptr;
          //           cmn_memcpy<Tint>( w->indexedpolygons3data.indices,
          //                             d->indices,
          //                             d->edge_count );
          memcpy( w->indexedpolygons3data.indices, d->indices, d->edge_count*sizeof(Tint) );
          cur_ptr += ( d->edge_count * sizeof( Tint ) );

          if( d->vertex_flag == TEL_VT_NORMAL )
          {
            if( d->vcolours )
            {
              w->indexedpolygons3data.vrtflag = TEL_VTFLAG_COLOURNORMAL;
              w->indexedpolygons3data.vnormals = (tel_point)(cur_ptr);
              for( j = 0; j < d->num_vertices; j++ )
              {
                w->indexedpolygons3data.vnormals[j] = d->vnormals[i];
              }
              cur_ptr += ( d->num_vertices * sizeof( TEL_POINT ) );

              w->indexedpolygons3data.colours = (tel_colour)(cur_ptr);

              for( j = 0; j < d->num_vertices; j++ )
              {
                w->indexedpolygons3data.colours[j] = d->vcolours[i];
              }
            }
            else
            {
              w->indexedpolygons3data.vrtflag = TEL_VTFLAG_NORMAL;
              w->indexedpolygons3data.colours = 0;
              w->indexedpolygons3data.vnormals = (tel_point)(cur_ptr);

              for( j = 0; j < d->num_vertices; j++ )
              {
                w->indexedpolygons3data.vnormals[j] = d->vnormals[i];
              }
            }
          }
          else
          {
            w->indexedpolygons3data.vnormals = 0;
            if( d->vcolours )
            {
              w->indexedpolygons3data.vrtflag = TEL_VTFLAG_COLOUR;
              w->indexedpolygons3data.colours = (tel_colour)(cur_ptr);
              for( j = 0; j < d->num_vertices; j++ )
              {
                w->indexedpolygons3data.colours[j] = d->vcolours[i];
              }
            }
            else
            {
              w->indexedpolygons3data.vrtflag = TEL_VTFLAG_NONE;
              w->indexedpolygons3data.colours = 0;
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
#ifdef G003
draw_edges ( tel_indexpoly_data p, tel_colour edge_colour,
            Tint interior_style, Tint forceDraw )
#else
draw_edges( tel_indexpoly_data p, tel_colour edge_colour, Tint interior_style )
#endif
{
  CMN_KEY k, k1, k2, k3, k4;
  Tint    *ind, *vis;
  Tint    i, j, a, newList = 0;
  Tint    edge_type, line_type_preserve;
  Tfloat  edge_width, line_width_preserve;
  GLboolean texture_on;

  tel_point  pv;

#ifdef G003
  if ( interior_style != TSM_HIDDENLINE && !forceDraw )
#else 
  if (interior_style != TSM_HIDDENLINE)
#endif
  {
    k.id = TelEdgeFlag;
    TsmGetAttri( 1, &k );
    if( k.data.ldata == TOff )return;
  } 

  pv  = p->vertices;
  ind = p->indices;
  vis = p->edge_vis;

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

#ifdef G003 
  if ( !forceDraw ) {

    glColor3fv    ( edge_colour -> rgb         );
    glPushAttrib  ( GL_POLYGON_BIT             );
    glPolygonMode ( GL_FRONT_AND_BACK, GL_LINE );

    for ( i = 0, j = 0, a = 0; i < p -> num_bounds; ++i ) {

      a = j + p -> bounds[ i ];

      glBegin ( GL_POLYGON );

      for ( ; j < a; ++j ) {

        glEdgeFlag (   ( GLboolean )( vis[ j ] == 1 ? GL_TRUE : GL_FALSE )  );
        glVertex3fv ( pv[  ind[ j ]  ].xyz );

      }  /* end for */

      glEnd();

      glEdgeFlag ( GL_TRUE );

    }  /* end for */

    glPopAttrib ();

  } else {

#ifdef BUC60876
    if ( !g_fAnimation ) 
      goto doDraw; /* Disable object display list out of animation */
#endif
#ifdef G004
    if ( g_fBitmap ) 
      goto doDraw;
#endif /* G004 */

    if ( p -> d.degMode != 2 || p -> d.skipRatio != g_fSkipRatio || !p -> d.dlist ) {

      if ( !p -> d.dlist ) p -> d.dlist = glGenLists ( 1 );

      p -> d.degMode   = 2;
      p -> d.skipRatio = g_fSkipRatio;
      glNewList ( p -> d.dlist, GL_COMPILE_AND_EXECUTE );
      newList = 1;

doDraw:
      glPushAttrib ( GL_POLYGON_BIT | GL_DEPTH_BUFFER_BIT );

      glEdgeFlag    ( GL_TRUE                    );
      glDisable     ( GL_DEPTH_TEST              );
      glPolygonMode ( GL_FRONT_AND_BACK, GL_LINE );

      if ( g_fSkipRatio == 0.0F )

        for ( i = 0, j = 0, a = 0; i < p -> num_bounds; ++i ) {

          a = j + p -> bounds[ i ];

          glBegin ( GL_POLYGON );

          for ( ; j < a; ++j ) glVertex3fv ( pv[  ind[ j ]  ].xyz );

          glEnd();

        }  /* end for */

      else if ( g_fSkipRatio != 1.0F ) {

        set_drawable_items ( p -> d.bDraw, p -> num_bounds );

        for ( i = 0, j = 0, a = 0; i < p -> num_bounds; ++i ) {

          a = j + p -> bounds[ i ];

          if ( p -> d.bDraw[ i ] ) {

            glBegin ( GL_POLYGON );

            for ( ; j < a; ++j ) glVertex3fv ( pv[  ind[ j ]  ].xyz );

            glEnd();

          } else j = a;

        }  /* end for */

      }  /* end if */

      glPopAttrib ();
      if ( newList ) glEndList ();

    } else glCallList ( p -> d.dlist );

  }  /* end else */
#else
  glColor3fv( edge_colour->rgb );
  glPushAttrib(GL_POLYGON_BIT);    
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); 
  for( i = 0, j = 0, a = 0; i < p->num_bounds; i++ )
  {
    a = j + p->bounds[i];
    glBegin(GL_POLYGON);
    for( ; j < a; j++ )
    {
      if( vis[j] == 1) glEdgeFlag( GL_TRUE ); 
      else glEdgeFlag( GL_FALSE ); 
      glVertex3fv( pv[ ind[j] ].xyz );
    }
    glEnd();
    glEdgeFlag(GL_TRUE);
  }

  glPopAttrib();
#endif  /* G003 */

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
#ifdef G003
static void draw_degenerates_as_points ( tel_indexpoly_data p ) {

  Tint*      ind, *vis;
  Tint       i, j, n, a, newList = 0;
  GLfloat    pt[ 3 ];
  tel_point  pv;

  pv  = p -> vertices;
  ind = p -> indices;
  vis = p -> edge_vis;

  LightOff ();

#ifdef G004
  if ( g_fBitmap ) goto doDraw;
#endif /* G004 */

  if ( p -> d.degMode != 3 || p -> d.skipRatio != g_fSkipRatio || !p -> d.dlist ) {

    if ( !p -> d.dlist ) p -> d.dlist = glGenLists ( 1 );

    p -> d.degMode   = 3;
    p -> d.skipRatio = g_fSkipRatio;
    glNewList ( p -> d.dlist, GL_COMPILE_AND_EXECUTE );
    newList = 1;
doDraw:
    if ( g_fSkipRatio == 0.0F ) {

      glBegin ( GL_POINTS );

      for ( i = 0, j = 0, a = 0; i < p -> num_bounds; ++i ) {

        n = p -> bounds[ i ];
        a = j + n;

        for ( pt [ 0 ] = pt[ 1 ] = pt[ 2 ] = 0.; j < a; ++j ) {

          pt[ 0 ] += pv[  ind[ j ]  ].xyz[ 0 ];
          pt[ 1 ] += pv[  ind[ j ]  ].xyz[ 1 ];
          pt[ 2 ] += pv[  ind[ j ]  ].xyz[ 2 ];

        }  /* end for ( j ) */

        pt[ 0 ] /= n;
        pt[ 1 ] /= n;
        pt[ 2 ] /= n;

        glVertex3fv ( pt );

      }  /* end for ( i ) */

      glEnd ();

    } else if ( g_fSkipRatio != 1.0 ) {

      set_drawable_items ( p -> d.bDraw, p -> num_bounds );

      glBegin ( GL_POINTS );

      for ( i = 0, j = 0, a = 0; i < p -> num_bounds; ++i ) {

        n = p -> bounds[ i ];
        a = j + n;

        if ( p -> d.bDraw[ i ] ) {

          for ( pt [ 0 ] = pt[ 1 ] = pt[ 2 ] = 0.; j < a; ++j ) {

            pt[ 0 ] += pv[  ind[ j ]  ].xyz[ 0 ];
            pt[ 1 ] += pv[  ind[ j ]  ].xyz[ 1 ];
            pt[ 2 ] += pv[  ind[ j ]  ].xyz[ 2 ];

          }  /* end for ( j ) */

          pt[ 0 ] /= n;
          pt[ 1 ] /= n;
          pt[ 2 ] /= n;

          glVertex3fv ( pt );

        } else j = a;

      }  /* end for ( i ) */

      glEnd ();

    }  /* end if */

#ifdef G004
    if ( !g_fBitmap )
#endif /* G004 */
      glEndList ();

  } else glCallList ( p -> d.dlist );

}  /* end draw_degenerates_as_points */

static void draw_degenerates_as_bboxs ( tel_indexpoly_data p ) {

  Tint*     ind, *vis;
  Tint      i, j, n, a, newList = 0;
  GLfloat   minp[ 3 ] = { FLT_MAX, FLT_MAX, FLT_MAX };
  GLfloat   maxp[ 3 ] = { FLT_MIN, FLT_MIN, FLT_MIN };
  tel_point pv;

  pv  = p -> vertices;
  ind = p -> indices;
  vis = p -> edge_vis;

  LightOff ();

#ifdef G004
  if ( g_fBitmap ) goto doDraw;
#endif /* G004 */

  if ( p -> d.degMode != 4 || !p -> d.dlist ) {

    if ( !p -> d.dlist ) p -> d.dlist = glGenLists ( 1 );

    p -> d.degMode = 4;

    glNewList ( p -> d.dlist, GL_COMPILE_AND_EXECUTE );
    newList = 1;
doDraw:
    for ( i = 0, j = 0, a = 0; i < p -> num_bounds; ++i ) {

      n = p -> bounds[ i ];
      a = j + n;

      for ( ; j < a; ++j ) {

        if ( pv[  ind[ j ]  ].xyz[ 0 ] < minp[ 0 ] )
          minp[ 0 ] = pv[  ind[ j ]  ].xyz[ 0 ] ;
        if ( pv[  ind[ j ]  ].xyz[ 1 ] < minp[ 1 ] )
          minp[ 1 ] = pv[  ind[ j ]  ].xyz[ 1 ] ;
        if ( pv[  ind[ j ]  ].xyz[ 2 ] < minp[ 2 ] )
          minp[ 2 ] = pv[  ind[ j ]  ].xyz[ 2 ] ;

        if ( pv[  ind[ j ]  ].xyz[ 0 ] > maxp[ 0 ] )
          maxp[ 0 ] = pv[  ind[ j ]  ].xyz[ 0 ] ;
        if ( pv[  ind[ j ]  ].xyz[ 1 ] > maxp[ 1 ] )
          maxp[ 1 ] = pv[  ind[ j ]  ].xyz[ 1 ] ;
        if ( pv[  ind[ j ]  ].xyz[ 2 ] > maxp[ 2 ] )
          maxp[ 2 ] = pv[  ind[ j ]  ].xyz[ 2 ] ;

      }  /* end for ( j ) */

    }  /* end for ( i ) */

    /* OCC11904 -- Temporarily disable environment mapping */
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_TEXTURE_1D);
    glDisable(GL_TEXTURE_2D);

    glBegin ( GL_LINE_STRIP );

    glVertex3fv ( minp );
    glVertex3f ( minp[ 0 ], maxp[ 1 ], minp[ 2 ] );
    glVertex3f ( minp[ 0 ], maxp[ 1 ], maxp[ 2 ] );
    glVertex3f ( minp[ 0 ], minp[ 1 ], maxp[ 2 ] );
    glVertex3f ( minp[ 0 ], minp[ 1 ], minp[ 2 ] );

    glVertex3f ( maxp[ 0 ], minp[ 1 ], minp[ 2 ] );
    glVertex3f ( maxp[ 0 ], maxp[ 1 ], minp[ 2 ] );
    glVertex3f ( maxp[ 0 ], maxp[ 1 ], maxp[ 2 ] );
    glVertex3f ( maxp[ 0 ], minp[ 1 ], maxp[ 2 ] );
    glVertex3f ( maxp[ 0 ], minp[ 1 ], minp[ 2 ] );

    glVertex3f ( maxp[ 0 ], minp[ 1 ], maxp[ 2 ] );
    glVertex3f ( minp[ 0 ], minp[ 1 ], maxp[ 2 ] );
    glVertex3f ( minp[ 0 ], maxp[ 1 ], maxp[ 2 ] );
    glVertex3fv ( maxp );
    glVertex3f ( maxp[ 0 ], maxp[ 1 ], minp[ 2 ] );
    glVertex3f ( minp[ 0 ], maxp[ 1 ], minp[ 2 ] );

    glEnd();
    glPopAttrib();
#ifdef G004
    if ( !g_fBitmap )
#endif /* G004 */
      glEndList ();

  } else glCallList ( p -> d.dlist );

}  /* end draw_degenerates_as_bboxs */
#endif  /* G003 */

void set_drawable_items ( GLboolean* pbDraw, int n ) {

  int i;

  memset (  pbDraw, 0, sizeof ( GLboolean ) * n  );

  i = ( int )(  ( 1.0F - g_fSkipRatio ) * n  );

  while ( i-- ) pbDraw[ OPENGL_RAND() % n ] = 1;

}  /* end set_drawable_items */
