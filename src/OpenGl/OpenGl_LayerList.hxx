// Created on: 2012-02-02
// Created by: Anton POLETAEV
// Copyright (c) -1999 Matra Datavision
// Copyright (c) 2012-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


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
  DEFINE_STANDARD_ALLOC
};

#endif //_OpenGl_LayerList_Header
