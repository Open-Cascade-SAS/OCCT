// File:      OpenGl_TriangleStrip.cxx
// Created:   13 July 2011
// Author:    Sergey ZERCHANINOV
// Copyright: OPEN CASCADE 2011

#include <OpenGl_GlCore11.hxx>

#include <OpenGl_TriangleStrip.hxx>

#include <OpenGl_telem_util.hxx>
#include <OpenGl_TextureBox.hxx>

#include <OpenGl_AspectFace.hxx>
#include <OpenGl_Structure.hxx>

/*----------------------------------------------------------------------*/

void draw_degenerates_as_points ( PDS_INTERNAL, tel_point, Tint, const Handle(OpenGl_Workspace) & );
void draw_degenerates_as_bboxs  ( PDS_INTERNAL, tel_point, Tint, const Handle(OpenGl_Workspace) & );
extern void set_drawable_items  ( GLboolean*, int, const float           );

/*----------------------------------------------------------------------*/

void OpenGl_TriangleStrip::draw_tmesh (const Tint front_lighting_model,
                                      const Aspect_InteriorStyle interior_style,
                                      const TEL_COLOUR *edge_colour,
                                      const Handle(OpenGl_Workspace) &AWorkspace) const
{
  Tint              i, newList = 0;

  tel_colour pfc = myData.fcolours;
  tel_point pv  = myData.vertices;
  tel_colour pvc = myData.vcolours;
  tel_point pvn = myData.vnormals;
  tel_texture_coord pvt = myData.vtexturecoord;

  if ( AWorkspace->NamedStatus & OPENGL_NS_HIGHLIGHT ) pvc = pfc = 0;

  if ( AWorkspace->DegenerateModel < 2 && interior_style != Aspect_IS_EMPTY )
  {
    if ( front_lighting_model )
      glEnable(GL_LIGHTING);
    else
      glDisable(GL_LIGHTING);

    if ( (AWorkspace->NamedStatus & OPENGL_NS_ANIMATION) == 0 )
      goto doDraw; /* Disable object display list out of animation */
    if ( myDS->model != front_lighting_model || !myDS->list || myDS->model == -1 ||
        ( AWorkspace->DegenerateModel && myDS->skipRatio != AWorkspace->SkipRatio ) )
    {
        myDS->skipRatio = AWorkspace->SkipRatio;
        myDS->model     = front_lighting_model;
        myDS->degMode   = AWorkspace->DegenerateModel;

        if ( AWorkspace->SkipRatio <= 0.f ) {

          if ( !myDS->list ) myDS->list = glGenLists ( 1 );

          glNewList ( myDS->list, GL_COMPILE_AND_EXECUTE );
          newList = 1;
doDraw:
          glBegin ( GL_TRIANGLE_STRIP );

          if ( front_lighting_model )
          {
            if ( pvt && (AWorkspace->NamedStatus & OPENGL_NS_FORBIDSETTEX) == 0 )
            {
              for ( i = 0; i < myData.num_facets + 2; ++i )
              {
                glNormal3fv   ( pvn[ i ].xyz );
                glTexCoord2fv ( pvt[ i ].xy  );
                glVertex3fv   ( pv [ i ].xyz );
              }
            }
            else
              for ( i = 0; i < myData.num_facets + 2; ++i )
              {
                glNormal3fv( pvn[ i ].xyz );
                glVertex3fv( pv[  i ].xyz );
              }
          }
          else
          {
            if ( pvc )
            {
              for ( i = 0; i < myData.num_facets + 2; ++i )
              {
                glColor3fv  ( pvc[ i ].rgb );
                glVertex3fv ( pv [ i ].xyz );
              }
            }
            else if ( pfc )
            {
              glColor3fv  ( pfc[ 0 ].rgb );
              glVertex3fv ( pv [ 0 ].xyz );
              glVertex3fv ( pv [ 1 ].xyz );
              for ( i = 2; i < myData.num_facets + 2; ++i )
              {
                glColor3fv ( pfc[ i - 2 ].rgb );
                glVertex3fv( pv [ i     ].xyz );
              }
            }
            else
              for ( i = 0; i < myData.num_facets + 2; ++i ) glVertex3fv ( pv[ i ].xyz );
          }
          glEnd ();
        }
        else if ( AWorkspace->SkipRatio < 1.f )
        {
          set_drawable_items ( myDS->bDraw, myData.num_facets + 2, AWorkspace->SkipRatio );

          if ( !myDS->dlist ) myDS->dlist = glGenLists ( 1 );

          glNewList ( myDS->dlist, GL_COMPILE_AND_EXECUTE );
          newList = 1;

          glBegin ( GL_TRIANGLES );

          if ( front_lighting_model )
          {
            if ( pvt && (AWorkspace->NamedStatus & OPENGL_NS_FORBIDSETTEX) == 0 )
            {
              for ( i = 0; i < myData.num_facets; ++i )
              {
                if ( myDS->bDraw[ i ] )
                {
                  if ( i % 2 )
                  {
                    glNormal3fv   ( pvn[ i     ].xyz );
                    glTexCoord2fv ( pvt[ i     ].xy  );
                    glVertex3fv   ( pv [ i     ].xyz );
                    glNormal3fv   ( pvn[ i + 2 ].xyz );
                    glTexCoord2fv ( pvt[ i + 2 ].xy  );
                    glVertex3fv   ( pv [ i + 2 ].xyz );
                    glNormal3fv   ( pvn[ i + 1 ].xyz );
                    glTexCoord2fv ( pvt[ i + 1 ].xy  );
                    glVertex3fv   ( pv [ i + 1 ].xyz );
                  }
                  else
                  {
                    glNormal3fv   ( pvn[ i + 2 ].xyz );
                    glTexCoord2fv ( pvt[ i + 2 ].xy  );
                    glVertex3fv   ( pv [ i + 2 ].xyz );
                    glNormal3fv   ( pvn[ i     ].xyz );
                    glTexCoord2fv ( pvt[ i     ].xy  );
                    glVertex3fv   ( pv [ i     ].xyz );
                    glNormal3fv   ( pvn[ i + 1 ].xyz );
                    glTexCoord2fv ( pvt[ i + 1 ].xy  );
                    glVertex3fv   ( pv [ i + 1 ].xyz );
                  }
                }
              }
            }
            else
            {
              for ( i = 0; i < myData.num_facets; ++i )
              {
                if ( myDS->bDraw[ i ] )
                {
                  if ( i % 2 )
                  {
                    glNormal3fv( pvn[ i     ].xyz );
                    glVertex3fv( pv[  i     ].xyz );
                    glNormal3fv( pvn[ i + 2 ].xyz );
                    glVertex3fv( pv[  i + 2 ].xyz );
                    glNormal3fv( pvn[ i + 1 ].xyz );
                    glVertex3fv( pv[  i + 1 ].xyz );
                  }
                  else
                  {
                    glNormal3fv( pvn[ i + 2 ].xyz );
                    glVertex3fv( pv[  i + 2 ].xyz );
                    glNormal3fv( pvn[ i     ].xyz );
                    glVertex3fv( pv[  i     ].xyz );
                    glNormal3fv( pvn[ i + 1 ].xyz );
                    glVertex3fv( pv[  i + 1 ].xyz );
                  }
                }
              }
            }
          }
          else
          {
            if ( pvc )
            {
              for ( i = 0; i < myData.num_facets; ++i )
              {
                if ( myDS->bDraw[ i ] )
                {
                  if ( i % 2 )
                  {
                    glColor3fv  ( pvc[ i     ].rgb );
                    glVertex3fv ( pv [ i     ].xyz );
                    glColor3fv  ( pvc[ i + 2 ].rgb );
                    glVertex3fv ( pv [ i + 2 ].xyz );
                    glColor3fv  ( pvc[ i + 1 ].rgb );
                    glVertex3fv ( pv [ i + 1 ].xyz );
                  }
                  else
                  {
                    glColor3fv  ( pvc[ i + 2 ].rgb );
                    glVertex3fv ( pv [ i + 2 ].xyz );
                    glColor3fv  ( pvc[ i     ].rgb );
                    glVertex3fv ( pv [ i     ].xyz );
                    glColor3fv  ( pvc[ i + 1 ].rgb );
                    glVertex3fv ( pv [ i + 1 ].xyz );
                  }
                }
              }
            }
            else if ( pfc )
            {
              for ( i = 0; i < myData.num_facets; ++i )
              {
                if ( myDS->bDraw[ i ] )
                {
                  if ( i % 2 )
                  {
                    glColor3fv ( pfc[ i     ].rgb );
                    glVertex3fv( pv [ i     ].xyz );
                    glColor3fv ( pfc[ i + 2 ].rgb );
                    glVertex3fv( pv [ i + 2 ].xyz );
                    glColor3fv ( pfc[ i + 1 ].rgb );
                    glVertex3fv( pv [ i + 1 ].xyz );
                  }
                  else
                  {
                    glColor3fv ( pfc[ i + 2 ].rgb );
                    glVertex3fv( pv [ i + 2 ].xyz );
                    glColor3fv ( pfc[ i     ].rgb );
                    glVertex3fv( pv [ i     ].xyz );
                    glColor3fv ( pfc[ i + 1 ].rgb );
                    glVertex3fv( pv [ i + 1 ].xyz );
                  }
                }
              }
            }
            else
            {
              for ( i = 0; i < myData.num_facets; ++i )
              {
                if ( myDS->bDraw[ i ] )
                {
                  if ( i % 2 )
                  {
                    glVertex3fv ( pv[ i     ].xyz );
                    glVertex3fv ( pv[ i + 2 ].xyz );
                    glVertex3fv ( pv[ i + 1 ].xyz );
                  }
                  else
                  {
                    glVertex3fv ( pv[ i + 2 ].xyz );
                    glVertex3fv ( pv[ i     ].xyz );
                    glVertex3fv ( pv[ i + 1 ].xyz );
                  }
                }
              }
            }
          }
          glEnd ();
        }
        else
        {
          if ( !myDS->dlist ) myDS->dlist = glGenLists ( 1 );

          glNewList ( myDS->dlist, GL_COMPILE_AND_EXECUTE );
          newList = 1;
        }
        if ( newList ) glEndList ();

        if ( AWorkspace->DegenerateModel ) return;
      }
      else
      {
        glCallList ( AWorkspace->SkipRatio <= 0.f ? myDS->list : myDS->dlist );
        if ( AWorkspace->DegenerateModel ) return;
      }
  }

  i = 0;

  /* OCC11904 -- Temporarily disable environment mapping */
  glPushAttrib(GL_ENABLE_BIT);
  glDisable(GL_TEXTURE_1D);
  glDisable(GL_TEXTURE_2D);

  switch ( AWorkspace->DegenerateModel )
  {
    default:
      break;

    case 2:  /* XXX_TDM_WIREFRAME */
      i = 1;
      break;

    case 3:  /* XXX_TDM_MARKER */
      draw_degenerates_as_points ( myDS, myData.vertices, myData.num_facets + 2, AWorkspace );
      glPopAttrib();
      return;

    case 4:  /* XXX_TDM_BBOX */
      draw_degenerates_as_bboxs ( myDS, myData.vertices, myData.num_facets + 2, AWorkspace );
      glPopAttrib();
      return;
  }

  draw_edges ( edge_colour, interior_style, i, AWorkspace );

  glPopAttrib();
}

/*----------------------------------------------------------------------*/

void OpenGl_TriangleStrip::draw_edges (const TEL_COLOUR *edge_colour, const Aspect_InteriorStyle interior_style,
                                      Tint forceDraw, const Handle(OpenGl_Workspace) &AWorkspace) const
{
  const OpenGl_AspectFace *aspect_face = AWorkspace->AspectFace( Standard_True );

  if ( interior_style != Aspect_IS_HIDDENLINE && !forceDraw && aspect_face->Context().Edge == TOff )
    return;

  glDisable(GL_LIGHTING);
  const GLboolean texture_on = IsTextureEnabled();
  if ( texture_on ) DisableTexture();

  // Setup line aspect
  const OpenGl_AspectLine *aspect_line_old = AWorkspace->SetAspectLine( aspect_face->AspectEdge() );
  AWorkspace->AspectLine( Standard_True );

  tel_point pv = myData.vertices;
  Tint i, newList = 0;

  glColor3fv ( edge_colour->rgb );
  if ( !forceDraw )
    draw_line_loop ();
  else
  {
    if ( (AWorkspace->NamedStatus & OPENGL_NS_ANIMATION) == 0 )
      goto doDraw; /* Disable object display list out of animation */
    if ( myDS->degMode != 2 || myDS->skipRatio != AWorkspace->SkipRatio || !myDS->dlist ) {

      if ( !myDS->dlist ) myDS->dlist = glGenLists ( 1 );

      myDS->degMode   = 2;
      myDS->skipRatio = AWorkspace->SkipRatio;
      glNewList ( myDS->dlist, GL_COMPILE_AND_EXECUTE );
      newList = 1;
doDraw:
      glPushAttrib ( GL_DEPTH_BUFFER_BIT );
      glDisable ( GL_DEPTH_TEST );

      if ( AWorkspace->SkipRatio <= 0.f )

        draw_line_loop ();

      else if ( AWorkspace->SkipRatio < 1.f ) {

        set_drawable_items ( myDS->bDraw, myData.num_facets + 2, AWorkspace->SkipRatio );

        for ( i = 0; i < myData.num_facets; ++i )

          if ( myDS->bDraw[ i ] ) {

            glBegin ( GL_LINE_LOOP );
            glVertex3fv ( pv[ i     ].xyz );
            glVertex3fv ( pv[ i + 1 ].xyz );
            glVertex3fv ( pv[ i + 2 ].xyz );
            glEnd();

          }  /* end if */

      }  /* end if */

      glPopAttrib ();
      if ( newList ) glEndList ();

    } else glCallList ( myDS->dlist );

  }  /* end else */

  // Restore line context
  AWorkspace->SetAspectLine( aspect_line_old );

  if ( texture_on ) EnableTexture ();
}

void draw_degenerates_as_points ( PDS_INTERNAL pd, tel_point p, Tint n, const Handle(OpenGl_Workspace) &AWorkspace )
{
  int i, newList = 0; 

  glDisable(GL_LIGHTING);
  if ( (AWorkspace->NamedStatus & OPENGL_NS_ANIMATION) == 0 )
    goto doDraw; /* Disable object display list out of animation */
  if ( pd -> degMode != 3 || pd -> skipRatio != AWorkspace->SkipRatio || !pd -> dlist ) {

    if ( !pd -> dlist ) pd -> dlist = glGenLists ( 1 );

    pd -> degMode   = 3;
    pd -> skipRatio = AWorkspace->SkipRatio;
    glNewList ( pd -> dlist, GL_COMPILE_AND_EXECUTE );
    newList = 1;

    if ( AWorkspace->SkipRatio <= 0.f ) {
doDraw:
      glBegin ( GL_POINTS );

      for ( i = 0; i < n; ++i ) glVertex3fv ( p[ i ].xyz );

      glEnd ();

    } else if ( AWorkspace->SkipRatio < 1.f ) {

      set_drawable_items ( pd -> bDraw, n, AWorkspace->SkipRatio );

      glBegin ( GL_POINTS );

      for ( i = 0; i < n; ++i )

        if ( pd -> bDraw[ i ] ) glVertex3fv ( p[ i ].xyz );

      glEnd ();

    }  /* end if */
    if ( newList ) glEndList ();

  } else glCallList ( pd -> dlist );

}  /* end draw_degenerates_as_points */

void draw_degenerates_as_bboxs ( PDS_INTERNAL pd, tel_point p, Tint n, const Handle(OpenGl_Workspace) &AWorkspace )
{
  int     i, newList = 0;
  GLfloat minp[ 3 ] = { FLT_MAX, FLT_MAX, FLT_MAX };
  GLfloat maxp[ 3 ] = { FLT_MIN, FLT_MIN, FLT_MIN };

  glDisable(GL_LIGHTING);
  if ( (AWorkspace->NamedStatus & OPENGL_NS_ANIMATION) == 0 )
    goto doDraw; /* Disable object display list out of animation */
  if ( pd -> degMode != 4 || !pd -> dlist ) {

    if ( !pd -> dlist ) pd -> dlist = glGenLists ( 1 );

    pd -> degMode = 4;

    glNewList ( pd -> dlist, GL_COMPILE_AND_EXECUTE );
    newList = 1; 
doDraw:
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

void OpenGl_TriangleStrip::draw_line_loop () const
{
  int i;

  for ( i = 0; i < myData.num_facets; ++i )
  {
    glBegin ( GL_LINE_LOOP );
    glVertex3fv ( myData.vertices[ i     ].xyz );
    glVertex3fv ( myData.vertices[ i + 1 ].xyz );
    glVertex3fv ( myData.vertices[ i + 2 ].xyz );
    glEnd();
  }
}

/*----------------------------------------------------------------------*/

OpenGl_TriangleStrip::OpenGl_TriangleStrip (const Graphic3d_Array1OfVertex& AListVertex)
{
  const Standard_Integer nv = AListVertex.Length();
  TEL_POINT *points = new TEL_POINT[nv];
  memcpy( points, &AListVertex(AListVertex.Lower()), nv*sizeof(TEL_POINT) );

  Init(nv - 2,points,NULL,NULL,NULL,NULL,NULL);
}

/*----------------------------------------------------------------------*/

OpenGl_TriangleStrip::OpenGl_TriangleStrip (const Graphic3d_Array1OfVertexN& AListVertex)
{
  const Standard_Integer nv = AListVertex.Length ();

  // Dynamic allocation
  TEL_POINT *points = new TEL_POINT[nv];
  TEL_POINT *normals = new TEL_POINT[nv];

  Standard_Integer i = 0, j = AListVertex.Lower();
  Standard_Real X, Y, Z;
  for ( ; i < nv; i++, j++)
  {
    AListVertex(j).Coord (X, Y, Z);
    points[i].xyz[0] = float (X);
    points[i].xyz[1] = float (Y);
    points[i].xyz[2] = float (Z);
    AListVertex(j).Normal (X, Y, Z);
    normals[i].xyz[0] = float (X);
    normals[i].xyz[1] = float (Y);
    normals[i].xyz[2] = float (Z);
  }

  Init(nv - 2,points,normals,NULL,NULL,NULL,NULL);
}

/*----------------------------------------------------------------------*/

OpenGl_TriangleStrip::OpenGl_TriangleStrip (const Graphic3d_Array1OfVertexNT& AListVertex)
{
  const Standard_Integer nv = AListVertex.Length();

  // Dynamic allocation
  TEL_POINT *points = new TEL_POINT[nv];
  TEL_POINT *normals = new TEL_POINT[nv];
  TEL_TEXTURE_COORD *tcoords = new TEL_TEXTURE_COORD[nv];

  Standard_Integer i = 0, j = AListVertex.Upper();
  Standard_Real X, Y, Z;
  for ( ; i < nv; i++, j++)
  {
    AListVertex(j).Coord (X, Y, Z);
    points[i].xyz[0] = float (X);
    points[i].xyz[1] = float (Y);
    points[i].xyz[2] = float (Z);
    AListVertex(j).Normal (X, Y, Z);
    normals[i].xyz[0] = float (X);
    normals[i].xyz[1] = float (Y);
    normals[i].xyz[2] = float (Z);
    AListVertex(j).TextureCoordinate(X, Y);
    tcoords[i].xy[0] = float(X);
    tcoords[i].xy[1] = float(Y);
  }

  Init(nv - 2,points,normals,NULL,tcoords,NULL,NULL);
}

/*----------------------------------------------------------------------*/

void OpenGl_TriangleStrip::Init (const Tint ANbFacets, tel_point AVertices,
                                tel_point AVNormals, tel_colour AVColors, tel_texture_coord ATCoords,
                                tel_point AFNormals, tel_colour AFColors)
{
  myData.num_facets = ANbFacets;

  const Tint nv = ANbFacets + 2;

  Tint i;

  // Store vertices
  myData.vertices = AVertices;

  // Store or compute (based on vertices) facet normals
  if (AFNormals)
  {
    myData.facet_flag = TEL_FA_NORMAL;
    myData.fnormals = AFNormals;
    for( i = 0; i < ANbFacets; i++ )
      vecnrm( myData.fnormals[i].xyz );
  }
  else
  {
    myData.facet_flag = TEL_FA_NONE;
    myData.fnormals = new TEL_POINT[ANbFacets];
    for( i = 0; i < ANbFacets; i++ )
    {
      if( i & 1 ) {
        TelGetNormal( myData.vertices[i].xyz, myData.vertices[i+2].xyz, myData.vertices[i+1].xyz, myData.fnormals[i].xyz );
      } else {
        TelGetNormal( myData.vertices[i].xyz, myData.vertices[i+1].xyz, myData.vertices[i+2].xyz, myData.fnormals[i].xyz );
      }
    }
  }

  // Store or compute (based on facet normals) vertex normals
  if (AVNormals)
  {
    myData.vertex_flag = TEL_VT_NORMAL;
    myData.vnormals = AVNormals;
    for( i = 0; i < nv; i++ )
      vecnrm( myData.vnormals[i].xyz );
  }
  else
  {
    myData.vertex_flag = TEL_VT_NONE;
    myData.vnormals = new TEL_POINT[nv];
    for( i = 2; i < ANbFacets; i++ )
    {
      myData.vnormals[i].xyz[0] = ( myData.fnormals[i-2].xyz[0] + myData.fnormals[i-1].xyz[0] + myData.fnormals[i].xyz[0] ) / 3.0F;
      myData.vnormals[i].xyz[1] = ( myData.fnormals[i-2].xyz[1] + myData.fnormals[i-1].xyz[1] + myData.fnormals[i].xyz[1] ) / 3.0F;
      myData.vnormals[i].xyz[2] = ( myData.fnormals[i-2].xyz[2] + myData.fnormals[i-1].xyz[2] + myData.fnormals[i].xyz[2] ) / 3.0F;
    }
    myData.vnormals[0] = myData.fnormals[0];
    if( ANbFacets > 1 )
    {
      myData.vnormals[1].xyz[0] = ( myData.fnormals[0].xyz[0] + myData.fnormals[1].xyz[0] ) / 2.0F;
      myData.vnormals[1].xyz[1] = ( myData.fnormals[0].xyz[1] + myData.fnormals[1].xyz[1] ) / 2.0F;
      myData.vnormals[1].xyz[2] = ( myData.fnormals[0].xyz[2] + myData.fnormals[1].xyz[2] ) / 2.0F;
    }
    else
      myData.vnormals[1] = myData.fnormals[0];
    // last vertex
    myData.vnormals[ANbFacets+1] = myData.fnormals[ANbFacets-1];
    // second last vertex
    if( ANbFacets > 1 )
    {
      myData.vnormals[ANbFacets].xyz[0] = ( myData.fnormals[ANbFacets-1].xyz[0] + myData.fnormals[ANbFacets-2].xyz[0] ) / 2.0F;
      myData.vnormals[ANbFacets].xyz[1] = ( myData.fnormals[ANbFacets-1].xyz[1] + myData.fnormals[ANbFacets-2].xyz[1] ) / 2.0F;
      myData.vnormals[ANbFacets].xyz[2] = ( myData.fnormals[ANbFacets-1].xyz[2] + myData.fnormals[ANbFacets-2].xyz[2] ) / 2.0F;
    }
  }

  myData.vcolours = AVColors;
  myData.vtexturecoord = ATCoords;
  myData.fcolours = AFColors;

  myDS = new DS_INTERNAL();
  myDS->list      =  0;
  myDS->dlist     =  0;
  myDS->degMode   =  0;
  myDS->model     = -1;
  myDS->skipRatio =  0.0F;
  myDS->bDraw = new unsigned char[nv];
}

/*----------------------------------------------------------------------*/

OpenGl_TriangleStrip::~OpenGl_TriangleStrip ()
{
  if( myData.fnormals )
    delete[] myData.fnormals;
  if( myData.fcolours )
    delete[] myData.fcolours;
  if( myData.vertices )
    delete[] myData.vertices;
  if( myData.vcolours )
    delete[] myData.vcolours;
  if( myData.vnormals )
    delete[] myData.vnormals;
  if ( myData.vtexturecoord )
    delete[] myData.vtexturecoord;

  if ( myDS )
  {
    if (  GET_GL_CONTEXT() != NULL  )
    {
      if ( myDS->list ) glDeleteLists ( myDS->list, 1 );
      if ( myDS->dlist ) glDeleteLists ( myDS->dlist, 1 );
    }
    if ( myDS->bDraw )
      delete[] myDS->bDraw;

	delete myDS;
  }
}

/*----------------------------------------------------------------------*/

void OpenGl_TriangleStrip::Render (const Handle(OpenGl_Workspace) &AWorkspace) const
{
  const OpenGl_AspectFace *aspect_face = AWorkspace->AspectFace( Standard_True );

  Tint front_lighting_model = aspect_face->Context().IntFront.color_mask;
  const TEL_COLOUR *interior_colour = &aspect_face->Context().IntFront.matcol;
  const TEL_COLOUR *edge_colour = &aspect_face->AspectEdge()->Color();

  // Use highlight colors
  if ( AWorkspace->NamedStatus & OPENGL_NS_HIGHLIGHT )
  {                         
    edge_colour = interior_colour = AWorkspace->HighlightColor;
    front_lighting_model = 0;
  }

  glColor3fv( interior_colour->rgb );

  draw_tmesh( front_lighting_model,
              aspect_face->Context().InteriorStyle,
              edge_colour,
              AWorkspace );
}

/*----------------------------------------------------------------------*/
