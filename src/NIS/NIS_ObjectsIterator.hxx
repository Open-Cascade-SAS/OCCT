// Created on: 2007-09-02
// Created by: Alexander GRIGORIEV
// Copyright (c) 2007-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



#ifndef NIS_ObjectsIterator_HeaderFile
#define NIS_ObjectsIterator_HeaderFile

#include <NCollection_SparseArray.hxx>
#include <Handle_NIS_InteractiveObject.hxx>

class Handle_NIS_InteractiveContext;

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
  inline NIS_ObjectsIterator    (const Handle_NIS_InteractiveContext& theCtx)
  { Initialize (theCtx); }

  /**
   * Reset the Iterator to start the iterations of objects.
   * @param theCtx
   *   Interactive context that is to be iterated for all objects.
   */
  Standard_EXPORT void          Initialize
                                (const Handle_NIS_InteractiveContext& theCtx);

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
  Standard_EXPORT const Handle_NIS_InteractiveObject&
                                Value   () const;

  /**
   * Step forward to the next valid InteractiveObject instance.
   */
  Standard_EXPORT void          Next    ();

 protected:
  // ---------- PROTECTED FIELDS ----------

  NCollection_SparseArray <Handle_NIS_InteractiveObject>::ConstIterator myIter;

};

#endif
