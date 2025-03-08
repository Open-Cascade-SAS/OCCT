// Copyright (c) 2023 OPEN CASCADE SAS
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

#ifndef _DE_PluginHolder_HeaderFile
#define _DE_PluginHolder_HeaderFile

#include <DE_Wrapper.hxx>

//! Base class to work with DE_Wrapper global registration of components.
//! Control life-time of current configuration node.
//! In creating stage load into global configuration.
//! On destroying stage unload from global configuration.
//! Operation to load/unload are thread safety.
template <class TheConfType>
class DE_PluginHolder
{
public:
  DE_PluginHolder()
  {
    Standard_Mutex::Sentry aLock(DE_Wrapper::GlobalLoadMutex());
    myInternalConfiguration = new TheConfType;
    DE_Wrapper::GlobalWrapper()->Bind(myInternalConfiguration);
  }

  ~DE_PluginHolder()
  {
    Standard_Mutex::Sentry aLock(DE_Wrapper::GlobalLoadMutex());
    DE_Wrapper::GlobalWrapper()->UnBind(myInternalConfiguration);
  }

private:
  Handle(TheConfType) myInternalConfiguration; //!< Wrapped object
};

#endif // _DE_PluginHolder_HeaderFile
