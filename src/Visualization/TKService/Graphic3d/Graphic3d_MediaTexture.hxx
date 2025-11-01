// Created by: Kirill GAVRILOV
// Copyright (c) 2019 OPEN CASCADE SAS
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

#ifndef _Graphic3d_MediaTexture_HeaderFile
#define _Graphic3d_MediaTexture_HeaderFile

#include <Graphic3d_Texture2D.hxx>
#include <Standard_Mutex.hxx>

class Media_Frame;

//! Texture adapter for Media_Frame.
class Graphic3d_MediaTexture : public Graphic3d_Texture2D
{
  DEFINE_STANDARD_RTTIEXT(Graphic3d_MediaTexture, Graphic3d_Texture2D)
public:
  //! Main constructor.
  Standard_EXPORT Graphic3d_MediaTexture(const Handle(Standard_HMutex)& theMutex,
                                         Standard_Integer               thePlane = -1);

  //! Image reader.
  Standard_EXPORT virtual Handle(Image_PixMap) GetImage(
    const Handle(Image_SupportedFormats)& theSupported) Standard_OVERRIDE;

  //! Return the frame.
  const Handle(Media_Frame)& Frame() const { return myFrame; }

  //! Set the frame.
  void SetFrame(const Handle(Media_Frame)& theFrame) { myFrame = theFrame; }

  //! Regenerate a new texture id
  void GenerateNewId() { generateId(); }

protected:
  mutable Handle(Standard_HMutex) myMutex;
  Handle(Media_Frame)             myFrame;
  Standard_Integer                myPlane;
  mutable Handle(Image_PixMap)    myPixMapWrapper;
};

#endif // _Graphic3d_MediaTexture_HeaderFile
