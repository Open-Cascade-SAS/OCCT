// Created on: 2012-02-02
// Created by: Anton POLETAEV
// Copyright (c) 2012-2014 OPEN CASCADE SAS
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

#ifndef _OpenGl_LayerList_Header
#define _OpenGl_LayerList_Header

#include <OpenGl_Layer.hxx>
#include <OpenGl_LayerFilter.hxx>

#include <NCollection_Array1.hxx>
#include <NCollection_Handle.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_DataMap.hxx>

class OpenGl_Structure;

typedef NCollection_Sequence<Handle(OpenGl_Layer)> OpenGl_SequenceOfLayers;
typedef NCollection_DataMap<int, int> OpenGl_LayerSeqIds;

//! Class defining the list of layers.
class OpenGl_LayerList
{
public:

  //! Constructor
  OpenGl_LayerList (const Standard_Integer theNbPriorities);

  //! Destructor
  virtual ~OpenGl_LayerList();

  //! Method returns the number of available priorities
  Standard_Integer NbPriorities() const { return myNbPriorities; }

  //! Number of displayed structures
  Standard_Integer NbStructures() const { return myNbStructures; }

  //! Return number of structures within immediate layers
  Standard_Integer NbImmediateStructures() const { return myImmediateNbStructures; }

  //! Insert a new layer with id.
  void AddLayer (const Graphic3d_ZLayerId theLayerId);

  //! Remove layer by its id.
  void RemoveLayer (const Graphic3d_ZLayerId theLayerId);

  //! Add structure to list with given priority. The structure will be inserted
  //! to specified layer. If the layer isn't found, the structure will be put
  //! to default bottom-level layer.
  void AddStructure (const OpenGl_Structure*  theStruct,
                     const Graphic3d_ZLayerId theLayerId,
                     const Standard_Integer   thePriority,
                     Standard_Boolean        isForChangePriority = Standard_False);

  //! Remove structure from structure list and return its previous priority
  void RemoveStructure (const OpenGl_Structure* theStructure);

  //! Change structure z layer
  //! If the new layer is not presented, the structure will be displayed
  //! in default z layer
  void ChangeLayer (const OpenGl_Structure*  theStructure,
                    const Graphic3d_ZLayerId theOldLayerId,
                    const Graphic3d_ZLayerId theNewLayerId);

  //! Changes structure priority within its ZLayer
  void ChangePriority (const OpenGl_Structure*  theStructure,
                       const Graphic3d_ZLayerId theLayerId,
                       const Standard_Integer   theNewPriority);

  //! Returns reference to the layer with given ID.
  OpenGl_Layer& Layer (const Graphic3d_ZLayerId theLayerId);

  //! Returns reference to the layer with given ID.
  const OpenGl_Layer& Layer (const Graphic3d_ZLayerId theLayerId) const;

  //! Assign new settings to the layer.
  void SetLayerSettings (const Graphic3d_ZLayerId        theLayerId,
                         const Graphic3d_ZLayerSettings& theSettings);

  //! Render this element
  void Render (const Handle(OpenGl_Workspace)& theWorkspace,
               const Standard_Boolean          theToDrawImmediate,
               const OpenGl_LayerFilter        theLayersToProcess,
               OpenGl_FrameBuffer*             theReadDrawFbo,
               OpenGl_FrameBuffer*             theOitAccumFbo) const;

  //! Returns the set of OpenGL Z-layers.
  const OpenGl_SequenceOfLayers& Layers() const { return myLayers; }

  //! Returns the map of Z-layer IDs to indexes.
  const OpenGl_LayerSeqIds& LayerIDs() const { return myLayerIds; }

  //! Marks BVH tree for given priority list as dirty and
  //! marks primitive set for rebuild.
  void InvalidateBVHData (const Graphic3d_ZLayerId theLayerId);

  //! Returns structure modification state (for ray-tracing).
  Standard_Size ModificationStateOfRaytracable() const { return myModifStateOfRaytraceable; }

  //! Returns BVH tree builder for frustom culling.
  const Handle(Select3D_BVHBuilder3d)& FrustumCullingBVHBuilder() const { return myBVHBuilder; }

  //! Assigns BVH tree builder for frustom culling.
  void SetFrustumCullingBVHBuilder (const Handle(Select3D_BVHBuilder3d)& theBuilder);

protected:

  //! Filter of TKOpenGl elements for processing only shading geometry and
  //! for collecting number of skipped elements to an external counter.
  class OpenGl_OpaqueFilter : public OpenGl_RenderFilter
  {
  public:

    //! Constructor.
    //! @param thePrevFilter [in] the previously active filter that should have additive effect.
    OpenGl_OpaqueFilter() : mySkippedCounter (0) {}

    //! Sets the current active filter in workspace.
    //! @param thePrevFilter [in] the previously active filter that should have additive effect.
    void SetPreviousFilter (const Handle(OpenGl_RenderFilter)& thePrevFitler) { myFilter = thePrevFitler; }

    //! Sets the value of the skipped elements counter.
    void SetSkippedCounter (const Standard_Size theCounter) { mySkippedCounter = theCounter; }

    //! Returns number of skipped elements.
    Standard_Size NbSkipped() const { return mySkippedCounter; }

    //! Checks whether the element should be rendered or skipped.
    //! @param theWorkspace [in] the currently used workspace for rendering.
    //! @param theGlElement [in] the TKOpenGl rendering queue element that should be checked before streaming to GPU.
    Standard_EXPORT virtual Standard_Boolean ShouldRender (const Handle(OpenGl_Workspace)& theWorkspace,
                                                           const OpenGl_Element*           theGlElement) Standard_OVERRIDE;

    DEFINE_STANDARD_RTTI_INLINE (OpenGl_OpaqueFilter, OpenGl_RenderFilter)

  private:

    Standard_Size mySkippedCounter;   //!< Counter of skipped elements.
    Handle(OpenGl_RenderFilter) myFilter; //!< Previous active filter that should be combined.
  };

  //! Filter of TKOpenGl elements for keeping only shading geometry with transparency.
  class OpenGl_TransparentFilter : public OpenGl_RenderFilter
  {
  public:

    //! Constructor.
    OpenGl_TransparentFilter() {}

    //! Sets the current active filter in workspace.
    //! @param thePrevFilter [in] the previously active filter that should have additive effect.
    void SetPreviousFilter (const Handle(OpenGl_RenderFilter)& thePrevFitler) { myFilter = thePrevFitler; }

    //! Checks whether the element should be rendered or skipped.
    //! @param theWorkspace [in] the currently used workspace for rendering.
    //! @param theGlElement [in] the TKOpenGl rendering queue element that should be checked before streaming to GPU.
    Standard_EXPORT virtual Standard_Boolean ShouldRender (const Handle(OpenGl_Workspace)& theWorkspace,
                                                           const OpenGl_Element*           theGlElement) Standard_OVERRIDE;

    DEFINE_STANDARD_RTTI_INLINE (OpenGl_TransparentFilter, OpenGl_RenderFilter)

  private:

    Handle(OpenGl_RenderFilter) myFilter; //!< Previous active filter that should be combined.
  };

  //! Stack of references to existing layers of predefined maximum size.
  class OpenGl_LayerStack
  {
  public:
    typedef NCollection_Array1<const OpenGl_Layer*>::iterator iterator;

    //! Reallocate internal buffer of the stack.
    void Allocate (Standard_Integer theSize)
    {
      if (theSize > 0)
      {
        myStackSpace = new NCollection_Array1<const OpenGl_Layer*> (1, theSize);
        myStackSpace->Init (NULL);
        myBackPtr    = myStackSpace->begin();
      }
      else
      {
        myStackSpace.Nullify();
        myBackPtr = iterator();
      }
    }

    //! Clear stack.
    void Clear()
    {
      if (!myStackSpace.IsNull())
      {
        myStackSpace->Init (NULL);
        myBackPtr = myStackSpace->begin();
      }
    }

    //! Push a new layer reference to the stack.
    void Push (const OpenGl_Layer* theLayer) { (*myBackPtr++) = theLayer; }

    //! Returns iterator to the origin of the stack.
    iterator Origin() const { return myStackSpace.IsNull() ? iterator() : myStackSpace->begin(); }

    //! Returns iterator to the back of the stack (after last item added).
    iterator Back() const { return myBackPtr; }

    //! Returns true if nothing has been pushed into the stack.
    Standard_Boolean IsEmpty() const { return Back() == Origin(); }

  private:

    NCollection_Handle<NCollection_Array1<const OpenGl_Layer*> > myStackSpace;
    iterator                                                     myBackPtr;
  };

  //! Render transparent objects using blending operator.
  //! Additional accumulation framebuffer is used for blended order-independent
  //! transparency algorithm. It should support floating-point color components
  //! and share depth with main reading/drawing framebuffer.
  //! @param theWorkspace [in] the currently used workspace for rendering.
  //! @param theLayerIter [in/out] the current iterator of transparent layers to process.
  //! @param theGlobalSettings [in] the set of global settings used for rendering.
  //! @param theReadDrawFbo [in] the framebuffer for reading depth and writing final color.
  //! @param theOitAccumFbo [in] the framebuffer for accumulating color and coverage for OIT process.
  void renderTransparent (const Handle(OpenGl_Workspace)&   theWorkspace,
                          OpenGl_LayerStack::iterator&      theLayerIter,
                          const OpenGl_GlobalLayerSettings& theGlobalSettings,
                          OpenGl_FrameBuffer*               theReadDrawFbo,
                          OpenGl_FrameBuffer*               theOitAccumFbo) const;

protected:

  // number of structures temporary put to default layer
  OpenGl_SequenceOfLayers myLayers;
  OpenGl_LayerSeqIds      myLayerIds;
  Handle(Select3D_BVHBuilder3d) myBVHBuilder;      //!< BVH tree builder for frustom culling
  Standard_Integer        myDefaultLayerIndex;     //!< index of Graphic3d_ZLayerId_Default layer in myLayers sequence

  Standard_Integer        myNbPriorities;
  Standard_Integer        myNbStructures;
  Standard_Integer        myImmediateNbStructures; //!< number of structures within immediate layers

  mutable Standard_Size   myModifStateOfRaytraceable;

  //! Collection of references to layers with transparency gathered during rendering pass.
  mutable OpenGl_LayerStack myTransparentToProcess;

  Handle(OpenGl_OpaqueFilter)      myRenderOpaqueFilter; //!< rendering filter for opaque drawing pass (blended OIT).
  Handle(OpenGl_TransparentFilter) myRenderTranspFilter; //!< rendering filter for transparency drawing pass (blended OIT).

public:

  DEFINE_STANDARD_ALLOC

};

#endif //_OpenGl_LayerList_Header
