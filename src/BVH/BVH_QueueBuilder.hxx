// Created on: 2014-09-15
// Created by: Denis BOGOLEPOV
// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#ifndef _BVH_QueueBuilder_Header
#define _BVH_QueueBuilder_Header

#include <BVH_Builder.hxx>
#include <BVH_BuildThread.hxx>
#include <NCollection_Vector.hxx>

//! Abstract BVH builder based on the concept of work queue.
//! Queue based BVH builders support parallelization with a
//! fixed number of threads (maximum efficiency is achieved
//! by setting the number of threads equal to the number of
//! CPU cores plus one). Note that to support parallel mode,
//! a corresponding BVH primitive set should provide thread
//! safe implementations of interface functions (e.g., Swap,
//! Box, Center). Otherwise, the results will be undefined.
//! \tparam T Numeric data type
//! \tparam N Vector dimension
template<class T, int N>
class BVH_QueueBuilder : public BVH_Builder<T, N>
{
public:

  //! Creates new BVH queue based builder.
  BVH_QueueBuilder (const Standard_Integer theLeafNodeSize,
                    const Standard_Integer theMaxTreeDepth,
                    const Standard_Integer theNumOfThreads = 1);

  //! Releases resources of BVH queue based builder.
  virtual ~BVH_QueueBuilder() = 0;

public:

  //! Builds BVH using specific algorithm.
  virtual void Build (BVH_Set<T, N>*       theSet,
                      BVH_Tree<T, N>*      theBVH,
                      const BVH_Box<T, N>& theBox);

protected:

  //! Stores range of primitives belonging to a BVH node.
  struct BVH_PrimitiveRange
  {
    Standard_Integer Start;
    Standard_Integer Final;

    //! Creates new primitive range.
    BVH_PrimitiveRange (Standard_Integer theStart = -1,
                        Standard_Integer theFinal = -1)
    : Start (theStart),
      Final (theFinal)
    {
      //
    }

    //! Returns total number of primitives.
    Standard_Integer Size() const
    {
      return Final - Start + 1;
    }

    //! Checks if the range is initialized.
    Standard_Boolean IsValid() const
    {
      return Start != -1;
    }
  };

  //! Stores parameters of constructed child nodes.
  struct BVH_ChildNodes
  {
    //! Bounding boxes of child nodes.
    BVH_Box<T, N> Boxes[2];

    //! Primitive ranges of child nodes.
    BVH_PrimitiveRange Ranges[2];

    //! Creates new parameters of BVH child nodes.
    BVH_ChildNodes()
    {
      //
    }

    //! Creates new parameters of BVH child nodes.
    BVH_ChildNodes (const BVH_Box<T, N>&      theLftBox,
                    const BVH_Box<T, N>&      theRghBox,
                    const BVH_PrimitiveRange& theLftRange,
                    const BVH_PrimitiveRange& theRghRange)
    {
      Boxes[0]  = theLftBox;
      Boxes[1]  = theRghBox;
      Ranges[0] = theLftRange;
      Ranges[1] = theRghRange;
    }

    //! Returns number of primitives in the given child.
    Standard_Integer NbPrims (const Standard_Integer theChild) const
    {
      return Ranges[theChild].Size();
    }

    //! Checks if the parameters is initialized.
    Standard_Boolean IsValid() const
    {
      return Ranges[0].IsValid() && Ranges[1].IsValid();
    }
  };

  //! Wrapper for BVH build data.
  class BVH_TypedBuildTool : public BVH_BuildTool
  {
  public:

    //! Creates new BVH build thread.
    BVH_TypedBuildTool (BVH_Set<T, N>*     theSet,
                        BVH_Tree<T, N>*    theBVH,
                        BVH_Builder<T, N>* theAlgo)
    : mySet  (theSet),
      myBVH  (theBVH)
    {
      myAlgo = dynamic_cast<BVH_QueueBuilder<T, N>* > (theAlgo);

      Standard_ASSERT_RAISE (myAlgo != NULL,
        "Error! BVH builder should be queue based");
    }

    //! Performs splitting of the given BVH node.
    virtual void Perform (const Standard_Integer theNode)
    {
      const typename BVH_QueueBuilder<T, N>::BVH_ChildNodes aChildren = myAlgo->BuildNode (mySet, myBVH, theNode);

      myAlgo->AddChildren (myBVH, theNode, aChildren);
    }

  protected:

    //! Primitive set to build BVH.
    BVH_Set<T, N>* mySet;

    //! Output BVH tree for the set.
    BVH_Tree<T, N>* myBVH;

    //! Queue based BVH builder to use.
    BVH_QueueBuilder<T, N>* myAlgo;
  };

protected:

  //! Performs splitting of the given BVH node.
  virtual typename BVH_QueueBuilder<T, N>::BVH_ChildNodes BuildNode (BVH_Set<T, N>*         theSet,
                                                                     BVH_Tree<T, N>*        theBVH,
                                                                     const Standard_Integer theNode) = 0;

  //! Processes child nodes of the splitted BVH node.
  virtual void AddChildren (BVH_Tree<T, N>*        theBVH,
                            const Standard_Integer theNode,
                            const BVH_ChildNodes&  theSubNodes);

protected:

  BVH_BuildQueue myBuildQueue; //!< Queue to manage BVH node building tasks

  Standard_Integer myNumOfThreads; //!< Number of threads used to build BVH

};

#include <BVH_QueueBuilder.lxx>

#endif // _BVH_QueueBuilder_Header
