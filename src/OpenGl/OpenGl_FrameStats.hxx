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

#include <Graphic3d_RenderingParams.hxx>
#include <NCollection_IndexedMap.hxx>
#include <OSD_Timer.hxx>
#include <Standard_Type.hxx>
#include <Standard_Transient.hxx>

class OpenGl_Workspace;
class OpenGl_Structure;
typedef NCollection_IndexedMap<const OpenGl_Structure*> OpenGl_IndexedMapOfStructure;

//! Class storing the frame statistics.
class OpenGl_FrameStats : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(OpenGl_FrameStats, Standard_Transient)
public:

  //! Stats counter.
  enum Counter
  {
    Counter_NbLayers = 0,           //!< number of ZLayers
    Counter_NbLayersNotCulled,      //!< number of not culled ZLayers
    Counter_NbStructs,              //!< number of defined OpenGl_Structure
    Counter_NbStructsNotCulled,     //!< number of not culled OpenGl_Structure
    Counter_NbGroupsNotCulled,      //!< number of not culled OpenGl_Group
    Counter_NbElemsNotCulled,       //!< number of not culled OpenGl_Element
    Counter_NbElemsFillNotCulled,   //!< number of not culled OpenGl_PrimitiveArray drawing triangles
    Counter_NbElemsLineNotCulled,   //!< number of not culled OpenGl_PrimitiveArray drawing lines
    Counter_NbElemsPointNotCulled,  //!< number of not culled OpenGl_PrimitiveArray drawing points
    Counter_NbElemsTextNotCulled,   //!< number of not culled OpenGl_Text
    Counter_NbTrianglesNotCulled,   //!< number of not culled (as structure) triangles
    Counter_NbPointsNotCulled,      //!< number of not culled (as structure) points
    Counter_EstimatedBytesGeom,     //!< estimated GPU memory used for geometry
    Counter_EstimatedBytesFbos,     //!< estimated GPU memory used for FBOs
    Counter_EstimatedBytesTextures, //!< estimated GPU memory used for textures
  };
  enum { Counter_NB = Counter_EstimatedBytesTextures + 1 };

public:

  //! Default constructor.
  Standard_EXPORT OpenGl_FrameStats();

  //! Destructor.
  Standard_EXPORT virtual ~OpenGl_FrameStats();

  //! Returns interval in seconds for updating meters across several frames; 1 second by default.
  Standard_Real UpdateInterval() const { return myUpdateInterval; }

  //! Sets interval in seconds for updating values.
  void SetUpdateInterval (Standard_Real theInterval) { myUpdateInterval = theInterval; }

  //! Prefer longer lines over more greater of lines.
  Standard_Boolean IsLongLineFormat() const { return myIsLongLineFormat; }

  //! Set if format should prefer longer lines over greater number of lines.
  void SetLongLineFormat (Standard_Boolean theValue) { myIsLongLineFormat = theValue; }

  //! Frame redraw started.
  Standard_EXPORT virtual void FrameStart (const Handle(OpenGl_Workspace)& theWorkspace = Handle(OpenGl_Workspace)());

  //! Frame redraw finished.
  Standard_EXPORT virtual void FrameEnd (const Handle(OpenGl_Workspace)& theWorkspace = Handle(OpenGl_Workspace)());

public:

  //! Returns formatted string.
  Standard_EXPORT virtual TCollection_AsciiString FormatStats (Graphic3d_RenderingParams::PerfCounters theFlags) const;

  //! Returns duration of the last frame in seconds.
  Standard_Real FrameDuration() const { return myFrameDuration; }

  //! Returns FPS (frames per seconds, elapsed time).
  //! This number indicates an actual frame rate averaged for several frames within UpdateInterval() duration,
  //! basing on a real elapsed time between updates.
  Standard_Real FrameRate() const { return myFps; }

  //! Returns CPU FPS (frames per seconds, CPU time).
  //! This number indicates a PREDICTED frame rate,
  //! basing on CPU elapsed time between updates and NOT real elapsed time (which might include periods of CPU inactivity).
  //! Number is expected to be greater then actual frame rate returned by FrameRate().
  //! Values significantly greater actual frame rate indicate that rendering is limited by GPU performance (CPU is stalled in-between),
  //! while values around actual frame rate indicate rendering being limited by CPU performance (GPU is stalled in-between).
  Standard_Real FrameRateCpu() const { return myFpsCpu; }

  //! Returns value of specified counter, cached between stats updates.
  //! Should NOT be called between ::FrameStart() and ::FrameEnd() calls.
  Standard_Size CounterValue (OpenGl_FrameStats::Counter theCounter) const { return myCounters[theCounter]; }

  //! Returns TRUE if some Layers have been culled.
  Standard_Boolean HasCulledLayers() const { return myCounters[Counter_NbLayersNotCulled] != myCounters[Counter_NbLayers]; }

  //! Returns TRUE if some structures have been culled.
  Standard_Boolean HasCulledStructs() const { return myCounters[Counter_NbStructsNotCulled] != myCounters[Counter_NbStructs]; }

public:

  //! Returns TRUE if this stats are equal to another.
  virtual Standard_Boolean IsEqual (const Handle(OpenGl_FrameStats)& theOther) const
  {
    // check just a couple of major counters
    return Abs (myFps    - theOther->myFps)    <= 0.001
        && Abs (myFpsCpu - theOther->myFpsCpu) <= 0.001
        && myCounters[Counter_NbLayers] == theOther->myCounters[Counter_NbLayers]
        && myCounters[Counter_NbLayersNotCulled] == theOther->myCounters[Counter_NbLayersNotCulled]
        && myCounters[Counter_NbStructs] == theOther->myCounters[Counter_NbStructs]
        && myCounters[Counter_NbStructsNotCulled] == theOther->myCounters[Counter_NbStructsNotCulled];
  }

  //! Copy stats values from another instance
  virtual void CopyFrom (const Handle(OpenGl_FrameStats)& theOther)
  {
    myFps    = theOther->myFps;
    myFpsCpu = theOther->myFpsCpu;
    memcpy (myCounters, theOther->myCounters, sizeof(myCounters));
  }

  //! Returns value of specified counter for modification, should be called between ::FrameStart() and ::FrameEnd() calls.
  Standard_Size& ChangeCounter (OpenGl_FrameStats::Counter theCounter) { return myCountersTmp[theCounter]; }

protected:

  //! Updates counters for structures.
  Standard_EXPORT virtual void updateStructures (Standard_Integer theViewId,
                                                 const OpenGl_IndexedMapOfStructure& theStructures,
                                                 Standard_Boolean theToCountElems,
                                                 Standard_Boolean theToCountTris,
                                                 Standard_Boolean theToCountMem);

protected:

  OSD_Timer        myFpsTimer;                //!< timer for FPS measurements
  Standard_Real    myFrameStartTime;          //!< time at the beginning of frame redraw
  Standard_Real    myFrameDuration;           //!< frame duration
  Standard_Real    myFps;                     //!< FPS     meter (frames per seconds, elapsed time)
  Standard_Real    myFpsCpu;                  //!< CPU FPS meter (frames per seconds, CPU time)
  Standard_Real    myUpdateInterval;          //!< interval to update meters
  Standard_Size    myFpsFrameCount;           //!< FPS counter (within short measurement time slice)
  Standard_Size    myCounters   [Counter_NB]; //!< counter values cached between updates
  Standard_Size    myCountersTmp[Counter_NB]; //!< counter values filled during
  Standard_Boolean myIsLongLineFormat;        //!< prefer longer lines over greater number of lines

};

DEFINE_STANDARD_HANDLE(OpenGl_FrameStats, Standard_Transient)

#endif // _OpenGl_FrameStats_HeaderFile
