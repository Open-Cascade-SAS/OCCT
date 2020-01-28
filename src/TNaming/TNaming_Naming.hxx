// Created on: 1997-09-08
// Created by: Yves FRICAUD
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

#ifndef _TNaming_Naming_HeaderFile
#define _TNaming_Naming_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TNaming_Name.hxx>
#include <TDF_Attribute.hxx>
#include <Standard_Boolean.hxx>
#include <TDF_LabelMap.hxx>
#include <Standard_OStream.hxx>
#include <TDF_AttributeIndexedMap.hxx>
class Standard_GUID;
class TDF_Label;
class TNaming_NamedShape;
class TopoDS_Shape;
class TNaming_Name;
class TDF_Attribute;
class TDF_RelocationTable;
class TDF_DataSet;
class TDF_IDFilter;


class TNaming_Naming;
DEFINE_STANDARD_HANDLE(TNaming_Naming, TDF_Attribute)

//! This attribute  store the  topological  naming of any
//! selected   shape,  when this  shape  is  not  already
//! attached to a specific label. This class is also used
//! to solve  it when  the argumentsof the  toipological
//! naming are modified.
class TNaming_Naming : public TDF_Attribute
{

public:

  
  //! following code from TDesignStd
  //! ==============================
  Standard_EXPORT static const Standard_GUID& GetID();
  
  Standard_EXPORT static Handle(TNaming_Naming) Insert (const TDF_Label& under);
  
  //! Creates  a   Namimg  attribute  at  label <where>   to
  //! identify  the   shape   <Selection>.    Geometry is
  //! Standard_True  if   we  are  only  interested  by  the
  //! underlying   geometry     (e.g.     setting   a
  //! constraint). <Context> is  used to find neighbours  of
  //! <S> when required by the naming.
  //! If KeepOrientation is True the Selection orientation is taken
  //! into  account. BNproblem == True points out that Context sub-shapes
  //! in  DF have orientation differences with Context shape itself.
  //! instance method
  //! ===============
  Standard_EXPORT static Handle(TNaming_NamedShape) Name (const TDF_Label& where, const TopoDS_Shape& Selection, const TopoDS_Shape& Context, const Standard_Boolean Geometry = Standard_False, const Standard_Boolean KeepOrientation = Standard_False, const Standard_Boolean BNproblem = Standard_False);
  
  Standard_EXPORT TNaming_Naming();
  
  Standard_EXPORT Standard_Boolean IsDefined() const;
  
  Standard_EXPORT const TNaming_Name& GetName() const;
  
  Standard_EXPORT TNaming_Name& ChangeName();
  
  //! regenerate only the Name associated to me
  Standard_EXPORT Standard_Boolean Regenerate (TDF_LabelMap& scope);
  
  //! Regenerate recursively the  whole name with scope.  If
  //! scope  is empty it  means that  all the labels  of the
  //! framework are valid.
  Standard_EXPORT Standard_Boolean Solve (TDF_LabelMap& scope);
  
  //! Deferred methods from TDF_Attribute
  //! ===================================
  Standard_EXPORT virtual const Standard_GUID& ID() const Standard_OVERRIDE;
  
  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;
  
  Standard_EXPORT void Restore (const Handle(TDF_Attribute)& With) Standard_OVERRIDE;
  
  Standard_EXPORT void Paste (const Handle(TDF_Attribute)& Into, const Handle(TDF_RelocationTable)& RT) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual void References (const Handle(TDF_DataSet)& aDataSet) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_OStream& Dump (Standard_OStream& anOS) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual void ExtendedDump (Standard_OStream& anOS, const TDF_IDFilter& aFilter, TDF_AttributeIndexedMap& aMap) const Standard_OVERRIDE;
  
  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(TNaming_Naming,TDF_Attribute)

protected:




private:


  TNaming_Name myName;


};







#endif // _TNaming_Naming_HeaderFile
