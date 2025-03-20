// Created on: 1994-05-02
// Created by: Christian CAILLET
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _IFSelect_AppliedModifiers_HeaderFile
#define _IFSelect_AppliedModifiers_HeaderFile

#include <Standard.hxx>

#include <IFSelect_SequenceOfGeneralModifier.hxx>
#include <Interface_IntList.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Transient.hxx>
#include <TColStd_HSequenceOfInteger.hxx>
class IFSelect_GeneralModifier;

class IFSelect_AppliedModifiers;
DEFINE_STANDARD_HANDLE(IFSelect_AppliedModifiers, Standard_Transient)

//! This class allows to memorize and access to the modifiers
//! which are to be applied to a file. To each modifier, is bound
//! a list of integers (optional) : if this list is absent,
//! the modifier applies to all the file. Else, it applies to the
//! entities designated by these numbers in the produced file.
//!
//! To record a modifier, and a possible list of entity numbers to be applied on:
//! AddModif (amodifier);
//! loop on  AddNum (anumber);
//!
//! To query it,  Count gives the count of recorded modifiers, then for each one:
//! Item (numodif, amodifier, entcount);
//! IsForAll ()  -> can be called, if True, applies on the whole file
//!
//! for (i = 1; i <= entcount; i ++)
//! nument = ItemNum (i);  -> return an entity number
class IFSelect_AppliedModifiers : public Standard_Transient
{
public:
  //! Creates an AppliedModifiers, ready to record up to <nbmax>
  //! modifiers, on a model of <nbent> entities
  Standard_EXPORT IFSelect_AppliedModifiers(const Standard_Integer nbmax,
                                            const Standard_Integer nbent);

  //! Records a modifier. By default, it is to apply on all a
  //! produced file. Further calls to AddNum will restrict this.
  //! Returns True if done, False if too many modifiers are already
  //! recorded
  Standard_EXPORT Standard_Boolean AddModif(const Handle(IFSelect_GeneralModifier)& modif);

  //! Adds a number of entity of the output file to be applied on.
  //! If a sequence of AddNum is called after AddModif, this
  //! Modifier will be applied on the list of designated entities.
  //! Else, it will be applied on all the file
  //! Returns True if done, False if no modifier has yet been added
  Standard_EXPORT Standard_Boolean AddNum(const Standard_Integer nument);

  //! Returns the count of recorded modifiers
  Standard_EXPORT Standard_Integer Count() const;

  //! Returns the description for applied modifier n0 <num> :
  //! the modifier itself, and the count of entities to be applied
  //! on. If no specific list of number has been defined, returns
  //! the total count of entities of the file
  //! If this count is zero, then the modifier applies to all
  //! the file (see below). Else, the numbers are then queried by
  //! calls to ItemNum between 1 and <entcount>
  //! Returns True if OK, False if <num> is out of range
  Standard_EXPORT Standard_Boolean Item(const Standard_Integer            num,
                                        Handle(IFSelect_GeneralModifier)& modif,
                                        Standard_Integer&                 entcount);

  //! Returns a numero of entity to be applied on, given its rank
  //! in the list. If no list is defined (i.e. for all the file),
  //! returns <nument> itself, to give all the entities of the file
  //! Returns 0 if <nument> out of range
  Standard_EXPORT Standard_Integer ItemNum(const Standard_Integer nument) const;

  //! Returns the list of entities to be applied on (see Item)
  //! as a HSequence (IsForAll produces the complete list of all
  //! the entity numbers of the file
  Standard_EXPORT Handle(TColStd_HSequenceOfInteger) ItemList() const;

  //! Returns True if the applied modifier queried by last call to
  //! Item is to be applied to all the produced file.
  //! Else, <entcount> returned by Item gives the count of entity
  //! numbers, each one is queried by ItemNum
  Standard_EXPORT Standard_Boolean IsForAll() const;

  DEFINE_STANDARD_RTTIEXT(IFSelect_AppliedModifiers, Standard_Transient)

private:
  IFSelect_SequenceOfGeneralModifier themodifs;
  Interface_IntList                  thelists;
  Standard_Integer                   thenbent;
  Standard_Integer                   theentcnt;
};

#endif // _IFSelect_AppliedModifiers_HeaderFile
