// Created on: 2014-10-08
// Created by: Denis BOGOLEPOV
// Copyright (c) 2014 OPEN CASCADE SAS
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

#ifndef _OpenGl_Sampler_Header
#define _OpenGl_Sampler_Header

#include <Graphic3d_TextureParams.hxx>
#include <OpenGl_Resource.hxx>

class OpenGl_Texture;

//! Class implements OpenGL sampler object resource that
//! stores the sampling parameters for a texture access.
class OpenGl_Sampler : public OpenGl_Resource
{
  friend class OpenGl_Context;
  friend class OpenGl_Texture;
  friend class OpenGl_Text;
  DEFINE_STANDARD_RTTIEXT(OpenGl_Sampler, OpenGl_Resource)
public:
  //! Helpful constant defining invalid sampler identifier
  static const unsigned int NO_SAMPLER = 0;

public:
  //! Creates new sampler object.
  Standard_EXPORT OpenGl_Sampler(const Handle(Graphic3d_TextureParams)& theParams);

  //! Releases resources of sampler object.
  Standard_EXPORT virtual ~OpenGl_Sampler();

  //! Destroys object - will release GPU memory if any.
  Standard_EXPORT virtual void Release(OpenGl_Context* theContext) Standard_OVERRIDE;

  //! Returns estimated GPU memory usage - not implemented.
  virtual Standard_Size EstimatedDataSize() const Standard_OVERRIDE { return 0; }

  //! Creates an uninitialized sampler object.
  Standard_EXPORT Standard_Boolean Create(const Handle(OpenGl_Context)& theContext);

  //! Creates and initializes sampler object.
  //! Existing object will be reused if possible, however if existing Sampler Object has Immutable
  //! flag and texture parameters should be re-initialized, then Sampler Object will be recreated.
  Standard_EXPORT Standard_Boolean Init(const Handle(OpenGl_Context)& theContext,
                                        const OpenGl_Texture&         theTexture);

  //! Returns true if current object was initialized.
  Standard_Boolean IsValid() const { return isValidSampler(); }

  //! Binds sampler object to texture unit specified in parameters.
  void Bind(const Handle(OpenGl_Context)& theCtx) { Bind(theCtx, myParams->TextureUnit()); }

  //! Unbinds sampler object from texture unit specified in parameters.
  void Unbind(const Handle(OpenGl_Context)& theCtx) { Unbind(theCtx, myParams->TextureUnit()); }

  //! Binds sampler object to the given texture unit.
  Standard_EXPORT void Bind(const Handle(OpenGl_Context)& theCtx,
                            const Graphic3d_TextureUnit   theUnit);

  //! Unbinds sampler object from the given texture unit.
  Standard_EXPORT void Unbind(const Handle(OpenGl_Context)& theCtx,
                              const Graphic3d_TextureUnit   theUnit);

  //! Sets specific sampler parameter.
  void SetParameter(const Handle(OpenGl_Context)& theCtx,
                    unsigned int                  theTarget,
                    unsigned int                  theParam,
                    Standard_Integer              theValue)
  {
    setParameter(theCtx, this, theTarget, theParam, theValue);
  }

  //! Returns OpenGL sampler ID.
  unsigned int SamplerID() const { return mySamplerID; }

  //! Return immutable flag preventing further modifications of sampler parameters, FALSE by
  //! default. Immutable flag might be set when Sampler Object is used within Bindless Texture.
  bool IsImmutable() const { return myIsImmutable; }

  //! Setup immutable flag. It is not possible unsetting this flag without Sampler destruction.
  void SetImmutable() { myIsImmutable = true; }

  //! Returns texture parameters.
  const Handle(Graphic3d_TextureParams)& Parameters() { return myParams; }

  //! Sets texture parameters.
  Standard_EXPORT void SetParameters(const Handle(Graphic3d_TextureParams)& theParams);

  //! Returns texture parameters initialization state.
  bool ToUpdateParameters() const { return mySamplerRevision != myParams->SamplerRevision(); }

protected:
  //! Checks if sampler object is valid.
  Standard_Boolean isValidSampler() const { return mySamplerID != NO_SAMPLER; }

  //! Sets specific sampler parameter.
  Standard_EXPORT static void setParameter(const Handle(OpenGl_Context)& theContext,
                                           OpenGl_Sampler*               theSampler,
                                           unsigned int                  theTarget,
                                           unsigned int                  theParam,
                                           Standard_Integer              theValue);

  //! Apply sampler parameters.
  //! @param[in] theCtx      active OpenGL context
  //! @param[in] theParams   texture parameters to apply
  //! @param[in] theSampler  apply parameters to Texture object (NULL)
  //!                        or to specified Sampler object (non-NULL, sampler is not required to be
  //!                        bound)
  //! @param[in] theTarget   OpenGL texture target
  //! @param[in] theMaxMipLevel  maximum mipmap level defined within the texture
  Standard_EXPORT static void applySamplerParams(const Handle(OpenGl_Context)&          theCtx,
                                                 const Handle(Graphic3d_TextureParams)& theParams,
                                                 OpenGl_Sampler*                        theSampler,
                                                 const unsigned int                     theTarget,
                                                 const Standard_Integer theMaxMipLevel);

  //! Apply global texture state for deprecated OpenGL functionality.
  Standard_EXPORT static void applyGlobalTextureParams(
    const Handle(OpenGl_Context)&          theCtx,
    const OpenGl_Texture&                  theTexture,
    const Handle(Graphic3d_TextureParams)& theParams);

  //! Reset global texture state for deprecated OpenGL functionality.
  Standard_EXPORT static void resetGlobalTextureParams(
    const Handle(OpenGl_Context)&          theCtx,
    const OpenGl_Texture&                  theTexture,
    const Handle(Graphic3d_TextureParams)& theParams);

protected:
  Handle(Graphic3d_TextureParams) myParams; //!< texture parameters
  // clang-format off
  unsigned int                    mySamplerRevision; //!< modification counter of parameters related to sampler state
  unsigned int                    mySamplerID;       //!< OpenGL sampler object ID
  bool                            myIsImmutable;     //!< immutable flag preventing further modifications of sampler parameters, FALSE by default
  // clang-format on
};

DEFINE_STANDARD_HANDLE(OpenGl_Sampler, OpenGl_Resource)

#endif // _OpenGl_Sampler_Header
