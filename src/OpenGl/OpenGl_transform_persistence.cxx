
#include <OpenGl_transform_persistence.hxx>

#include <OpenGl_tgl_all.hxx>

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h> /* pour M_PI */

#include <GL/gl.h>
#include <GL/glu.h>

/* ABD 29/10/04  Transform Persistence of Presentation( pan, zoom, rotate ) */

#include <OpenGl_cmn_varargs.hxx>
#include <OpenGl_telem_attri.hxx>
#include <OpenGl_tsm.hxx>
#include <OpenGl_telem.hxx>
#include <OpenGl_telem_inquire.hxx>
#include <OpenGl_tsm_ws.hxx>  /* pour TsmGetWSAttri */
#include <OpenGl_telem_view.hxx>  /* pour tel_view_data */


#include <OpenGl_tgl_funcs.hxx>

#include <OpenGl_Memory.hxx>


/*----------------------------------------------------------------------*/
/*
* Type definitions
*/

typedef  struct
{
  TEL_VIEW_REP    vrep;
#ifdef CAL_100498
  Tmatrix3        inverse_matrix;/* accelerates UVN2XYZ conversion */
#endif
} TEL_VIEW_DATA, *tel_view_data;   /* Internal data stored for every view rep */



static  TStatus  TransformPersistenceDisplay( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  TransformPersistenceAdd( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  TransformPersistenceDelete( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  TransformPersistencePrint( TSM_ELEM_DATA, Tint, cmn_key* );

static  TStatus  (*MtdTbl[])( TSM_ELEM_DATA, Tint, cmn_key* ) =
{
  TransformPersistenceDisplay,             /* PickTraverse */
  TransformPersistenceDisplay,
  TransformPersistenceAdd,
  0,                                       /*TransformPersistenceDelete,*/
  TransformPersistencePrint,
  0                                        /* Inquire */
};


MtblPtr
TelTransformPersistentInitClass( TelType *el )
{
  *el = TelTransformPersistence;
  return MtdTbl;
}

static  TStatus
TransformPersistenceAdd( TSM_ELEM_DATA d, Tint n, cmn_key *k )
{
  tel_transform_persistence data = new TEL_TRANSFORM_PERSISTENCE();

  if( !data )
    return TFailure;

  *data = *(tel_transform_persistence)(k[0]->data.pdata);

  ((tsm_elem_data)(d.pdata))->pdata = data;

  return TSuccess;
}


static  TStatus
TransformPersistenceDisplay( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  CMN_KEY  key;

  key.id = TelTransformPersistence;
  key.data.pdata = data.pdata;
  TsmSetAttri( 1, &key );
  return TSuccess;
}

static  TStatus
TransformPersistenceDelete( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  /*    transform_persistence_end();*/
  return TSuccess;
}


static  TStatus
TransformPersistencePrint( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{

  return TSuccess;
}

/***********************************/
static int isGenerated = 0;

void transform_persistence_end(void)
{
  if( isGenerated == 1 )
    /* restore matrix */
  {
    glMatrixMode (GL_PROJECTION);
    glPopMatrix ();
    glMatrixMode (GL_MODELVIEW);
    glPopMatrix ();
    isGenerated = 0;
  }

  /*printf( "Transform Persistence End: %i \n", isGenerated );*/
} 


void transform_persistence_begin( int theFlag, 
                                 float theX, 
                                 float theY, 
                                 float theZ )
{
  CMN_KEY_DATA key;
  tel_view_data   vptr;
  int i, j;
  GLdouble modelMatrix[4][4];
  GLdouble projMatrix[4][4];
  GLdouble idenMatrix[4][4] = { {1.,0.,0.,0.}, {0.,1.,0.,0.}, {0.,0.,1.,0.},{0.,0.,0.,1.} };
  GLint  viewport[4];
  double det2;

  double W, H, W1, W2, H1, H2;
  double winx, winy, winz;
  double scaleX, scaleY, scaleZ;

#ifdef PRINT1
  printf( "Transform Persistence Mode: %i\n", theFlag );
  printf( "Transform Persistence PointX: %f\n", theX );
  printf( "Transform Persistence PointY: %f\n", theY );
  printf( "Transform Persistence PointZ: %f\n", theZ );
#endif /*PRINT1*/

  if( theFlag == 0 )
  {
    transform_persistence_end();
    return;
  }    

  glGetIntegerv (GL_VIEWPORT, viewport);
  glGetDoublev( GL_MODELVIEW_MATRIX,  (GLdouble *) modelMatrix );
  glGetDoublev( GL_PROJECTION_MATRIX, (GLdouble *) projMatrix );

  W = viewport[2];
  H = viewport[3];

  if( isGenerated == 0 )
    isGenerated = 1;
  else
  {
    /* restore matrix */
    glMatrixMode (GL_PROJECTION);
    glPopMatrix ();
    glMatrixMode (GL_MODELVIEW);
    glPopMatrix ();
  }

  glMatrixMode( GL_MODELVIEW );
  glPushMatrix();
  glLoadIdentity();

  glMatrixMode( GL_PROJECTION );
  glPushMatrix();
  glLoadIdentity();

  /*pre loading matrix*/
  if( theFlag & TPF_PAN )
    /* Annulate translate matrix */
  {
    modelMatrix[3][0] = 0.;
    modelMatrix[3][1] = 0.;
    modelMatrix[3][2] = 0.;
    projMatrix[3][0] = 0.;
    projMatrix[3][1] = 0.;
    projMatrix[3][2] = 0.;
  }

  if( theFlag & TPF_ZOOM )
    /* Annulate zoom matrix */
  {
    if( W > H )
      det2 = 0.002 / projMatrix[1][1];
    else
      det2 = 0.002 / projMatrix[0][0];

    TsmGetWSAttri( TglActiveWs, WSViews, &key );
    vptr = (tel_view_data)key.pdata ; /* Obtain defined view data*/
    scaleX = vptr->vrep.extra.scaleFactors[0];
    scaleY = vptr->vrep.extra.scaleFactors[1];
    scaleZ = vptr->vrep.extra.scaleFactors[2];

    for( i = 0; i < 3; i++ )
    {
      for( j = 0; j < 3; j++ )
      {
        if(i == 0)
          modelMatrix[i][j] /= scaleX;
        if(i == 1)
          modelMatrix[i][j] /= scaleY;
        if(i == 2)
          modelMatrix[i][j] /= scaleZ;
      }
    }
    for( i = 0 ; i < 2; i++ )
      projMatrix[i][i] *= det2;
  }

  if( theFlag & TPF_ROTATE )
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
  else if( theFlag & TPF_RELATIVE_ROTATE )
    /* Initialize relative rotate matrix*/
  {
    modelMatrix[3][0] = 0.;
    modelMatrix[3][1] = 0.;
    modelMatrix[3][2] = 0.;

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    glTranslated( theX, theY, theZ );        
  }

  if( theFlag == TPF_TRIEDRON )
  {
    /* Annulate translation matrix */
    modelMatrix[3][0] = 0.;
    modelMatrix[3][1] = 0.;
    modelMatrix[3][2] = 0.;

    projMatrix[3][0] = 0.;
    projMatrix[3][1] = 0.;
    projMatrix[3][2] = 0.;

    if( W > H )
      det2 = 0.002 / projMatrix[1][1];
    else
      det2 = 0.002 / projMatrix[0][0];

    for( i = 0 ; i < 2; i++ )
      projMatrix[i][i] *= det2;
  }

  /* load matrix */
  glMatrixMode (GL_MODELVIEW);
  glMultMatrixd( (GLdouble *) modelMatrix);

  glMatrixMode ( GL_PROJECTION );
  glMultMatrixd( (GLdouble *) projMatrix);


  /*post loading matrix*/
  if( theFlag == TPF_TRIEDRON )
  {
    glMatrixMode( GL_PROJECTION );

    gluUnProject( W/2, H/2., 0, (GLdouble*)idenMatrix, (GLdouble*)projMatrix, (GLint*)viewport, &winx, &winy, &winz);
    W1 = winx;
    H1 = winy;
    gluUnProject( -W/2, -H/2., 0, (GLdouble*)idenMatrix, (GLdouble*)projMatrix, (GLint*)viewport, &winx, &winy, &winz);
    W2 = winx;
    H2 = winy;

    if( theX == 0. && theY == 0. )
    {
      /*center*/
    }
    else if( theX > 0. && theY > 0. )
    {
      /*right upper*/
      glTranslated( (W1 - W2)/2. - theZ/2., (H1-H2)/2. - theZ/2., 0. );
    }
    else if( theX > 0. && theY < 0. )
    {
      /*right lower*/
      glTranslated( (W1 - W2)/2. - theZ/2., -(H1-H2)/2. + theZ/2., 0. );
    }
    else if( theX < 0. && theY > 0. )
    {
      /*left upper*/
      glTranslated( -(W1 - W2)/2. + theZ/2., (H1-H2)/2. - theZ/2., 0. );
    }
    else if( theX < 0 && theY < 0 )
    {
      /*left lower*/
      glTranslated( -(W1 - W2)/2. + theZ/2., -(H1-H2)/2. + theZ/2., 0. );
    }
  }
}
