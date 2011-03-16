#define G003
#define G004
/***********************************************************************

FONCTION :
----------
File OpenGl_qstrip :


REMARQUES:
---------- 


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
27-08-97 : FMN ; Correction affichage des edges
On n'affiche que les edges dans le mode IS_EMPTY
10-09-97 : FMN ; Amelioration des perfs liees aux lights.
15-09-97 : PCT ; Ajout coordonnees textures
24-09-97 : FMN ; Suppression OPTIMISATION_FMN.
08-12-97 : FMN ; Suppression appel TsmGetAttri inutile.
31-12-97 : FMN ; Simplification pour le highlight
15-01-98 : FMN ; Ajout Hidden line
17-02-00 : EUG ; Degeneration support
17-02-00 : EUG ; Bitmap drawing support
08-03-01 : GG  ; BUC60823 Avoid crash in the normal computation method
on confuse point.

************************************************************************/

#define BUC60876        /* GG 5/4/2001 Disable local display list
//                      when animation is not required
*/

/*----------------------------------------------------------------------*/
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

# define OPENGL_RAND() (  ( unsigned )( s_Rand = s_Rand * 214013L + 2531011L ))

extern int   g_nDegenerateModel;
extern float g_fSkipRatio;
extern GLboolean g_fAnimation;

#endif  /* G003 */

/*----------------------------------------------------------------------*/

struct TEL_QSTRIP_DATA
{
  Tint       shape_flag;  /* TEL_SHAPE_UNKNOWN or TEL_SHAPE_COMPLEX or
                          TEL_SHAPE_CONVEX  or TEL_SHAPE_CONCAVE */
  Tint       facet_flag;  /* TEL_FA_NONE or TEL_FA_NORMAL */
  Tint       vertex_flag; /* TEL_VT_NONE or TEL_VT_NORMAL */
  Tint       num_rows,
    num_columns; /* Number of rows and columns */
  tel_point  fnormals;    /* Facet normals */
  tel_colour fcolours;    /* Facet colours */
  tel_point  vertices;    /* Vertices */
  tel_colour vcolours;    /* Vertex colour values */
  tel_point  vnormals;    /* Vertex normals */
  tel_texture_coord vtexturecoord; /* Texture coordinates */
#ifdef G003
  DS_INTERNAL d;
#endif  /* G003 */
  IMPLEMENT_MEMORY_OPERATORS
};
typedef TEL_QSTRIP_DATA* tel_qstrip_data;

static  TStatus  QuadrangleDisplay( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  QuadrangleAdd( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  QuadrangleDelete( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  QuadranglePrint( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  QuadrangleInquire( TSM_ELEM_DATA, Tint, cmn_key* );

static void
draw_qstrip( tel_qstrip_data,
            Tint,          /* highlight flag */
            Tint,          /* front_lighting_model,  */
            Tint,          /* interior_style,  */
            tel_colour    /* edge_colour, */
            );

#ifdef G003
static void draw_edges                 ( tel_qstrip_data, tel_colour, Tint, Tint );
extern void draw_degenerates_as_points ( PDS_INTERNAL, tel_point, Tint           );
extern void draw_degenerates_as_bboxs  ( PDS_INTERNAL, tel_point, Tint           );
extern void set_drawable_items         ( GLboolean*, int                         );
#else
static void draw_edges ( tel_qstrip_data, tel_colour, Tint );
#endif  /* G003 */

static void draw_line_strip ( tel_qstrip_data, tel_colour, Tint, Tint );

static TEL_QSTRIP_DATA qstrip_defaults = {

  TEL_SHAPE_UNKNOWN, /* shape_flag */
  TEL_FA_NONE,       /* facet_flag */
  TEL_VT_NONE        /* vertex_flag */

};

static TStatus ( *MtdTbl[] ) ( TSM_ELEM_DATA, Tint, cmn_key* ) = {

  QuadrangleDisplay, /* PickTraverse */
  QuadrangleDisplay,
  QuadrangleAdd,
  QuadrangleDelete,
  QuadranglePrint,
  QuadrangleInquire

};

/*----------------------------------------------------------------------*/

MtblPtr TelQuadrangleInitClass ( TelType* el ) {

  *el = TelQuadrangle;

  return MtdTbl;

}  /* end TelQuadrangleInitClass */

static  TStatus
QuadrangleAdd( TSM_ELEM_DATA d, Tint n, cmn_key *k )

{
  Tint i, j;
  Tint nf, nv;
  tel_qstrip_data  data;

  for( i = 0; i < n; i++ )
  {
    if( k[i]->id == NUM_ROWS_ID )
      break;
  }
  if( i == n )
    return TFailure;

  if( !(k[i]->data.ldata) )
    return TFailure;

  for( j = 0; j < n; j++ )
  {
    if( k[j]->id == NUM_COLUMNS_ID )
      break;
  }
  if( j == n )
    return TFailure;

  if( !(k[j]->data.ldata) )
    return TFailure;

  data = new TEL_QSTRIP_DATA();
  if( !data )
    return TFailure;

  /* load defaults */
  *data = qstrip_defaults;

  data->num_rows    = k[i]->data.ldata;
  data->num_columns = k[j]->data.ldata;
  nf = data->num_rows * data->num_columns;
  nv = ( data->num_rows+1 ) * ( data->num_columns+1 );

  data->vtexturecoord = 0;

  for( i = 0; i < n; i++ )
  {
    switch( k[i]->id )
    {
    case SHAPE_FLAG_ID:
      {
        data->shape_flag = k[i]->data.ldata;
        break;
      }
    case FNORMALS_ID:
      {
        data->facet_flag = TEL_FA_NORMAL;
        data->fnormals = new TEL_POINT[nf];
        memcpy( data->fnormals, k[i]->data.pdata, nf*sizeof(TEL_POINT) );
        for( j = 0; j < nf; j++ )
          vecnrm( data->fnormals[j].xyz );
        break;
      }
    case FACET_COLOUR_VALS_ID:
      {
        data->fcolours = new TEL_COLOUR[nf];
        memcpy( data->fcolours, k[i]->data.pdata, nf*sizeof(TEL_COLOUR) );
        break;
      }
    case VERTICES_ID:
      {
        data->vertices = new TEL_POINT[nv];
        memcpy( data->vertices, k[i]->data.pdata, nv*sizeof(TEL_POINT) );
        break;
      }
    case VERTEX_COLOUR_VALS_ID:
      {
        data->vcolours = new TEL_COLOUR[nv];
        memcpy( data->vcolours, k[i]->data.pdata, nv*sizeof(TEL_COLOUR) );
        break;
      }
    case VNORMALS_ID:
      {
        data->vertex_flag = TEL_VT_NORMAL;
        data->vnormals = new TEL_POINT[nv];
        memcpy( data->vnormals, k[i]->data.pdata, nv*sizeof(TEL_POINT) );
        for( j = 0; j < nv; j++ )
          vecnrm( data->vnormals[j].xyz );
        break;
      }
    case VTEXTURECOORD_ID:
      {
        data->vtexturecoord = new TEL_TEXTURE_COORD[nv];
        memcpy( data->vtexturecoord, k[i]->data.pdata, nv*sizeof(TEL_TEXTURE_COORD) );
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
    data->fnormals = new TEL_POINT[nf];
    for( i = 0; i < nf; i++ )
    {
      TEL_POINT  p1, p2, p3, p4;
      Tint       r, c, id;

      r = i/data->num_columns;
      c = i%data->num_columns;

      id = r*(data->num_columns+1) + c;
      p1 = data->vertices[id];
      p2 = data->vertices[id+data->num_columns+1];
      p3 = data->vertices[id+data->num_columns+2];
      p4 = data->vertices[id+1];

      p4.xyz[0] -= p2.xyz[0];
      p4.xyz[1] -= p2.xyz[1];
      p4.xyz[2] -= p2.xyz[2];

      p4.xyz[0] += p1.xyz[0];
      p4.xyz[1] += p1.xyz[1];
      p4.xyz[2] += p1.xyz[2];

      TelGetNormal( p4.xyz, p1.xyz, p3.xyz, data->fnormals[i].xyz );
    }
#ifndef BUC60823
    for( j = 0; j < nf; j++ )
      vecnrm( data->fnormals[j].xyz );
#endif
  }
  if( data->vertex_flag != TEL_VT_NORMAL )
  {
    Tint r, c, id;
    /* calculate the vertex normals */
    data->vnormals = new TEL_POINT[nv];
    for( i = 0; i < nv; i++ )
    {
      if( i == 0 )
        data->vnormals[i] = data->fnormals[i];
      else if( i == (nv-1) )
        data->vnormals[i] = data->fnormals[nf-1];
      else if( i == (nv-data->num_columns-1) )
        data->vnormals[i] = data->fnormals[nf-data->num_columns];
      else if( i == data->num_columns )
        data->vnormals[i] = data->fnormals[i-1];
      else if( i%(data->num_columns+1) == 0 )
      {
        r = i/(data->num_columns+1);
        data->vnormals[i].xyz[0] =
          ( data->fnormals[(r-1)*data->num_columns].xyz[0] +
          data->fnormals[r*data->num_columns].xyz[0] ) / ( float )2.0;
        data->vnormals[i].xyz[1] =
          ( data->fnormals[(r-1)*data->num_columns].xyz[1] +
          data->fnormals[r*data->num_columns].xyz[1] ) / ( float )2.0;
        data->vnormals[i].xyz[2] =
          ( data->fnormals[(r-1)*data->num_columns].xyz[2] +
          data->fnormals[r*data->num_columns].xyz[2] ) / ( float )2.0;
      }
      else if( i <= data->num_columns )
      {
        data->vnormals[i].xyz[0] = ( data->fnormals[i-1].xyz[0] +
          data->fnormals[i].xyz[0] ) / ( float )2.0;
        data->vnormals[i].xyz[1] = ( data->fnormals[i-1].xyz[1] +
          data->fnormals[i].xyz[1] ) / ( float )2.0;
        data->vnormals[i].xyz[2] = ( data->fnormals[i-1].xyz[2] +
          data->fnormals[i].xyz[2] ) / ( float )2.0;
      }
      else if( i >= (nv-data->num_columns-1 ) )
      {
        c = i%(data->num_columns+1);
        id = nf-data->num_columns+c;

        data->vnormals[i].xyz[0] = ( data->fnormals[id-1].xyz[0] +
          data->fnormals[id].xyz[0] ) / ( float )2.0;
        data->vnormals[i].xyz[1] = ( data->fnormals[id-1].xyz[1] +
          data->fnormals[id].xyz[1] ) / ( float )2.0;
        data->vnormals[i].xyz[2] = ( data->fnormals[id-1].xyz[2] +
          data->fnormals[id].xyz[2] ) / ( float )2.0;
      }
      else if( (i+1)%(data->num_columns+1) == 0 )
      {
        r = (i+1)/(data->num_columns+1);
        data->vnormals[i].xyz[0] =
          ( data->fnormals[(r-1)*data->num_columns-1].xyz[0] +
          data->fnormals[r*data->num_columns-1].xyz[0] ) / ( float )2.0;
        data->vnormals[i].xyz[1] =
          ( data->fnormals[(r-1)*data->num_columns-1].xyz[1] +
          data->fnormals[r*data->num_columns-1].xyz[1] ) / ( float )2.0;
        data->vnormals[i].xyz[2] =
          ( data->fnormals[(r-1)*data->num_columns-1].xyz[2] +
          data->fnormals[r*data->num_columns-1].xyz[2] ) / ( float )2.0;
      }
      else
      {
        r = i/(data->num_columns+1);
        c = i%(data->num_columns+1);

        id = r*data->num_columns+c;

        data->vnormals[i].xyz[0] =
          ( data->fnormals[id].xyz[0] +
          data->fnormals[id-data->num_columns].xyz[0] +
          data->fnormals[id-1].xyz[0] +
          data->fnormals[id-data->num_columns-1].xyz[0] ) / ( float )4.0;
        data->vnormals[i].xyz[1] =
          ( data->fnormals[id].xyz[1] +
          data->fnormals[id-data->num_columns].xyz[1] +
          data->fnormals[id-1].xyz[1] +
          data->fnormals[id-data->num_columns-1].xyz[1] ) / ( float )4.0;
        data->vnormals[i].xyz[2] =
          ( data->fnormals[id].xyz[2] +
          data->fnormals[id-data->num_columns].xyz[2] +
          data->fnormals[id-1].xyz[2] +
          data->fnormals[id-data->num_columns-1].xyz[2] ) / ( float )4.0;
      }
    }
  }
#ifdef G003
  data -> d.list      =  0;
  data -> d.dlist     =  0;
  data -> d.degMode   =  0;
  data -> d.model     = -1;
  data -> d.skipRatio =  0.0F;
  data->d.bDraw = new unsigned char[nv];
#endif  /* G003 */
  ((tsm_elem_data)(d.pdata))->pdata = data;

  return TSuccess;
}
/*----------------------------------------------------------------------*/

static  TStatus
QuadrangleDisplay( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  CMN_KEY       k11, k12, k17, k111, k114;

  Tint           front_lighting_model;
  Tint           interior_style;
  TEL_COLOUR     interior_colour;
  TEL_COLOUR     edge_colour;

  tel_qstrip_data d;

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
  printf("QuadrangleDisplay \n"); 
#endif

  /*   
  *  Use highlight colours  
  */

  if( k[0]->id == TOn )
  {                         
    TEL_HIGHLIGHT  hrep;

    k11.id = TelHighlightIndex;
    TsmGetAttri( 1, &k11 );
    if( TelGetHighlightRep( TglActiveWs, k11.data.ldata, &hrep )
      == TSuccess )
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

  d = (tel_qstrip_data)data.pdata;

  glColor3fv( interior_colour.rgb );

  draw_qstrip( d,
    k[0]->id,
    front_lighting_model,
    interior_style,
    &edge_colour );

  return TSuccess;
}

/*----------------------------------------------------------------------*/

static void draw_qstrip (
                         tel_qstrip_data                  p,
                         Tint                         hflag,
                         Tint          front_lighting_model,
                         Tint                interior_style,
                         tel_colour             edge_colour
                         )
{

  Tint              i, m, newList = 0;
  Tint              nf, nv, nr, nc;
  Tint              lighting_model;
  tel_point         pv, pfn, pvn;
  tel_colour        pfc, pvc;
  tel_texture_coord pvt;

  pfn = p -> fnormals;
  pfc = p -> fcolours;
  pv  = p -> vertices;
  pvc = p -> vcolours;
  pvn = p -> vnormals;
  pvt = p -> vtexturecoord;

  if ( hflag == TOn ) {

    pvc = 0;
    pfc = 0;

  }  /* end if */

#ifdef G003
  if ( g_nDegenerateModel < 2 && interior_style != TSM_EMPTY ) {
#else
  if ( interior_style != TSM_EMPTY ) {
#endif  /* G003 */
    if ( front_lighting_model == CALL_PHIGS_REFL_NONE )

      LightOff ();

    else LightOn ();

    lighting_model = front_lighting_model;
    nr             = p -> num_rows;
    nc             = p -> num_columns;
    nf             = nr * nc;
    nv             = ( nr + 1 ) * ( nc + 1 );
#ifdef G003
#ifdef BUC60876
    if ( !g_fAnimation )
      goto doDraw; /* Disable object display list out of animation */
#endif
# ifdef G004
    if ( g_fBitmap ) 
      goto doDraw;  /* display lists are NOT shared between */
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
          if ( lighting_model == CALL_PHIGS_REFL_NONE ) {

            if ( pvc ) {

              for ( m = 0; m < nr; ++m ) {

                glBegin ( GL_QUAD_STRIP );
                glColor3fv  ( pvc[         m * ( nc + 1 ) ].rgb );
                glVertex3fv ( pv[          m * ( nc + 1 ) ].xyz );
                glColor3fv  ( pvc[ ( m + 1 ) * ( nc + 1 ) ].rgb );
                glVertex3fv ( pv[  ( m + 1 ) * ( nc + 1 ) ].xyz );

                for ( i = 1; i < nc + 1; ++i ) {

                  glColor3fv  ( pvc[         i + m * ( nc + 1 ) ].rgb );
                  glVertex3fv ( pv[          i + m * ( nc + 1 ) ].xyz );
                  glColor3fv  ( pvc[ i + ( m + 1 ) * ( nc + 1 ) ].rgb );
                  glVertex3fv ( pv[  i + ( m + 1 ) * ( nc + 1 ) ].xyz );

                }  /* end for */

                glEnd ();

              }  /* end for */

            } else if ( pfc ) {

              glColor3fv ( pfc[ 0 ].rgb );

              for ( m = 0; m < nr; ++m ) {

                glBegin ( GL_QUAD_STRIP );
                glColor3fv  ( pfc[                m * nc ].rgb );
                glVertex3fv ( pv[         m * ( nc + 1 ) ].xyz );
                glVertex3fv ( pv[ ( m + 1 ) * ( nc + 1 ) ].xyz );

                for ( i = 1; i < nc + 1; ++i ) {

                  glVertex3fv ( pv[         i + m * ( nc + 1 ) ].xyz );
                  glColor3fv  ( pfc[            i - 1 + m * nc ].rgb );
                  glVertex3fv ( pv[ i + ( m + 1 ) * ( nc + 1 ) ].xyz );

                }  /* end for */

                glEnd ();

              }  /* end for */

            } else {

              for ( m = 0; m < nr; ++m ) {

                glBegin ( GL_QUAD_STRIP );
                glVertex3fv ( pv[         m * ( nc + 1 ) ].xyz );
                glVertex3fv ( pv[ ( m + 1 ) * ( nc + 1 ) ].xyz );

                for ( i = 1; i < nc + 1; ++i ) {

                  glVertex3fv( pv[         i + m * ( nc + 1 ) ].xyz );
                  glVertex3fv( pv[ i + ( m + 1 ) * ( nc + 1 ) ].xyz );

                }  /* end for */

                glEnd ();

              }  /* end for */

            }  /* end else */

          } else {  /* lighting_model != TelLModelNone */

            if ( pvt && !ForbidSetTextureMapping )

              for ( m = 0; m < nr; ++m ) {

                glBegin ( GL_QUAD_STRIP );
                glNormal3fv   ( pvn[         m * ( nc + 1 ) ].xyz );
                glTexCoord2fv ( pvt[         m * ( nc + 1 ) ].xy  );
                glVertex3fv   ( pv[          m * ( nc + 1 ) ].xyz );
                glNormal3fv   ( pvn[ ( m + 1 ) * ( nc + 1 ) ].xyz );
                glTexCoord2fv ( pvt[ ( m + 1 ) * ( nc + 1 ) ].xy  );
                glVertex3fv   ( pv[  ( m + 1 ) * ( nc + 1 ) ].xyz );

                for ( i = 1; i < nc + 1; ++i ) {

                  glNormal3fv   ( pvn[         i + m * ( nc + 1 ) ].xyz );
                  glTexCoord2fv ( pvt[         i + m * ( nc + 1 ) ].xy  );
                  glVertex3fv   ( pv[          i + m * ( nc + 1 ) ].xyz );
                  glNormal3fv   ( pvn[ i + ( m + 1 ) * ( nc + 1 ) ].xyz );
                  glTexCoord2fv ( pvt[ i + ( m + 1 ) * ( nc + 1 ) ].xy  );
                  glVertex3fv   ( pv[  i + ( m + 1 ) * ( nc + 1 ) ].xyz );

                }  /* end for */

                glEnd ();

              }  /* end for */

            else

              for ( m = 0; m < nr; ++m ) {

                glBegin ( GL_QUAD_STRIP );
                glNormal3fv ( pvn[         m * ( nc + 1 ) ].xyz );
                glVertex3fv ( pv[          m * ( nc + 1 ) ].xyz );
                glNormal3fv ( pvn[ ( m + 1 ) * ( nc + 1 ) ].xyz );
                glVertex3fv ( pv[  ( m + 1 ) * ( nc + 1 ) ].xyz );

                for ( i = 1; i < nc + 1; ++i ) {

                  glNormal3fv ( pvn[         i + m * ( nc + 1 ) ].xyz );
                  glVertex3fv ( pv[          i + m * ( nc + 1 ) ].xyz );
                  glNormal3fv ( pvn[ i + ( m + 1 ) * ( nc + 1 ) ].xyz );
                  glVertex3fv ( pv[  i + ( m + 1 ) * ( nc + 1 ) ].xyz );

                }  /* end for */

                glEnd ();

              }  /* end for */

          }  /* end else */
#ifdef G003
        } else if ( g_fSkipRatio != 1.0 ) {

          set_drawable_items ( p -> d.bDraw, nr );

          if ( !p -> d.dlist ) p -> d.dlist = glGenLists ( 1 );

          glNewList ( p -> d.dlist, GL_COMPILE_AND_EXECUTE );
          newList = 1;

          if ( lighting_model == CALL_PHIGS_REFL_NONE ) {

            if ( pvc ) {

              for ( m = 0; m < nr; ++m )

                if ( p -> d.bDraw[ m ] ) {

                  glBegin ( GL_QUAD_STRIP );
                  glColor3fv  ( pvc[         m * ( nc + 1 ) ].rgb );
                  glVertex3fv ( pv[          m * ( nc + 1 ) ].xyz );
                  glColor3fv  ( pvc[ ( m + 1 ) * ( nc + 1 ) ].rgb );
                  glVertex3fv ( pv[  ( m + 1 ) * ( nc + 1 ) ].xyz );

                  for ( i = 1; i < nc + 1; ++i ) {

                    glColor3fv  ( pvc[ i + ( m + 1 ) * ( nc + 1 ) ].rgb );
                    glVertex3fv ( pv[  i + ( m + 1 ) * ( nc + 1 ) ].xyz );
                    glColor3fv  ( pvc[         i + m * ( nc + 1 ) ].rgb );
                    glVertex3fv ( pv[          i + m * ( nc + 1 ) ].xyz );

                  }  /* end for */

                  glEnd ();

                }  /* end if */

            } else if ( pfc ) {

              glColor3fv ( pfc[ 0 ].rgb );

              for ( m = 0; m < nr; ++m )

                if ( p -> d.bDraw[ m ] ) {

                  glBegin ( GL_QUADS );
                  glColor3fv  ( pfc[                m * nc ].rgb );
                  glVertex3fv ( pv[         m * ( nc + 1 ) ].xyz );
                  glVertex3fv ( pv[ ( m + 1 ) * ( nc + 1 ) ].xyz );

                  for ( i = 1; i < nc + 1; ++i ) {

                    glColor3fv  ( pfc[            i - 1 + m * nc ].rgb );
                    glVertex3fv ( pv[ i + ( m + 1 ) * ( nc + 1 ) ].xyz );
                    glVertex3fv ( pv[         i + m * ( nc + 1 ) ].xyz );

                  }  /* end for */

                  glEnd ();

                }  /* end if */

            } else {

              for ( m = 0; m < nr; ++m )

                if ( p -> d.bDraw[ m ] ) {

                  glBegin ( GL_QUADS );
                  glVertex3fv ( pv[         m * ( nc + 1 ) ].xyz );
                  glVertex3fv ( pv[ ( m + 1 ) * ( nc + 1 ) ].xyz );

                  for ( i = 1; i < nc + 1; ++i ) {

                    glVertex3fv( pv[ i + ( m + 1 ) * ( nc + 1 ) ].xyz );
                    glVertex3fv( pv[         i + m * ( nc + 1 ) ].xyz );

                  }  /* end for */

                  glEnd ();

                }  /* end if */

            }  /* end else */

          } else {  /* lighting_model != TelLModelNone */

            if ( pvt && !ForbidSetTextureMapping ) {

              for ( m = 0; m < nr; ++m )

                if ( p -> d.bDraw[ m ] ) {

                  glBegin ( GL_QUADS );
                  glNormal3fv   ( pvn[         m * ( nc + 1 ) ].xyz );
                  glTexCoord2fv ( pvt[         m * ( nc + 1 ) ].xy  );
                  glVertex3fv   ( pv[          m * ( nc + 1 ) ].xyz );
                  glNormal3fv   ( pvn[ ( m + 1 ) * ( nc + 1 ) ].xyz );
                  glTexCoord2fv ( pvt[ ( m + 1 ) * ( nc + 1 ) ].xy  );
                  glVertex3fv   ( pv[  ( m + 1 ) * ( nc + 1 ) ].xyz );

                  for ( i = 1; i < nc + 1; ++i ) {

                    glNormal3fv   ( pvn[ i + ( m + 1 ) * ( nc + 1 ) ].xyz );
                    glTexCoord2fv ( pvt[ i + ( m + 1 ) * ( nc + 1 ) ].xy  );
                    glVertex3fv   ( pv[  i + ( m + 1 ) * ( nc + 1 ) ].xyz );
                    glNormal3fv   ( pvn[         i + m * ( nc + 1 ) ].xyz );
                    glTexCoord2fv ( pvt[         i + m * ( nc + 1 ) ].xy  );
                    glVertex3fv   ( pv[          i + m * ( nc + 1 ) ].xyz );

                  }  /* end for */

                  glEnd ();

                }  /* end if */

            } else

              for ( m = 0; m < nr; ++m )

                if ( p -> d.bDraw[ m ] ) {

                  glBegin ( GL_QUADS );
                  glNormal3fv ( pvn[         m * ( nc + 1 ) ].xyz );
                  glVertex3fv ( pv[          m * ( nc + 1 ) ].xyz );
                  glNormal3fv ( pvn[ ( m + 1 ) * ( nc + 1 ) ].xyz );
                  glVertex3fv ( pv[  ( m + 1 ) * ( nc + 1 ) ].xyz );

                  for ( i = 1; i < nc + 1; ++i ) {

                    glNormal3fv ( pvn[ i + ( m + 1 ) * ( nc + 1 ) ].xyz );
                    glVertex3fv ( pv[  i + ( m + 1 ) * ( nc + 1 ) ].xyz );
                    glNormal3fv ( pvn[         i + m * ( nc + 1 ) ].xyz );
                    glVertex3fv ( pv[          i + m * ( nc + 1 ) ].xyz );

                  }  /* end for */

                  glEnd ();

                }  /* end if */

          }  /* end else */

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
  draw_degenerates_as_points (
    &p -> d, p -> vertices, ( p -> num_rows + 1 ) * ( p -> num_columns + 1 )
    );

  glPopAttrib();

  return;

case 4:  /* XXX_TDM_BBOX */
  draw_degenerates_as_bboxs (
    &p -> d, p -> vertices, ( p -> num_rows + 1 ) * ( p -> num_columns + 1 )
    );

  glPopAttrib();

  return;

  }  /* end switch */

  draw_edges ( p, edge_colour, interior_style, i );
#else
  draw_edges ( p, edge_colour, interior_style    );
#endif  /* G003 */

  glPopAttrib();
}  /* end draw_qstrip */
/*----------------------------------------------------------------------*/
static  TStatus
QuadrangleDelete( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  tel_qstrip_data p;

  p = (tel_qstrip_data)data.pdata;
  if( p->fnormals ) //cmn_freemem( p->fnormals );
    delete[] p->fnormals;
  if( p->fcolours ) //cmn_freemem( p->fcolours );
    delete[] p->fcolours;
  if( p->vertices ) //cmn_freemem( p->vertices );
    delete[] p->vertices;
  if( p->vcolours ) //cmn_freemem( p->vcolours );
    delete[] p->vcolours;
  if( p->vnormals ) //cmn_freemem( p->vnormals );
    delete[] p->vnormals;
  if (p->vtexturecoord) //cmn_freemem(p->vtexturecoord);
    delete[] p->vtexturecoord;

  if (data.pdata)
    delete data.pdata;

  return TSuccess;
}

/*----------------------------------------------------------------------*/
static  TStatus
QuadranglePrint( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  Tint           i;
  tel_qstrip_data p = (tel_qstrip_data)data.pdata;

  fprintf(stdout, "TelQuadrangle. ");
  fprintf(stdout, "\n\tNumber of Rows : %d\n", p->num_rows);
  fprintf(stdout, "\n\tNumber of Columns : %d\n", p->num_columns);
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
  if( p->fnormals )
  {
    fprintf( stdout, "\n\t\tFacet Normals : " );
    for( i = 0; i < (p->num_rows * p->num_columns); i++ )
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
    for( i = 0; i < (p->num_rows * p->num_columns+1); i++ )
      fprintf( stdout, "\n\t\t v[%d] = %g %g %g", i,
      p->fcolours[i].rgb[0],
      p->fcolours[i].rgb[1],
      p->fcolours[i].rgb[2] );    }
  else
    fprintf( stdout, "\n\t\tFacet Colours not specified\n" );
  if( p->vertices )
  {
    fprintf( stdout, "\n\t\tVertices : " );
    for( i = 0; i < (p->num_rows+1)*(p->num_columns+1); i++ )
      fprintf( stdout, "\n\t\t v[%d] = %g %g %g", i,
      p->vertices[i].xyz[0],
      p->vertices[i].xyz[1],
      p->vertices[i].xyz[2] );    }

  fprintf( stdout, "\n" );
  if( p->vcolours )
  {
    fprintf( stdout, "\n\t\tVertex Colours : " );
    for( i = 0; i < (p->num_rows+1)*(p->num_columns+1); i++ )
      fprintf( stdout, "\n\t\t v[%d] = %g %g %g", i,
      p->vcolours[i].rgb[0],
      p->vcolours[i].rgb[1],
      p->vcolours[i].rgb[2] );    }
  else
    fprintf( stdout, "\n\t\tVertex Colours not specified\n" );

  if( p->vnormals )
  {
    fprintf( stdout, "\n\t\tVertex Normals : " );
    for( i = 0; i < (p->num_rows+1)*(p->num_columns+1); i++ )
      fprintf( stdout, "\n\t\t v[%d] = %g %g %g", i,
      p->vnormals[i].xyz[0],
      p->vnormals[i].xyz[1],
      p->vnormals[i].xyz[2] );    }
  else
    fprintf( stdout, "\n\t\tVertex Normals not specified\n" );

  if (p->vtexturecoord)
  {
    fprintf(stdout, "\n\t\tTexture Coordinates : ");
    for (i=0; i<(p->num_rows+1)*(p->num_columns+1); i++)
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
#ifdef G003
static void draw_edges (
                        tel_qstrip_data              p,
                        tel_colour         edge_colour,
                        Tint            interior_style,
                        Tint                 forceDraw
                        ) 
{
#else
static void draw_edges (
                        tel_qstrip_data              p,
                        tel_colour         edge_colour,
                        Tint            interior_style
                        ) 
{
#endif  /* G003 */
  CMN_KEY   k, k1, k2, k3, k4;
  Tint      nf, nc, newList = 0;
  Tint      edge_type, line_type_preserve;
  Tfloat    edge_width, line_width_preserve;
  GLboolean texture_on;
  tel_point pv;     
#ifdef G003
  if ( interior_style != TSM_HIDDENLINE && !forceDraw ) 
  {
#else     
  if ( interior_style != TSM_HIDDENLINE ) 
  {
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

  nc = p -> num_columns;
  nf = p -> num_rows * p->num_columns;

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
#ifdef G003
  if ( !forceDraw )
#endif  /* G003 */
    draw_line_strip ( p, edge_colour, nf, nc );
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

        draw_line_strip ( p, edge_colour, nf, nc );

      else if ( g_fSkipRatio != 1.0 ) {

        int i, m1, m2, n1, n2, n3, n4;

        set_drawable_items ( p -> d.bDraw, nf );

        glColor3fv ( edge_colour -> rgb );

        for ( i = 0; i < nf; ++i )

          if ( p -> d.bDraw[ i ] ) {

            glBegin ( GL_LINE_STRIP );
            m1 = i / nc;
            m2 = i % nc;
            n1 = m1 * ( nc + 1 ) + m2;
            n2 = n1 + nc + 1;
            n3 = n2 + 1;
            n4 = n1 + 1;

            glVertex3fv ( p -> vertices[ n1 ].xyz );
            glVertex3fv ( p -> vertices[ n2 ].xyz );
            glVertex3fv ( p -> vertices[ n3 ].xyz );
            glVertex3fv ( p -> vertices[ n4 ].xyz );
            glVertex3fv ( p -> vertices[ n1 ].xyz );
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

static void draw_line_strip ( tel_qstrip_data p, tel_colour c, Tint nf, Tint nc ) {

  int i, m1, m2, n1, n2, n3, n4;

  glColor3fv ( c -> rgb );

  for ( i = 0; i < nf; ++i ) {

    glBegin ( GL_LINE_STRIP );
    m1 = i / nc;
    m2 = i % nc;
    n1 = m1 * ( nc + 1 ) + m2;
    n2 = n1 + nc + 1;
    n3 = n2 + 1;
    n4 = n1 + 1;

    glVertex3fv ( p -> vertices[ n1 ].xyz );
    glVertex3fv ( p -> vertices[ n2 ].xyz );
    glVertex3fv ( p -> vertices[ n3 ].xyz );
    glVertex3fv ( p -> vertices[ n4 ].xyz );
    glVertex3fv ( p -> vertices[ n1 ].xyz );
    glEnd();

  }  /* end for */

}  /* end draw_line_strip */
/*----------------------------------------------------------------------*/

static TStatus
QuadrangleInquire( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  Tint             i, j;
  tel_qstrip_data  d;
  Tint             size_reqd=0;
  TStatus          status = TSuccess;
  Tchar            *cur_ptr = 0;
  Tint             nf, nv;

  d = (tel_qstrip_data)data.pdata;
  nf = d->num_rows * d->num_columns;
  nv = ( d->num_rows + 1 ) * ( d->num_columns + 1 );

  if( d->fcolours )
    size_reqd += ( nf * sizeof( TEL_COLOUR ) );

  if( d->facet_flag == TEL_FA_NORMAL )
    size_reqd += ( nf * sizeof( TEL_POINT ) );

  size_reqd += ( nv * sizeof( TEL_POINT ) );

  if( d->vcolours )
    size_reqd += ( nv * sizeof( TEL_COLOUR ) );

  if( d->vertex_flag == TEL_VT_NORMAL )
    size_reqd += ( nv * sizeof( TEL_POINT ) );

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
        w->quadmesh3data.shpflag = d->shape_flag;
        w->quadmesh3data.dim.x_dim = d->num_rows;
        w->quadmesh3data.dim.y_dim = d->num_columns;

        if( c->size >= size_reqd )
        {
          if( d->facet_flag == TEL_FA_NORMAL )
          {
            if( d->fcolours )
            {
              w->quadmesh3data.fctflag = TEL_FAFLAG_COLOURNORMAL;
              w->quadmesh3data.gnormals = (tel_point)(c->buf);
              for( j = 0; j < nf; j++ )
              {
                w->quadmesh3data.gnormals[j] = d->fnormals[j];
              }
              cur_ptr += ( nf * sizeof( TEL_POINT ) );

              w->quadmesh3data.facet_colour_vals =
                (tel_colour)(cur_ptr);
              for( j = 0; j < nf; j++ )
              {
                w->quadmesh3data.facet_colour_vals[j] =
                  d->fcolours[j];
              }
              cur_ptr += ( nf * sizeof( TEL_COLOUR ) );
            }
            else
            {
              w->quadmesh3data.fctflag = TEL_FAFLAG_NORMAL;
              w->quadmesh3data.facet_colour_vals = 0;
              w->quadmesh3data.gnormals = (tel_point)(c->buf);
              for( j = 0; j < nf; j++ )
              {
                w->quadmesh3data.gnormals[j] = d->fnormals[j];
              }
              cur_ptr += ( nf * sizeof( TEL_POINT ) );
            }
          }
          else
          {
            w->quadmesh3data.gnormals = 0;
            if( d->fcolours )
            {
              w->quadmesh3data.fctflag = TEL_FAFLAG_COLOUR;
              w->quadmesh3data.facet_colour_vals =
                (tel_colour)(c->buf );
              for( j = 0; j < nf; j++ )
              {
                w->quadmesh3data.facet_colour_vals[j] =
                  d->fcolours[j];
              }
              cur_ptr += ( nf * sizeof( TEL_COLOUR ) );
            }
            else
            {
              w->quadmesh3data.fctflag = TEL_FAFLAG_NONE;
              w->quadmesh3data.facet_colour_vals = 0;
            }
          }

          w->quadmesh3data.points = (tel_point)cur_ptr;
          for( j = 0; j < nv; j++ )
          {
            w->quadmesh3data.points[j] = d->vertices[j];
          }
          cur_ptr += ( nv * sizeof( TEL_POINT ) );

          if( d->vertex_flag == TEL_VT_NORMAL )
          {
            if( d->vcolours )
            {
              w->quadmesh3data.vrtflag = TEL_VTFLAG_COLOURNORMAL;
              w->quadmesh3data.vnormals = (tel_point)(cur_ptr);
              for( j = 0; j < nv; j++ )
              {
                w->quadmesh3data.vnormals[j] = d->vnormals[i];
              }
              cur_ptr += ( nv * sizeof( TEL_POINT ) );

              w->quadmesh3data.colours = (tel_colour)(cur_ptr);

              for( j = 0; j < nv; j++ )
              {
                w->quadmesh3data.colours[j] = d->vcolours[i];
              }
            }
            else
            {
              w->quadmesh3data.vrtflag = TEL_VTFLAG_NORMAL;
              w->quadmesh3data.colours = 0;
              w->quadmesh3data.vnormals = (tel_point)(cur_ptr);

              for( j = 0; j < nv; j++ )
              {
                w->quadmesh3data.vnormals[j] = d->vnormals[i];
              }
            }
          }
          else
          {
            w->quadmesh3data.vnormals = 0;
            if( d->vcolours )
            {
              w->quadmesh3data.vrtflag = TEL_VTFLAG_COLOUR;
              w->quadmesh3data.colours = (tel_colour)(cur_ptr);
              for( j = 0; j < nv; j++ )
              {
                w->quadmesh3data.colours[j] = d->vcolours[i];
              }
            }
            else
            {
              w->quadmesh3data.vrtflag = TEL_VTFLAG_NONE;
              w->quadmesh3data.colours = 0;
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
