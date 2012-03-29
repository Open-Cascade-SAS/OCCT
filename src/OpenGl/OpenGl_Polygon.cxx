// Created on: 2011-07-13
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <OpenGl_GlCore11.hxx>

#include <OpenGl_Polygon.hxx>

#include <OpenGl_telem_util.hxx>
#include <OpenGl_TextureBox.hxx>

#include <OpenGl_AspectFace.hxx>
#include <OpenGl_Structure.hxx>

#include <GL/glu.h>

#if (defined(_WIN32) || defined(__WIN32__))
  #define STATIC
#else
  #define STATIC static
#endif

struct EXTRA_VERTEX
{
  GLfloat vert[3];
  int ind;
  DEFINE_STANDARD_ALLOC
};
typedef EXTRA_VERTEX* extra_vertex;

struct SEQ_
{
  NCollection_Vector<void *> tmesh_sequence;
  GLenum triangle_type; /* FSXXX OPTI */
  DEFINE_STANDARD_ALLOC
};

static void bgntriangulate( const TEL_POLYGON_DATA *, void (APIENTRY*)() );
static void endtriangulate(void);

#ifndef GLU_VERSION_1_2
  #define GLUtesselator GLUtriangulatorObj
  void gluTessBeginContour();
  void gluTessBeginPolygon();
  void gluTessEndPolygon();
  void gluTessEndContour();
  #define GLU_TESS_BEGIN   100100
  #define GLU_TESS_VERTEX  100101
  #define GLU_TESS_END     100102
  #define GLU_TESS_ERROR   100103
  #define GLU_TESS_COMBINE 100105
#endif

/*----------------------------------------------------------------------*/

void OpenGl_Polygon::draw_polygon (const Handle(OpenGl_Workspace) &AWorkspace, Tint front_lighting_model) const
{
  Tint      i;

  tel_point ptr;
  tel_point  pvn;
  tel_colour pfc, pvc;
  tel_texture_coord pvt;

  pfc = myData.fcolour;
  pvc = myData.vcolours;
  pvn = myData.vnormals;
  pvt = myData.vtexturecoord;

  if ( AWorkspace->NamedStatus & OPENGL_NS_HIGHLIGHT )
    pvc = pfc = NULL;

  ptr = myData.vertices;
  if ( pfc )
    glColor3fv( pfc->rgb );
  if ( front_lighting_model )
    glNormal3fv( myData.fnormal.xyz );

  if( myData.reverse_order ) glFrontFace( GL_CW );

  if (myData.num_vertices == 3) glBegin(GL_TRIANGLES);
  else if(myData.num_vertices == 4) glBegin(GL_QUADS);
  else glBegin(GL_POLYGON);
  if( front_lighting_model )
  {
    if( pvn )
    {
      if (pvt && (AWorkspace->NamedStatus & OPENGL_NS_FORBIDSETTEX) == 0)
        for( i=0; i<myData.num_vertices; i++, ptr++ )
        {
          glNormal3fv( pvn[i].xyz );
          glTexCoord2fv( pvt[i].xy );
          glVertex3fv( ptr->xyz );
        }
      else
        for( i=0; i<myData.num_vertices; i++, ptr++ )
        {
          glNormal3fv( pvn[i].xyz );
          glVertex3fv( ptr->xyz );
        }      
    }
    else
    {
      for( i=0; i<myData.num_vertices; i++, ptr++ )
      {
        glVertex3fv( ptr->xyz );
      }
    }
  }
  else
  {
    if( pvc )
    {
      for( i=0; i<myData.num_vertices; i++, ptr++ )
      {
        glColor3fv( pvc[i].rgb );
        glVertex3fv( ptr->xyz );
      }
    }
    else
    {
      for( i=0; i<myData.num_vertices; i++, ptr++ )
      {
        glVertex3fv( ptr->xyz );
      }
    }
  } 
  glEnd();
  if( myData.reverse_order ) glFrontFace( GL_CCW );

}

/*----------------------------------------------------------------------*/

/* JWR - allow varying the size */

static const TEL_POLYGON_DATA *DaTa;
static GLUtesselator *tripak = 0;

STATIC void APIENTRY
out_bgntmesh( GLenum triangle_type )
{
  NCollection_Vector<SEQ_> *dis = DaTa->dsply;

  SEQ_ aSeq;
#ifdef JWR_DEC_TRIFAN_BUG
  aSeq.triangle_type = GL_POLYGON;
  dis->Append(aSeq);
  glBegin(GL_POLYGON);
#else
  aSeq.triangle_type = triangle_type;
  dis->Append(aSeq);
  glBegin(triangle_type);
#endif
}

/*----------------------------------------------------------------------*/

STATIC void APIENTRY
out_vert1( void *data )
{
  SEQ_ &s = DaTa->dsply->ChangeValue(DaTa->dsply->Length() - 1);

  s.tmesh_sequence.Append(data);

  if ( data < (void *)0xffff ) {
    long a = (long)data;

    glVertex3fv( DaTa->vertices[a].xyz );
  }
  else {
    extra_vertex b = (extra_vertex) data;

    glVertex3fv( b->vert );
  }

}

/*----------------------------------------------------------------------*/

STATIC void APIENTRY
out_vert2( void *data )
{
  SEQ_ &s = DaTa->dsply->ChangeValue(DaTa->dsply->Length() - 1);

  s.tmesh_sequence.Append(data);

  if ( data < (void *)0xffff ) {
    long a = (long)data;

    glColor3fv( DaTa->vcolours[a].rgb );
    glVertex3fv(  DaTa->vertices[a].xyz );
  }
  else {
    extra_vertex b = (extra_vertex) data;

    glColor3fv( DaTa->vcolours[(b->ind)].rgb );
    glVertex3fv( b->vert );
  }
}

/*----------------------------------------------------------------------*/

STATIC void APIENTRY
out_vert3( void *data )
{
  SEQ_ &s = DaTa->dsply->ChangeValue(DaTa->dsply->Length() - 1);

  s.tmesh_sequence.Append(data);

  if ( data <= (void *)0xffff ) {
    long a = (long)data;

    glNormal3fv(  DaTa->vnormals[a].xyz );
    glVertex3fv(  DaTa->vertices[a].xyz);
  }
  else {
    extra_vertex b = (extra_vertex) data;

    glNormal3fv(  DaTa->vnormals[(b->ind)].xyz );
    glVertex3fv( b->vert );
  }
}

/*----------------------------------------------------------------------*/

STATIC void APIENTRY
mycombine( GLdouble coords[3], int *data, GLfloat w[4], void **dataout)
{
  extra_vertex new_vertex = new EXTRA_VERTEX();

  new_vertex->vert[0] = ( float )coords[0];
  new_vertex->vert[1] = ( float )coords[1];
  new_vertex->vert[2] = ( float )coords[2];
  new_vertex->ind =   *data;
  *dataout = new_vertex;
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
bgntriangulate(const TEL_POLYGON_DATA *d, void ( APIENTRY * out_ver)() )
{
  DaTa = d;

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

void OpenGl_Polygon::draw_polygon_concav (const Handle(OpenGl_Workspace) &AWorkspace, Tint front_lighting_model) const
{
  long       i;

  tel_point  pvn;
  tel_point  ptr;
  tel_colour pfc, pvc;
  GLdouble  xyz[3];

  pfc = myData.fcolour;
  pvc = myData.vcolours;
  pvn = myData.vnormals;

  if ( AWorkspace->NamedStatus & OPENGL_NS_HIGHLIGHT )
    pvc = pfc = NULL;

  ptr = myData.vertices;
  DaTa = &myData;

  if ( pfc )
    glColor3fv( pfc->rgb );
  if ( front_lighting_model )
    glNormal3fv( myData.fnormal.xyz );

  if( myData.reverse_order ) glFrontFace( GL_CW );

  if( !myData.dsply )
  {
    if( front_lighting_model )
    {
      if( pvn )
      {
        bgntriangulate(&myData, (void (APIENTRY*)())out_vert3);
      }
      else
      {
        bgntriangulate(&myData, (void (APIENTRY*)())out_vert1);
      }
    }
    else
    {
      if( pvc )
      {
        bgntriangulate(&myData, (void (APIENTRY*)())out_vert2);
      }
      else
      {
        bgntriangulate(&myData, (void (APIENTRY*)())out_vert1);
      }
    }
    gluTessBeginPolygon( tripak, NULL );
    gluTessBeginContour( tripak);

    for( i=0; i<myData.num_vertices; i++, ptr++ )
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
    gluTessEndContour( tripak );
    gluTessEndPolygon( tripak );
    endtriangulate();
  } 
  else 
  {
    if( front_lighting_model )
    {
      draw_tmesh( pvn? 3 : 1 );
    }
    else
    {
      draw_tmesh( pvc? 2 : 1 );
    }
  }

  if( myData.reverse_order ) glFrontFace( GL_CCW );
}

/*----------------------------------------------------------------------*/

void OpenGl_Polygon::draw_edges (const TEL_COLOUR *edge_colour, const Aspect_InteriorStyle interior_style, const Handle(OpenGl_Workspace) &AWorkspace) const
{
  const OpenGl_AspectFace *aspect_face = AWorkspace->AspectFace( Standard_True );

  if ( interior_style != Aspect_IS_HIDDENLINE && aspect_face->Context().Edge == TOff )
    return;

  glDisable(GL_LIGHTING);
  const GLboolean texture_on = IsTextureEnabled();
  if (texture_on) DisableTexture();

  // Setup line aspect
  const OpenGl_AspectLine *aspect_line_old = AWorkspace->SetAspectLine( aspect_face->AspectEdge() );
  AWorkspace->AspectLine( Standard_True );

  Tint i;
  tel_point ptr = myData.vertices;

  glColor3fv( edge_colour->rgb );

  glBegin(GL_LINE_LOOP);
  for( i=0; i<myData.num_vertices; i++, ptr++ )
  {
    glVertex3fv( ptr->xyz );
  }
  glEnd();

  // Restore line context
  AWorkspace->SetAspectLine( aspect_line_old );

  if (texture_on) EnableTexture();
}

/*----------------------------------------------------------------------*/

void OpenGl_Polygon::draw_tmesh ( Tint v ) const
{
  Tint      i, j, k;
  SEQ_     *s;
  extra_vertex b;

  NCollection_Vector<SEQ_> *dis = myData.dsply;
  for( i = 0; i < dis->Length(); i++ )
  {
    s = &(dis->ChangeValue(i));

    glBegin(s->triangle_type);
    switch( v )
    {
    case 1:
      {
        for( j = 0, k = 0; j < s->tmesh_sequence.Length(); j++ )
        {
          if ( s->tmesh_sequence(j) < (void *)0xffff )
            glVertex3fv( myData.vertices[ (long)s->tmesh_sequence.Value(j) ].xyz );
          else {
            b = (extra_vertex) s->tmesh_sequence(j);
            glVertex3fv( b->vert );
          }

        }
        break;
      }
    case 2:
      {
        for( j = 0, k = 0; j < s->tmesh_sequence.Length(); j++ )
        {
          if ( s->tmesh_sequence(j) < (void *)0xffff ) {
            glColor3fv( myData.vcolours[ (long) s->tmesh_sequence(j) ].rgb );
            glVertex3fv( myData.vertices[ (long) s->tmesh_sequence(j) ].xyz );
          } else {
            b = (extra_vertex) s->tmesh_sequence(j);
            glColor3fv( myData.vcolours[(b->ind)].rgb);
            glVertex3fv( b->vert );
          }
        }
        break;
      }
    case 3:
      {
        for( j = 0, k = 0; j < s->tmesh_sequence.Length(); j++ )
        {
          if ( s->tmesh_sequence(j) < (void *)0xffff ) {
            glNormal3fv( myData.vnormals[ (long) s->tmesh_sequence(j) ].xyz);
            glVertex3fv( myData.vertices[ (long) s->tmesh_sequence(j) ].xyz);
          } else {
            b = (extra_vertex) s->tmesh_sequence(j);
            glNormal3fv( myData.vnormals[(b->ind)].xyz);
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

OpenGl_Polygon::OpenGl_Polygon (const Graphic3d_Array1OfVertex& AListVertex,
                              const Graphic3d_TypeOfPolygon AType)
{
  const Standard_Integer nv = AListVertex.Length();

  myData.num_vertices = nv;

  myData.vertices = new TEL_POINT[nv];
  memcpy( myData.vertices, &AListVertex(AListVertex.Lower()), nv*sizeof(TEL_POINT) );

  myData.vertex_flag = TEL_VT_NONE;
  myData.vnormals = NULL;

  myData.vcolours = NULL;

  myData.vtexturecoord = NULL;

  myData.reverse_order = 0;

  myData.facet_flag = TEL_FA_NONE;
  TelGetPolygonNormal( myData.vertices, NULL, nv, myData.fnormal.xyz );

  myData.fcolour = NULL;

#if defined(__sgi) || defined(IRIX)
  // Pb with tesselator on sgi
  myData.shape_flag = TEL_SHAPE_CONVEX;
#else
  switch (AType)
  {
    case Graphic3d_TOP_UNKNOWN :
      myData.shape_flag = TEL_SHAPE_UNKNOWN;
      break;
    case Graphic3d_TOP_COMPLEX :
      myData.shape_flag = TEL_SHAPE_COMPLEX;
      break;
    case Graphic3d_TOP_CONCAVE :
      myData.shape_flag = TEL_SHAPE_CONCAVE;
      break;
    //case Graphic3d_TOP_CONVEX :
    default :
      myData.shape_flag = TEL_SHAPE_CONVEX;
      break;
  }
#endif

  myData.dsply = new NCollection_Vector<SEQ_>();
}

/*----------------------------------------------------------------------*/

OpenGl_Polygon::~OpenGl_Polygon ()
{
  if( myData.fcolour )
    delete myData.fcolour;
  if( myData.vertices )
    delete[] myData.vertices;
  if( myData.vcolours )
    delete[] myData.vcolours;
  if( myData.vnormals )
    delete[] myData.vnormals;
  if ( myData.vtexturecoord )
    delete myData.vtexturecoord;

  if ( myData.dsply )
  {
    Tint i, j;

    for( i = 0; i <  myData.dsply->Length(); i++ )
    {
      for ( j = 0; j < myData.dsply->Value(i).tmesh_sequence.Length() ; j++ )
      {
        if ( myData.dsply->Value(i).tmesh_sequence(j) >= (void *)0xffff )
          delete myData.dsply->Value(i).tmesh_sequence(j);
      }
    }

    delete myData.dsply;
  }
}

/*----------------------------------------------------------------------*/

void OpenGl_Polygon::Render (const Handle(OpenGl_Workspace) &AWorkspace) const
{
  const OpenGl_AspectFace *aspect_face = AWorkspace->AspectFace( Standard_True );

  Tint front_lighting_model = aspect_face->Context().IntFront.color_mask;
  const Aspect_InteriorStyle interior_style = aspect_face->Context().InteriorStyle;
  const TEL_COLOUR *interior_colour = &aspect_face->Context().IntFront.matcol;
  const TEL_COLOUR *edge_colour = &aspect_face->AspectEdge()->Color();

  // Use highlight colous
  if ( AWorkspace->NamedStatus & OPENGL_NS_HIGHLIGHT )
  {                         
    edge_colour = interior_colour = AWorkspace->HighlightColor;
    front_lighting_model = 0;
  }

  if( interior_style != Aspect_IS_EMPTY && AWorkspace->DegenerateModel < 2 )
  {          
    if ( front_lighting_model )
      glEnable(GL_LIGHTING);
    else
      glDisable(GL_LIGHTING);

    glColor3fv( interior_colour->rgb );

    if( myData.shape_flag != TEL_SHAPE_CONVEX )
      draw_polygon_concav( AWorkspace, front_lighting_model );
    else
      draw_polygon( AWorkspace, front_lighting_model );
  }

  /* OCC11904 -- Temporarily disable environment mapping */
  glPushAttrib(GL_ENABLE_BIT);
  glDisable(GL_TEXTURE_1D);
  glDisable(GL_TEXTURE_2D);

  switch ( AWorkspace->DegenerateModel )
  {
    default:
      draw_edges ( edge_colour, interior_style, AWorkspace );
      break;
    case 3:  /* marker degeneration */
      break;
  }

  glPopAttrib(); /* skt: GL_ENABLE_BIT*/
}

/*----------------------------------------------------------------------*/
