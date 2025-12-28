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

#include <OpenGl_Aspects.hxx>
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
  Standard_EXPORT ~OpenGl_FrameStatsPrs() override;

  //! Render element.
  Standard_EXPORT void Render(
    const occ::handle<OpenGl_Workspace>& theWorkspace) const override;

  //! Release OpenGL resources.
  Standard_EXPORT void Release(OpenGl_Context* theCtx) override;

  //! Update text.
  Standard_EXPORT void Update(const occ::handle<OpenGl_Workspace>& theWorkspace);

  //! Assign text aspect.
  void SetTextAspect(const occ::handle<Graphic3d_AspectText3d>& theAspect)
  {
    myTextAspect.SetAspect(theAspect);
  }

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream,
                                        int               theDepth = -1) const override;

protected:
  //! Update chart presentation.
  Standard_EXPORT void updateChart(const occ::handle<OpenGl_Workspace>& theWorkspace);

protected:
  occ::handle<OpenGl_FrameStats> myStatsPrev;               //!< currently displayed stats
                                                            // clang-format off
  occ::handle<Graphic3d_TransformPers>    myCountersTrsfPers;  //!< transformation persistence for counters presentation
  OpenGl_Text                        myCountersText;      //!< counters presentation
  OpenGl_Aspects                     myTextAspect;        //!< text aspect
  occ::handle<Graphic3d_TransformPers>    myChartTrsfPers;     //!< transformation persistence for chart presentation
                                                            // clang-format on
  occ::handle<Graphic3d_ArrayOfTriangles> myChartArray;     //!< array of chart triangles
  occ::handle<OpenGl_VertexBuffer>        myChartVertices;  //!< VBO with chart triangles
  occ::handle<OpenGl_IndexBuffer>         myChartIndices;   //!< VBO with chart triangle indexes
  occ::handle<OpenGl_VertexBuffer>        myChartLines;     //!< array of chart lines
  OpenGl_Text                             myChartLabels[3]; //!< chart labels
};

#endif // _OpenGl_FrameStatsPrs_HeaderFile
