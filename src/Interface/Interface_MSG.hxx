// Created on: 1995-03-08
// Created by: Christian CAILLET
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

#ifndef _Interface_MSG_HeaderFile
#define _Interface_MSG_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_CString.hxx>
#include <Standard_PCharacter.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <Standard_IStream.hxx>
#include <Standard_OStream.hxx>
#include <Standard_Boolean.hxx>


//! This class gives a set of functions to manage and use a list
//! of translated messages (messagery)
//!
//! Keys are strings, their corresponding (i.e. translated) items
//! are strings, managed by a dictionary (a global one).
//!
//! If the dictionary is not set, or if a key is not recorded,
//! the key is returned as item, and it is possible to :
//! - trace or not this fail, record or not it for further trace
//!
//! It is also possible to suspend the translation (keys are then
//! always returned as items)
//!
//! This class also provides a file format for loading :
//! It is made of couples of lines, the first one begins by '@'
//! the following is the key, the second one is the message
//! Lines which are empty or which begin by '@@' are skipped
class Interface_MSG 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! A MSG is created to write a "functional code" in conjunction
  //! with operator () attached to Value
  //! Then, to have a translated message, write in C++ :
  //!
  //! Interface_MSG("...mykey...")  which returns a CString
  //! See also some help which follow
  Standard_EXPORT Interface_MSG(const Standard_CString key);
  
  //! Translates a message which contains one integer variable
  //! It is just a help which avoid the following :
  //! char mess[100];  sprintf(mess,Interface_MSG("code"),ival);
  //! then  AddFail(mess);
  //! replaced by  AddFail (Interface_MSG("code",ival));
  //!
  //! The basic message is intended to be in  C-sprintf  format,
  //! with one %d form in it
  Standard_EXPORT Interface_MSG(const Standard_CString key, const Standard_Integer i1);
  
  //! Translates a message which contains two integer variables
  //! As for one integer, it is just a writing help
  //!
  //! The basic message is intended to be in  C-sprintf  format
  //! with two %d forms in it
  Standard_EXPORT Interface_MSG(const Standard_CString key, const Standard_Integer i1, const Standard_Integer i2);
  
  //! Translates a message which contains one real variable
  //! <intervals> if set, commands the variable to be rounded to an
  //! interval (see below, method Intervals)
  //! As for one integer, it is just a writing help
  //!
  //! The basic message is intended to be in  C-sprintf  format
  //! with one %f form (or equivalent : %e etc) in it
  Standard_EXPORT Interface_MSG(const Standard_CString key, const Standard_Real r1, const Standard_Integer intervals = -1);
  
  //! Translates a message which contains one string variable
  //! As for one integer, it is just a writing help
  //!
  //! The basic message is intended to be in  C-sprintf  format
  //! with one %s form in it
  Standard_EXPORT Interface_MSG(const Standard_CString key, const Standard_CString str);
  
  //! Translates a message which contains one integer and one
  //! string variables
  //! As for one integer, it is just a writing help
  //! Used for instance to say "Param n0.<ival> i.e. <str> is not.."
  //!
  //! The basic message is intended to be in  C-sprintf  format
  //! with one %d then one %s forms in it
  Standard_EXPORT Interface_MSG(const Standard_CString key, const Standard_Integer ival, const Standard_CString str);
  
  //! Optimised destructor (applies for additional forms of Create)
  Standard_EXPORT void Destroy();
~Interface_MSG()
{
  Destroy();
}
  
  //! Returns the translated message, in a functional form with
  //! operator ()
  //! was C++ : return const
  Standard_EXPORT Standard_CString Value() const;
operator Standard_CString() const;
  
  //! Reads a list of messages from a stream, returns read count
  //! 0 means empty file, -1 means error
  Standard_EXPORT static Standard_Integer Read (Standard_IStream& S);
  
  //! Reads a list of messages from a file defined by its name
  Standard_EXPORT static Standard_Integer Read (const Standard_CString file);
  
  //! Writes the list of messages recorded to be translated, to a
  //! stream. Writes all the list (Default) or only keys which begin
  //! by <rootkey>. Returns the count of written messages
  Standard_EXPORT static Standard_Integer Write (Standard_OStream& S, const Standard_CString rootkey = "");
  
  //! Returns True if a given message is surely a key
  //! (according to the form adopted for keys)
  //! (before activating messages, answer is false)
  Standard_EXPORT static Standard_Boolean IsKey (const Standard_CString mess);
  
  //! Returns the item recorded for a key.
  //! Returns the key itself if :
  //! - it is not recorded (then, the trace system is activated)
  //! - MSG has been required to be hung on
  Standard_EXPORT static Standard_CString Translated (const Standard_CString key);
  
  //! Fills the dictionary with a couple (key-item)
  //! If a key is already recorded, it is possible to :
  //! - keep the last definition, and activate the trace system
  Standard_EXPORT static void Record (const Standard_CString key, const Standard_CString item);
  
  //! Sets the trace system to work when activated, as follow :
  //! - if <toprint>  is True, print immediately on standard output
  //! - if <torecord> is True, record it for further print
  Standard_EXPORT static void SetTrace (const Standard_Boolean toprint, const Standard_Boolean torecord);
  
  //! Sets the main modes for MSG :
  //! - if <running> is True, translation works normally
  //! - if <running> is False, translated item equate keys
  //! - if <raising> is True, errors (from Record or Translate)
  //! cause MSG to raise an exception
  //! - if <raising> is False, MSG runs without exception, then
  //! see also Trace Modes above
  Standard_EXPORT static void SetMode (const Standard_Boolean running, const Standard_Boolean raising);
  
  //! Prints the recorded errors (without title; can be empty, this
  //! is the normally expected case)
  Standard_EXPORT static void PrintTrace (Standard_OStream& S);
  
  //! Returns an "intervalled" value from a starting real <val> :
  //! i.e. a value which is rounded on an interval limit
  //! Interval limits are defined to be in a coarsely "geometric"
  //! progression (two successive intervals are inside a limit ratio)
  //!
  //! <order> gives the count of desired intervals in a range <1-10>
  //! <upper> False, returns the first lower interval (D)
  //! <upper> True,  returns the first upper interval
  //! Values of Intervals according <order> :
  //! 0,1 : 1 10 100 ...
  //! 2   : 1 3 10 30 100 ...
  //! 3(D): 1 2 5 10 20 50 100 ...
  //! 4   : 1 2 3 6 10 20 30 60 100 ...
  //! 6   : 1 1.5 2 3 5 7 10 15 20 ...
  //! 10  : 1 1.2 1.5 2 2.5 3 4 5 6 8 10 12 15 20 25 ...
  Standard_EXPORT static Standard_Real Intervalled (const Standard_Real val, const Standard_Integer order = 3, const Standard_Boolean upper = Standard_False);
  
  //! Codes a date as a text, from its numeric value (-> seconds) :
  //! YYYY-MM-DD:HH-MN-SS  fixed format, completed by leading zeros
  //! Another format can be provided, as follows :
  //! C:%d ...   C like format, preceded by  C:
  //! S:...      format to call system (not yet implemented)
  Standard_EXPORT static void TDate (const Standard_CString text, const Standard_Integer yy, const Standard_Integer mm, const Standard_Integer dd, const Standard_Integer hh, const Standard_Integer mn, const Standard_Integer ss, const Standard_CString format = "");
  
  //! Decodes a date to numeric integer values
  //! Returns True if OK, False if text does not fit with required
  //! format. Incomplete forms are allowed (for instance, for only
  //! YYYY-MM-DD, hour is zero)
  Standard_EXPORT static Standard_Boolean NDate (const Standard_CString text, Standard_Integer& yy, Standard_Integer& mm, Standard_Integer& dd, Standard_Integer& hh, Standard_Integer& mn, Standard_Integer& ss);
  
  //! Returns a value about comparison of two dates
  //! 0 : equal. <0 text1 anterior. >0 text1 posterior
  Standard_EXPORT static Standard_Integer CDate (const Standard_CString text1, const Standard_CString text2);
  
  //! Returns a blank string, of length between 0 and <max>, to fill
  //! the printing of a numeric value <val>, i.e. :
  //! If val < 10 , max-1 blanks
  //! If val between 10 and 99, max-2 blanks  ...   etc...
  Standard_EXPORT static Standard_CString Blanks (const Standard_Integer val, const Standard_Integer max);
  
  //! Returns a blank string, to complete a given string <val> up to
  //! <max> characters :
  //! If strlen(val) is 0, max blanks
  //! If strlen(val) is 5, max-5 blanks    etc...
  Standard_EXPORT static Standard_CString Blanks (const Standard_CString val, const Standard_Integer max);
  
  //! Returns a blank string of <count> blanks (mini 0, maxi 76)
  Standard_EXPORT static Standard_CString Blanks (const Standard_Integer count);
  
  //! Prints a String on an Output Stream, as follows :
  //! Accompanied with blanks, to give up to <max> charis at all,
  //! justified according just :
  //! -1 (D) : left     0 : center    1 : right
  //! Maximum 76 characters
  Standard_EXPORT static void Print (Standard_OStream& S, const Standard_CString val, const Standard_Integer max, const Standard_Integer just = -1);




protected:





private:



  Standard_CString thekey;
  Standard_PCharacter theval;


};







#endif // _Interface_MSG_HeaderFile
