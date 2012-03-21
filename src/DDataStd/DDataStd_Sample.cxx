// Created on: 1999-12-28
// Created by: Sergey RUIN
// Copyright (c) 1999-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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



#include <TDF_Data.hxx>
#include <TDF_Label.hxx>
#include <TDataXtd_Axis.hxx>
#include <TDataStd_Current.hxx>
#include <TDataStd_Comment.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_IntegerArray.hxx>
#include <TDataStd_Real.hxx>
#include <TDataStd_RealArray.hxx>
#include <TDF_Reference.hxx>
#include <TDataStd_UAttribute.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDataXtd_Constraint.hxx>
#include <TDataXtd_Shape.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_Directory.hxx>
#include <TDataXtd_Point.hxx>
#include <TDataXtd_Plane.hxx>
#include <TDataXtd_Geometry.hxx>
#include <TDataXtd_GeometryEnum.hxx>
#include <TDataXtd_ConstraintEnum.hxx>
#include <TNaming_NamedShape.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pln.hxx>
#include <gp_Lin.hxx>
#include <gp_Ax1.hxx>
#include <gp_Cylinder.hxx>
#include <TopoDS_Shape.hxx>
#include <TopLoc_Location.hxx>
#include <TDF_AttributeList.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TDataStd_ListOfExtendedString.hxx>
#include <TDocStd_Document.hxx>
#include <TDataStd_TreeNode.hxx>

#ifdef DEB

static void Sample()
{
  // Starting with data framework 
  Handle(TDF_Data) DF = new TDF_Data();
  TDF_Label aLabel = DF->Root();

  //------------------------- TDataStd_Integer (Real) ---------------------------------
  //-----------------------------------------------------------------------------------
  
  // Setting TDataStd_Integer attribute to a label (work with TDataStd_Real is the same)

  Standard_Integer i = 10;

  Handle(TDataStd_Integer) I = TDataStd_Integer::Set(aLabel, i);

  // Getting the value stored in TDataStd_Integer attribute

  Standard_Integer aValue;

  aValue = I->Get();

  // Setting the new value in the attribute 
    
  I->Set( 25 );

  //------------------------- TDataStd_RealArray (IntegerArray is analogical) --------------
  //-----------------------------------------------------------------------------------------

  // Setting TDataStd_RealArray attribute to a label 

  Standard_Integer lower = 1, upper = 20;

  Handle(TDataStd_RealArray) realarray = TDataStd_RealArray::Set(aLabel, lower, upper);
 
  // Filling array

  for(Standard_Integer j = realarray->Lower(); j<= realarray->Upper(); j++) {
    realarray->SetValue(j, M_PI * j); 
  }

  // Getting length of array

  Standard_Integer length = realarray->Length();

  // Retrieving value by index

  Standard_Real value = realarray->Value(3);

  // Getting handle to an underlying array TColStd_HArray1OfReal

  Handle(TColStd_HArray1OfReal) array = realarray->Array();

  //------------------------- TDataStd_Comment --------------
  //---------------------------------------------------------

  // Setting TDataStd_Comment attribute to a label

  Handle(TDataStd_Comment) comment = TDataStd_Comment::Set(aLabel);

  // Setting comment string in to the attribute
 
  TCollection_ExtendedString message = "This is a remark";

  comment->Set(message);

  // Getting comment string stored in the attribute

  TCollection_ExtendedString string = comment->Get();  

  //------------------------- TDataStd_Name -----------------
  //---------------------------------------------------------
 
  // Setting TDataStd_Name attribute to a label

//  Handle(TDataStd_Name) name = TDataStd_Name::Set(aLabel);


  // Checking if the label has a name (TDataStd_Name attribute with not empty name field)

  // Standard_Boolean isempty = name->IsEmpty();

  // Getting the name of the label

  //if( !isempty ) TCollection_ExtendedString thename = name->Get();

  // Erasing the name of the label in TDataStd_Name attribute

  // name->SetEmpty(); 
  
  // Setting a new name string in the attribute
  
  TCollection_ExtendedString aname = "Name of the label";
  Handle(TDataStd_Name) name = TDataStd_Name::Set(aLabel,aname);

  name->Set(aname); 

  // Getting comment string stored in the attribute

  TCollection_ExtendedString namestring = name->Get();

  // Getting the first father label which has TDataStd_Name attribute

  //Handle(TDataStd_Name) father;

  //Standard_Boolean hasfather = name->Father(father);

  // Find if there exists label with full path "Assembly1:Part3:Prism7"

  // Converting string to list of names
 
  //TCollection_ExtendedString fullpath = "Assembly_1:Part_3:Prism_7";
  //TDataStd_ListOfExtendedString listofstring;
  
  //TDataStd_Name::MakePath(fullpath, listofstring);

  //Handle(TDataStd_Name) nameattribute;
  
  //Standard_Boolean found = TDataStd_Name::Find(DF, listofstring, nameattribute);
   
  //if( found ) {
    // Getting the fullpath of the nameattribute (fullpath consists of list of TDataStd_Name attributes situated on 
    // father labels from label where nameattribute is situated to root label
    //TDF_AttributeList list;
   
    //nameattribute->FullPath(list);
  //}

  //TDF_Label currentLabel;

  // ... Finding currentLabel...

  // Search  under <currentLabel>  a  label which fits with the given name
   
  //Handle(TDataStd_Name) givenname;

  //found = TDataStd_Name::Find(currentLabel, "Sketch_11", givenname);

  //if( found ) {
    
    // Getting all named child labels of the label where givenname attribute situated 
    //TDF_AttributeList listOfChildren;name to the 
    //Standard_Boolean isAtLeastOneFound =  givenname->ChildNames(listOfChildren);
  //}


  //------------------------- TDataStd_UAttribute -----------
  //---------------------------------------------------------

  // Setting TDataStd_UAttribute to a label

  Standard_GUID guid("01010101-0101-0101-1010-010101010101");

  Handle(TDataStd_UAttribute) uattribute = TDataStd_UAttribute::Set(aLabel, guid);

  // Finding a TDataStd_UAttribute on the label using standard mechanism
  
  Handle(TDataStd_UAttribute) theUA;
  if (aLabel.FindAttribute(guid,theUA)) {
    // do something
  }
  
  // Checking that a  TDataStd_UAttribute exist on a given label using standard mechanism
  
  if (aLabel.IsAttribute(guid)) {
    // do something
  }
  
  //------------------------- TDF_Reference  -----------
  //---------------------------------------------------------

  TDF_Label referencedlabel;

  // ... Finding referencedlabel ...

  
  // Setting TDF_Reference attribute to a label
  
  Handle(TDF_Reference) reference = TDF_Reference::Set(aLabel, referencedlabel);

  // Getting a label to TDF_Reference attribute refers to
   
  TDF_Label refLabel = reference->Get();


  //------------------------- TDataXtd_Point ----------------
  //---------------------------------------------------------
  gp_Pnt Pnt;

  // ... Defining point <Pnt> ...

  // Setting TDataXtd_Point attribute to a label

  Handle(TDataXtd_Point) P = TDataXtd_Point::Set(aLabel, Pnt);

  //Retrieve gp_Pnt associated with attribute
     
  gp_Pnt aPnt;
  TDataXtd_Geometry::Point(aLabel, aPnt);

  //------------------------- TDataXtd_Plane ----------------
  //---------------------------------------------------------
  gp_Pln Plane;

  // ... Defining plane <Plane> ...

  // Setting TDataXtd_Plane attribute to a label  

  Handle(TDataXtd_Plane) Pl = TDataXtd_Plane::Set(aLabel, Plane);

  //Retrieve gp_Plane associated with attribute
     
  gp_Pln aPlane;
  TDataXtd_Geometry::Plane(aLabel, aPlane);

  //------------------------- TDataXtd_Axis ----------------
  //---------------------------------------------------------

  gp_Lin Axis;

  // ... Defining axis <Axis> ...

  // Setting TDataXtd_Axis attribute to a label

  Handle(TDataXtd_Axis) axis = TDataXtd_Axis::Set(aLabel, Axis);
  
  //Retrieve gp_Ax1 associated with attribute
  
  gp_Ax1 anAxis;
  TDataXtd_Geometry::Axis(aLabel, anAxis); 


  //------------------------- TDataXtd_Geometry ----------------
  //---------------------------------------------------------

   Handle(TNaming_NamedShape) NS;

   // ... Constructing NS which contains cylinder...

   // Setting TDataXtd_Geometry attribute to the label where <NS> is situated

   TDF_Label NSLabel = NS->Label();

   Handle(TDataXtd_Geometry) geom = TDataXtd_Geometry::Set(NSLabel);

   // Setting a type of geometry   

   geom->SetType(TDataXtd_CYLINDER); 

   // Retrieving gp_Cylinder stored in associated NamedShape

   // Checking the type of geometry

   if( geom->GetType() == TDataXtd_CYLINDER ) {
   
     gp_Cylinder Cylinder;

     TDataXtd_Geometry::Cylinder(geom->Label(), Cylinder);     

   }

  //------------------------- TDataXtd_Constraint -------------
  //-----------------------------------------------------------

  // Setting TDataXtd_Constraint to a label

  Handle(TDataXtd_Constraint) constraint = TDataXtd_Constraint::Set(aLabel);

  Handle(TNaming_NamedShape) NS1, NS2;
  Handle(TDataStd_Real) aDistance;

  // ... Constructing NS1 and NS2 which contain lines and calculating the distance between them ...

  // Setting DISTANCE Dimension  between NS1 and NS2

  constraint->Set(TDataXtd_DISTANCE, NS1, NS2);

  constraint->SetValue(aDistance);     // <aDistance> should contain the calucalated distance

  // Checking if <constraint> is a Dimension rather than a Constraint

  Standard_Boolean isdimension = constraint->IsDimension();

  if(isdimension) {

   // Getting the distance between NS1 and NS2

   Handle(TDataStd_Real) valOfdistance = constraint->GetValue();

   Standard_Real distance = valOfdistance->Get();
  }

  // Setting PARALLEL constraint between NS1 and NS2

  constraint->Set(TDataXtd_PARALLEL, NS1, NS2);

  // Getting number of geometries which define a constarint
  
  Standard_Integer number = constraint->NbGeometries();

  // Checking if a constraint is verified

  Standard_Boolean isverified =  constraint->Verified();
  if( !isverified ) {
   cout << "Constraint is not valid"   << endl;
  }


  //------------------------- TDataStd_Directory --------------
  //-----------------------------------------------------------

  // Setting TDataStd_Directory to a label

  Handle(TDataStd_Directory) directory = TDataStd_Directory::New(aLabel);

  // Creating a new sub directory of the given directory

  Handle(TDataStd_Directory) newdirectory = TDataStd_Directory::AddDirectory(directory);

  // Creating a new label in directory

  TDF_Label newlabel = TDataStd_Directory::MakeObjectLabel(newdirectory);

   
  //------------------------- TDataStd_TreeNode ---------------
  //-----------------------------------------------------------

  // Let's create a tree:

  //  Root
  //   |
  //   --- FirstChild
  //   |
  //   --- SecondChild
  //           |
  //           --- FirstChildOfSecondChild



  // Setting a TDataStd_TreeNode attribute to a label. 
  // It becomes a root because it hasn't a father:
  
  Handle(TDataStd_TreeNode) Root = TDataStd_TreeNode::Set(aLabel);

  // Create a child TreeNode:
  
  Handle(TDataStd_TreeNode) FirstChild = TDataStd_TreeNode::Set(aLabel.FindChild(1));
  Root->Append(FirstChild);

  // Let's add a second child node to the Root:
  
  Handle(TDataStd_TreeNode) SecondChild = TDataStd_TreeNode::Set(aLabel.FindChild(2));
  Root->Append(SecondChild);
  
  // Now, it's time to create the a child for the SecondChild node - FirstChildOfSecondChild:
  
  Handle(TDataStd_TreeNode) FirstChildOfSecondChild = TDataStd_TreeNode::Set(aLabel.FindChild(3));
  SecondChild->Append(FirstChildOfSecondChild);

  // Let's redesign the tree:

  //  Root
  //   |
  //   --- SecondChild
  //           |
  //           --- FirstChild
  //           |
  //           --- FirstChildOfSecondChild
  
  // Removing of the FirstChild from our tree:
  
  FirstChild->Remove();
  
  // Setting the FirstChild node as a first child of the SecondChild node:
  
  SecondChild->Prepend(FirstChild);
}

#endif
