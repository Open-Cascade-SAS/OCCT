// Created by: DAUTRY Philippe
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _TDF_ChildIterator_HeaderFile
#define _TDF_ChildIterator_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <TDF_Label.hxx>
class TDF_Label;


//! Iterates on the children of a label, at the first
//! level only. It is possible to ask the iterator to
//! explore all the sub label levels of the given one,
//! with the option "allLevels".
class TDF_ChildIterator 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates an empty iterator  object to
  //! explore the children of a label.
  Standard_EXPORT TDF_ChildIterator();
  
  //! Constructs the iterator object defined by
  //! the label aLabel.  Iterates on the children of the given label. If
  //! <allLevels> option is set to true, it explores not
  //! only the first, but all the sub label levels.
  Standard_EXPORT TDF_ChildIterator(const TDF_Label& aLabel, const Standard_Boolean allLevels = Standard_False);
  
  //! Initializes the iteration on the children of the
  //! given label.
  //! If <allLevels> option is set to true,
  //! it explores not only the first, but all the sub
  //! label levels.
  //! If allLevels is false, only the first level of
  //! child labels is explored.
  //! In the example below, the label is iterated
  //! using Initialize, More and Next and its
  //! child labels dumped using TDF_Tool::Entry.
  //! Example
  //! void DumpChildren(const
  //! TDF_Label& aLabel)
  //! {
  //! TDF_ChildIterator it;
  //! TCollection_AsciiString es;
  //! for
  //! (it.Initialize(aLabel,Standard_True);
  //! it.More(); it.Next()){
  //! TDF_Tool::Entry(it.Value(),es);
  //! std::cout << as.ToCString() << std::endl;
  //! }
  //! }
  Standard_EXPORT void Initialize (const TDF_Label& aLabel, const Standard_Boolean allLevels = Standard_False);
  
  //! Returns true if a current label is found in the
  //! iteration process.
    Standard_Boolean More() const;
  
  //! Move the  current  iteration  to the next Item.
  Standard_EXPORT void Next();
  
  //! Moves this iteration to the next brother
  //! label. A brother label is one with the same
  //! father as an initial label.
  //! Use this function when the non-empty
  //! constructor or Initialize has allLevels set to
  //! true. The result is that the iteration does not
  //! explore the children of the current label.
  //! This method is interesting only with
  //! "allLevels" behavior, because it avoids to explore
  //! the current label children.
  Standard_EXPORT void NextBrother();
  
  //! Returns the current label; or, if there is
  //! none, a null label.
    const TDF_Label Value() const;




protected:





private:



  TDF_LabelNodePtr myNode;
  Standard_Integer myFirstLevel;


};


#include <TDF_ChildIterator.lxx>





#endif // _TDF_ChildIterator_HeaderFile
