// Created on: 1992-11-18
// Created by: Christian CAILLET
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _IFSelect_SelectRoots_HeaderFile
#define _IFSelect_SelectRoots_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IFSelect_SelectExtract.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
class Interface_EntityIterator;
class Interface_Graph;
class Standard_Transient;
class Interface_InterfaceModel;
class TCollection_AsciiString;


class IFSelect_SelectRoots;
DEFINE_STANDARD_HANDLE(IFSelect_SelectRoots, IFSelect_SelectExtract)

//! A SelectRoots sorts the Entities which are local roots of a
//! set of Entities (not shared by other Entities inside this set,
//! even if they are shared by other Entities outside it)
class IFSelect_SelectRoots : public IFSelect_SelectExtract
{

public:

  
  //! Creates a SelectRoots
  Standard_EXPORT IFSelect_SelectRoots();
  
  //! Returns the list of local roots. It is redefined for a purpose
  //! of effeciency : calling a Sort routine for each Entity would
  //! cost more ressource than to work in once using a Map
  //! RootResult takes in account the Direct status
  Standard_EXPORT virtual Interface_EntityIterator RootResult (const Interface_Graph& G) const Standard_OVERRIDE;
  
  //! Returns always True, because RootResult has done work
  Standard_EXPORT Standard_Boolean Sort (const Standard_Integer rank, const Handle(Standard_Transient)& ent, const Handle(Interface_InterfaceModel)& model) const Standard_OVERRIDE;
  
  //! Returns a text defining the criterium : "Local Root Entities"
  Standard_EXPORT TCollection_AsciiString ExtractLabel() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(IFSelect_SelectRoots,IFSelect_SelectExtract)

protected:

  
  //! Returns True, because RootResult assures uniqueness
  Standard_EXPORT virtual Standard_Boolean HasUniqueResult() const Standard_OVERRIDE;



private:




};







#endif // _IFSelect_SelectRoots_HeaderFile
