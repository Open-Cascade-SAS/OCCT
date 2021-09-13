// Created on: 2014-10-08
// Created by: Kirill Gavrilov
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

#ifndef _OpenGl_SetOfShaderPrograms_HeaderFile
#define _OpenGl_SetOfShaderPrograms_HeaderFile

#include <Graphic3d_ShaderFlags.hxx>
#include <Graphic3d_TypeOfShadingModel.hxx>
#include <NCollection_DataMap.hxx>

class OpenGl_ShaderProgram;

//! Alias to programs array of predefined length
class OpenGl_SetOfPrograms : public Standard_Transient
{
  DEFINE_STANDARD_RTTI_INLINE(OpenGl_SetOfPrograms, Standard_Transient)
public:

  //! Empty constructor
  OpenGl_SetOfPrograms() {}

  //! Access program by index
  Handle(OpenGl_ShaderProgram)& ChangeValue (Standard_Integer theProgramBits) { return myPrograms[theProgramBits]; }

protected:
  Handle(OpenGl_ShaderProgram) myPrograms[Graphic3d_ShaderFlags_NB]; //!< programs array
};

//! Alias to 2D programs array of predefined length
class OpenGl_SetOfShaderPrograms : public Standard_Transient
{
  DEFINE_STANDARD_RTTI_INLINE(OpenGl_SetOfShaderPrograms, Standard_Transient)
public:

  //! Empty constructor
  OpenGl_SetOfShaderPrograms() {}

  //! Constructor
  OpenGl_SetOfShaderPrograms (const Handle(OpenGl_SetOfPrograms)& thePrograms)
  {
    for (Standard_Integer aSetIter = 0; aSetIter < Graphic3d_TypeOfShadingModel_NB - 1; ++aSetIter)
    {
      myPrograms[aSetIter] = thePrograms;
    }
  }

  //! Access program by index
  Handle(OpenGl_ShaderProgram)& ChangeValue (Graphic3d_TypeOfShadingModel theShadingModel,
                                             Standard_Integer theProgramBits)
  {
    Handle(OpenGl_SetOfPrograms)& aSet = myPrograms[theShadingModel - 1];
    if (aSet.IsNull())
    {
      aSet = new OpenGl_SetOfPrograms();
    }
    return aSet->ChangeValue (theProgramBits);
  }

protected:
  Handle(OpenGl_SetOfPrograms) myPrograms[Graphic3d_TypeOfShadingModel_NB - 1]; //!< programs array, excluding Graphic3d_TypeOfShadingModel_Unlit
};

typedef NCollection_DataMap<TCollection_AsciiString, Handle(OpenGl_SetOfShaderPrograms)> OpenGl_MapOfShaderPrograms;

#endif // _OpenGl_SetOfShaderPrograms_HeaderFile
