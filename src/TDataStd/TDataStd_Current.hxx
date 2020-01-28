// Created on: 1999-08-02
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

#ifndef _TDataStd_Current_HeaderFile
#define _TDataStd_Current_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TDF_Label.hxx>
#include <TDF_Attribute.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_OStream.hxx>
class Standard_GUID;
class TDF_Label;
class TDF_Attribute;
class TDF_RelocationTable;


class TDataStd_Current;
DEFINE_STANDARD_HANDLE(TDataStd_Current, TDF_Attribute)

//! this attribute,  located at root label,  manage an
//! access to a current label.
class TDataStd_Current : public TDF_Attribute
{

public:

  
  //! class methods
  //! =============
  Standard_EXPORT static const Standard_GUID& GetID();
  
  //! Set <L> as current of <L> Framework.
  Standard_EXPORT static void Set (const TDF_Label& L);
  
  //! returns current of <acces> Framework. raise if (!Has)
  Standard_EXPORT static TDF_Label Get (const TDF_Label& acces);
  
  //! returns True if a  current label is managed in <acces>
  //! Framework.
  //! class methods
  //! =============
  Standard_EXPORT static Standard_Boolean Has (const TDF_Label& acces);
  
  Standard_EXPORT TDataStd_Current();
  
  Standard_EXPORT void SetLabel (const TDF_Label& current);
  
  Standard_EXPORT TDF_Label GetLabel() const;
  
  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;
  
  Standard_EXPORT void Restore (const Handle(TDF_Attribute)& With) Standard_OVERRIDE;
  
  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;
  
  Standard_EXPORT void Paste (const Handle(TDF_Attribute)& Into, const Handle(TDF_RelocationTable)& RT) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_OStream& Dump (Standard_OStream& anOS) const Standard_OVERRIDE;
  
  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(TDataStd_Current,TDF_Attribute)

protected:




private:


  TDF_Label myLabel;


};







#endif // _TDataStd_Current_HeaderFile
