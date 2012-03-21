// Created on: 2004-11-23
// Created by: Pavel TELKOV
// Copyright (c) 2004-2012 OPEN CASCADE SAS
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

// The original implementation Copyright: (C) RINA S.p.A

#ifndef TObj_Partition_HeaderFile
#define TObj_Partition_HeaderFile

#include <TObj_Object.hxx>
#include <TObj_Persistence.hxx>

class Handle(TCollection_HExtendedString);
class Handle(TObj_Partition);

/**
* This class privides tool handling one of partitions (the set of
* homogenious elements) in the OCAF based model`s data structure
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
  Standard_EXPORT TObj_Partition (const TDF_Label& theLabel);
  
 public:
  /**
  * Method for create partition
  */
  
  //! Creates a new partition on given label.
  static Standard_EXPORT Handle(TObj_Partition) Create
                        (const TDF_Label& theLabel);
  
 public:
  /**
  * Methods hanling name of the object
  */

  //! Sets name of the object. partition does not check unique of own name
  virtual Standard_EXPORT Standard_Boolean SetName
                        (const Handle(TCollection_HExtendedString)& theName) const;

 public:
  /**
  * Method for updating object afrer restoring
  */

  //! Preforms updating the links and dependances of the object which are not
  //! stored in persistence. Does not register the partition name
  virtual Standard_EXPORT void AfterRetrieval();

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
  Standard_EXPORT Handle(TCollection_HExtendedString) GetNamePrefix() const
  { return myPrefix; }
  
  //! Generates and returns name for new object in partition.
  //! if theIsToChangeCount is true partition icrease own counter
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
  virtual Standard_EXPORT Standard_Boolean Update()
  {return Standard_True;}

 protected:
  /**
  * protected redefined methods
  */

  //! Coping the data of me to Target object.
  //! return Standard_False is Target object is different type
  Standard_EXPORT virtual Standard_Boolean copyData
                        (const Handle(TObj_Object)& theTargetObject);
 
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
  DEFINE_STANDARD_RTTI(TObj_Partition)

 public:
  friend class TObj_Model;

};

//! Define handle class for TObj_Partition
DEFINE_STANDARD_HANDLE(TObj_Partition,TObj_Object)

#endif

#ifdef _MSC_VER
#pragma once
#endif
