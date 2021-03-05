// Created on: 2013-10-02
// Created by: Denis BOGOLEPOV
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

#ifndef _OpenGl_MaterialState_HeaderFile
#define _OpenGl_MaterialState_HeaderFile

#include <OpenGl_ShaderStates.hxx>
#include <OpenGl_Material.hxx>

//! Defines generic state of material properties.
class OpenGl_MaterialState : public OpenGl_StateInterface
{
public:

  //! Creates new material state.
  OpenGl_MaterialState() : myAlphaCutoff (0.5f), myToDistinguish (false), myToMapTexture (false) {}

  //! Sets new material aspect.
  void Set (const OpenGl_Material& theMat,
            const float theAlphaCutoff,
            const bool theToDistinguish,
            const bool theToMapTexture)
  {
    myMaterial      = theMat;
    myAlphaCutoff   = theAlphaCutoff;
    myToDistinguish = theToDistinguish;
    myToMapTexture  = theToMapTexture;
  }

  //! Return front material.
  const OpenGl_Material& Material() const { return myMaterial; }

  //! Alpha cutoff value.
  float AlphaCutoff() const { return myAlphaCutoff; }

  //! Return TRUE if alpha test should be enabled.
  bool HasAlphaCutoff() const { return myAlphaCutoff <= 1.0f; }

  //! Distinguish front/back flag.
  bool ToDistinguish() const { return myToDistinguish; }

  //! Flag for mapping a texture.
  bool ToMapTexture()  const { return myToMapTexture; }

private:

  OpenGl_Material myMaterial;      //!< material
  float           myAlphaCutoff;   //!< alpha cutoff value
  bool            myToDistinguish; //!< distinguish front/back flag
  bool            myToMapTexture;  //!< flag for mapping a texture

};

#endif // _OpenGl_MaterialState_HeaderFile
