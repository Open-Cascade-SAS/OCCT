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

#include <OpenGl_FrameStats.hxx>

#include <OpenGl_GlCore20.hxx>
#include <OpenGl_View.hxx>
#include <OpenGl_Workspace.hxx>

IMPLEMENT_STANDARD_RTTIEXT(OpenGl_FrameStats, Standard_Transient)

namespace
{
  //! Format counter.
  static std::ostream& formatCounter (std::ostream& theStream,
                                      Standard_Integer theWidth,
                                      const char* thePrefix,
                                      Standard_Size theValue,
                                      const char* thePostfix = NULL)
  {
    if (thePrefix != NULL)
    {
      theStream << thePrefix;
    }
    theStream << std::setfill(' ') << std::setw (theWidth);
    if (theValue >= 1000000000)
    {
      Standard_Real aValM = Standard_Real(theValue) / 1000000000.0;
      theStream << std::fixed << std::setprecision (1) << aValM << "G";
    }
    else if (theValue >= 1000000)
    {
      Standard_Real aValM = Standard_Real(theValue) / 1000000.0;
      theStream << std::fixed << std::setprecision (1) << aValM << "M";
    }
    else if (theValue >= 1000)
    {
      Standard_Real aValK = Standard_Real(theValue) / 1000.0;
      theStream << std::fixed << std::setprecision (1) << aValK << "k";
    }
    else
    {
      theStream << theValue;
    }
    if (thePostfix != NULL)
    {
      theStream << thePostfix;
    }
    return theStream;
  }

  //! Format memory counter.
  static std::ostream& formatBytes (std::ostream& theStream,
                                    Standard_Integer theWidth,
                                    const char* thePrefix,
                                    Standard_Size theValue,
                                    const char* thePostfix = NULL)
  {
    if (thePrefix != NULL)
    {
      theStream << thePrefix;
    }
    theStream << std::setfill(' ') << std::setw (theWidth);
    if (theValue >= 1024 * 1024 * 1024)
    {
      Standard_Real aValM = Standard_Real(theValue) / (1024.0 * 1024.0 * 1024.0);
      theStream << std::fixed << std::setprecision (1) << aValM << " GiB";
    }
    else if (theValue >= 1024 * 1024)
    {
      Standard_Real aValM = Standard_Real(theValue) / (1024.0 * 1024.0);
      theStream << std::fixed << std::setprecision (1) << aValM << " MiB";
    }
    else if (theValue >= 1024)
    {
      Standard_Real aValK = Standard_Real(theValue) / 1024.0;
      theStream << std::fixed << std::setprecision (1) << aValK << " KiB";
    }
    else
    {
      theStream << theValue;
    }
    if (thePostfix != NULL)
    {
      theStream << thePostfix;
    }
    return theStream;
  }

  //! Return estimated data size.
  static Standard_Size estimatedDataSize (const Handle(OpenGl_Resource)& theRes)
  {
    return !theRes.IsNull() ? theRes->EstimatedDataSize() : 0;
  }
}

// =======================================================================
// function : OpenGl_FrameStats
// purpose  :
// =======================================================================
OpenGl_FrameStats::OpenGl_FrameStats()
: myFpsTimer (Standard_True),
  myFrameStartTime (0.0),
  myFrameDuration  (0.0),
  myFps    (-1.0),
  myFpsCpu (-1.0),
  myUpdateInterval (1.0),
  myFpsFrameCount (0),
  myIsLongLineFormat (Standard_False)
{
  memset (myCounters,    0, sizeof(myCounters));
  memset (myCountersTmp, 0, sizeof(myCountersTmp));
}

// =======================================================================
// function : ~OpenGl_FrameStats
// purpose  :
// =======================================================================
OpenGl_FrameStats::~OpenGl_FrameStats()
{
  //
}

// =======================================================================
// function : FormatStats
// purpose  :
// =======================================================================
TCollection_AsciiString OpenGl_FrameStats::FormatStats (Graphic3d_RenderingParams::PerfCounters theFlags) const
{
  const Standard_Integer aValWidth = 5;
  std::stringstream aBuf;
  const Standard_Boolean isCompact = theFlags == Graphic3d_RenderingParams::PerfCounters_FrameRate; // only FPS is displayed
  if (myIsLongLineFormat
   && (theFlags & Graphic3d_RenderingParams::PerfCounters_FrameRate) != 0
   && (theFlags & Graphic3d_RenderingParams::PerfCounters_CPU) != 0)
  {
    aBuf << "FPS: "     << std::setfill(' ') << std::setw (isCompact ? aValWidth : 9)  << std::fixed << std::setprecision (1) << myFps
         << " [CPU: "   << std::setfill(' ') << std::setw (isCompact ? aValWidth : 10) << std::fixed << std::setprecision (1) << myFpsCpu << "]\n";
  }
  else
  {
    if ((theFlags & Graphic3d_RenderingParams::PerfCounters_FrameRate) != 0)
    {
      aBuf << "FPS:     " << std::setfill(' ') << std::setw (isCompact ? aValWidth : aValWidth + 3) << std::fixed << std::setprecision (1) << myFps  << "\n";
    }
    if ((theFlags & Graphic3d_RenderingParams::PerfCounters_CPU) != 0)
    {
      aBuf << "CPU FPS: " << std::setfill(' ') << std::setw (isCompact ? aValWidth : aValWidth + 3) << std::fixed << std::setprecision (1) << myFpsCpu << "\n";
    }
  }
  if ((theFlags & Graphic3d_RenderingParams::PerfCounters_Layers) != 0)
  {
    if (myIsLongLineFormat)
    {
      formatCounter (aBuf, aValWidth, "Layers:  ", myCounters[Counter_NbLayers]);
      if (HasCulledLayers())
      {
        formatCounter (aBuf, aValWidth, " [rendered: ", myCounters[Counter_NbLayersNotCulled], "]");
      }
      aBuf << "\n";
    }
    else
    {
      formatCounter (aBuf, aValWidth + 3, "Layers:  ", myCounters[Counter_NbLayers], "\n");
    }
  }
  if ((theFlags & Graphic3d_RenderingParams::PerfCounters_Structures) != 0)
  {
    if (myIsLongLineFormat)
    {
      formatCounter (aBuf, aValWidth, "Structs: ", myCounters[Counter_NbStructs]);
      if (HasCulledStructs())
      {
        formatCounter (aBuf, aValWidth, " [rendered: ", myCounters[Counter_NbStructsNotCulled], "]");
      }
      aBuf << "\n";
    }
    else
    {
      formatCounter (aBuf, aValWidth + 3, "Structs: ", myCounters[Counter_NbStructs], "\n");
    }
  }
  if ((theFlags & Graphic3d_RenderingParams::PerfCounters_Groups) != 0
   || (theFlags & Graphic3d_RenderingParams::PerfCounters_GroupArrays) != 0
   || (theFlags & Graphic3d_RenderingParams::PerfCounters_Triangles) != 0
   || (theFlags & Graphic3d_RenderingParams::PerfCounters_Points) != 0
   || (!myIsLongLineFormat
    && ((theFlags & Graphic3d_RenderingParams::PerfCounters_Structures) != 0
     || (theFlags & Graphic3d_RenderingParams::PerfCounters_Layers) != 0)))
  {
    aBuf << "Rendered\n";
  }
  if (!myIsLongLineFormat
   && (theFlags & Graphic3d_RenderingParams::PerfCounters_Layers) != 0)
  {
    formatCounter (aBuf, aValWidth, "    Layers: ", myCounters[Counter_NbLayersNotCulled], "\n");
  }
  if (!myIsLongLineFormat
   && (theFlags & Graphic3d_RenderingParams::PerfCounters_Structures) != 0)
  {
    formatCounter (aBuf, aValWidth, "   Structs: ", myCounters[Counter_NbStructsNotCulled], "\n");
  }
  if ((theFlags & Graphic3d_RenderingParams::PerfCounters_Groups) != 0)
  {
    formatCounter (aBuf, aValWidth, "    Groups: ", myCounters[Counter_NbGroupsNotCulled], "\n");
  }
  if ((theFlags & Graphic3d_RenderingParams::PerfCounters_GroupArrays) != 0)
  {
    formatCounter (aBuf, aValWidth, "    Arrays: ", myCounters[Counter_NbElemsNotCulled], "\n");
    formatCounter (aBuf, aValWidth, "    [fill]: ", myCounters[Counter_NbElemsFillNotCulled], "\n");
    formatCounter (aBuf, aValWidth, "    [line]: ", myCounters[Counter_NbElemsLineNotCulled], "\n");
    formatCounter (aBuf, aValWidth, "   [point]: ", myCounters[Counter_NbElemsPointNotCulled], "\n");
    formatCounter (aBuf, aValWidth, "    [text]: ", myCounters[Counter_NbElemsTextNotCulled], "\n");
  }
  if ((theFlags & Graphic3d_RenderingParams::PerfCounters_Triangles) != 0)
  {
    formatCounter (aBuf, aValWidth, " Triangles: ", myCounters[Counter_NbTrianglesNotCulled], "\n");
  }
  if ((theFlags & Graphic3d_RenderingParams::PerfCounters_Points) != 0)
  {
    formatCounter (aBuf, aValWidth, "    Points: ", myCounters[Counter_NbPointsNotCulled], "\n");
  }
  if ((theFlags & Graphic3d_RenderingParams::PerfCounters_EstimMem) != 0)
  {
    aBuf << "GPU Memory\n";
    formatBytes (aBuf, aValWidth, "  Geometry: ", myCounters[Counter_EstimatedBytesGeom], "\n");
    formatBytes (aBuf, aValWidth, "  Textures: ", myCounters[Counter_EstimatedBytesTextures], "\n");
    formatBytes (aBuf, aValWidth, "    Frames: ", myCounters[Counter_EstimatedBytesFbos], "\n");
  }
  return TCollection_AsciiString (aBuf.str().c_str());
}

// =======================================================================
// function : FrameStart
// purpose  :
// =======================================================================
void OpenGl_FrameStats::FrameStart (const Handle(OpenGl_Workspace)& )
{
  memset (myCountersTmp, 0, sizeof(myCountersTmp));
  myFrameStartTime = myFpsTimer.ElapsedTime();
  if (!myFpsTimer.IsStarted())
  {
    myFpsTimer.Reset();
    myFpsTimer.Start();
    myFpsFrameCount = 0;
  }
}

// =======================================================================
// function : FrameEnd
// purpose  :
// =======================================================================
void OpenGl_FrameStats::FrameEnd (const Handle(OpenGl_Workspace)& theWorkspace)
{
  const Graphic3d_RenderingParams::PerfCounters aBits = !theWorkspace.IsNull()
                                                      ? theWorkspace->View()->RenderingParams().CollectedStats
                                                      : Graphic3d_RenderingParams::PerfCounters_NONE;
  const double aTime = myFpsTimer.ElapsedTime();
  myFrameDuration = aTime - myFrameStartTime;
  ++myFpsFrameCount;
  if (!theWorkspace.IsNull())
  {
    myUpdateInterval = theWorkspace->View()->RenderingParams().StatsUpdateInterval;
  }

  if (aTime < myUpdateInterval)
  {
    return;
  }

  if (aTime > gp::Resolution())
  {
    // update FPS
    myFpsTimer.Stop();
    const double aCpuSec = myFpsTimer.UserTimeCPU();
    myFps    = double(myFpsFrameCount) / aTime;
    myFpsCpu = aCpuSec > gp::Resolution()
             ? double(myFpsFrameCount) / aCpuSec
             : -1.0;
    myFpsTimer.Reset();
    myFpsTimer.Start();
    myFpsFrameCount = 0;
  }

  // update structure counters
  if (theWorkspace.IsNull())
  {
    memcpy (myCounters, myCountersTmp, sizeof(myCounters));
    return;
  }

  const Standard_Boolean toCountMem     = (aBits & Graphic3d_RenderingParams::PerfCounters_EstimMem)  != 0;
  const Standard_Boolean toCountTris    = (aBits & Graphic3d_RenderingParams::PerfCounters_Triangles) != 0
                                       || (aBits & Graphic3d_RenderingParams::PerfCounters_Points)    != 0;
  const Standard_Boolean toCountElems   = (aBits & Graphic3d_RenderingParams::PerfCounters_GroupArrays) != 0 || toCountTris || toCountMem;
  const Standard_Boolean toCountGroups  = (aBits & Graphic3d_RenderingParams::PerfCounters_Groups)      != 0 || toCountElems;
  const Standard_Boolean toCountStructs = (aBits & Graphic3d_RenderingParams::PerfCounters_Structures)  != 0
                                       || (aBits & Graphic3d_RenderingParams::PerfCounters_Layers)      != 0 || toCountGroups;

  myCountersTmp[Counter_NbLayers] = theWorkspace->View()->LayerList().Layers().Size();
  if (toCountStructs
   || (aBits & Graphic3d_RenderingParams::PerfCounters_Layers)    != 0)
  {
    const Standard_Integer aViewId = theWorkspace->View()->Identification();
    for (OpenGl_SequenceOfLayers::Iterator aLayerIter (theWorkspace->View()->LayerList().Layers()); aLayerIter.More(); aLayerIter.Next())
    {
      const Handle(OpenGl_Layer)& aLayer = aLayerIter.Value();
      if (!aLayer->IsCulled())
      {
        ++myCountersTmp[Counter_NbLayersNotCulled];
      }
      myCountersTmp[Counter_NbStructs]          += aLayer->NbStructures();
      myCountersTmp[Counter_NbStructsNotCulled] += aLayer->NbStructuresNotCulled();
      if (toCountGroups)
      {
        updateStructures (aViewId, aLayer->CullableStructuresBVH().Structures(), toCountElems, toCountTris, toCountMem);
        updateStructures (aViewId, aLayer->CullableTrsfPersStructuresBVH().Structures(), toCountElems, toCountTris, toCountMem);
        updateStructures (aViewId, aLayer->NonCullableStructures(), toCountElems, toCountTris, toCountMem);
      }
    }
  }
  if (toCountMem
  && !theWorkspace.IsNull())
  {
    for (OpenGl_Context::OpenGl_ResourcesMap::Iterator aResIter (theWorkspace->GetGlContext()->SharedResources());
         aResIter.More(); aResIter.Next())
    {
      myCountersTmp[Counter_EstimatedBytesTextures] += aResIter.Value()->EstimatedDataSize();
    }

    const OpenGl_View* aView = theWorkspace->View();
    {
      Standard_Size& aMemFbos = myCountersTmp[Counter_EstimatedBytesFbos];
      // main FBOs
      aMemFbos += estimatedDataSize (aView->myMainSceneFbos[0]);
      aMemFbos += estimatedDataSize (aView->myMainSceneFbos[1]);
      aMemFbos += estimatedDataSize (aView->myImmediateSceneFbos[0]);
      aMemFbos += estimatedDataSize (aView->myImmediateSceneFbos[1]);
      // OIT FBOs
      aMemFbos += estimatedDataSize (aView->myMainSceneFbosOit[0]);
      aMemFbos += estimatedDataSize (aView->myMainSceneFbosOit[1]);
      aMemFbos += estimatedDataSize (aView->myImmediateSceneFbosOit[0]);
      aMemFbos += estimatedDataSize (aView->myImmediateSceneFbosOit[1]);
      // dump FBO
      aMemFbos += estimatedDataSize (aView->myFBO);
      // RayTracing FBO
      aMemFbos += estimatedDataSize (aView->myOpenGlFBO);
      aMemFbos += estimatedDataSize (aView->myOpenGlFBO2);
      aMemFbos += estimatedDataSize (aView->myRaytraceFBO1[0]);
      aMemFbos += estimatedDataSize (aView->myRaytraceFBO1[1]);
      aMemFbos += estimatedDataSize (aView->myRaytraceFBO2[0]);
      aMemFbos += estimatedDataSize (aView->myRaytraceFBO2[1]);
      // also RayTracing
      aMemFbos += estimatedDataSize (aView->myRaytraceOutputTexture[0]);
      aMemFbos += estimatedDataSize (aView->myRaytraceOutputTexture[1]);
      aMemFbos += estimatedDataSize (aView->myRaytraceVisualErrorTexture[0]);
      aMemFbos += estimatedDataSize (aView->myRaytraceVisualErrorTexture[1]);
      aMemFbos += estimatedDataSize (aView->myRaytraceTileOffsetsTexture[0]);
      aMemFbos += estimatedDataSize (aView->myRaytraceTileOffsetsTexture[1]);
    }
    {
      // Ray Tracing geometry
      Standard_Size& aMemGeom = myCountersTmp[Counter_EstimatedBytesGeom];
      aMemGeom += estimatedDataSize (aView->mySceneNodeInfoTexture);
      aMemGeom += estimatedDataSize (aView->mySceneMinPointTexture);
      aMemGeom += estimatedDataSize (aView->mySceneMaxPointTexture);
      aMemGeom += estimatedDataSize (aView->mySceneTransformTexture);
      aMemGeom += estimatedDataSize (aView->myGeometryVertexTexture);
      aMemGeom += estimatedDataSize (aView->myGeometryNormalTexture);
      aMemGeom += estimatedDataSize (aView->myGeometryTexCrdTexture);
      aMemGeom += estimatedDataSize (aView->myGeometryTriangTexture);
      aMemGeom += estimatedDataSize (aView->myRaytraceMaterialTexture);
      aMemGeom += estimatedDataSize (aView->myRaytraceLightSrcTexture);
    }
  }
  memcpy (myCounters, myCountersTmp, sizeof(myCounters));
}

// =======================================================================
// function : updateStructures
// purpose  :
// =======================================================================
void OpenGl_FrameStats::updateStructures (Standard_Integer theViewId,
                                          const OpenGl_IndexedMapOfStructure& theStructures,
                                          Standard_Boolean theToCountElems,
                                          Standard_Boolean theToCountTris,
                                          Standard_Boolean theToCountMem)
{
  for (OpenGl_IndexedMapOfStructure::Iterator aStructIter (theStructures); aStructIter.More(); aStructIter.Next())
  {
    const OpenGl_Structure* aStruct = aStructIter.Value();
    if (aStruct->IsCulled()
    || !aStruct->IsVisible (theViewId))
    {
      if (theToCountMem)
      {
        for (OpenGl_Structure::GroupIterator aGroupIter (aStruct->Groups()); aGroupIter.More(); aGroupIter.Next())
        {
          const OpenGl_Group* aGroup = aGroupIter.Value();
          for (const OpenGl_ElementNode* aNodeIter = aGroup->FirstNode(); aNodeIter != NULL; aNodeIter = aNodeIter->next)
          {
            if (const OpenGl_PrimitiveArray* aPrim = dynamic_cast<const OpenGl_PrimitiveArray*> (aNodeIter->elem))
            {
              myCountersTmp[Counter_EstimatedBytesGeom] += estimatedDataSize (aPrim->AttributesVbo());
              myCountersTmp[Counter_EstimatedBytesGeom] += estimatedDataSize (aPrim->IndexVbo());
            }
          }
        }
      }
      continue;
    }

    myCountersTmp[Counter_NbGroupsNotCulled] += aStruct->Groups().Size();
    if (!theToCountElems)
    {
      continue;
    }

    for (OpenGl_Structure::GroupIterator aGroupIter (aStruct->Groups()); aGroupIter.More(); aGroupIter.Next())
    {
      const OpenGl_Group* aGroup = aGroupIter.Value();
      for (const OpenGl_ElementNode* aNodeIter = aGroup->FirstNode(); aNodeIter != NULL; aNodeIter = aNodeIter->next)
      {
        if (const OpenGl_PrimitiveArray* aPrim = dynamic_cast<const OpenGl_PrimitiveArray*> (aNodeIter->elem))
        {
          ++myCountersTmp[Counter_NbElemsNotCulled];
          if (theToCountMem)
          {
            myCountersTmp[Counter_EstimatedBytesGeom] += estimatedDataSize (aPrim->AttributesVbo());
            myCountersTmp[Counter_EstimatedBytesGeom] += estimatedDataSize (aPrim->IndexVbo());
          }

          if (aPrim->IsFillDrawMode())
          {
            ++myCountersTmp[Counter_NbElemsFillNotCulled];
            if (!theToCountTris)
            {
              continue;
            }

            const Handle(OpenGl_VertexBuffer)& anAttribs = aPrim->AttributesVbo();
            if (anAttribs.IsNull()
            || !anAttribs->IsValid())
            {
              continue;
            }

            const Handle(OpenGl_VertexBuffer)& anIndices = aPrim->IndexVbo();
            const Standard_Integer aNbIndices = !anIndices.IsNull() ? anIndices->GetElemsNb() : anAttribs->GetElemsNb();
            const Standard_Integer aNbBounds  = !aPrim->Bounds().IsNull() ? aPrim->Bounds()->NbBounds : 1;
            switch (aPrim->DrawMode())
            {
              case GL_TRIANGLES:
              {
                myCountersTmp[Counter_NbTrianglesNotCulled] += aNbIndices / 3;
                break;
              }
              case GL_TRIANGLE_STRIP:
              case GL_TRIANGLE_FAN:
              {
                myCountersTmp[Counter_NbTrianglesNotCulled] += aNbIndices - 2 * aNbBounds;
                break;
              }
              case GL_TRIANGLES_ADJACENCY:
              {
                myCountersTmp[Counter_NbTrianglesNotCulled] += aNbIndices / 6;
                break;
              }
              case GL_TRIANGLE_STRIP_ADJACENCY:
              {
                myCountersTmp[Counter_NbTrianglesNotCulled] += aNbIndices - 4 * aNbBounds;
                break;
              }
            #if !defined(GL_ES_VERSION_2_0)
              case GL_QUADS:
              {
                myCountersTmp[Counter_NbTrianglesNotCulled] += aNbIndices / 2;
                break;
              }
              case GL_QUAD_STRIP:
              {
                myCountersTmp[Counter_NbTrianglesNotCulled] += (aNbIndices / 2 - aNbBounds) * 2;
                break;
              }
            #endif
            }
          }
          else if (aPrim->DrawMode() == GL_POINTS)
          {
            ++myCountersTmp[Counter_NbElemsPointNotCulled];
            if (theToCountTris)
            {
              const Handle(OpenGl_VertexBuffer)& anAttribs = aPrim->AttributesVbo();
              if (!anAttribs.IsNull()
                && anAttribs->IsValid())
              {
                const Handle(OpenGl_VertexBuffer)& anIndices = aPrim->IndexVbo();
                const Standard_Integer aNbIndices = !anIndices.IsNull() ? anIndices->GetElemsNb() : anAttribs->GetElemsNb();
                myCountersTmp[Counter_NbPointsNotCulled] += aNbIndices;
              }
            }
          }
          else
          {
            ++myCountersTmp[Counter_NbElemsLineNotCulled];
          }
        }
        else if (const OpenGl_Text* aText = dynamic_cast<const OpenGl_Text*> (aNodeIter->elem))
        {
          (void )aText;
          ++myCountersTmp[Counter_NbElemsNotCulled];
          ++myCountersTmp[Counter_NbElemsTextNotCulled];
        }
      }
    }
  }
}
