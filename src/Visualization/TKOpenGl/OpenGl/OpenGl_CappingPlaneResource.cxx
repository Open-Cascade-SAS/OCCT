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

#include <OpenGl_CappingPlaneResource.hxx>

#include <OpenGl_Context.hxx>
#include <OpenGl_Vec.hxx>
#include <OpenGl_ShaderManager.hxx>

IMPLEMENT_STANDARD_RTTIEXT(OpenGl_CappingPlaneResource, OpenGl_Resource)

namespace
{
//! 12 plane vertices, interleaved:
//!  - 4 floats, position
//!  - 4 floats, normal
//!  - 4 floats, UV texture coordinates
static const GLfloat THE_CAPPING_PLN_VERTS[12 * (4 + 4 + 4)] = {
  0.0f,  0.0f, 0.0f,  1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,  0.0f,  0.0f, 1.0f,
  0.0f,  0.0f, 1.0f,  0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
  1.0f,  0.0f, 0.0f,  0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,  0.0f,  0.0f, 0.0f,

  0.0f,  0.0f, 0.0f,  1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,  0.0f,  0.0f, 1.0f,
  -1.0f, 0.0f, 0.0f,  0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f,  0.0f, 0.0f,
  0.0f,  0.0f, 1.0f,  0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,

  0.0f,  0.0f, 0.0f,  1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,  0.0f,  0.0f, 1.0f,
  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,  1.0f,  0.0f, 0.0f,
  -1.0f, 0.0f, 0.0f,  0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f,  0.0f, 0.0f,

  0.0f,  0.0f, 0.0f,  1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,  0.0f,  0.0f, 1.0f,
  1.0f,  0.0f, 0.0f,  0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,  0.0f,  0.0f, 0.0f,
  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,  1.0f,  0.0f, 0.0f};
} // namespace

//=================================================================================================

OpenGl_CappingPlaneResource::OpenGl_CappingPlaneResource(
  const occ::handle<Graphic3d_ClipPlane>& thePlane)
    : myPrimitives(NULL),
      myOrientation(NCollection_Mat4<float>::Identity()),
      myAspect(NULL),
      myPlaneRoot(thePlane),
      myEquationMod((unsigned int)-1),
      myAspectMod((unsigned int)-1)
{
  occ::handle<Graphic3d_Buffer> anAttribs = new Graphic3d_Buffer(Graphic3d_Buffer::DefaultAllocator());
  Graphic3d_Attribute      anAttribInfo[] = {{Graphic3d_TOA_POS, Graphic3d_TOD_VEC4},
                                             {Graphic3d_TOA_NORM, Graphic3d_TOD_VEC4},
                                             {Graphic3d_TOA_UV, Graphic3d_TOD_VEC4}};
  if (anAttribs->Init(12, anAttribInfo, 3))
  {
    memcpy(anAttribs->ChangeData(), THE_CAPPING_PLN_VERTS, sizeof(THE_CAPPING_PLN_VERTS));
    myPrimitives.InitBuffers(NULL, Graphic3d_TOPA_TRIANGLES, NULL, anAttribs, NULL);
  }
}

//=================================================================================================

OpenGl_CappingPlaneResource::~OpenGl_CappingPlaneResource()
{
  Release(NULL);
}

//=================================================================================================

void OpenGl_CappingPlaneResource::Update(const occ::handle<OpenGl_Context>&    theCtx,
                                         const occ::handle<Graphic3d_Aspects>& theObjAspect)
{
  updateTransform(theCtx);
  updateAspect(theObjAspect);
}

//=================================================================================================

void OpenGl_CappingPlaneResource::Release(OpenGl_Context* theContext)
{
  OpenGl_Element::Destroy(theContext, myAspect);
  myPrimitives.Release(theContext);
  myEquationMod = (unsigned int)-1;
  myAspectMod   = (unsigned int)-1;
}

//=================================================================================================

void OpenGl_CappingPlaneResource::updateAspect(const occ::handle<Graphic3d_Aspects>& theObjAspect)
{
  if (myAspect == NULL)
  {
    myAspect    = new OpenGl_Aspects();
    myAspectMod = myPlaneRoot->MCountAspect() - 1; // mark out of sync
  }

  if (theObjAspect.IsNull())
  {
    if (myAspectMod != myPlaneRoot->MCountAspect())
    {
      myAspect->SetAspect(myPlaneRoot->CappingAspect());
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
    myFillAreaAspect->SetFrontMaterial(theObjAspect->FrontMaterial());
    myFillAreaAspect->SetInteriorColor(theObjAspect->InteriorColor());
  }
  if (myPlaneRoot->ToUseObjectTexture())
  {
    myFillAreaAspect->SetTextureSet(theObjAspect->TextureSet());
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
    myFillAreaAspect->SetShaderProgram(theObjAspect->ShaderProgram());
  }

  myAspect->SetAspect(myFillAreaAspect);
}

//=================================================================================================

void OpenGl_CappingPlaneResource::updateTransform(const occ::handle<OpenGl_Context>& theCtx)
{
  if (myEquationMod == myPlaneRoot->MCountEquation()
      && myLocalOrigin.IsEqual(theCtx->ShaderManager()->LocalOrigin(), gp::Resolution()))
  {
    return; // nothing to update
  }

  myEquationMod = myPlaneRoot->MCountEquation();
  myLocalOrigin = theCtx->ShaderManager()->LocalOrigin();

  const Graphic3d_ClipPlane::Equation& anEq = myPlaneRoot->GetEquation();
  const double anEqW = theCtx->ShaderManager()->LocalClippingPlaneW(*myPlaneRoot);

  // re-evaluate infinite plane transformation matrix
  const NCollection_Vec3<float> aNorm(anEq.xyz());
  const NCollection_Vec3<float> T(anEq.xyz() * -anEqW);

  // project plane normal onto OX to find left vector
  const float aProjLen = sqrt((float)anEq.xz().SquareModulus());
  NCollection_Vec3<float>           aLeft;
  if (aProjLen < ShortRealSmall())
  {
    aLeft[0] = 1.0f;
  }
  else
  {
    aLeft[0] = aNorm[2] / aProjLen;
    aLeft[2] = -aNorm[0] / aProjLen;
  }

  const NCollection_Vec3<float> F = NCollection_Vec3<float>::Cross(-aLeft, aNorm);
  myOrientation.SetColumn(0, aLeft);
  myOrientation.SetColumn(1, aNorm);
  myOrientation.SetColumn(2, F);
  myOrientation.SetColumn(3, T);
}
