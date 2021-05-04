// Created on: 2004-11-23
// Created by: Pavel TELKOV
// Copyright (c) 2004-2014 OPEN CASCADE SAS
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

// The original implementation Copyright: (C) RINA S.p.A

#ifndef TObj_Partition_HeaderFile
#define TObj_Partition_HeaderFile

#include <TObj_Object.hxx>
#include <TObj_Persistence.hxx>


/**
* This class provides tool handling one of partitions (the set of
* homogeneous elements) in the OCAF based model`s data structure
*/

class TObj_Partition : public TObj_Object
{
 protected:
  enum DataTag
  {
    DataTag_First = TObj_Object::DataTag_Last,
    DataTag_LastIndex,
    DataTag_Last = DataTag_First + 100
  };

 protected:
  /**
  * Constructor
  */

  //! Constructor is protected;
  //! static methods are used for creation of this type of objects
  Standard_EXPORT TObj_Partition (const TDF_Label& theLabel, const Standard_Boolean theSetName = Standard_True);

 public:
  /**
  * Method for create partition
  */

  //! Creates a new partition on given label.
  static Standard_EXPORT Handle(TObj_Partition) Create
                        (const TDF_Label& theLabel, const Standard_Boolean theSetName = Standard_True);

 public:
  /**
  * Methods handling name of the object
  */

  //! Sets name of the object. partition does not check unique of own name
  virtual Standard_EXPORT Standard_Boolean SetName
                        (const Handle(TCollection_HExtendedString)& theName) const Standard_OVERRIDE;

 public:
  /**
  * Method for updating object afrer restoring
  */

  //! Performs updating the links and dependencies of the object which are not
  //! stored in persistence. Does not register the partition name
  virtual Standard_EXPORT void AfterRetrieval() Standard_OVERRIDE;

 public:
  /**
  * Methods handling of the objects in partition
  */

  //! Creates and Returns label for new object in partition.
  Standard_EXPORT TDF_Label NewLabel() const;

  //! Sets prefix for names of the objects in partition.
  Standard_EXPORT void SetNamePrefix
                        (const Handle(TCollection_HExtendedString)& thePrefix);

  //! Returns prefix for names of the objects in partition.
  Handle(TCollection_HExtendedString) GetNamePrefix() const
  { return myPrefix; }

  //! Generates and returns name for new object in partition.
  //! if theIsToChangeCount is true partition increase own counter
  //!  to generate new name next time starting from new counter value
  Standard_EXPORT Handle(TCollection_HExtendedString) GetNewName
    ( const Standard_Boolean theIsToChangeCount = Standard_True );

  //! Return Last index in partition (reserved);
  Standard_EXPORT Standard_Integer GetLastIndex() const;

  //! Sets Last index in partition (reserved);
  Standard_EXPORT void SetLastIndex(const Standard_Integer theIndex);

 public:
  /**
  * Methods to define partition by object
  */

  //! Returns the partition in which object is stored. Null partition
  //! returned if not found
  static Standard_EXPORT Handle(TObj_Partition) GetPartition
                        (const Handle(TObj_Object)& theObject);

 public:
  /**
  * Methods for updating the object
  */

  //! Does nothing in the partition.
  virtual Standard_Boolean Update()
  {return Standard_True;}

 protected:
  /**
  * protected redefined methods
  */

  //! Coping the data of me to Target object.
  //! return Standard_False is Target object is different type
  Standard_EXPORT virtual Standard_Boolean copyData
                        (const Handle(TObj_Object)& theTargetObject) Standard_OVERRIDE;

 private:
  /**
  * fields
  */

  //! prefix for naming of objects in the partition
  Handle(TCollection_HExtendedString) myPrefix;

 protected:
  //! Persistence of TObj object
  DECLARE_TOBJOCAF_PERSISTENCE(TObj_Partition,TObj_Object)

 public:
  //! CASCADE RTTI
  DEFINE_STANDARD_RTTIEXT(TObj_Partition,TObj_Object)

 public:
  friend class TObj_Model;

};

//! Define handle class for TObj_Partition
DEFINE_STANDARD_HANDLE(TObj_Partition,TObj_Object)

#endif

#ifdef _MSC_VER
#pragma once
#endif
