// Created by: Anastasia BORISOVA
// Copyright (c) 2016 OPEN CASCADE SAS
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

#ifndef _AIS_AnimationObject_HeaderFile
#define _AIS_AnimationObject_HeaderFile

#include <AIS_BaseAnimationObject.hxx>
#include <gp_TrsfNLerp.hxx>

//! Animation defining object transformation.
class AIS_AnimationObject : public AIS_BaseAnimationObject
{
  DEFINE_STANDARD_RTTIEXT(AIS_AnimationObject, AIS_BaseAnimationObject)
public:

  //! Constructor with initialization.
  //! Note that start/end transformations specify exactly local transformation of the object,
  //! not the transformation to be applied to existing local transformation.
  //! @param[in] theAnimationName animation identifier
  //! @param[in] theContext       interactive context where object have been displayed
  //! @param[in] theObject        object to apply local transformation
  //! @param[in] theTrsfStart     local transformation at the start of animation (e.g. theObject->LocalTransformation())
  //! @param[in] theTrsfEnd       local transformation at the end   of animation
  Standard_EXPORT AIS_AnimationObject (const TCollection_AsciiString& theAnimationName,
                                       const Handle(AIS_InteractiveContext)& theContext,
                                       const Handle(AIS_InteractiveObject)&  theObject,
                                       const gp_Trsf& theTrsfStart,
                                       const gp_Trsf& theTrsfEnd);

protected:

  //! Update the progress.
  Standard_EXPORT virtual void update (const AIS_AnimationProgress& theProgress) Standard_OVERRIDE;

private:

  gp_TrsfNLerp  myTrsfLerp; //!< interpolation tool

};

#endif // _AIS_AnimationObject_HeaderFile
