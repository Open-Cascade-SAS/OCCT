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


#include <OpenGl_GlCore11.hxx>

#include <OpenGl_LayerList.hxx>
#include <OpenGl_Structure.hxx>

#include <InterfaceGraphic_Graphic3d.hxx>
#include <InterfaceGraphic.hxx>

//=======================================================================
//function : OpenGl_LayerList
//purpose  : Constructor
//=======================================================================

OpenGl_LayerList::OpenGl_LayerList (const Standard_Integer theNbPriorities)
 : myNbPriorities (theNbPriorities),
   myNbStructures (0)
{
  // insert default priority layer
  myLayers.Append (OpenGl_PriorityList (myNbPriorities));
  myLayerIds.Bind (0, myLayers.Length());
}

//=======================================================================
//function : ~OpenGl_LayerList
//purpose  : Destructor
//=======================================================================

OpenGl_LayerList::~OpenGl_LayerList ()
{
}

//=======================================================================
//function : defaultLayer
//purpose  :
//=======================================================================

OpenGl_PriorityList& OpenGl_LayerList::defaultLayer()
{
  return myLayers.ChangeValue (1);
}

//=======================================================================
//function : NbPriorities
//purpose  : Method returns the number of available priorities
//=======================================================================

Standard_Integer OpenGl_LayerList::NbPriorities () const
{
  return myNbPriorities;
}

//=======================================================================
//function : NbStructures
//purpose  : Method returns the number of available structures
//=======================================================================

Standard_Integer OpenGl_LayerList::NbStructures () const
{
  return myNbStructures;
}

//=======================================================================
//function : AddLayer
//purpose  : 
//=======================================================================

void OpenGl_LayerList::AddLayer (const Standard_Integer theLayerId)
{
  if (HasLayer (theLayerId))
    return;

  // add the new layer
  myLayers.Append (OpenGl_PriorityList (myNbPriorities));
  myLayerIds.Bind (theLayerId, myLayers.Length());
}

//=======================================================================
//function : HasLayer
//purpose  : 
//=======================================================================

Standard_Boolean OpenGl_LayerList::HasLayer 
  (const Standard_Integer theLayerId) const
{
  return myLayerIds.IsBound (theLayerId);
}

//=======================================================================
//function : RemoveLayer
//purpose  :
//=======================================================================

void OpenGl_LayerList::RemoveLayer (const Standard_Integer theLayerId)
{
  if (!HasLayer (theLayerId) || theLayerId == 0)
    return;

  Standard_Integer aRemovePos = myLayerIds.Find (theLayerId);
  
  // move all displayed structures to first layer
  const OpenGl_PriorityList& aList = myLayers.Value (aRemovePos);
  defaultLayer ().Append (aList);

  // remove layer
  myLayers.Remove (aRemovePos);
  myLayerIds.UnBind (theLayerId);

  // updated sequence indexes in map
  OpenGl_LayerSeqIds::Iterator aMapIt (myLayerIds);
  for ( ; aMapIt.More (); aMapIt.Next ())
  {
    Standard_Integer& aSeqIdx = aMapIt.ChangeValue ();
    if (aSeqIdx > aRemovePos)
      aSeqIdx--;
  }
}

//=======================================================================
//function : AddStructure
//purpose  :
//=======================================================================

void OpenGl_LayerList::AddStructure (const OpenGl_Structure *theStructure,
                                     const Standard_Integer  theLayerId,
                                     const Standard_Integer  thePriority)
{
  // add structure to associated layer,
  // if layer doesn't exists, display structure in default layer
  OpenGl_PriorityList& aList = !HasLayer (theLayerId) ? defaultLayer () :
    myLayers.ChangeValue (myLayerIds.Find (theLayerId));

  aList.Add (theStructure, thePriority);
  myNbStructures++;
}

//=======================================================================
//function : RemoveStructure
//purpose  : 
//=======================================================================

void OpenGl_LayerList::RemoveStructure (const OpenGl_Structure *theStructure,
                                        const Standard_Integer  theLayerId)
{
  Standard_Integer aSeqPos = !HasLayer (theLayerId) ?
    1 : myLayerIds.Find (theLayerId);
  
  OpenGl_PriorityList& aList = myLayers.ChangeValue (aSeqPos);

  // remove structure from associated list
  // if the structure is not found there,
  // scan through layers and remove it
  if (aList.Remove (theStructure) >= 0)
  {
    myNbStructures--;
    return;
  }
  
  // scan through layers and remove it
  Standard_Integer aSeqId = 1;
  OpenGl_SequenceOfLayers::Iterator anIts;
  for (anIts.Init (myLayers); anIts.More (); anIts.Next (), aSeqId++)
  {
    OpenGl_PriorityList& aScanList = anIts.ChangeValue ();
    if (aSeqPos == aSeqId)
      continue;
  
    if (aScanList.Remove (theStructure) >= 0)
    {
      myNbStructures--;
      return;
    }
  }
}

//=======================================================================
//function : ChangeLayer
//purpose  :
//=======================================================================

void OpenGl_LayerList::ChangeLayer (const OpenGl_Structure *theStructure,
                                    const Standard_Integer  theOldLayerId,
                                    const Standard_Integer  theNewLayerId)
{
  Standard_Integer aSeqPos = !HasLayer (theOldLayerId) ?
    1 : myLayerIds.Find (theOldLayerId);
  
  OpenGl_PriorityList& aList = myLayers.ChangeValue (aSeqPos);
  Standard_Integer aPriority;

  // take priority and remove structure from list found by <theOldLayerId>
  // if the structure is not found there, scan through all other layers
  if ((aPriority = aList.Remove (theStructure)) >= 0)
  {
    myNbStructures--;
    AddStructure (theStructure, theNewLayerId, aPriority);
  }
  else
  {
    // scan through layers and remove it
    Standard_Integer aSeqId = 1;
    OpenGl_SequenceOfLayers::Iterator anIts;
    for (anIts.Init (myLayers); anIts.More (); anIts.Next (), aSeqId++)
    {
      OpenGl_PriorityList& aScanList = anIts.ChangeValue ();
      if (aSeqPos == aSeqId)
        continue;
  
      // try to remove structure and get priority value from this layer
      if ((aPriority = aList.Remove (theStructure)) >= 0)
      {
        myNbStructures--;
        AddStructure (theStructure, theNewLayerId, aPriority);
        break;
      }
    }
  }
}

//=======================================================================
//function : Render
//purpose  : Render this element
//=======================================================================

void OpenGl_LayerList::Render (const Handle(OpenGl_Workspace) &theWorkspace) const
{
  OpenGl_SequenceOfLayers::Iterator anIts;
  for(anIts.Init (myLayers); anIts.More (); anIts.Next ())
  {
    const OpenGl_PriorityList& aList = anIts.Value ();
    if (aList.NbStructures () > 0)
    {
      // separate depth buffers
      glClear (GL_DEPTH_BUFFER_BIT);

      // render priority list
      aList.Render (theWorkspace);
    }
  }
}
