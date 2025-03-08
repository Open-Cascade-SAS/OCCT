// Created on: 1996-09-30
// Created by: Arnaud BOUZY
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef Resource_ConvertUnicode_HeaderFile
#define Resource_ConvertUnicode_HeaderFile

extern "C"
{

  void Resource_sjis_to_unicode(unsigned int* ph, unsigned int* pl);
  void Resource_unicode_to_sjis(unsigned int* ph, unsigned int* pl);
  void Resource_unicode_to_euc(unsigned int* ph, unsigned int* pl);
  void Resource_euc_to_unicode(unsigned int* ph, unsigned int* pl);
  void Resource_gb_to_unicode(unsigned int* ph, unsigned int* pl);
  void Resource_unicode_to_gb(unsigned int* ph, unsigned int* pl);
}

#endif
