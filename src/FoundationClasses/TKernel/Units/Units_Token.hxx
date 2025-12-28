// Created on: 1992-06-22
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

#ifndef _Units_Token_HeaderFile
#define _Units_Token_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TCollection_AsciiString.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Integer.hxx>
class Units_Dimensions;

//! This class defines an elementary word contained in
//! a Sentence object.
class Units_Token : public Standard_Transient
{

public:
  //! Creates and returns a empty token.
  Standard_EXPORT Units_Token();

  //! Creates and returns a token. <aword> is a string
  //! containing the available word.
  Standard_EXPORT Units_Token(const char* aword);

  //! Creates and returns a token. <atoken> is copied in
  //! the returned token.
  Standard_EXPORT Units_Token(const occ::handle<Units_Token>& atoken);

  //! Creates and returns a token. <aword> is a string
  //! containing the available word and <amean> gives the
  //! signification of the token.
  Standard_EXPORT Units_Token(const char* aword, const char* amean);

  //! Creates and returns a token. <aword> is a string
  //! containing the available word, <amean> gives the
  //! signification of the token and <avalue> is the numeric
  //! value of the dimension.
  Standard_EXPORT Units_Token(const char* aword, const char* amean, const double avalue);

  //! Creates and returns a token. <aword> is a string
  //! containing the available word, <amean> gives the
  //! signification of the token, <avalue> is the numeric
  //! value of the dimension, and <adimensions> is the
  //! dimension of the given word <aword>.
  Standard_EXPORT Units_Token(const char*                          aword,
                              const char*                          amean,
                              const double                         avalue,
                              const occ::handle<Units_Dimensions>& adimension);

  //! Creates and returns a token, which is a ShiftedToken.
  Standard_EXPORT virtual occ::handle<Units_Token> Creates() const;

  //! Returns the length of the word.
  Standard_EXPORT int Length() const;

  //! Returns the string <theword>
  TCollection_AsciiString Word() const;

  //! Sets the field <theword> to <aword>.
  void Word(const char* aword);

  //! Returns the significance of the word <theword>, which
  //! is in the field <themean>.
  TCollection_AsciiString Mean() const;

  //! Sets the field <themean> to <amean>.
  void Mean(const char* amean);

  //! Returns the value stored in the field <thevalue>.
  double Value() const;

  //! Sets the field <thevalue> to <avalue>.
  void Value(const double avalue);

  //! Returns the dimensions of the token <thedimensions>.
  occ::handle<Units_Dimensions> Dimensions() const;

  //! Sets the field <thedimensions> to <adimensions>.
  Standard_EXPORT void Dimensions(const occ::handle<Units_Dimensions>& adimensions);

  //! Updates the token <me> with the additional
  //! signification <amean> by concatenation of the two
  //! strings <themean> and <amean>. If the two
  //! significations are the same, an information message
  //! is written in the output device.
  Standard_EXPORT void Update(const char* amean);

  Standard_EXPORT occ::handle<Units_Token> Add(const int aninteger) const;

  //! Returns a token which is the addition of <me> and
  //! another token <atoken>. The addition is possible if
  //! and only if the dimensions are the same.
  Standard_EXPORT occ::handle<Units_Token> Add(const occ::handle<Units_Token>& atoken) const;

  //! Returns a token which is the subtraction of <me> and
  //! another token <atoken>. The subtraction is possible if
  //! and only if the dimensions are the same.
  Standard_EXPORT occ::handle<Units_Token> Subtract(const occ::handle<Units_Token>& atoken) const;

  //! Returns a token which is the product of <me> and
  //! another token <atoken>.
  Standard_EXPORT occ::handle<Units_Token> Multiply(const occ::handle<Units_Token>& atoken) const;

  //! This virtual method is called by the Measurement
  //! methods, to compute the measurement during a
  //! conversion.
  [[nodiscard]] Standard_EXPORT virtual double Multiplied(const double avalue) const;

  //! Returns a token which is the division of <me> by another
  //! token <atoken>.
  Standard_EXPORT occ::handle<Units_Token> Divide(const occ::handle<Units_Token>& atoken) const;

  //! This virtual method is called by the Measurement
  //! methods, to compute the measurement during a
  //! conversion.
  [[nodiscard]] Standard_EXPORT virtual double Divided(const double avalue) const;

  //! Returns a token which is <me> to the power of another
  //! token <atoken>. The computation is possible only if
  //! <atoken> is a dimensionless constant.
  Standard_EXPORT occ::handle<Units_Token> Power(const occ::handle<Units_Token>& atoken) const;

  //! Returns a token which is <me> to the power of <anexponent>.
  Standard_EXPORT occ::handle<Units_Token> Power(const double anexponent) const;

  //! Returns true if the field <theword> and the string
  //! <astring> are the same, false otherwise.
  Standard_EXPORT bool IsEqual(const char* astring) const;

  //! Returns true if the field <theword> and the string
  //! <theword> contained in the token <atoken> are the
  //! same, false otherwise.
  Standard_EXPORT bool IsEqual(const occ::handle<Units_Token>& atoken) const;

  //! Returns false if the field <theword> and the string
  //! <astring> are the same, true otherwise.
  bool IsNotEqual(const char* astring) const;

  //! Returns false if the field <theword> and the string
  //! <theword> contained in the token <atoken> are the
  //! same, true otherwise.
  bool IsNotEqual(const occ::handle<Units_Token>& atoken) const;

  //! Returns true if the field <theword> is strictly
  //! contained at the beginning of the string <astring>,
  //! false otherwise.
  bool IsLessOrEqual(const char* astring) const;

  //! Returns false if the field <theword> is strictly
  //! contained at the beginning of the string <astring>,
  //! true otherwise.
  bool IsGreater(const char* astring) const;

  //! Returns false if the field <theword> is strictly
  //! contained at the beginning of the string <astring>,
  //! true otherwise.
  bool IsGreater(const occ::handle<Units_Token>& atoken) const;

  //! Returns true if the string <astring> is strictly
  //! contained at the beginning of the field <theword>
  //! false otherwise.
  bool IsGreaterOrEqual(const occ::handle<Units_Token>& atoken) const;

  //! Useful for debugging
  Standard_EXPORT virtual void Dump(const int ashift, const int alevel) const;

  DEFINE_STANDARD_RTTIEXT(Units_Token, Standard_Transient)

private:
  TCollection_AsciiString       theword;
  TCollection_AsciiString       themean;
  double                        thevalue;
  occ::handle<Units_Dimensions> thedimensions;
};

#include <Units_Token.lxx>

#endif // _Units_Token_HeaderFile
