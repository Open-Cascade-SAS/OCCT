// Copyright (c) 2021 OPEN CASCADE SAS
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

#ifndef _OpenGl_ShadowMap_HeaderFile
#define _OpenGl_ShadowMap_HeaderFile

#include <NCollection_Mat4.hxx>
#include <Standard_TypeDef.hxx>
#include <NCollection_Array1.hxx>
#include <OpenGl_NamedResource.hxx>

class gp_XYZ;
class Graphic3d_Camera;
class Graphic3d_CLight;
class Graphic3d_CView;
class OpenGl_FrameBuffer;
class OpenGl_Texture;

//! This class contains shadow mapping resources.
class OpenGl_ShadowMap : public OpenGl_NamedResource
{
  DEFINE_STANDARD_RTTIEXT(OpenGl_ShadowMap, OpenGl_NamedResource)
public:
  //! Empty constructor.
  OpenGl_ShadowMap();

  //! Releases all OpenGL resources.
  Standard_EXPORT virtual void Release(OpenGl_Context* theCtx) override;

  //! Returns estimated GPU memory usage for holding data without considering overheads and
  //! allocation alignment rules.
  Standard_EXPORT virtual size_t EstimatedDataSize() const override;

  //! Destructor.
  Standard_EXPORT virtual ~OpenGl_ShadowMap();

  //! Return TRUE if defined.
  Standard_EXPORT bool IsValid() const;

  //! Return framebuffer.
  const occ::handle<OpenGl_FrameBuffer>& FrameBuffer() const { return myShadowMapFbo; }

  //! Return depth texture.
  Standard_EXPORT const occ::handle<OpenGl_Texture>& Texture() const;

  //! Return light source casting the shadow or NULL if undefined.
  const occ::handle<Graphic3d_CLight>& LightSource() const { return myShadowLight; }

  //! Set light source casting the shadow.
  void SetLightSource(const occ::handle<Graphic3d_CLight>& theLight) { myShadowLight = theLight; }

  //! Return rendering camera.
  const occ::handle<Graphic3d_Camera>& Camera() const { return myShadowCamera; }

  //! Return light source mapping matrix.
  const NCollection_Mat4<float>& LightSourceMatrix() const { return myLightMatrix; }

  //! Set light source mapping matrix.
  void SetLightSourceMatrix(const NCollection_Mat4<float>& theMat) { myLightMatrix = theMat; }

  //! Returns shadowmap bias.
  float ShadowMapBias() const { return myShadowMapBias; }

  //! Sets shadowmap bias.
  void SetShadowMapBias(float theBias) { myShadowMapBias = theBias; }

  //! Compute camera.
  //! @param[in] theView    active view
  //! @param[in] theOrigin  when not-NULL - displace shadow map camera to specified Z-Layer origin
  Standard_EXPORT bool UpdateCamera(const Graphic3d_CView& theView, const gp_XYZ* theOrigin = NULL);

private:
  occ::handle<OpenGl_FrameBuffer> myShadowMapFbo;  //!< frame buffer for rendering shadow map
  occ::handle<Graphic3d_CLight>   myShadowLight;   //!< light source to render shadow map
  occ::handle<Graphic3d_Camera>   myShadowCamera;  //!< rendering camera
  NCollection_Mat4<float>         myLightMatrix;   //!< light source matrix
  float                           myShadowMapBias; //!< shadowmap bias
};

//! Array of shadow maps.
class OpenGl_ShadowMapArray : public Standard_Transient,
                              public NCollection_Array1<occ::handle<OpenGl_ShadowMap>>
{
public:
  //! Empty constructor.
  OpenGl_ShadowMapArray() {}

  //! Releases all OpenGL resources.
  Standard_EXPORT void Release(OpenGl_Context* theCtx);

  //! Return TRUE if defined.
  bool IsValid() const { return !IsEmpty() && First()->IsValid(); }

  //! Returns estimated GPU memory usage for holding data without considering overheads and
  //! allocation alignment rules.
  Standard_EXPORT size_t EstimatedDataSize() const;

public:
  DEFINE_STANDARD_ALLOC
  DEFINE_NCOLLECTION_ALLOC
};

#endif // _OpenGl_ShadowMap_HeaderFile
