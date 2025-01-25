// Created on: 1992-11-05
// Created by: Gilles DEBARBOUILLE
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

#ifndef _Units_ShiftedToken_HeaderFile
#define _Units_ShiftedToken_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Units_Token.hxx>
#include <Standard_Integer.hxx>
class Units_Dimensions;

class Units_ShiftedToken;
DEFINE_STANDARD_HANDLE(Units_ShiftedToken, Units_Token)

//! The  ShiftedToken class  inherits   from Token and
//! describes tokens which have  a gap in  addition of
//! the  multiplicative factor.   This kind  of  token
//! allows  the  description of linear functions which
//! do not pass through the origin, of the form :
//!
//! y = ax  +b
//!
//! where <x> and  <y>  are the unknown variables, <a>
//! the mutiplicative factor, and <b> the gap relative
//! to the ordinate axis.
//!
//! An example is the  translation between the  Celsius
//! and Fahrenheit degree of temperature.
class Units_ShiftedToken : public Units_Token
{

public:
  //! Creates and returns a  shifted   token.  <aword> is  a
  //! string containing the   available word, <amean>  gives
  //! the signification   of the   token,  <avalue> is   the
  //! numeric value  of the  dimension, <amove> is  the gap,
  //! and <adimensions> is  the dimension of the given  word
  //! <aword>.
  Standard_EXPORT Units_ShiftedToken(const Standard_CString          aword,
                                     const Standard_CString          amean,
                                     const Standard_Real             avalue,
                                     const Standard_Real             amove,
                                     const Handle(Units_Dimensions)& adimensions);

  //! Creates and returns a  token, which is a ShiftedToken.
  Standard_EXPORT virtual Handle(Units_Token) Creates() const Standard_OVERRIDE;

  //! Returns the gap <themove>
  Standard_EXPORT Standard_Real Move() const;

  //! This  virtual   method  is  called  by the Measurement
  //! methods,  to   compute  the   measurement    during  a
  //! conversion.
  Standard_EXPORT virtual Standard_Real Multiplied(const Standard_Real avalue) const
    Standard_OVERRIDE;

  //! This   virtual  method is  called  by  the Measurement
  //! methods,   to   compute   the   measurement   during a
  //! conversion.
  Standard_EXPORT virtual Standard_Real Divided(const Standard_Real avalue) const Standard_OVERRIDE;

  Standard_EXPORT virtual void Dump(const Standard_Integer ashift,
                                    const Standard_Integer alevel) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(Units_ShiftedToken, Units_Token)

protected:
private:
  Standard_Real themove;
};

#endif // _Units_ShiftedToken_HeaderFile
