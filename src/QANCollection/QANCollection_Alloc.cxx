// Created on: 2004-03-05
// Created by: Mikhail KUZMITCHEV
// Copyright (c) 2004-2014 OPEN CASCADE SAS
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

#include <QANCollection.hxx>
#include <Draw_Interpretor.hxx>

#include <NCollection_StdAllocator.hxx>
#include <NCollection_IncAllocator.hxx>
#include <Standard_Assert.hxx>

#include <list>
#include <vector>

//=======================================================================
//function : QANColStdAllocator1
//purpose  :
//=======================================================================
static Standard_Integer QANColStdAllocator1(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if ( argc != 1) {
    di << "Usage : " << argv[0] << "\n";
    return 1;
  }

  //type definitions
  typedef Handle(Standard_Transient) elem_type;
  typedef NCollection_StdAllocator<elem_type> allocator_type;
  Standard_STATIC_ASSERT (sizeof (allocator_type::value_type) == sizeof (elem_type));
  Standard_STATIC_ASSERT (sizeof (allocator_type::pointer) == sizeof (void*));
  Standard_STATIC_ASSERT (sizeof (allocator_type::const_pointer) == sizeof (void*));

  elem_type aDummy;
  allocator_type::reference aRef = aDummy;
  (void)aRef; // avoid compiler warning on unused
  allocator_type::const_reference aConstRef = aDummy;
  (void)aConstRef; // avoid compiler warning on unused
  Standard_STATIC_ASSERT (sizeof (allocator_type::size_type) == sizeof (size_t));
  Standard_STATIC_ASSERT (sizeof (allocator_type::difference_type) == sizeof (ptrdiff_t));

  typedef int other_elem_type;
  Standard_STATIC_ASSERT (sizeof (allocator_type::rebind<other_elem_type>::other::value_type) == sizeof (other_elem_type));

  return 0;
}

//=======================================================================
//function : QANColStdAllocator2
//purpose  :
//=======================================================================
static Standard_Integer QANColStdAllocator2(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if ( argc != 1) {
    di << "Usage : " << argv[0] << "\n";
    return 1;
  }

  //create incremental allocator outside the scope of objects it will manage
  Handle(NCollection_IncAllocator) anIncAlloc = new NCollection_IncAllocator();

  {
    //typed allocator
    NCollection_StdAllocator<int> aSAlloc (anIncAlloc);
    std::list<int, NCollection_StdAllocator<int> > aL (aSAlloc);
    aL.push_back (2);
    if ( aL.size() == size_t (1) ) {
      di << "Test1 : OK\n";
    } else {
      di << "Test1 : Error\n";
    }

    //type cast
    NCollection_StdAllocator<char> aCAlloc;
    std::vector<int, NCollection_StdAllocator<int> > aV (aCAlloc);
    aV.push_back (1);
    if ( aV.size() == size_t (1) ) {
      di << "Test2 : OK\n";
    } else {
      di << "Test2 : Error\n";
    }

    //using void-specialization allocator
    NCollection_StdAllocator<void> aVAlloc;
    std::vector<int, NCollection_StdAllocator<int> > aV2 (aVAlloc);

    aV2.resize (10);
    aV2.push_back (-1);
    if ( aV2.size() == size_t (11) ) {
      di << "Test3 : OK\n";
    } else {
      di << "Test3 : Error\n";
    }

    //equality of allocators
    if ( aSAlloc != aCAlloc ) {
      di << "Test4 : OK\n";
    } else {
      di << "Test4 : Error\n";
    }
    NCollection_StdAllocator<int> anIAlloc (anIncAlloc);
    if ( aSAlloc == anIAlloc ) {
      di << "Test5 : OK\n";
    } else {
      di << "Test5 : Error\n";
    }

  }

  return 0;
}

void QANCollection::CommandsAlloc(Draw_Interpretor& theCommands) {
  const char *group = "QANCollection";

  theCommands.Add("QANColStdAllocator1", "QANColStdAllocator1", __FILE__, QANColStdAllocator1, group);
  theCommands.Add("QANColStdAllocator2", "QANColStdAllocator2", __FILE__, QANColStdAllocator2, group);

  return;
}
