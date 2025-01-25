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

#ifndef _Graphic3d_TransformPersScaledAbove_HeaderFile
#define _Graphic3d_TransformPersScaledAbove_HeaderFile

#include <Graphic3d_TransformPers.hxx>

DEFINE_STANDARD_HANDLE(Graphic3d_TransformPersScaledAbove, Graphic3d_TransformPers)

//! Transformation Zoom persistence with the above boundary of scale.
//! This persistence works only when the camera scale value is below the scale value of this
//! persistence. Otherwise, no persistence is applied.
class Graphic3d_TransformPersScaledAbove : public Graphic3d_TransformPers
{
public:
  //! Create a Zoom transformation persistence with an anchor 3D point and a scale value
  Standard_EXPORT Graphic3d_TransformPersScaledAbove(const Standard_Real theScale,
                                                     const gp_Pnt&       thePnt);

  //! Destructor
  virtual ~Graphic3d_TransformPersScaledAbove() {}

  //! Find scale value based on the camera position and view dimensions
  //! If the camera scale value less than the persistence scale, zoom persistence is not applied.
  //! @param[in] theCamera  camera definition
  //! @param[in] theViewportWidth  the width of viewport.
  //! @param[in] theViewportHeight  the height of viewport.
  Standard_EXPORT virtual Standard_Real persistentScale(
    const Handle(Graphic3d_Camera)& theCamera,
    const Standard_Integer          theViewportWidth,
    const Standard_Integer          theViewportHeight) const Standard_OVERRIDE;

public:
  DEFINE_STANDARD_RTTIEXT(Graphic3d_TransformPersScaledAbove, Graphic3d_TransformPers)

private:
  Standard_Real myScale; //!< scale bound value
};

#endif
