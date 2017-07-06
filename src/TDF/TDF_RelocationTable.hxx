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

#ifndef _TDF_RelocationTable_HeaderFile
#define _TDF_RelocationTable_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Boolean.hxx>
#include <TDF_LabelDataMap.hxx>
#include <TDF_AttributeDataMap.hxx>
#include <TColStd_IndexedDataMapOfTransientTransient.hxx>
#include <Standard_Transient.hxx>
#include <TDF_LabelMap.hxx>
#include <TDF_AttributeMap.hxx>
#include <Standard_OStream.hxx>
class TDF_Label;
class TDF_Attribute;
class Standard_Transient;


class TDF_RelocationTable;
DEFINE_STANDARD_HANDLE(TDF_RelocationTable, Standard_Transient)

//! This is a relocation dictionnary between source
//! and target labels, attributes or any
//! transient(useful for copy or paste actions).
//! Note that one target value may be the
//! relocation value of more than one source object.
//!
//! Common behaviour: it returns true and the found
//! relocation value as target object; false
//! otherwise.
//!
//! Look at SelfRelocate method for more explanation
//! about self relocation behavior of this class.
class TDF_RelocationTable : public Standard_Transient
{

public:

  
  //! Creates an relocation table. <selfRelocate> says
  //! if a value without explicit relocation is its own
  //! relocation.
  Standard_EXPORT TDF_RelocationTable(const Standard_Boolean selfRelocate = Standard_False);
  
  //! Sets <mySelfRelocate> to <selfRelocate>.
  //!
  //! This flag affects the HasRelocation method
  //! behavior like this:
  //!
  //! <mySelfRelocate> == False:
  //!
  //! If no relocation object is found in the map, a
  //! null object is returned
  //!
  //! <mySelfRelocate> == True:
  //!
  //! If no relocation object is found in the map, the
  //! method assumes the source object is relocation
  //! value; so the source object is returned as target
  //! object.
  Standard_EXPORT void SelfRelocate (const Standard_Boolean selfRelocate);
  
  //! Returns <mySelfRelocate>.
  Standard_EXPORT Standard_Boolean SelfRelocate() const;
  
  Standard_EXPORT void AfterRelocate (const Standard_Boolean afterRelocate);
  
  //! Returns <myAfterRelocate>.
  Standard_EXPORT Standard_Boolean AfterRelocate() const;
  
  //! Sets the relocation value of <aSourceLabel> to
  //! <aTargetLabel>.
  Standard_EXPORT void SetRelocation (const TDF_Label& aSourceLabel, const TDF_Label& aTargetLabel);
  
  //! Finds the relocation value of <aSourceLabel>
  //! and returns it into <aTargetLabel>.
  //!
  //! (See above SelfRelocate method for more
  //! explanation about the method behavior)
  Standard_EXPORT Standard_Boolean HasRelocation (const TDF_Label& aSourceLabel, TDF_Label& aTargetLabel) const;
  
  //! Sets the relocation value of <aSourceAttribute> to
  //! <aTargetAttribute>.
  Standard_EXPORT void SetRelocation (const Handle(TDF_Attribute)& aSourceAttribute, const Handle(TDF_Attribute)& aTargetAttribute);
  
  //! Finds the relocation value of <aSourceAttribute>
  //! and returns it into <aTargetAttribute>.
  //!
  //! (See above SelfRelocate method for more
  //! explanation about the method behavior)
  Standard_EXPORT Standard_Boolean HasRelocation (const Handle(TDF_Attribute)& aSourceAttribute, Handle(TDF_Attribute)& aTargetAttribute) const;
  
  //! Safe variant for arbitrary type of argument
  template <class T> 
  Standard_Boolean HasRelocation (const Handle(TDF_Attribute)& theSource, Handle(T)& theTarget) const
  {
    Handle(TDF_Attribute) anAttr = theTarget;
    return HasRelocation (theSource, anAttr) && ! (theTarget = Handle(T)::DownCast(anAttr)).IsNull();
  }

  //! Sets the relocation value of <aSourceTransient> to
  //! <aTargetTransient>.
  Standard_EXPORT void SetTransientRelocation (const Handle(Standard_Transient)& aSourceTransient, const Handle(Standard_Transient)& aTargetTransient);
  
  //! Finds the relocation value of <aSourceTransient>
  //! and returns it into <aTargetTransient>.
  //!
  //! (See above SelfRelocate method for more
  //! explanation about the method behavior)
  Standard_EXPORT Standard_Boolean HasTransientRelocation (const Handle(Standard_Transient)& aSourceTransient, Handle(Standard_Transient)& aTargetTransient) const;
  
  //! Clears the relocation dictionnary, but lets the
  //! self relocation flag to its current value.
  Standard_EXPORT void Clear();
  
  //! Fills <aLabelMap> with target relocation
  //! labels. <aLabelMap> is not cleared before use.
  Standard_EXPORT void TargetLabelMap (TDF_LabelMap& aLabelMap) const;
  
  //! Fills <anAttributeMap> with target relocation
  //! attributes. <anAttributeMap> is not cleared before
  //! use.
  Standard_EXPORT void TargetAttributeMap (TDF_AttributeMap& anAttributeMap) const;
  
  //! Returns <myLabelTable> to be used or updated.
  Standard_EXPORT TDF_LabelDataMap& LabelTable();
  
  //! Returns <myAttributeTable> to be used or updated.
  Standard_EXPORT TDF_AttributeDataMap& AttributeTable();
  
  //! Returns <myTransientTable> to be used or updated.
  Standard_EXPORT TColStd_IndexedDataMapOfTransientTransient& TransientTable();
  
  //! Dumps the relocation table.
  Standard_EXPORT Standard_OStream& Dump (const Standard_Boolean dumpLabels, const Standard_Boolean dumpAttributes, const Standard_Boolean dumpTransients, Standard_OStream& anOS) const;




  DEFINE_STANDARD_RTTIEXT(TDF_RelocationTable,Standard_Transient)

protected:




private:


  Standard_Boolean mySelfRelocate;
  Standard_Boolean myAfterRelocate;
  TDF_LabelDataMap myLabelTable;
  TDF_AttributeDataMap myAttributeTable;
  TColStd_IndexedDataMapOfTransientTransient myTransientTable;


};







#endif // _TDF_RelocationTable_HeaderFile
