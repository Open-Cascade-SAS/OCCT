// Created on: 1999-07-12
// Created by: Denis PASCAL
// Copyright (c) 1999 Matra Datavision
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

#ifndef _TDocStd_Modified_HeaderFile
#define _TDocStd_Modified_HeaderFile

#include <Standard.hxx>

#include <TDF_LabelMap.hxx>
#include <TDF_Attribute.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_OStream.hxx>
class TDF_Label;
class Standard_GUID;
class TDF_RelocationTable;


class TDocStd_Modified;
DEFINE_STANDARD_HANDLE(TDocStd_Modified, TDF_Attribute)

//! Transient attribute which register modified labels.
//! This attribute is attached to root label.
class TDocStd_Modified : public TDF_Attribute
{

public:

  
  //! API class methods
  //! =================
  Standard_EXPORT static Standard_Boolean IsEmpty (const TDF_Label& access);
  
  Standard_EXPORT static Standard_Boolean Add (const TDF_Label& alabel);
  
  Standard_EXPORT static Standard_Boolean Remove (const TDF_Label& alabel);
  
  Standard_EXPORT static Standard_Boolean Contains (const TDF_Label& alabel);
  
  //! if <IsEmpty> raise an exception.
  Standard_EXPORT static const TDF_LabelMap& Get (const TDF_Label& access);
  
  //! remove all modified labels. becomes empty
  Standard_EXPORT static void Clear (const TDF_Label& access);
  
  //! Modified methods
  //! ================
  Standard_EXPORT static const Standard_GUID& GetID();
  
  Standard_EXPORT TDocStd_Modified();
  
  Standard_EXPORT Standard_Boolean IsEmpty() const;
  
  Standard_EXPORT void Clear();
  
  //! add <L> as modified
  Standard_EXPORT Standard_Boolean AddLabel (const TDF_Label& L);
  
  //! remove  <L> as modified
  Standard_EXPORT Standard_Boolean RemoveLabel (const TDF_Label& L);
  
  //! returns modified label map
  Standard_EXPORT const TDF_LabelMap& Get() const;
  
  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;
  
  Standard_EXPORT void Restore (const Handle(TDF_Attribute)& With) Standard_OVERRIDE;
  
  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;
  
  Standard_EXPORT void Paste (const Handle(TDF_Attribute)& Into, const Handle(TDF_RelocationTable)& RT) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_OStream& Dump (Standard_OStream& anOS) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(TDocStd_Modified,TDF_Attribute)

protected:




private:


  TDF_LabelMap myModified;


};







#endif // _TDocStd_Modified_HeaderFile
