// File:        TDF_LabelNode.cxx
//              ------------------
// Author:      DAUTRY Philippe
//              <fid@fox.paris1.matra-dtv.fr>
// Copyright:   Matra Datavision 1997

// Version:     0.0
// History:     Version Date            Purpose
//              0.0     Feb  6 1997     Creation
//              MSV 21.03.2003: protect against stack overflow in destructor


#include <TDF_LabelNode.hxx>

#include <TDF_Data.hxx>
#include <TDF_Label.hxx>

//=======================================================================
//function : TDF_LabelNode
//purpose  : Constructor with TDF_Data*, only used for root node.
//=======================================================================

TDF_LabelNode::TDF_LabelNode
(TDF_Data* aDataPtr)
: myFather          (NULL), // The sign it is the root.
#ifdef KEEP_LOCAL_ROOT
  myBrother         (NULL),
#else
  myBrother         ((TDF_LabelNode *)aDataPtr),
#endif
  myFirstChild      (NULL),
  myLastFoundChild  (NULL), //jfa 10.01.2003
  myTag             (0), // Always 0 for root.
  myFlags           (0),
#ifdef KEEP_LOCAL_ROOT
  myData            (aDataPtr)
#endif
{
#ifdef DEB
  myDebugEntry = '0';
#endif
}


//=======================================================================
//function : TDF_LabelNode
//purpose  : Constructor
//=======================================================================

TDF_LabelNode::TDF_LabelNode
(const Standard_Integer aTag, TDF_LabelNode* aFather)
: myFather          (aFather),
  myBrother         (NULL),
  myFirstChild      (NULL),
  myLastFoundChild  (NULL), //jfa 10.01.2003
  myTag             (aTag),
  myFlags           (0),
#ifdef KEEP_LOCAL_ROOT
  myData            (NULL)
#endif
{
  if (aFather != NULL) {
    Depth(aFather->Depth() + 1);
#ifdef KEEP_LOCAL_ROOT
    myData = aFather -> Data();
#endif
  }
#ifdef DEB
  myDebugEntry = myFather->myDebugEntry;
  myDebugEntry += ':';
  myDebugEntry += aTag;
#endif
}

//=======================================================================
//function : ~TDF_LabelNode
//purpose  : 
//=======================================================================

TDF_LabelNode::~TDF_LabelNode()
{
  // MSV 21.03.2003: do not delete brother, rather delete all children in a loop
  //                 to avoid stack overflow
  while (myFirstChild != NULL) {
    TDF_LabelNode* aSecondChild = myFirstChild->Brother();
    delete myFirstChild;
    myFirstChild = aSecondChild;
  }
  myFirstAttribute.Nullify();
  myLastFoundChild = NULL;      //jfa 10.01.2003
}

//=======================================================================
//function : AddAttribute
//purpose  : Adds an attribute at the first or the specified position.
//=======================================================================

void TDF_LabelNode::AddAttribute
(const Handle(TDF_Attribute)& afterAtt,
 const Handle(TDF_Attribute)& newAtt)
{
  newAtt->myFlags = 1; // Valid.
  newAtt->myLabelNode  = this;
  if (afterAtt.IsNull()) { // Inserts at beginning.
    newAtt->myNext   = myFirstAttribute;
    myFirstAttribute = newAtt;
  }
  else { // Inserts at specified place.
    newAtt->myNext   = afterAtt->myNext;
    afterAtt->myNext = newAtt;
  }
}


//=======================================================================
//function : RemoveAttribute
//purpose  : Removes an attribute from the first or the specified position.
//=======================================================================

void TDF_LabelNode::RemoveAttribute
(const Handle(TDF_Attribute)& afterAtt,
 const Handle(TDF_Attribute)& oldAtt)
{
  oldAtt->myFlags = 0; // Unvalid.
  oldAtt->myLabelNode  = NULL;
  if (afterAtt.IsNull()) { // Removes from beginning.
    myFirstAttribute = oldAtt->myNext;
  }
  else { // Removes from specified place.
    afterAtt->myNext = oldAtt->myNext;
  }
  // Nullifier le next induit l'iterateur d'attribut en erreur.
  //oldAtt->myNext.Nullify();
}


//=======================================================================
//function : RootNode
//purpose  : used for non const object.
//=======================================================================

TDF_LabelNode* TDF_LabelNode::RootNode ()
{
#ifdef KEEP_LOCAL_ROOT
  return myData? myData -> myRoot: 0L;
#else
  TDF_LabelNode* lp = this;
  while (lp->myFather != NULL) lp = lp->myFather;
  return lp;
#endif
}


//=======================================================================
//function : RootNode
//purpose  : used for const object.
//=======================================================================

const TDF_LabelNode* TDF_LabelNode::RootNode () const
{
#ifdef KEEP_LOCAL_ROOT
  return myData? myData -> myRoot: 0L;
#else
  const TDF_LabelNode* lp = this;
  while (lp->myFather != NULL) lp = lp->myFather;
  return lp;
#endif
}


//=======================================================================
//function : Data
//purpose  : 
//=======================================================================

TDF_Data * TDF_LabelNode::Data () const
{
#ifdef KEEP_LOCAL_ROOT
  return myData;
#else
  const TDF_LabelNode* ln = RootNode()->myBrother;
  return ((TDF_Data*) ln);
#endif
}


//=======================================================================
//function : AllMayBeModified
//purpose  : 
//=======================================================================

void TDF_LabelNode::AllMayBeModified()
{
  MayBeModified(Standard_True);
  if (myFather && !myFather->MayBeModified()) myFather->AllMayBeModified();
}
