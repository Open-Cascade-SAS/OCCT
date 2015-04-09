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

#include <InterfaceGraphic_telem.hxx>

#include <NCollection_Sequence.hxx>
#include <NCollection_DataMap.hxx>

class OpenGl_Structure;
class Handle(OpenGl_Workspace);

typedef NCollection_Sequence<OpenGl_Layer> OpenGl_SequenceOfLayers;
typedef NCollection_DataMap<int, int> OpenGl_LayerSeqIds;

class OpenGl_LayerList
{
public:

  //! Constructor
  OpenGl_LayerList (const Standard_Integer theNbPriorities = 11);

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
  void RemoveStructure (const Handle(Graphic3d_Structure)& theStructure);

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
               const OpenGl_LayerFilter        theLayersToProcess) const;

  //! Returns the set of OpenGL Z-layers.
  const OpenGl_SequenceOfLayers& Layers() const { return myLayers; }

  //! Marks BVH tree for given priority list as dirty and
  //! marks primitive set for rebuild.
  void InvalidateBVHData (const Graphic3d_ZLayerId theLayerId);

  //! Returns structure modification state (for ray-tracing).
  Standard_Size ModificationState() const { return myModificationState; }

protected:

  // number of structures temporary put to default layer
  OpenGl_SequenceOfLayers myLayers;
  OpenGl_LayerSeqIds      myLayerIds;

  Standard_Integer        myNbPriorities;
  Standard_Integer        myNbStructures;
  Standard_Integer        myImmediateNbStructures; //!< number of structures within immediate layers

  mutable Standard_Size   myModificationState;

public:

  DEFINE_STANDARD_ALLOC

};

#endif //_OpenGl_LayerList_Header
