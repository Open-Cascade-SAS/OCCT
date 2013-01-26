// Created on: 2004-03-05
// Created by: Mikhail KUZMITCHEV
// Copyright (c) 2004-2012 OPEN CASCADE SAS
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



#include <QANCollection.hxx>
#include <Draw_Interpretor.hxx>

#include <NCollection_StdAllocator.hxx>
#include <NCollection_IncAllocator.hxx>
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
  typedef Handle_Standard_Transient elem_type;
  typedef NCollection_StdAllocator<elem_type> allocator_type;
  if ( sizeof (allocator_type::value_type) == sizeof (elem_type) ) {
    di << "value_type : OK\n";
  } else {
    di << "value_type : Error\n";
  }
  if ( sizeof (allocator_type::pointer) == sizeof (void*) ) {
    di << "pointer : OK\n";
  } else {
    di << "pointer : Error\n";
  }
  if (sizeof (allocator_type::const_pointer)  == sizeof (void*) ) {
    di << "const_pointer : OK\n";
  } else {
    di << "const_pointer : Error\n";
  }

  elem_type aDummy;
  allocator_type::reference aRef = aDummy;
  allocator_type::const_reference aConstRef = aDummy;
  if ( sizeof (allocator_type::size_type) == sizeof (size_t) ) {
    di << "size_type : OK\n";
  } else {
    di << "size_type : Error\n";
  }
  if ( sizeof (allocator_type::difference_type) == sizeof (ptrdiff_t) ) {
    di << "allocator_type : OK\n";
  } else {
    di << "allocator_type : Error\n";
  }

  typedef int other_elem_type;
  if ( sizeof (allocator_type::rebind<other_elem_type>::other::value_type) == sizeof (other_elem_type) ) {
    di << "other_elem_type : OK\n";
  } else {
    di << "other_elem_type : Error\n";
  }

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

void QANCollection::Commands4(Draw_Interpretor& theCommands) {
  const char *group = "QANCollection";

  theCommands.Add("QANColStdAllocator1", "QANColStdAllocator1", __FILE__, QANColStdAllocator1, group);
  theCommands.Add("QANColStdAllocator2", "QANColStdAllocator2", __FILE__, QANColStdAllocator2, group);

  return;
}
