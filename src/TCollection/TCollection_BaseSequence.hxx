// Created on: 1992-09-11
// Created by: Mireille MERCIEN
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _TCollection_BaseSequence_HeaderFile
#define _TCollection_BaseSequence_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Address.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class Standard_NoSuchObject;
class Standard_OutOfRange;


//! Definition of a base class for all instanciations
//! of sequence.
//!
//! The methods : Clear, Remove accepts a pointer to a
//! function  to use  to delete the  nodes. This allow
//! proper    call of  the  destructor  on  the Items.
//! Without adding a  virtual function pointer to each
//! node or each sequence.
class TCollection_BaseSequence 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! returns True if the sequence <me> contains no elements.
    Standard_Boolean IsEmpty() const;
  
  //! Returns  the  number  of element(s) in the
  //! sequence.  Returns zero if the sequence is empty.
    Standard_Integer Length() const;
  
  //! Reverses the order of items on <me>.
  //! Example:
  //! before
  //! me = (A B C)
  //! after
  //! me = (C B A)
  Standard_EXPORT void Reverse();
  
  //! Swaps  elements  which    are  located  at
  //! positions <I> and <J> in <me>.
  //! Raises an exception if I or J is out of bound.
  //! Example:
  //! before
  //! me = (A B C), I = 1, J = 3
  //! after
  //! me = (C B A)
  Standard_EXPORT void Exchange (const Standard_Integer I, const Standard_Integer J);




protected:

  
  //! Creation of an empty sequence.
  Standard_EXPORT TCollection_BaseSequence();
  
  Standard_EXPORT void Clear (const Standard_Address DelNode);
  
  Standard_EXPORT void PAppend (const Standard_Address Node);
  
  //! Concatenates <S> at the end of <me>.
  //! <S> is cleared.
  //! Example:
  //! before
  //! me = (A B C)
  //! S  = (D E F)
  //! after
  //! me = (A B C D E F)
  //! S  = ()
  Standard_EXPORT void PAppend (TCollection_BaseSequence& S);
  
  Standard_EXPORT void PPrepend (const Standard_Address Node);
  
  //! Concatenates <S> at the beginning of <me>.
  //! <S> is cleared.
  //! Example:
  //! before
  //! me = (A B C) S =  (D E F)
  //! after me = (D E F A B C)
  //! S = ()
  Standard_EXPORT void PPrepend (TCollection_BaseSequence& S);
  
  Standard_EXPORT void PInsertAfter (const Standard_Integer Index, const Standard_Address Node);
  
  //! Inserts the sequence <S> in <me> after the
  //! position <Index>. <S> is cleared.
  //! Raises an exception if the index is out of bound.
  //! Example:
  //! before
  //! me = (A B C), Index = 3, S = (D E F)
  //! after
  //! me = (A B C D E F)
  //! S  = ()
  Standard_EXPORT void PInsertAfter (const Standard_Integer Index, TCollection_BaseSequence& S);
  
  //! Keeps in <me> the items 1 to <Index>-1 and
  //! puts  in  <Sub> the  items <Index>  to the end.
  //! Example:
  //! before
  //! me = (A B C D) ,Index = 3
  //! after
  //! me  = (A B)
  //! Sub = (C D)
  Standard_EXPORT void PSplit (const Standard_Integer Index, TCollection_BaseSequence& Sub);
  
  Standard_EXPORT void Remove (const Standard_Integer Index, const Standard_Address DelNode);
  
  Standard_EXPORT void Remove (const Standard_Integer FromIndex, const Standard_Integer ToIndex, const Standard_Address DelNode);
  
  //! Returns the node at position <index>.
  Standard_EXPORT Standard_Address Find (const Standard_Integer Index) const;


  Standard_Address FirstItem;
  Standard_Address LastItem;
  Standard_Address CurrentItem;
  Standard_Integer CurrentIndex;
  Standard_Integer Size;


private:

  
  //! Creation by copy of existing Sequence.
  //! Warning: This constructor prints a warning message.
  //! We recommand to use the operator =.
  Standard_EXPORT TCollection_BaseSequence(const TCollection_BaseSequence& Other);
  
  //! Clear all fields.
  Standard_EXPORT void Nullify();




};


#include <TCollection_BaseSequence.lxx>





#endif // _TCollection_BaseSequence_HeaderFile
