// File:      OpenGl_Mesh.cxx
// Created:   13 July 2011
// Author:    Sergey ZERCHANINOV
// Copyright: OPEN CASCADE 2011

#define OCC749          /* SAV 19/09/02 added processing of colored vertices */
#define OCC7824         /* ASL 26/01/05 transparency of polygon with colors assigned to vertices */

/*----------------------------------------------------------------------*/
/*
* Includes
*/ 

#include <OpenGl_tgl_all.hxx>
#include <GL/gl.h>

#include <OpenGl_Mesh.hxx>

#include <OpenGl_telem_util.hxx>
#include <OpenGl_TextureBox.hxx>

#include <OpenGl_AspectFace.hxx>
#include <OpenGl_Structure.hxx>

#include <float.h>

static long s_Rand = 1L;
# define OPENGL_RAND() ( ( unsigned )( s_Rand = s_Rand * 214013L + 2531011L ) )

/*----------------------------------------------------------------------*/
/*
* Prototypes
*/ 

typedef TEL_INDEXPOLY_DATA* tel_indexpoly_data;

void set_drawable_items ( GLboolean*, int, const float );

/*----------------------------------------------------------------------*/

void OpenGl_Mesh::draw_indexpoly (const Tint front_lighting_model,
                                 const Aspect_InteriorStyle interior_style,
                                 const TEL_COLOUR *edge_colour,
                                 const OPENGL_SURF_PROP *prop,
                                 const Handle(OpenGl_Workspace) &AWorkspace) const
{
  Tint i, j, k, a, newList = 0;
  Tint  lighting_model;

  /* Following pointers have been provided for performance improvement */
  Tint       *ind;
  tel_point  pfn, pvn, pv;
  tel_colour pvc, pfc;
  tel_texture_coord pvt;

  ind = myData.indices;
  pfn = myData.fnormals;
  pvn = myData.vnormals;
  pvc = myData.vcolours;
  pfc = myData.fcolours;
  pv  = myData.vertices;
  pvt = myData.vtexturecoord;
  if ( AWorkspace->DegenerateModel < 2 && interior_style != Aspect_IS_EMPTY )
  {      
    if ( AWorkspace->NamedStatus & OPENGL_NS_HIGHLIGHT )
    {
      pvc = 0;
      pfc = 0;
    }

    if ( interior_style == Aspect_IS_HIDDENLINE)
    {
      pvc = 0;
      pfc = 0;
    }

    if ( front_lighting_model )
      glEnable(GL_LIGHTING);
    else
      glDisable(GL_LIGHTING);

    lighting_model = front_lighting_model;

    if ( (AWorkspace->NamedStatus & OPENGL_NS_ANIMATION) == 0 )
      goto doDraw; /* Disable object display list out of animation */
    /*  window's context and bitmap's one    */

    if ( myDS->model != lighting_model || !myDS->list ||
         myDS->model == -1 || ( AWorkspace->DegenerateModel && myDS->skipRatio != AWorkspace->SkipRatio ))
    {
        myDS->skipRatio = AWorkspace->SkipRatio;
        myDS->model     = lighting_model;
        myDS->degMode   = AWorkspace->DegenerateModel;
        if ( AWorkspace->SkipRatio <= 0.f ) {
          if ( !myDS->list ) myDS->list = glGenLists ( 1 );

          glNewList ( myDS->list, GL_COMPILE_AND_EXECUTE );
          newList = 1;
doDraw:
          if ( !lighting_model )
          {
            if ( myData.num_bounds == 3 )
              glBegin ( GL_TRIANGLES );
            else if ( myData.num_bounds == 4 )
              glBegin ( GL_QUADS );
            else glBegin ( GL_POLYGON );

            if ( pvc )
            {
              for ( i = 0, j = 0, a = 0; i < myData.num_facets; ++i )
              {
                a = j + myData.num_bounds;
                for ( ; j < a; ++j )
                {
                  glColor3fv  ( pvc[  ind[ j ]  ].rgb  );
                  glVertex3fv ( pv[   ind[ j ]  ].xyz  );
                }
              }
            }
            else if ( pfc )
            {
              for ( i = 0, j = 0, a = 0; i < myData.num_facets; ++i )
              {
                a = j + myData.num_bounds;
                glColor3fv ( pfc[ i ].rgb );
                for ( ; j < a; ++j ) glVertex3fv ( pv[  ind[ j ]  ].xyz );
              }
            }
            else
            {
              for ( i = 0, j = 0, a = 0; i < myData.num_facets; ++i )
              {
                a = j + myData.num_bounds;
                for ( ; j < a; ++j ) glVertex3fv ( pv[  ind[ j ]  ].xyz  );
              }
            }
            glEnd ();
          }
          else
          {
            /* lighting_model != TelLModelNone */
            if ( myData.num_bounds == 3 )
              glBegin ( GL_TRIANGLES );
            else if ( myData.num_bounds == 4 )
              glBegin ( GL_QUADS );
            else glBegin ( GL_POLYGON );

#ifdef OCC749
            for ( i = a = 0; i < myData.num_facets; ++i ) {
              j = a; a += myData.num_bounds;
              if( pfn ) glNormal3fv ( pfn[ i ].xyz );
              if( pfc && !prop->isphysic ) {
                GLfloat diff[4], ambi[4], emsv[4], r, g, b;

                ambi[3] = diff[3] = emsv[3] = prop->trans;

                r = pfc[ i ].rgb[0];  g = pfc[ i ].rgb[1];  b = pfc[ i ].rgb[2];

                if( prop->color_mask & OPENGL_AMBIENT_MASK ) {
                  ambi[0] = prop->amb * r;
                  ambi[1] = prop->amb * g;
                  ambi[2] = prop->amb * b;
                  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambi);
                }
                if( prop->color_mask & OPENGL_DIFFUSE_MASK ) {
                  diff[0] = prop->diff * r;
                  diff[1] = prop->diff * g;
                  diff[2] = prop->diff * b;
                  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff);
                }
                if( prop->color_mask & OPENGL_EMISSIVE_MASK ) {
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

                  ambi[3] = diff[3] = emsv[3] = prop->trans;

                  r = pvc[ k ].rgb[0];  g = pvc[ k ].rgb[1];  b = pvc[ k ].rgb[2];

                  if( prop->color_mask & OPENGL_AMBIENT_MASK ) {
                    ambi[0] = prop->amb * r;
                    ambi[1] = prop->amb * g;
                    ambi[2] = prop->amb * b;
                    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambi);
                  }
                  if( prop->color_mask & OPENGL_DIFFUSE_MASK ) {
                    diff[0] = prop->diff * r;
                    diff[1] = prop->diff * g;
                    diff[2] = prop->diff * b;
                    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff);
                  }
                  if( prop->color_mask & OPENGL_EMISSIVE_MASK ) {
                    emsv[0] = prop->emsv * r;
                    emsv[1] = prop->emsv * g;
                    emsv[2] = prop->emsv * b;
                    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emsv);
                  }
                }
                if( pvt && (AWorkspace->NamedStatus & OPENGL_NS_FORBIDSETTEX) == 0 ) glTexCoord2fv ( pvt[ k ].xy  );
                glVertex3fv ( pv[ k ].xyz );
              }  /* end for ( j . . . ) */
            }  /* end for ( i . . . ) */
            glEnd ();  
#else
            if ( pvn ) {
              if ( pvt && (AWorkspace->NamedStatus & OPENGL_NS_FORBIDSETTEX) == 0 )
                for ( i = 0, j = 0, a = 0; i < myData.num_facets; ++i ) {
                  a = j + myData.num_bounds;
                  for ( ; j < a; ++j ) {
                    glNormal3fv   ( pvn[  ind[ j ]  ].xyz );
                    glTexCoord2fv ( pvt[  ind[ j ]  ].xy  );
                    glVertex3fv   ( pv[   ind[ j ]  ].xyz );
                  }  /* end for ( j . . . ) */
                }  /* end for ( i . . . ) */
              else
                for ( i = 0, j = 0, a = 0; i < myData.num_facets; ++i ) {
                  a = j + myData.num_bounds;
                  for ( ; j < a; ++j ) {
                    glNormal3fv ( pvn[  ind[ j ]  ].xyz  );
                    glVertex3fv ( pv[   ind[ j ]  ].xyz  );
                  }  /* end for ( j . . . ) */
                }  /* end for ( i . . . ) */
            } else { /* !pvn */
              for ( i = 0, j = 0, a = 0; i < myData.num_facets; ++i ) {
                a = j + myData.num_bounds;
                glNormal3fv ( pfn[ i ].xyz );
                for ( ; j < a; ++j ) glVertex3fv ( pv[  ind[ j ]  ].xyz  );
              }  /* end for */
            }  /* end else */
            glEnd ();  
#endif /* OCC749 */

          }  /* end else */
        } else if ( AWorkspace->SkipRatio < 1.f ) {
          if ( !myDS->dlist ) myDS->dlist = glGenLists ( 1 );
          glNewList ( myDS->dlist, GL_COMPILE_AND_EXECUTE );
          newList = 1;
          set_drawable_items ( myDS->bDraw, myData.num_facets, AWorkspace->SkipRatio );
          if ( !lighting_model ) {
            if ( myData.num_bounds == 3 )
              glBegin ( GL_TRIANGLES );
            else if ( myData.num_bounds == 4 )
              glBegin ( GL_QUADS );
            else glBegin ( GL_POLYGON );

            if ( pvc ) {
              for ( i = 0, j = 0, a = 0; i < myData.num_facets; ++i ) {
                a = j + myData.num_bounds;
                if ( myDS->bDraw[ i ] )
                  for ( ; j < a; ++j ) {
                    glColor3fv  ( pvc[  ind[ j ]  ].rgb  );
                    glVertex3fv ( pv[   ind[ j ]  ].xyz  );
                  }  /* end for ( j . . . ) */
                else j = a;
              }  /* end for ( i . . . ) */
            } else if ( pfc ) {
              for ( i = 0, j = 0, a = 0; i < myData.num_facets; ++i ) {
                a = j + myData.num_bounds;
                if ( myDS->bDraw[ i ] ) {
                  glColor3fv ( pfc[ i ].rgb );
                  for ( ; j < a; ++j ) glVertex3fv ( pv[  ind[ j ]  ].xyz );
                } else j = a;
              }  /* end for */
            } else {
              for ( i = 0, j = 0, a = 0; i < myData.num_facets; ++i ) {
                a = j + myData.num_bounds;
                if ( myDS->bDraw[ i ] )
                  for ( ; j < a; ++j ) glVertex3fv ( pv[  ind[ j ]  ].xyz  );
                else j = a;
              }  /* end for */
            }  /* end else */
            glEnd ();
          } else {   /* lighting_model != TelLModelNone */
            if ( myData.num_bounds == 3 )
              glBegin ( GL_TRIANGLES );
            else if ( myData.num_bounds == 4 )
              glBegin ( GL_QUADS );
            else glBegin ( GL_POLYGON );

            if ( pvn ) {
              if ( pvt && (AWorkspace->NamedStatus & OPENGL_NS_FORBIDSETTEX) == 0 )
                for ( i = 0, j = 0, a = 0; i < myData.num_facets; ++i ) {
                  a = j + myData.num_bounds;
                  if ( myDS->bDraw[ i ] )
                    for ( ; j < a; ++j ) {
                      glNormal3fv   ( pvn[  ind[ j ]  ].xyz );
                      glTexCoord2fv ( pvt[  ind[ j ]  ].xy  );
                      glVertex3fv   ( pv[   ind[ j ]  ].xyz );
                    }  /* end for ( j . . . ) */
                  else j = a;
                }  /* end for ( i . . . ) */
              else
                for ( i = 0, j = 0, a = 0; i < myData.num_facets; ++i ) {
                  a = j + myData.num_bounds;
                  if ( myDS->bDraw[ i ] )
                    for ( ; j < a; ++j ) {
                      glNormal3fv ( pvn[  ind[ j ]  ].xyz  );
                      glVertex3fv ( pv[   ind[ j ]  ].xyz  );
                    }  /* end for ( j . . . ) */
                  else j = a;
                }  /* end for ( i . . . ) */
            } else {  /* !pvn */
              for ( i = 0, j = 0, a = 0; i < myData.num_facets; ++i ) {
                a = j + myData.num_bounds;
                if ( myDS->bDraw[ i ] ) {
                  glNormal3fv ( pfn[ i ].xyz );
                  for ( ; j < a; ++j ) glVertex3fv ( pv[  ind[ j ]  ].xyz  );
                } else j = a;
              }  /* end for */
            }  /* end else */
            glEnd ();  
          }  /* end else */
        } else {
          if ( !myDS->dlist ) myDS->dlist = glGenLists ( 1 );
          glNewList ( myDS->dlist, GL_COMPILE_AND_EXECUTE );
          newList = 1;
        }  /* end else */
        if ( newList ) glEndList ();
        if ( AWorkspace->DegenerateModel ) return;
      } else {
        glCallList ( AWorkspace->SkipRatio <= 0.f ? myDS->list : myDS->dlist );
        if ( AWorkspace->DegenerateModel ) return;
      }  /* end else */
  }

  i = 0;

  switch ( AWorkspace->DegenerateModel )
  {
    default:
      break;

    case 2:  /* XXX_TDM_WIREFRAME */
      i = 1;
      break;

    case 3:  /* XXX_TDM_MARKER */
      draw_degenerates_as_points ( AWorkspace->SkipRatio );
      return;

    case 4:  /* XXX_TDM_BBOX */
      draw_degenerates_as_bboxs ();
      return;
  }

  draw_edges ( edge_colour, interior_style, i, AWorkspace );
}

/*----------------------------------------------------------------------*/

void OpenGl_Mesh::draw_edges (const TEL_COLOUR *edge_colour, const Aspect_InteriorStyle interior_style,
                             Tint forceDraw, const Handle(OpenGl_Workspace) &AWorkspace) const
{
  const OpenGl_AspectFace *aspect_face = AWorkspace->AspectFace( Standard_True );

  if ( interior_style != Aspect_IS_HIDDENLINE && !forceDraw && aspect_face->Context().Edge == TOff )
    return;

  glDisable(GL_LIGHTING);
  const GLboolean texture_on = IsTextureEnabled();
  if (texture_on) DisableTexture();

  // Setup line aspect
  const OpenGl_AspectLine *aspect_line_old = AWorkspace->SetAspectLine( aspect_face->AspectEdge() );
  AWorkspace->AspectLine( Standard_True );

  tel_point pv = myData.vertices;
  Tint *ind = myData.indices;
  Tint *vis = myData.edge_vis;

  Tint i, j, a, newList = 0;

  if ( !forceDraw ) {

    glColor3fv    ( edge_colour -> rgb         );
    glPushAttrib  ( GL_POLYGON_BIT             );
    glPolygonMode ( GL_FRONT_AND_BACK, GL_LINE );

    for ( i = 0, j = 0, a = 0; i < myData.num_facets; ++i ) {

      a = j + myData.num_bounds;

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

    if ( (AWorkspace->NamedStatus & OPENGL_NS_ANIMATION) == 0 ) 
      goto doDraw; /* Disable object display list out of animation */

    if ( myDS->degMode != 2 || myDS->skipRatio != AWorkspace->SkipRatio || !myDS->dlist ) {

      if ( !myDS->dlist ) myDS->dlist = glGenLists ( 1 );

      myDS->degMode   = 2;
      myDS->skipRatio = AWorkspace->SkipRatio;
      glNewList ( myDS->dlist, GL_COMPILE_AND_EXECUTE );
      newList = 1;

doDraw:
      glPushAttrib ( GL_POLYGON_BIT | GL_DEPTH_BUFFER_BIT );

      glEdgeFlag    ( GL_TRUE                    );
      glDisable     ( GL_DEPTH_TEST              );
      glPolygonMode ( GL_FRONT_AND_BACK, GL_LINE );

      if ( AWorkspace->SkipRatio <= 0.f )

        for ( i = 0, j = 0, a = 0; i < myData.num_facets; ++i ) {

          a = j + myData.num_bounds;

          glBegin ( GL_POLYGON );

          for ( ; j < a; ++j ) glVertex3fv ( pv[  ind[ j ]  ].xyz );

          glEnd();

        }  /* end for */

      else if ( AWorkspace->SkipRatio < 1.f ) {

        set_drawable_items ( myDS->bDraw, myData.num_facets, AWorkspace->SkipRatio );

        for ( i = 0, j = 0, a = 0; i < myData.num_facets; ++i ) {

          a = j + myData.num_bounds;

          if ( myDS->bDraw[ i ] ) {

            glBegin ( GL_POLYGON );

            for ( ; j < a; ++j ) glVertex3fv ( pv[  ind[ j ]  ].xyz );

            glEnd();

          } else j = a;

        }  /* end for */

      }  /* end if */

      glPopAttrib ();
      if ( newList ) glEndList ();

    } else glCallList ( myDS->dlist );

  }  /* end else */

  // Restore line context
  AWorkspace->SetAspectLine( aspect_line_old );

  if (texture_on) EnableTexture();
}

/*----------------------------------------------------------------------*/

void OpenGl_Mesh::draw_degenerates_as_points (const float aSkipRatio) const
{
  Tint*      ind, *vis;
  Tint       i, j, n, a, newList = 0;
  GLfloat    pt[ 3 ];
  tel_point  pv;

  pv  = myData.vertices;
  ind = myData.indices;
  vis = myData.edge_vis;

  glDisable(GL_LIGHTING);

  if ( myDS->degMode != 3 || myDS->skipRatio != aSkipRatio || !myDS->dlist ) {

    if ( !myDS->dlist ) myDS->dlist = glGenLists ( 1 );

    myDS->degMode   = 3;
    myDS->skipRatio = aSkipRatio;
    glNewList ( myDS->dlist, GL_COMPILE_AND_EXECUTE );
    newList = 1;

    if ( aSkipRatio <= 0.f ) {

      glBegin ( GL_POINTS );

      for ( i = 0, j = 0, a = 0; i < myData.num_facets; ++i ) {

        n = myData.num_bounds;
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

    } else if ( aSkipRatio < 1.f ) {

      set_drawable_items ( myDS->bDraw, myData.num_facets, aSkipRatio );

      glBegin ( GL_POINTS );

      for ( i = 0, j = 0, a = 0; i < myData.num_facets; ++i ) {

        n = myData.num_bounds;
        a = j + n;

        if ( myDS->bDraw[ i ] ) {

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

    glEndList ();

  } else glCallList ( myDS->dlist );

}

void OpenGl_Mesh::draw_degenerates_as_bboxs () const
{
  Tint*     ind, *vis;
  Tint      i, j, n, a, newList = 0;
  GLfloat   minp[ 3 ] = { FLT_MAX, FLT_MAX, FLT_MAX };
  GLfloat   maxp[ 3 ] = { FLT_MIN, FLT_MIN, FLT_MIN };
  tel_point pv;

  pv  = myData.vertices;
  ind = myData.indices;
  vis = myData.edge_vis;

  glDisable(GL_LIGHTING);

  if ( myDS->degMode != 4 || !myDS->dlist ) {

    if ( !myDS->dlist ) myDS->dlist = glGenLists ( 1 );

    myDS->degMode = 4;

    glNewList ( myDS->dlist, GL_COMPILE_AND_EXECUTE );
    newList = 1;

    for ( i = 0, j = 0, a = 0; i < myData.num_facets; ++i ) {

      n = myData.num_bounds;
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
    glEndList ();

  } else glCallList ( myDS->dlist );
}

/*----------------------------------------------------------------------*/

void set_drawable_items ( GLboolean* pbDraw, int n, const float aSkipRatio )
{
  memset (  pbDraw, 0, sizeof ( GLboolean ) * n  );

  int i = ( int )(  ( 1.0F - aSkipRatio ) * n  );

  while ( i-- ) pbDraw[ OPENGL_RAND() % n ] = 1;
}

/*----------------------------------------------------------------------*/

OpenGl_Mesh::OpenGl_Mesh (const Graphic3d_Array1OfVertex& AListVertex, const Aspect_Array1OfEdge& AListEdge)
{
  const Standard_Integer nv = AListVertex.Length();

  // Dynamic allocation
  TEL_POINT *points = new TEL_POINT[nv];
  memcpy( points, &AListVertex(AListVertex.Lower()), nv*sizeof(TEL_POINT) );

  Init (nv,points,NULL,NULL,NULL,AListEdge,3);
}

/*----------------------------------------------------------------------*/

OpenGl_Mesh::OpenGl_Mesh (const Graphic3d_Array1OfVertexN& AListVertex, const Aspect_Array1OfEdge& AListEdge)
{
  const Standard_Integer nv = AListVertex.Length ();

  // Dynamic allocation
  TEL_POINT *points = new TEL_POINT[nv];
  TEL_POINT *normals = new TEL_POINT[nv];

  Standard_Integer i = 0, j = AListVertex.Lower();
  Standard_Real X, Y, Z;
  for ( ; i < nv; i++, j++)
  {
    AListVertex(j).Coord(X, Y, Z);
    points[i].xyz[0] = float (X);
    points[i].xyz[1] = float (Y);
    points[i].xyz[2] = float (Z);
    AListVertex(j).Normal(X, Y, Z);
    normals[i].xyz[0] = float (X);
    normals[i].xyz[1] = float (Y);
    normals[i].xyz[2] = float (Z);
  }

  Init (nv,points,normals,NULL,NULL,AListEdge,3);
}

/*----------------------------------------------------------------------*/

OpenGl_Mesh::OpenGl_Mesh (const Graphic3d_Array1OfVertexC& AListVertex, const Aspect_Array1OfEdge& AListEdge)
{
  const Standard_Integer nv = AListVertex.Length ();

  // Dynamic allocation
  TEL_POINT *points = new TEL_POINT[nv];
  TEL_COLOUR *colors = new TEL_COLOUR[nv];

  Standard_Integer i = 0, j = AListVertex.Lower();
  Standard_Real X, Y, Z;
  for ( ; i < nv; i++, j++)
  {
    AListVertex(j).Coord(X, Y, Z);
    points[i].xyz[0] = float (X);
    points[i].xyz[1] = float (Y);
    points[i].xyz[2] = float (Z);
    AListVertex(j).Color().Values (X, Y, Z, Quantity_TOC_RGB);
    colors[i].rgb[0] = float (X);
    colors[i].rgb[1] = float (Y);
    colors[i].rgb[2] = float (Z);
    colors[i].rgb[3] = 1.0F;
  }

  Init (nv,points,NULL,colors,NULL,AListEdge,3);
}

/*----------------------------------------------------------------------*/

OpenGl_Mesh::OpenGl_Mesh (const Graphic3d_Array1OfVertexNC& AListVertex, const Aspect_Array1OfEdge& AListEdge)
{
  const Standard_Integer nv = AListVertex.Length ();

  // Dynamic allocation
  TEL_POINT *points = new TEL_POINT[nv];
  TEL_POINT *normals = new TEL_POINT[nv];
  TEL_COLOUR *colors = new TEL_COLOUR[nv];

  Standard_Integer i = 0, j = AListVertex.Lower();
  Standard_Real X, Y, Z;
  for ( ; i < nv; i++, j++)
  {
    AListVertex(j).Coord(X, Y, Z);
    points[i].xyz[0] = float (X);
    points[i].xyz[1] = float (Y);
    points[i].xyz[2] = float (Z);
    AListVertex(j).Normal(X, Y, Z);
    normals[i].xyz[0] = float (X);
    normals[i].xyz[1] = float (Y);
    normals[i].xyz[2] = float (Z);
    AListVertex(j).Color().Values (X, Y, Z, Quantity_TOC_RGB);
    colors[i].rgb[0] = float (X);
    colors[i].rgb[1] = float (Y);
    colors[i].rgb[2] = float (Z);
    colors[i].rgb[3] = 1.0F;
  }

  Init (nv,points,normals,colors,NULL,AListEdge,3);
}

/*----------------------------------------------------------------------*/

OpenGl_Mesh::OpenGl_Mesh (const Graphic3d_Array1OfVertexNT& AListVertex, const Aspect_Array1OfEdge& AListEdge)
{
  const Standard_Integer nv = AListVertex.Length ();

  // Dynamic allocation
  TEL_POINT *points = new TEL_POINT[nv];
  TEL_POINT *normals = new TEL_POINT[nv];
  TEL_TEXTURE_COORD *tcoords = new TEL_TEXTURE_COORD[nv];

  Standard_Integer i = 0, j = AListVertex.Lower();
  Standard_Real X, Y, Z;
  for ( ; i < nv; i++, j++)
  {
    AListVertex(j).Coord(X, Y, Z);
    points[i].xyz[0] = float (X);
    points[i].xyz[1] = float (Y);
    points[i].xyz[2] = float (Z);
    AListVertex(j).Normal(X, Y, Z);
    normals[i].xyz[0] = float (X);
    normals[i].xyz[1] = float (Y);
    normals[i].xyz[2] = float (Z);
    AListVertex(j).TextureCoordinate(X, Y);
    tcoords[i].xy[0] = float(X);
    tcoords[i].xy[1] = float(Y);
  }

  Init (nv,points,normals,NULL,tcoords,AListEdge,3);
}

/*----------------------------------------------------------------------*/

void OpenGl_Mesh::Init (const Tint ANbVertices, tel_point AVertices,
                       tel_point AVNormals, tel_colour AVColors, tel_texture_coord ATCoords,
                       const Aspect_Array1OfEdge& AListEdge, const Tint ANbBounds)
{
  // Get number of bounds in a facet
  myData.num_bounds = ANbBounds;

  // Get number of vertices
  myData.num_vertices = ANbVertices;

  // Get vertices
  myData.vertices = AVertices;

  // Get number of edges
  const Standard_Integer nb_edges = AListEdge.Length ();

  myData.indices = new Tint[nb_edges];
  myData.edge_vis = new Tint[nb_edges];

  const Standard_Integer LowerE = AListEdge.Lower ();
  const Standard_Integer UpperE = AListEdge.Upper ();

  // Loop on edges
  Standard_Integer i, j;
  for (j=0, i=LowerE; i<=UpperE; i++, j++)
  {
    myData.indices[j] = AListEdge(i).FirstIndex() - LowerE; //LastIndex unused
    myData.edge_vis[j]  = AListEdge(i).Type() ? TOff : TOn;
  }

  // Get number of facets
  myData.num_facets = nb_edges / ANbBounds;

  myData.vnormals = AVNormals;
  if (AVNormals)
  {
    myData.vertex_flag = TEL_VT_NORMAL;
    for( i = 0; i < ANbVertices; i++ )
      vecnrm( myData.vnormals[i].xyz );
  }
  else
  {
    myData.vertex_flag = TEL_VT_NONE;
  }

  myData.vcolours = AVColors;
  myData.vtexturecoord = ATCoords;

  myData.facet_flag = TEL_FA_NONE;
  myData.fnormals = new TEL_POINT[myData.num_facets];
  for( i = 0, j = 0; i < myData.num_facets; i++ )
  {
    TelGetPolygonNormal( myData.vertices, &myData.indices[j], myData.num_bounds, myData.fnormals[i].xyz );
    j += myData.num_bounds;
  }

  myData.fcolours = NULL;

  myDS = new DS_INTERNAL();
  myDS->list      =  0;
  myDS->dlist     =  0;
  myDS->degMode   =  0;
  myDS->model     = -1;
  myDS->skipRatio =  0.0F;
  myDS->bDraw = new unsigned char[myData.num_facets];
}

/*----------------------------------------------------------------------*/

OpenGl_Mesh::~OpenGl_Mesh ()
{
  if( myData.edge_vis )
    delete[] myData.edge_vis;
  if( myData.indices )
    delete[] myData.indices;
  if( myData.fcolours )
    delete[] myData.fcolours;
  if( myData.fnormals )
    delete[] myData.fnormals;
  if( myData.vertices )
    delete[] myData.vertices;
  if( myData.vcolours )
    delete[] myData.vcolours;
  if( myData.vnormals )
    delete[] myData.vnormals;
  if( myData.vtexturecoord )
    delete[] myData.vtexturecoord;

  if ( myDS )
  {
    if ( GET_GL_CONTEXT() != NULL )
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

void OpenGl_Mesh::Render (const Handle(OpenGl_Workspace) &AWorkspace) const
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

  draw_indexpoly( front_lighting_model,
                  aspect_face->Context().InteriorStyle,
                  edge_colour,
                  &aspect_face->Context().IntFront,
                  AWorkspace );
}

/*----------------------------------------------------------------------*/
