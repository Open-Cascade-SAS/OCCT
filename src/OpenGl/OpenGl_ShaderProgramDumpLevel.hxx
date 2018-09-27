// Created on: 2018-10-04
// Created by: Maxim NEVROV
// Copyright (c) 2018 OPEN CASCADE SAS
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

#ifndef _OpenGl_ShaderProgramDumpLevel_H__
#define _OpenGl_ShaderProgramDumpLevel_H__

//! Definition of shader programs source code dump levels.
enum OpenGl_ShaderProgramDumpLevel
{
  OpenGl_ShaderProgramDumpLevel_Off,  //!< Disable shader programs source code dump.
  OpenGl_ShaderProgramDumpLevel_Short, //!< Shader programs source code dump in short format (except common declarations).
  OpenGl_ShaderProgramDumpLevel_Full //!< Shader programs source code dump in full format.
};

#endif // _OpenGl_ShaderProgramDumpLevel_H__