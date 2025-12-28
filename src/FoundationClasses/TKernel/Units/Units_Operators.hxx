// Created on: 2007-05-14
// Created by: data exchange team
// Copyright (c) 2007-2014 OPEN CASCADE SAS
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

#ifndef Units_Operators_HeaderFile
#define Units_Operators_HeaderFile

#include <Units_Token.hxx>
#include <Units_Quantity.hxx>

Standard_EXPORT bool operator==(const occ::handle<Units_Quantity>&, const char*);
Standard_EXPORT bool operator==(const occ::handle<Units_Token>&, const char*);
Standard_EXPORT bool operator==(const occ::handle<Units_Unit>&, const char*);

Standard_EXPORT occ::handle<Units_Dimensions> operator*(const occ::handle<Units_Dimensions>&,
                                                   const occ::handle<Units_Dimensions>&);
Standard_EXPORT occ::handle<Units_Dimensions> operator/(const occ::handle<Units_Dimensions>&,
                                                   const occ::handle<Units_Dimensions>&);

Standard_EXPORT occ::handle<Units_Dimensions> pow(const occ::handle<Units_Dimensions>&, const double);
Standard_EXPORT occ::handle<Units_Token> pow(const occ::handle<Units_Token>&, const occ::handle<Units_Token>&);
Standard_EXPORT occ::handle<Units_Token> pow(const occ::handle<Units_Token>&, const double);

Standard_EXPORT occ::handle<Units_Token> operator+(const occ::handle<Units_Token>&, const int);
Standard_EXPORT occ::handle<Units_Token> operator+(const occ::handle<Units_Token>&,
                                              const occ::handle<Units_Token>&);
Standard_EXPORT occ::handle<Units_Token> operator-(const occ::handle<Units_Token>&,
                                              const occ::handle<Units_Token>&);
Standard_EXPORT occ::handle<Units_Token> operator*(const occ::handle<Units_Token>&,
                                              const occ::handle<Units_Token>&);
Standard_EXPORT occ::handle<Units_Token> operator/(const occ::handle<Units_Token>&,
                                              const occ::handle<Units_Token>&);

Standard_EXPORT bool operator!=(const occ::handle<Units_Token>&, const char*);
Standard_EXPORT bool operator<=(const occ::handle<Units_Token>&, const char*);
Standard_EXPORT bool operator>(const occ::handle<Units_Token>&, const char*);
Standard_EXPORT bool operator>(const occ::handle<Units_Token>&, const occ::handle<Units_Token>&);
Standard_EXPORT bool operator>=(const occ::handle<Units_Token>&, const occ::handle<Units_Token>&);

#endif
