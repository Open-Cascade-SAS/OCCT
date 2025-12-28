// Created on: 2002-11-20
// Created by: Vladimir ANIKIN
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

//=================================================================================================

inline int TDocStd_MultiTransactionManager::GetUndoLimit() const
{
  return myUndoLimit;
}

//=================================================================================================

inline const NCollection_Sequence<occ::handle<TDocStd_ApplicationDelta>>&
  TDocStd_MultiTransactionManager::GetAvailableUndos() const
{
  return myUndos;
}

//=================================================================================================

inline const NCollection_Sequence<occ::handle<TDocStd_ApplicationDelta>>&
  TDocStd_MultiTransactionManager::GetAvailableRedos() const
{
  return myRedos;
}

//=================================================================================================

inline const NCollection_Sequence<occ::handle<TDocStd_Document>>& TDocStd_MultiTransactionManager::
  Documents() const
{
  return myDocuments;
}

//=================================================================================================

inline bool TDocStd_MultiTransactionManager::IsNestedTransactionMode() const
{
  return myIsNestedTransactionMode;
}

//=================================================================================================

inline bool TDocStd_MultiTransactionManager::HasOpenCommand() const
{
  return myOpenTransaction;
}

//=======================================================================
// function : ModificationMode
// purpose  : returns True if changes allowed only inside transactions
//=======================================================================

inline bool TDocStd_MultiTransactionManager::ModificationMode() const
{
  return myOnlyTransactionModification;
}
