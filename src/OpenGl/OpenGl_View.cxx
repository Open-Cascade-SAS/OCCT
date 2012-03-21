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

#include <OpenGl_Workspace.hxx>
#include <OpenGl_Display.hxx>

#include <OpenGl_Display.hxx>
#include <OpenGl_Trihedron.hxx>
#include <OpenGl_GraduatedTrihedron.hxx>

#include <OpenGl_transform_persistence.hxx>

#include <GL/glu.h> // gluUnProject()

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

/*----------------------------------------------------------------------*/

OpenGl_View::OpenGl_View (const CALL_DEF_VIEWCONTEXT &AContext)
: myTextureEnv(0),
  mySurfaceDetail(Visual3d_TOD_NONE),
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
  myAnimationListIndex(0),
  myAnimationListReady(Standard_False),
  myTransPers(&myDefaultTransPers),
  myIsTransPers(Standard_False),
  myResetFLIST(Standard_False)
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
  if (myAnimationListIndex)
    glDeleteLists((GLuint)myAnimationListIndex,1);

  if ( myBgTexture.TexId != 0 )
    glDeleteTextures( 1, (GLuint*)&(myBgTexture.TexId) );
}

/*----------------------------------------------------------------------*/

void OpenGl_View::SetBackfacing (const Standard_Integer AMode)
{
  myBackfacing = AMode;
  myResetFLIST = Standard_True;
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
void OpenGl_View::SetClipLimit (const CALL_DEF_VIEW &ACView)
{
  myZClip.Back.Limit =
    ( ACView.Context.ZClipBackPlane - ACView.Mapping.BackPlaneDistance ) /
    ( ACView.Mapping.FrontPlaneDistance - ACView.Mapping.BackPlaneDistance );
  myZClip.Front.Limit =
    ( ACView.Context.ZClipFrontPlane - ACView.Mapping.BackPlaneDistance ) /
    ( ACView.Mapping.FrontPlaneDistance - ACView.Mapping.BackPlaneDistance );
  if ( myZClip.Back.Limit < 0.F )
    myZClip.Back.Limit = 0.F;
  if ( myZClip.Front.Limit > 1.F )
    myZClip.Front.Limit = 1.F;
  if ( myZClip.Back.Limit > myZClip.Front.Limit )
  {
    myZClip.Back.Limit = 0.F;
    myZClip.Front.Limit = 1.F;
  }

  myZClip.Back.IsOn = (ACView.Context.BackZClipping != 0);
  myZClip.Front.IsOn = (ACView.Context.FrontZClipping != 0);
}

/*----------------------------------------------------------------------*/

//call_togl_viewmapping
void OpenGl_View::SetMapping (const CALL_DEF_VIEW &ACView)
{
  const float ratio = ACView.DefWindow.dy / ACView.DefWindow.dx;
  const float r_ratio = ACView.DefWindow.dx / ACView.DefWindow.dy;

  TEL_VIEW_MAPPING Map;

  Map.window.xmin = ACView.Mapping.WindowLimit.um;
  Map.window.ymin = ACView.Mapping.WindowLimit.vm;
  Map.window.xmax = ACView.Mapping.WindowLimit.uM;
  Map.window.ymax = ACView.Mapping.WindowLimit.vM;

  Map.viewport.xmin = 0.F;
  Map.viewport.xmax = ( 1.F < r_ratio ? 1.F : r_ratio );
  Map.viewport.ymin = 0.F;
  Map.viewport.ymax = ( 1.F < ratio ? 1.F : ratio );
  Map.viewport.zmin = 0.F;
  Map.viewport.zmax = 1.F;

  /* projection type */
  switch( ACView.Mapping.Projection )
  {
    case 0 :
      Map.proj = TelPerspective;
      break;
    case 1 :
      Map.proj = TelParallel;
      break;
  }

  /* projection reference point */
  Map.prp[0] = ACView.Mapping.ProjectionReferencePoint.x;
  Map.prp[1] = ACView.Mapping.ProjectionReferencePoint.y;
  Map.prp[2] = ACView.Mapping.ProjectionReferencePoint.z;
  if (!openglDisplay.IsNull() && !openglDisplay->Walkthrough())
    Map.prp[2] += ACView.Mapping.FrontPlaneDistance;

  /* view plane distance */
  Map.vpd = ACView.Mapping.ViewPlaneDistance;

  /* back plane distance */
  Map.bpd = ACView.Mapping.BackPlaneDistance;

  /* front plane distance */
  Map.fpd = ACView.Mapping.FrontPlaneDistance;

  Tint err_ind = 0;

  /* use user-defined matrix */
  if ( ACView.Mapping.IsCustomMatrix )
  {
    int i, j;
    for( i = 0; i < 4; i++ )
      for( j = 0; j < 4; j++ )
        myMappingMatrix[i][j] = ACView.Mapping.ProjectionMatrix[i][j];
  }
  else 
    TelEvalViewMappingMatrix( &Map, &err_ind, myMappingMatrix );

  if ( !err_ind )
    myExtra.map = Map;
}

/*----------------------------------------------------------------------*/

//call_togl_vieworientation
void OpenGl_View::SetOrientation (const CALL_DEF_VIEW &ACView)
{
  Tfloat Vrp[3];
  Tfloat Vpn[3];
  Tfloat Vup[3];
  Tfloat ScaleFactors[3];

  Vrp[0] = ACView.Orientation.ViewReferencePoint.x;
  Vrp[1] = ACView.Orientation.ViewReferencePoint.y;
  Vrp[2] = ACView.Orientation.ViewReferencePoint.z;

  Vpn[0] = ACView.Orientation.ViewReferencePlane.x;
  Vpn[1] = ACView.Orientation.ViewReferencePlane.y;
  Vpn[2] = ACView.Orientation.ViewReferencePlane.z;

  Vup[0] = ACView.Orientation.ViewReferenceUp.x;
  Vup[1] = ACView.Orientation.ViewReferenceUp.y;
  Vup[2] = ACView.Orientation.ViewReferenceUp.z;

  ScaleFactors[0] = ACView.Orientation.ViewScaleX;
  ScaleFactors[1] = ACView.Orientation.ViewScaleY;
  ScaleFactors[2] = ACView.Orientation.ViewScaleZ;

  Tint err_ind = 0;

  // use user-defined matrix
  if ( ACView.Orientation.IsCustomMatrix )
  {
    int i, j;
    for( i = 0; i < 4; i++ )
      for( j = 0; j < 4; j++ )
        myOrientationMatrix[i][j] = ACView.Orientation.ModelViewMatrix[i][j];
  }
  else
    TelEvalViewOrientationMatrix( Vrp, Vpn, Vup, ScaleFactors, &err_ind, myOrientationMatrix );

  if ( !err_ind )
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

void OpenGl_View::SetFog (const CALL_DEF_VIEW &ACView, const Standard_Boolean AFlag)
{
  if( !AFlag )
  {
    myFog.IsOn = Standard_False;
  }
  else
  {
    myFog.IsOn = Standard_True;

    myFog.Front =
      (ACView.Context.DepthFrontPlane - ACView.Mapping.BackPlaneDistance) /
      (ACView.Mapping.FrontPlaneDistance - ACView.Mapping.BackPlaneDistance);

    myFog.Back =
      (ACView.Context.DepthBackPlane - ACView.Mapping.BackPlaneDistance) /
      (ACView.Mapping.FrontPlaneDistance - ACView.Mapping.BackPlaneDistance);

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

    myFog.Color.rgb[0] = ACView.DefWindow.Background.r;
    myFog.Color.rgb[1] = ACView.DefWindow.Background.g;
    myFog.Color.rgb[2] = ACView.DefWindow.Background.b;
    myFog.Color.rgb[3] = 1.F;
  }
}

/*----------------------------------------------------------------------*/

void OpenGl_View::TriedronDisplay (const Aspect_TypeOfTriedronPosition APosition, const Quantity_NameOfColor AColor,
                                  const Standard_Real AScale, const Standard_Boolean AsWireframe)
{
  myTrihedron = new OpenGl_Trihedron (APosition, AColor, AScale, AsWireframe);
}

/*----------------------------------------------------------------------*/

void OpenGl_View::TriedronErase ()
{
  myTrihedron.Nullify();
}

/*----------------------------------------------------------------------*/

void OpenGl_View::GraduatedTrihedronDisplay (const Graphic3d_CGraduatedTrihedron &data)
{
  myGraduatedTrihedron = new OpenGl_GraduatedTrihedron(data);
}

/*----------------------------------------------------------------------*/

void OpenGl_View::GraduatedTrihedronErase ()
{
  myGraduatedTrihedron.Nullify();
}

/*----------------------------------------------------------------------*/

//transform_persistence_end
void OpenGl_View::EndTransformPersistence ()
{
  if ( myIsTransPers )
  {
    /* restore matrix */
    glMatrixMode (GL_PROJECTION);
    glPopMatrix ();
    glMatrixMode (GL_MODELVIEW);
    glPopMatrix ();
    myIsTransPers = Standard_False;
  }
} 

/*----------------------------------------------------------------------*/

//transform_persistence_begin
const TEL_TRANSFORM_PERSISTENCE * OpenGl_View::BeginTransformPersistence (const TEL_TRANSFORM_PERSISTENCE *ATransPers)
{
  const TEL_TRANSFORM_PERSISTENCE *TransPers_old = myTransPers;

  if ( ATransPers->mode == 0 )
  {
    EndTransformPersistence();
    return TransPers_old;
  }

  myTransPers = ATransPers;

  GLint viewport[4];
  glGetIntegerv (GL_VIEWPORT, viewport);
  GLdouble modelMatrix[4][4];
  glGetDoublev( GL_MODELVIEW_MATRIX,  (GLdouble *) modelMatrix );
  GLdouble projMatrix[4][4];
  glGetDoublev( GL_PROJECTION_MATRIX, (GLdouble *) projMatrix );

  double W = viewport[2];
  double H = viewport[3];

  if ( myIsTransPers )
  {
    /* restore matrix */
    glMatrixMode (GL_PROJECTION);
    glPopMatrix ();
    glMatrixMode (GL_MODELVIEW);
    glPopMatrix ();
  }
  else
    myIsTransPers = Standard_True;

  glMatrixMode( GL_MODELVIEW );
  glPushMatrix();
  glLoadIdentity();

  glMatrixMode( GL_PROJECTION );
  glPushMatrix();
  glLoadIdentity();

  /*pre loading matrix*/
  if( ATransPers->mode & TPF_PAN )
    /* Annulate translate matrix */
  {
    modelMatrix[3][0] = 0.;
    modelMatrix[3][1] = 0.;
    modelMatrix[3][2] = 0.;
    projMatrix[3][0] = 0.;
    projMatrix[3][1] = 0.;
    projMatrix[3][2] = 0.;
  }

  if( ATransPers->mode & TPF_ZOOM )
    /* Annulate zoom matrix */
  {
    const double scaleX = myExtra.scaleFactors[0];
    const double scaleY = myExtra.scaleFactors[1];
    const double scaleZ = myExtra.scaleFactors[2];

    for (int i = 0; i < 3; ++i)
    {
      modelMatrix[0][i] /= scaleX;
      modelMatrix[1][i] /= scaleY;
      modelMatrix[2][i] /= scaleZ;
    }

    const double det2 = 0.002 / ( W > H ? projMatrix[1][1] : projMatrix[0][0]);
    projMatrix[0][0] *= det2;
    projMatrix[1][1] *= det2;
  }

  if( ATransPers->mode & TPF_ROTATE )
    /* Annulate rotate matrix */
  {
    modelMatrix[0][0] = 1.;
    modelMatrix[1][1] = 1.;
    modelMatrix[2][2] = 1.;

    modelMatrix[1][0] = 0.;
    modelMatrix[2][0] = 0.;
    modelMatrix[0][1] = 0.;
    modelMatrix[2][1] = 0.;
    modelMatrix[0][2] = 0.;
    modelMatrix[1][2] = 0.;
  }
  else if( ATransPers->mode & TPF_RELATIVE_ROTATE )
    /* Initialize relative rotate matrix*/
  {
    modelMatrix[3][0] = 0.;
    modelMatrix[3][1] = 0.;
    modelMatrix[3][2] = 0.;

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    glTranslated( ATransPers->pointX, ATransPers->pointY, ATransPers->pointZ );        
  }

  if( ATransPers->mode == TPF_TRIEDRON )
  {
    /* Annulate translation matrix */
    modelMatrix[3][0] = 0.;
    modelMatrix[3][1] = 0.;
    modelMatrix[3][2] = 0.;

    projMatrix[3][0] = 0.;
    projMatrix[3][1] = 0.;
    projMatrix[3][2] = 0.;

    const double det2 = 0.002 / ( W > H ? projMatrix[1][1] : projMatrix[0][0]);
    projMatrix[0][0] *= det2;
    projMatrix[1][1] *= det2;
  }

  /* load matrix */
  glMatrixMode (GL_MODELVIEW);
  glMultMatrixd ((GLdouble *) modelMatrix);

  glMatrixMode (GL_PROJECTION);
  glMultMatrixd ((GLdouble *) projMatrix);

  /*post loading matrix*/
  if( ATransPers->mode == TPF_TRIEDRON )
  {
    glMatrixMode( GL_PROJECTION );

	double winx, winy, winz;
    const GLdouble idenMatrix[4][4] = { {1.,0.,0.,0.}, {0.,1.,0.,0.}, {0.,0.,1.,0.}, {0.,0.,0.,1.} };

    gluUnProject( W/2., H/2., 0., (GLdouble*)idenMatrix, (GLdouble*)projMatrix, (GLint*)viewport, &winx, &winy, &winz);
    double W1, H1;
    W1 = winx;
    H1 = winy;
    gluUnProject( -W/2., -H/2., 0., (GLdouble*)idenMatrix, (GLdouble*)projMatrix, (GLint*)viewport, &winx, &winy, &winz);
    double W2, H2;
    W2 = winx;
    H2 = winy;

    if( ATransPers->pointX == 0. && ATransPers->pointY == 0. )
    {
      /*center*/
    }
    else if( ATransPers->pointX > 0. && ATransPers->pointY > 0. )
    {
      /*right upper*/
      glTranslated( 0.5*(W1 - W2 - ATransPers->pointZ), 0.5*(H1 - H2 - ATransPers->pointZ), 0. );
    }
    else if( ATransPers->pointX > 0. && ATransPers->pointY < 0. )
    {
      /*right lower*/
      glTranslated( 0.5*(W1 - W2 - ATransPers->pointZ), 0.5*(H2 - H1 + ATransPers->pointZ), 0. );
    }
    else if( ATransPers->pointX < 0. && ATransPers->pointY > 0. )
    {
      /*left upper*/
      glTranslated( 0.5*(W2 - W1 + ATransPers->pointZ), 0.5*(H1 - H2 - ATransPers->pointZ), 0. );
    }
    else if( ATransPers->pointX < 0 && ATransPers->pointY < 0 )
    {
      /*left lower*/
      glTranslated( -(W1 - W2)/2. + ATransPers->pointZ/2., -(H1-H2)/2. + ATransPers->pointZ/2., 0. );
    }
  }

  return TransPers_old;
}

/*----------------------------------------------------------------------*/
