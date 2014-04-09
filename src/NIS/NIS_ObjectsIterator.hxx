// Created on: 2007-09-02
// Created by: Alexander GRIGORIEV
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

#ifndef NIS_ObjectsIterator_HeaderFile
#define NIS_ObjectsIterator_HeaderFile

#include <NCollection_SparseArray.hxx>
#include <Handle_NIS_InteractiveObject.hxx>

class Handle(NIS_InteractiveContext);

/**
 * Iterator of objects contained in a NIS_InteractiveContext instance. The
 * iteration is always in the ascending sense of object ID. Examples:
 * @code
 *    // Erase all objects in the Context
 * NIS_ObjectsIterator anIter (myContext);
 * for (; anIter.More(); anIter.Next())
 *   myContext->Erase (anIter.Value(), Standard_False);
 *
 *    // Set object attributes to their IDs (hardly useful outside the example)
 * anIter.Initialize (myContext);
 * Handle(NIS_InteractiveObject) anObj;
 * while (!(anObj = anIter.Value().IsNull())) {
 *   anObj->SetAttribute (static_cast<void *> (anObj->ID()));
 *   anIter.Next();
 * }
 * @endcode
 */

class NIS_ObjectsIterator 
{
 public:
  // ---------- PUBLIC METHODS ----------


  /**
   * Empty Constructor.
   */
  inline NIS_ObjectsIterator    () {}

  /**
   * Constructor.
   * @param theCtx
   *   Interactive context that is to be iterated for all objects.
   */
  inline NIS_ObjectsIterator    (const Handle(NIS_InteractiveContext)& theCtx)
  { Initialize (theCtx); }

  /**
   * Reset the Iterator to start the iterations of objects.
   * @param theCtx
   *   Interactive context that is to be iterated for all objects.
   */
  Standard_EXPORT void          Initialize
                                (const Handle(NIS_InteractiveContext)& theCtx);

  /**
   * Query if the Iterator has an object (not yet finished the iteration
   * process).
   */
  inline Standard_Boolean       More    () const
  { return myIter.More(); }

  /**
   * Returns the current object at the iteration pointer. If the iteration is
   * over (More() == False) this method returns NULL Handle.
   */
  Standard_EXPORT const Handle(NIS_InteractiveObject)&
                                Value   () const;

  /**
   * Step forward to the next valid InteractiveObject instance.
   */
  Standard_EXPORT void          Next    ();

 protected:
  // ---------- PROTECTED FIELDS ----------

  NCollection_SparseArray <Handle(NIS_InteractiveObject)>::ConstIterator myIter;

};

#endif
