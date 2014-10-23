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

#ifndef OPENGL_SAMPLER_H
#define OPENGL_SAMPLER_H

#include <OpenGl_Context.hxx>
#include <Handle_OpenGl_Sampler.hxx>

//! Class implements OpenGL sampler object resource that
//! stores the sampling parameters for a texture access.
class OpenGl_Sampler : public OpenGl_Resource
{
public:

  //! Helpful constant defining invalid sampler identifier
  static const GLuint NO_SAMPLER = 0;

public:

  //! Creates new sampler object.
  Standard_EXPORT OpenGl_Sampler();

  //! Releases resources of sampler object.
  Standard_EXPORT virtual ~OpenGl_Sampler();

  //! Destroys object - will release GPU memory if any.
  Standard_EXPORT virtual void Release (OpenGl_Context* theContext);

  //! Initializes sampler object.
  Standard_EXPORT Standard_Boolean Init (OpenGl_Context& theContext);

  //! Returns true if current object was initialized.
  Standard_Boolean IsValid() const
  {
    return isValidSampler();
  }

  //! Binds sampler object to the given texture unit.
  Standard_EXPORT void Bind (OpenGl_Context& theContext, const GLuint theUnit = 0);

  //! Unbinds sampler object from the given texture unit.
  Standard_EXPORT void Unbind (OpenGl_Context& theContext, const GLuint theUnit = 0);

  //! Sets specific sampler parameter.
  Standard_EXPORT void SetParameter (OpenGl_Context& theContext, const GLenum theParam, const GLint theValue);

  //! Returns OpenGL sampler ID.
  GLuint SamplerID() const
  {
    return mySamplerID;
  }

protected:

  //! Checks if sampler object is valid.
  Standard_Boolean isValidSampler() const
  {
    return mySamplerID != NO_SAMPLER;
  }

protected:

  GLuint mySamplerID; //!< OpenGL sampler object ID

public:

  DEFINE_STANDARD_RTTI(OpenGl_Sampler)

};

#endif // OPENGL_SAMPLER_H
