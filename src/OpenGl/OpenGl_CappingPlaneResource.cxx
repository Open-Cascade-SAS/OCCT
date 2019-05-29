// Created on: 2013-08-15
// Created by: Anton POLETAEV
// Copyright (c) 2013-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <NCollection_AlignedAllocator.hxx>
#include <OpenGl_CappingPlaneResource.hxx>
#include <OpenGl_Context.hxx>
#include <OpenGl_Vec.hxx>
#include <OpenGl_ShaderManager.hxx>
#include <Precision.hxx>

IMPLEMENT_STANDARD_RTTIEXT(OpenGl_CappingPlaneResource,OpenGl_Resource)

namespace
{
  //! 12 plane vertices, interleaved:
  //!  - 4 floats, position
  //!  - 4 floats, normal
  //!  - 4 floats, UV texture coordinates
  static const GLfloat THE_CAPPING_PLN_VERTS[12 * (4 + 4 + 4)] =
  {
    0.0f, 0.0f, 0.0f, 1.0f,  0.0f,-1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 0.0f,  0.0f,-1.0f, 0.0f, 0.0f,   0.0f,-1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f, 0.0f,  0.0f,-1.0f, 0.0f, 0.0f,   1.0f, 0.0f, 0.0f, 0.0f,

    0.0f, 0.0f, 0.0f, 1.0f,  0.0f,-1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f, 1.0f,
   -1.0f, 0.0f, 0.0f, 0.0f,  0.0f,-1.0f, 0.0f, 0.0f,  -1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,  0.0f,-1.0f, 0.0f, 0.0f,   0.0f,-1.0f, 0.0f, 0.0f,

    0.0f, 0.0f, 0.0f, 1.0f,  0.0f,-1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f, 1.0f,
    0.0f, 0.0f,-1.0f, 0.0f,  0.0f,-1.0f, 0.0f, 0.0f,   0.0f, 1.0f, 0.0f, 0.0f,
   -1.0f, 0.0f, 0.0f, 0.0f,  0.0f,-1.0f, 0.0f, 0.0f,  -1.0f, 0.0f, 0.0f, 0.0f,

    0.0f, 0.0f, 0.0f, 1.0f,  0.0f,-1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 0.0f, 0.0f,  0.0f,-1.0f, 0.0f, 0.0f,   1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f,-1.0f, 0.0f,  0.0f,-1.0f, 0.0f, 0.0f,   0.0f, 1.0f, 0.0f, 0.0f
  };

  static const OpenGl_Matrix OpenGl_IdentityMatrix =
  {
    // mat[4][4]
    { { 1.0f, 0.0f, 0.0f, 0.0f },
      { 0.0f, 1.0f, 0.0f, 0.0f },
      { 0.0f, 0.0f, 1.0f, 0.0f },
      { 0.0f, 0.0f, 0.0f, 1.0f } }
  };

}


// =======================================================================
// function : OpenGl_CappingPlaneResource
// purpose  :
// =======================================================================
OpenGl_CappingPlaneResource::OpenGl_CappingPlaneResource (const Handle(Graphic3d_ClipPlane)& thePlane)
: myPrimitives  (NULL),
  myOrientation (OpenGl_IdentityMatrix),
  myAspect      (NULL),
  myPlaneRoot   (thePlane),
  myEquationMod ((unsigned int )-1),
  myAspectMod   ((unsigned int )-1)
{
  // Fill primitive array
  Handle(NCollection_AlignedAllocator) anAlloc = new NCollection_AlignedAllocator (16);
  Handle(Graphic3d_Buffer) anAttribs = new Graphic3d_Buffer (anAlloc);
  Graphic3d_Attribute anAttribInfo[] =
  {
    { Graphic3d_TOA_POS,  Graphic3d_TOD_VEC4 },
    { Graphic3d_TOA_NORM, Graphic3d_TOD_VEC4 },
    { Graphic3d_TOA_UV,   Graphic3d_TOD_VEC4 }
  };
  if (anAttribs->Init (12, anAttribInfo, 3))
  {
    memcpy (anAttribs->ChangeData(), THE_CAPPING_PLN_VERTS, sizeof(THE_CAPPING_PLN_VERTS));
    myPrimitives.InitBuffers (NULL, Graphic3d_TOPA_TRIANGLES, NULL, anAttribs, NULL);
  }
}

// =======================================================================
// function : OpenGl_CappingPlaneResource
// purpose  :
// =======================================================================
OpenGl_CappingPlaneResource::~OpenGl_CappingPlaneResource()
{
  Release (NULL);
}

// =======================================================================
// function : Update
// purpose  :
// =======================================================================
void OpenGl_CappingPlaneResource::Update (const Handle(OpenGl_Context)& theCtx,
                                          const Handle(Graphic3d_Aspects)& theObjAspect)
{
  updateTransform (theCtx);
  updateAspect (theObjAspect);
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void OpenGl_CappingPlaneResource::Release (OpenGl_Context* theContext)
{
  OpenGl_Element::Destroy (theContext, myAspect);
  myPrimitives.Release (theContext);
  myEquationMod = (unsigned int )-1;
  myAspectMod   = (unsigned int )-1;
}

// =======================================================================
// function : updateAspect
// purpose  :
// =======================================================================
void OpenGl_CappingPlaneResource::updateAspect (const Handle(Graphic3d_Aspects)& theObjAspect)
{
  if (myAspect == NULL)
  {
    myAspect = new OpenGl_Aspects();
    myAspectMod = myPlaneRoot->MCountAspect() - 1; // mark out of sync
  }

  if (theObjAspect.IsNull())
  {
    if (myAspectMod != myPlaneRoot->MCountAspect())
    {
      myAspect->SetAspect (myPlaneRoot->CappingAspect());
      myAspectMod = myPlaneRoot->MCountAspect();
    }
    return;
  }

  if (myFillAreaAspect.IsNull())
  {
    myFillAreaAspect = new Graphic3d_AspectFillArea3d();
  }
  if (myAspectMod != myPlaneRoot->MCountAspect())
  {
    *myFillAreaAspect = *myPlaneRoot->CappingAspect();
  }

  if (myPlaneRoot->ToUseObjectMaterial())
  {
    // only front material currently supported by capping rendering
    myFillAreaAspect->SetFrontMaterial (theObjAspect->FrontMaterial());
    myFillAreaAspect->SetInteriorColor (theObjAspect->InteriorColor());
  }
  if (myPlaneRoot->ToUseObjectTexture())
  {
    myFillAreaAspect->SetTextureSet (theObjAspect->TextureSet());
    if (theObjAspect->ToMapTexture())
    {
      myFillAreaAspect->SetTextureMapOn();
    }
    else
    {
      myFillAreaAspect->SetTextureMapOff();
    }
  }
  if (myPlaneRoot->ToUseObjectShader())
  {
    myFillAreaAspect->SetShaderProgram (theObjAspect->ShaderProgram());
  }

  myAspect->SetAspect (myFillAreaAspect);
}

// =======================================================================
// function : updateTransform
// purpose  :
// =======================================================================
void OpenGl_CappingPlaneResource::updateTransform (const Handle(OpenGl_Context)& theCtx)
{
  if (myEquationMod == myPlaneRoot->MCountEquation()
   && myLocalOrigin.IsEqual (theCtx->ShaderManager()->LocalOrigin(), gp::Resolution()))
  {
    return; // nothing to update
  }

  myEquationMod = myPlaneRoot->MCountEquation();
  myLocalOrigin = theCtx->ShaderManager()->LocalOrigin();

  const Graphic3d_ClipPlane::Equation& anEq = myPlaneRoot->GetEquation();
  const Standard_Real anEqW = theCtx->ShaderManager()->LocalClippingPlaneW (*myPlaneRoot);

  // re-evaluate infinite plane transformation matrix
  const Graphic3d_Vec3 aNorm (anEq.xyz());
  const Graphic3d_Vec3 T (anEq.xyz() * -anEqW);

  // project plane normal onto OX to find left vector
  const Standard_ShortReal aProjLen = sqrt ((Standard_ShortReal)anEq.xz().SquareModulus());
  Graphic3d_Vec3 aLeft;
  if (aProjLen < ShortRealSmall())
  {
    aLeft[0] = 1.0f;
  }
  else
  {
    aLeft[0] =  aNorm[2] / aProjLen;
    aLeft[2] = -aNorm[0] / aProjLen;
  }

  const Graphic3d_Vec3 F = Graphic3d_Vec3::Cross (-aLeft, aNorm);

  myOrientation.mat[0][0] = aLeft[0];
  myOrientation.mat[0][1] = aLeft[1];
  myOrientation.mat[0][2] = aLeft[2];
  myOrientation.mat[0][3] = 0.0f;

  myOrientation.mat[1][0] = aNorm[0];
  myOrientation.mat[1][1] = aNorm[1];
  myOrientation.mat[1][2] = aNorm[2];
  myOrientation.mat[1][3] = 0.0f;

  myOrientation.mat[2][0] = F[0];
  myOrientation.mat[2][1] = F[1];
  myOrientation.mat[2][2] = F[2];
  myOrientation.mat[2][3] = 0.0f;

  myOrientation.mat[3][0] = T[0];
  myOrientation.mat[3][1] = T[1];
  myOrientation.mat[3][2] = T[2];
  myOrientation.mat[3][3] = 1.0f;
}
