/*
File OpenGl_PrimitiveArray.c

Created 16/06/2000 :  ATS : G005 : Modified version of OpenGl_indexpolygons.c, which use glDrawArrays or glDrawElements for rendering of primitive
21.06.03 : SAN : suppress display at all in animation mode if degenartion ratio == 1 (FULL_DEGENER)
03.07.03 : SAN : draw_degenerates_as_lines() function - all client states disabled except GL_VERTEX_ARRAY
in order to avoid exceptions in animation mode (OCC3192)
*/

#define xOGLBUG         /* UNFORTUNATLY the edge flags are attached to vertexes 
and not to the edges. So the edge visibillity flags array 
is not usable.
*/

#define xPRINT

#define TEST

#define FULL_DEGENER
#define OCC3192


#ifndef OGL_Max
#define OGL_Max(a,b) ((a) > (b) ? (a) : (b))
#endif

#ifndef OGL_Rand
static unsigned long vRand = 1L;
#define OGL_Rand() (vRand = vRand * 214013L + 2531011L)
#endif


#define OCC7833         /* ASL 26/01/05 transparency of polygon with colors assigned to vertices */

/*----------------------------------------------------------------------*/
/*
* Includes
*/ 

#include <OpenGl_tgl_all.hxx>

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <iostream>

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
#include <OpenGl_ResourceCleaner.hxx>
#include <OpenGl_ResourceVBO.hxx>
#include <InterfaceGraphic_PrimitiveArray.hxx>
#include <OpenGl_Memory.hxx>
#include <Standard.hxx>

# include <float.h>
# define DEF_DS_INTERNAL
# include <OpenGl_degeneration.hxx>

#include <OpenGl_Extension.hxx>

extern GLboolean g_fBitmap;

typedef CALL_DEF_PARRAY *call_def_parray;

#define GL_ARRAY_BUFFER_ARB           0x8892
#define GL_STATIC_DRAW_ARB            0x88E4
#define GL_ELEMENTS_ARRAY_BUFFER_ARB  0x8893
#define GL_EDGE_FLAG_ARRAY_EXT        0x8079

#ifdef WNT
typedef void (APIENTRY* PFNGLBINDBUFFERARBPROC)    (GLenum target, GLuint buffer);
typedef void (APIENTRY* PFNGLDELETEBUFFERSARBPROC) (GLsizei n, const GLuint *buffers);
typedef void (APIENTRY* PFNGLGENBUFFERSARBPROC)    (GLsizei n, GLuint *buffers);
typedef void (APIENTRY* PFNGLBUFFERDATAARBPROC)    (GLenum target, int size, const GLvoid *data, GLenum usage);
#else//WNT
typedef void (*PFNGLBINDBUFFERARBPROC)    (GLenum target, GLuint buffer);
typedef void (*PFNGLDELETEBUFFERSARBPROC) (GLsizei n, const GLuint *buffers);
typedef void (*PFNGLGENBUFFERSARBPROC)    (GLsizei n, GLuint *buffers);
#if defined ( __sgi ) 
typedef void (*PFNGLBUFFERDATAARBPROC)    (GLenum target, int size, const GLvoid *data, GLenum usage);
#else                                                              
typedef void (*PFNGLBUFFERDATAARBPROC)    (GLenum target, GLsizeiptrARB size, const GLvoid *data, GLenum usage);
#endif

#endif//WNT

PFNGLGENBUFFERSARBPROC    glVBOGenBuffersARB    = NULL; // VBO Name Generation Procedure
PFNGLBINDBUFFERARBPROC    glVBOBindBufferARB    = NULL; // VBO Bind Procedure
PFNGLBUFFERDATAARBPROC    glVBOBufferDataARB    = NULL; // VBO Data Loading Procedure
PFNGLDELETEBUFFERSARBPROC glVBODeleteBuffersARB = NULL; // VBO Data Deleting Procedure

int VBOExtension = 0;
int VBOenabled = 1;

#define VBO_NOT_INITIALIZED  -1
#define VBO_ERROR             0
#define VBO_OK                1

#ifndef WNT
#define glGetProcAddress( x )  glXGetProcAddress( (const GLubyte*) x )
#else
#define glGetProcAddress( x )  wglGetProcAddress( x )
#endif

void clearRAMMemory( CALL_DEF_PARRAY* p )
{
    if( p->bufferVBO[VBOEdges] ){
      Standard::Free((Standard_Address&)p->edges);
      p->edges = NULL;
    }
    if( p->bufferVBO[VBOVertices] ){
      Standard::Free((Standard_Address&)p->vertices);
      p->vertices = NULL;
    }
    if( p->bufferVBO[VBOVcolours] ){
      Standard::Free((Standard_Address&)p->vcolours);
      p->vcolours = NULL;
    }
    if( p->bufferVBO[VBOVnormals] ){
      Standard::Free((Standard_Address&)p->vnormals);
      p->vnormals = NULL;
    }
    if( p->bufferVBO[VBOVtexels] ){
      Standard::Free((Standard_Address&)p->vtexels);
      p->vtexels = NULL;
    }
    if( p->edge_vis ){
      Standard::Free((Standard_Address&)p->edge_vis);
      p->edge_vis = NULL;
    }
}

void clearGraphicRAMMemory( CALL_DEF_PARRAY* p )
{
  if( p->bufferVBO[VBOEdges] ){
    glVBODeleteBuffersARB( 1 , &p->bufferVBO[VBOEdges]);
  }
  if( p->bufferVBO[VBOVertices] ){
    glVBODeleteBuffersARB( 1 , &p->bufferVBO[VBOVertices]);
  }
  if( p->bufferVBO[VBOVcolours] ){
    glVBODeleteBuffersARB( 1 , &p->bufferVBO[VBOVcolours]);
  }
  if( p->bufferVBO[VBOVnormals] ){
    glVBODeleteBuffersARB( 1 , &p->bufferVBO[VBOVnormals]);
  } 
  if( p->bufferVBO[VBOVtexels] ){
    glVBODeleteBuffersARB( 1 , &p->bufferVBO[VBOVtexels]);
  }
  glVBOBindBufferARB(GL_ARRAY_BUFFER_ARB, 0); 
  glVBOBindBufferARB(GL_ELEMENTS_ARRAY_BUFFER_ARB, 0); 
}

int checkSizeForGraphicMemory( CALL_DEF_PARRAY* p )
{
  if( GL_OUT_OF_MEMORY == glGetError() ){
    p->flagBufferVBO = VBO_ERROR;
    clearGraphicRAMMemory(p);
  }
  else 
    p->flagBufferVBO = VBO_OK;
  return p->flagBufferVBO ;
}

int initVBO()
{
  if(CheckExtension((char *)"GL_ARB_vertex_buffer_object",(const char *)glGetString( GL_EXTENSIONS ))){
    glVBOGenBuffersARB = (PFNGLGENBUFFERSARBPROC) glGetProcAddress("glGenBuffersARB");
    glVBOBindBufferARB = (PFNGLBINDBUFFERARBPROC) glGetProcAddress("glBindBufferARB");
    glVBOBufferDataARB = (PFNGLBUFFERDATAARBPROC) glGetProcAddress("glBufferDataARB");
    glVBODeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC) glGetProcAddress("glDeleteBuffersARB");
    VBOExtension = 1;
    return 1;
  }
  return 0;
}

static void  BuildVBO( CALL_DEF_PARRAY* p )
{
  int size_reqd = 0;
  if( p->edges ){
    size_reqd = ( p->num_edges * sizeof( Tint ) );
    glVBOGenBuffersARB( 1, &p->bufferVBO[VBOEdges] );   
    glVBOBindBufferARB( GL_ELEMENTS_ARRAY_BUFFER_ARB, p->bufferVBO[VBOEdges] );
    glVBOBufferDataARB( GL_ELEMENTS_ARRAY_BUFFER_ARB, size_reqd , p->edges, GL_STATIC_DRAW_ARB );
    if( checkSizeForGraphicMemory( p ) == VBO_ERROR ) 
      return;
  }

  if( p->vertices ){
    size_reqd = ( p->num_vertexs * sizeof( TEL_POINT ) );
    glVBOGenBuffersARB( 1, &p->bufferVBO[VBOVertices]); 
    glVBOBindBufferARB( GL_ARRAY_BUFFER_ARB, p->bufferVBO[VBOVertices] );
    glVBOBufferDataARB( GL_ARRAY_BUFFER_ARB, size_reqd , p->vertices, GL_STATIC_DRAW_ARB );
    if( checkSizeForGraphicMemory( p ) == VBO_ERROR ) 
      return;
  }

  if( p->vcolours ){
    size_reqd = ( p->num_vertexs * sizeof( Tint ) );
    glVBOGenBuffersARB( 1, &p->bufferVBO[VBOVcolours] );
    glVBOBindBufferARB( GL_ARRAY_BUFFER_ARB, p->bufferVBO[VBOVcolours]); 
    glVBOBufferDataARB( GL_ARRAY_BUFFER_ARB, size_reqd , p->vcolours, GL_STATIC_DRAW_ARB );
    if( checkSizeForGraphicMemory( p ) == VBO_ERROR ) 
      return;
  }

  if( p->vnormals ){
    size_reqd = ( p->num_vertexs * sizeof( TEL_POINT ) );
    glVBOGenBuffersARB( 1, &p->bufferVBO[VBOVnormals] ); 
    glVBOBindBufferARB( GL_ARRAY_BUFFER_ARB, p->bufferVBO[VBOVnormals] );
    glVBOBufferDataARB( GL_ARRAY_BUFFER_ARB, size_reqd , p->vnormals, GL_STATIC_DRAW_ARB );
    if( checkSizeForGraphicMemory( p ) == VBO_ERROR) 
      return;
  }

  if( p->vtexels ){
    size_reqd = ( p->num_vertexs * sizeof( TEL_TEXTURE_COORD ) );
    glVBOGenBuffersARB( 1, &p->bufferVBO[VBOVtexels] ); 
    glVBOBindBufferARB( GL_ARRAY_BUFFER_ARB, p->bufferVBO[VBOVtexels] ); 
    glVBOBufferDataARB( GL_ARRAY_BUFFER_ARB, size_reqd , p->vtexels , GL_STATIC_DRAW_ARB );
    if( checkSizeForGraphicMemory( p ) == VBO_ERROR ) 
      return;
  } 

  if( p->flagBufferVBO == VBO_OK )
    clearRAMMemory(p);

  //specify context for VBO resource
  p->contextId = (Standard_Address)GET_GL_CONTEXT();

}

/*----------------------------------------------------------------------*/
/*
* Constantes
*/ 

/*----------------------------------------------------------------------*/
/*
* Prototypes
*/ 

static  TStatus  ParrayDisplay( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  ParrayAdd( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  ParrayDelete( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  ParrayPrint( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  ParrayInquire( TSM_ELEM_DATA, Tint, cmn_key* );

/*static  GLboolean       lighting_mode;*/

static void draw_array(
                       call_def_parray,
                       Tint,          /* highlight_flag */
                       Tint,          /* front_lighting_model,  */
                       Tint,          /* interior_style,  */
                       Tint,          /* edge_flag,  */
                       tel_colour,    /* interior_colour, */
                       tel_colour,    /* line_colour, */
                       tel_colour     /* edge_colour, */
#ifdef OCC7833
                       ,tel_surf_prop
#endif
                       );

static void draw_edges                 ( call_def_parray, tel_colour );
static void draw_degenerates_as_points ( call_def_parray, tel_colour );
static void draw_degenerates_as_lines  ( call_def_parray, tel_colour );
static void draw_degenerates_as_bboxs  ( call_def_parray, tel_colour );

static  TStatus  (*MtdTbl[])( TSM_ELEM_DATA, Tint, cmn_key* ) =
{
  NULL,             /* PickTraverse */
  ParrayDisplay,
  ParrayAdd,
  ParrayDelete,
  0,
  0
};
static GLenum draw_mode;

/*----------------------------------------------------------------------*/
/*
* Variables externes
*/

extern  Tint  ForbidSetTextureMapping; /* currently defined in tsm/tsm.c */

extern int   g_nDegenerateModel;
extern float g_fSkipRatio;

/*----------------------------------------------------------------------*/

MtblPtr
TelParrayInitClass( TelType* el )
{
  *el = TelParray;
  return MtdTbl;
}

/*----------------------------------------------------------------------*/

static  TStatus
ParrayAdd( TSM_ELEM_DATA d, Tint n, cmn_key *k )
{

  if( k[0]->id != PARRAY_ID ) return TFailure;

  ((tsm_elem_data)(d.pdata))->pdata = k[0]->data.pdata;

  return TSuccess;
}

/*----------------------------------------------------------------------*/

static  TStatus
ParrayDisplay( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  CMN_KEY       k11, k12, k17, k18, k111, k114, k115;

  Tint           front_lighting_model;
  Tint           interior_style;
  TEL_COLOUR     interior_colour;
  TEL_COLOUR     edge_colour;
  TEL_COLOUR     line_colour;
  Tint           edge_flag;

#ifdef OCC7833
  CMN_KEY        k117;
  TEL_SURF_PROP  prop;
#endif

  call_def_parray d = (call_def_parray)data.pdata;

  k12.id          = TelInteriorReflectanceEquation;
  k17.id          = TelInteriorStyle;
  k18.id          = TelEdgeFlag;
  k111.id         = TelInteriorColour;
  k111.data.pdata = &interior_colour;
  k114.id         = TelEdgeColour;
  k114.data.pdata = &edge_colour;
  k115.id         = TelPolylineColour;
  k115.data.pdata = &line_colour;

#ifdef OCC7833
  k117.id         = TelSurfaceAreaProperties;
  k117.data.pdata = &prop;
#endif

#ifdef OCC7833
  TsmGetAttri( 7, &k12, &k17, &k18, &k111, &k114, &k115, &k117 );
#else
  TsmGetAttri( 6, &k12, &k17, &k18, &k111, &k114, &k115 );
#endif

  front_lighting_model     = k12.data.ldata;
  interior_style           = k17.data.ldata;
  edge_flag                = k18.data.ldata;

#ifdef PRINT
  printf("ParrayDisplay type %d\n",d->type); 
#endif

  /* 
  * Use highlight colours 
  */

  if( k[0]->id == TOn ) {
    TEL_HIGHLIGHT  hrep;

    k11.id = TelHighlightIndex;
    TsmGetAttri( 1, &k11 );
    if( TelGetHighlightRep( TglActiveWs, k11.data.ldata, &hrep ) == TSuccess ) {
      if( hrep.type == TelHLForcedColour ) {
        edge_colour = interior_colour = line_colour = hrep.col;
        front_lighting_model = CALL_PHIGS_REFL_NONE;
      } else if( hrep.type == TelHLColour ) {
        edge_colour = hrep.col;
        k[0]->id = TOff;
      }
    } else {
      TelGetHighlightRep( TglActiveWs, 0, &hrep );
      if( hrep.type == TelHLForcedColour ) {
        edge_colour = interior_colour = line_colour = hrep.col;
        front_lighting_model = CALL_PHIGS_REFL_NONE;
      } else if( hrep.type == TelHLColour ) {
        edge_colour = hrep.col;
        k[0]->id = TOff;
      }
    }
  }

  draw_array( d, k[0]->id,
    front_lighting_model,
    interior_style,
    edge_flag,     
    &interior_colour,
    &line_colour,
    &edge_colour
#ifdef OCC7833
    ,&prop
#endif
    );

  return TSuccess;

}

/*----------------------------------------------------------------------*/

static void 
draw_primitive_array( call_def_parray p, GLenum mode, GLint first, GLsizei count )
{
  int i;
  glBegin( mode );
  for( i=first; i<(first + count); i++ ){
    if( p->vnormals )
      glNormal3fv( p->vnormals[i].xyz );
    if( p->vtexels && !ForbidSetTextureMapping )
      glTexCoord3fv( p->vtexels[i].xy );
    if( p->vertices )
      glVertex3fv( p->vertices[i].xyz );
    if ( p->vcolours )
      glColor4ubv( (GLubyte*) p->vcolours[i] );
  }
  glEnd();
}

/*----------------------------------------------------------------------*/
static void 
draw_primitive_elements( call_def_parray p, GLenum mode, GLsizei count, GLenum type, GLenum *indices )
{
  int i;
  GLenum index;
  glBegin( mode );
  for( i=0; i<count; i++ ){
    index = indices[i];
    if( p->vnormals )
      glNormal3fv( p->vnormals[index].xyz );
    if( p->vtexels && !ForbidSetTextureMapping )
      glTexCoord3fv( p->vtexels[index].xy );
    if( p->vertices ) 
      glVertex3fv( p->vertices[index].xyz );
     if ( p->vcolours ) 
      glColor4ubv( (GLubyte*) p->vcolours[index] );
  }
  glEnd();
}
/*----------------------------------------------------------------------*/
static void
draw_array( call_def_parray p, Tint hflag,
             Tint lighting_model,
             Tint interior_style,
             Tint edge_flag,
             tel_colour interior_colour,
             tel_colour line_colour,
             tel_colour edge_colour
#ifdef OCC7833
             ,tel_surf_prop prop
#endif
             ){
  Tint i,n;
  Tint transp = 0;
  GLint renderMode;
  /* Following pointers have been provided for performance improvement */
  tel_colour pfc;
  Tint * pvc;

  pvc = p->vcolours;
  pfc = p->fcolours;

#ifdef OCC7833
  if( pvc ){
    for( i=0; i<p->num_vertexs; i++ ){
      transp = (int)(prop->trans * 255.);
#if defined (sparc) || defined (__sparc__) || defined (__sparc) 
      pvc[i] = ( pvc[i] & 0xffffff00 );
      pvc[i] += transp ;
	                
#else     
      pvc[i] = ( pvc[i] & 0x00ffffff );
      pvc[i] += transp << 24;
#endif
    }
  }
#endif

  switch( p->type ) {
    case TelPointsArrayType:
      draw_mode = GL_POINTS; 
      glColor3fv( line_colour->rgb );
      break;
    case TelPolylinesArrayType:
      draw_mode = GL_LINE_STRIP; 
      glColor3fv( line_colour->rgb );
      break;
    case TelSegmentsArrayType:
      draw_mode = GL_LINES;
      glColor3fv( line_colour->rgb );
      break;
    case TelPolygonsArrayType:
      draw_mode = GL_POLYGON;
      glColor3fv( interior_colour->rgb );
      break;
    case TelTrianglesArrayType:
      draw_mode = GL_TRIANGLES;
      glColor3fv( interior_colour->rgb );
      break;
    case TelQuadranglesArrayType:
      draw_mode = GL_QUADS;
      glColor3fv( interior_colour->rgb );
      break;
    case TelTriangleStripsArrayType:
      draw_mode = GL_TRIANGLE_STRIP;
      glColor3fv( interior_colour->rgb );
      break;
    case TelQuadrangleStripsArrayType:
      draw_mode = GL_QUAD_STRIP;
      glColor3fv( interior_colour->rgb );
      break;
    case TelTriangleFansArrayType:
      draw_mode = GL_TRIANGLE_FAN;
      glColor3fv( interior_colour->rgb );
      break;
    default:
      return;
  }

   /* OCC11904 -- Temporarily disable environment mapping */
  if( draw_mode <= GL_LINE_STRIP ){
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_TEXTURE_1D);
    glDisable(GL_TEXTURE_2D);
  }

  if( p->VBOEnabled == -1 ){
      p->VBOEnabled = VBOenabled;
      if( VBOExtension == 0)
        initVBO();
      if( (VBOExtension == 1) && (p->VBOEnabled != 0))
        BuildVBO( p );
  }


  #ifdef PRINT
  printf(" $$$ g_nDegenerateModel %d\n",g_nDegenerateModel);
  #endif
  if ( g_nDegenerateModel < 2 && 
     ( (draw_mode > GL_LINE_STRIP && interior_style != TSM_EMPTY) ||
       (draw_mode <= GL_LINE_STRIP /*&& !hflag*/)) ) {

    if( hflag == TOn ) {
      pfc = NULL;
      pvc = NULL;
    }

    if ( interior_style == TSM_HIDDENLINE) {
      edge_flag = 1;
      pfc = NULL;
      pvc = NULL;
    }

    /*OCC12297 - Sometimes the GL_LIGHTING mode is activated here
    without LightOn() call for an unknown reason, so it is necessary 
    to call LightOn() to synchronize LightOn/Off mechanism*/
    LightOn();

    if( lighting_model == CALL_PHIGS_REFL_NONE || draw_mode <= GL_LINE_STRIP )
        LightOff();

    glGetIntegerv( GL_RENDER_MODE, &renderMode );

    if ( p->num_vertexs > 0 && p->flagBufferVBO != VBO_OK) {
      if( renderMode != GL_FEEDBACK ){
          if( p->vertices ) {
            glVertexPointer(3, GL_FLOAT, 0, p->vertices);/* array of vertices  */
            glEnableClientState(GL_VERTEX_ARRAY);
          }

          if( p->vnormals ) {
            glNormalPointer(GL_FLOAT, 0, p->vnormals);/* array of normals  */
            glEnableClientState(GL_NORMAL_ARRAY);
          }

          if ( p->vtexels) {
            glTexCoordPointer(2, GL_FLOAT, 0, p->vtexels);/* array of texture coordinates  */
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
          }

          if ( pvc ) {
#ifdef OCC7833
            glColorPointer(4, GL_UNSIGNED_BYTE, 0, pvc);  /* array of colors */
#else
            glColorPointer(3, GL_UNSIGNED_BYTE, 0, pvc);  /* array of colors */
#endif
            glEnableClientState(GL_COLOR_ARRAY);
            glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
            glEnable(GL_COLOR_MATERIAL);
          }
      }
    }

    //Bindings concrete pointer in accordance with VBO buffer
    //Using VBO
    if ( p->num_vertexs > 0 && p->flagBufferVBO == VBO_OK ) {
      if( p->bufferVBO[VBOVertices] ){
        glVBOBindBufferARB( GL_ARRAY_BUFFER_ARB, p->bufferVBO[VBOVertices]);
        glVertexPointer(3, GL_FLOAT, 0, (char *) NULL);// array of vertices 
        glEnableClientState(GL_VERTEX_ARRAY);
      }

      if( p->bufferVBO[VBOVnormals] ){
        glVBOBindBufferARB( GL_ARRAY_BUFFER_ARB,  p->bufferVBO[VBOVnormals]);
        glNormalPointer(GL_FLOAT, 0, (char *) NULL);// array of normals  
        glEnableClientState(GL_NORMAL_ARRAY);
      }

      if ( p->bufferVBO[VBOVtexels] && !ForbidSetTextureMapping ) {
        glVBOBindBufferARB( GL_ARRAY_BUFFER_ARB,  p->bufferVBO[VBOVtexels]);
        glTexCoordPointer(2, GL_FLOAT, 0, (char *) NULL);/* array of texture coordinates */
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      }

      if ( p->bufferVBO[VBOVcolours] ) {
        glVBOBindBufferARB( GL_ARRAY_BUFFER_ARB,  p->bufferVBO[VBOVcolours]);
        glColorPointer( 4, GL_UNSIGNED_BYTE, 0, (char *) NULL);// array of colors 
        glEnableClientState(GL_COLOR_ARRAY);
        glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
        glEnable(GL_COLOR_MATERIAL);
      }
    }
    // OCC22236 NOTE: draw for all situations:
    // 1) draw elements from p->bufferVBO[VBOEdges] indicies array
    // 2) draw elements from vertice array, when bounds defines count of primitive's verts.
    // 3) draw primitive by vertexes if no edges and bounds array is specified
    if(  p->flagBufferVBO == VBO_OK ){
      if ( p->num_edges > 0 && p->bufferVBO[VBOEdges] ) {
        glVBOBindBufferARB(GL_ELEMENTS_ARRAY_BUFFER_ARB, p->bufferVBO[VBOEdges]); // for edge indices
        
        // draw primitives by vertex count with the indicies
        if( p->num_bounds > 0 ) {
          Tint* offset = 0;
          for( i = 0, offset = 0 ; i < p->num_bounds ; i++ ) {
            glDrawElements(draw_mode, p->bounds[i], GL_UNSIGNED_INT, offset);
            offset += p->bounds[i]; 
          }
        }
        // draw one (or sequential) primitive by the indicies
        else {
          glDrawElements(draw_mode, p->num_edges , GL_UNSIGNED_INT, 0);
        }
      }
      else if( p->num_bounds > 0 ) {
        for( i = n = 0 ; i < p->num_bounds ; i ++ ){
          glDrawArrays(draw_mode, n, p->bounds[i]);
          n += p->bounds[i];
        }
      }
      else {
        glDrawArrays(draw_mode, 0, p->num_vertexs);
      }

      //bind with 0
      glVBOBindBufferARB(GL_ARRAY_BUFFER_ARB, 0); 
      glVBOBindBufferARB(GL_ELEMENTS_ARRAY_BUFFER_ARB, 0); 
    } 
    else {
      if( p->num_bounds > 0 ) {
        if( p->num_edges > 0 ) {
            for( i=n=0 ; i<p->num_bounds ; i++ ) {
              if( pfc ) glColor3fv  ( pfc[i].rgb  );
              if( renderMode == GL_FEEDBACK )
                draw_primitive_elements( p, draw_mode, p->bounds[i], 
                                        GL_UNSIGNED_INT,(GLenum*) &p->edges[n]);
              else
                glDrawElements( draw_mode, p->bounds[i], 
                                GL_UNSIGNED_INT, &p->edges[n]);
              n += p->bounds[i];
            }
        } else {
          for( i=n=0 ; i<p->num_bounds ; i++ ) {
                if( pfc ) 
                  glColor3fv  ( pfc[i].rgb  );
                if( renderMode == GL_FEEDBACK )
                  draw_primitive_array( p, draw_mode, n, p->bounds[i]);
                else
                {
                  glDrawArrays( draw_mode, n, p->bounds[i]);
                }
                n += p->bounds[i];
              }
            }
      } else if( p->num_edges > 0 ) {
          if( renderMode == GL_FEEDBACK )
            draw_primitive_elements( p, draw_mode, p->num_edges, 
                                    GL_UNSIGNED_INT,(GLenum*) &p->edges[0]);
          else
         glDrawElements( draw_mode, p->num_edges, GL_UNSIGNED_INT, p->edges);
      } else {
            if( renderMode == GL_FEEDBACK )
            draw_primitive_array( p, draw_mode, 0, p->num_vertexs);
          else
            glDrawArrays( draw_mode, 0, p->num_vertexs);
        }
    }

#ifdef TEST
    if( p->bufferVBO[VBOVcolours] || pvc ) {
      glDisable(GL_COLOR_MATERIAL);
      TelResetMaterial();
    }
#endif

    if( p->bufferVBO[VBOVertices] || p->vertices )
      glDisableClientState(GL_VERTEX_ARRAY);
    if( p->bufferVBO[VBOVcolours] || p->vcolours )
      glDisableClientState(GL_COLOR_ARRAY);
    if( p->bufferVBO[VBOVnormals] || p->vnormals )
      glDisableClientState(GL_NORMAL_ARRAY);
    if ( (p->bufferVBO[VBOVtexels] && !ForbidSetTextureMapping ) || p->vtexels )
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    if ( g_nDegenerateModel ){
      if(draw_mode <= GL_LINE_STRIP)
        glPopAttrib();
      return;
    }
  }

  if( edge_flag || g_nDegenerateModel ) 
    switch ( g_nDegenerateModel ) {
      default: /* XXX_TDM_NODE or TINY */
          /*
            On some NVIDIA graphic cards, using glEdgeFlagPointer() in 
            combination with VBO ( edge flag data put into a VBO buffer)
            leads to a crash in a driver. Therefore, edge flags are simply 
            igonored when VBOs are enabled, so all the edges are drawn if 
            edge visibility is turned on. In order to draw edges selectively, 
            either disable VBO or turn off edge visibilty in the current 
            primitive array and create a separate primitive array (segments) 
            and put edges to be drawn into it.
          */
          draw_edges ( p, edge_flag ? edge_colour : interior_colour );
        break;
        // DegenerateModel(as Lines, Points, BBoxs) are used only without VBO
          case 2:  /* XXX_TDM_WIREFRAME */
            if( p->VBOEnabled == 0 )
              draw_degenerates_as_lines ( p, edge_flag ? edge_colour : interior_colour );
            break;
          case 3:  /* XXX_TDM_MARKER */
            if( p->VBOEnabled == 0  )
              draw_degenerates_as_points ( p, edge_flag ? edge_colour : interior_colour );
            break;
          case 4:  /* XXX_TDM_BBOX */
            if( p->VBOEnabled == 0  )
              draw_degenerates_as_bboxs ( p, edge_flag ? edge_colour : interior_colour  );
            break;
   }

  if(draw_mode <= GL_LINE_STRIP)
    glPopAttrib();
}

/*----------------------------------------------------------------------*/

static  TStatus
ParrayDelete( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  call_def_parray p = (call_def_parray)data.pdata;
  if( p->VBOEnabled == VBO_OK ) {
    OpenGl_ResourceCleaner* resCleaner = OpenGl_ResourceCleaner::GetInstance();
    if( p->bufferVBO[VBOEdges] )
      resCleaner->AddResource( (GLCONTEXT)p->contextId, new OpenGl_ResourceVBO(p->bufferVBO[VBOEdges]) ); 
    if( p->bufferVBO[VBOVertices] )
      resCleaner->AddResource( (GLCONTEXT)p->contextId, new OpenGl_ResourceVBO(p->bufferVBO[VBOVertices]) ); 
    if( p->bufferVBO[VBOVcolours] )
	  resCleaner->AddResource( (GLCONTEXT)p->contextId, new OpenGl_ResourceVBO(p->bufferVBO[VBOVcolours]) ); 
    if( p->bufferVBO[VBOVnormals] )
      resCleaner->AddResource( (GLCONTEXT)p->contextId, new OpenGl_ResourceVBO(p->bufferVBO[VBOVnormals]) ); 
    if( p->bufferVBO[VBOVtexels] )
      resCleaner->AddResource( (GLCONTEXT)p->contextId, new OpenGl_ResourceVBO(p->bufferVBO[VBOVtexels]) ); 
  }

  return TSuccess;
}

/*----------------------------------------------------------------------*/

static void
draw_edges ( call_def_parray p, tel_colour edge_colour )
{
  Tint    i, j, n;
  Tint    edge_type=0, line_type_preserve=0;
  Tfloat  edge_width=0, line_width_preserve=0; 
  /* GLboolean texture_on;*/

  GLint renderMode;
  
  LightOff();
 
  if( draw_mode > GL_LINE_STRIP ) { 
    CMN_KEY k, k1, k2, k3, k4;

    k1.id  = TelPolylineWidth;
    k2.id  = TelPolylineType;
    k3.id  = TelEdgeType;
    k4.id  = TelEdgeWidth;
    
    TsmGetAttri( 4, &k1, &k2, &k3, &k4 );
    
    line_width_preserve = k1.data.fdata;
    line_type_preserve  = k2.data.ldata;
    edge_type           = k3.data.ldata;
    edge_width          = k4.data.fdata;

    if( line_width_preserve != edge_width ) {
      k.id = TelPolylineWidth;
      k.data.fdata = edge_width;
      TsmSetAttri( 1, &k );
    }
    if( line_type_preserve != edge_type ) {
      k.id = TelPolylineType;
      k.data.ldata = edge_type;
      TsmSetAttri( 1, &k );
    }

    glPushAttrib( GL_POLYGON_BIT );
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE ); 
  }

  // OCC22236 NOTE: draw edges for all situations:
  // 1) draw elements with GL_LINE style as edges from p->bufferVBO[VBOEdges] indicies array
  // 2) draw elements from vertice array, when bounds defines count of primitive's verts.
  // 3) draw primitive's edges by vertexes if no edges and bounds array is specified
  if(p->flagBufferVBO == VBO_OK)
  { 
    glVBOBindBufferARB( GL_ARRAY_BUFFER_ARB, p->bufferVBO[VBOVertices] );
    glEnableClientState( GL_VERTEX_ARRAY );
    glColor3fv( edge_colour->rgb );
    if ( p->num_edges > 0 && p->bufferVBO[VBOEdges] ) {
      glVBOBindBufferARB(GL_ELEMENTS_ARRAY_BUFFER_ARB, p->bufferVBO[VBOEdges]); 
      
      // draw primitives by vertex count with the indicies
      if( p->num_bounds > 0 ) {
        Tint* offset = 0;
        for( i = 0, offset = 0 ; i < p->num_bounds ; i++ ) {
          glDrawElements( draw_mode, p->bounds[i], GL_UNSIGNED_INT, offset);
          offset += p->bounds[i];
        }
      }
      // draw one (or sequential) primitive by the indicies
      else {
        glDrawElements( draw_mode, p->num_edges , GL_UNSIGNED_INT, 0);
      }
    }
    else if( p->num_bounds > 0 ) {
      for( i = n = 0 ; i < p->num_bounds ; i ++ ){
        glDrawArrays( draw_mode, n, p->bounds[i]);
        n += p->bounds[i];
      }
    }
    else {
      glDrawArrays( draw_mode, 0, p->num_vertexs);
    }

    // unbind buffers
    glDisableClientState(GL_VERTEX_ARRAY);
    glVBOBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
    glVBOBindBufferARB(GL_ELEMENTS_ARRAY_BUFFER_ARB, 0); 
  }
  else {

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, p->vertices); /* array of vertices  */
#ifdef OGLBUG
      if( p->edge_vis ) {
        glEnableClientState(GL_EDGE_FLAG_ARRAY);
        glEdgeFlagPointer( sizeof(Tchar), p->edge_vis);
      } else
        glDisableClientState(GL_EDGE_FLAG_ARRAY);
#endif

   glGetIntegerv( GL_RENDER_MODE, &renderMode );

      glColor3fv( edge_colour->rgb );
      if( p->num_bounds > 0 ) {
        if( p->num_edges > 0 ) {
          for( i=n=0 ; i<p->num_bounds ; i++ ) {
#ifndef OGLBUG
            if( p->edge_vis ) {
              glBegin( draw_mode );
              for( j=0 ; j<p->bounds[i] ; j++ ) {
                glEdgeFlag( p->edge_vis[n+j] );
                glVertex3fv( &p->vertices[p->edges[n+j]].xyz[0] );
              }
              glEnd();
            } else
#endif
        if( renderMode == GL_FEEDBACK )
          draw_primitive_elements( p, draw_mode, p->bounds[i], 
                                  GL_UNSIGNED_INT, (GLenum*)&p->edges[n]);
        else
          glDrawElements( draw_mode, p->bounds[i], 
                                  GL_UNSIGNED_INT, &p->edges[n]);
            n += p->bounds[i];
          }
        } else {
          for( i=n=0 ; i<p->num_bounds ; i++ ) {
        if( renderMode == GL_FEEDBACK )
          draw_primitive_array( p, draw_mode, n, p->bounds[i]);
        else
          glDrawArrays( draw_mode, n, p->bounds[i]);
            n += p->bounds[i];
          }
        }
      } 
      else if( p->num_edges > 0 ) {
#ifndef OGLBUG
        if( p->edge_vis ) {
          glBegin( draw_mode );
          for( i=0 ; i<p->num_edges ; i++ ) {
            glEdgeFlag( p->edge_vis[i] );
            glVertex3fv( &p->vertices[p->edges[i]].xyz[0] );
          }
          glEnd();
        } else
#endif
      if( renderMode == GL_FEEDBACK )
        draw_primitive_elements( p, draw_mode, p->num_edges, 
                                  GL_UNSIGNED_INT,(GLenum*) p->edges);
      else
        glDrawElements( draw_mode, p->num_edges, 
                                  GL_UNSIGNED_INT, p->edges);
      } else {
      if( renderMode == GL_FEEDBACK )
        draw_primitive_array( p, draw_mode, 0, p->num_vertexs);
      else
        glDrawArrays( draw_mode, 0, p->num_vertexs);
      }
    }

  if( draw_mode > GL_LINE_STRIP ) {
    CMN_KEY k;
    if( line_width_preserve != edge_width ) {
      k.id = TelPolylineWidth;
      k.data.fdata = line_width_preserve;
      TsmSetAttri( 1, &k );
    }
    if( line_type_preserve != edge_type ) {
      k.id = TelPolylineType;
      k.data.ldata = line_type_preserve;
      TsmSetAttri( 1, &k );
    }
    glPopAttrib();
  }
}

/*----------------------------------------------------------------------*/
static void draw_degenerates_points_as_points ( call_def_parray p ) { 

  Tint       j;
  tel_point  pv = p -> vertices;

#ifdef FULL_DEGENER
  if ( g_fSkipRatio == 1. )
    return;
#endif

  for ( j=0 ; j<p->num_vertexs ; j++ ) {
    glVertex3fv ( &pv[j].xyz[0] );
  }
}  

/*----------------------------------------------------------------------*/
static void draw_degenerates_lines_as_points ( call_def_parray p ) { 

  Tint       j;
  GLfloat    pt[ 3 ];
  tel_point  pv = p -> vertices;

#ifdef FULL_DEGENER
  if ( g_fSkipRatio == 1. )
    return;
#endif

  for ( j=0 ; j<p->num_vertexs ; j+=2 ) {
    pt[0] = pt[1] = pt[2] = 0.;
    pt[ 0 ] += pv[j].xyz[ 0 ];
    pt[ 1 ] += pv[j].xyz[ 1 ];
    pt[ 2 ] += pv[j].xyz[ 2 ];
    pt[ 0 ] += pv[j+1].xyz[ 0 ];
    pt[ 1 ] += pv[j+1].xyz[ 1 ];
    pt[ 2 ] += pv[j+1].xyz[ 2 ];
    pt[ 0 ] /= 2;
    pt[ 1 ] /= 2;
    pt[ 2 ] /= 2;
    glVertex3fv ( pt );
  }
}  

/*----------------------------------------------------------------------*/
static void draw_degenerates_triangles_as_points ( call_def_parray p ) { 

  Tint       i, j, iv;
  GLfloat    pt[ 3 ];
  tel_point  pv = p -> vertices;

#ifdef FULL_DEGENER
  if ( g_fSkipRatio == 1. )
    return;
#endif

  if( p->num_edges > 0 ) {
    for ( j=0 ; j<p->num_edges ; j+=3 ) {
      pt[0] = pt[1] = pt[2] = 0.;
      for ( i=0 ; i<3 ; i++ ) {
        iv = p->edges[j+i];
        pt[ 0 ] += pv[iv].xyz[ 0 ];
        pt[ 1 ] += pv[iv].xyz[ 1 ];
        pt[ 2 ] += pv[iv].xyz[ 2 ];
      }
      pt[ 0 ] /= 3;
      pt[ 1 ] /= 3;
      pt[ 2 ] /= 3;
      glVertex3fv ( pt );
    }
  } else {
    for ( j=0 ; j<p->num_vertexs ; j+=3 ) {
      pt[0] = pt[1] = pt[2] = 0.;
      for ( i=0 ; i<3 ; i++ ) {
        pt[ 0 ] += pv[j+i].xyz[ 0 ];
        pt[ 1 ] += pv[j+i].xyz[ 1 ];
        pt[ 2 ] += pv[j+i].xyz[ 2 ];
      }
      pt[ 0 ] /= 3;
      pt[ 1 ] /= 3;
      pt[ 2 ] /= 3;
      glVertex3fv ( pt );
    }
  }
}  

/*----------------------------------------------------------------------*/
static void draw_degenerates_trianglestrips_as_points ( call_def_parray p ) { 

  Tint       i, j, k, n;
  GLfloat    pt[ 3 ];
  tel_point  pv = p -> vertices;

#ifdef FULL_DEGENER
  if ( g_fSkipRatio == 1. )
    return;
#endif

  if( p->num_bounds > 0 ) {
    for ( k=n=0 ; k<p->num_bounds ; k++ ) {
      for ( j=0 ; j<p->bounds[k]-2 ; j++ ) {
        pt[0] = pt[1] = pt[2] = 0.;
        for ( i=0 ; i<3 ; i++ ) {
          pt[ 0 ] += pv[n+j+i].xyz[ 0 ];
          pt[ 1 ] += pv[n+j+i].xyz[ 1 ];
          pt[ 2 ] += pv[n+j+i].xyz[ 2 ];
        }
        pt[ 0 ] /= 3;
        pt[ 1 ] /= 3;
        pt[ 2 ] /= 3;
        glVertex3fv ( pt );
      }
      n += p->bounds[k];
    }
  } else {
    for ( j=0 ; j<p->num_vertexs-2 ; j++ ) {
      pt[0] = pt[1] = pt[2] = 0.;
      for ( i=0 ; i<3 ; i++ ) {
        pt[ 0 ] += pv[j+i].xyz[ 0 ];
        pt[ 1 ] += pv[j+i].xyz[ 1 ];
        pt[ 2 ] += pv[j+i].xyz[ 2 ];
      }
      pt[ 0 ] /= 3;
      pt[ 1 ] /= 3;
      pt[ 2 ] /= 3;
      glVertex3fv ( pt );
    }
  }
}  

/*----------------------------------------------------------------------*/
static void draw_degenerates_polygons_as_points ( call_def_parray p ) { 

  Tint       j, k, n, iv;
  GLfloat    pt[ 3 ];
  tel_point  pv = p -> vertices;

#ifdef FULL_DEGENER
  if ( g_fSkipRatio == 1. )
    return;
#endif

  if( p->num_bounds > 0 ) {
    if( p->num_edges > 0 ) {
      for ( k=n=0 ; k<p->num_bounds ; k++ ) {
        pt[0] = pt[1] = pt[2] = 0.;
        for ( j=0 ; j<p->bounds[k] ; j++ ) {
          iv = p->edges[n+j];
          pt[ 0 ] += pv[iv].xyz[ 0 ];
          pt[ 1 ] += pv[iv].xyz[ 1 ];
          pt[ 2 ] += pv[iv].xyz[ 2 ];
        }
        pt[ 0 ] /= p->bounds[k];
        pt[ 1 ] /= p->bounds[k];
        pt[ 2 ] /= p->bounds[k];
        glVertex3fv ( pt );
        n += p->bounds[k];
      }
    } else {  
      for ( k=n=0 ; k<p->num_bounds ; k++ ) {
        pt[0] = pt[1] = pt[2] = 0.;
        for ( j=0 ; j<p->bounds[k] ; j++ ) {
          pt[ 0 ] += pv[n+j].xyz[ 0 ];
          pt[ 1 ] += pv[n+j].xyz[ 1 ];
          pt[ 2 ] += pv[n+j].xyz[ 2 ];
        }
        pt[ 0 ] /= p->bounds[k];
        pt[ 1 ] /= p->bounds[k];
        pt[ 2 ] /= p->bounds[k];
        glVertex3fv ( pt );
        n += p->bounds[k];
      }
    }
  } else if( p->num_edges > 0 ) {
    pt[0] = pt[1] = pt[2] = 0.;
    for ( j=0 ; j<p->num_edges ; j++ ) {
      iv = p->edges[j];
      pt[ 0 ] += pv[iv].xyz[ 0 ];
      pt[ 1 ] += pv[iv].xyz[ 1 ];
      pt[ 2 ] += pv[iv].xyz[ 2 ];
    }
    pt[ 0 ] /= p->num_edges;
    pt[ 1 ] /= p->num_edges;
    pt[ 2 ] /= p->num_edges;
    glVertex3fv ( pt );
  } else {
    pt[0] = pt[1] = pt[2] = 0.;
    for ( j=0 ; j<p->num_vertexs ; j++ ) {
      pt[ 0 ] += pv[j].xyz[ 0 ];
      pt[ 1 ] += pv[j].xyz[ 1 ];
      pt[ 2 ] += pv[j].xyz[ 2 ];
    }
    pt[ 0 ] /= p->num_vertexs;
    pt[ 1 ] /= p->num_vertexs;
    pt[ 2 ] /= p->num_vertexs;
    glVertex3fv ( pt );
  }
}  

/*----------------------------------------------------------------------*/
static void draw_degenerates_quadrangles_as_points ( call_def_parray p ) { 

  Tint       i, j, iv;
  GLfloat    pt[ 3 ];
  tel_point  pv = p -> vertices;

#ifdef FULL_DEGENER
  if ( g_fSkipRatio == 1. )
    return;
#endif

  if( p->num_edges > 0 ) {
    for ( j=0 ; j<p->num_edges ; j+=4 ) {
      pt[0] = pt[1] = pt[2] = 0.;
      for ( i=0 ; i<4 ; i++ ) {
        iv = p->edges[j+i];
        pt[ 0 ] += pv[iv].xyz[ 0 ];
        pt[ 1 ] += pv[iv].xyz[ 1 ];
        pt[ 2 ] += pv[iv].xyz[ 2 ];
      }
      pt[ 0 ] /= 4;
      pt[ 1 ] /= 4;
      pt[ 2 ] /= 4;
      glVertex3fv ( pt );
    }
  } else {
    for ( j=0 ; j<p->num_vertexs ; j+=4 ) {
      pt[0] = pt[1] = pt[2] = 0.;
      for ( i=0 ; i<4 ; i++ ) {
        pt[ 0 ] += pv[j+i].xyz[ 0 ];
        pt[ 1 ] += pv[j+i].xyz[ 1 ];
        pt[ 2 ] += pv[j+i].xyz[ 2 ];
      }
      pt[ 0 ] /= 4;
      pt[ 1 ] /= 4;
      pt[ 2 ] /= 4;
      glVertex3fv ( pt );
    }
  }
}  

/*----------------------------------------------------------------------*/
static void draw_degenerates_quadranglestrips_as_points ( call_def_parray p ) { 

  Tint       i, j, k, n;
  GLfloat    pt[ 3 ];
  tel_point  pv = p -> vertices;

#ifdef FULL_DEGENER
  if ( g_fSkipRatio == 1. )
    return;
#endif

  if( p->num_bounds > 0 ) {
    for ( k=n=0 ; k<p->num_bounds ; k++ ) {
      for ( j=0 ; j<p->bounds[k]-2 ; j+=2 ) {
        pt[0] = pt[1] = pt[2] = 0.;
        for ( i=0 ; i<4 ; i++ ) {
          pt[ 0 ] += pv[n+j+i].xyz[ 0 ];
          pt[ 1 ] += pv[n+j+i].xyz[ 1 ];
          pt[ 2 ] += pv[n+j+i].xyz[ 2 ];
        }
        pt[ 0 ] /= 4;
        pt[ 1 ] /= 4;
        pt[ 2 ] /= 4;
        glVertex3fv ( pt );
      }
      n += p->bounds[k];
    }
  } else {
    for ( j=0 ; j<p->num_vertexs-2 ; j+=2 ) {
      pt[0] = pt[1] = pt[2] = 0.;
      for ( i=0 ; i<4 ; i++ ) {
        pt[ 0 ] += pv[j+i].xyz[ 0 ];
        pt[ 1 ] += pv[j+i].xyz[ 1 ];
        pt[ 2 ] += pv[j+i].xyz[ 2 ];
      }
      pt[ 0 ] /= 4;
      pt[ 1 ] /= 4;
      pt[ 2 ] /= 4;
      glVertex3fv ( pt );
    }
  }
}  

/*----------------------------------------------------------------------*/
static void draw_degenerates_as_points ( call_def_parray p, 
                                        tel_colour edge_colour ) 
{

  GLboolean zbuff_state = glIsEnabled(GL_DEPTH_TEST);
  LightOff ();
  if( zbuff_state ) glDisable(GL_DEPTH_TEST); 
  glColor3fv( edge_colour->rgb );
  glBegin ( GL_POINTS );
  switch( draw_mode ) {
      case GL_POINTS:
        draw_degenerates_points_as_points( p );
        break;
      case GL_LINES:
        draw_degenerates_lines_as_points( p );
        break;
      case GL_LINE_STRIP:
      case GL_POLYGON:
        draw_degenerates_polygons_as_points( p );
        break;
      case GL_TRIANGLES:
        draw_degenerates_triangles_as_points( p );
        break;
      case GL_QUADS:
        draw_degenerates_quadrangles_as_points( p );
        break;
      case GL_TRIANGLE_FAN:
      case GL_TRIANGLE_STRIP:
        draw_degenerates_trianglestrips_as_points( p );
        break;
      case GL_QUAD_STRIP:
        draw_degenerates_quadranglestrips_as_points( p );
        break;
      default:
        break;
  }

  glEnd ();

  if( zbuff_state ) glEnable(GL_DEPTH_TEST); 
}

/*----------------------------------------------------------------------*/
static void draw_degenerates_lines_as_lines ( call_def_parray p ) { 

  Tint       i,j,n,iv;
  tel_point  pv = p -> vertices;

  n = p->num_vertexs;
  j = (int)((1.-g_fSkipRatio)*n);
  while ( j-- ) {
    i = OGL_Rand() % n;
    p->keys[i] = -p->keys[i];
  };

  if( p->num_bounds > 0 ) {
    if( p->num_edges > 0 ) {
      for ( i=n=0 ; i<p->num_bounds ; i++ ) {
        glBegin ( GL_LINES );
        for ( j=0 ; j<p->bounds[i] ; j++ ) {
          iv = p->edges[n+j];
          if( p->keys[iv] < 0 ) {
            p->keys[iv] = -p->keys[iv];
            glVertex3fv ( pv[iv].xyz );
          }
        }
        glEnd();
        n += p->bounds[i];
      }
    } else {  
      for ( i=n=0 ; i<p->num_bounds ; i++ ) {
        glBegin ( GL_LINES );
        for ( j=0 ; j<p->bounds[i] ; j++ ) {
          if( p->keys[n+j] < 0 ) {
            p->keys[n+j] = -p->keys[n+j];
            glVertex3fv ( pv[n+j].xyz );
          }
        }
        glEnd();
        n += p->bounds[i];
      }
    }
  } else if( p->num_edges > 0 ) {
    glBegin ( GL_LINES );
    for ( j=0 ; j<p->num_edges ; j++ ) {
      iv = p->edges[j];
      if( p->keys[iv] < 0 ) {
        p->keys[iv] = -p->keys[iv];
        glVertex3fv ( pv[iv].xyz );
      }
    }
    glEnd();
  } else {
    glBegin ( GL_LINES );
    for ( j=0 ; j<p->num_vertexs ; j++ ) {
      if( p->keys[j] < 0 ) {
        p->keys[j] = -p->keys[j];
        glVertex3fv ( pv[j].xyz );
      }
    }
    glEnd();
  }
}  

/*----------------------------------------------------------------------*/
static void draw_degenerates_triangles_as_lines ( call_def_parray p ) 
{ 

  Tint       i,j,n,iv;
  tel_point  pv = p -> vertices;

  n = p->num_vertexs/3;
  j = (int)((1.-g_fSkipRatio)*n);
  while ( j-- ) {
    i = OGL_Rand() % n; i *= 3;
    p->keys[i] = -p->keys[i];
  };

  if( p->num_edges > 0 ) {
    for ( j=0 ; j<p->num_edges ; j+=3 ) {
      iv = p->edges[j];
      if( p->keys[iv] < 0 ) { 
        p->keys[iv] = -p->keys[iv];
        glBegin ( GL_LINE_LOOP );
        for ( i=0 ; i<3 ; i++ ) {
          iv = p->edges[j+i];
          glVertex3fv ( pv[iv].xyz );
        }
        glEnd();
      }
    }
  } else {
    for ( j=0 ; j<p->num_vertexs ; j+=3 ) {
      if( p->keys[j] < 0 ) { 
        p->keys[j] = -p->keys[j];
        glBegin ( GL_LINE_LOOP );
        for ( i=0 ; i<3 ; i++ ) {
          glVertex3fv ( pv[j+i].xyz );
        }
        glEnd();
      }
    }
  }
}  

/*----------------------------------------------------------------------*/
static void draw_degenerates_trianglestrips_as_lines ( call_def_parray p ) 
{ 

  Tint       i,j,k,n,ni;
  tel_point  pv = p -> vertices;

  if( p->num_bounds > 0 ) {
    for ( i=n=0 ; i<p->num_bounds ; i++ ) {
      ni = p->bounds[i]-2;
      k = (int)((1.-g_fSkipRatio)*ni);
      while ( k-- ) {
        j = OGL_Rand() % ni; j += 2;
        p->keys[n+j] = -p->keys[n+j];
      };
      for ( j=2 ; j<p->bounds[i] ; j++ ) {
        if( p->keys[n+j] < 0 ) { 
          p->keys[n+j] = -p->keys[n+j];
          glBegin ( GL_LINE_LOOP );
          glVertex3fv ( pv[n+j-2].xyz );
          glVertex3fv ( pv[n+j-1].xyz );
          glVertex3fv ( pv[n+j].xyz );
          glEnd();
        }
      }
      n += p->bounds[i];
    }
  } else {
    ni = p->num_vertexs-2;
    k = (int)((1.-g_fSkipRatio)*ni);
    while ( k-- ) {
      j = OGL_Rand() % ni; j += 2;
      p->keys[j] = -p->keys[j];
    };
    for ( j=2 ; j<p->num_vertexs ; j++ ) {
      if( p->keys[j] < 0 ) { 
        p->keys[j] = -p->keys[j];
        glBegin ( GL_LINE_LOOP );
        glVertex3fv ( pv[j-2].xyz );
        glVertex3fv ( pv[j-1].xyz );
        glVertex3fv ( pv[j].xyz );
        glEnd();
      }
    }
  }
}  

/*----------------------------------------------------------------------*/
static void draw_degenerates_polygons_as_lines ( call_def_parray p )
{ 
  Tint       i,j,n,iv;
  tel_point  pv = p -> vertices;

  n = p->num_vertexs;
  j = (int)((1.-g_fSkipRatio)*n);
  while ( j-- ) {
    i = OGL_Rand() % n;
    p->keys[i] = -p->keys[i];
  };

  if( p->num_bounds > 0 ) {
    if( p->num_edges > 0 ) {
      for ( i=n=0 ; i<p->num_bounds ; i++ ) {
        glBegin ( GL_LINE_LOOP );
        for ( j=0 ; j<p->bounds[i] ; j++ ) {
          iv = p->edges[n+j];
          if( p->keys[iv] < 0 ) {
            p->keys[iv] = -p->keys[iv];
            glVertex3fv ( pv[iv].xyz );
          }
        }
        glEnd();
        n += p->bounds[i];
      }
    } else {  
      for ( i=n=0 ; i<p->num_bounds ; i++ ) {
        glBegin ( GL_LINE_LOOP );
        for ( j=0 ; j<p->bounds[i] ; j++ ) {
          if( p->keys[n+j] < 0 ) {
            p->keys[n+j] = -p->keys[n+j];
            glVertex3fv ( pv[n+j].xyz );
          }
        }
        glEnd();
        n += p->bounds[i];
      }
    }
  } else if( p->num_edges > 0 ) {
    glBegin ( GL_LINE_LOOP );
    for ( j=0 ; j<p->num_edges ; j++ ) {
      iv = p->edges[j];
      if( p->keys[iv] < 0 ) {
        p->keys[iv] = -p->keys[iv];
        glVertex3fv ( pv[iv].xyz );
      }
    }
    glEnd();
  } else {
    glBegin ( GL_LINE_LOOP );
    for ( j=0 ; j<p->num_vertexs ; j++ ) {
      if( p->keys[j] < 0 ) {
        p->keys[j] = -p->keys[j];
        glVertex3fv ( pv[j].xyz );
      }
    }
    glEnd();
  }

}  

/*----------------------------------------------------------------------*/
static void draw_degenerates_quadrangles_as_lines ( call_def_parray p ) 
{ 

  Tint       i,j,n,iv;
  tel_point  pv = p -> vertices;

  n = p->num_vertexs/4;
  j = (int)((1.-g_fSkipRatio)*n);
  while ( j-- ) {
    i = OGL_Rand() % n; i *= 4;
    p->keys[i] = -p->keys[i];
  };

  if( p->num_edges > 0 ) {
    for ( j=0 ; j<p->num_edges ; j+=4 ) {
      iv = p->edges[j];
      if( p->keys[iv] < 0 ) {
        p->keys[iv] = -p->keys[iv];
        glBegin ( GL_LINE_LOOP );
        for ( i=0 ; i<4 ; i++ ) {
          iv = p->edges[j+i];
          glVertex3fv ( pv[iv].xyz );
        }
        glEnd();
      }
    }
  } else {
    for ( j=0 ; j<p->num_vertexs ; j+=4 ) {
      if( p->keys[j] < 0 ) {
        p->keys[j] = -p->keys[j];
        glBegin ( GL_LINE_LOOP );
        for ( i=0 ; i<4 ; i++ ) {
          glVertex3fv ( pv[j+i].xyz );
        }
        glEnd();
      }
    }
  }
}  

/*----------------------------------------------------------------------*/
static void draw_degenerates_quadranglestrips_as_lines ( call_def_parray p )
{ 

  Tint       i,j,k,n,ni;
  tel_point  pv = p -> vertices;

  if( p->num_bounds > 0 ) {
    for ( i=n=0 ; i<p->num_bounds ; i++ ) {
      ni = p->bounds[i]/2-2;
      k = (int)((1.-g_fSkipRatio)*ni);
      while ( k-- ) {
        j = OGL_Rand() % ni; j = j*2+2;
        p->keys[n+j] = -p->keys[n+j];
      };
      for ( j=3 ; j<p->bounds[i] ; j+=2 ) {
        if( p->keys[n+j] < 0 ) {
          p->keys[n+j] = -p->keys[n+j];
          glBegin ( GL_LINE_LOOP );
          glVertex3fv ( pv[n+j-3].xyz );
          glVertex3fv ( pv[n+j-2].xyz );
          glVertex3fv ( pv[n+j-1].xyz );
          glVertex3fv ( pv[n+j].xyz );
          glEnd();
        }
      }
      n += p->bounds[i];
    }
  } else {
    ni = p->num_vertexs/2-2;
    k = (int)((1.-g_fSkipRatio)*ni);
    while ( k-- ) {
      j = OGL_Rand() % ni; j = j*2+2;
      p->keys[j] = -p->keys[j];
    };
    for ( j=3 ; j<p->num_vertexs ; j+=2 ) {
      if( p->keys[j] < 0 ) {
        p->keys[j] = -p->keys[j];
        glBegin ( GL_LINE_LOOP );
        glVertex3fv ( pv[j-3].xyz );
        glVertex3fv ( pv[j-2].xyz );
        glVertex3fv ( pv[j-1].xyz );
        glVertex3fv ( pv[j].xyz );
        glEnd();
      }
    }
  }
}  

/*----------------------------------------------------------------------*/
static void draw_degenerates_as_lines ( call_def_parray p, 
                                       tel_colour edge_colour )
{

  GLint renderMode;

  GLboolean zbuff_state = glIsEnabled(GL_DEPTH_TEST);

  LightOff ();

  if( zbuff_state ) glDisable(GL_DEPTH_TEST); 

  glColor3fv( edge_colour->rgb );

  if( g_fSkipRatio != 0 ) switch( draw_mode ) {
      case GL_POINTS:
        draw_degenerates_points_as_points( p );
        break;
      case GL_LINES:
        draw_degenerates_lines_as_lines( p );
        break;
      case GL_LINE_STRIP:
      case GL_POLYGON:
        draw_degenerates_polygons_as_lines( p );
        break;
      case GL_TRIANGLES:
        draw_degenerates_triangles_as_lines( p );
        break;
      case GL_QUADS:
        draw_degenerates_quadrangles_as_lines( p );
        break;
      case GL_TRIANGLE_FAN:
      case GL_TRIANGLE_STRIP:
        draw_degenerates_trianglestrips_as_lines( p );
        break;
      case GL_QUAD_STRIP:
        draw_degenerates_quadranglestrips_as_lines( p );
        break;
      default:
        break;
  }
  else {
    int i,n;

#ifdef OCC3192
    GLboolean color_array_mode, 
      edge_flag_array_mode,
      index_array_mode,
      normal_array_mode,
      texture_coord_array_mode,
      vertex_array_mode;
#endif
    glPushAttrib( GL_POLYGON_BIT );
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

#ifdef OCC3192
    color_array_mode         = glIsEnabled( GL_COLOR_ARRAY );
    edge_flag_array_mode     = glIsEnabled( GL_EDGE_FLAG_ARRAY );
    index_array_mode         = glIsEnabled( GL_INDEX_ARRAY );
    normal_array_mode        = glIsEnabled( GL_NORMAL_ARRAY );
    texture_coord_array_mode = glIsEnabled( GL_TEXTURE_COORD_ARRAY );
    vertex_array_mode        = glIsEnabled( GL_VERTEX_ARRAY );

    glDisableClientState( GL_COLOR_ARRAY );
    glDisableClientState( GL_EDGE_FLAG_ARRAY );
    glDisableClientState( GL_INDEX_ARRAY );
    glDisableClientState( GL_NORMAL_ARRAY );
    glDisableClientState( GL_TEXTURE_COORD_ARRAY );

    if( !vertex_array_mode ) glEnableClientState( GL_VERTEX_ARRAY );
#else
    glEnableClientState( GL_VERTEX_ARRAY );
#endif
    glVertexPointer(3, GL_FLOAT, 0, p->vertices); /* array of vertices  */

    glGetIntegerv( GL_RENDER_MODE, &renderMode );

    if( p->num_bounds > 0 ) {
      if( p->num_edges > 0 ) {
        for( i=n=0 ; i<p->num_bounds ; i++ ) {
          if( renderMode == GL_FEEDBACK )
            draw_primitive_elements( p, draw_mode, p->bounds[i], 
            GL_UNSIGNED_INT, (GLenum*) &p->edges[n]);
          else
            glDrawElements( draw_mode, p->bounds[i],
            GL_UNSIGNED_INT, &p->edges[n]);
          n += p->bounds[i];
        }
      } else {
        for( i=n=0 ; i<p->num_bounds ; i++ ) {
          if( renderMode == GL_FEEDBACK )
            draw_primitive_array( p, draw_mode, n, p->bounds[i]);
          else
            glDrawArrays( draw_mode, n, p->bounds[i]);
          n += p->bounds[i];
        }
      }
    } else if( p->num_edges > 0 ) {
      if( renderMode == GL_FEEDBACK )
        draw_primitive_elements( p, draw_mode, p->num_edges, 
        GL_UNSIGNED_INT, (GLenum*) p->edges);
      else
        glDrawElements( draw_mode, p->num_edges,
        GL_UNSIGNED_INT, p->edges);
    } else {
      if( renderMode == GL_FEEDBACK )
        draw_primitive_array( p, draw_mode, 0, p->num_vertexs);
      else
        glDrawArrays( draw_mode, 0, p->num_vertexs);
    }

#ifdef OCC3192
    if( !vertex_array_mode ) glDisableClientState( GL_VERTEX_ARRAY );

    if( color_array_mode )         glEnableClientState( GL_COLOR_ARRAY );
    if( edge_flag_array_mode )     glEnableClientState( GL_EDGE_FLAG_ARRAY );
    if( index_array_mode )         glEnableClientState( GL_INDEX_ARRAY );
    if( normal_array_mode )        glEnableClientState( GL_NORMAL_ARRAY );
    if( texture_coord_array_mode ) glEnableClientState( GL_TEXTURE_COORD_ARRAY );
#endif
    glPopAttrib();
  }

  if( zbuff_state ) glEnable(GL_DEPTH_TEST); 

}  

static void draw_degenerates_as_bboxs ( call_def_parray p, 
                                        tel_colour edge_colour )
{

  Tint      i;
  GLfloat   minp[ 3 ] = { FLT_MAX, FLT_MAX, FLT_MAX };
  GLfloat   maxp[ 3 ] = { FLT_MIN, FLT_MIN, FLT_MIN };
  tel_point pv = p -> vertices;

  LightOff ();

  glColor3fv( edge_colour->rgb );

  for ( i=0 ; i<p->num_vertexs ; ++i ) {
    if ( pv[  i  ].xyz[ 0 ] < minp[ 0 ] )
      minp[ 0 ] = pv[  i  ].xyz[ 0 ] ;
    if ( pv[  i  ].xyz[ 1 ] < minp[ 1 ] )
      minp[ 1 ] = pv[  i  ].xyz[ 1 ] ;
    if ( pv[  i  ].xyz[ 2 ] < minp[ 2 ] )
      minp[ 2 ] = pv[  i  ].xyz[ 2 ] ;

    if ( pv[  i  ].xyz[ 0 ] > maxp[ 0 ] )
      maxp[ 0 ] = pv[  i  ].xyz[ 0 ] ;
    if ( pv[  i  ].xyz[ 1 ] > maxp[ 1 ] )
      maxp[ 1 ] = pv[  i  ].xyz[ 1 ] ;
    if ( pv[  i ].xyz[ 2 ] > maxp[ 2 ] )
      maxp[ 2 ] = pv[  i  ].xyz[ 2 ] ;
  }  /* end for ( i ) */

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

}  /* end draw_degenerates_as_bboxs */
