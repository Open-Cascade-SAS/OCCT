// Created on: 1992-10-26
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

#ifndef _IGESData_ParamCursor_HeaderFile
#define _IGESData_ParamCursor_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>


//! Auxiliary class for ParamReader.
//! It stores commands for a ParamReader to manage the current
//! parameter number. Used by methods Read... from ParamReader.
//! It allows to define the following commands :
//! - read a parameter specified by a precise Number (basic case)
//! - read a parameter then set Current Number to follow its number
//! - read the current parameter (with Current Number) then
//! advance Current Number by one
//! - idem with several : read "nb" parameters from one specified,
//! included, with or without setting Current Number to follow
//! last parameter read
//! - read several parameter from the current one, then advance
//! Current Number to follow the last one read
//! - Read several parameters (as above) but in interlaced lists,
//! i.e. from complex items (each one including successively for
//! instance, an Integer, a Real, an Entity ...)
//!
//! If commands to advance Current Number are not set, it must be
//! set by the user (with method SetCurrent from ParamReader)
//! ParamReader offers methods which create most useful cases
class IGESData_ParamCursor 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates a Cursor to read a precise parameter of ParamReader,
  //! identified by its number, then set Current Number to "num + 1"
  //! (this constructor allows to simply give a Number to a method
  //! Read... from ParamReader, which will be translated into a
  //! ParamCursor by compiler)
  Standard_EXPORT IGESData_ParamCursor(const Standard_Integer num);
  
  //! Creates a Cursor to read a list of parameters (count "nb")
  //! starting from a precise one (number "num") included, then
  //! set Current Number of ParamNumber to the first following one
  //! ("num + nb")
  //! If size is given, it means that each parameter is made of more
  //! than one term. One term is the normal (default) case : for
  //! instance, a Parameter comprises one Integer, or one Entity ...
  //! Size gives the complete size of each Item if it is complex.
  //! To be used ONLY IF it is constant
  Standard_EXPORT IGESData_ParamCursor(const Standard_Integer num, const Standard_Integer nb, const Standard_Integer size = 1);
  
  //! Defines the size of a term to read in the item : this commands
  //! ParamReader to read "size" parameters for each item, then
  //! skip the remainder of the item to the same term of next Item
  //! (that is, skip "item size" - "term size")
  //!
  //! In addition, Offset from beginning of Item is managed :
  //! After being created, and for the first call to SetTerm, the
  //! part of Item to be read begins exactly as the Item begins
  //! But after a SetTerm, the next read will add an offset which is
  //! the size of former term.
  //!
  //! autoadv commands Advance management. If it is True (default),
  //! the last SetTerm (Item size has been covered) calls SetAdvance
  //! If it is False, SetAdvance must be called directly if necessary
  //!
  //! Error if a SetTerm overpasses the size of the Item
  Standard_EXPORT void SetTerm (const Standard_Integer size, const Standard_Boolean autoadv = Standard_True);
  
  //! Defines a term of one Parameter (very current case)
  Standard_EXPORT void SetOne (const Standard_Boolean autoadv = Standard_True);
  
  //! Defines a term of two Parameters for a XY (current case)
  Standard_EXPORT void SetXY (const Standard_Boolean autoadv = Standard_True);
  
  //! Defines a term of three Parameters for XYZ (current case)
  Standard_EXPORT void SetXYZ (const Standard_Boolean autoadv = Standard_True);
  
  //! Changes command to advance current cursor after reading
  //! parameters. If "advance" True, sets advance, if "False",
  //! resets it. ParamCursor is created by default with True.
  Standard_EXPORT void SetAdvance (const Standard_Boolean advance);
  
  //! Returns (included) starting number for reading parameters
    Standard_Integer Start() const;
  
  //! Returns (excluded) upper limit number for reading parameters
    Standard_Integer Limit() const;
  
  //! Returns required count of items to be read
    Standard_Integer Count() const;
  
  //! Returns length of item (count of parameters per item)
    Standard_Integer ItemSize() const;
  
  //! Returns length of current term (count of parameters) in item
    Standard_Integer TermSize() const;
  
  //! Returns offset from which current term must be read in item
    Standard_Integer Offset() const;
  
  //! Returns True if Advance command has been set
    Standard_Boolean Advance() const;




protected:





private:



  Standard_Integer thestart;
  Standard_Integer thelimit;
  Standard_Integer thecount;
  Standard_Integer theisize;
  Standard_Integer theoffst;
  Standard_Integer thetsize;
  Standard_Boolean theadv;


};


#include <IGESData_ParamCursor.lxx>





#endif // _IGESData_ParamCursor_HeaderFile
