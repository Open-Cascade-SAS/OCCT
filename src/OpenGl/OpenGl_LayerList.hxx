// File:      OpenGl_LayerList.hxx
// Created:   2 February 2012
// Author:    Anton POLETAEV
// Copyright: OPEN CASCADE 2012

#ifndef _OpenGl_LayerList_Header
#define _OpenGl_LayerList_Header

#include <OpenGl_PriorityList.hxx>

#include <InterfaceGraphic_telem.hxx>

#include <NCollection_Sequence.hxx>
#include <NCollection_DataMap.hxx>

class OpenGl_Structure;
class Handle(OpenGl_Workspace);

typedef NCollection_Sequence<OpenGl_PriorityList> OpenGl_SequenceOfLayers;
typedef NCollection_DataMap<int, int> OpenGl_LayerSeqIds;

class OpenGl_LayerList
{
 public:
  
  //! Constructor
  OpenGl_LayerList (const Standard_Integer theNbPriorities = 11);
  
  //! Destructor
  virtual ~OpenGl_LayerList ();
  
  //! Method returns the number of available priorities
  Standard_Integer NbPriorities () const;

  //! Number of displayed structures
  Standard_Integer NbStructures () const;

  //! Insert a new layer with id.
  void AddLayer (const Standard_Integer theLayerId);
  
  //! Check whether the layer with given id is already created.
  Standard_Boolean HasLayer (const Standard_Integer theLayerId) const;
  
  //! Remove layer by its id.
  void RemoveLayer (const Standard_Integer theLayerId);
  
  //! Add structure to list with given priority. The structure will be inserted
  //! to specified layer. If the layer isn't found, the structure will be put
  //! to default bottom-level layer.
  void AddStructure (const OpenGl_Structure *theStructure,
                     const Standard_Integer  theLayerId,
                     const Standard_Integer  thePriority);
  
  //! Remove structure from structure list and return its previous priority
  void RemoveStructure (const OpenGl_Structure *theStructure,
                        const Standard_Integer  theZLayerId);
  
  //! Change structure z layer
  //! If the new layer is not presented, the structure will be displayed
  //! in default z layer
  void ChangeLayer (const OpenGl_Structure *theStructure,
                    const Standard_Integer  theOldLayerId,
                    const Standard_Integer  theNewLayerId);
  
  //! Render this element
  void Render (const Handle(OpenGl_Workspace) &theWorkspace) const;
  
 private:
  
  //! Get default layer
  OpenGl_PriorityList& defaultLayer ();
  
 protected:

  // number of structures temporary put to default layer
  OpenGl_SequenceOfLayers myLayers;
  OpenGl_LayerSeqIds      myLayerIds;
  Standard_Integer        myNbPriorities;
  Standard_Integer        myNbStructures;

 public:
  IMPLEMENT_MEMORY_OPERATORS
};

#endif //_OpenGl_LayerList_Header
