// Copyright (c) 2001-2024 OPEN CASCADE SAS
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

#ifndef NCollection_HSequence_HeaderFile
#define NCollection_HSequence_HeaderFile

#include <NCollection_Sequence.hxx>
#include <Standard_Type.hxx>
#include <Standard_Transient.hxx>

//! Template class for Handle-managed sequences.
//! Inherits from both NCollection_Sequence<TheItemType> and Standard_Transient,
//! providing reference-counted sequence functionality.
template <typename TheItemType>
class NCollection_HSequence : public NCollection_Sequence<TheItemType>, public Standard_Transient
{
public:
  DEFINE_STANDARD_ALLOC
  DEFINE_NCOLLECTION_ALLOC

  typedef NCollection_Sequence<TheItemType> SequenceType;
  typedef TheItemType                       value_type;

public:
  //! Default constructor.
  NCollection_HSequence() {}

  //! Copy constructor from sequence.
  //! @param theOther the sequence to copy from
  NCollection_HSequence(const SequenceType& theOther)
      : SequenceType(theOther)
  {
  }

  //! Returns const reference to the underlying sequence.
  const SequenceType& Sequence() const noexcept { return *this; }

  //! Returns mutable reference to the underlying sequence.
  SequenceType& ChangeSequence() noexcept { return *this; }

  //! Append single item.
  //! @param theItem the item to append
  void Append(const TheItemType& theItem) { SequenceType::Append(theItem); }

  //! Append another sequence.
  //! @param theSequence the sequence to append
  void Append(SequenceType& theSequence) { SequenceType::Append(theSequence); }

  //! Append items from another HSequence.
  //! @param theOther handle to another HSequence
  template <class T>
  void Append(const opencascade::handle<T>& theOther,
              typename std::enable_if<std::is_base_of<NCollection_HSequence, T>::value>::type* = 0)
  {
    SequenceType::Append(theOther->ChangeSequence());
  }

  DEFINE_STANDARD_RTTI_INLINE(NCollection_HSequence, Standard_Transient)
};

#endif // NCollection_HSequence_HeaderFile
