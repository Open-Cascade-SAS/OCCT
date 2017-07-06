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

#ifndef _TDF_Delta_HeaderFile
#define _TDF_Delta_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <TDF_AttributeDeltaList.hxx>
#include <TCollection_ExtendedString.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Boolean.hxx>
#include <TDF_LabelList.hxx>
#include <Standard_OStream.hxx>
class Standard_OutOfRange;
class TDF_Data;
class TDF_AttributeDelta;
class TCollection_ExtendedString;


class TDF_Delta;
DEFINE_STANDARD_HANDLE(TDF_Delta, Standard_Transient)

//! A set of AttributeDelta for a given transaction
//! number and reference time number.
//! A delta set is available at <aSourceTime>. If
//! applied, it restores the TDF_Data in the state it
//! was at <aTargetTime>.
class TDF_Delta : public Standard_Transient
{

public:

  
  //! Creates a delta.
  Standard_EXPORT TDF_Delta();
  
  //! Returns true if there is nothing to undo.
    Standard_Boolean IsEmpty() const;
  
  //! Returns true if the Undo action of <me> is
  //! applicable at <aCurrentTime>.
    Standard_Boolean IsApplicable (const Standard_Integer aCurrentTime) const;
  
  //! Returns the field <myBeginTime>.
    Standard_Integer BeginTime() const;
  
  //! Returns the field <myEndTime>.
    Standard_Integer EndTime() const;
  
  //! Adds in <aLabelList> the labels of the attribute deltas.
  //! Caution: <aLabelList> is not cleared before use.
  Standard_EXPORT void Labels (TDF_LabelList& aLabelList) const;
  
  //! Returns the field <myAttDeltaList>.
    const TDF_AttributeDeltaList& AttributeDeltas() const;
  
  //! Returns a name associated with this delta.
    TCollection_ExtendedString Name() const;
  
  //! Associates a name <theName> with this delta
    void SetName (const TCollection_ExtendedString& theName);
  
  Standard_EXPORT void Dump (Standard_OStream& OS) const;


friend class TDF_Data;


  DEFINE_STANDARD_RTTIEXT(TDF_Delta,Standard_Transient)

protected:

  
  //! Validates <me> at <aBeginTime>. If applied, it
  //! restores the TDF_Data in the state it was at
  //! <anEndTime>. Reserved to TDF_Data.
  Standard_EXPORT void Validity (const Standard_Integer aBeginTime, const Standard_Integer anEndTime);
  
  //! Adds an AttributeDelta to the list. Reserved to
  //! TDF_Data.
  Standard_EXPORT void AddAttributeDelta (const Handle(TDF_AttributeDelta)& anAttributeDelta);



private:

  
  Standard_EXPORT void BeforeOrAfterApply (const Standard_Boolean before) const;
  
  Standard_EXPORT void Apply();

  Standard_Integer myBeginTime;
  Standard_Integer myEndTime;
  TDF_AttributeDeltaList myAttDeltaList;
  TCollection_ExtendedString myName;


};


#include <TDF_Delta.lxx>





#endif // _TDF_Delta_HeaderFile
