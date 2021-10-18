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

#ifndef OpenGl_LayerList_HeaderFile
#define OpenGl_LayerList_HeaderFile

#include <OpenGl_Layer.hxx>
#include <OpenGl_LayerFilter.hxx>

#include <Graphic3d_ZLayerId.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Handle.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_DataMap.hxx>

class OpenGl_FrameBuffer;
class OpenGl_Structure;
class OpenGl_Workspace;
struct OpenGl_GlobalLayerSettings;

//! Class defining the list of layers.
class OpenGl_LayerList
{
public:

  //! Constructor
  Standard_EXPORT OpenGl_LayerList (const Standard_Integer theNbPriorities);

  //! Destructor
  Standard_EXPORT virtual ~OpenGl_LayerList();

  //! Method returns the number of available priorities
  Standard_Integer NbPriorities() const { return myNbPriorities; }

  //! Number of displayed structures
  Standard_Integer NbStructures() const { return myNbStructures; }

  //! Return number of structures within immediate layers
  Standard_Integer NbImmediateStructures() const { return myImmediateNbStructures; }

  //! Insert a new layer with id.
  Standard_EXPORT void InsertLayerBefore (const Graphic3d_ZLayerId theNewLayerId,
                                          const Graphic3d_ZLayerSettings& theSettings,
                                          const Graphic3d_ZLayerId theLayerAfter);

  //! Insert a new layer with id.
  Standard_EXPORT void InsertLayerAfter (const Graphic3d_ZLayerId theNewLayerId,
                                         const Graphic3d_ZLayerSettings& theSettings,
                                         const Graphic3d_ZLayerId theLayerBefore);

  //! Remove layer by its id.
  Standard_EXPORT void RemoveLayer (const Graphic3d_ZLayerId theLayerId);

  //! Add structure to list with given priority. The structure will be inserted
  //! to specified layer. If the layer isn't found, the structure will be put
  //! to default bottom-level layer.
  Standard_EXPORT void AddStructure (const OpenGl_Structure*  theStruct,
                                     const Graphic3d_ZLayerId theLayerId,
                                     const Standard_Integer   thePriority,
                                     Standard_Boolean        isForChangePriority = Standard_False);

  //! Remove structure from structure list and return its previous priority
  Standard_EXPORT void RemoveStructure (const OpenGl_Structure* theStructure);

  //! Change structure z layer
  //! If the new layer is not presented, the structure will be displayed
  //! in default z layer
  Standard_EXPORT void ChangeLayer (const OpenGl_Structure*  theStructure,
                                    const Graphic3d_ZLayerId theOldLayerId,
                                    const Graphic3d_ZLayerId theNewLayerId);

  //! Changes structure priority within its ZLayer
  Standard_EXPORT void ChangePriority (const OpenGl_Structure*  theStructure,
                                       const Graphic3d_ZLayerId theLayerId,
                                       const Standard_Integer   theNewPriority);

  //! Returns reference to the layer with given ID.
  OpenGl_Layer& Layer (const Graphic3d_ZLayerId theLayerId) { return *myLayerIds.Find (theLayerId); }

  //! Returns reference to the layer with given ID.
  const OpenGl_Layer& Layer (const Graphic3d_ZLayerId theLayerId) const { return *myLayerIds.Find (theLayerId); }

  //! Assign new settings to the layer.
  Standard_EXPORT void SetLayerSettings (const Graphic3d_ZLayerId        theLayerId,
                                         const Graphic3d_ZLayerSettings& theSettings);

  //! Update culling state - should be called before rendering.
  Standard_EXPORT void UpdateCulling (const Handle(OpenGl_Workspace)& theWorkspace,
                                      const Standard_Boolean theToDrawImmediate);

  //! Render this element
  Standard_EXPORT void Render (const Handle(OpenGl_Workspace)& theWorkspace,
                               const Standard_Boolean          theToDrawImmediate,
                               const OpenGl_LayerFilter        theLayersToProcess,
                               OpenGl_FrameBuffer*             theReadDrawFbo,
                               OpenGl_FrameBuffer*             theOitAccumFbo) const;

  //! Returns the set of OpenGL Z-layers.
  const NCollection_List<Handle(Graphic3d_Layer)>& Layers() const { return myLayers; }

  //! Returns the map of Z-layer IDs to indexes.
  const NCollection_DataMap<Graphic3d_ZLayerId, Handle(Graphic3d_Layer)>& LayerIDs() const { return myLayerIds; }

  //! Marks BVH tree for given priority list as dirty and
  //! marks primitive set for rebuild.
  Standard_EXPORT void InvalidateBVHData (const Graphic3d_ZLayerId theLayerId);

  //! Returns structure modification state (for ray-tracing).
  Standard_Size ModificationStateOfRaytracable() const { return myModifStateOfRaytraceable; }

  //! Returns BVH tree builder for frustom culling.
  const Handle(Select3D_BVHBuilder3d)& FrustumCullingBVHBuilder() const { return myBVHBuilder; }

  //! Assigns BVH tree builder for frustom culling.
  Standard_EXPORT void SetFrustumCullingBVHBuilder (const Handle(Select3D_BVHBuilder3d)& theBuilder);

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;

protected:

  //! Stack of references to existing layers of predefined maximum size.
  class OpenGl_LayerStack
  {
  public:
    typedef NCollection_Array1<const Graphic3d_Layer*>::iterator iterator;

    //! Reallocate internal buffer of the stack.
    void Allocate (Standard_Integer theSize)
    {
      if (theSize > 0)
      {
        myStackSpace.Resize (1, theSize, false);
        myStackSpace.Init (NULL);
        myBackPtr = myStackSpace.begin();
      }
      else
      {
        NCollection_Array1<const Graphic3d_Layer*> aDummy;
        myStackSpace.Move (aDummy);
        myBackPtr = iterator();
      }
    }

    //! Clear stack.
    void Clear()
    {
      myStackSpace.Init (NULL);
      myBackPtr = myStackSpace.begin();
    }

    //! Push a new layer reference to the stack.
    void Push (const OpenGl_Layer* theLayer) { (*myBackPtr++) = theLayer; }

    //! Returns iterator to the origin of the stack.
    iterator Origin() const { return myStackSpace.IsEmpty() ? iterator() : myStackSpace.begin(); }

    //! Returns iterator to the back of the stack (after last item added).
    iterator Back() const { return myBackPtr; }

    //! Returns true if nothing has been pushed into the stack.
    Standard_Boolean IsEmpty() const { return Back() == Origin(); }

  private:

    NCollection_Array1<const OpenGl_Layer*> myStackSpace;
    iterator                                myBackPtr;
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
  Standard_EXPORT void renderTransparent (const Handle(OpenGl_Workspace)&   theWorkspace,
                                          OpenGl_LayerStack::iterator&      theLayerIter,
                                          const OpenGl_GlobalLayerSettings& theGlobalSettings,
                                          OpenGl_FrameBuffer*               theReadDrawFbo,
                                          OpenGl_FrameBuffer*               theOitAccumFbo) const;

  // Render structures within specified layer.
  Standard_EXPORT void renderLayer (const Handle(OpenGl_Workspace)& theWorkspace,
                                    const OpenGl_GlobalLayerSettings& theDefaultSettings,
                                    const Graphic3d_Layer& theLayer) const;

protected:

  NCollection_List<Handle(Graphic3d_Layer)> myLayers;
  NCollection_DataMap<Graphic3d_ZLayerId, Handle(Graphic3d_Layer)> myLayerIds;
  Handle(Select3D_BVHBuilder3d) myBVHBuilder;      //!< BVH tree builder for frustom culling

  Standard_Integer        myNbPriorities;
  Standard_Integer        myNbStructures;
  Standard_Integer        myImmediateNbStructures; //!< number of structures within immediate layers

  mutable Standard_Size   myModifStateOfRaytraceable;

  //! Collection of references to layers with transparency gathered during rendering pass.
  mutable OpenGl_LayerStack myTransparentToProcess;

public:

  DEFINE_STANDARD_ALLOC

};

#endif //_OpenGl_LayerList_Header
