#define G003
#define G004
/***********************************************************************

FONCTION :
----------
File OpenGl_tmesh :


REMARQUES:
---------- 

Le culling et le backfacing ne marchent pas.


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : xxx ; Creation.
11-03-96 : FMN ; Correction warning compilation
01-04-96 : CAL ; Integration MINSK portage WNT
22-04-96 : FMN ; Suppression prototype inutile.
09-08-96 : FMN ; Suppression appel glMatrixMode() avant glGetFloatv()
21-10-96 : FMN ; Suppression LMC_COLOR fait dans OpenGl_execstruct.c
30-06-97 : FMN ; Suppression OpenGl_telem_light.h
18-07-97 : FMN ; Ajout desactivation des lights suivant front_lighting_model
21-07-97 : FMN ; Amelioration des performances OPTIMISATION_FMN
- suppression calcul inutile sur les front faces
- utilisation de GL_LINE_LOOP
27-08-97 : FMN ; Correction affichage des edges
On n'affiche que les edges dans le mode IS_EMPTY
10-09-97 : FMN ; Amelioration des perfs liees aux lights.
15-09-97 : PCT ; Ajout coordonnees textures
24-09-97 : FMN ; Suppression OPTIMISATION_FMN.
08-12-97 : FMN ; Suppression appel TsmGetAttri inutile.
23-12-97 : FMN ; Suppression IsBackFace()
31-12-97 : FMN ; Simplification pour le highlight
15-01-98 : FMN ; Ajout Hidden line
16-02-00 : EUG ; Degeneration support
16-02-00 : EUG ; Bitmap drawing support
08-03-01 : GG  ; BUC60823 Avoid crash in the normal computation method
on confuse point.

************************************************************************/

#define BUC60876        /* GG 5/4/2001 Disable local display list
//                      when animation is not required
*/

/*----------------------------------------------------------------------*/

#define OCC4298 // SAN : 12/11/2003 : Memory beyond array bound read in TriangleMeshAdd()
/*
* Includes
*/ 

#include <OpenGl_tgl_all.hxx>

#include <stddef.h>
#include <stdio.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <OpenGl_cmn_varargs.hxx>
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

/*----------------------------------------------------------------------*/
/*
* Variables externes
*/

extern  Tint  ForbidSetTextureMapping; /* currently defined in tsm/tsm.c */

#ifdef G003
# include <float.h>
# define DEF_DS_INTERNAL
# include <OpenGl_degeneration.hxx>

# ifdef G004
extern GLboolean g_fBitmap;
# endif /* G004 */

extern int   g_nDegenerateModel;
extern float g_fSkipRatio;
extern GLboolean g_fAnimation;

#endif  /* G003 */

/*----------------------------------------------------------------------*/

typedef struct
{
  Tint       num_facets; /* Number of facets */
  Tint       facet_flag; /* TEL_FA_NONE or TEL_FA_NORMAL */
  Tint       vertex_flag;/* TEL_VT_NONE or TEL_VT_NORMAL */
  tel_point  fnormals;   /* length = num_facets */
  tel_colour fcolours;   /* length = num_facets */
  tel_point  vertices;   /* length = num_facets + 2 */
  tel_colour vcolours;   /* length = num_facets + 2 */
  tel_point  vnormals;   /* length = num_facets + 2 */
  tel_texture_coord vtexturecoord; /* Texture coordinates */
#ifdef G003
  DS_INTERNAL d;
#endif  /* G003 */
} TEL_TMESH_DATA, *tel_tmesh_data;

static  TStatus  TriangleMeshDisplay( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  TriangleMeshAdd( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  TriangleMeshDelete( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  TriangleMeshPrint( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  TriangleMeshInquire( TSM_ELEM_DATA, Tint, cmn_key* );

static void draw_tmesh( tel_tmesh_data,
                        Tint,          /* highlight flag        */
                        Tint,          /* front_lighting_model, */
                        Tint,          /* interior_style,       */
                        tel_colour     /* edge_colour,          */
                       );
#ifndef G003
static void draw_edges( tel_tmesh_data, tel_colour, Tint);
#else
static void draw_edges                 ( tel_tmesh_data, tel_colour, Tint, Tint );
void draw_degenerates_as_points ( PDS_INTERNAL, tel_point, Tint          );
void draw_degenerates_as_bboxs  ( PDS_INTERNAL, tel_point, Tint          );
extern void set_drawable_items         ( GLboolean*, int                        );
#endif  /* G003 */

static void draw_line_loop ( tel_tmesh_data );

static TStatus ( *MtdTbl[] )( TSM_ELEM_DATA, Tint, cmn_key* ) = {

  TriangleMeshDisplay, /* PickTraverse */
  TriangleMeshDisplay,
  TriangleMeshAdd,
  TriangleMeshDelete,
  TriangleMeshPrint,
  TriangleMeshInquire

};

/*----------------------------------------------------------------------*/

MtblPtr TelTriangleMeshInitClass ( TelType* el ) {

  *el = TelTriangleMesh;

  return MtdTbl;

}  /* end TelTriangleMeshInitClass */

/*----------------------------------------------------------------------*/
static  TStatus
TriangleMeshAdd( TSM_ELEM_DATA d, Tint n, cmn_key *k )
/* expects keys
NUM_FACETS_ID
FNORMALS_ID
FACET_COLOUR_VALS_ID
VERTICES_ID
VERTEX_COLOUR_VALS_ID
VNORMALS_ID
VTEXTURECOORD_ID */
{
  Tint i, j;
  tel_tmesh_data  data;

  for( i = 0; i < n; i++ )
  {
    if( k[i]->id == NUM_FACETS_ID )
      break;
  }
  if( i == n )
    return TFailure;

  if( !(k[i]->data.ldata) )
    return TFailure;

  data = new TEL_TMESH_DATA();
  if( !data )
    return TFailure;

  data->num_facets = k[i]->data.ldata;
  data->vtexturecoord = 0;

  for( i = 0; i < n; i++ )
  {
    switch( k[i]->id )
    {
    case FNORMALS_ID:
      {
        data->facet_flag = TEL_FA_NORMAL;
        data->fnormals = new TEL_POINT[data->num_facets];
        memcpy( data->fnormals, k[i]->data.pdata, data->num_facets*sizeof(TEL_POINT) );
        for( j = 0; j < data->num_facets; j++ )
          vecnrm( data->fnormals[j].xyz );
        break;
      }
    case FACET_COLOUR_VALS_ID:
      {
        data->fcolours = new TEL_COLOUR[data->num_facets];
        memcpy( data->fcolours,
          k[i]->data.pdata,
          data->num_facets*sizeof(TEL_COLOUR) );
        break;
      }
    case VERTICES_ID:
      {
        data->vertices = new TEL_POINT[data->num_facets + 2];
        memcpy( data->vertices, k[i]->data.pdata,
          (data->num_facets+2)*sizeof(TEL_POINT) );
        break;
      }
    case VERTEX_COLOUR_VALS_ID:
      {
        data->vcolours = new TEL_COLOUR[data->num_facets];
        memcpy( data->vcolours, k[i]->data.pdata, 
          (data->num_facets+2)*sizeof(TEL_COLOUR) );

        break;
      }
    case VNORMALS_ID:
      {
        data->vertex_flag = TEL_VT_NORMAL;
        data->vnormals = new TEL_POINT[data->num_facets + 2];
        memcpy( data->vnormals, k[i]->data.pdata,
          (data->num_facets + 2)*sizeof(TEL_POINT) );
        for( j = 0; j < (data->num_facets+2); j++ )
          vecnrm( data->vnormals[j].xyz );
        break;
      }
    case VTEXTURECOORD_ID:
      {
        data->vtexturecoord = new TEL_TEXTURE_COORD[data->num_facets + 2];
        memcpy( data->vtexturecoord,
          k[i]->data.pdata,
          (data->num_facets + 2)*sizeof(TEL_TEXTURE_COORD) );
        break;
      }

    default:
      {
        break;
      }
    }
  }

  if( data->facet_flag != TEL_FA_NORMAL )
  {
    /* calculate the normals and store */

    data->fnormals = new TEL_POINT[data->num_facets];
    for( i = 0; i < data->num_facets; i++ ) {
      if( i & 1 ) {
        TelGetNormal( data->vertices[i].xyz, data->vertices[i+2].xyz,
          data->vertices[i+1].xyz, data->fnormals[i].xyz );
      } else {
        TelGetNormal( data->vertices[i].xyz, data->vertices[i+1].xyz,
          data->vertices[i+2].xyz, data->fnormals[i].xyz );
      }
    }
#ifndef BUC60823
    for( j = 0; j < data->num_facets; j++ )
      vecnrm( data->fnormals[j].xyz );
#endif
  }
  if( data->vertex_flag != TEL_VT_NORMAL )
  {
    /* calculate the vertex normals */
    data->vnormals = new TEL_POINT[data->num_facets + 2];
    for( i = 2; i < data->num_facets; i++ )
    {
      data->vnormals[i].xyz[0] = ( data->fnormals[i-2].xyz[0] +
        data->fnormals[i-1].xyz[0] +
        data->fnormals[ i ].xyz[0] ) / ( float )3.0;
      data->vnormals[i].xyz[1] = ( data->fnormals[i-2].xyz[1] +
        data->fnormals[i-1].xyz[1] +
        data->fnormals[ i ].xyz[1] ) / ( float )3.0;
      data->vnormals[i].xyz[2] = ( data->fnormals[i-2].xyz[2] +
        data->fnormals[i-1].xyz[2] +
        data->fnormals[ i ].xyz[2] ) / ( float )3.0;
    }
    data->vnormals[0] = data->fnormals[0];
#ifdef OCC4298
    if( data->num_facets > 1 )
    {
#endif
      data->vnormals[1].xyz[0] = ( data->fnormals[0].xyz[0] +
        data->fnormals[1].xyz[0] )/( float )2.0;
      data->vnormals[1].xyz[1] = ( data->fnormals[0].xyz[1] +
        data->fnormals[1].xyz[1] )/( float )2.0;
      data->vnormals[1].xyz[2] = ( data->fnormals[0].xyz[2] +
        data->fnormals[1].xyz[2] )/( float )2.0;
#ifdef OCC4298
    }
    else
      data->vnormals[1] = data->fnormals[0];
#endif
    /* last vertex */
    data->vnormals[data->num_facets+1] = data->fnormals[data->num_facets-1];

    /* second last vertex */
#ifdef OCC4298
    if( data->num_facets > 1 )
    {
#endif
      data->vnormals[data->num_facets].xyz[0] =
        ( data->fnormals[data->num_facets-1].xyz[0]  +
        data->fnormals[data->num_facets-2].xyz[0]  ) / ( float )2.0;
      data->vnormals[data->num_facets].xyz[1] =
        ( data->fnormals[data->num_facets-1].xyz[1]  +
        data->fnormals[data->num_facets-2].xyz[1]  ) / ( float )2.0;
      data->vnormals[data->num_facets].xyz[2] =
        ( data->fnormals[data->num_facets-1].xyz[2]  +
        data->fnormals[data->num_facets-2].xyz[2]  ) / ( float )2.0;
#ifdef OCC4298
    }
#endif
  }
#ifdef G003
  data -> d.list      =  0;
  data -> d.dlist     =  0;
  data -> d.degMode   =  0;
  data -> d.model     = -1;
  data -> d.skipRatio =  0.0F;
  data->d.bDraw = new unsigned char[data->num_facets + 2];
#endif  /* G003 */
  ((tsm_elem_data)(d.pdata))->pdata = data;

  return TSuccess;
}
/*----------------------------------------------------------------------*/

static  TStatus
TriangleMeshDisplay( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  CMN_KEY       k11, k12, k17, k111, k114;

  Tint           front_lighting_model;
  Tint           interior_style;
  TEL_COLOUR     interior_colour;
  TEL_COLOUR     edge_colour;

  tel_tmesh_data d;

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
  printf("TriangleMeshDisplay \n");
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

  d = (tel_tmesh_data)data.pdata;

  glColor3fv( interior_colour.rgb );

  draw_tmesh( d, k[0]->id,
    front_lighting_model, 
    interior_style, 
    &edge_colour);

  return TSuccess;

}

/*----------------------------------------------------------------------*/

static void draw_tmesh (
                        tel_tmesh_data                   p,
                        Tint                         hflag,
                        Tint          front_lighting_model,
                        Tint                interior_style,
                        tel_colour             edge_colour
                        ) 
{

  Tint              i, newList = 0;
  tel_point         pv, pfn, pvn;
  tel_colour        pfc, pvc;
  tel_texture_coord pvt;

  pfn = p -> fnormals;
  pfc = p -> fcolours;
  pv  = p -> vertices;
  pvc = p -> vcolours;
  pvn = p -> vnormals;
  pvt = p -> vtexturecoord;

  if ( hflag ) pvc = pfc = 0;
#ifdef G003
  if ( g_nDegenerateModel < 2 && interior_style != TSM_EMPTY ) {
#else
  if( interior_style != TSM_EMPTY ) {
#endif  /* G003 */
    if ( front_lighting_model == CALL_PHIGS_REFL_NONE )

      LightOff ();

    else LightOn ();
#ifdef G003
#ifdef BUC60876
    if ( !g_fAnimation )
      goto doDraw; /* Disable object display list out of animation */
#endif
# ifdef G004
    if ( g_fBitmap ) goto doDraw;  /* display lists are NOT shared between */
    /*  window's context and bitmap's one   */
# endif  /* G004 */
    if ( p -> d.model != front_lighting_model || !p -> d.list ||
      p -> d.model == -1                                 ||
      ( g_nDegenerateModel && p -> d.skipRatio != g_fSkipRatio )
      ) {

        p -> d.skipRatio = g_fSkipRatio;
        p -> d.model     = front_lighting_model;
        p -> d.degMode   = g_nDegenerateModel;

        if ( g_fSkipRatio == 0.0 ) {

          if ( !p -> d.list ) p -> d.list = glGenLists ( 1 );

          glNewList ( p -> d.list, GL_COMPILE_AND_EXECUTE );
          newList = 1;
#endif  /* G003 */
#ifdef G004
doDraw:
#endif  /* G004 */
          glBegin ( GL_TRIANGLE_STRIP );

          if ( front_lighting_model == CALL_PHIGS_REFL_NONE ) {

            if ( pvc )

              for ( i = 0; i < p -> num_facets + 2; ++i ) {

                glColor3fv  ( pvc[ i ].rgb );
                glVertex3fv ( pv [ i ].xyz );

              }  /* end for */

            else if ( pfc ) {

              glColor3fv  ( pfc[ 0 ].rgb );
              glVertex3fv ( pv [ 0 ].xyz );
              glVertex3fv ( pv [ 1 ].xyz );

              for ( i = 2; i < p -> num_facets + 2; ++i ) {

                glColor3fv ( pfc[ i - 2 ].rgb );
                glVertex3fv( pv [ i     ].xyz );

              }  /* end for */

            } else for ( i = 0; i < p -> num_facets + 2; ++i ) glVertex3fv ( pv[ i ].xyz );

          } else {  /* front_lighting_model != TelLModelNone */

            if ( pvt && !ForbidSetTextureMapping )

              for ( i = 0; i < p -> num_facets + 2; ++i ) {

                glNormal3fv   ( pvn[ i ].xyz );
                glTexCoord2fv ( pvt[ i ].xy  );
                glVertex3fv   ( pv [ i ].xyz );

              }  /* end for */

            else for ( i = 0; i < p -> num_facets + 2; ++i ) {

              glNormal3fv( pvn[ i ].xyz );
              glVertex3fv( pv[  i ].xyz );

            }  /* end for */

          }  /* end else */

          glEnd ();
#ifdef G003
        } else if ( g_fSkipRatio != 1.0 ) {

          set_drawable_items ( p -> d.bDraw, p -> num_facets + 2 );

          if ( !p -> d.dlist ) p -> d.dlist = glGenLists ( 1 );

          glNewList ( p -> d.dlist, GL_COMPILE_AND_EXECUTE );
          newList = 1;

          glBegin ( GL_TRIANGLES );

          if ( front_lighting_model == CALL_PHIGS_REFL_NONE ) {

            if ( pvc ) {

              for ( i = 0; i < p -> num_facets; ++i ) {

                if ( p -> d.bDraw[ i ] ) {

                  if ( i % 2 ) {

                    glColor3fv  ( pvc[ i     ].rgb );
                    glVertex3fv ( pv [ i     ].xyz );
                    glColor3fv  ( pvc[ i + 2 ].rgb );
                    glVertex3fv ( pv [ i + 2 ].xyz );
                    glColor3fv  ( pvc[ i + 1 ].rgb );
                    glVertex3fv ( pv [ i + 1 ].xyz );

                  } else {

                    glColor3fv  ( pvc[ i + 2 ].rgb );
                    glVertex3fv ( pv [ i + 2 ].xyz );
                    glColor3fv  ( pvc[ i     ].rgb );
                    glVertex3fv ( pv [ i     ].xyz );
                    glColor3fv  ( pvc[ i + 1 ].rgb );
                    glVertex3fv ( pv [ i + 1 ].xyz );

                  }  /* end else */
                } /* if */
              } /* for */

            } else if ( pfc ) {

              for ( i = 0; i < p -> num_facets; ++i ) {

                if ( p -> d.bDraw[ i ] ) {

                  if ( i % 2 ) {

                    glColor3fv ( pfc[ i     ].rgb );
                    glVertex3fv( pv [ i     ].xyz );
                    glColor3fv ( pfc[ i + 2 ].rgb );
                    glVertex3fv( pv [ i + 2 ].xyz );
                    glColor3fv ( pfc[ i + 1 ].rgb );
                    glVertex3fv( pv [ i + 1 ].xyz );

                  } else {

                    glColor3fv ( pfc[ i + 2 ].rgb );
                    glVertex3fv( pv [ i + 2 ].xyz );
                    glColor3fv ( pfc[ i     ].rgb );
                    glVertex3fv( pv [ i     ].xyz );
                    glColor3fv ( pfc[ i + 1 ].rgb );
                    glVertex3fv( pv [ i + 1 ].xyz );

                  }  /* end else */
                }  /* if */
              } /* for */

            } else {
              for ( i = 0; i < p -> num_facets; ++i ) {

                if ( p -> d.bDraw[ i ] ) {

                  if ( i % 2 ) {

                    glVertex3fv ( pv[ i     ].xyz );
                    glVertex3fv ( pv[ i + 2 ].xyz );
                    glVertex3fv ( pv[ i + 1 ].xyz );

                  } else {

                    glVertex3fv ( pv[ i + 2 ].xyz );
                    glVertex3fv ( pv[ i     ].xyz );
                    glVertex3fv ( pv[ i + 1 ].xyz );

                  }  /* end else */
                }  /* if */
              } /* for */
            }

          } else {  /* front_lighting_model != TelLModelNone */

            if ( pvt && !ForbidSetTextureMapping ) {

              for ( i = 0; i < p -> num_facets; ++i ) {

                if ( p -> d.bDraw[ i ] ) {

                  if ( i % 2 ) {

                    glNormal3fv   ( pvn[ i     ].xyz );
                    glTexCoord2fv ( pvt[ i     ].xy  );
                    glVertex3fv   ( pv [ i     ].xyz );
                    glNormal3fv   ( pvn[ i + 2 ].xyz );
                    glTexCoord2fv ( pvt[ i + 2 ].xy  );
                    glVertex3fv   ( pv [ i + 2 ].xyz );
                    glNormal3fv   ( pvn[ i + 1 ].xyz );
                    glTexCoord2fv ( pvt[ i + 1 ].xy  );
                    glVertex3fv   ( pv [ i + 1 ].xyz );

                  } else {

                    glNormal3fv   ( pvn[ i + 2 ].xyz );
                    glTexCoord2fv ( pvt[ i + 2 ].xy  );
                    glVertex3fv   ( pv [ i + 2 ].xyz );
                    glNormal3fv   ( pvn[ i     ].xyz );
                    glTexCoord2fv ( pvt[ i     ].xy  );
                    glVertex3fv   ( pv [ i     ].xyz );
                    glNormal3fv   ( pvn[ i + 1 ].xyz );
                    glTexCoord2fv ( pvt[ i + 1 ].xy  );
                    glVertex3fv   ( pv [ i + 1 ].xyz );

                  }  /* end else */
                } /* if */
              }  /* end for */

            } else {
              for ( i = 0; i < p -> num_facets; ++i ) {

                if ( p -> d.bDraw[ i ] ) {

                  if ( i % 2 ) {

                    glNormal3fv( pvn[ i     ].xyz );
                    glVertex3fv( pv[  i     ].xyz );
                    glNormal3fv( pvn[ i + 2 ].xyz );
                    glVertex3fv( pv[  i + 2 ].xyz );
                    glNormal3fv( pvn[ i + 1 ].xyz );
                    glVertex3fv( pv[  i + 1 ].xyz );

                  } else {

                    glNormal3fv( pvn[ i + 2 ].xyz );
                    glVertex3fv( pv[  i + 2 ].xyz );
                    glNormal3fv( pvn[ i     ].xyz );
                    glVertex3fv( pv[  i     ].xyz );
                    glNormal3fv( pvn[ i + 1 ].xyz );
                    glVertex3fv( pv[  i + 1 ].xyz );

                  }  /* end else */

                } /* if */
              } /* for */
            } /* end else */
          }  /* end else front_lighting_model != TelLModelNone */

          glEnd ();

        } else {

          if ( !p -> d.dlist ) p -> d.dlist = glGenLists ( 1 );

          glNewList ( p -> d.dlist, GL_COMPILE_AND_EXECUTE );
          newList = 1;

        }  /* end else */
        if ( newList ) glEndList ();

        if ( g_nDegenerateModel ) return;

      } else {

        glCallList ( g_fSkipRatio == 0.0 ? p -> d.list : p -> d.dlist );

        if ( g_nDegenerateModel ) return;

      }  /* end else */
#endif  /* G003 */
  }  /* end if ( interior_style != TSM_EMPTY ) */
#ifdef G003
  i = 0;

  /* OCC11904 -- Temporarily disable environment mapping */
  glPushAttrib(GL_ENABLE_BIT);
  glDisable(GL_TEXTURE_1D);
  glDisable(GL_TEXTURE_2D);

  switch ( g_nDegenerateModel ) {

  default:
    break;

  case 2:  /* XXX_TDM_WIREFRAME */
    i = 1;
    break;

  case 3:  /* XXX_TDM_MARKER */
    draw_degenerates_as_points ( &p -> d, p -> vertices, p -> num_facets + 2 );
    glPopAttrib();
    return;

  case 4:  /* XXX_TDM_BBOX */
    draw_degenerates_as_bboxs ( &p -> d, p -> vertices, p -> num_facets + 2 );
    glPopAttrib();
    return;

  }  /* end switch */

  draw_edges ( p, edge_colour, interior_style, i );
#else
  draw_edges ( p, edge_colour, interior_style    );
#endif  /* G003 */

  glPopAttrib();
}  /* end draw_tmesh */

/*----------------------------------------------------------------------*/
static  TStatus
TriangleMeshDelete( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  tel_tmesh_data p;

  p = (tel_tmesh_data)data.pdata;
  if( p->fnormals )
    delete[] p->fnormals;
  if( p->fcolours )
    delete[] p->fcolours;
  if( p->vertices )
    delete[] p->vertices;
  if( p->vcolours )
    delete[] p->vcolours;
  if( p->vnormals )
    delete[] p->vnormals;
  if (p->vtexturecoord)
    delete[] p->vtexturecoord;
#ifdef G003
  if (  GET_GL_CONTEXT() != NULL  ) {

    if ( p ->  d.list ) glDeleteLists ( p ->  d.list, 1 );
    if ( p -> d.dlist ) glDeleteLists ( p -> d.dlist, 1 );

  }  /* end if */

  if ( p -> d.bDraw )
    delete[] p->d.bDraw;
#endif  /* G003 */
  if (data.pdata)
    delete data.pdata;
  return TSuccess;
}

/*----------------------------------------------------------------------*/
static  TStatus
TriangleMeshPrint( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  Tint           i;
  tel_tmesh_data p;

  p = (tel_tmesh_data)data.pdata;

  fprintf(stdout, "TelTriangleMesh. Number of Facets : %d\n", p->num_facets);
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
  if( p->fnormals )
  {
    fprintf( stdout, "\n\t\tFacet Normals : " );
    for( i = 0; i < p->num_facets; i++ )
      fprintf( stdout, "\n\t\t v[%d] = %g %g %g", i,
      p->fnormals[i].xyz[0],
      p->fnormals[i].xyz[1],
      p->fnormals[i].xyz[2] );
  }
  else
    fprintf( stdout, "\n\t\tFacet Normals not specified\n" );

  if( p->fcolours )
  {
    fprintf( stdout, "\n\t\tFacet Colours : " );
    for( i = 0; i < p->num_facets; i++ )
      fprintf( stdout, "\n\t\t v[%d] = %g %g %g", i,
      p->fcolours[i].rgb[0],
      p->fcolours[i].rgb[1],
      p->fcolours[i].rgb[2] );    }
  else
    fprintf( stdout, "\n\t\tFacet Colours not specified\n" );
  if( p->vertices )
  {
    fprintf( stdout, "\n\t\tVertices : " );
    for( i = 0; i < p->num_facets+2; i++ )
      fprintf( stdout, "\n\t\t v[%d] = %g %g %g", i,
      p->vertices[i].xyz[0],
      p->vertices[i].xyz[1],
      p->vertices[i].xyz[2] );    }

  fprintf( stdout, "\n" );
  if( p->vcolours )
  {
    fprintf( stdout, "\n\t\tVertex Colours : " );
    for( i = 0; i < p->num_facets+2; i++ )
      fprintf( stdout, "\n\t\t v[%d] = %g %g %g", i,
      p->vcolours[i].rgb[0],
      p->vcolours[i].rgb[1],
      p->vcolours[i].rgb[2] );    }
  else
    fprintf( stdout, "\n\t\tVertex Colours not specified\n" );

  if( p->vnormals )
  {
    fprintf( stdout, "\n\t\tVertex Normals : " );
    for( i = 0; i < p->num_facets+2; i++ )
      fprintf( stdout, "\n\t\t v[%d] = %g %g %g", i,
      p->vnormals[i].xyz[0],
      p->vnormals[i].xyz[1],
      p->vnormals[i].xyz[2] );    }
  else
    fprintf( stdout, "\n\t\tVertex Normals not specified\n" );

  if (p->vtexturecoord)
  {
    fprintf(stdout, "\n\t\tTexture Coordinates : ");
    for (i=0; i<p->num_facets+2; i++)
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
#ifndef G003
static void draw_edges ( tel_tmesh_data p, tel_colour edge_colour, Tint interior_style ) 
{
#else
static void draw_edges (
                        tel_tmesh_data           p,
                        tel_colour     edge_colour,
                        Tint        interior_style,
                        Tint             forceDraw
                        ) 
{
#endif  /* G003 */
  CMN_KEY   k, k1, k2, k3, k4;
  Tint      i, edge_type, line_type_preserve, newList = 0;
  Tfloat    edge_width, line_width_preserve;
  GLboolean texture_on;
  tel_point pv;
#ifdef G003
  if ( interior_style != TSM_HIDDENLINE && !forceDraw ) {
#else
  if ( interior_style != TSM_HIDDENLINE ) {
#endif  /* G003 */     

    k.id = TelEdgeFlag;
    TsmGetAttri ( 1, &k );

    if ( k.data.ldata == TOff ) return;

  }  /* end if */

  pv = p -> vertices;

  LightOff ();
  texture_on = IsTextureEnabled ();
  if ( texture_on ) DisableTexture ();

  k1.id = TelPolylineWidth;
  k2.id = TelPolylineType;
  k3.id = TelEdgeType;
  k4.id = TelEdgeWidth;
  TsmGetAttri ( 4, &k1, &k2, &k3, &k4 );

  line_width_preserve = k1.data.fdata;
  line_type_preserve  = k2.data.ldata;
  edge_type           = k3.data.ldata;
  edge_width          = k4.data.fdata;

  if ( line_width_preserve != edge_width ) {

    k.id         = TelPolylineWidth;
    k.data.fdata = edge_width;
    TsmSetAttri ( 1, &k );

  }  /* end if */

  if ( line_type_preserve != edge_type ) {

    k.id         = TelPolylineType;
    k.data.ldata = edge_type;
    TsmSetAttri ( 1, &k );

  }  /* end if */

  glColor3fv ( edge_colour -> rgb );
#ifdef G003
  if ( !forceDraw )
#endif  /* G003 */
    draw_line_loop ( p );
#ifdef G003
  else {
#ifdef BUC60876
    if ( !g_fAnimation )
      goto doDraw; /* Disable object display list out of animation */
#endif
# ifdef G004
    if ( g_fBitmap ) 
      goto doDraw;
# endif  /* G004 */
    if ( p -> d.degMode != 2 || p -> d.skipRatio != g_fSkipRatio || !p -> d.dlist ) {

      if ( !p -> d.dlist ) p -> d.dlist = glGenLists ( 1 );

      p -> d.degMode   = 2;
      p -> d.skipRatio = g_fSkipRatio;
      glNewList ( p -> d.dlist, GL_COMPILE_AND_EXECUTE );
      newList = 1;
doDraw:
      glPushAttrib ( GL_DEPTH_BUFFER_BIT );
      glDisable ( GL_DEPTH_TEST );

      if ( g_fSkipRatio == 0.0 )

        draw_line_loop ( p );

      else if ( g_fSkipRatio != 1.0 ) {

        set_drawable_items ( p -> d.bDraw, p -> num_facets + 2 );

        for ( i = 0; i < p -> num_facets; ++i )

          if ( p -> d.bDraw[ i ] ) {

            glBegin ( GL_LINE_LOOP );
            glVertex3fv ( pv[ i     ].xyz );
            glVertex3fv ( pv[ i + 1 ].xyz );
            glVertex3fv ( pv[ i + 2 ].xyz );
            glEnd();

          }  /* end if */

      }  /* end if */

      glPopAttrib ();
      if ( newList ) glEndList ();

    } else glCallList ( p -> d.dlist );

  }  /* end else */
#endif  /* G003 */
  if ( line_width_preserve != edge_width ) {

    k.id         = TelPolylineWidth;
    k.data.fdata = line_width_preserve;
    TsmSetAttri ( 1, &k );

  }  /* end if */

  if ( line_type_preserve != edge_type ) {

    k.id         = TelPolylineType;
    k.data.ldata = line_type_preserve;
    TsmSetAttri ( 1, &k );

  }  /* end if */

  if ( texture_on ) EnableTexture ();

}  /* end draw_edges */
#ifdef G003
void draw_degenerates_as_points ( PDS_INTERNAL pd, tel_point p, Tint n ) {

  int i, newList = 0; 

  LightOff ();
#ifdef BUC60876
  if ( !g_fAnimation )
    goto doDraw; /* Disable object display list out of animation */
#endif
#ifdef G004
  if ( g_fBitmap ) 
    goto doDraw;
#endif  /* G004 */
  if ( pd -> degMode != 3 || pd -> skipRatio != g_fSkipRatio || !pd -> dlist ) {

    if ( !pd -> dlist ) pd -> dlist = glGenLists ( 1 );

    pd -> degMode   = 3;
    pd -> skipRatio = g_fSkipRatio;
    glNewList ( pd -> dlist, GL_COMPILE_AND_EXECUTE );
    newList = 1;

    if ( g_fSkipRatio == 0.0 ) {
#ifdef G004
doDraw:
#endif  /* G004 */
      glBegin ( GL_POINTS );

      for ( i = 0; i < n; ++i ) glVertex3fv ( p[ i ].xyz );

      glEnd ();

    } else if ( g_fSkipRatio != 1.0 ) {

      set_drawable_items ( pd -> bDraw, n );

      glBegin ( GL_POINTS );

      for ( i = 0; i < n; ++i )

        if ( pd -> bDraw[ i ] ) glVertex3fv ( p[ i ].xyz );

      glEnd ();

    }  /* end if */
    if ( newList ) glEndList ();

  } else glCallList ( pd -> dlist );

}  /* end draw_degenerates_as_points */

void draw_degenerates_as_bboxs ( PDS_INTERNAL pd, tel_point p, Tint n ) {

  int     i, newList = 0;
  GLfloat minp[ 3 ] = { FLT_MAX, FLT_MAX, FLT_MAX };
  GLfloat maxp[ 3 ] = { FLT_MIN, FLT_MIN, FLT_MIN };

  LightOff ();
#ifdef BUC60876
  if ( !g_fAnimation )
    goto doDraw; /* Disable object display list out of animation */
#endif
#ifdef G004
  if ( g_fBitmap ) 
    goto doDraw;
#endif  /* G004 */
  if ( pd -> degMode != 4 || !pd -> dlist ) {

    if ( !pd -> dlist ) pd -> dlist = glGenLists ( 1 );

    pd -> degMode = 4;

    glNewList ( pd -> dlist, GL_COMPILE_AND_EXECUTE );
    newList = 1; 
#ifdef G004
doDraw:
#endif  /* G004 */
    for ( i = 0; i < n; ++i ) {

      TEL_POINT pt = p[ i ];

      if ( pt.xyz[ 0 ] < minp[ 0 ] )
        minp[ 0 ] = pt.xyz[ 0 ] ;
      if ( pt.xyz[ 1 ] < minp[ 1 ] )
        minp[ 1 ] = pt.xyz[ 1 ] ;
      if ( pt.xyz[ 2 ] < minp[ 2 ] )
        minp[ 2 ] = pt.xyz[ 2 ] ;

      if ( pt.xyz[ 0 ] > maxp[ 0 ] )
        maxp[ 0 ] = pt.xyz[ 0 ] ;
      if ( pt.xyz[ 1 ] > maxp[ 1 ] )
        maxp[ 1 ] = pt.xyz[ 1 ] ;
      if ( pt.xyz[ 2 ] > maxp[ 2 ] )
        maxp[ 2 ] = pt.xyz[ 2 ] ;

    }  /* end for */

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
    if ( newList ) glEndList ();

  } else glCallList ( pd -> dlist );

}  /* end draw_degenerates_as_bboxs */
#endif  /* G003 */

static void draw_line_loop ( tel_tmesh_data p ) {

  int i;

  for ( i = 0; i < p -> num_facets; ++i ) {

    glBegin ( GL_LINE_LOOP );
    glVertex3fv ( p -> vertices[ i     ].xyz );
    glVertex3fv ( p -> vertices[ i + 1 ].xyz );
    glVertex3fv ( p -> vertices[ i + 2 ].xyz );
    glEnd();

  }  /* end for */

}  /* end draw_line_loop */

/*----------------------------------------------------------------------*/
static TStatus
TriangleMeshInquire( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  Tint             i, j;
  tel_tmesh_data   d;
  Tint             size_reqd=0;
  TStatus          status = TSuccess;
  Tchar            *cur_ptr = 0;

  d = (tel_tmesh_data)data.pdata;

  if( d->fcolours )
    size_reqd += ( d->num_facets * sizeof( TEL_COLOUR ) );

  if( d->facet_flag == TEL_FA_NORMAL )
    size_reqd += ( d->num_facets * sizeof( TEL_POINT ) );

  size_reqd += ( (d->num_facets+2) * sizeof( TEL_POINT ) );

  if( d->vcolours )
    size_reqd += ( (d->num_facets+2) * sizeof( TEL_COLOUR ) );

  if( d->vertex_flag == TEL_VT_NORMAL )
    size_reqd += ( (d->num_facets+2) * sizeof( TEL_POINT ) );

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

        c = (TEL_INQ_CONTENT*)k[i]->data.pdata;
        c->act_size = size_reqd;
        w = c->data;

        cur_ptr = c->buf;
        w->trianglestrip3data.num_facets = d->num_facets;

        if( c->size >= size_reqd )
        {
          if( d->facet_flag == TEL_FA_NORMAL )
          {
            if( d->fcolours )
            {
              w->trianglestrip3data.fctflag = TEL_FAFLAG_COLOURNORMAL;
              w->trianglestrip3data.gnormals = (tel_point)(c->buf);
              for( j = 0; j < d->num_facets; j++ )
              {
                w->trianglestrip3data.gnormals[j] = d->fnormals[j];
              }
              cur_ptr += ( d->num_facets * sizeof( TEL_POINT ) );

              w->trianglestrip3data.facet_colour_vals =
                (tel_colour)(cur_ptr);
              for( j = 0; j < d->num_facets; j++ )
              {
                w->trianglestrip3data.facet_colour_vals[j] =
                  d->fcolours[j];
              }
              cur_ptr += ( d->num_facets * sizeof( TEL_COLOUR ) );
            }
            else
            {
              w->trianglestrip3data.fctflag = TEL_FAFLAG_NORMAL;
              w->trianglestrip3data.facet_colour_vals = 0;
              w->trianglestrip3data.gnormals = (tel_point)(c->buf);
              for( j = 0; j < d->num_facets; j++ )
              {
                w->trianglestrip3data.gnormals[j] = d->fnormals[j];
              }
              cur_ptr += ( d->num_facets * sizeof( TEL_POINT ) );
            }
          }
          else
          {
            w->trianglestrip3data.gnormals = 0;
            if( d->fcolours )
            {
              w->trianglestrip3data.fctflag = TEL_FAFLAG_COLOUR;
              w->trianglestrip3data.facet_colour_vals =
                (tel_colour)(c->buf );
              for( j = 0; j < d->num_facets; j++ )
              {
                w->trianglestrip3data.facet_colour_vals[j] =
                  d->fcolours[j];
              }
              cur_ptr += ( d->num_facets * sizeof( TEL_COLOUR ) );
            }
            else
            {
              w->trianglestrip3data.fctflag = TEL_FAFLAG_NONE;
              w->trianglestrip3data.facet_colour_vals = 0;
            }
          }

          w->trianglestrip3data.points = (tel_point)cur_ptr;
          for( j = 0; j < (d->num_facets+2); j++ )
          {
            w->trianglestrip3data.points[j] = d->vertices[j];
          }
          cur_ptr += ( (d->num_facets+2) * sizeof( TEL_POINT ) );

          if( d->vertex_flag == TEL_VT_NORMAL )
          {
            if( d->vcolours )
            {
              w->trianglestrip3data.vrtflag = TEL_VTFLAG_COLOURNORMAL;
              w->trianglestrip3data.vnormals = (tel_point)(cur_ptr);
              for( j = 0; j < (d->num_facets+2); j++ )
              {
                w->trianglestrip3data.vnormals[j] = d->vnormals[i];
              }
              cur_ptr += ( (d->num_facets+2) * sizeof( TEL_POINT ) );

              w->trianglestrip3data.colours = (tel_colour)(cur_ptr);

              for( j = 0; j < (d->num_facets+2); j++ )
              {
                w->trianglestrip3data.colours[j] = d->vcolours[i];
              }
            }
            else
            {
              w->trianglestrip3data.vrtflag = TEL_VTFLAG_NORMAL;
              w->trianglestrip3data.colours = 0;
              w->trianglestrip3data.vnormals = (tel_point)(cur_ptr);

              for( j = 0; j < (d->num_facets+2); j++ )
              {
                w->trianglestrip3data.vnormals[j] = d->vnormals[i];
              }
            }
          }
          else
          {
            w->trianglestrip3data.vnormals = 0;
            if( d->vcolours )
            {
              w->trianglestrip3data.vrtflag = TEL_VTFLAG_COLOUR;
              w->trianglestrip3data.colours = (tel_colour)(cur_ptr);
              for( j = 0; j < (d->num_facets+2); j++ )
              {
                w->trianglestrip3data.colours[j] = d->vcolours[i];
              }
            }
            else
            {
              w->trianglestrip3data.vrtflag = TEL_VTFLAG_NONE;
              w->trianglestrip3data.colours = 0;
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
