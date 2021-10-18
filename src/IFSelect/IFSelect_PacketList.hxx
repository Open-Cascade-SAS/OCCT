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
#include <Standard_Type.hxx>

#include <TColStd_Array1OfInteger.hxx>
#include <Interface_IntList.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <TCollection_AsciiString.hxx>
#include <Standard_Transient.hxx>
#include <Standard_CString.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
class Interface_InterfaceModel;
class Interface_EntityIterator;


class IFSelect_PacketList;
DEFINE_STANDARD_HANDLE(IFSelect_PacketList, Standard_Transient)

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
  Standard_EXPORT IFSelect_PacketList(const Handle(Interface_InterfaceModel)& model);
  
  //! Sets a name to a packet list : this makes easier a general
  //! routine to print it. Default is "Packets"
  Standard_EXPORT void SetName (const Standard_CString name);
  
  //! Returns the recorded name for a packet list
  Standard_EXPORT Standard_CString Name() const;
  
  //! Returns the Model of reference
  Standard_EXPORT Handle(Interface_InterfaceModel) Model() const;
  
  //! Declares a new Packet, ready to be filled
  //! The entities to be added will be added to this Packet
  Standard_EXPORT void AddPacket();
  
  //! Adds an entity from the Model into the current packet for Add
  Standard_EXPORT void Add (const Handle(Standard_Transient)& ent);
  
  //! Adds an list of entities into the current packet for Add
  Standard_EXPORT void AddList (const Handle(TColStd_HSequenceOfTransient)& list);
  
  //! Returns the count of non-empty packets
  Standard_EXPORT Standard_Integer NbPackets() const;
  
  //! Returns the count of entities in a Packet given its rank, or 0
  Standard_EXPORT Standard_Integer NbEntities (const Standard_Integer numpack) const;
  
  //! Returns the content of a Packet given its rank
  //! Null Handle if <numpack> is out of range
  Standard_EXPORT Interface_EntityIterator Entities (const Standard_Integer numpack) const;
  
  //! Returns the highest number of packets which know a same entity
  //! For no duplication, should be one
  Standard_EXPORT Standard_Integer HighestDuplicationCount() const;
  
  //! Returns the count of entities duplicated :
  //! <count> times, if <andmore> is False, or
  //! <count> or more times, if <andmore> is True
  //! See Duplicated for more details
  Standard_EXPORT Standard_Integer NbDuplicated (const Standard_Integer count, const Standard_Boolean andmore) const;
  
  //! Returns a list of entities duplicated :
  //! <count> times, if <andmore> is False, or
  //! <count> or more times, if <andmore> is True
  //! Hence, count=2 & andmore=True gives all duplicated entities
  //! count=1 gives non-duplicated entities (in only one packet)
  //! count=0 gives remaining entities (in no packet at all)
  Standard_EXPORT Interface_EntityIterator Duplicated (const Standard_Integer count, const Standard_Boolean andmore) const;




  DEFINE_STANDARD_RTTIEXT(IFSelect_PacketList,Standard_Transient)

protected:




private:


  Handle(Interface_InterfaceModel) themodel;
  TColStd_Array1OfInteger thedupls;
  Interface_IntList thepacks;
  TColStd_Array1OfInteger theflags;
  Standard_Integer thelast;
  Standard_Boolean thebegin;
  TCollection_AsciiString thename;


};







#endif // _IFSelect_PacketList_HeaderFile
