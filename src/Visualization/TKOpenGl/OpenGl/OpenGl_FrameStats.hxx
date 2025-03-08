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

#ifndef _OpenGl_FrameStats_HeaderFile
#define _OpenGl_FrameStats_HeaderFile

#include <Graphic3d_FrameStats.hxx>
#include <NCollection_IndexedMap.hxx>

class Graphic3d_CStructure;

//! Class storing the frame statistics.
class OpenGl_FrameStats : public Graphic3d_FrameStats
{
  DEFINE_STANDARD_RTTIEXT(OpenGl_FrameStats, Graphic3d_FrameStats)
public:
  //! Default constructor.
  Standard_EXPORT OpenGl_FrameStats();

  //! Destructor.
  Standard_EXPORT virtual ~OpenGl_FrameStats();

public:
  //! Copy stats values into another instance (create new instance, if not exists).
  //! The main use of this method is to track changes in statistics (e.g. in conjunction with
  //! IsEqual() method).
  //! @return TRUE if frame data has been changed so that the presentation should be updated
  Standard_EXPORT virtual bool IsFrameUpdated(Handle(OpenGl_FrameStats)& thePrev) const;

protected:
  //! Method to collect statistics from the View; called by FrameEnd().
  Standard_EXPORT virtual void updateStatistics(const Handle(Graphic3d_CView)& theView,
                                                bool theIsImmediateOnly) Standard_OVERRIDE;

  //! Updates counters for structures.
  Standard_EXPORT virtual void updateStructures(
    Standard_Integer                                           theViewId,
    const NCollection_IndexedMap<const Graphic3d_CStructure*>& theStructures,
    Standard_Boolean                                           theToCountElems,
    Standard_Boolean                                           theToCountTris,
    Standard_Boolean                                           theToCountMem);
};

DEFINE_STANDARD_HANDLE(OpenGl_FrameStats, Graphic3d_FrameStats)

#endif // _OpenGl_FrameStats_HeaderFile
