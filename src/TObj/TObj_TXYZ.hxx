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

#ifndef TObj_TXYZ_HeaderFile
#define TObj_TXYZ_HeaderFile

#include <TObj_Common.hxx>

#include <gp_XYZ.hxx>
#include <TDF_Attribute.hxx>


class Handle(TObj_TXYZ);
class Standard_GUID;
class TDF_Label;
class Handle(TDF_RelocationTable);

/*
* Attribute for storing gp_XYZ
*/

class TObj_TXYZ : public TDF_Attribute
{
 public:
  //! Standard methods of OCAF attribute

  //! Empty constructor
  Standard_EXPORT TObj_TXYZ();
  
  //! This method is used in implementation of ID()
  static Standard_EXPORT const Standard_GUID& GetID();
  
  //! Returns the ID of TObj_TXYZ attribute.
  Standard_EXPORT const Standard_GUID& ID() const;
  
 public:
  //! Method for create TObj_TXYZ object
  
  //! Creates attribute and sets the XYZ
  static Standard_EXPORT Handle(TObj_TXYZ) Set (const TDF_Label& theLabel,
                                                const gp_XYZ&    theXYZ);
 public:
  //! Methods for setting and obtaining XYZ
  
  //! Sets the XYZ
  Standard_EXPORT void Set(const gp_XYZ& theXYZ);
  
  //! Returns the XYZ
  Standard_EXPORT gp_XYZ Get() const;
  
 public:
  //! Redefined OCAF abstract methods
    
  //! Returns an new empty TObj_TXYZ attribute. It is used by the
  //! copy algorithm.
  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const;
  
  //! Restores the backuped contents from <theWith> into this one. It is used 
  //! when aborting a transaction.
  Standard_EXPORT void Restore(const Handle(TDF_Attribute)& theWith);
  
  //! This method is used when copying an attribute from a source structure
  //! into a target structure.
  Standard_EXPORT void Paste(const Handle(TDF_Attribute)& theInto,
                             const Handle(TDF_RelocationTable)& theRT) const;

  //! This method dumps the attribute value into the stream
  Standard_EXPORT virtual Standard_OStream& Dump(Standard_OStream& theOS) const;
  
 private:
  //! Fields
  gp_XYZ myXYZ; //!< The object interface stored by the attribute
  
 public:
  //! CASCADE RTTI
  DEFINE_STANDARD_RTTI(TObj_TXYZ)
};

//! Define handle class for TObj_TXYZ
DEFINE_STANDARD_HANDLE(TObj_TXYZ,TDF_Attribute)

#endif

#ifdef _MSC_VER
#pragma once
#endif
