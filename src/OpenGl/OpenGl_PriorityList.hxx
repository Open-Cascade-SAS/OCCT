// Created on: 2011-11-02
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2014 OPEN CASCADE SAS
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

  //! Returns array of OpenGL structures.
  const OpenGl_ArrayOfStructure& ArrayOfStructures() const { return myArray; }

 protected:

  OpenGl_ArrayOfStructure myArray;
  Standard_Integer        myNbStructures;

 public:
  DEFINE_STANDARD_ALLOC
};

#endif //_OpenGl_PriorityList_Header
