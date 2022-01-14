// Created on: 1994-04-15
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

#ifndef _Interface_FloatWriter_HeaderFile
#define _Interface_FloatWriter_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>


//! This class converts a floating number (Real) to a string
//! It can be used if the standard C-C++ output functions
//! (sprintf or std::cout<<) are not convenient. That is to say :
//! - to suppress trailing '0' and 'E+00' (if desired)
//! - to control exponent output and floating point output
//!
//! Formats are given in the form used by printf-sprintf
class Interface_FloatWriter 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates a FloatWriter ready to work, with default options  -
  //! - zero suppress option is set
  //! - main format is set to "%E"
  //! - secondary format is set to "%f" for values between 0.1 and
  //! 1000. in absolute values
  //! If <chars> is given (and positive), it will produce options
  //! to produce this count of characters : "%<chars>f","%<chars>%E"
  Standard_EXPORT Interface_FloatWriter(const Standard_Integer chars = 0);
  
  //! Sets a specific Format for Sending Reals (main format)
  //! (Default from Creation is "%E")
  //! If <reset> is given True (default), this call clears effects
  //! of former calls to SetFormatForRange and SetZeroSuppress
  Standard_EXPORT void SetFormat (const Standard_CString form, const Standard_Boolean reset = Standard_True);
  
  //! Sets a secondary Format for Real, to be applied between R1 and
  //! R2 (in absolute values). A Call to SetRealForm cancels this
  //! secondary form if <reset> is True.
  //! (Default from Creation is "%f" between 0.1 and 1000.)
  //! Warning : if the condition (0. <= R1 < R2) is not fulfilled, this
  //! secondary form is canceled.
  Standard_EXPORT void SetFormatForRange (const Standard_CString form, const Standard_Real R1, const Standard_Real R2);
  
  //! Sets Sending Real Parameters to suppress trailing Zeros and
  //! Null Exponent ("E+00"), if <mode> is given True, Resets this
  //! mode if <mode> is False (in addition to Real Forms)
  //! A call to SetRealFrom resets this mode to False ig <reset> is
  //! given True (Default from Creation is True)
  Standard_EXPORT void SetZeroSuppress (const Standard_Boolean mode);
  
  //! Sets again options to the defaults given by Create
  Standard_EXPORT void SetDefaults (const Standard_Integer chars = 0);
  
  //! Returns active options : <zerosup> is the option ZeroSuppress,
  //! <range> is True if a range is set, False else
  //! R1,R2 give the range (if it is set)
  Standard_EXPORT void Options (Standard_Boolean& zerosup, Standard_Boolean& range, Standard_Real& R1, Standard_Real& R2) const;
  
  //! Returns the main format
  //! was C++ : return const
  Standard_EXPORT Standard_CString MainFormat() const;
  
  //! Returns the format for range, if set
  //! Meaningful only if <range> from Options is True
  //! was C++ : return const
  Standard_EXPORT Standard_CString FormatForRange() const;
  
  //! Writes a Real value <val> to a string <text> by using the
  //! options. Returns the useful Length of produced string.
  //! It calls the class method Convert.
  //! Warning : <text> is assumed to be wide enough (20-30 is correct)
  //! And, even if declared in, its content will be modified
  Standard_EXPORT Standard_Integer Write (const Standard_Real val, const Standard_CString text) const;
  
  //! This class method converts a Real Value to a string, given
  //! options given as arguments. It can be called independently.
  //! Warning : even if declared in, content of <text> will be modified
  Standard_EXPORT static Standard_Integer Convert (const Standard_Real val, const Standard_CString text, const Standard_Boolean zerosup, const Standard_Real Range1, const Standard_Real Range2, const Standard_CString mainform, const Standard_CString rangeform);




protected:





private:



  Standard_Character themainform[12];
  Standard_Real therange1;
  Standard_Real therange2;
  Standard_Character therangeform[12];
  Standard_Boolean thezerosup;


};







#endif // _Interface_FloatWriter_HeaderFile
