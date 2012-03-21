// Created on: 2011-11-02
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2012 OPEN CASCADE SAS
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


#ifndef _OpenGl_PriorityList_Header
#define _OpenGl_PriorityList_Header

#include <NCollection_Sequence.hxx>
#include <NCollection_Array1.hxx>

#include <InterfaceGraphic_telem.hxx>

#include <Handle_OpenGl_Workspace.hxx>

class OpenGl_Structure;

typedef NCollection_Sequence<const OpenGl_Structure*> OpenGl_SequenceOfStructure;
typedef NCollection_Array1<OpenGl_SequenceOfStructure> OpenGl_ArrayOfStructure;

class OpenGl_PriorityList
{
 public:

  OpenGl_PriorityList (const Standard_Integer ANbPriorities = 11) : myArray(0,(ANbPriorities-1)), myNbStructures(0) {}

  virtual ~OpenGl_PriorityList () {}

  void Add (const OpenGl_Structure *AStructure, const Standard_Integer APriority);

  //! Remove structure and returns its priority, if the structure is not found,
  //! method returns negative value
  Standard_Integer Remove (const OpenGl_Structure *AStructure);

  Standard_Integer NbStructures () const { return myNbStructures; }

  void Render (const Handle(OpenGl_Workspace) &AWorkspace) const;

  //! Returns the number of available priority levels
  Standard_Integer NbPriorities() const;

  //! Append priority list of acceptable type (with similar number of priorities
  //! or less). Returns Standard_False if the list can not be accepted.
  Standard_Boolean Append (const OpenGl_PriorityList& theOther);

 protected:

  OpenGl_ArrayOfStructure myArray;
  Standard_Integer        myNbStructures;

 public:
  DEFINE_STANDARD_ALLOC
};

#endif //_OpenGl_PriorityList_Header
