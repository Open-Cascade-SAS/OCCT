// File:      LDOM_CDATASection.hxx
// Created:   12.09.01 14:13:44
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2001


#ifndef LDOM_CDATASection_HeaderFile
#define LDOM_CDATASection_HeaderFile

#include <LDOM_Text.hxx>

//  Class LDOM_CDATASection
//

class LDOM_CDATASection : public LDOM_Text
{
 public:
  // ---------- PUBLIC METHODS ----------

  LDOM_CDATASection () {}
  // Empty constructor

  LDOM_CDATASection (const LDOM_CDATASection& theOther): LDOM_Text (theOther) {}
  // Copy constructor

  LDOM_CDATASection&    operator =      (const LDOM_NullPtr * theNull)
        { return (LDOM_CDATASection&) LDOM_CharacterData::operator = (theNull);}
  // Nullify

  LDOM_CDATASection&    operator =      (const LDOM_CDATASection& theOther)
        { return (LDOM_CDATASection&) LDOM_CharacterData::operator= (theOther);}
  // Assignment

 protected:
  friend class LDOM_Document;

  LDOM_CDATASection                     (const LDOM_BasicText&          theText,
                                         const Handle(LDOM_MemManager)& theDoc)
    : LDOM_Text (theText, theDoc) {}
};

#endif
