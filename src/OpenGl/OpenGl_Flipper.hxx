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

#ifndef OpenGl_Flipper_HeaderFile
#define OpenGl_Flipper_HeaderFile

#include <OpenGl_Element.hxx>
#include <OpenGl_Vec.hxx>

class gp_Ax2;

//! Being rendered, the elements modifies current model-view matrix such that the axes of
//! the specified reference system (in model space) become oriented in the following way:
//! - X    - heads to the right side of view.
//! - Y    - heads to the up side of view.
//! - N(Z) - heads towards the screen.
//! Originally, this element serves for need of flipping the 3D text of dimension presentations.
class OpenGl_Flipper : public OpenGl_Element
{
public:

  //! Construct rendering element to flip model-view matrix
  //! along the reference system to ensure up-Y, right-X orientation.
  //! @param theReferenceSystem [in] the reference coordinate system.
  Standard_EXPORT OpenGl_Flipper (const gp_Ax2& theReferenceSystem);

  //! Set options for the element.
  //! @param theIsEnabled [in] flag indicates whether the flipper
  //! matrix modification should be set up or restored back.
  void SetOptions (const Standard_Boolean theIsEnabled) { myIsEnabled = theIsEnabled; }

  Standard_EXPORT virtual void Render (const Handle(OpenGl_Workspace)& theWorkspace) const Standard_OVERRIDE;
  Standard_EXPORT virtual void Release (OpenGl_Context* theCtx) Standard_OVERRIDE;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const Standard_OVERRIDE;

public:

  DEFINE_STANDARD_ALLOC

protected:

  OpenGl_Vec4      myReferenceOrigin;
  OpenGl_Vec4      myReferenceX;
  OpenGl_Vec4      myReferenceY;
  OpenGl_Vec4      myReferenceZ;
  Standard_Boolean myIsEnabled;

};

#endif // OpenGl_Flipper_Header
