// File:      OpenGl_PriorityList.hxx
// Created:   2 November 2011
// Author:    Sergey ZERCHANINOV
// Copyright: OPEN CASCADE 2011

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
  IMPLEMENT_MEMORY_OPERATORS
};

#endif //_OpenGl_PriorityList_Header
