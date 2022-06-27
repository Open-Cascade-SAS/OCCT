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

#ifndef _TDF_AttributeDelta_HeaderFile
#define _TDF_AttributeDelta_HeaderFile

#include <Standard.hxx>

#include <TDF_Label.hxx>
#include <Standard_Transient.hxx>
#include <Standard_OStream.hxx>
class TDF_Attribute;
class Standard_GUID;


class TDF_AttributeDelta;
DEFINE_STANDARD_HANDLE(TDF_AttributeDelta, Standard_Transient)

//! This class describes the services we need to
//! implement Delta and Undo/Redo services.
//!
//! AttributeDeltas are applied in an unpredictable
//! order. But by the redefinition of the method
//! IsNowApplicable, a condition can be verified
//! before application. If the AttributeDelta is not
//! yet applicable, it is put at the end of the
//! AttributeDelta list, to be treated later. If a
//! dead lock if found on the list, the
//! AttributeDeltas are forced to be applied in an
//! unpredictable order.
class TDF_AttributeDelta : public Standard_Transient
{

public:

  
  //! Applies the delta to the attribute.
  Standard_EXPORT virtual void Apply() = 0;
  
  //! Returns the label concerned by <me>.
  Standard_EXPORT TDF_Label Label() const;
  
  //! Returns the reference attribute.
  Standard_EXPORT Handle(TDF_Attribute) Attribute() const;
  
  //! Returns the ID of the attribute concerned by <me>.
  Standard_EXPORT Standard_GUID ID() const;
  
  //! Dumps the contents.
  Standard_EXPORT virtual Standard_OStream& Dump (Standard_OStream& OS) const;
Standard_OStream& operator<< (Standard_OStream& OS) const
{
  return Dump(OS);
}

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;




  DEFINE_STANDARD_RTTIEXT(TDF_AttributeDelta,Standard_Transient)

protected:

  
  Standard_EXPORT TDF_AttributeDelta(const Handle(TDF_Attribute)& anAttribute);



private:


  Handle(TDF_Attribute) myAttribute;
  TDF_Label myLabel;


};







#endif // _TDF_AttributeDelta_HeaderFile
