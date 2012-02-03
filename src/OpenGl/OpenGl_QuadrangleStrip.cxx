// File:      OpenGl_QuadrangleStrip.cxx
// Created:   13 July 2011
// Author:    Sergey ZERCHANINOV
// Copyright: OPEN CASCADE 2011

#include <OpenGl_tgl_all.hxx>
#include <GL/gl.h>

#include <OpenGl_QuadrangleStrip.hxx>

#include <OpenGl_telem_util.hxx>
#include <OpenGl_TextureBox.hxx>

#include <OpenGl_AspectFace.hxx>
#include <OpenGl_Structure.hxx>

/*----------------------------------------------------------------------*/

#define OPENGL_RAND() ( ( unsigned )( s_Rand = s_Rand * 214013L + 2531011L ) )

/*----------------------------------------------------------------------*/

extern void draw_degenerates_as_points ( PDS_INTERNAL, tel_point, Tint, const Handle(OpenGl_Workspace) & );
extern void draw_degenerates_as_bboxs  ( PDS_INTERNAL, tel_point, Tint, const Handle(OpenGl_Workspace) & );
extern void set_drawable_items         ( GLboolean*, int, const float            );

/*----------------------------------------------------------------------*/

void OpenGl_QuadrangleStrip::draw_qstrip (const Tint front_lighting_model,
                                         const Aspect_InteriorStyle interior_style,
                                         const TEL_COLOUR *edge_colour,
                                         const Handle(OpenGl_Workspace) &AWorkspace) const
{
  Tint              i, m, newList = 0;
  Tint              nf, nv, nr, nc;
  Tint              lighting_model;

  tel_point pfn = myData.fnormals;
  tel_colour pfc = myData.fcolours;
  tel_point pv  = myData.vertices;
  tel_colour pvc = myData.vcolours;
  tel_point pvn = myData.vnormals;
  tel_texture_coord pvt = myData.vtexturecoord;

  if ( AWorkspace->NamedStatus & OPENGL_NS_HIGHLIGHT )
  {
    pvc = 0;
    pfc = 0;
  }

  if ( AWorkspace->DegenerateModel < 2 && interior_style != Aspect_IS_EMPTY )
  {
    if ( front_lighting_model )
      glEnable(GL_LIGHTING);
    else
      glDisable(GL_LIGHTING);

    lighting_model = front_lighting_model;
    nr             = myData.num_rows;
    nc             = myData.num_columns;
    nf             = nr * nc;
    nv             = ( nr + 1 ) * ( nc + 1 );
    if ( (AWorkspace->NamedStatus & OPENGL_NS_ANIMATION) == 0 )
      goto doDraw; /* Disable object display list out of animation */
    /*  window's context and bitmap's one   */
    if ( myDS->model != front_lighting_model || !myDS->list || myDS->model == -1 ||
        ( AWorkspace->DegenerateModel && myDS->skipRatio != AWorkspace->SkipRatio ) )
    {
        myDS->skipRatio = AWorkspace->SkipRatio;
        myDS->model     = front_lighting_model;
        myDS->degMode   = AWorkspace->DegenerateModel;

        if ( AWorkspace->SkipRatio == 0.f ) {

          if ( !myDS->list ) myDS->list = glGenLists ( 1 );

          glNewList ( myDS->list, GL_COMPILE_AND_EXECUTE );
          newList = 1;
doDraw:
          if ( lighting_model )
          {
            if ( pvt && (AWorkspace->NamedStatus & OPENGL_NS_FORBIDSETTEX) == 0 )
            {
              for ( m = 0; m < nr; ++m )
              {
                glBegin ( GL_QUAD_STRIP );
                glNormal3fv   ( pvn[         m * ( nc + 1 ) ].xyz );
                glTexCoord2fv ( pvt[         m * ( nc + 1 ) ].xy  );
                glVertex3fv   ( pv[          m * ( nc + 1 ) ].xyz );
                glNormal3fv   ( pvn[ ( m + 1 ) * ( nc + 1 ) ].xyz );
                glTexCoord2fv ( pvt[ ( m + 1 ) * ( nc + 1 ) ].xy  );
                glVertex3fv   ( pv[  ( m + 1 ) * ( nc + 1 ) ].xyz );
                for ( i = 1; i < nc + 1; ++i )
                {
                  glNormal3fv   ( pvn[         i + m * ( nc + 1 ) ].xyz );
                  glTexCoord2fv ( pvt[         i + m * ( nc + 1 ) ].xy  );
                  glVertex3fv   ( pv[          i + m * ( nc + 1 ) ].xyz );
                  glNormal3fv   ( pvn[ i + ( m + 1 ) * ( nc + 1 ) ].xyz );
                  glTexCoord2fv ( pvt[ i + ( m + 1 ) * ( nc + 1 ) ].xy  );
                  glVertex3fv   ( pv[  i + ( m + 1 ) * ( nc + 1 ) ].xyz );
                }
                glEnd ();
              }
            }
            else
            {
              for ( m = 0; m < nr; ++m )
              {
                glBegin ( GL_QUAD_STRIP );
                glNormal3fv ( pvn[         m * ( nc + 1 ) ].xyz );
                glVertex3fv ( pv[          m * ( nc + 1 ) ].xyz );
                glNormal3fv ( pvn[ ( m + 1 ) * ( nc + 1 ) ].xyz );
                glVertex3fv ( pv[  ( m + 1 ) * ( nc + 1 ) ].xyz );
                for ( i = 1; i < nc + 1; ++i )
                {
                  glNormal3fv ( pvn[         i + m * ( nc + 1 ) ].xyz );
                  glVertex3fv ( pv[          i + m * ( nc + 1 ) ].xyz );
                  glNormal3fv ( pvn[ i + ( m + 1 ) * ( nc + 1 ) ].xyz );
                  glVertex3fv ( pv[  i + ( m + 1 ) * ( nc + 1 ) ].xyz );
                }
                glEnd ();
              }
            }
          }
          else
          {
            if ( pvc )
            {
              for ( m = 0; m < nr; ++m )
              {
                glBegin ( GL_QUAD_STRIP );
                glColor3fv  ( pvc[         m * ( nc + 1 ) ].rgb );
                glVertex3fv ( pv[          m * ( nc + 1 ) ].xyz );
                glColor3fv  ( pvc[ ( m + 1 ) * ( nc + 1 ) ].rgb );
                glVertex3fv ( pv[  ( m + 1 ) * ( nc + 1 ) ].xyz );
                for ( i = 1; i < nc + 1; ++i )
                {
                  glColor3fv  ( pvc[         i + m * ( nc + 1 ) ].rgb );
                  glVertex3fv ( pv[          i + m * ( nc + 1 ) ].xyz );
                  glColor3fv  ( pvc[ i + ( m + 1 ) * ( nc + 1 ) ].rgb );
                  glVertex3fv ( pv[  i + ( m + 1 ) * ( nc + 1 ) ].xyz );
                }
                glEnd ();
              }
            }
            else if ( pfc )
            {
              glColor3fv ( pfc[ 0 ].rgb );
              for ( m = 0; m < nr; ++m )
              {
                glBegin ( GL_QUAD_STRIP );
                glColor3fv  ( pfc[                m * nc ].rgb );
                glVertex3fv ( pv[         m * ( nc + 1 ) ].xyz );
                glVertex3fv ( pv[ ( m + 1 ) * ( nc + 1 ) ].xyz );
                for ( i = 1; i < nc + 1; ++i )
                {
                  glVertex3fv ( pv[         i + m * ( nc + 1 ) ].xyz );
                  glColor3fv  ( pfc[            i - 1 + m * nc ].rgb );
                  glVertex3fv ( pv[ i + ( m + 1 ) * ( nc + 1 ) ].xyz );
                }
                glEnd ();
              }
            }
            else
            {
              for ( m = 0; m < nr; ++m )
              {
                glBegin ( GL_QUAD_STRIP );
                glVertex3fv ( pv[         m * ( nc + 1 ) ].xyz );
                glVertex3fv ( pv[ ( m + 1 ) * ( nc + 1 ) ].xyz );
                for ( i = 1; i < nc + 1; ++i )
                {
                  glVertex3fv( pv[         i + m * ( nc + 1 ) ].xyz );
                  glVertex3fv( pv[ i + ( m + 1 ) * ( nc + 1 ) ].xyz );
                }
                glEnd ();
              }
            }
          }
        }
        else if ( AWorkspace->SkipRatio < 1.f )
        {
          set_drawable_items ( myDS->bDraw, nr, AWorkspace->SkipRatio );

          if ( !myDS->dlist ) myDS->dlist = glGenLists ( 1 );

          glNewList ( myDS->dlist, GL_COMPILE_AND_EXECUTE );
          newList = 1;

          if ( lighting_model )
          {
            if ( pvt && (AWorkspace->NamedStatus & OPENGL_NS_FORBIDSETTEX) == 0 )
            {
              for ( m = 0; m < nr; ++m )
              {
                if ( myDS->bDraw[ m ] )
                {
                  glBegin ( GL_QUADS );
                  glNormal3fv   ( pvn[         m * ( nc + 1 ) ].xyz );
                  glTexCoord2fv ( pvt[         m * ( nc + 1 ) ].xy  );
                  glVertex3fv   ( pv[          m * ( nc + 1 ) ].xyz );
                  glNormal3fv   ( pvn[ ( m + 1 ) * ( nc + 1 ) ].xyz );
                  glTexCoord2fv ( pvt[ ( m + 1 ) * ( nc + 1 ) ].xy  );
                  glVertex3fv   ( pv[  ( m + 1 ) * ( nc + 1 ) ].xyz );
                  for ( i = 1; i < nc + 1; ++i )
                  {
                    glNormal3fv   ( pvn[ i + ( m + 1 ) * ( nc + 1 ) ].xyz );
                    glTexCoord2fv ( pvt[ i + ( m + 1 ) * ( nc + 1 ) ].xy  );
                    glVertex3fv   ( pv[  i + ( m + 1 ) * ( nc + 1 ) ].xyz );
                    glNormal3fv   ( pvn[         i + m * ( nc + 1 ) ].xyz );
                    glTexCoord2fv ( pvt[         i + m * ( nc + 1 ) ].xy  );
                    glVertex3fv   ( pv[          i + m * ( nc + 1 ) ].xyz );
                  }
                  glEnd ();
                }
              }
            }
            else
            {
              for ( m = 0; m < nr; ++m )
			  {
                if ( myDS->bDraw[ m ] )
                {
                  glBegin ( GL_QUADS );
                  glNormal3fv ( pvn[         m * ( nc + 1 ) ].xyz );
                  glVertex3fv ( pv[          m * ( nc + 1 ) ].xyz );
                  glNormal3fv ( pvn[ ( m + 1 ) * ( nc + 1 ) ].xyz );
                  glVertex3fv ( pv[  ( m + 1 ) * ( nc + 1 ) ].xyz );
                  for ( i = 1; i < nc + 1; ++i )
                  {
                    glNormal3fv ( pvn[ i + ( m + 1 ) * ( nc + 1 ) ].xyz );
                    glVertex3fv ( pv[  i + ( m + 1 ) * ( nc + 1 ) ].xyz );
                    glNormal3fv ( pvn[         i + m * ( nc + 1 ) ].xyz );
                    glVertex3fv ( pv[          i + m * ( nc + 1 ) ].xyz );
                  }
                  glEnd ();
                }
              }
            }
          }
          else
          {
            if ( pvc )
            {
              for ( m = 0; m < nr; ++m )
              {
                if ( myDS->bDraw[ m ] )
                {
                  glBegin ( GL_QUAD_STRIP );
                  glColor3fv  ( pvc[         m * ( nc + 1 ) ].rgb );
                  glVertex3fv ( pv[          m * ( nc + 1 ) ].xyz );
                  glColor3fv  ( pvc[ ( m + 1 ) * ( nc + 1 ) ].rgb );
                  glVertex3fv ( pv[  ( m + 1 ) * ( nc + 1 ) ].xyz );
                  for ( i = 1; i < nc + 1; ++i )
                  {
                    glColor3fv  ( pvc[ i + ( m + 1 ) * ( nc + 1 ) ].rgb );
                    glVertex3fv ( pv[  i + ( m + 1 ) * ( nc + 1 ) ].xyz );
                    glColor3fv  ( pvc[         i + m * ( nc + 1 ) ].rgb );
                    glVertex3fv ( pv[          i + m * ( nc + 1 ) ].xyz );
                  }
                  glEnd ();
                }
              }
            }
            else if ( pfc )
            {
              glColor3fv ( pfc[ 0 ].rgb );
              for ( m = 0; m < nr; ++m )
              {
                if ( myDS->bDraw[ m ] )
                {
                  glBegin ( GL_QUADS );
                  glColor3fv  ( pfc[                m * nc ].rgb );
                  glVertex3fv ( pv[         m * ( nc + 1 ) ].xyz );
                  glVertex3fv ( pv[ ( m + 1 ) * ( nc + 1 ) ].xyz );
                  for ( i = 1; i < nc + 1; ++i )
                  {
                    glColor3fv  ( pfc[            i - 1 + m * nc ].rgb );
                    glVertex3fv ( pv[ i + ( m + 1 ) * ( nc + 1 ) ].xyz );
                    glVertex3fv ( pv[         i + m * ( nc + 1 ) ].xyz );
                  }
                  glEnd ();
                }
              }
            }
            else
            {
              for ( m = 0; m < nr; ++m )
              {
                if ( myDS->bDraw[ m ] )
                {
                  glBegin ( GL_QUADS );
                  glVertex3fv ( pv[         m * ( nc + 1 ) ].xyz );
                  glVertex3fv ( pv[ ( m + 1 ) * ( nc + 1 ) ].xyz );
                  for ( i = 1; i < nc + 1; ++i )
                  {
                    glVertex3fv( pv[ i + ( m + 1 ) * ( nc + 1 ) ].xyz );
                    glVertex3fv( pv[         i + m * ( nc + 1 ) ].xyz );
                  }
                  glEnd ();
                }
              }
            }
          }
        }
        else
        {
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
      draw_degenerates_as_points ( myDS, myData.vertices, ( myData.num_rows + 1 ) * ( myData.num_columns + 1 ), AWorkspace );
      glPopAttrib();
      return;

    case 4:  /* XXX_TDM_BBOX */
      draw_degenerates_as_bboxs ( myDS, myData.vertices, ( myData.num_rows + 1 ) * ( myData.num_columns + 1 ), AWorkspace );
      glPopAttrib();
      return;
  }  /* end switch */

  draw_edges ( edge_colour, interior_style, i, AWorkspace );

  glPopAttrib();
}

/*----------------------------------------------------------------------*/

void OpenGl_QuadrangleStrip::draw_edges (const TEL_COLOUR *edge_colour, const Aspect_InteriorStyle interior_style,
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
  Tint nc = myData.num_columns;
  Tint nf = myData.num_rows * myData.num_columns;

  Tint newList = 0;

  if ( !forceDraw )
    draw_line_strip ( edge_colour, nf, nc );
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

        draw_line_strip ( edge_colour, nf, nc );

      else if ( AWorkspace->SkipRatio < 1.f ) {

        int i, m1, m2, n1, n2, n3, n4;

        set_drawable_items ( myDS->bDraw, nf, AWorkspace->SkipRatio );

        glColor3fv ( edge_colour -> rgb );

        for ( i = 0; i < nf; ++i )

          if ( myDS->bDraw[ i ] ) {

            glBegin ( GL_LINE_STRIP );
            m1 = i / nc;
            m2 = i % nc;
            n1 = m1 * ( nc + 1 ) + m2;
            n2 = n1 + nc + 1;
            n3 = n2 + 1;
            n4 = n1 + 1;

            glVertex3fv ( myData.vertices[ n1 ].xyz );
            glVertex3fv ( myData.vertices[ n2 ].xyz );
            glVertex3fv ( myData.vertices[ n3 ].xyz );
            glVertex3fv ( myData.vertices[ n4 ].xyz );
            glVertex3fv ( myData.vertices[ n1 ].xyz );
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

void OpenGl_QuadrangleStrip::draw_line_strip (const TEL_COLOUR *c, Tint nf, Tint nc) const
{
  int i, m1, m2, n1, n2, n3, n4;

  glColor3fv ( c->rgb );

  for ( i = 0; i < nf; ++i )
  {
    glBegin ( GL_LINE_STRIP );
    m1 = i / nc;
    m2 = i % nc;
    n1 = m1 * ( nc + 1 ) + m2;
    n2 = n1 + nc + 1;
    n3 = n2 + 1;
    n4 = n1 + 1;

    glVertex3fv ( myData.vertices[ n1 ].xyz );
    glVertex3fv ( myData.vertices[ n2 ].xyz );
    glVertex3fv ( myData.vertices[ n3 ].xyz );
    glVertex3fv ( myData.vertices[ n4 ].xyz );
    glVertex3fv ( myData.vertices[ n1 ].xyz );
    glEnd();
  }
}

/*----------------------------------------------------------------------*/

OpenGl_QuadrangleStrip::OpenGl_QuadrangleStrip (const Graphic3d_Array2OfVertex& AListVertex)
{
  const Standard_Integer nbr = AListVertex.RowLength ();
  const Standard_Integer nbc = AListVertex.ColLength ();
  const Standard_Integer nbf = nbr*nbc;

  // Dynamic allocation
  TEL_POINT *points = new TEL_POINT[nbf];

  const Standard_Integer LowerRow = AListVertex.LowerRow ();
  const Standard_Integer UpperRow = AListVertex.UpperRow ();
  const Standard_Integer LowerCol = AListVertex.LowerCol ();
  const Standard_Integer UpperCol = AListVertex.UpperCol ();

  Standard_Integer i, j, k = 0;
  Standard_Real X, Y, Z;
  for (i=LowerRow; i<=UpperRow; i++)
    for (j=LowerCol; j<=UpperCol; j++)
    {
      AListVertex (i, j).Coord (X, Y, Z);
      points[k].xyz[0] = float (X);
      points[k].xyz[1] = float (Y);
      points[k].xyz[2] = float (Z);
      k++;
    }

  Init(nbr,nbc,points,NULL,NULL,NULL,NULL,NULL,TEL_SHAPE_UNKNOWN);
}

/*----------------------------------------------------------------------*/

OpenGl_QuadrangleStrip::OpenGl_QuadrangleStrip (const Graphic3d_Array2OfVertexN& AListVertex)
{
  const Standard_Integer nbr = AListVertex.RowLength ();
  const Standard_Integer nbc = AListVertex.ColLength ();
  const Standard_Integer nbf = nbr*nbc;

  // Dynamic allocation
  TEL_POINT *points = new TEL_POINT[nbf];
  TEL_POINT *normals = new TEL_POINT[nbf];

  const Standard_Integer LowerRow = AListVertex.LowerRow ();
  const Standard_Integer UpperRow = AListVertex.UpperRow ();
  const Standard_Integer LowerCol = AListVertex.LowerCol ();
  const Standard_Integer UpperCol = AListVertex.UpperCol ();

  Standard_Integer i, j, k = 0;
  Standard_Real X, Y, Z;
  for (i=LowerRow; i<=UpperRow; i++)
    for (j=LowerCol; j<=UpperCol; j++)
    {
      AListVertex (i, j).Coord (X, Y, Z);
      points[k].xyz[0] = float (X);
      points[k].xyz[1] = float (Y);
      points[k].xyz[2] = float (Z);
      AListVertex (i, j).Normal (X, Y, Z);
      normals[k].xyz[0] = float (X);
      normals[k].xyz[1] = float (Y);
      normals[k].xyz[2] = float (Z);
      k++;
    }

  Init(nbr,nbc,points,normals,NULL,NULL,NULL,NULL,TEL_SHAPE_UNKNOWN);
}

/*----------------------------------------------------------------------*/

OpenGl_QuadrangleStrip::OpenGl_QuadrangleStrip (const Graphic3d_Array2OfVertexNT& AListVertex)
{
  const Standard_Integer nbr = AListVertex.RowLength ();
  const Standard_Integer nbc = AListVertex.ColLength ();
  const Standard_Integer nbf = nbr*nbc;

  // Dynamic allocation
  TEL_POINT *points = new TEL_POINT[nbf];
  TEL_POINT *normals = new TEL_POINT[nbf];
  TEL_TEXTURE_COORD *tcoords = new TEL_TEXTURE_COORD[nbf];

  const Standard_Integer LowerRow = AListVertex.LowerRow ();
  const Standard_Integer UpperRow = AListVertex.UpperRow ();
  const Standard_Integer LowerCol = AListVertex.LowerCol ();
  const Standard_Integer UpperCol = AListVertex.UpperCol ();

  Standard_Integer i, j, k = 0;
  Standard_Real X, Y, Z;
  for (i=LowerRow; i<=UpperRow; i++)
    for (j=LowerCol; j<=UpperCol; j++)
    {
      AListVertex (i, j).Coord (X, Y, Z);
      points[k].xyz[0] = float (X);
      points[k].xyz[1] = float (Y);
      points[k].xyz[2] = float (Z);
      AListVertex (i, j).Normal (X, Y, Z);
      normals[k].xyz[0] = float (X);
      normals[k].xyz[1] = float (Y);
      normals[k].xyz[2] = float (Z);
      AListVertex (i, j).TextureCoordinate(X, Y);
      tcoords[k].xy[0] = float(X);
      tcoords[k].xy[1] = float(Y);
      k++;
    }

  Init(nbr,nbc,points,normals,NULL,tcoords,NULL,NULL,TEL_SHAPE_UNKNOWN);
}

/*----------------------------------------------------------------------*/

void OpenGl_QuadrangleStrip::Init (const Tint ANbRows, const Tint ANbCols, tel_point AVertices,
                                  tel_point AVNormals, tel_colour AVColors, tel_texture_coord ATCoords,
                                  tel_point AFNormals, tel_colour AFColors, const Tint AShapeFlag)
{
  myData.num_rows    = ANbRows;
  myData.num_columns = ANbCols;

  const Tint nf = ANbRows * ANbCols;
  const Tint nv = ( ANbRows+1 ) * ( ANbCols+1 );

  Tint i;

  // Store vertices
  myData.vertices = AVertices;

  // Store or compute (based on vertices) facet normals
  if (AFNormals)
  {
    myData.facet_flag = TEL_FA_NORMAL;
    myData.fnormals = new TEL_POINT[nf];
    memcpy( myData.fnormals, AFNormals, nf*sizeof(TEL_POINT) );
    for( i = 0; i < nf; i++ )
      vecnrm( myData.fnormals[i].xyz );
  }
  else
  {
    myData.facet_flag = TEL_FA_NONE;
    myData.fnormals = new TEL_POINT[nf];
    for( i = 0; i < nf; i++ )
    {
      const Tint r = i/ANbCols;
      const Tint c = i%ANbCols;
      const Tint id = r*(ANbCols+1) + c;

      TEL_POINT  p1, p2, p3, p4;

      p1 = myData.vertices[id];
      p2 = myData.vertices[id+ANbCols+1];
      p3 = myData.vertices[id+ANbCols+2];
      p4 = myData.vertices[id+1];

      p4.xyz[0] -= p2.xyz[0];
      p4.xyz[1] -= p2.xyz[1];
      p4.xyz[2] -= p2.xyz[2];

      p4.xyz[0] += p1.xyz[0];
      p4.xyz[1] += p1.xyz[1];
      p4.xyz[2] += p1.xyz[2];

      TelGetNormal( p4.xyz, p1.xyz, p3.xyz, myData.fnormals[i].xyz );
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
    Tint r, c, id;
    for( i = 0; i < nv; i++ )
    {
      if( i == 0 )
        myData.vnormals[i] = myData.fnormals[i];
      else if( i == (nv-1) )
        myData.vnormals[i] = myData.fnormals[nf-1];
      else if( i == (nv-ANbCols-1) )
        myData.vnormals[i] = myData.fnormals[nf-ANbCols];
      else if( i == ANbCols )
        myData.vnormals[i] = myData.fnormals[i-1];
      else if( i%(ANbCols+1) == 0 )
      {
        r = i/(myData.num_columns+1);
        myData.vnormals[i].xyz[0] = ( myData.fnormals[(r-1)*ANbCols].xyz[0] + myData.fnormals[r*ANbCols].xyz[0] ) / 2.0F;
        myData.vnormals[i].xyz[1] = ( myData.fnormals[(r-1)*ANbCols].xyz[1] + myData.fnormals[r*ANbCols].xyz[1] ) / 2.0F;
        myData.vnormals[i].xyz[2] = ( myData.fnormals[(r-1)*ANbCols].xyz[2] + myData.fnormals[r*ANbCols].xyz[2] ) / 2.0F;
      }
      else if( i <= ANbCols )
      {
        myData.vnormals[i].xyz[0] = ( myData.fnormals[i-1].xyz[0] + myData.fnormals[i].xyz[0] ) / 2.0F;
        myData.vnormals[i].xyz[1] = ( myData.fnormals[i-1].xyz[1] + myData.fnormals[i].xyz[1] ) / 2.0F;
        myData.vnormals[i].xyz[2] = ( myData.fnormals[i-1].xyz[2] + myData.fnormals[i].xyz[2] ) / 2.0F;
      }
      else if( i >= (nv-ANbCols-1 ) )
      {
        c = i%(ANbCols+1);
        id = nf-ANbCols+c;

        myData.vnormals[i].xyz[0] = ( myData.fnormals[id-1].xyz[0] + myData.fnormals[id].xyz[0] ) / 2.0F;
        myData.vnormals[i].xyz[1] = ( myData.fnormals[id-1].xyz[1] + myData.fnormals[id].xyz[1] ) / 2.0F;
        myData.vnormals[i].xyz[2] = ( myData.fnormals[id-1].xyz[2] + myData.fnormals[id].xyz[2] ) / 2.0F;
      }
      else if( (i+1)%(ANbCols+1) == 0 )
      {
        r = (i+1)/(ANbCols+1);
        myData.vnormals[i].xyz[0] = ( myData.fnormals[(r-1)*ANbCols-1].xyz[0] + myData.fnormals[r*ANbCols-1].xyz[0] ) / 2.0F;
        myData.vnormals[i].xyz[1] = ( myData.fnormals[(r-1)*ANbCols-1].xyz[1] + myData.fnormals[r*ANbCols-1].xyz[1] ) / 2.0F;
        myData.vnormals[i].xyz[2] = ( myData.fnormals[(r-1)*ANbCols-1].xyz[2] + myData.fnormals[r*ANbCols-1].xyz[2] ) / 2.0F;
      }
      else
      {
        r = i/(ANbCols+1);
        c = i%(ANbCols+1);

        id = r*ANbCols+c;

        myData.vnormals[i].xyz[0] = ( myData.fnormals[id].xyz[0] + myData.fnormals[id-ANbCols].xyz[0] + myData.fnormals[id-1].xyz[0] + myData.fnormals[id-ANbCols-1].xyz[0] ) / 4.0F;
        myData.vnormals[i].xyz[1] = ( myData.fnormals[id].xyz[1] + myData.fnormals[id-ANbCols].xyz[1] + myData.fnormals[id-1].xyz[1] + myData.fnormals[id-ANbCols-1].xyz[1] ) / 4.0F;
        myData.vnormals[i].xyz[2] = ( myData.fnormals[id].xyz[2] + myData.fnormals[id-ANbCols].xyz[2] + myData.fnormals[id-1].xyz[2] + myData.fnormals[id-ANbCols-1].xyz[2] ) / 4.0F;
      }
    }
  }

  myData.vcolours = AVColors;
  myData.vtexturecoord = ATCoords;
  myData.fcolours = AFColors;

  myData.shape_flag = AShapeFlag;

  myDS = new DS_INTERNAL;
  myDS->list      =  0;
  myDS->dlist     =  0;
  myDS->degMode   =  0;
  myDS->model     = -1;
  myDS->skipRatio =  0.0F;
  myDS->bDraw = new unsigned char[nv];
}

/*----------------------------------------------------------------------*/

OpenGl_QuadrangleStrip::~OpenGl_QuadrangleStrip ()
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

void OpenGl_QuadrangleStrip::Render (const Handle(OpenGl_Workspace) &AWorkspace) const
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

  draw_qstrip( front_lighting_model,
               aspect_face->Context().InteriorStyle,
               edge_colour,
               AWorkspace );
}

/*----------------------------------------------------------------------*/
