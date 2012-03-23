// Created on: 2011-08-05
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

#include <OpenGl_Workspace.hxx>

#include <OpenGl_AspectLine.hxx>
#include <OpenGl_AspectFace.hxx>
#include <OpenGl_AspectMarker.hxx>
#include <OpenGl_AspectText.hxx>

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

/* OCC22218 NOTE: project dependency on gl2ps is specified by macro */
#ifdef HAVE_GL2PS
  #include <gl2ps.h>
  /* OCC22216 NOTE: linker dependency can be switched off by undefining macro. 
     Pragma comment for gl2ps.lib is defined only here. */ 
  #ifdef _MSC_VER 
  #pragma comment( lib, "gl2ps.lib" )
  #endif
#endif

#include <OpenGl_TextureBox.hxx>
#include <Aspect_PolygonOffsetMode.hxx>
#include <OpenGl_View.hxx>

/*----------------------------------------------------------------------*/

static void TelUpdatePolygonOffsets( const TEL_POFFSET_PARAM *pdata )
{
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

/*----------------------------------------------------------------------*/

void OpenGl_Workspace::UpdateMaterial( const int flag )
{
  // Case of Hiddenline
  if (AspectFace_set->Context().InteriorStyle == Aspect_IS_HIDDENLINE)
  {
    /* szvgl - IMPORTANT!!! */
    static TEL_CONTEXT_FACE hl_context_face;
    static OpenGl_AspectFace hl_aspect_face;

	hl_context_face = AspectFace_set->Context();

    hl_context_face.IntFront.matcol = BackgroundColor();
    hl_context_face.IntFront.color_mask = 0;
    hl_context_face.IntBack.color_mask = 0;

    hl_aspect_face.SetContext(hl_context_face);
    hl_aspect_face.SetAspectEdge(AspectFace_set->AspectEdge());

	AspectFace_set = &hl_aspect_face;
    return;
  }

  const OPENGL_SURF_PROP *prop = NULL;
  GLenum face = 0;
  if ( flag == TEL_FRONT_MATERIAL )
  {
    prop = &AspectFace_set->Context().IntFront;
    face = GL_FRONT_AND_BACK;
  }
  else
  {
    prop = &AspectFace_set->Context().IntBack;
    face = GL_BACK;
  }

  const unsigned int rm = prop->color_mask;

  if ( !rm ) return;

  // Handling transparency
  if ( (NamedStatus & OPENGL_NS_2NDPASSDO) == 0 )
  {
    if ( myUseTransparency && prop->trans != 1.0F )
    {
      // Render transparent
      glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
      glEnable (GL_BLEND);
      glDepthMask (GL_FALSE);
    }
    else 
    {
      // Render opaque
      if ( (NamedStatus & OPENGL_NS_ANTIALIASING) == 0 )
      {
        glBlendFunc (GL_ONE, GL_ZERO);
        glDisable (GL_BLEND);
      }
      glDepthMask (GL_TRUE);
    }       
  }

  static float  mAmb[4];
  static float  mDiff[4];
  static float  mSpec[4];
  static float  mEmsv[4];
  static float  mShin;

  static const float defspeccol[4] = { 1.F, 1.F, 1.F, 1.F };

  // Reset material
  if ( NamedStatus & OPENGL_NS_RESMAT )
  {   
    // Ambient component
    if( rm & OPENGL_AMBIENT_MASK )
    {
      const float *c = prop->isphysic? prop->ambcol.rgb : prop->matcol.rgb;

      mAmb[0] = prop->amb * c[0];
      mAmb[1] = prop->amb * c[1];
      mAmb[2] = prop->amb * c[2];
    }
    else
    {
      mAmb[0] = 0.F;
      mAmb[1] = 0.F;
      mAmb[2] = 0.F;
    }
    mAmb[3] = 1.F;

    // Diffusion component
    if( rm & OPENGL_DIFFUSE_MASK )
    {
      const float *c = prop->isphysic? prop->difcol.rgb : prop->matcol.rgb;

      mDiff[0] = prop->diff * c[0];
      mDiff[1] = prop->diff * c[1];
      mDiff[2] = prop->diff * c[2];
    }
    else
    {
      mDiff[0] = 0.F;
      mDiff[1] = 0.F;
      mDiff[2] = 0.F;
    }
    mDiff[3] = 1.F;

    if (NamedStatus & OPENGL_NS_2NDPASSDO)
    {
      mDiff[3] = prop->env_reflexion; 
    }
    else
    {
      if (myUseTransparency) mDiff[3] = prop->trans;
      // If the material reflects the environment scene, the second pass is needed
      if (prop->env_reflexion != 0.0) NamedStatus |= OPENGL_NS_2NDPASSNEED;
    }

    // Specular component
    if( rm & OPENGL_SPECULAR_MASK )
    {
      const float *c = prop->isphysic? prop->speccol.rgb : defspeccol;

      mSpec[0] = prop->spec * c[0];
      mSpec[1] = prop->spec * c[1];
      mSpec[2] = prop->spec * c[2];
    } 
    else {
      mSpec[0] = 0.F;
      mSpec[1] = 0.F;
      mSpec[2] = 0.F;
    }
    mSpec[3] = 1.F;

    // Emissive component
    if( rm & OPENGL_EMISSIVE_MASK )
    {         
      const float *c = prop->isphysic? prop->emscol.rgb : prop->matcol.rgb;

      mEmsv[0] = prop->emsv * c[0];
      mEmsv[1] = prop->emsv * c[1];
      mEmsv[2] = prop->emsv * c[2];
    }
    else {
      mEmsv[0] = 0.F;
      mEmsv[1] = 0.F;
      mEmsv[2] = 0.F;
    }
    mEmsv[3] = 1.F;

    /* Coeficient de brillance */
    mShin = prop->shine;

    glMaterialfv(face, GL_AMBIENT, mAmb );
    glMaterialfv(face, GL_DIFFUSE, mDiff );
    glMaterialfv(face, GL_SPECULAR, mSpec);
    glMaterialfv(face, GL_EMISSION, mEmsv);
    glMaterialf(face, GL_SHININESS, mShin);

    NamedStatus &= ~OPENGL_NS_RESMAT;
  } 

  // Set Material Optimize
  else 
  {
    // Ambient component
    if( rm & OPENGL_AMBIENT_MASK )
    {
      const float *c = prop->isphysic? prop->ambcol.rgb : prop->matcol.rgb;

      if (mAmb[0] != prop->amb * c[0] ||
          mAmb[1] != prop->amb * c[1] ||
          mAmb[2] != prop->amb * c[2] )
      {
        mAmb[0] = prop->amb * c[0];
        mAmb[1] = prop->amb * c[1];
        mAmb[2] = prop->amb * c[2];
        mAmb[3] = 1.F;

        glMaterialfv(face, GL_AMBIENT, mAmb);
      }
    }
    else
    {
      if ( mAmb[0] != 0.F || mAmb[1] != 0.F || mAmb[2] != 0.F )
      {
        mAmb[0] = 0.F;
        mAmb[1] = 0.F;
        mAmb[2] = 0.F;
        mAmb[3] = 1.F;

        glMaterialfv(face, GL_AMBIENT, mAmb);
      }
    }

    // Diffusion component
    if( rm & OPENGL_DIFFUSE_MASK )
    {
      const float *c = prop->isphysic? prop->difcol.rgb : prop->matcol.rgb;

      if (mDiff[0] != prop->diff * c[0] ||
          mDiff[1] != prop->diff * c[1] ||
          mDiff[2] != prop->diff * c[2] ||
          mDiff[3] != ((NamedStatus & OPENGL_NS_2NDPASSDO)? prop->env_reflexion : (myUseTransparency? prop->trans : 1.0F)))
      {
        mDiff[0] = prop->diff * c[0];
        mDiff[1] = prop->diff * c[1];
        mDiff[2] = prop->diff * c[2];
        mDiff[3] = 1.F;

        if (NamedStatus & OPENGL_NS_2NDPASSDO)
        {
          mDiff[3] = prop->env_reflexion; 
        }
        else
        {
          if (myUseTransparency) mDiff[3] = prop->trans;
          // If the material reflects the environment scene, the second pass is needed
          if (prop->env_reflexion != 0.0) NamedStatus |= OPENGL_NS_2NDPASSNEED;
        }

        glMaterialfv(face, GL_DIFFUSE, mDiff );
      }
    }
    else
    {
      Tfloat newDiff3 = 1.F;

      if (NamedStatus & OPENGL_NS_2NDPASSDO)
      {
        newDiff3 = prop->env_reflexion; 
      }
      else
      {
        if (myUseTransparency) newDiff3 = prop->trans;
        // If the material reflects the environment scene, the second pass is needed
        if (prop->env_reflexion != 0.0) NamedStatus |= OPENGL_NS_2NDPASSNEED;
      }

      /* OCC19915: Even if diffuse reflectance is disabled,
      still trying to update the current transparency if it
      differs from the previous value  */
      if ( mDiff[0] != 0.F || mDiff[1] != 0.F || mDiff[2] != 0.F || fabs(mDiff[3] - newDiff3) > 0.01F )
      {
        mDiff[0] = 0.F;
        mDiff[1] = 0.F;
        mDiff[2] = 0.F;
        mDiff[3] = newDiff3;

        glMaterialfv(face, GL_DIFFUSE, mDiff);
      }
    }

    // Specular component
    if( rm & OPENGL_SPECULAR_MASK )
    {   
      const float *c = prop->isphysic? prop->speccol.rgb : defspeccol;

      if (mSpec[0] != prop->spec * c[0] ||
          mSpec[1] != prop->spec * c[1] ||
          mSpec[2] != prop->spec * c[2])
      {
        mSpec[0] = prop->spec * c[0];
        mSpec[1] = prop->spec * c[1];
        mSpec[2] = prop->spec * c[2];
        mSpec[3] = 1.F;

        glMaterialfv(face, GL_SPECULAR, mSpec);
      }
    }
    else
    {
      if ( mSpec[0] != 0.F || mSpec[1] != 0.F || mSpec[2] != 0.F )
      {
        mSpec[0] = 0.F;
        mSpec[1] = 0.F;
        mSpec[2] = 0.F;
        mSpec[3] = 1.F;

        glMaterialfv(face, GL_SPECULAR, mSpec);
      }
    }

    // Emissive component
    if( rm & OPENGL_EMISSIVE_MASK )
    {
      const float *c = prop->isphysic? prop->emscol.rgb : prop->matcol.rgb;

      if (mEmsv[0] != prop->emsv * c[0] ||
          mEmsv[1] != prop->emsv * c[1] ||
          mEmsv[2] != prop->emsv * c[2])
      {
        mEmsv[0] = prop->emsv * c[0];
        mEmsv[1] = prop->emsv * c[1];
        mEmsv[2] = prop->emsv * c[2];
        mEmsv[3] = 1.F;

        glMaterialfv(face, GL_EMISSION, mEmsv);
      }
    }
    else 
    { 
      if ( mEmsv[0] != 0.F || mEmsv[1] != 0.F || mEmsv[2] != 0.F )
      {
        mEmsv[0] = 0.F;
        mEmsv[1] = 0.F;
        mEmsv[2] = 0.F;
        mEmsv[3] = 1.F;

        glMaterialfv(face, GL_EMISSION, mEmsv);
      }
    }

    // Shining coefficient
    if( mShin != prop->shine )
    {
      mShin = prop->shine;
      glMaterialf(face, GL_SHININESS, mShin);
    }
  }
}

/*----------------------------------------------------------------------*/

const OpenGl_AspectLine * OpenGl_Workspace::SetAspectLine(const OpenGl_AspectLine *AnAspect)
{
  const OpenGl_AspectLine *AspectLine_old = AspectLine_set;
  AspectLine_set = AnAspect;
  return AspectLine_old;
}

/*----------------------------------------------------------------------*/

const OpenGl_AspectFace * OpenGl_Workspace::SetAspectFace(const OpenGl_AspectFace *AnAspect)
{
  const OpenGl_AspectFace *AspectFace_old = AspectFace_set;
  AspectFace_set = AnAspect;
  return AspectFace_old;
}

/*----------------------------------------------------------------------*/

const OpenGl_AspectMarker * OpenGl_Workspace::SetAspectMarker(const OpenGl_AspectMarker *AnAspect)
{
  const OpenGl_AspectMarker *AspectMarker_old = AspectMarker_set;
  AspectMarker_set = AnAspect;
  return AspectMarker_old;
}

/*----------------------------------------------------------------------*/

const OpenGl_AspectText * OpenGl_Workspace::SetAspectText(const OpenGl_AspectText *AnAspect)
{
  const OpenGl_AspectText *AspectText_old = AspectText_set;
  AspectText_set = AnAspect;
  return AspectText_old;
}

/*----------------------------------------------------------------------*/

const OpenGl_Matrix * OpenGl_Workspace::SetViewMatrix(const OpenGl_Matrix *AMatrix)
{
  const OpenGl_Matrix *ViewMatrix_old = ViewMatrix_applied;
  ViewMatrix_applied = AMatrix;

  OpenGl_Matrix lmat;
  OpenGl_Transposemat3( &lmat, StructureMatrix_applied );

  glMatrixMode (GL_MODELVIEW);

  if ( (NamedStatus & OPENGL_NS_ANIMATION) == 0 )
  {
    OpenGl_Matrix rmat;
    OpenGl_Multiplymat3( &rmat, &lmat, ViewMatrix_applied );
    glLoadMatrixf((const GLfloat *) rmat.mat);
  }

  return ViewMatrix_old;
}

/*----------------------------------------------------------------------*/

const OpenGl_Matrix * OpenGl_Workspace::SetStructureMatrix(const OpenGl_Matrix *AMatrix)
{
  const OpenGl_Matrix *StructureMatrix_old = StructureMatrix_applied;
  StructureMatrix_applied = AMatrix;

  OpenGl_Matrix lmat;
  OpenGl_Transposemat3( &lmat, AMatrix );

  glMatrixMode (GL_MODELVIEW);

  if ( (NamedStatus & OPENGL_NS_ANIMATION) == 0 )
  {
    OpenGl_Matrix rmat;
    OpenGl_Multiplymat3( &rmat, &lmat, ViewMatrix_applied );
    glLoadMatrixf((const GLfloat *) rmat.mat);
  }
  else
  {
    glMultMatrixf((const GLfloat *) lmat.mat);
  }

  return StructureMatrix_old;
}

/*----------------------------------------------------------------------*/

const OpenGl_AspectLine * OpenGl_Workspace::AspectLine(const Standard_Boolean WithApply)
{
  if ( WithApply && (AspectLine_set != AspectLine_applied) )
  {
    glColor3fv(AspectLine_set->Color().rgb);

    if ( !AspectLine_applied || (AspectLine_set->Type() != AspectLine_applied->Type() ) )
    {
      myDisplay->SetTypeOfLine(AspectLine_set->Type());
    }

    if ( !AspectLine_applied || ( AspectLine_set->Width() != AspectLine_applied->Width() ) )
    {
      glLineWidth( (GLfloat)AspectLine_set->Width() );
#ifdef HAVE_GL2PS
      gl2psLineWidth( (GLfloat)AspectLine_set->Width() );
#endif
    }

    AspectLine_applied = AspectLine_set;
  }
  return AspectLine_set;
}

/*----------------------------------------------------------------------*/

const OpenGl_AspectFace * OpenGl_Workspace::AspectFace(const Standard_Boolean WithApply)
{
  if ( WithApply && (AspectFace_set != AspectFace_applied) )
  {
    const Aspect_InteriorStyle intstyle = AspectFace_set->Context().InteriorStyle;
    if ( !AspectFace_applied || AspectFace_applied->Context().InteriorStyle != intstyle )
    {
      switch( intstyle )
      {
        case Aspect_IS_EMPTY:
        case Aspect_IS_HOLLOW:
          glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
          break;

        case Aspect_IS_HATCH:
          glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		  myDisplay->SetTypeOfHatch(AspectFace_applied? AspectFace_applied->Context().Hatch : TEL_HS_SOLID);
          break;

        case Aspect_IS_SOLID:
        case Aspect_IS_HIDDENLINE:
          glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
          glDisable(GL_POLYGON_STIPPLE);
          break;

        case 5: //szvgl - no corresponding enumeration item Aspect_IS_POINT
          glPolygonMode(GL_FRONT_AND_BACK,GL_POINT);
          break;
      }
    }
    if( intstyle == Aspect_IS_HATCH )
    {
      const Tint hatchstyle = AspectFace_set->Context().Hatch;
      if( !AspectFace_applied || AspectFace_applied->Context().Hatch != hatchstyle )
      {
        myDisplay->SetTypeOfHatch(hatchstyle);
      }
    }
    if ( !ActiveView()->Backfacing() )
    {
      const Tint mode = AspectFace_set->Context().CullingMode;
      if( !AspectFace_applied || AspectFace_applied->Context().CullingMode != mode )
      {
        switch( (TelCullMode)mode )
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

    // Aspect_POM_None means: do not change current settings
    if ( ( AspectFace_set->Context().PolygonOffset.mode & Aspect_POM_None ) != Aspect_POM_None )
    {
      if ( !PolygonOffset_applied ||
           PolygonOffset_applied->mode   != AspectFace_set->Context().PolygonOffset.mode ||
           PolygonOffset_applied->factor != AspectFace_set->Context().PolygonOffset.factor ||
           PolygonOffset_applied->units  != AspectFace_set->Context().PolygonOffset.units )
      {
        PolygonOffset_applied = &AspectFace_set->Context().PolygonOffset;
        TelUpdatePolygonOffsets( PolygonOffset_applied );
      }
    }

    UpdateMaterial( TEL_FRONT_MATERIAL );
    if (AspectFace_set->Context().DistinguishingMode == TOn)
      UpdateMaterial( TEL_BACK_MATERIAL );

    if ((NamedStatus & OPENGL_NS_FORBIDSETTEX) == 0)
    {
      DisableTexture();
      if (AspectFace_set->Context().doTextureMap)
      {
        SetCurrentTexture(AspectFace_set->Context().TexId);
        EnableTexture();
      }
    }

    AspectFace_applied = AspectFace_set;
  }
  return AspectFace_set;
}

/*----------------------------------------------------------------------*/

const OpenGl_AspectMarker * OpenGl_Workspace::AspectMarker(const Standard_Boolean WithApply)
{
  if ( WithApply && (AspectMarker_set != AspectMarker_applied) )
  {
    AspectMarker_applied = AspectMarker_set;
  }
  return AspectMarker_set;
}

/*----------------------------------------------------------------------*/

const OpenGl_AspectText * OpenGl_Workspace::AspectText(const Standard_Boolean WithApply)
{
  if ( WithApply )
  {
    Standard_Boolean toApply = Standard_False;
    if ( AspectText_set != AspectText_applied )
    {
      if ( !AspectText_applied )
        toApply = Standard_True;
      else if ( strcmp( AspectText_set->Font(), AspectText_applied->Font() ) ||
                ( AspectText_set->FontAspect() != AspectText_applied->FontAspect() ) )
        toApply = Standard_True;

      AspectText_applied = AspectText_set;
    }
    if ( TextParam_set != TextParam_applied )
    {
      if ( !TextParam_applied )
        toApply = Standard_True;
      else if ( TextParam_set->Height != TextParam_applied->Height )
        toApply = Standard_True;

      TextParam_applied = TextParam_set;
    }
    if ( toApply )
    {
      FindFont(AspectText_applied->Font(), AspectText_applied->FontAspect(), TextParam_applied->Height);
    }
  }
  return AspectText_set;
}

/*----------------------------------------------------------------------*/

//=======================================================================
//function : ResetAppliedAspect
//purpose  : 
//=======================================================================

void OpenGl_Workspace::ResetAppliedAspect()
{
  AspectLine_applied   = NULL;
  AspectFace_applied   = NULL;
  AspectMarker_applied = NULL;
  AspectText_applied   = NULL;
  TextParam_applied    = NULL;
}
