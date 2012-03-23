// Created on: 2011-09-20
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


#include <math.h>
#include <stdio.h>

#include <OpenGl_GlCore11.hxx>
#include <OpenGl_Context.hxx>
#include <OpenGl_telem_util.hxx>
#include <OpenGl_AspectLine.hxx>
#include <OpenGl_Structure.hxx>

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifdef HAVE_GL2PS
  #include <gl2ps.h>
#endif

/*----------------------------------------------------------------------*/
/*
* Prototypes Private functions
*/

static void call_util_transform_pt (float *x, float *y, float *z);
static void call_util_transpose_mat (float tmat[16], float mat[4][4]);

/*----------------------------------------------------------------------*/
/* 
* Variables statiques
*/

static int openglNumberOfPoints = 0;

static int myImmediateMatIsIdentity = 1;

static int partial = -1;  /* -1 init, 0 complete, 1 partielle */

static float xm, ym, zm, XM, YM, ZM;

static float myImmediateMat[4][4] = {
  {1., 0., 0., 0.},
  {0., 1., 0., 0.},
  {0., 0., 1., 0.},
  {0., 0., 0., 1.},
};

/*----------------------------------------------------------------------*/
/*  Mode Ajout              */
/*----------------------------------------------------------------------*/

//call_togl_begin_ajout_mode
Standard_Boolean OpenGl_Workspace::BeginAddMode ()
{
  if (!Activate())
    return Standard_False;

  NamedStatus |= OPENGL_NS_ADD;

  MakeFrontAndBackBufCurrent();

  //TsmPushAttri();

  return Standard_True;
}

/*----------------------------------------------------------------------*/

//call_togl_end_ajout_mode
void OpenGl_Workspace::EndAddMode ()
{
  if (NamedStatus & OPENGL_NS_ADD)
  {
    OpenGl_Workspace::MakeBackBufCurrent();

    // Clear add mode flag
    NamedStatus &= ~OPENGL_NS_ADD;
  }

  myImmediateMatIsIdentity = 1;

  /* FMN necessaire pour l'affichage sur WNT */
  glFlush();

  //TsmPopAttri();
}

/*----------------------------------------------------------------------*/
/*  Mode Transient              */
/*----------------------------------------------------------------------*/

//call_togl_clear_immediat_mode
void OpenGl_Workspace::ClearImmediatMode (const Graphic3d_CView& ACView, const Standard_Boolean AFlush)
{
  if ( myIsTransientOpen )
    EndImmediatMode();

  if (!Activate()) return;

  if ( !myBackBufferRestored )
  {
    EraseAnimation();

    Redraw1(ACView,*((CALL_DEF_LAYER *)ACView.ptrUnderLayer),*((CALL_DEF_LAYER *)ACView.ptrOverLayer),AFlush);

    // After a redraw,
    // Made the back identical to the front buffer.
    // Always perform full copy (partial update optimization is useless on mordern hardware)!
    if (myRetainMode)
      CopyBuffers (ACView.ViewId, 1 /* GL_FRONT -> GL_BACK */, xm, ym, zm, XM, YM, ZM, 0);

    myBackBufferRestored = Standard_True;
  }
  else if ( partial >= 0 )
  {
    // Restore pixels from the back buffer.
    // Always perform full copy (partial update optimization is useless on mordern hardware)!
    CopyBuffers (ACView.ViewId, 0 /* GL_BACK -> GL_FRONT */, xm, ym, zm, XM, YM, ZM, 0);
  }

  if (myTransientList)
  {
    /* Clear current list contents */
    glNewList( (GLuint) myTransientList, GL_COMPILE_AND_EXECUTE);
    glEndList();
  }
  partial = -1;
  XM = YM = ZM = (float ) shortrealfirst ();
  xm = ym = zm = (float ) shortreallast ();
}

/*----------------------------------------------------------------------*/

//call_togl_redraw_immediat_mode
void OpenGl_Workspace::RedrawImmediatMode ()
{
  if (myRetainMode)
  {
    if (myTransientList)
    {
      MakeFrontBufCurrent();
      glDisable(GL_LIGHTING);
      glCallList((GLuint) myTransientList);
      /* FMN necessaire pour l'affichage sur WNT */
      glFlush();
      MakeBackBufCurrent();
    } 
  }
}

/*----------------------------------------------------------------------*/

//call_togl_begin_immediat_mode
Standard_Boolean OpenGl_Workspace::BeginImmediatMode (const Graphic3d_CView& ACView, const Standard_Boolean UseDepthTest, const Standard_Boolean RetainMode)
{
  if (!Activate())
    return Standard_False;

  OpenGl_Workspace::ClearImmediatMode(ACView,1);

  NamedStatus |= OPENGL_NS_IMMEDIATE;
  myRetainMode = RetainMode;

  MakeFrontBufCurrent();

  //TsmPushAttri();

  if ( myRetainMode )
  {
    GLuint listid = (GLuint) myTransientList;
    if (!listid)
      listid = glGenLists(1);
    if (!listid) return Standard_False;

    glNewList(listid, GL_COMPILE_AND_EXECUTE);
    myTransientList = listid;
    myIsTransientOpen = Standard_True;
  }

  if ( UseDepthTest )
    glEnable(GL_DEPTH_TEST);
  else
    glDisable(GL_DEPTH_TEST);

  return Standard_True;
}

/*----------------------------------------------------------------------*/

//call_togl_end_immediat_mode
void OpenGl_Workspace::EndImmediatMode ()
{
  if (NamedStatus & OPENGL_NS_IMMEDIATE)
  {
    if (myIsTransientOpen)
    {
      glEndList();
      myIsTransientOpen = Standard_False;
    }
    MakeBackBufCurrent();

    // Clear immediate mode flag
    NamedStatus &= ~OPENGL_NS_IMMEDIATE;
  }

  // Ajout CAL : pour voir quelque chose avant le prochain begin_immediat_mode
  glFinish ();

  myImmediateMatIsIdentity = 1;

  //TsmPopAttri();
}

/*----------------------------------------------------------------------*/

//call_togl_transform
void OpenGl_Workspace::Transform (const TColStd_Array2OfReal& AMatrix, const Graphic3d_TypeOfComposition AType)
{
  //call_togl_transform in OpenGl_togl_begin_immediat_mode.cxx
  const Standard_Integer lr = AMatrix.LowerRow ();
  const Standard_Integer lc = AMatrix.LowerCol ();

  Standard_Integer i, j;
  if ((AType == Graphic3d_TOC_REPLACE) || myImmediateMatIsIdentity)
  {
    for (i=0; i<4; i++)
      for (j=0; j<4; j++)
        myImmediateMat[i][j] = float (AMatrix (i+lr, j+lc));
  }
  else
  {
    float theMatrix[4][4];
    for (i=0; i<4; i++)
      for (j=0; j<4; j++)
        theMatrix[i][j] = float (AMatrix (i+lr, j+lc));

    TelMultiplymat3 (myImmediateMat, myImmediateMat, theMatrix);
  }

  myImmediateMatIsIdentity = 1;
  for (i = 0; i < 4; i++)
    for (j = 0; j < 4; j++)
      if (myImmediateMat[i][j] != (i == j? 1. : 0.))
      {
        myImmediateMatIsIdentity = 0;
        return;
      }
}

/*----------------------------------------------------------------------*/

//call_togl_begin_polyline
void OpenGl_Workspace::BeginPolyline ()
{
  if (NamedStatus & (OPENGL_NS_ADD | OPENGL_NS_IMMEDIATE))
  {
    openglNumberOfPoints = 0;
    glDisable(GL_LIGHTING);
    glBegin(GL_LINE_STRIP);
  }
}

/*----------------------------------------------------------------------*/

//call_togl_end_polyline
void OpenGl_Workspace::EndPolyline ()
{
  if (NamedStatus & (OPENGL_NS_ADD | OPENGL_NS_IMMEDIATE))
    glEnd();
}

/*----------------------------------------------------------------------*/

//call_togl_draw
void OpenGl_Workspace::Draw (const Standard_ShortReal X, const Standard_ShortReal Y, const Standard_ShortReal Z)
{
  if (NamedStatus & (OPENGL_NS_ADD | OPENGL_NS_IMMEDIATE))
  {
    openglNumberOfPoints++;
    float x = X, y = Y, z = Z;
    if (!myImmediateMatIsIdentity)
      call_util_transform_pt (&x, &y, &z);
    if (x > XM) XM = x;
    if (y > YM) YM = y;
    if (z > ZM) ZM = z;
    if (x < xm) xm = x;
    if (y < ym) ym = y;
    if (z < zm) zm = z;
    glVertex3f (x, y, z);
    partial = 1;
  }
}

/*----------------------------------------------------------------------*/

//call_togl_move
void OpenGl_Workspace::Move (const Standard_ShortReal X, const Standard_ShortReal Y, const Standard_ShortReal Z)
{
  if (NamedStatus & (OPENGL_NS_ADD | OPENGL_NS_IMMEDIATE))
  {
    if (openglNumberOfPoints != 0)
    {
      OpenGl_Workspace::EndPolyline();
      OpenGl_Workspace::BeginPolyline();
    }
    OpenGl_Workspace::Draw(X,Y,Z);
  }
}

/*----------------------------------------------------------------------*/

//call_togl_set_linecolor
void OpenGl_Workspace::SetLineColor (const Standard_ShortReal R, const Standard_ShortReal G, const Standard_ShortReal B) 
{
  if (NamedStatus & (OPENGL_NS_ADD | OPENGL_NS_IMMEDIATE))
  {
    GLfloat color[3];
    color[0] = R;
    color[1] = G;
    color[2] = B;
    glColor3fv(color);
  }
}

/*----------------------------------------------------------------------*/

//call_togl_set_linetype
void OpenGl_Workspace::SetLineType (const Standard_Integer Type)
{
  if (NamedStatus & (OPENGL_NS_ADD | OPENGL_NS_IMMEDIATE))
  {
    myDisplay->SetTypeOfLine((Aspect_TypeOfLine)Type);
  }
}

/*----------------------------------------------------------------------*/

//call_togl_set_linewidth
void OpenGl_Workspace::SetLineWidth (const Standard_ShortReal Width)
{
  if (NamedStatus & (OPENGL_NS_ADD | OPENGL_NS_IMMEDIATE))
  {
    glLineWidth( (GLfloat)Width );
#ifdef HAVE_GL2PS
    gl2psLineWidth( (GLfloat)Width );
#endif
  }
}

/*----------------------------------------------------------------------*/

//call_togl_draw_structure
void OpenGl_Workspace::DrawStructure (const OpenGl_Structure *AStructure)
{
  if (NamedStatus & (OPENGL_NS_ADD | OPENGL_NS_IMMEDIATE))
  {
    float mat16[16];
    GLint mode1;

    //TsmPushAttri();

    /* mise en place de la matrice de transformation du trace transient */
    if (!myImmediateMatIsIdentity)
    {
      call_util_transpose_mat (mat16, myImmediateMat);
      glGetIntegerv (GL_MATRIX_MODE, &mode1);
      glMatrixMode (GL_MODELVIEW);
      glPushMatrix ();
      glScalef (1.F, 1.F, 1.F);
      glMultMatrixf (mat16);
    }

    // Render structure
    Handle(OpenGl_Workspace) aWS(this);
    AStructure->Render(aWS);

    //TsmPopAttri();

    if (!myImmediateMatIsIdentity)
    {
      glPopMatrix ();
      glMatrixMode (mode1);
    }
  }
}

/*----------------------------------------------------------------------*/

//call_togl_set_minmax
void OpenGl_Workspace::SetMinMax (const Standard_ShortReal X1, const Standard_ShortReal Y1, const Standard_ShortReal Z1, const Standard_ShortReal X2, const Standard_ShortReal Y2, const Standard_ShortReal Z2)
{
  if ((X1 > shortreallast ()) || (Y1 > shortreallast ()) || (Z1 > shortreallast ()) ||
      (X2 > shortreallast ()) || (Y2 > shortreallast ()) || (Z2 > shortreallast ()) ||
      (X1 < shortrealfirst ()) || (Y1 < shortrealfirst ()) || (Z1 < shortrealfirst ()) ||
      (X2 < shortrealfirst ()) || (Y2 < shortrealfirst ()) || (Z2 < shortrealfirst ()))
  {
    XM = YM = ZM = (float ) shortreallast ();
    xm = ym = zm = (float ) shortrealfirst ();
    partial = 0;
  }
  else
  {
    float x1=X1,y1=Y1,z1=Z1,x2=X2,y2=Y2,z2=Z2;
    if (!myImmediateMatIsIdentity)
    {
      call_util_transform_pt (&x1, &y1, &z1);
      call_util_transform_pt (&x2, &y2, &z2);
    }
    if (x1 > XM) XM = x1;
    if (x1 < xm) xm = x1;
    if (y1 > YM) YM = y1;
    if (y1 < ym) ym = y1;
    if (z1 > ZM) ZM = z1;
    if (z1 < zm) zm = z1;

    if (x2 > XM) XM = x2;
    if (x2 < xm) xm = x2;
    if (y2 > YM) YM = y2;
    if (y2 < ym) ym = y2;
    if (z2 > ZM) ZM = z2;
    if (z2 < zm) zm = z2;
    if (partial != 0) partial = 1;
  }
}

/*----------------------------------------------------------------------*/
/*
* Private functions
*/

/*----------------------------------------------------------------------*/
/*
Transform the point pt
*/
static void call_util_transform_pt ( float *x, float *y, float *z )
{
  float tpt[4], pt[4];
  pt[0] = *x, pt[1] = *y, pt[2] = *z, pt[3] = 1.0;

  int i, j;
  for (i = 0; i < 4; i++)
  {
    float sum = 0.;
    for (j = 0; j < 4; j++)
      sum += myImmediateMat[i][j] * pt[j];
    tpt[i] = sum;
  }

  *x = tpt[0], *y = tpt[1], *z = tpt[2];
}

/*----------------------------------------------------------------------*/
/*
void call_util_transpose_mat (tmat, mat)
float tmat[16];
float mat[4][4];

Transpose mat and returns tmat.
*/

static void call_util_transpose_mat (float tmat[16], float mat[4][4])
{
  int i, j;

  for (i=0; i<4; i++)
    for (j=0; j<4; j++)
      tmat[j*4+i] = mat[i][j];
}
