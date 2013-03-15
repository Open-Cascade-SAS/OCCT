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

#include <OpenGl_GlCore11.hxx>

#include <OpenGl_View.hxx>
#include <OpenGl_Context.hxx>
#include <OpenGl_Workspace.hxx>
#include <OpenGl_Display.hxx>

#include <OpenGl_Texture.hxx>
#include <OpenGl_Trihedron.hxx>
#include <OpenGl_GraduatedTrihedron.hxx>

#include <OpenGl_transform_persistence.hxx>

#include <Graphic3d_TextureEnv.hxx>

IMPLEMENT_STANDARD_HANDLE(OpenGl_View,MMgt_TShared)
IMPLEMENT_STANDARD_RTTIEXT(OpenGl_View,MMgt_TShared)

/*----------------------------------------------------------------------*/

static const OPENGL_BG_TEXTURE myDefaultBgTexture = { 0, 0, 0, Aspect_FM_CENTERED };
static const OPENGL_BG_GRADIENT myDefaultBgGradient = { {{ 0.F, 0.F, 0.F, 1.F }}, {{ 0.F, 0.F, 0.F, 1.F }}, Aspect_GFM_NONE };
static const Tmatrix3 myDefaultMatrix = { { 1.F, 0.F, 0.F, 0.F }, { 0.F, 1.F, 0.F, 0.F }, { 0.F, 0.F, 1.F, 0.F }, { 0.F, 0.F, 0.F, 1.F } };
static const OPENGL_ZCLIP myDefaultZClip = { { Standard_True, 0.F }, { Standard_True, 1.F } };
static const OPENGL_EXTRA_REP myDefaultExtra =
{
  //vrp
  { 0.F, 0.F, 0.F },
  //vpn
  { 0.F, 0.F, 1.F },
  //vup
  { 0.F, 1.F, 0.F },
  //map
  {
    //window
    { 0.F, 0.F, 1.F, 1.F },
    //viewport
    { 0.F, 0.F, 0.F, 1.F, 1.F, 1.F },
    //proj
    TelParallel,
    //prp
    { 0.F, 0.F, 0.F },
    //vpd
    0.F,
    //fpd
    0.F,
    //bpd
    -1.F
  },
  //scaleFactors
  { 1.F, 1.F, 1.F }
};

static const OPENGL_FOG myDefaultFog = { Standard_False, 0.F, 1.F, { { 0.F, 0.F, 0.F, 1.F } } };
static const TEL_TRANSFORM_PERSISTENCE myDefaultTransPers = { 0, 0.F, 0.F, 0.F };
static const GLdouble THE_IDENTITY_MATRIX[4][4] =
{
  {1.0, 0.0, 0.0, 0.0},
  {0.0, 1.0, 0.0, 0.0},
  {0.0, 0.0, 1.0, 0.0},
  {0.0, 0.0, 0.0, 1.0}
};

/*----------------------------------------------------------------------*/

OpenGl_View::OpenGl_View (const CALL_DEF_VIEWCONTEXT &AContext)
: mySurfaceDetail(Visual3d_TOD_NONE),
  myBackfacing(0),
  myBgTexture(myDefaultBgTexture),
  myBgGradient(myDefaultBgGradient),
  //myOrientationMatrix(myDefaultMatrix),
  //myMappingMatrix(myDefaultMatrix),
  //shield_indicator = TOn,
  //shield_colour = { { 0.F, 0.F, 0.F, 1.F } },
  //border_indicator = TOff,
  //border_colour = { { 0.F, 0.F, 0.F, 1.F } },
  //active_status = TOn,
  myZClip(myDefaultZClip),
  myExtra(myDefaultExtra),
  myFog(myDefaultFog),
  myVisualization(AContext.Visualization),
  myIntShadingMethod(TEL_SM_GOURAUD),
  myAntiAliasing(Standard_False),
  myTransPers(&myDefaultTransPers),
  myIsTransPers(Standard_False)
{
  // Initialize matrices
  memcpy(myOrientationMatrix,myDefaultMatrix,sizeof(Tmatrix3));
  memcpy(myMappingMatrix,myDefaultMatrix,sizeof(Tmatrix3));

  // Shading method
  switch (AContext.Model)
  {
    case 1 : /* VISUAL3D_TOM_INTERP_COLOR */
    case 3 : /* VISUAL3D_TOM_VERTEX */
      myIntShadingMethod = TEL_SM_GOURAUD;
      break;
    default :
      myIntShadingMethod = TEL_SM_FLAT;
      break;
  }
}

/*----------------------------------------------------------------------*/

OpenGl_View::~OpenGl_View ()
{
  ReleaseGlResources (NULL); // ensure ReleaseGlResources() was called within valid context
}

void OpenGl_View::ReleaseGlResources (const Handle(OpenGl_Context)& theCtx)
{
  OpenGl_Element::Destroy (theCtx, myTrihedron);
  OpenGl_Element::Destroy (theCtx, myGraduatedTrihedron);
  if (!myTextureEnv.IsNull())
  {
    theCtx->DelayedRelease (myTextureEnv);
    myTextureEnv.Nullify();
  }
  if (myBgTexture.TexId != 0)
  {
    glDeleteTextures (1, (GLuint*)&(myBgTexture.TexId));
    myBgTexture.TexId = 0;
  }
}

void OpenGl_View::SetTextureEnv (const Handle(OpenGl_Context)&       theCtx,
                                 const Handle(Graphic3d_TextureEnv)& theTexture)
{
  if (!myTextureEnv.IsNull())
  {
    theCtx->DelayedRelease (myTextureEnv);
    myTextureEnv.Nullify();
  }

  if (theTexture.IsNull())
  {
    return;
  }

  myTextureEnv = new OpenGl_Texture (theTexture->GetParams());
  Handle(Image_PixMap) anImage = theTexture->GetImage();
  myTextureEnv->Init (theCtx, *anImage.operator->(), theTexture->Type());
}

/*----------------------------------------------------------------------*/

void OpenGl_View::SetBackfacing (const Standard_Integer theMode)
{
  myBackfacing = theMode;
}

/*----------------------------------------------------------------------*/

//call_togl_setlight
void OpenGl_View::SetLights (const CALL_DEF_VIEWCONTEXT &AContext)
{
  myLights.Clear();

  const int nb_lights = AContext.NbActiveLight;

  int i = 0;
  const CALL_DEF_LIGHT *alight = &(AContext.ActiveLight[0]);
  for ( ; i < nb_lights; i++, alight++ )
  {
    OpenGl_Light rep;

	switch( alight->LightType )
    {
      case 0 : /* TOLS_AMBIENT */
        rep.type = TLightAmbient;
        rep.col.rgb[0] = alight->Color.r;
        rep.col.rgb[1] = alight->Color.g;
        rep.col.rgb[2] = alight->Color.b;
        break;

      case 1 : /* TOLS_DIRECTIONAL */
        rep.type = TLightDirectional;
        rep.col.rgb[0] = alight->Color.r;
        rep.col.rgb[1] = alight->Color.g;
        rep.col.rgb[2] = alight->Color.b;
        rep.dir[0] = alight->Direction.x;
        rep.dir[1] = alight->Direction.y;
        rep.dir[2] = alight->Direction.z;
        break;

      case 2 : /* TOLS_POSITIONAL */
        rep.type = TLightPositional;
        rep.col.rgb[0] = alight->Color.r;
        rep.col.rgb[1] = alight->Color.g;
        rep.col.rgb[2] = alight->Color.b;
        rep.pos[0] = alight->Position.x;
        rep.pos[1] = alight->Position.y;
        rep.pos[2] = alight->Position.z;
        rep.atten[0] = alight->Attenuation[0];
        rep.atten[1] = alight->Attenuation[1];
        break;

      case 3 : /* TOLS_SPOT */
        rep.type = TLightSpot;
        rep.col.rgb[0] = alight->Color.r;
        rep.col.rgb[1] = alight->Color.g;
        rep.col.rgb[2] = alight->Color.b;
        rep.pos[0] = alight->Position.x;
        rep.pos[1] = alight->Position.y;
        rep.pos[2] = alight->Position.z;
        rep.dir[0] = alight->Direction.x;
        rep.dir[1] = alight->Direction.y;
        rep.dir[2] = alight->Direction.z;
        rep.shine = alight->Concentration;
        rep.atten[0] = alight->Attenuation[0];
        rep.atten[1] = alight->Attenuation[1];
        rep.angle = alight->Angle;
        break;
    }

    rep.HeadLight = alight->Headlight;

    myLights.Append(rep);
  }
}

/*----------------------------------------------------------------------*/

//call_togl_setplane
void OpenGl_View::SetClippingPlanes (const CALL_DEF_VIEWCONTEXT &AContext)
{
  // clear clipping planes information
  myClippingPlanes.Clear();
  // update information
  int i = 0;
  for (; i < AContext.NbActivePlane; i++)
  {
    const CALL_DEF_PLANE &aCPlane = AContext.ActivePlane[i];
    if ( aCPlane.Active && aCPlane.PlaneId > 0 )
    {
      OPENGL_CLIP_REP aPlane;
      aPlane.equation[0] = aCPlane.CoefA;
      aPlane.equation[1] = aCPlane.CoefB;
      aPlane.equation[2] = aCPlane.CoefC;
      aPlane.equation[3] = aCPlane.CoefD;
      myClippingPlanes.Append( aPlane );
    }
  }
}

/*----------------------------------------------------------------------*/

//call_togl_setvisualisation
void OpenGl_View::SetVisualisation (const CALL_DEF_VIEWCONTEXT &AContext)
{
  myVisualization = AContext.Visualization;
  // Shading method
  switch (AContext.Model)
  {
    case 1 : /* VISUAL3D_TOM_INTERP_COLOR */
    case 3 : /* VISUAL3D_TOM_VERTEX */
      myIntShadingMethod = TEL_SM_GOURAUD;
      break;
    default :
      myIntShadingMethod = TEL_SM_FLAT;
      break;
  }
}

/*----------------------------------------------------------------------*/

//call_togl_cliplimit
void OpenGl_View::SetClipLimit (const Graphic3d_CView& theCView)
{
  myZClip.Back.Limit =
    (theCView.Context.ZClipBackPlane     - theCView.Mapping.BackPlaneDistance) /
    (theCView.Mapping.FrontPlaneDistance - theCView.Mapping.BackPlaneDistance);
  myZClip.Front.Limit =
    (theCView.Context.ZClipFrontPlane    - theCView.Mapping.BackPlaneDistance) /
    (theCView.Mapping.FrontPlaneDistance - theCView.Mapping.BackPlaneDistance);
  if (myZClip.Back.Limit < 0.0f)
    myZClip.Back.Limit = 0.0f;
  if (myZClip.Front.Limit > 1.0f)
    myZClip.Front.Limit = 1.0f;
  if (myZClip.Back.Limit > myZClip.Front.Limit)
  {
    myZClip.Back.Limit  = 0.0f;
    myZClip.Front.Limit = 1.0f;
  }

  myZClip.Back.IsOn  = (theCView.Context.BackZClipping  != 0);
  myZClip.Front.IsOn = (theCView.Context.FrontZClipping != 0);
}

/*----------------------------------------------------------------------*/

//call_togl_viewmapping
void OpenGl_View::SetMapping (const Graphic3d_CView& theCView)
{
  const float ratio   = theCView.DefWindow.dy / theCView.DefWindow.dx;
  const float r_ratio = theCView.DefWindow.dx / theCView.DefWindow.dy;

  TEL_VIEW_MAPPING Map;

  Map.window.xmin = theCView.Mapping.WindowLimit.um;
  Map.window.ymin = theCView.Mapping.WindowLimit.vm;
  Map.window.xmax = theCView.Mapping.WindowLimit.uM;
  Map.window.ymax = theCView.Mapping.WindowLimit.vM;

  Map.viewport.xmin = 0.F;
  Map.viewport.xmax = ( 1.F < r_ratio ? 1.F : r_ratio );
  Map.viewport.ymin = 0.F;
  Map.viewport.ymax = ( 1.F < ratio ? 1.F : ratio );
  Map.viewport.zmin = 0.F;
  Map.viewport.zmax = 1.F;

  // projection type
  switch (theCView.Mapping.Projection)
  {
    case 0 :
      Map.proj = TelPerspective;
      break;
    case 1 :
      Map.proj = TelParallel;
      break;
  }

  // projection reference point
  Map.prp[0] = theCView.Mapping.ProjectionReferencePoint.x;
  Map.prp[1] = theCView.Mapping.ProjectionReferencePoint.y;
  Map.prp[2] = theCView.Mapping.ProjectionReferencePoint.z;
  if (!openglDisplay.IsNull() && !openglDisplay->Walkthrough())
    Map.prp[2] += theCView.Mapping.FrontPlaneDistance;

  // view plane distance
  Map.vpd = theCView.Mapping.ViewPlaneDistance;

  // back plane distance
  Map.bpd = theCView.Mapping.BackPlaneDistance;

  // front plane distance
  Map.fpd = theCView.Mapping.FrontPlaneDistance;

  Tint err_ind = 0;

  // use user-defined matrix
  if (theCView.Mapping.IsCustomMatrix)
  {
    int i, j;
    for( i = 0; i < 4; i++ )
      for( j = 0; j < 4; j++ )
        myMappingMatrix[i][j] = theCView.Mapping.ProjectionMatrix[i][j];
  }
  else
    TelEvalViewMappingMatrix( &Map, &err_ind, myMappingMatrix );

  if (!err_ind)
    myExtra.map = Map;
}

/*----------------------------------------------------------------------*/

//call_togl_vieworientation
void OpenGl_View::SetOrientation (const Graphic3d_CView& theCView)
{
  Tfloat Vrp[3];
  Tfloat Vpn[3];
  Tfloat Vup[3];
  Tfloat ScaleFactors[3];

  Vrp[0] = theCView.Orientation.ViewReferencePoint.x;
  Vrp[1] = theCView.Orientation.ViewReferencePoint.y;
  Vrp[2] = theCView.Orientation.ViewReferencePoint.z;

  Vpn[0] = theCView.Orientation.ViewReferencePlane.x;
  Vpn[1] = theCView.Orientation.ViewReferencePlane.y;
  Vpn[2] = theCView.Orientation.ViewReferencePlane.z;

  Vup[0] = theCView.Orientation.ViewReferenceUp.x;
  Vup[1] = theCView.Orientation.ViewReferenceUp.y;
  Vup[2] = theCView.Orientation.ViewReferenceUp.z;

  ScaleFactors[0] = theCView.Orientation.ViewScaleX;
  ScaleFactors[1] = theCView.Orientation.ViewScaleY;
  ScaleFactors[2] = theCView.Orientation.ViewScaleZ;

  Tint err_ind = 0;

  // use user-defined matrix
  if (theCView.Orientation.IsCustomMatrix)
  {
    int i, j;
    for( i = 0; i < 4; i++ )
      for( j = 0; j < 4; j++ )
        myOrientationMatrix[i][j] = theCView.Orientation.ModelViewMatrix[i][j];
  }
  else
  {
    TelEvalViewOrientationMatrix (Vrp, Vpn, Vup, ScaleFactors, &err_ind, myOrientationMatrix);
  }

  if (!err_ind)
  {
    myExtra.vrp[0] = Vrp[0];
    myExtra.vrp[1] = Vrp[1];
    myExtra.vrp[2] = Vrp[2];

    myExtra.vpn[0] = Vpn[0];
    myExtra.vpn[1] = Vpn[1];
    myExtra.vpn[2] = Vpn[2];

    myExtra.vup[0] = Vup[0];
    myExtra.vup[1] = Vup[1];
    myExtra.vup[2] = Vup[2];

    myExtra.scaleFactors[0] = ScaleFactors[0],
    myExtra.scaleFactors[1] = ScaleFactors[1],
    myExtra.scaleFactors[2] = ScaleFactors[2];
  }
}

/*----------------------------------------------------------------------*/

void OpenGl_View::SetFog (const Graphic3d_CView& theCView,
                          const Standard_Boolean theFlag)
{
  if (!theFlag)
  {
    myFog.IsOn = Standard_False;
  }
  else
  {
    myFog.IsOn = Standard_True;

    myFog.Front =
      (theCView.Context.DepthFrontPlane    - theCView.Mapping.BackPlaneDistance) /
      (theCView.Mapping.FrontPlaneDistance - theCView.Mapping.BackPlaneDistance);

    myFog.Back =
      (theCView.Context.DepthBackPlane     - theCView.Mapping.BackPlaneDistance) /
      (theCView.Mapping.FrontPlaneDistance - theCView.Mapping.BackPlaneDistance);

    if (myFog.Front < 0.F)
      myFog.Front = 0.F;
    else if (myFog.Front > 1.F)
      myFog.Front = 1.F;

    if (myFog.Back < 0.F)
      myFog.Back = 0.F;
    else if (myFog.Back > 1.F)
      myFog.Back = 1.F;

    if (myFog.Back > myFog.Front)
    {
      myFog.Front = 1.F;
      myFog.Back = 0.F;
    }

    myFog.Color.rgb[0] = theCView.DefWindow.Background.r;
    myFog.Color.rgb[1] = theCView.DefWindow.Background.g;
    myFog.Color.rgb[2] = theCView.DefWindow.Background.b;
    myFog.Color.rgb[3] = 1.0f;
  }
}

/*----------------------------------------------------------------------*/

void OpenGl_View::TriedronDisplay (const Handle(OpenGl_Context)&       theCtx,
                                   const Aspect_TypeOfTriedronPosition thePosition,
                                   const Quantity_NameOfColor          theColor,
                                   const Standard_Real                 theScale,
                                   const Standard_Boolean              theAsWireframe)
{
  OpenGl_Element::Destroy (theCtx, myTrihedron);
  myTrihedron = new OpenGl_Trihedron (thePosition, theColor, theScale, theAsWireframe);
}

/*----------------------------------------------------------------------*/

void OpenGl_View::TriedronErase (const Handle(OpenGl_Context)& theCtx)
{
  OpenGl_Element::Destroy (theCtx, myTrihedron);
}

/*----------------------------------------------------------------------*/

void OpenGl_View::GraduatedTrihedronDisplay (const Handle(OpenGl_Context)&        theCtx,
                                             const Graphic3d_CGraduatedTrihedron& theData)
{
  OpenGl_Element::Destroy (theCtx, myGraduatedTrihedron);
  myGraduatedTrihedron = new OpenGl_GraduatedTrihedron (theData);
}

/*----------------------------------------------------------------------*/

void OpenGl_View::GraduatedTrihedronErase (const Handle(OpenGl_Context)& theCtx)
{
  OpenGl_Element::Destroy (theCtx, myGraduatedTrihedron);
}

/*----------------------------------------------------------------------*/

//transform_persistence_end
void OpenGl_View::EndTransformPersistence()
{
  if (myIsTransPers)
  {
    // restore matrix
    glMatrixMode (GL_PROJECTION);
    glPopMatrix();
    glMatrixMode (GL_MODELVIEW);
    glPopMatrix();
    myIsTransPers = Standard_False;
  }
}

/*----------------------------------------------------------------------*/

//transform_persistence_begin
const TEL_TRANSFORM_PERSISTENCE* OpenGl_View::BeginTransformPersistence (const TEL_TRANSFORM_PERSISTENCE* theTransPers)
{
  const TEL_TRANSFORM_PERSISTENCE* aTransPersPrev = myTransPers;
  myTransPers = theTransPers;
  if (theTransPers->mode == 0)
  {
    EndTransformPersistence();
    return aTransPersPrev;
  }

  GLint aViewport[4];
  GLdouble aModelMatrix[4][4];
  GLdouble aProjMatrix[4][4];
  glGetIntegerv (GL_VIEWPORT,          aViewport);
  glGetDoublev  (GL_MODELVIEW_MATRIX,  (GLdouble* )aModelMatrix);
  glGetDoublev  (GL_PROJECTION_MATRIX, (GLdouble *)aProjMatrix);
  const GLdouble aViewportW = (GLdouble )aViewport[2];
  const GLdouble aViewportH = (GLdouble )aViewport[3];

  if (myIsTransPers)
  {
    // pop matrix stack - it will be overridden later
    glMatrixMode (GL_PROJECTION);
    glPopMatrix();
    glMatrixMode (GL_MODELVIEW);
    glPopMatrix();
  }
  else
  {
    myIsTransPers = Standard_True;
  }

  // push matrices into stack and reset them
  glMatrixMode (GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glMatrixMode (GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();

  // get the window's (fixed) coordinates for theTransPers->point before matrixes modifications
  GLdouble aWinX = 0.0, aWinY = 0.0, aWinZ = 0.0;
  if ((theTransPers->mode & TPF_PAN) != TPF_PAN)
  {
    gluProject (theTransPers->pointX, theTransPers->pointY, theTransPers->pointZ,
                (GLdouble* )aModelMatrix, (GLdouble* )aProjMatrix, aViewport,
                &aWinX, &aWinY, &aWinZ);
  }

  // prevent zooming
  if ((theTransPers->mode & TPF_ZOOM)
   || (theTransPers->mode == TPF_TRIEDRON))
  {
    // compute fixed-zoom multiplier
    // actually function works ugly with TelPerspective!
    const GLdouble aDet2 = 0.002 / (aViewportW > aViewportH ? aProjMatrix[1][1] : aProjMatrix[0][0]);
    aProjMatrix[0][0] *= aDet2;
    aProjMatrix[1][1] *= aDet2;
    aProjMatrix[2][2] *= aDet2;
  }

  // prevent translation - annulate translate matrix
  if ((theTransPers->mode & TPF_PAN)
   || (theTransPers->mode == TPF_TRIEDRON))
  {
    aModelMatrix[3][0] = 0.0;
    aModelMatrix[3][1] = 0.0;
    aModelMatrix[3][2] = 0.0;
    aProjMatrix [3][0] = 0.0;
    aProjMatrix [3][1] = 0.0;
    aProjMatrix [3][2] = 0.0;
  }

  // prevent scaling-on-axis
  if (theTransPers->mode & TPF_ZOOM)
  {
    const double aScaleX = myExtra.scaleFactors[0];
    const double aScaleY = myExtra.scaleFactors[1];
    const double aScaleZ = myExtra.scaleFactors[2];
    for (int i = 0; i < 3; ++i)
    {
      aModelMatrix[0][i] /= aScaleX;
      aModelMatrix[1][i] /= aScaleY;
      aModelMatrix[2][i] /= aScaleZ;
    }
  }

  // prevent rotating - annulate rotate matrix
  if (theTransPers->mode & TPF_ROTATE)
  {
    aModelMatrix[0][0] = 1.0;
    aModelMatrix[1][1] = 1.0;
    aModelMatrix[2][2] = 1.0;

    aModelMatrix[1][0] = 0.0;
    aModelMatrix[2][0] = 0.0;
    aModelMatrix[0][1] = 0.0;
    aModelMatrix[2][1] = 0.0;
    aModelMatrix[0][2] = 0.0;
    aModelMatrix[1][2] = 0.0;
  }

  // load computed matrices
  glMatrixMode (GL_MODELVIEW);
  glMultMatrixd ((GLdouble* )aModelMatrix);

  glMatrixMode (GL_PROJECTION);
  glMultMatrixd ((GLdouble* )aProjMatrix);

  if (theTransPers->mode == TPF_TRIEDRON)
  {
    // move to the window corner
    if (theTransPers->pointX != 0.0
     && theTransPers->pointY != 0.0)
    {
      GLdouble aW1, aH1, aW2, aH2, aDummy;
      glMatrixMode (GL_PROJECTION);
      gluUnProject ( 0.5 * aViewportW,  0.5 * aViewportH, 0.0,
                    (GLdouble* )THE_IDENTITY_MATRIX, (GLdouble* )aProjMatrix, aViewport,
                    &aW1, &aH1, &aDummy);
      gluUnProject (-0.5 * aViewportW, -0.5 * aViewportH, 0.0,
                    (GLdouble* )THE_IDENTITY_MATRIX, (GLdouble* )aProjMatrix, aViewport,
                    &aW2, &aH2, &aDummy);
      GLdouble aMoveX = 0.5 * (aW1 - aW2 - theTransPers->pointZ);
      GLdouble aMoveY = 0.5 * (aH1 - aH2 - theTransPers->pointZ);
      aMoveX = (theTransPers->pointX > 0.0) ? aMoveX : -aMoveX;
      aMoveY = (theTransPers->pointY > 0.0) ? aMoveY : -aMoveY;
      glTranslated (aMoveX, aMoveY, 0.0);
    }
  }
  else if ((theTransPers->mode & TPF_PAN) != TPF_PAN)
  {
    // move to thePoint using saved win-coordinates ('marker-behaviour')
    GLdouble aMoveX, aMoveY, aMoveZ;
    glGetDoublev (GL_MODELVIEW_MATRIX,  (GLdouble* )aModelMatrix);
    glGetDoublev (GL_PROJECTION_MATRIX, (GLdouble* )aProjMatrix);
    gluUnProject (aWinX, aWinY, aWinZ,
                  (GLdouble* )aModelMatrix, (GLdouble* )aProjMatrix, aViewport,
                  &aMoveX, &aMoveY, &aMoveZ);

    glMatrixMode (GL_MODELVIEW);
    glTranslated (aMoveX, aMoveY, aMoveZ);
  }

  return aTransPersPrev;
}
