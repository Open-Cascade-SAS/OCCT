// Created on: 1992-07-28
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

#ifndef _Dico_DictionaryOfTransient_HeaderFile
#define _Dico_DictionaryOfTransient_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Character.hxx>
#include <MMgt_TShared.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_CString.hxx>
#include <Standard_Size.hxx>
#include <Standard_Integer.hxx>
class Standard_Transient;
class Standard_NoSuchObject;
class Dico_IteratorOfDictionaryOfTransient;
class Dico_StackItemOfDictionaryOfTransient;
class TCollection_AsciiString;


class Dico_DictionaryOfTransient;
DEFINE_STANDARD_HANDLE(Dico_DictionaryOfTransient, MMgt_TShared)


class Dico_DictionaryOfTransient : public MMgt_TShared
{

public:

  
  //! Creates a Dictionary cell.
  //! A Dictionary is always created then handled by its first cell
  //! After creating a Dictionary, user can call methods HasItem,
  //! Item, SetItem ... (internal cells and entries are also
  //! defined as objects from the same class)
  //! Also iteration can be made, by an Iterator on this Dictionary
  Standard_EXPORT Dico_DictionaryOfTransient();
  
  //! Returns True if an Item is bound to a Name in the Dictionnary
  //! <exact> : if True, commands exact matching
  //! if False, accept completion, only if ONE AND ONLY ONE
  //! Dictionnary Entry has <name> as beginning of its name
  Standard_EXPORT Standard_Boolean HasItem (const Standard_CString name, const Standard_Boolean exact = Standard_False) const;
  
  //! Works as above method but accepts a String from TCollection
  Standard_EXPORT Standard_Boolean HasItem (const TCollection_AsciiString& name, const Standard_Boolean exact = Standard_True) const;
  
  //! Returns item bound to a name in the Dictionnary
  //! <exact> : same as for HasItem
  Standard_EXPORT const Handle(Standard_Transient)& Item (const Standard_CString name, const Standard_Boolean exact = Standard_True) const;
  
  //! Works as above method but accepts a String from TCollection
  Standard_EXPORT const Handle(Standard_Transient)& Item (const TCollection_AsciiString& name, const Standard_Boolean exact = Standard_True) const;
  
  //! Gathers HasItem and Item, in a less regular but faster way
  //! If return is True, <anitem> is returned too, else it is not
  //! <exact> : same as for HasItem
  Standard_EXPORT Standard_Boolean GetItem (const Standard_CString name, Handle(Standard_Transient)& anitem, const Standard_Boolean exact = Standard_True) const;
  
  //! Works as above method but accepts a String from TCollection
  Standard_EXPORT Standard_Boolean GetItem (const TCollection_AsciiString& name, Handle(Standard_Transient)& anitem, const Standard_Boolean exact = Standard_True) const;
  
  //! Binds an item to a dictionnary entry
  //! If <name> is already known in the dictionary, its value
  //! is changed. Else, the dictionary entry is created.
  //! If <exact> is given False, completion is tried, it is accepted
  //! If it gives a UNIQUE entry : hence this one will be modified
  //! Else, new entry is created with the exact name given
  Standard_EXPORT void SetItem (const Standard_CString name, const Handle(Standard_Transient)& anitem, const Standard_Boolean exact = Standard_True);
  
  //! Works as above method but accepts a String from TCollection
  Standard_EXPORT void SetItem (const TCollection_AsciiString& name, const Handle(Standard_Transient)& anitem, const Standard_Boolean exact = Standard_True);
  
  //! Returns the Item AS AN ADDRESS which corresponds to a Name,
  //! in order to be changed or set.
  //! If this name is not yet recorded, the Dictionary creates it.
  //! <isvalued> is returned True if the Item is recorded in the
  //! Dictionary, False else, in that case the Item is reserved and
  //! the name is noted as beeing valued now.
  Standard_EXPORT Handle(Standard_Transient)& NewItem (const Standard_CString name, Standard_Boolean& isvalued, const Standard_Boolean exact = Standard_True);
  
  //! Works as above method but accepts a String from TCollection
  Standard_EXPORT Handle(Standard_Transient)& NewItem (const TCollection_AsciiString& name, Standard_Boolean& isvalued, const Standard_Boolean exact = Standard_True);
  
  //! Removes a dictionary entry given by its name then Returns True
  //! If the entry does not exists, Does nothing then Returns False
  //! <exact> : as for HasItem, if completion works, the found entry
  //! is removed (else returned value is False)
  //! <cln> commands cleaning dictionary (to recover memory space)
  //! For an isolated call, it is recommanded to give it at True
  //! For a sequence of calls, rather give False, then call Clean
  Standard_EXPORT Standard_Boolean RemoveItem (const Standard_CString name, const Standard_Boolean cln = Standard_True, const Standard_Boolean exact = Standard_True);
  
  //! Works as above method but accepts a String from TCollection
  Standard_EXPORT Standard_Boolean RemoveItem (const TCollection_AsciiString& name, const Standard_Boolean cln = Standard_True, const Standard_Boolean exact = Standard_True);
  
  //! Deletes physically in one step the entries which were removed
  //! (can be used for a more efficient Memory Management : first
  //! Remove several Items (<cln> = False), then Clean the Memory)
  Standard_EXPORT void Clean();
  
  //! Returns True if no Item is recorded
  Standard_EXPORT Standard_Boolean IsEmpty() const;
  
  //! Clears all the Dictionary : all recorded Items are removed
  Standard_EXPORT void Clear();
  
  //! Copies the Dictionary as a Tree, without Copying the Items
  Standard_EXPORT Handle(Dico_DictionaryOfTransient) Copy() const;
  
  //! Internal routine used for completion (returns True if success)
  Standard_EXPORT Standard_Boolean Complete (Handle(Dico_DictionaryOfTransient)& acell) const;


friend class Dico_IteratorOfDictionaryOfTransient;


  DEFINE_STANDARD_RTTI(Dico_DictionaryOfTransient,MMgt_TShared)

protected:




private:

  
  //! Defines cell's character (internal use, to build dict. tree)
  Standard_EXPORT void SetChar (const Standard_Character car);
  
  //! Returns True if this cell has a subcell
  Standard_EXPORT Standard_Boolean HasSub() const;
  
  //! Returns subcell
  Standard_EXPORT Handle(Dico_DictionaryOfTransient) Sub() const;
  
  //! Returns True if this cell has a next cell
  Standard_EXPORT Standard_Boolean HasNext() const;
  
  //! Returns next cell
  Standard_EXPORT Handle(Dico_DictionaryOfTransient) Next() const;
  
  //! Defines subcell
  Standard_EXPORT void SetSub (const Handle(Dico_DictionaryOfTransient)& acell);
  
  //! Defines next cell
  Standard_EXPORT void SetNext (const Handle(Dico_DictionaryOfTransient)& acell);
  
  //! Internal method used to get an entry from a given name
  Standard_EXPORT void SearchCell (const Standard_CString name, const Standard_Size lmax, const Standard_Character car, const Standard_Size level, Handle(Dico_DictionaryOfTransient)& acell, Standard_Size& reslev, Standard_Integer& stat) const;
  
  //! Internal method used to create a new entry for a name
  Standard_EXPORT void NewCell (const Standard_CString name, const Standard_Size namlen, Handle(Dico_DictionaryOfTransient)& acell, const Standard_Size reslev, const Standard_Integer stat);
  
  //! Returns True if a cell has an associated item value
  Standard_EXPORT Standard_Boolean HasIt() const;
  
  //! Returns item value associated to a cell
  Standard_EXPORT const Handle(Standard_Transient)& It() const;
  
  //! Returns item address associated to a cell
  Standard_EXPORT Handle(Standard_Transient)& ItAdr();
  
  //! Binds an item value to a cell
  Standard_EXPORT void SetIt (const Handle(Standard_Transient)& anitem);
  
  //! Declares a cell as Valued : used by NewItem (when an Item
  //! is created if it did not exist and is returned)
  Standard_EXPORT void DeclIt();
  
  //! Removes item bound to a cell (cancels effect of DeclIt)
  Standard_EXPORT void RemoveIt();
  
  //! Returns cell's character as a node feature
  Standard_EXPORT Standard_Character CellChar() const;
  
  //! Performs Copy from an original <fromcell> to <me>
  //! Called by Copy
  Standard_EXPORT void GetCopied (const Handle(Dico_DictionaryOfTransient)& fromcell);

  Standard_Character thecars[4];
  Handle(Dico_DictionaryOfTransient) thesub;
  Handle(Dico_DictionaryOfTransient) thenext;
  Handle(Standard_Transient) theitem;


};







#endif // _Dico_DictionaryOfTransient_HeaderFile
