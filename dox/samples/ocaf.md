OCAF: Usage Tutorial {#samples__ocaf}
========

## Getting Started

At the beginning of your development, you first define an application class by inheriting from the Application abstract class.
You only have to create and determine the resources of the application for specifying the format of your documents (you generally use the standard one) and their file extension.

Then, you design the application data model by organizing attributes you choose among those provided with OCAF.
You can specialize these attributes using the User attribute. For example, if you need a reflection coefficient,
you aggregate a User attribute identified as a reflection coefficient
with a Real attribute containing the value of the coefficient (as such, you don't define a new class).

If you need application specific data not provided with OCAF, for example, to incorporate a finite element model in the data structure,
you define a new attribute class containing the mesh, and you include its persistent homologue in a new file format.

Once you have implemented the commands which create and modify the data structure according to your specification, OCAF provides you, without any additional programming:

* Persistent reference to any data, including geometric elements - several documents can be linked with such reference;
* Document-View association;
* Ready-to-use functions such as:
  * Undo-redo;
  * Save and open application data.

Finally, you develop the application's graphical user interface using the toolkit of your choice, for example Qt, GTK, MFC, .NET, or a similar framework.

## An example of OCAF usage

To create a useful OCAF-based application, it is necessary to redefine two deferred methods: <i>Formats</i> and <i>ResourcesName</i>

In the <i>Formats</i> method, add the format of the documents, which need to be read by the application and may have been built in other applications.

For example:

~~~~{.cpp}
    void myApplication::Formats(NCollection_Sequence<TCollection_ExtendedString>& Formats)
    {
      Formats.Append(TCollection_ExtendedString ("OCAF-myApplication"));
    }
~~~~

In the <i>ResourcesName</i> method, you only define the name of the resource file.
This file contains several definitions for the saving and opening mechanisms associated with each format and calling of the plug-in file.

~~~~{.cpp}
    const char* myApplication::ResourcesName()
    {
      return const char* ("Resources");
    }
~~~~

To obtain the saving and opening mechanisms, it is necessary to set two environment variables: <i>CSF_PluginDefaults</i>, which defines the path of the plug-in file,
and <i>CSF_ResourcesDefaults</i>, which defines the resource file (the name of the resource environment variable is built from the value returned by `ResourcesName()`):

~~~~{.cpp}
    SetEnvironmentVariable ("CSF_PluginDefaults", myDirectory);
    SetEnvironmentVariable ("CSF_ResourcesDefaults", myDirectory);
~~~~

The plugin and the resource files of the application will be located in <i>myDirectory</i>.
The name of the plugin file must be <i>Plugin</i>.

### Resource File

The resource file describes the documents (type and extension) and the type of data that the application can manipulate
by identifying the storage and retrieval drivers appropriate for this data.

Each driver is unique and identified by a GUID generated, for example, with the <i>uuidgen</i> tool in Windows.

Five drivers are required to use all standard attributes provided within OCAF:

  * the schema driver (ad696002-5b34-11d1-b5ba-00a0c9064368)
  * the document storage driver (ad696000-5b34-11d1-b5ba-00a0c9064368)
  * the document retrieval driver (ad696001-5b34-11d1-b5ba-00a0c9064368)
  * the attribute storage driver (47b0b826-d931-11d1-b5da-00a0c9064368)
  * the attribute retrieval driver (47b0b827-d931-11d1-b5da-00a0c9064368)

These drivers are provided as plug-ins by OCCT toolkits such as *TKStd* / *TKStdL* (legacy binary), *TKBin* / *TKBinL* (current binary) and *TKXml* / *TKXmlL* (XML). The mapping from GUID to toolkit lives in the standard `Plugin` resource file shipped under `resources/StdResource/Plugin`.

For example, this is a resource file, which declares a new model document OCAF-MyApplication:

~~~~
formatlist:OCAF-MyApplication
OCAF-MyApplication.Description: MyApplication Document Version 1.0
OCAF-MyApplication.FileExtension: sta
OCAF-MyApplication.StoragePlugin: ad696000-5b34-11d1-b5ba-00a0c9064368
OCAF-MyApplication.RetrievalPlugin: ad696001-5b34-11d1-b5ba-00a0c9064368
OCAF-MyApplicationSchema: ad696002-5b34-11d1-b5ba-00a0c9064368
OCAF-MyApplication.AttributeStoragePlugin: 47b0b826-d931-11d1-b5da-00a0c9064368
OCAF-MyApplication.AttributeRetrievalPlugin: 47b0b827-d931-11d1-b5da-00a0c9064368
~~~~
   
### Plugin File

The plugin file describes the list of required plug-ins to run the application and the toolkits in which the plug-ins are located.

You need the OCAF plug-in drivers (such as *TKStd*, *TKBin*, *TKXml*) to run an OCAF application; OCCT ships them in `resources/StdResource/Plugin`, which is loaded via the *CSF_PluginDefaults* environment variable.

The syntax of each item is <i>Identification.Location: Toolkit_Name</i>, where:
* Identification is the *Standard_GUID* of the service implemented by the plug-in.
* Location is the kind of attribute (here always `.Location`).
* Toolkit_Name is the OCCT toolkit name without the platform-specific `lib`/`.dll`/`.so` decoration; the platform's dynamic loader resolves the actual library at runtime.

For example, an excerpt from the standard `Plugin` file:

~~~~
! standard attribute drivers plugin
ad696001-5b34-11d1-b5ba-00a0c9064368.Location: TKStd

! BinOcaf Document Plugin
03a56835-8269-11d5-aab2-0050044b1af1.Location: TKBin
03a56836-8269-11d5-aab2-0050044b1af1.Location: TKBin

! XmlOcaf Document Plugin
03a56820-8269-11d5-aab2-0050044b1af1.Location: TKXml
03a56822-8269-11d5-aab2-0050044b1af1.Location: TKXml
03a56824-8269-11d5-aab2-0050044b1af1.Location: TKXml
~~~~

## Implementation of Attribute Transformation in a HXX file

~~~~{.cpp}
#include <TDF_Attribute.hxx>

#include <gp_Ax3.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Trsf.hxx>

//! This attribute implements a transformation data container
class MyPackage_Transformation : public TDF_Attribute
{
public: //!@ name Static methods

  //! The method returns a unique GUID of this attribute.
  //! By means of this GUID this attribute may be identified
  //! among other attributes attached to the same label.
  Standard_EXPORT static const Standard_GUID& GetID ();

  //! Finds or creates the attribute attached to <theLabel>.
  //! The found or created attribute is returned.
  Standard_EXPORT static occ::handle<MyPackage_Transformation> Set (const TDF_Label theLabel);

public: //!@ name Methods for access to the attribute data

  //! The method returns the transformation.
  Standard_EXPORT gp_Trsf Get () const;

public: //!@ name Methods for setting the data of transformation

  //! The method defines a rotation type of transformation.
  Standard_EXPORT void SetRotation (const gp_Ax1& theAxis, double theAngle);

  //! The method defines a translation type of transformation.
  Standard_EXPORT void SetTranslation (const gp_Vec& theVector);

  //! The method defines a point mirror type of transformation (point symmetry).
  Standard_EXPORT void SetMirror (const gp_Pnt& thePoint);

  //! The method defines an axis mirror type of transformation (axial symmetry).
  Standard_EXPORT void SetMirror (const gp_Ax1& theAxis);

  //! The method defines a planar mirror type of transformation (planar symmetry).
  Standard_EXPORT void SetMirror (const gp_Ax2& thePlane);

  //! The method defines a scale type of transformation.
  Standard_EXPORT void SetScale (const gp_Pnt& thePoint, double theScale);

  //! The method defines a complex type of transformation from one coordinate system to another.
  Standard_EXPORT void SetTransformation (const gp_Ax3& theCoordinateSystem1, const gp_Ax3& theCoordinateSystem2);

public: //!@ name Overridden methods from TDF_Attribute

  //! The method returns a unique GUID of the attribute.
  //! By means of this GUID this attribute may be identified among other attributes attached to the same label.
  Standard_EXPORT const Standard_GUID& ID () const;

  //! The method is called on Undo / Redo.
  //! It copies the content of theAttribute into this attribute (copies the fields).
  Standard_EXPORT void Restore (const occ::handle<TDF_Attribute>& theAttribute);

  //! It creates a new instance of this attribute.
  //! It is called on Copy / Paste, Undo / Redo.
  Standard_EXPORT occ::handle<TDF_Attribute> NewEmpty () const;

  //! The method is called on Copy / Paste.
  //! It copies the content of this attribute into theAttribute (copies the fields).
  Standard_EXPORT void Paste (const occ::handle<TDF_Attribute>& theAttribute, const occ::handle<TDF_RelocationTable>& theRelocationTable);

  //! Prints the content of this attribute into the stream.
  Standard_EXPORT Standard_OStream& Dump(Standard_OStream& theOS);

public: //!@ name Constructor

  //! The C++ constructor of this attribute class.
  //! Usually it is never called outside this class.
  Standard_EXPORT MyPackage_Transformation();

private:
  gp_TrsfForm myType;

  // Axes (Ax1, Ax2, Ax3)
  gp_Ax1 myAx1;
  gp_Ax2 myAx2;
  gp_Ax3 myFirstAx3;
  gp_Ax3 mySecondAx3;

  // Scalar values
  double myAngle;
  double myScale;

  // Points
  gp_Pnt myFirstPoint;
  gp_Pnt mySecondPoint;
};
~~~~

## Implementation of Attribute Transformation in a CPP file

~~~~{.cpp}
#include <MyPackage_Transformation.hxx>

//==================================================================================================

const Standard_GUID& MyPackage_Transformation::GetID()
{
  static Standard_GUID ID("4443368E-C808-4468-984D-B26906BA8573");
  return ID;
}

//==================================================================================================

occ::handle<MyPackage_Transformation> MyPackage_Transformation::Set(const TDF_Label& theLabel)
{
  occ::handle<MyPackage_Transformation> T;
  if (!theLabel.FindAttribute(MyPackage_Transformation::GetID(), T))
  {
    T = new MyPackage_Transformation();
    theLabel.AddAttribute(T);
  }
  return T;
}

//==================================================================================================

gp_Trsf MyPackage_Transformation::Get() const
{
  gp_Trsf transformation;
  switch (myType)
  {
    case gp_Identity:
    {
      break;
    }
    case gp_Rotation:
    {
      transformation.SetRotation(myAx1, myAngle);
      break;
    }
    case gp_Translation:
    {
      transformation.SetTranslation(myFirstPoint, mySecondPoint);
      break;
    }
    case gp_PntMirror:
    {
      transformation.SetMirror(myFirstPoint);
      break;
    }
    case gp_Ax1Mirror:
    {
      transformation.SetMirror(myAx1);
      break;
    }
    case gp_Ax2Mirror:
    {
      transformation.SetMirror(myAx2);
      break;
    }
    case gp_Scale:
    {
      transformation.SetScale(myFirstPoint, myScale);
      break;
    }
    case gp_CompoundTrsf:
    {
      transformation.SetTransformation(myFirstAx3, mySecondAx3);
      break;
    }
    case gp_Other:
    {
      break;
    }
  }
  return transformation;
}

//==================================================================================================

void MyPackage_Transformation::SetRotation(const gp_Ax1& theAxis, const double theAngle)
{
  Backup();
  myType = gp_Rotation;
  myAx1 = theAxis;
  myAngle = theAngle;
}

//==================================================================================================

void MyPackage_Transformation::SetTranslation(const gp_Vec& theVector)
{
  Backup();
  myType = gp_Translation;
  myFirstPoint.SetCoord(0, 0, 0);
  mySecondPoint.SetCoord(theVector.X(), theVector.Y(), theVector.Z());
}

//==================================================================================================

void MyPackage_Transformation::SetMirror(const gp_Pnt& thePoint)
{
  Backup();
  myType = gp_PntMirror;
  myFirstPoint = thePoint;
}

//==================================================================================================

void MyPackage_Transformation::SetMirror(const gp_Ax1& theAxis)
{
  Backup();
  myType = gp_Ax1Mirror;
  myAx1 = theAxis;
}

//==================================================================================================

void MyPackage_Transformation::SetMirror(const gp_Ax2& thePlane)
{
  Backup();
  myType = gp_Ax2Mirror;
  myAx2 = thePlane;
}

//==================================================================================================

void MyPackage_Transformation::SetScale(const gp_Pnt& thePoint, const double theScale)
{
  Backup();
  myType = gp_Scale;
  myFirstPoint = thePoint;
  myScale = theScale;
}

//==================================================================================================

void MyPackage_Transformation::SetTransformation (const gp_Ax3& theCoordinateSystem1,
                                                  const gp_Ax3& theCoordinateSystem2)
{
  Backup();
  myFirstAx3 = theCoordinateSystem1;
  mySecondAx3 = theCoordinateSystem2;
}

//==================================================================================================

const Standard_GUID& MyPackage_Transformation::ID() const
{
  return GetID();
}

//==================================================================================================

void MyPackage_Transformation::Restore(const occ::handle<TDF_Attribute>& theAttribute)
{
  occ::handle<MyPackage_Transformation> theTransformation = occ::down_cast<MyPackage_Transformation>(theAttribute);
  myType = theTransformation->myType;
  myAx1 = theTransformation->myAx1;
  myAx2 = theTransformation->myAx2;
  myFirstAx3 = theTransformation->myFirstAx3;
  mySecondAx3 = theTransformation->mySecondAx3;
  myAngle = theTransformation->myAngle;
  myScale = theTransformation->myScale;
  myFirstPoint = theTransformation->myFirstPoint;
  mySecondPoint = theTransformation->mySecondPoint;
}

//==================================================================================================

occ::handle<TDF_Attribute> MyPackage_Transformation::NewEmpty() const
{
  return new MyPackage_Transformation();
}

//==================================================================================================

void MyPackage_Transformation::Paste (const occ::handle<TDF_Attribute>& theAttribute,
                                      const occ::handle<TDF_RelocationTable>& ) const
{
  occ::handle<MyPackage_Transformation> theTransformation = occ::down_cast<MyPackage_Transformation>(theAttribute);
  theTransformation->myType = myType;
  theTransformation->myAx1 = myAx1;
  theTransformation->myAx2 = myAx2;
  theTransformation->myFirstAx3 = myFirstAx3;
  theTransformation->mySecondAx3 = mySecondAx3;
  theTransformation->myAngle = myAngle;
  theTransformation->myScale = myScale;
  theTransformation->myFirstPoint = myFirstPoint;
  theTransformation->mySecondPoint = mySecondPoint;
}

//==================================================================================================

Standard_OStream& MyPackage_Transformation::Dump(Standard_OStream& theOS) const
{
  theOS << "Transformation: ";
  switch (myType)
  {
    case gp_Identity:
    {
      theOS << "gp_Identity";
      break;
    }
    case gp_Rotation:
    {
      theOS << "gp_Rotation";
      break;
    }
    case gp_Translation:
    {
      theOS << "gp_Translation";
      break;
    }
    case gp_PntMirror:
    {
      theOS << "gp_PntMirror";
      break;
    }
    case gp_Ax1Mirror:
    {
      theOS << "gp_Ax1Mirror";
      break;
    }
    case gp_Ax2Mirror:
    {
      theOS << "gp_Ax2Mirror";
      break;
    }
    case gp_Scale:
    {
      theOS << "gp_Scale";
      break;
    }
    case gp_CompoundTrsf:
    {
      theOS << "gp_CompoundTrsf";
      break;
    }
    case gp_Other:
    {
      theOS << "gp_Other";
      break;
    }
  }
  return theOS;
}

//==================================================================================================

MyPackage_Transformation::MyPackage_Transformation()
: myType (gp_Identity)
{
  //
}
~~~~

## Implementation of typical actions with standard OCAF attributes.

The following four examples present typical actions with OCAF services (mainly for newcomers).
The method *Sample()* of each example is not dedicated for execution "as is"; it is rather a set of logical actions using some OCAF services.

### TDataStd_Sample.cxx
This sample contains templates for typical actions with the following standard OCAF attributes:
- Starting with data framework;
- TDataStd_Integer attribute management;
- TDataStd_RealArray attribute management;
- TDataStd_Comment attribute management;
- TDataStd_Name attribute management;
- TDataStd_UAttribute attribute management;
- TDF_Reference attribute management;
- TDataXtd_Point attribute management;
- TDataXtd_Plane attribute management;
- TDataXtd_Axis attribute management;
- TDataXtd_Geometry attribute management;
- TDataXtd_Constraint attribute management;
- TDataStd_Directory attribute management;
- TDataStd_TreeNode attribute management.

### TDocStd_Sample.cxx
This sample contains template for the following typical actions:
- creating application;
- creating the new document (document contains a framework);
- retrieving the document from a label of its framework;
- filling a document with data;
- saving a document in the file;
- closing a document;
- opening the document stored in the file;
- copying content of a document to another document with possibility to update the copy in the future.

### TPrsStd_Sample.cxx
This sample contains template for the following typical actions:
- starting with data framework;
- setting the TPrsStd_AISViewer in the framework;
- initialization of aViewer;
- finding TPrsStd_AISViewer attribute in the DataFramework;
- getting AIS_InteractiveContext from TPrsStd_AISViewer;
- adding driver to the map of drivers;
- getting driver from the map of drivers;
- setting TNaming_NamedShape to \<ShapeLabel\>;
- setting the new  TPrsStd_AISPresentation to \<ShapeLabel\>;
- displaying;
- erasing;
- updating and displaying presentation of the attribute to be displayed;
- setting a color to the displayed attribute;
- getting transparency of the displayed attribute;
- modify attribute;
- updating presentation of the attribute in viewer.

### TNaming_Sample.cxx
This sample contains template for typical actions with OCAF Topological Naming services.
The following scenario is used:
- data framework initialization;
- creating Box1 and pushing it as PRIMITIVE in DF;
- creating Box2 and pushing it as PRIMITIVE in DF;
- moving Box2 (applying a transformation);
- pushing the selected edges of the top face of Box1 in DF;
- creating a Fillet (using the selected edges) and pushing the result as a modification of Box1;
- creating a Cut (Box1, Box2) as a modification of Box1 and push it in DF;
- recovering the result from DF.
