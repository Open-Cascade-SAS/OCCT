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

class Graphic3d_ArrayOfTriangles;
class Graphic3d_TransformPers;
class OpenGl_IndexBuffer;
class OpenGl_VertexBuffer;

//! Element rendering frame statistics.
class OpenGl_FrameStatsPrs : public OpenGl_Element
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

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const Standard_OVERRIDE;

protected:

  //! Update chart presentation.
  Standard_EXPORT void updateChart (const Handle(OpenGl_Workspace)& theWorkspace);

protected:

  Handle(OpenGl_FrameStats)          myStatsPrev;         //!< currently displayed stats
  Handle(Graphic3d_TransformPers)    myCountersTrsfPers;  //!< transformation persistence for counters presentation
  OpenGl_Text                        myCountersText;      //!< counters presentation
  OpenGl_Aspects                     myTextAspect;        //!< text aspect
  Handle(Graphic3d_TransformPers)    myChartTrsfPers;     //!< transformation persistence for chart presentation
  Handle(Graphic3d_ArrayOfTriangles) myChartArray;        //!< array of chart triangles
  Handle(OpenGl_VertexBuffer)        myChartVertices;     //!< VBO with chart triangles
  Handle(OpenGl_IndexBuffer)         myChartIndices;      //!< VBO with chart triangle indexes
  Handle(OpenGl_VertexBuffer)        myChartLines;        //!< array of chart lines
  OpenGl_Text                        myChartLabels[3];    //!< chart labels

};

#endif // _OpenGl_FrameStatsPrs_HeaderFile
