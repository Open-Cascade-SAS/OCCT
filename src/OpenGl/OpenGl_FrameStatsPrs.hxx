// Copyright (c) 2017 OPEN CASCADE SAS
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

#ifndef _OpenGl_FrameStatsPrs_HeaderFile
#define _OpenGl_FrameStatsPrs_HeaderFile

#include <OpenGl_FrameStats.hxx>
#include <OpenGl_Text.hxx>

class Graphic3d_TransformPers;

//! Element rendering frame statistics.
class OpenGl_FrameStatsPrs : public OpenGl_Text
{
public:

  //! Default constructor.
  Standard_EXPORT OpenGl_FrameStatsPrs();

  //! Destructor
  Standard_EXPORT virtual ~OpenGl_FrameStatsPrs();

  //! Render element.
  Standard_EXPORT virtual void Render (const Handle(OpenGl_Workspace)& theWorkspace) const Standard_OVERRIDE;

  //! Release OpenGL resources.
  Standard_EXPORT virtual void Release (OpenGl_Context* theCtx) Standard_OVERRIDE;

  //! Update text.
  Standard_EXPORT void Update (const Handle(OpenGl_Workspace)& theWorkspace);

  //! Assign text aspect.
  void SetTextAspect (const Handle(Graphic3d_AspectText3d)& theAspect) { myTextAspect.SetAspect (theAspect); }

protected:

  OpenGl_AspectText               myTextAspect; //!< text aspect
  Handle(Graphic3d_TransformPers) myTrsfPers;   //!< transformation persistence
  Handle(OpenGl_FrameStats)       myStatsPrev;  //!< currently displayed stats

};

#endif // _OpenGl_FrameStatsPrs_HeaderFile
