// Copyright (c) 2023 OPEN CASCADE SAS
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

#ifndef _AIS_AnimationAxisRotation_HeaderFile
#define _AIS_AnimationAxisRotation_HeaderFile

#include <AIS_BaseAnimationObject.hxx>
#include <gp_TrsfNLerp.hxx>

//! Animation defining object transformation.
class AIS_AnimationAxisRotation : public AIS_BaseAnimationObject
{
  DEFINE_STANDARD_RTTIEXT(AIS_AnimationAxisRotation, AIS_BaseAnimationObject)
public:

  //! Constructor with initialization.
  //! @param[in] theAnimationName animation identifier
  //! @param[in] theContext       interactive context where object have been displayed
  //! @param[in] theObject        object to apply rotation
  //! @param[in] theAxis          rotation axis
  //! @param[in] theAngleStart    rotation angle at the start of animation
  //! @param[in] theAngleEnd      rotation angle at the end   of animation
  Standard_EXPORT AIS_AnimationAxisRotation (const TCollection_AsciiString& theAnimationName,
                                             const Handle(AIS_InteractiveContext)& theContext,
                                             const Handle(AIS_InteractiveObject)& theObject,
                                             const gp_Ax1& theAxis,
                                             const Standard_Real theAngleStart,
                                             const Standard_Real theAngleEnd);

protected:

  //! Update the progress.
  Standard_EXPORT virtual void update (const AIS_AnimationProgress& theProgress) Standard_OVERRIDE;

private:

  gp_Ax1         myRotAxis;     //!< rotation axis
  Standard_Real  myAngleStart;  //!< start angle for rotation
  Standard_Real  myAngleEnd;    //!< end angle for rotation

};

#endif // _AIS_AnimationAxisRotation_HeaderFile
