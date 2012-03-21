// Created on: 2001-06-27
// Created by: Alexander GRIGORIEV
// Copyright (c) 2001-2012 OPEN CASCADE SAS
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


#include <LDOM_BasicNode.hxx>
#include <LDOM_BasicElement.hxx>
#include <LDOM_BasicAttribute.hxx>
#include <LDOM_BasicText.hxx>

//=======================================================================
//function : operator =
//purpose  : Assignment
//=======================================================================

LDOM_BasicNode& LDOM_BasicNode::operator = (const LDOM_BasicNode& anOther)
{
  myNodeType = anOther.getNodeType();
  mySibling  = anOther.GetSibling();
  return * this;
}

//=======================================================================
//function : GetSibling
//purpose  : also detaches NULL siblings
//=======================================================================

const LDOM_BasicNode * LDOM_BasicNode::GetSibling () const
{
  while (mySibling)
    if (mySibling -> isNull())
      (const LDOM_BasicNode *&) mySibling = mySibling -> mySibling;
    else break;
  return mySibling;
}

#ifdef DEB
#ifndef WNT
//=======================================================================
// Debug Function for DBX: use "print -p <Variable> or pp <Variable>"
//=======================================================================
#include <LDOM_OSStream.hxx>
#define FLITERAL 0x10
#define MAX_SIBLINGS 8
char * db_pretty_print (const LDOM_BasicNode * aBNode, int fl, char *)
{
  LDOM_OSStream out (128);
  switch (aBNode -> getNodeType()) {
  case LDOM_Node::ELEMENT_NODE:
    {
      const LDOM_BasicElement& aBElem = * (const LDOM_BasicElement *) aBNode;
      if ((fl & FLITERAL) == 0) out << "LDOM_BasicElement: ";
      out << '<' << aBElem.GetTagName();
      aBNode = aBElem.GetFirstChild();
      while (aBNode) {
        if (aBNode -> getNodeType() == LDOM_Node::ATTRIBUTE_NODE)
          out << ' ' <<
            db_pretty_print ((const LDOM_BasicAttribute *) aBNode, FLITERAL, 0);
        aBNode = aBNode -> GetSibling();
      }
      out << '>';
      if ((fl & FLITERAL) == 0) {
        aBNode = aBElem.GetFirstChild();
        int counter = MAX_SIBLINGS;
        if (aBNode) {
          out << endl << " == Children:" << endl;
          while (aBNode && counter--) {
            if (aBNode -> getNodeType() == LDOM_Node::ATTRIBUTE_NODE) break;
            out << "  *(LDOM_BasicNode*)" << aBNode << " : " <<
              db_pretty_print (aBNode, FLITERAL, 0) << endl;
            aBNode = aBNode -> GetSibling();
          }
        }
        aBNode = aBElem.GetSibling();
        if (aBNode) {
          out << " == Siblings:" << endl;
          counter = MAX_SIBLINGS;
          while (aBNode && counter--) {
            if (aBNode -> getNodeType() == LDOM_Node::ATTRIBUTE_NODE) break;
            out << "  *(LDOM_BasicNode*)" << aBNode << " : " <<
              db_pretty_print (aBNode, FLITERAL, 0) << endl;
            aBNode = aBNode -> GetSibling();
          }
        }
      }
      out << ends;
      break;
    }
  case LDOM_Node::ATTRIBUTE_NODE:
    {
      const LDOM_BasicAttribute& aBAtt = * (const LDOM_BasicAttribute *) aBNode;
      if ((fl & FLITERAL) == 0) out << "LDOM_BasicAttribute: ";
      out << aBAtt.GetName() << '='
        << db_pretty_print (&aBAtt.GetValue(), FLITERAL, 0) << ends;
      break;
    }
  case LDOM_Node::TEXT_NODE:
  case LDOM_Node::COMMENT_NODE:
  case LDOM_Node::CDATA_SECTION_NODE:
    {
      const LDOM_BasicText& aBText = * (const LDOM_BasicText *) aBNode;
      if ((fl & FLITERAL) == 0) out << "LDOM_BasicText: ";
      out << db_pretty_print (&aBText.GetData(), FLITERAL, 0) << ends;
      break;
    }
  default:
    out << "UNKNOWN" << ends;
    break;
  }
  return (char *)out.str();
}

char * db_pretty_print (const LDOM_Node * aBNode, int fl, char * c)
{
  return db_pretty_print (&aBNode -> Origin(), fl, c);
}

#endif
#endif
