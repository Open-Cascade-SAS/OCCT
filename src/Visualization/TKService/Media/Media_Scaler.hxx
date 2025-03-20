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

#ifndef _Media_Scaler_HeaderFile
#define _Media_Scaler_HeaderFile

#include <Media_Frame.hxx>

#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <Graphic3d_Vec2.hxx>

struct SwsContext;

//! SwsContext wrapper - tool performing image scaling and pixel format conversion.
class Media_Scaler : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Media_Scaler, Standard_Transient)
public:
  //! Empty constructor.
  Standard_EXPORT Media_Scaler();

  //! Destructor.
  Standard_EXPORT virtual ~Media_Scaler();

  //! sws_freeContext() wrapper.
  Standard_EXPORT void Release();

  //! sws_getContext() wrapper - creates conversion context.
  //! @param theSrcDims   dimensions of input frame
  //! @param theSrcFormat pixel format (AVPixelFormat) of input frame
  //! @param theResDims   dimensions of destination frame
  //! @param theResFormat pixel format (AVPixelFormat) of destination frame
  Standard_EXPORT bool Init(const Graphic3d_Vec2i& theSrcDims,
                            int                    theSrcFormat,
                            const Graphic3d_Vec2i& theResDims,
                            int                    theResFormat);

  //! Convert one frame to another.
  Standard_EXPORT bool Convert(const Handle(Media_Frame)& theSrc,
                               const Handle(Media_Frame)& theRes);

  //! Return TRUE if context was initialized.
  bool IsValid() const { return mySwsContext != NULL; }

protected:
  SwsContext*     mySwsContext; //!< conversion context
  Graphic3d_Vec2i mySrcDims;    //!< dimensions of input frame
  int             mySrcFormat;  //!< pixel format (AVPixelFormat) of input frame
  Graphic3d_Vec2i myResDims;    //!< dimensions of destination frame
  int             myResFormat;  //!< pixel format (AVPixelFormat) of destination frame
};

#endif // _Media_Scaler_HeaderFile
