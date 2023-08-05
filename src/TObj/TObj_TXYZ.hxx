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

#ifndef TObj_TXYZ_HeaderFile
#define TObj_TXYZ_HeaderFile


#include <gp_XYZ.hxx>
#include <TDF_Attribute.hxx>


class Standard_GUID;
class TDF_Label;

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
  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;
  
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
  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;
  
  //! Restores the backuped contents from <theWith> into this one. It is used 
  //! when aborting a transaction.
  Standard_EXPORT void Restore(const Handle(TDF_Attribute)& theWith) Standard_OVERRIDE;
  
  //! This method is used when copying an attribute from a source structure
  //! into a target structure.
  Standard_EXPORT void Paste(const Handle(TDF_Attribute)& theInto,
                             const Handle(TDF_RelocationTable)& theRT) const Standard_OVERRIDE;

  //! This method dumps the attribute value into the stream
  Standard_EXPORT virtual Standard_OStream& Dump(Standard_OStream& theOS) const Standard_OVERRIDE;
  
 private:
  //! Fields
  gp_XYZ myXYZ; //!< The object interface stored by the attribute
  
 public:
  //! CASCADE RTTI
  DEFINE_STANDARD_RTTIEXT(TObj_TXYZ,TDF_Attribute)
};

//! Define handle class for TObj_TXYZ
DEFINE_STANDARD_HANDLE(TObj_TXYZ,TDF_Attribute)

#endif

#ifdef _MSC_VER
#pragma once
#endif
