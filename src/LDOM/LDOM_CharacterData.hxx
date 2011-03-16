// File:      LDOM_CharacterData.hxx
// Created:   12.09.01 14:53:13
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2001


#ifndef LDOM_CharacterData_HeaderFile
#define LDOM_CharacterData_HeaderFile

#include <LDOM_Node.hxx>

class LDOM_BasicText;

//  Class LDOM_CharacterData
//

class LDOM_CharacterData : public LDOM_Node
{
 public:
  // ---------- PUBLIC METHODS ----------

  LDOM_CharacterData () : myLength (-1) {}
  // Empty constructor

  LDOM_CharacterData (const LDOM_CharacterData& theOther)
    : LDOM_Node (theOther), myLength (-1) {}
  // Copy constructor

  Standard_EXPORT LDOM_CharacterData&
                        operator =      (const LDOM_NullPtr * aNull);
  // Nullify

  Standard_EXPORT LDOM_CharacterData&
                        operator =      (const LDOM_CharacterData& anOther);
  // Assignment

  LDOMString            getData         () const { return getNodeValue(); }
  // Query data

  Standard_EXPORT void  setData         (const LDOMString& aValue);
  // Assign to data

  Standard_EXPORT Standard_Integer
                        getLength       () const;
  // Length of the string

 protected:
  // ---------- PROTECTED METHODS ----------

  LDOM_CharacterData                    (const LDOM_BasicText&          aText,
                                         const Handle(LDOM_MemManager)& aDoc);
 private:
  // ------------ PRIVATE FIELDS -----------
  Standard_Integer      myLength;

};

#endif
