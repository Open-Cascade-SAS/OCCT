// Created on: 1995-03-21
// Created by: Jean-Louis Frenkel
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _AIS_Selection_HeaderFile
#define _AIS_Selection_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <AIS_NListTransient.hxx>
#include <AIS_NListIteratorOfListTransient.hxx>
#include <AIS_NDataMapOfTransientIteratorOfListTransient.hxx>
#include <Standard_Integer.hxx>
#include <MMgt_TShared.hxx>
#include <Standard_CString.hxx>
#include <Standard_Boolean.hxx>
#include <AIS_SelectStatus.hxx>
class Standard_NoSuchObject;
class Standard_MultiplyDefined;
class Standard_TypeMismatch;
class Standard_Transient;


class AIS_Selection;
DEFINE_STANDARD_HANDLE(AIS_Selection, MMgt_TShared)


class AIS_Selection : public MMgt_TShared
{

public:

  
  //! creates a new selection.

  Standard_EXPORT AIS_Selection();
  
  //! removes all the object of the selection.
  Standard_EXPORT void Select();
  
  //! if the object is not yet in the selection, it will be added.
  //! if the object is already in the selection, it will be removed.
  Standard_EXPORT AIS_SelectStatus Select (const Handle(Standard_Transient)& anObject);
  
  //! the object is always add int the selection.
  //! faster when the number of objects selected is great.
  Standard_EXPORT AIS_SelectStatus AddSelect (const Handle(Standard_Transient)& anObject);
  
  //! clears the selection and adds the object in the selection.
  Standard_EXPORT void ClearAndSelect (const Handle(Standard_Transient)& anObject);

  //! checks if the object is in the selection.
  Standard_EXPORT Standard_Boolean IsSelected (const Handle(Standard_Transient)& anObject) const;
  
  //! returns the number of objects selected.
  Standard_EXPORT Standard_Integer Extent() const;
  
  //! returns the single object selected.
  //! Warning: raises TypeMismatch from Standard if Extent is not equal to 1.
  Standard_EXPORT Handle(Standard_Transient) Single();
  
  void Init();
  
  Standard_Boolean More() const;
  
  void Next();
  
  const Handle(Standard_Transient)& Value() const;
  
  Standard_Integer NbStored() const;
  
  const AIS_NListTransient& Objects() const;

  DEFINE_STANDARD_RTTIEXT(AIS_Selection,MMgt_TShared)

protected:




private:

  AIS_NListTransient myresult;
  AIS_NListIteratorOfListTransient myIterator;
  AIS_NDataMapOfTransientIteratorOfListTransient myResultMap;
  Standard_Integer myNb;


};


#include <AIS_Selection.lxx>





#endif // _AIS_Selection_HeaderFile
