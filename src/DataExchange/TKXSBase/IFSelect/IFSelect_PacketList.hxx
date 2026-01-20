// Created on: 1994-09-02
// Created by: Christian CAILLET
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _IFSelect_PacketList_HeaderFile
#define _IFSelect_PacketList_HeaderFile

#include <Standard.hxx>

#include <Interface_IntList.hxx>
#include <Standard_Integer.hxx>
#include <TCollection_AsciiString.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
class Interface_InterfaceModel;
class Interface_EntityIterator;

//! This class gives a simple way to return then consult a
//! list of packets, determined from the content of a Model,
//! by various criteria.
//!
//! It allows to describe several lists with entities from a
//! given model, possibly more than one list knowing every entity,
//! and to determine the remaining list (entities in no lists) and
//! the duplications (with their count).
class IFSelect_PacketList : public Standard_Transient
{

public:
  //! Creates a PackList, empty, ready to receive entities from a
  //! given Model
  Standard_EXPORT IFSelect_PacketList(const occ::handle<Interface_InterfaceModel>& model);

  //! Sets a name to a packet list : this makes easier a general
  //! routine to print it. Default is "Packets"
  Standard_EXPORT void SetName(const char* const name);

  //! Returns the recorded name for a packet list
  Standard_EXPORT const char* Name() const;

  //! Returns the Model of reference
  Standard_EXPORT occ::handle<Interface_InterfaceModel> Model() const;

  //! Declares a new Packet, ready to be filled
  //! The entities to be added will be added to this Packet
  Standard_EXPORT void AddPacket();

  //! Adds an entity from the Model into the current packet for Add
  Standard_EXPORT void Add(const occ::handle<Standard_Transient>& ent);

  //! Adds an list of entities into the current packet for Add
  Standard_EXPORT void AddList(const occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>>& list);

  //! Returns the count of non-empty packets
  Standard_EXPORT int NbPackets() const;

  //! Returns the count of entities in a Packet given its rank, or 0
  Standard_EXPORT int NbEntities(const int numpack) const;

  //! Returns the content of a Packet given its rank
  //! Null Handle if <numpack> is out of range
  Standard_EXPORT Interface_EntityIterator Entities(const int numpack) const;

  //! Returns the highest number of packets which know a same entity
  //! For no duplication, should be one
  Standard_EXPORT int HighestDuplicationCount() const;

  //! Returns the count of entities duplicated :
  //! <count> times, if <andmore> is False, or
  //! <count> or more times, if <andmore> is True
  //! See Duplicated for more details
  Standard_EXPORT int NbDuplicated(const int count,
                                                const bool andmore) const;

  //! Returns a list of entities duplicated :
  //! <count> times, if <andmore> is False, or
  //! <count> or more times, if <andmore> is True
  //! Hence, count=2 & andmore=True gives all duplicated entities
  //! count=1 gives non-duplicated entities (in only one packet)
  //! count=0 gives remaining entities (in no packet at all)
  Standard_EXPORT Interface_EntityIterator Duplicated(const int count,
                                                      const bool andmore) const;

  DEFINE_STANDARD_RTTIEXT(IFSelect_PacketList, Standard_Transient)

private:
  occ::handle<Interface_InterfaceModel> themodel;
  NCollection_Array1<int>          thedupls;
  Interface_IntList                thepacks;
  NCollection_Array1<int>          theflags;
  int                 thelast;
  bool                 thebegin;
  TCollection_AsciiString          thename;
};

#endif // _IFSelect_PacketList_HeaderFile
