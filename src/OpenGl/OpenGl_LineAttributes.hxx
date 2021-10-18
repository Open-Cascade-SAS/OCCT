// Created on: 2011-09-20
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2014 OPEN CASCADE SAS
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

#ifndef OpenGl_LineAttributes_HeaderFile
#define OpenGl_LineAttributes_HeaderFile

#include <OpenGl_Resource.hxx>

#include <Graphic3d_HatchStyle.hxx>
#include <NCollection_DataMap.hxx>

typedef NCollection_DataMap<Handle(Graphic3d_HatchStyle), unsigned int> OpenGl_MapOfHatchStylesAndIds;

class OpenGl_Context;

DEFINE_STANDARD_HANDLE(OpenGl_LineAttributes, OpenGl_Resource)

//! Utility class to manage OpenGL resources of polygon hatching styles.
//! @note the implementation is not supported by Core Profile and by ES version.
class OpenGl_LineAttributes : public OpenGl_Resource
{
  DEFINE_STANDARD_RTTIEXT(OpenGl_LineAttributes, OpenGl_Resource)
public:

  //! Default constructor.
  Standard_EXPORT OpenGl_LineAttributes();

  //! Default destructor.
  Standard_EXPORT virtual ~OpenGl_LineAttributes();

  //! Release GL resources.
  Standard_EXPORT virtual void Release (OpenGl_Context* theGlCtx) Standard_OVERRIDE;

  //! Returns estimated GPU memory usage - not implemented.
  virtual Standard_Size EstimatedDataSize() const Standard_OVERRIDE { return 0; }

  //! Sets type of the hatch.
  Standard_EXPORT bool SetTypeOfHatch (const OpenGl_Context*               theGlCtx,
                                       const Handle(Graphic3d_HatchStyle)& theStyle);

private:

  unsigned int init (const OpenGl_Context* theGlCtx,
                     const Handle(Graphic3d_HatchStyle)& theStyle);

protected:

  OpenGl_MapOfHatchStylesAndIds myStyles; //!< Hatch patterns

};

#endif // _OpenGl_LineAttributes_Header
