/***********************************************************************

FONCTION :
----------
File OpenGl_attri :


REMARQUES:
---------- 

ATTENTION: Actuellement les materiaux ont le Front et Back identiques.
(cf PRO6327). On met temporairement de cote le mode Distinguish.


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : xxx ; Creation.
06-03-96 : FMN ; Ajout OpenGl_tgl_tox.h pour call_thedisplay
01-04-96 : CAL ; Integration MINSK portage WNT
12-04-96 : CAL ; Travail sur les materiaux
07-05-96 : CAL ; Debug temporaire pour ROB, TelRobSetRGB ()
10-06-96 : CAL ; Transparence dans TsmSetAttri
13-06-96 : CAL ; Introduction de TsmPushAttriLight et TsmPopAttriLight
28-06-96 : FMN ; Amelioration des performances sur les materiaux
01-07-96 : FMN ; Correction + performance antialiasing;
03-07-96 : FMN ; Test sur ZBuffer avant appel a TelUpdateMaterial.
17-07-96 : FMN ; Suppression de HLHSRMode.
07-10-96 : FMN ; Suppression appel TelUpdateMaterial et define TELCO.
08-11-96 : FMN ; Suppression code CAL_OPTIM
21-11-96 : FMN ; Correction PRO6181 et PRO4138 Suppression (balise FMN)
05-12-96 : FMN ; PRO6327: Pas defaut on positionne les materiaux pour
le front et le back. 
15-02-97 : FMN ; Suppression OpenGl_gl_compat.h
25-02-97 : FMN ; Ajout print informations
30-06-97 : FMN ; Appel de la toolkit OpenGl_LightBox.
03-07-97 : PCT ; Modif material.
05-08-97 : PCT ; support texture mapping
21-08-97 : PCT ; ajout deuxieme passe pour les textures
08-12-97 : FMN ; Correction Flat/Gouraud
09-12-97 : FMN ; Correction Antialing + Textures + Transparence
22-12-97 : FMN ; Suppression DEBUG_TEMPO_FOR_ROB
Correction PRO10217 sur le mode Transient
23-12-97 : FMN ; Suppression TelBackInteriorStyle, TelBackInteriorStyleIndex
et TelBackInteriorShadingMethod
30-12-97 : FMN ; CTS18312: Correction back material
31-12-97 : FMN ; Optimisation gestion material
15-01-98 : FMN ; Ajout Hidden line
08-04-98 : FGU ; Ajout emission
30-06-98 : FMN ; Correction init des materiaux dans TelUpdateMateriaux.
08-07-98 : FMN ; PRO14399: Meilleure gestion de la matrice locale sur
une structure.
21-07-98 : FMN ; Correction init attri_default et du hatch
30-11-98 : FMN ; S4069 : Textes toujours visibles
05-01-99 : CAL ; Warning WNT
22-03-04 : SAN ; OCC4895 High-level interface for controlling polygon offsets

************************************************************************/

#define BUC60577        /*GG_101099     Enable to compute correctly
//                      transparency with more than one object in the view.
*/

#define IMP190100       /*GG     Push and Pop polyline type and width
//                      attributes correctly.
*/

#define G003            /*EUG   degeneration mode management
*/

#define GER61394        /*GG 26/05/00 Enable AntiAliasing also on polygons.*/

#define BUC60742        /*GG 18/09/00 Enable to use both animation and 
graphic structure transformation
*/

#define IMP250401       /*GG    Sets always the current polyline colour 
used in immediat mode context.
*/

/*OCC8568 changes made by GG rolled back due to regression*/
/*#define BUC60918*/        /*GG CADPAK_V2/RINA Enable transparency using Zbuffer
                            */

/*----------------------------------------------------------------------*/
/*
* Constantes
*/

#define NO_TRACE                   
#define NO_TRACE_UPMATERIAL   
#define NO_TRACE_MATERIAL          
#define NO_TRACE_LIGHT      
#define NO_TRACE_MATRIX     
#define NO_TRACE_TEXT       

/*----------------------------------------------------------------------*/
/*
* Includes
*/

/* OCC22218 NOTE: project dependency on gl2ps is specified by macro */
#ifdef HAVE_GL2PS
  #include <gl2ps.h>
  /* OCC22216 NOTE: linker dependency can be switched off by undefining macro. 
     Pragma comment for gl2ps.lib is defined only here. */ 
  #ifdef _MSC_VER 
  #pragma comment( lib, "gl2ps.lib" )
  #endif
#endif

#include <OpenGl_tgl_all.hxx>

#include <stdio.h>
#include <string.h>

#include <GL/gl.h>

#include <OpenGl_cmn_varargs.hxx>
#include <OpenGl_telem_attri.hxx>
#include <OpenGl_telem_view.hxx>
#include <OpenGl_telem_depthcue.hxx>
#include <OpenGl_telem.hxx>
#include <OpenGl_telem_util.hxx>
#include <OpenGl_tsm.hxx>
#include <OpenGl_tsm_ws.hxx>
#include <OpenGl_tgl.hxx>
#include <OpenGl_tgl_util.hxx>
#include <OpenGl_tgl_tox.hxx>
#include <OpenGl_animation.hxx>

#include <OpenGl_trsf_stack.hxx>
#include <OpenGl_tgl_funcs.hxx>

#include <OpenGl_TextureBox.hxx>

/* OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets */
#include <Aspect_PolygonOffsetMode.hxx>
/* OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets */

/* ABD 29/10/04  Transform Persistence of Presentation( pan, zoom, rotate ) */
#include <OpenGl_transform_persistence.hxx>
/* ABD 29/10/04  Transform Persistence of Presentation( pan, zoom, rotate ) */

#include <OpenGl_Memory.hxx>

#include <OpenGl_TextRender.hxx>
#include <OSD_FontAspect.hxx> 

                                                                          
/*----------------------------------------------------------------------*/
/*
* Definitions type
*/

struct  TSM_ATTRI_BLK_STRUCT {
  //struct          TSM_ATTRI_BLK_STRUCT  *next;
  /* ABD 29/10/04  Transform Persistence of Presentation( pan, zoom, rotate ) */
  /*Tint            TransPersFlag;*/
  TEL_TRANSFORM_PERSISTENCE TransPers;
  /* ABD 29/10/04  Transform Persistence of Presentation( pan, zoom, rotate ) */
  TEL_COLOUR      PolylineColour;
  Tfloat          PolylineWidth;
  Tint            PolylineType;
  TEL_COLOUR      EdgeColour;
  Tfloat          EdgeWidth;
  Tint            EdgeType;
  Tint            AntiAliasing;
  TEL_COLOUR      InteriorColour;
  Tint            EdgeFlag;
  Tint            InteriorStyle;
  Tint            InteriorStyleIndex;
  Tfloat          PolymarkerSize;
  TEL_COLOUR      PolymarkerColour;
  TEL_COLOUR      BackInteriorColour;
  Tint            FaceDistinguishingMode;
  TEL_COLOUR      TextColour;
  TEL_COLOUR      TextColourSubTitle;
  Tint            TextStyle;
  Tint            TextDisplayType;
  Tchar           *TextFont;
  Tfloat          TextHeight;
  Tfloat          CharacterSpacing;
  Tfloat          CharacterExpansionFactor;
  Tint            TextZoomable;
  Tfloat          TextAngle; 
  OSD_FontAspect  TextFontAspect;   
  /*OCC7456 abd 14.12.2004 Text alingnment attributes  */
  TEL_ALIGN_DATA  TextAlign;
  /*OCC7456 abd 14.12.2004 Text alingnment attributes  */
  Tint            PolymarkerType;
  Tint            InteriorReflectanceEquation;
  Tint            BackInteriorReflectanceEquation;
  TEL_SURF_PROP   SurfaceAreaProperties;
  TEL_SURF_PROP   BackSurfaceAreaProperties;
  TelCullMode     FaceCullingMode;
  Tint            DepthCueIndex;
  Tint            InteriorShadingMethod;
  Tint            ViewIndex;
  Tint            HighlightIndex;
  Tmatrix3        LocalTran3;
  Tint            DoTextureMap;
  Tint            TextureId;
#ifdef G003
  Tint            DegenerationMode;
#endif  /* G003 */
  /* OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets */
  TEL_POFFSET_PARAM PolygonOffsetParams;
  /* OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets */

  IMPLEMENT_MEMORY_OPERATORS
};
typedef TSM_ATTRI_BLK_STRUCT  TSM_ATTRI_BLK;
typedef TSM_ATTRI_BLK_STRUCT* tsm_attri_blk;


/*----------------------------------------------------------------------*/
/*
* Variables statiques
*/
typedef NCollection_Stack<tsm_attri_blk>  Attri_Stack;
static Attri_Stack attri_stk;

/* FSXXX */
static  Tint need_reset = 0;

/* Est-il necessaire de faire de la transparence ? */
static  Tint need_trans = 0;

static  TSM_ATTRI_BLK  attri_default =
{
  {0 , 0.0F, 0.0F, 0.0F },
    /* ABD 29/10/04  Transform Persistence of Presentation( pan, zoom, rotate ) */
  {{ 1.0F, 1.0F, 1.0F }},   /* PolylineColour */
  1.0F,                   /* PolylineWidth */
  TEL_LS_SOLID,           /* PolylineType */
  {{ 1.0F, 1.0F, 1.0F }},   /* EdgeColour */
  1.0F,                   /* EdgeWidth */
  0,                      /* EdgeType */
  TOff,                   /* AntiAliasing */
  {{ 1.0F, 1.0F, 1.0F }},   /* InteriorColour */
  TOn,                    /* EdgeFlag */
  TSM_SOLID,              /* InteriorStyle */
  TEL_HS_SOLID,           /* InteriorStyleIndex */
  1.0F,                   /* PolymarkerSize */
  {{ 1.0F, 1.0F, 1.0F }},   /* PolymarkerColour */
  {{ 1.0F, 1.0F, 1.0F }},   /* BackInteriorColour */
  TOn,                    /* FaceDistinguishingMode */
  {{ 1.0F, 1.0F, 1.0F }},   /* TextColour */
  {{ 1.0F, 1.0F, 1.0F }},   /* TextColourSubTitle */
  ASPECT_TOST_NORMAL,     /* TextStyle */
  ASPECT_TODT_NORMAL,     /* TextDisplayType */
  (Tchar*)"Courier",  /* TextFont */
  16.0F,                  /* TextHeight */
  0.3F,                   /* CharacterSpacing */
  1.0F,                   /* CharacterExpansionFactor */ 
  0,                      /*TextZoomable*/
  0,                      /*TextAngle*/ 
  OSD_FA_Regular,         /*TextFontAspect - OSD_FA_Regular*/
  /*OCC7456 abd 14.12.2004 Text alingnment attributes  */
  { 0,                    /*Horizontal Text Alignment*/
    0 },                  /*Vertical Text Alignment*/
  /*OCC7456 abd 14.12.2004 Text alingnment attributes  */
  TEL_PM_PLUS,            /* PolymarkerType */
  CALL_PHIGS_REFL_NONE,      /* InteriorReflectanceEquation */
  CALL_PHIGS_REFL_NONE,      /* BackInteriorReflectanceEquation */
  { 0.2F, 0.8F, 0.1F, 0.0F,  /* Coef d eclairage */
  1.0F, 10.0F, 0.0F,  
  1,  1,  1,  0, 0,          /* Etat des eclairage */
  {{ 1.0F, 1.0F, 1.0F }},    /* ambient color */
  {{ 1.0F, 1.0F, 1.0F }},    /* diffuse color */
  {{ 1.0F, 1.0F, 1.0F }},    /* specular color */
  {{ 1.0F, 1.0F, 1.0F }},    /* emissive color */
  {{ 1.0F, 1.0F, 1.0F }}},   /* material color */       /* SurfaceAreaProperties */
  { 0.2F, 0.8F, 0.1F, 0.0F,  /* Coef d eclairage */
  1.0F, 10.0F, 0.0F,
  1,  1,  1,  0, 0,          /* Etat des eclairage */
  {{ 1.0F, 1.0F, 1.0F }},    /* ambient color */
  {{ 1.0F, 1.0F, 1.0F }},    /* diffuse color */
  {{ 1.0F, 1.0F, 1.0F }},    /* specular color */
  {{ 1.0F, 1.0F, 1.0F }},    /* emissive color */
  {{ 1.0F, 1.0F, 1.0F }}},   /* material color */      /* BackSurfaceAreaProperties */
  TelCullNone,            /* FaceCullingMode */
  0,                      /* DepthCueIndex */
  TEL_SM_GOURAUD,         /* InteriorShadingMethod */
  0,                      /* ViewIndex */
  0,                      /* HighlightIndex */
  {
    { 1.0F, 0.0F, 0.0F, 0.0F },
    { 0.0F, 1.0F, 0.0F, 0.0F },
    { 0.0F, 0.0F, 1.0F, 0.0F },
    { 0.0F, 0.0F, 0.0F, 1.0F }
  },                     /* LocalTran3 */
    TEL_SD_SHADING,
    -1
#ifdef G003
    , 0  /* DegenerationMode */
#endif  /* G003 */
    /* OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets */
    , { Aspect_POM_Fill, 1.0F, 0.0F }
    /* OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets */
};

#ifdef G003
extern int g_nBackfacing;
#endif  /* G003 */

/*----------------------------------------------------------------------*/
/*
* Prototypes
*/

static  void  TelUpdateFog( Tint, Tint, Tint ); /* ws, vid, dcid */
static  void  TelSetViewMatrix( Tmatrix3, Tint ); /* mat, vid */
static  void  call_util_transpose_mat (float tmat[16], float mat[4][4]);
static  void  TelUpdatePolygonOffsets( tel_poffset_param );

/*----------------------------------------------------------------------*/
/*
* Variables externes
*/

extern tsm_trsf_stack cur_trsf_stack;

GLuint fontBase;
extern GLuint linestyleBase;
extern GLuint patternBase;

extern  Tint  ForbidSetTextureMapping; /* currently defined in tsm/tsm.c */
extern  Tint  SecondPassNeeded;     /* currently defined in tsm/tsm.c */
extern  Tint  SecondPassDo;         /* currently defined in tsm/tsm.c */

/*----------------------------------------------------------------------*/


TStatus
TsmPushAttri( void )
{
  tsm_attri_blk node;

  node = new TSM_ATTRI_BLK();

  if( !attri_stk.IsEmpty() )
  {
    *node = *attri_stk.Top();
#ifdef TRACE
    printf("TsmPushAttri %x\n",attri_tail);
#endif
  }
  else
  {
    *node = attri_default;
#ifdef TRACE
    printf("TsmPushAttri attri_default %x\n",attri_tail);
#endif

    /* Register all default attributes with gl */

    glLineWidth( ( GLfloat )attri_default.PolylineWidth );
#ifdef HAVE_GL2PS
    gl2psLineWidth( ( GLfloat )attri_default.PolylineWidth );
#endif

    OpenGl_TextRender* textRender=OpenGl_TextRender::instance();
    textRender->FindFont(node->TextFont, node->TextFontAspect, node->TextHeight,1,1);

#ifdef G003
    if ( !g_nBackfacing ) 
#endif  /* G003 */
      switch (node->FaceCullingMode) {
case TelCullNone:
  glDisable (GL_CULL_FACE);
  break;

case TelCullFront:
  glCullFace (GL_FRONT);
  glEnable (GL_CULL_FACE);
  break;

case TelCullBack:
  glCullFace (GL_BACK);
  glEnable (GL_CULL_FACE);
  break;
      }
      /* ABD 29/10/04  Transform Persistence of Presentation( pan, zoom, rotate ) */
      transform_persistence_begin( 0, 0.0, 0.0, 0.0 );
      /* ABD 29/10/04  Transform Persistence of Presentation( pan, zoom, rotate ) */

  }
  attri_stk.Push( node );
  return TSuccess;
}

/*----------------------------------------------------------------------*/

TStatus
TsmPopAttri( void )
{
  tsm_attri_blk  node;

  if ( attri_stk.IsEmpty() )
    return TFailure; /* Underflow */

  node = attri_stk.ChangeTop();
  attri_stk.Pop();

#ifdef TRACE
  printf("TsmPopAttri %x\n",attri_tail);
#endif

  if( !attri_stk.IsEmpty() )
  {
    tsm_attri_blk attri_tail = attri_stk.Top();

    /* Reset the attributes */
    Tint    i;
    Tfloat  f;

    f = attri_tail->PolylineWidth;
    if( node->PolylineWidth != f ) {
#ifdef TRACE
      printf("TsmPopAttri.linewidth %f\n",f);
#endif

      glLineWidth( (GLfloat)f );
#ifdef HAVE_GL2PS
      gl2psLineWidth( (GLfloat)f );
#endif
    }

    i = attri_tail->InteriorStyle;
    if( node->InteriorStyle != i )
    {
      switch( i )
      {
      case TSM_SOLID:
      case TSM_HIDDENLINE:
        glPolygonMode(GL_FRONT_AND_BACK,  GL_FILL );
        glDisable(GL_POLYGON_STIPPLE);
        break;

      case TSM_HOLLOW:
        glPolygonMode(GL_FRONT_AND_BACK,  GL_LINE );
        break;

      case TSM_EMPTY:
        glPolygonMode(GL_FRONT_AND_BACK,  GL_LINE );
        break;

      case TSM_HATCH:
        glPolygonMode(GL_FRONT_AND_BACK,  GL_FILL );
        if( node->InteriorStyleIndex ) {
          glCallList( patternBase + node->InteriorStyleIndex );
          glEnable(GL_POLYGON_STIPPLE);
        } else
          glDisable(GL_POLYGON_STIPPLE);
        break;

      case TSM_POINT:
        glPolygonMode(GL_FRONT_AND_BACK,  GL_POINT );
        break;
      }
    }

    i = attri_tail->PolylineType;
    if( node->PolylineType != i ) {
#ifdef TRACE
      printf("TsmPopAttri.linetype %d\n",i);
#endif
      if (i) {
        glCallList( linestyleBase+i );

        glEnable( GL_LINE_STIPPLE );
#ifdef HAVE_GL2PS
        gl2psEnable( GL2PS_LINE_STIPPLE );
#endif
      } else {

        glDisable( GL_LINE_STIPPLE );
#ifdef HAVE_GL2PS
        gl2psDisable( GL2PS_LINE_STIPPLE );
#endif
      }
    }

    if( strcmp( (char*)attri_tail->TextFont, (char*)node->TextFont ) ||
      ( attri_tail->TextHeight != node->TextHeight ) )
    {

      OpenGl_TextRender*  textRender =  OpenGl_TextRender::instance();
      textRender->FindFont(attri_tail -> TextFont, attri_tail->TextFontAspect, attri_tail -> TextHeight);
    }
#ifdef G003
    if ( !g_nBackfacing )
#endif  /* G003 */
    {
      if( attri_tail->FaceCullingMode != node->FaceCullingMode )
      {
        switch( attri_tail->FaceCullingMode )
        {
        case  TelCullNone:
          glDisable(GL_CULL_FACE);
          break;

        case  TelCullFront:
          glCullFace(GL_FRONT) ;
          glEnable(GL_CULL_FACE);
          break;

        case  TelCullBack:
          glCullFace(GL_BACK);
          glEnable(GL_CULL_FACE);
          break;
        }
      }
    }

    /* OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets */
    if( attri_tail->PolygonOffsetParams.mode   != node->PolygonOffsetParams.mode   ||
        attri_tail->PolygonOffsetParams.factor != node->PolygonOffsetParams.factor ||
        attri_tail->PolygonOffsetParams.units  != node->PolygonOffsetParams.units )
    {
      TelUpdatePolygonOffsets( &attri_tail->PolygonOffsetParams );
    }
    /* OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets */

    /* ABD 29/10/04  Transform Persistence of Presentation( pan, zoom, rotate ) */
    if( attri_tail->TransPers.mode != node->TransPers.mode ||
        attri_tail->TransPers.pointX != node->TransPers.pointX ||
        attri_tail->TransPers.pointY != node->TransPers.pointY ||
        attri_tail->TransPers.pointZ != node->TransPers.pointX )

    {
      transform_persistence_begin( attri_tail->TransPers.mode, 
                                   attri_tail->TransPers.pointX, 
                                   attri_tail->TransPers.pointY, 
                                   attri_tail->TransPers.pointZ );
    }
    /* ABD 29/10/04  Transform Persistence of Presentation( pan, zoom, rotate ) */

    if( attri_tail->ViewIndex != node->ViewIndex )
    {
      TelSetViewProjection( TglActiveWs, attri_tail->ViewIndex );
#ifdef TRACE_MATRIX
      printf("TsmPopAttri::TelSetViewMatrix::attri_tail->ViewIndex != node->ViewIndex \n");
#endif
      TelSetViewMatrix( attri_tail->LocalTran3, attri_tail->ViewIndex );
      TelUpdateFog( TglActiveWs, attri_tail->ViewIndex, attri_tail->DepthCueIndex );
    }
    else
    {
      if( attri_tail->DepthCueIndex != node->DepthCueIndex )
        TelUpdateFog( TglActiveWs, attri_tail->ViewIndex, attri_tail->DepthCueIndex );
      if( memcmp( node->LocalTran3, attri_tail->LocalTran3, sizeof(Tmatrix3) ) )
      {
#ifdef TRACE_MATRIX
        printf("TsmPopAttri::TelSetViewMatrix::memcmp( node->LocalTran3, attri_tail->LocalTran3, sizeof(Tmatrix3) ) \n");
#endif
        TelSetViewMatrix(attri_tail->LocalTran3,attri_tail->ViewIndex); 
      }
    }
  }
#ifdef IMP190100        /* Reset default attribs at the top */
  else {
    Tint    i;
    Tfloat  f;

    f = attri_default.PolylineWidth;
    if( node->PolylineWidth != f ) {


      glLineWidth( (GLfloat)f );
#ifdef HAVE_GL2PS
      gl2psLineWidth( (GLfloat)f );
#endif

#ifdef TRACE
      printf("TsmPopAttri.defaultlinewidth %f\n",f);
#endif
    }

    i = attri_default.PolylineType;
    if( node->PolylineType != i ) {
      if (i) {
        glCallList( linestyleBase+i );

        glEnable( GL_LINE_STIPPLE );
#ifdef HAVE_GL2PS
        gl2psEnable( GL2PS_LINE_STIPPLE );
#endif
      } else {

        glDisable( GL_LINE_STIPPLE );
#ifdef HAVE_GL2PS
        gl2psDisable( GL2PS_LINE_STIPPLE );
#endif
      }


#ifdef TRACE
      printf("TsmPopAttri.defaultlinetype %d\n",i);
#endif
    }
    /* OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets */
    TelUpdatePolygonOffsets( &attri_default.PolygonOffsetParams );
    /* OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets */

    /* ABD 29/10/04  Transform Persistence of Presentation( pan, zoom, rotate ) */
    /*transform_persistence_begin( attri_default.TransPersFlag );*/
    transform_persistence_begin( attri_default.TransPers.mode, 
                                 attri_default.TransPers.pointX, 
                                 attri_default.TransPers.pointY, 
                                 attri_default.TransPers.pointZ );
    /* ABD 29/10/04  Transform Persistence of Presentation( pan, zoom, rotate ) */
  }
#endif  /*IMP190100*/

  /* abd
  cmn_stg_tbl_free( (void *) node );*/
  delete node;

  return TSuccess;
}

/*----------------------------------------------------------------------*/

TStatus
TsmSetAttri( Tint n, ... )
{
  Tint      i;

#if defined (SOLARIS) || defined (IRIXO32)
  cmn_key  *k;
#else
  cmn_key  k[TMaxArgs];
#endif

#ifdef GER61394
  static int antiAliasingMode = -1;
  if( antiAliasingMode < 0 ) {
    char string[64];
    if (call_util_osd_getenv("CALL_OPENGL_ANTIALIASING_MODE", string, 64) ) {
      int v;
      if( sscanf(string,"%d",&v) > 0 ) antiAliasingMode = v;
      printf(".OpenGl driver.AntiAliasing mode is %d\n",antiAliasingMode);
    } else {
      antiAliasingMode = 3;
    }
  } 
#endif

  if (attri_stk.IsEmpty())
    return TFailure;

  tsm_attri_blk attri_tail = attri_stk.ChangeTop();

  CMN_GET_STACK( n, k );
  for( i = 0; i < n; i++ )
  {
    switch( k[i]->id )
    {
    case  TelTextureId:
      attri_tail->TextureId = k[i]->data.ldata;

      if (!ForbidSetTextureMapping) {
        if (attri_tail->DoTextureMap)
        {
          DisableTexture();
          SetCurrentTexture(attri_tail->TextureId);           
          EnableTexture();
        }
        else
          DisableTexture();
      }
      break;

    case TelDoTextureMap:
      attri_tail->DoTextureMap = k[i]->data.ldata;
      break;

    case TelTextStyle:
      attri_tail->TextStyle = k[i]->data.ldata;
      break;

    case TelTextDisplayType:
      attri_tail->TextDisplayType = k[i]->data.ldata;
      break;

    case  TelPolylineColour:
      attri_tail->PolylineColour = *(tel_colour)(k[i]->data.pdata);
#ifdef IMP250401
      glColor3f(attri_tail->PolylineColour.rgb[0],
                attri_tail->PolylineColour.rgb[1],
                attri_tail->PolylineColour.rgb[2]);
#endif
      break;

    case  TelPolylineWidth:
      {
        Tfloat  w;

        w = k[i]->data.fdata;
        if( attri_tail->PolylineWidth != w )
        {
#ifdef TRACE
          printf("TsmSetAttri.linewidth %f\n",w);
#endif
          attri_tail->PolylineWidth = w;

          glLineWidth( (GLfloat)w );
#ifdef HAVE_GL2PS
          gl2psLineWidth( (GLfloat)w );
#endif
        }
        break;
      }

    case  TelPolylineType:
      {
        Tint  ls;

        ls = k[i]->data.ldata;
        if( attri_tail->PolylineType != ls )
        {
#ifdef TRACE
          printf("TsmSetAttri.linetype %d\n",ls);
#endif
          attri_tail->PolylineType = ls;
          if (ls) {
            glCallList(linestyleBase+ls);

            glEnable( GL_LINE_STIPPLE );
#ifdef HAVE_GL2PS
            gl2psEnable( GL2PS_LINE_STIPPLE );
#endif
          } else {

            glDisable( GL_LINE_STIPPLE );
#ifdef HAVE_GL2PS
            gl2psDisable( GL2PS_LINE_STIPPLE );
#endif
          }
        }
        break;
      }

    case  TelEdgeType:
      attri_tail->EdgeType = k[i]->data.ldata;
      break;

    case  TelEdgeWidth:
      attri_tail->EdgeWidth = k[i]->data.fdata;
      break;

    case  TelEdgeColour:
      attri_tail->EdgeColour = *(tel_colour)(k[i]->data.pdata);
      break;

    case  TelAntiAliasing:
      {
        attri_tail->AntiAliasing = k[i]->data.ldata;

        if (!SecondPassNeeded)
        {
          if( attri_tail->AntiAliasing == TOff )
          {
            glDisable(GL_POINT_SMOOTH);
            glDisable(GL_LINE_SMOOTH);
#ifdef GER61394
            if( antiAliasingMode & 2 ) glDisable(GL_POLYGON_SMOOTH);
#endif
            glBlendFunc (GL_ONE, GL_ZERO);
            glDisable (GL_BLEND);
          }
          else /* TOn */
          {
            glEnable(GL_POINT_SMOOTH);
            glEnable(GL_LINE_SMOOTH);
#ifdef GER61394
            if( antiAliasingMode & 2 ) glEnable(GL_POLYGON_SMOOTH);
#endif
            glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
            glEnable (GL_BLEND);
          }
        }
        break;
      }

    case  TelInteriorColour:
      {
        attri_tail->InteriorColour = *(tel_colour)(k[i]->data.pdata);
        break;
      }

    case  TelEdgeFlag:
      {
        attri_tail->EdgeFlag = k[i]->data.ldata;
        break;
      }

    case  TelInteriorStyle:
      {
        Tint  s;

        s = k[i]->data.ldata;
        if( attri_tail->InteriorStyle != s )
        {
          attri_tail->InteriorStyle = s;
          switch( s )
          {
          case TSM_SOLID:
          case TSM_HIDDENLINE:
            glPolygonMode(GL_FRONT_AND_BACK,  GL_FILL );
            glDisable(GL_POLYGON_STIPPLE);
            break;

          case TSM_HOLLOW:
            glPolygonMode(GL_FRONT_AND_BACK,  GL_LINE );
            break;

          case TSM_EMPTY:
            glPolygonMode(GL_FRONT_AND_BACK,  GL_LINE );
            break;

          case TSM_HATCH:
            glPolygonMode(GL_FRONT_AND_BACK,  GL_FILL );
            if( attri_tail->InteriorStyleIndex ) {
              glCallList( patternBase + attri_tail->InteriorStyleIndex );
              glEnable(GL_POLYGON_STIPPLE);
            } else
              glDisable(GL_POLYGON_STIPPLE);
            break;

          case TSM_POINT:
            glPolygonMode(GL_FRONT_AND_BACK,  GL_POINT );
            break;
          }
        }
        break;
      }

    case  TelInteriorStyleIndex:
      {
        Tint  is = k[i]->data.ldata;
        if( attri_tail->InteriorStyleIndex != is )
        {
          attri_tail->InteriorStyleIndex = is;
          if( attri_tail->InteriorStyle == TSM_HATCH )
          {
            if (is)
            {
              glCallList( patternBase + is );
              glEnable(GL_POLYGON_STIPPLE);
            } else
              glDisable(GL_POLYGON_STIPPLE);
          }
        }
        break;
      }

    case  TelPolymarkerColour:
      attri_tail->PolymarkerColour = *(tel_colour)(k[i]->data.pdata);
      break;

    case  TelPolymarkerSize:
      attri_tail->PolymarkerSize = k[i]->data.fdata;
      break;

    case  TelLocalTran3:
      {
        tel_matrix3_data  d = ((tel_matrix3_data)(k[i]->data.pdata));
        switch(d->mode)
        {
        case  TPreConcatenate: /* not used */
          TelMultiplymat3( attri_tail->LocalTran3,
            d->mat, attri_tail->LocalTran3 );
          break;

        case  TPostConcatenate: /* not used */
          TelMultiplymat3( attri_tail->LocalTran3,
            attri_tail->LocalTran3, d->mat );
          break;

        case  TReplace:
#ifdef BUC60742
          matcpy( attri_tail->LocalTran3, d->mat );
          TelTransposemat3( attri_tail->LocalTran3 );
#else
          /*
          * On multiplie la trsf courante par la trsf
          * de la structure que l'on est en train de
          * parcourir.
          */
          TelMultiplymat3( cur_trsf_stack->LocalTran3,
            cur_trsf_stack->LocalTran3, d->mat );
          /*
          * On copie de resultat dans la queue d'attributs
          * pour pouvoir changer l'orientation du reste
          * du trace.
          */
          matcpy( attri_tail->LocalTran3,
            cur_trsf_stack->LocalTran3 );
          /*
          * Transformation definie dans un repere direct
          * a appliquer dans un repere indirect
          */
          TelTransposemat3( attri_tail->LocalTran3 );
#endif  /*BUC60742*/
          break;
        }
#ifdef TRACE_MATRIX
        printf("TsmSetAttri::TelSetViewMatrix::TelLocalTran3 \n");
#endif
        TelSetViewMatrix(attri_tail->LocalTran3,attri_tail->ViewIndex);
        break;
      }

    case  TelBackInteriorColour:
      {
        attri_tail->BackInteriorColour = *(tel_colour)(k[i]->data.pdata);
        break;
      }

    case  TelFaceDistinguishingMode:
      {
        attri_tail->FaceDistinguishingMode = (Tint) k[i]->data.ldata;
        break;
      }

    case  TelTextColour:
      attri_tail->TextColour = *(tel_colour)(k[i]->data.pdata);
      break;

    case  TelTextColourSubTitle:
      attri_tail->TextColourSubTitle = *(tel_colour)(k[i]->data.pdata);
      break;

    case  TelTextFont:
      {
        Tchar *fn = (Tchar*)k[i]->data.pdata;
#ifdef TRACE_TEXT
        printf ("TelTextFont:: %s %s \n", fn, attri_tail->TextFont);
#endif
        if( strcmp( (char*)fn, (char*)attri_tail->TextFont ) )
        {
          OpenGl_TextRender* textRender = OpenGl_TextRender::instance();
          textRender->FindFont ( fn, attri_tail->TextFontAspect, attri_tail -> TextHeight );
          attri_tail->TextFont = fn;
        }
        break;
      }

    case  TelTextHeight:
      {
        Tfloat         h;

        h = k[i]->data.fdata;
#ifdef TRACE_TEXT
        printf ("TelTextHeight:: %f %f \n", h, attri_tail->TextHeight);
#endif
        if( h != attri_tail->TextHeight )
        {
          OpenGl_TextRender* textRender = OpenGl_TextRender::instance();
          textRender->FindFont (attri_tail->TextFont, attri_tail->TextFontAspect,h );
          attri_tail->TextHeight = h;
        }
        break;
      }

    case  TelCharacterSpacing:
      attri_tail->CharacterSpacing = k[i]->data.fdata;
      break;

    case  TelCharacterExpansionFactor:
      {
        attri_tail->CharacterExpansionFactor = k[i]->data.fdata;
        break;
      }

    case TelTextZoomable:
      attri_tail->TextZoomable = k[i]->data.ldata;  
      break;
    case TelTextAngle:
      attri_tail->TextAngle = k[i]->data.ldata;
      break;  
    case TelTextFontAspect:
      attri_tail->TextFontAspect = OSD_FontAspect(k[i]->data.ldata);
      break;                   

      /*OCC7456 abd 14.12.2004 Text alingnment attributes  */
    case TelTextAlign:
      attri_tail->TextAlign = *(tel_align_data)k[i]->data.pdata;
      break;
      /*OCC7456 abd 14.12.2004 Text alingnment attributes  */

    case  TelPolymarkerType:
      attri_tail->PolymarkerType = k[i]->data.ldata;
      break;

    case  TelInteriorReflectanceEquation:
      attri_tail->InteriorReflectanceEquation =
        (Tint) k[i]->data.ldata;
      break;

    case  TelBackInteriorReflectanceEquation:
      attri_tail->BackInteriorReflectanceEquation =
        (Tint) k[i]->data.ldata;
      break;

    case  TelSurfaceAreaProperties:
      attri_tail->SurfaceAreaProperties =
        *(tel_surf_prop)(k[i]->data.pdata);
#ifdef TRACE_UPMATERIAL
      printf("TsmSetAttri::TelSurfaceAreaProperties:TelUpdateMaterial (TEL_FRONT_MATERIAL)\n");
#endif
      TelUpdateMaterial( TEL_FRONT_MATERIAL );
      break;

    case  TelBackSurfaceAreaProperties:
      attri_tail->BackSurfaceAreaProperties =
        *(tel_surf_prop)(k[i]->data.pdata);
#ifdef TRACE_UPMATERIAL
      printf("TsmSetAttri::TelBackSurfaceAreaProperties:TelUpdateMaterial (TEL_BACK_MATERIAL)\n");
#endif
      if (attri_tail->FaceDistinguishingMode == TOn)
        TelUpdateMaterial( TEL_BACK_MATERIAL ); 
      break;

    case  TelFaceCullingMode:
#ifdef G003
      if ( !g_nBackfacing )
#endif  /* G003 */
      {
        TelCullMode mode = (TelCullMode) k[i]->data.ldata;
        if( mode != attri_tail->FaceCullingMode )
        {
          attri_tail->FaceCullingMode = mode;
          switch( mode )
          {
          case  TelCullNone:
            glDisable(GL_CULL_FACE);
            break;

          case  TelCullFront:
            glCullFace(GL_FRONT);
            glEnable(GL_CULL_FACE);
            break;

          case  TelCullBack:
            glCullFace(GL_BACK);
            glEnable(GL_CULL_FACE);
            break;
          }
        }
      }
      break;

    case TelDepthCueIndex:
      {
        Tint dc;

        dc = k[i]->data.ldata;
        if( dc != attri_tail->DepthCueIndex )
        {
          attri_tail->DepthCueIndex = dc;
          TelUpdateFog( TglActiveWs, attri_tail->ViewIndex, dc );
        }
        break;
      }

    case  TelInteriorShadingMethod:
      {
        Tint  a;

        a = k[i]->data.ldata;
        attri_tail->InteriorShadingMethod = a;
        a == TEL_SM_FLAT ? glShadeModel( GL_FLAT ) :
        glShadeModel( GL_SMOOTH );
        break;
      }

    case  TelViewIndex:
      {
        Tint  vid = k[i]->data.ldata;

        if( vid != attri_tail->ViewIndex )
        {
          attri_tail->ViewIndex = vid;
          TelUpdateFog( TglActiveWs, vid, attri_tail->DepthCueIndex );
        }
        break;
      }

    case  TelHighlightIndex:
      attri_tail->HighlightIndex = k[i]->data.ldata;
      break;
#ifdef G003
    case TelDegenerationMode:
      attri_tail -> DegenerationMode = k[ i ] -> data.ldata;
      break;
#endif  /* G003 */

      /* OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets */
    case TelPolygonOffset:
      {
        tel_poffset_param pdata = (tel_poffset_param)(k[i]->data.pdata);
        /* Aspect_POM_None means: do not change current settings */
        if ( ( pdata->mode & Aspect_POM_None ) == Aspect_POM_None )
          break;
        attri_tail->PolygonOffsetParams = *pdata;
        TelUpdatePolygonOffsets( pdata );
        break;
      }
      /* OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets */


    case TelTransformPersistence:
      {
        attri_tail->TransPers = *(tel_transform_persistence)(k[i]->data.pdata);
        transform_persistence_begin( attri_tail->TransPers.mode, 
          attri_tail->TransPers.pointX,
          attri_tail->TransPers.pointY,
          attri_tail->TransPers.pointZ );

        break;
      }
      /* ABD 29/10/04  Transform Persistence of Presentation( pan, zoom, rotate ) */

    default:
      break;
    }
  }
  return TSuccess;
}

/*----------------------------------------------------------------------*/

TStatus
TsmGetAttri( Tint n, ... )
{
  Tint      i;
#if defined (SOLARIS) || defined (IRIXO32)
  cmn_key  *k;
#else
  cmn_key  k[TMaxArgs];
#endif

  tsm_attri_blk attri_tail = NULL;

  if (attri_stk.IsEmpty())  
  {
#ifdef TRACE
    printf ("TsmGetAttri with attri_tail == NULL\n");
#endif
    attri_tail = &attri_default;
  } else {
    attri_tail = attri_stk.Top();
  }

  CMN_GET_STACK( n, k );
  for( i = 0; i < n; i++ )
  {
    switch( k[i]->id )
    {
    case  TelTextureId:
      k[i]->data.ldata = attri_tail->TextureId;
      break;

    case  TelDoTextureMap:
      k[i]->data.ldata = attri_tail->DoTextureMap;
      break;

    case  TelTextStyle:
      k[i]->data.ldata = attri_tail->TextStyle;
      break;

    case  TelTextDisplayType:
      k[i]->data.ldata = attri_tail->TextDisplayType;
      break;

    case  TelPolylineColour:
      *(tel_colour)(k[i]->data.pdata) = attri_tail->PolylineColour;
      break;

    case  TelPolylineWidth:
      k[i]->data.fdata = attri_tail->PolylineWidth;   
      break;

    case  TelPolylineType:
      k[i]->data.ldata = attri_tail->PolylineType;
      break;

    case  TelEdgeType:
      k[i]->data.ldata = attri_tail->EdgeType;
      break;

    case  TelEdgeWidth:
      k[i]->data.fdata = attri_tail->EdgeWidth;     
      break;

    case  TelEdgeColour:
      *(tel_colour)(k[i]->data.pdata) = attri_tail->EdgeColour;
      break;

    case  TelAntiAliasing:
      k[i]->data.ldata = attri_tail->AntiAliasing;
      break;

    case  TelInteriorColour:
      *(tel_colour)(k[i]->data.pdata) = attri_tail->InteriorColour;
      break;

    case  TelEdgeFlag:
      k[i]->data.ldata = attri_tail->EdgeFlag;
      break;

    case  TelInteriorStyle:
      k[i]->data.ldata = attri_tail->InteriorStyle;
      break;

    case  TelInteriorStyleIndex:
      k[i]->data.ldata = attri_tail->InteriorStyleIndex;
      break;

    case  TelPolymarkerColour:
      *(tel_colour)(k[i]->data.pdata) = attri_tail->PolymarkerColour;
      break;

    case  TelPolymarkerSize:
      k[i]->data.fdata = attri_tail->PolymarkerSize;   
      break;

    case  TelLocalTran3:
      matcpy( k[i]->data.pdata, attri_tail->LocalTran3 );
      break;

    case  TelBackInteriorColour:
      *(tel_colour)(k[i]->data.pdata) = attri_tail->BackInteriorColour;
      break;

    case  TelFaceDistinguishingMode:
      k[i]->data.ldata = attri_tail->FaceDistinguishingMode;
      break;

    case  TelTextColour:
      *(tel_colour)(k[i]->data.pdata) = attri_tail->TextColour;
      break;

    case  TelTextColourSubTitle:
      *(tel_colour)(k[i]->data.pdata) = attri_tail->TextColourSubTitle;
      break;

    case  TelTextFont:
      k[i]->data.pdata = attri_tail->TextFont;
      break;

    case  TelTextHeight:
      k[i]->data.fdata = attri_tail->TextHeight;          
      break;

    case  TelCharacterSpacing:
      k[i]->data.fdata = attri_tail->CharacterSpacing;   
      break;

    case  TelCharacterExpansionFactor:
      k[i]->data.fdata = attri_tail->CharacterExpansionFactor;   
      break;

    case TelTextZoomable:
      k[i]->data.ldata = attri_tail->TextZoomable;
      break;
    case TelTextAngle:
      k[i]->data.ldata = attri_tail->TextAngle;
      break;   
    case TelTextFontAspect:
      k[i]->data.ldata = attri_tail->TextFontAspect;
      break;    
      /*OCC7456 abd 14.12.2004 Text alingnment attributes  */
    case TelTextAlign:
      *(tel_align_data)(k[i]->data.pdata) = attri_tail->TextAlign;
      break;
      /*OCC7456 abd 14.12.2004 Text alingnment attributes  */
    case  TelPolymarkerType:
      k[i]->data.ldata = attri_tail->PolymarkerType;
      break;

    case  TelInteriorReflectanceEquation:
      k[i]->data.ldata = attri_tail->InteriorReflectanceEquation;
      break;

    case  TelBackInteriorReflectanceEquation:
      k[i]->data.ldata = attri_tail->BackInteriorReflectanceEquation;
      break;

    case  TelSurfaceAreaProperties:
      *(tel_surf_prop)(k[i]->data.pdata) =
        attri_tail->SurfaceAreaProperties;
      break;

    case  TelBackSurfaceAreaProperties:
      *(tel_surf_prop)(k[i]->data.pdata) =
        attri_tail->BackSurfaceAreaProperties;
      break;

    case  TelFaceCullingMode:
      k[i]->data.ldata = attri_tail->FaceCullingMode;
      break;

    case TelDepthCueIndex:
      k[i]->data.ldata = attri_tail->DepthCueIndex;
      break;

    case  TelInteriorShadingMethod:
      k[i]->data.ldata = attri_tail->InteriorShadingMethod;
      break;

    case  TelViewIndex:
      k[i]->data.ldata = attri_tail->ViewIndex;
      break;

    case  TelHighlightIndex:
      k[i]->data.ldata = attri_tail->HighlightIndex;
      break;

#ifdef G003
    case TelDegenerationMode:
      k[ i ] -> data.ldata = attri_tail -> DegenerationMode;
      break;
#endif  /* G003 */

      /* OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets */
    case TelPolygonOffset:
      *(tel_poffset_param)(k[i]->data.pdata) = attri_tail->PolygonOffsetParams;
      break;
      /* OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets */

    case  TelTransformPersistence:
      *(tel_transform_persistence)(k[i]->data.pdata) = attri_tail->TransPers;
      break;
      /* ABD 29/10/04  Transform Persistence of Presentation( pan, zoom, rotate ) */             

    default:
      break;
    }
  }

  return TSuccess;
}

/*----------------------------------------------------------------------*/

void TelUpdateMaterial( Tint flag )  
{
  GLenum        face=0;
  tel_colour    col=NULL;
  tel_surf_prop prop=NULL;
  Tint rm=CALL_PHIGS_REFL_NONE;
  static float  mAmb[4];
  static float  mDiff[4];
  static float  mSpec[4];
  static float  mEmsv[4];
  static float  mShin;
  static float  r, g, b;
  static int    physic;
  CMN_KEY_DATA key;
  Tfloat *rgb;
  Tfloat newDiff3;

  if (attri_stk.IsEmpty())
    return;

  tsm_attri_blk attri_tail = attri_stk.ChangeTop();

  /* Hors Hiddenline */
  if (attri_tail->InteriorStyle != TSM_HIDDENLINE)
  {
    if( flag == TEL_FRONT_MATERIAL ) {
      col  = &attri_tail->InteriorColour;
      rm   = attri_tail->InteriorReflectanceEquation;
      prop = &attri_tail->SurfaceAreaProperties;
      face = GL_FRONT_AND_BACK;       
    }
    if( flag == TEL_BACK_MATERIAL ) {
      col  = &attri_tail->BackInteriorColour;
      rm   = attri_tail->BackInteriorReflectanceEquation;
      prop = &attri_tail->BackSurfaceAreaProperties;
      face = GL_BACK;        
    }
  }
  /* Cas HiddenLine */
  else
  {
#ifdef TRACE_UPMATERIAL
    printf("OpenGl_attri::TelUpdateMaterial HiddenLine\n");
#endif
    TsmGetWSAttri (TglActiveWs, WSBackground, &key);
    rgb = (Tfloat*)key.pdata;

    attri_tail->InteriorColour.rgb[0]=rgb[0];
    attri_tail->InteriorColour.rgb[1]=rgb[1];
    attri_tail->InteriorColour.rgb[2]=rgb[2];
    attri_tail->InteriorReflectanceEquation = CALL_PHIGS_REFL_NONE;
    attri_tail->BackInteriorReflectanceEquation = CALL_PHIGS_REFL_NONE;        
    return;  
  }

  /* Type material */   
  physic = prop->isphysic;

#ifdef TRACE_UPMATERIAL
  if( flag == TEL_FRONT_MATERIAL ) printf("OpenGl_attri::TelUpdateMaterial TEL_FRONT_MATERIAL\n");
  else printf("OpenGl_attri::TelUpdateMaterial TEL_BACK_MATERIAL\n");
#endif
#ifdef TRACE_MATERIAL
  printf("    InteriorReflectanceEquation: %d\n", attri_tail->InteriorReflectanceEquation);
  printf(" controle mode rm == CALL_PHIGS_REFL_NONE : %d\n",  rm == CALL_PHIGS_REFL_NONE);
#endif

  if( rm == CALL_PHIGS_REFL_NONE ) return;

  /* 
  * Gestion de la transparence 
  */

  if (!SecondPassDo) {
    /* Passage en transparence */
    if (need_trans && prop->trans != 1.0F) {
      glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
      glEnable (GL_BLEND);
      glDepthMask (GL_FALSE);
    }

    /* Passage en opaque */
#ifdef BUC60577
    else 
#else 
    if (need_trans && prop->trans == 1.0F)
#endif
    {
      if( attri_tail->AntiAliasing == TOff )
      {
        glBlendFunc (GL_ONE, GL_ZERO);
        glDisable (GL_BLEND);
      }
      glDepthMask (GL_TRUE);
    }       
  }

  /* 
  * Reset Material
  */
  if( need_reset ) 
  {   
    /* Composant ambient */
    if( rm & AMBIENT_MASK )
    {
      /* Type materiau */
      if( physic )
      {r=prop->ambcol.rgb[0];g=prop->ambcol.rgb[1];b=prop->ambcol.rgb[2];}
      else
      {r=col->rgb[0];g=col->rgb[1];b=col->rgb[2];}

      mAmb[0] = prop->amb * r;
      mAmb[1] = prop->amb * g;
      mAmb[2] = prop->amb * b;
      mAmb[3] = 1.0F;
    }
    else
    {
      mAmb[0] = 0.0;
      mAmb[1] = 0.0;
      mAmb[2] = 0.0;
      mAmb[3] = 1.0F;
    }

    /* Composant diffus */
    if( rm & DIFFUSE_MASK )
    {
      /* Type materiau */
      if( physic )
      {r=prop->difcol.rgb[0];g=prop->difcol.rgb[1];b=prop->difcol.rgb[2];}
      else
      {r=col->rgb[0];g=col->rgb[1];b=col->rgb[2];}

      mDiff[0] = prop->diff * r;
      mDiff[1] = prop->diff * g;
      mDiff[2] = prop->diff * b;
      mDiff[3] = 1.0F;
    }
    else
    {
      mDiff[0] = 0.0;
      mDiff[1] = 0.0;
      mDiff[2] = 0.0;
      mDiff[3] = 1.0F;
    }

    if (SecondPassDo)
    {
      mDiff[3] = prop->env_reflexion; 
    }
    else
    {
      if (need_trans) mDiff[3] = prop->trans;
      /* si le materiau reflechi la scene environnante,
      alors il faudra faire une seconde passe */
      if (prop->env_reflexion != 0.0) SecondPassNeeded = 1;
    }

    /* Composant speculaire */
    if( rm & SPECULAR_MASK )
    {
      if( physic )
      {r=prop->speccol.rgb[0];g=prop->speccol.rgb[1];b=prop->speccol.rgb[2];}
      else
      {r=1.0;g=1.0;b=1.0;} /* cas des generiques */

      mSpec[0] = prop->spec * r;
      mSpec[1] = prop->spec * g;
      mSpec[2] = prop->spec * b;
      mSpec[3] = 1.0F;
    } 
    else {
      mSpec[0] = 0.0F;
      mSpec[1] = 0.0F;
      mSpec[2] = 0.0F;
      mSpec[3] = 1.0F;
    }


    /* Composant emission */        
    if( rm & EMISSIVE_MASK )
    {         
      if( physic )
      {r=prop->emscol.rgb[0];g=prop->emscol.rgb[1];b=prop->emscol.rgb[2];}
      else
      {r=col->rgb[0];g=col->rgb[1];b=col->rgb[2];}

      mEmsv[0] = prop->emsv * r;
      mEmsv[1] = prop->emsv * g;
      mEmsv[2] = prop->emsv * b;
      mEmsv[3] = 1.0F;
    }
    else {
      mEmsv[0] = 0.0F;
      mEmsv[1] = 0.0F;
      mEmsv[2] = 0.0F;
      mEmsv[3] = 1.0F;
    }

    /* Coeficient de brillance */
    mShin = prop -> shine;

    glMaterialfv(face, GL_AMBIENT, mAmb );
    glMaterialfv(face, GL_DIFFUSE, mDiff );
    glMaterialfv(face, GL_SPECULAR, mSpec);
    glMaterialfv(face, GL_EMISSION,  mEmsv);
    glMaterialf(face, GL_SHININESS, mShin);

#ifdef TRACE_MATERIAL
    printf(" Material Properties are need_reset:\n") ;
    printf("    Ambient color   is (%f, %f, %f)\n", mAmb[0], mAmb[1], mAmb[2]);
    printf("    Diffuse color   is (%f, %f, %f, %f)\n", mDiff[0], mDiff[1], mDiff[2], mDiff[3]);
    printf("    Specular color  is (%f, %f, %f)\n", mSpec[0], mSpec[1], mSpec[2]);
    printf("    Emission color  is (%f, %f, %f)\n", mEmsv[0], mEmsv[1], mEmsv[2]);
    printf("    Shininess: %f Transparency: %f EnvReflexion: %f \n", mShin, prop->trans, prop->env_reflexion);
    printf("    Prop Specular %f Prop Emissive %f \n", prop->spec, prop->emsv);
    printf("    materiel de type physic : %d\n",  prop->isphysic);
#endif

    need_reset = 0; 
  } 

  /* 
  * Set Material Optimize
  */

  else 
  {
    physic = prop->isphysic;
    /* Composant ambient */
    if( rm & AMBIENT_MASK )
    {
      /* Type materiau */
      if( physic )
      {r=prop->ambcol.rgb[0];g=prop->ambcol.rgb[1];b=prop->ambcol.rgb[2];}
      else
      {r=col->rgb[0];g=col->rgb[1];b=col->rgb[2];}

      if (mAmb[0] != prop->amb * r ||
        mAmb[1] != prop->amb * g ||
        mAmb[2] != prop->amb * b )
      {
        mAmb[0] = prop->amb * r;
        mAmb[1] = prop->amb * g;
        mAmb[2] = prop->amb * b;
        mAmb[3] = 1.0F;

        glMaterialfv(face, GL_AMBIENT, mAmb );
#ifdef TRACE_MATERIAL
        printf("    Ambient color   is (%f, %f, %f)\n", mAmb[0], mAmb[1], mAmb[2]);
#endif
      }
    }
    else
    {
      if (  mAmb[0] != 0.0 || mAmb[1] != 0.0 || mAmb[2] != 0.0 )
      {
        mAmb[0] = 0.0F;
        mAmb[1] = 0.0F;
        mAmb[2] = 0.0F;
        mAmb[3] = 1.0F;

        glMaterialfv(face, GL_AMBIENT, mAmb);
#ifdef TRACE_MATERIAL
        printf("    Ambient color  is (%f, %f, %f)\n", mAmb[0], mAmb[1], mAmb[2]);
#endif
      }
    }

    /* Composant diffus */
    if( rm & DIFFUSE_MASK )
    {
      /* Type materiau */
      if( physic )
      {r=prop->difcol.rgb[0];g=prop->difcol.rgb[1];b=prop->difcol.rgb[2];}
      else
      {r=col->rgb[0];g=col->rgb[1];b=col->rgb[2];}

      if (mDiff[0] != prop->diff * r ||
          mDiff[1] != prop->diff * g ||
          mDiff[2] != prop->diff * b ||
#ifdef BUC60577
          mDiff[3] != (SecondPassDo ? prop->env_reflexion :
      (need_trans ? prop->trans : 1.0F)) )
#else
        mDiff[3] != (SecondPassDo ? prop->env_reflexion : 1.0F) )
#endif
      {
        mDiff[0] = prop->diff * r;
        mDiff[1] = prop->diff * g;
        mDiff[2] = prop->diff * b;
        mDiff[3] = 1.0F;

        if (SecondPassDo)
        {
          mDiff[3] = prop->env_reflexion; 
        }
        else
        {
          if (need_trans) mDiff[3] = prop->trans;
          /* si le materiau reflechi la scene environnante,
          alors il faudra faire une seconde passe */
          if (prop->env_reflexion != 0.0) SecondPassNeeded = 1;
        }

        glMaterialfv(face, GL_DIFFUSE, mDiff );
#ifdef TRACE_MATERIAL
        printf("    Diffuse color   is (%f, %f, %f, %f)\n", mDiff[0], mDiff[1], mDiff[2], mDiff[3]);
#endif
      }
    }
    else
    {
      newDiff3 = 1.0F;
      if (SecondPassDo)
      {
        newDiff3 = prop->env_reflexion; 
      }
      else
      {
        if (need_trans) newDiff3 = prop->trans;
        /* si le materiau reflechi la scene environnante,
        alors il faudra faire une seconde passe */
        if (prop->env_reflexion != 0.0) SecondPassNeeded = 1;
      }

      /* OCC19915: Even if diffuse reflectance is disabled,
      still trying to update the current transparency if it
      differs from the previous value  */
      if (  mDiff[0] != 0.0 || mDiff[1] != 0.0 || mDiff[2] != 0.0 
        || fabs(mDiff[3] - newDiff3) > 0.01 )
      {
        mDiff[0] = 0.0F;
        mDiff[1] = 0.0F;
        mDiff[2] = 0.0F;
        mDiff[3] = newDiff3;

        glMaterialfv(face, GL_DIFFUSE, mDiff);
#ifdef TRACE_MATERIAL
        printf("    Diffuse color  is (%f, %f, %f, %f)\n", mDiff[0], mDiff[1], mDiff[2], mDiff[3]);
#endif
      }
    }

    /* composant speculaire */
    if( rm & SPECULAR_MASK )
    {   
      /* Type materiau */
      if( physic )
      {r=prop->speccol.rgb[0];g=prop->speccol.rgb[1];b=prop->speccol.rgb[2];}
      else
      {r=1.0;g=1.0;b=1.0;} /* cas des generiques */          

      if (mSpec[0] != prop->spec * r ||
        mSpec[1] != prop->spec * g ||
        mSpec[2] != prop->spec * b)
      {
        mSpec[0] = prop->spec * r;
        mSpec[1] = prop->spec * g;
        mSpec[2] = prop->spec * b;
        mSpec[3] = 1.0F;
        glMaterialfv(face, GL_SPECULAR, mSpec);

#ifdef TRACE_MATERIAL
        printf("    Specular color  is (%f, %f, %f)\n", mSpec[0], mSpec[1], mSpec[2]);
#endif
      }
    }
    else
    {
      if (  mSpec[0] != 0.0 || mSpec[1] != 0.0 || mSpec[2] != 0.0 )
      {
        mSpec[0] = 0.0F;
        mSpec[1] = 0.0F;
        mSpec[2] = 0.0F;
        mSpec[3] = 1.0F;

        glMaterialfv(face, GL_SPECULAR, mSpec);
#ifdef TRACE_MATERIAL
        printf("    Specular color  is (%f, %f, %f)\n", mSpec[0], mSpec[1], mSpec[2]);
#endif
      }
    }

    /* Composant emission */        
    if( rm & EMISSIVE_MASK )
    {
      /* type materiau */
      if( physic )
      {r=prop->emscol.rgb[0];g=prop->emscol.rgb[1];b=prop->emscol.rgb[2];}
      else
      {r=col->rgb[0];g=col->rgb[1];b=col->rgb[2];}

      if ( mEmsv[0] != prop->emsv * r ||
        mEmsv[1] != prop->emsv * g ||
        mEmsv[2] != prop->emsv * b )
      {
        mEmsv[0] = prop->emsv * r;
        mEmsv[1] = prop->emsv * g;
        mEmsv[2] = prop->emsv * b;
        mEmsv[3] = 1.0F;
        glMaterialfv(face, GL_EMISSION, mEmsv);
#ifdef TRACE_MATERIAL
        printf("    Emissive color  is (%f, %f, %f)\n", mEmsv[0], mEmsv[1], mEmsv[2]);
#endif
      }
    }
    else 
    { 
      if (  mEmsv[0] != 0.0 || mEmsv[1] != 0.0 || mEmsv[2] != 0.0 )
      {
        mEmsv[0] = 0.0F;
        mEmsv[1] = 0.0F;
        mEmsv[2] = 0.0F;
        mEmsv[3] = 1.0F;

        glMaterialfv(face, GL_EMISSION, mEmsv);
#ifdef TRACE_MATERIAL
        printf("    Emissive surface  is (%f, %f, %f)\n", mEmsv[0], mEmsv[1], mEmsv[2]);
#endif
      }
    }           

    /* coef de brillance */ 
    if( mShin != prop->shine )
    {
      mShin = prop->shine;
      glMaterialf(face, GL_SHININESS, mShin);
#ifdef TRACE_MATERIAL
      printf("    Shininess factor is %f\n", mShin);
#endif
    }
  } /* need_reset */
}

/*----------------------------------------------------------------------*/
void TelResetMaterial()  /* force material definition, called by TelClearViews */
{
  need_reset = 1;
}

/*----------------------------------------------------------------------*/
void TelSetTransparency( Tint tag )
/* force transparency managment, called by redraw_all_structs */
{
  need_trans = tag;
}

/*----------------------------------------------------------------------*/

static  void
TelUpdateFog( Tint ws, Tint vid, Tint dcid )
{
  TEL_GL_DEPTHCUE  dcrep;
  float  params[5];

  if( TelGetGLDepthCue( ws, vid, dcid, &dcrep ) == TSuccess )
  {
    if( dcrep.dcrep.mode == TelDCAllowed )
    {
      params[0] = dcrep.dcrep.col.rgb[0],
      params[1] = dcrep.dcrep.col.rgb[1],
      params[2] = dcrep.dcrep.col.rgb[2];
      params[3] = 1.0F;
      glFogi(GL_FOG_MODE, GL_LINEAR);
      glFogf(GL_FOG_START, dcrep.dist[0]);
      glFogf(GL_FOG_END, dcrep.dist[1]);
      glFogfv(GL_FOG_COLOR, params);
      glEnable(GL_FOG);
#ifdef TRACE
      printf(" *** TelUpdateFog START %f END %f\n",dcrep.dist[0],dcrep.dist[1]);
#endif
      return;
    }
  }
  glDisable(GL_FOG);
  return;
}

/*----------------------------------------------------------------------*/

static  void TelSetViewMatrix( Tmatrix3 mat, Tint vid )
{
  TEL_VIEW_REP  vrep;
  Tmatrix3      rmat;
  float         mat16[16];
#ifdef TRACE_MATRIX
  int           i, j;
  GLfloat       pmat[4][4];
#endif

  if( TelGetViewRepresentation( TglActiveWs, vid, &vrep ) == TSuccess )
  {
#ifdef TRACE_MATRIX
    printf ("LocalTran3 :\n");
    for (i = 0; i < 4; i++) {
      printf ("\t");
      for (j = 0; j < 4; j++)
        printf ("%f ", mat[i][j]);
      printf ("\n");
    }
#endif
    if ((!animationUseFlag) || (animationUseFlag && !animationFlag)) 
    {
#ifdef TRACE_MATRIX
      printf ("vrep.orientation_matrix :\n");
      for (i = 0; i < 4; i++) {
        printf ("\t");
        for (j = 0; j < 4; j++)
          printf ("%f ", vrep.orientation_matrix[i][j]);
        printf ("\n");
      }
#endif
      TelMultiplymat3( rmat, mat, vrep.orientation_matrix );
#ifdef TRACE_MATRIX
      printf("OpenGl_attri.c::TelSetViewMatrix::glLoadMatrixf \n"); 
#endif
      glMatrixMode(GL_MODELVIEW);
      glLoadMatrixf((GLfloat *) rmat);
#ifdef TRACE_MATRIX
      printf ("GL_MODELVIEW_MATRIX set :\n");
      for (i = 0; i < 4; i++) {
        printf ("\t");
        for (j = 0; j < 4; j++)
          printf ("%f ", rmat[i][j]);
        printf ("\n");
      }
#endif
    }
    else /* animationUseFlag */
    {
      call_util_transpose_mat (mat16, mat); 
      glMatrixMode (GL_MODELVIEW);
#ifdef TRACE_MATRIX
      glGetFloatv( GL_MODELVIEW_MATRIX,(GLfloat *) pmat );
      printf ("GL_MODELVIEW_MATRIX :\n");
      for (i = 0; i < 4; i++) {
        printf ("\t");
        for (j = 0; j < 4; j++)
          printf ("%f ", pmat[i][j]);
        printf ("\n");
      }
#endif
#ifdef TRACE_MATRIX
      printf("OpenGl_attri.c::TelSetViewMatrix::glMultMatrixf \n"); 
#endif
      glMultMatrixf ((GLfloat *)mat);
#ifdef TRACE_MATRIX
      glGetFloatv( GL_MODELVIEW_MATRIX,(GLfloat *) pmat );
      printf ("GL_MODELVIEW_MATRIX set :\n");
      for (i = 0; i < 4; i++) {
        printf ("\t");
        for (j = 0; j < 4; j++)
          printf ("%f ", pmat[i][j]);
        printf ("\n");
      }
#endif
    }
  }
  return;
}

/*----------------------------------------------------------------------*/

TStatus
TsmPushAttriLight( void )
{
  tsm_attri_blk  node;

  node = new TSM_ATTRI_BLK(); 

  if( !attri_stk.IsEmpty() )
    *node = *attri_stk.Top();
  else
    *node = attri_default;

  attri_stk.Push( node );
  return TSuccess;
}

/*----------------------------------------------------------------------*/

TStatus
TsmPopAttriLight( void )
{
  tsm_attri_blk  node;

  if ( attri_stk.IsEmpty() )
    return TFailure; /* Underflow */

  node = attri_stk.ChangeTop();
  attri_stk.Pop();
  delete node;

  return TSuccess;
}

/*----------------------------------------------------------------------*/

static void call_util_transpose_mat (float tmat[16], float mat[4][4]) {
  int i, j;

  for (i=0; i<4; i++)
    for (j=0; j<4; j++)
      tmat[j*4+i] = mat[i][j];

#ifdef TRACE
  printf ("Transposee :\n");
  for (i = 0; i < 4; i++) {
    printf ("\t");
    for (j = 0; j < 4; j++)
      printf ("%f ", tmat[i*4+j]);
    printf ("\n");
  }
#endif
}
/*----------------------------------------------------------------------*/

/* OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets */
static void TelUpdatePolygonOffsets( tel_poffset_param pdata )
{
#ifdef TRACE
  printf(" *** TelUpdatePolygonOffsets: mode = %x, factor = %f, units = %f", 
    pdata->mode, pdata->factor, pdata->units );
#endif
  if ( ( pdata->mode & Aspect_POM_Fill ) == Aspect_POM_Fill )
    glEnable ( GL_POLYGON_OFFSET_FILL );
  else 
    glDisable ( GL_POLYGON_OFFSET_FILL );

  if ( ( pdata->mode & Aspect_POM_Line ) == Aspect_POM_Line )
    glEnable ( GL_POLYGON_OFFSET_LINE );
  else
    glDisable( GL_POLYGON_OFFSET_LINE );

  if ( ( pdata->mode & Aspect_POM_Point ) == Aspect_POM_Point )
    glEnable ( GL_POLYGON_OFFSET_POINT );
  else
    glDisable( GL_POLYGON_OFFSET_POINT );

  glPolygonOffset( pdata->factor, pdata->units );
}
/* OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets */
