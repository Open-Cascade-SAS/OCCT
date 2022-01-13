AIS: Custom Presentation {#tutorials__ais_object}
========

@tableofcontents

@section intro Getting Started

OCCT provides a strong set of built-in Interactive Objects for rapid application development,
but the real power and flexibility of **Application Interactive Services** (@c AIS) could be revealed by subclassing and implementing custom presentations.
In this tutorial we will focus on the development of a custom @c AIS_InteractiveObject and show the basics step by step.

Let's start from the very beginning and try subclassing @c AIS_InteractiveObject object:

~~~~{.cpp}
class MyAisObject : public AIS_InteractiveObject
{
  DEFINE_STANDARD_RTTI_INLINE(MyAisObject, AIS_InteractiveObject)
public:
  MyAisObject() {}
public:
  virtual void Compute (const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                        const Handle(Prs3d_Presentation)& thePrs,
                        const Standard_Integer theMode) override {}

  virtual void ComputeSelection (const Handle(SelectMgr_Selection)& theSel,
                                 const Standard_Integer theMode) override {}

  virtual bool AcceptDisplayMode (const Standard_Integer theMode) const override
  { return true; }
};
~~~~

@c DEFINE_STANDARD_RTTI_INLINE() macro will register the new class within the OCCT Run-Time Type Information (RTTI) system.
This step is optional (you may skip it if you are not going to use methods like @c Standard_Transient::DynamicType() in application code), but it is a common practice while subclassing OCCT classes.

The @c AIS_InteractiveObject interface defines only a couple of pure virtual methods - @c @::Compute() defining an object presentation and @c @::ComputeSelection() defining a selectable (pickable) volume.
Selection and presentation are two independent mechanisms in **AIS**. Presentation rendering is done with help of OpenGL or a similar low-level graphics library, while selection doesn't depend on a graphic driver at all.
Providing an empty implementation of these two methods would be enough for adding the object to @c AIS_InteractiveContext (@c @::Display()), but obviously nothing will appear on the screen.

@section prs_builders Presentation builders

To go ahead, we need to define some presentation of our object.
OCCT provides a set of presentation building tools for common elements like arrows, shapes, boxes, etc.
These tools could be found within @c Prs3d, @c StdPrs and @c DsgPrs packages:

- **Prs3d**
  provides builders for simple geometric elements.
  - @c Prs3d_Arrow, @c Prs3d_BndBox, @c Prs3d_Point, @c Prs3d_Text, @c Prs3d_ToolCylinder, @c Prs3d_ToolDisk, @c Prs3d_ToolSector, @c Prs3d_ToolSphere, @c Prs3d_ToolTorus
- **StdPrs**
  provides builders for analytical geometry and B-Rep shapes (@c TopoDS_Shape).
  - @c StdPrs_WFShape, @c StdPrs_ShadedShape, @c StdPrs_BRepTextBuilder
- **DsgPrs**
  provides builders for datums, dimensions and relations.

Presentation builders are reusable bricks for constructing @c AIS objects.
Standard OCCT interactive objects highly rely on them, so that you may easily replicate @c AIS_Shape presentation for displaying a shape with just a couple of lines calling @c StdPrs_ShadedShape:

~~~~{.cpp}
void MyAisObject::Compute (const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                           const Handle(Prs3d_Presentation)& thePrs,
                           const Standard_Integer theMode)
{
  TopoDS_Shape aShape = BRepPrimAPI_MakeCylinder (100.0, 100.0);
  StdPrs_ShadedShape::Add (thePrs, aShape, myDrawer);
}
...
Handle(AIS_InteractiveContext) theCtx;
Handle(MyAisObject) aPrs = new MyAisObject();
theCtx->Display (aPrs, true);
~~~~

@figure{ais_object_step1_shaded.png,"@c StdPrs_ShadedShape presentation builder.",409} height=409px

@c PrsMgr_PresentableObject::Compute() method takes three arguments:
- **Presentation Manager** (@c PrsMgr_PresentationManager).
  Rarely used parameter, but might be necessary for some advanced use cases.
- **Presentation** (@c Prs3d_Presentation or @c Graphic3d_Structure).
  Defines the structure to fill in with presentation elements.
- **Display Mode** (integer number).
  Specifies the display mode to compute.
  **0** is a default display mode, if not overridden by @c AIS_InteractiveObject::SetDisplayMode() or by @c AIS_InteractiveContext::Display().

For each supported display mode, the **Presentation Manager** creates a dedicated @c Prs3d_Presentation and stores it within the object itself as a list of presentations @c PrsMgr_PresentableObject::Presentations().
It is a good practice to reject unsupported display modes within @c @::Compute() method:

~~~~{.cpp}
void MyAisObject::Compute (const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                           const Handle(Prs3d_Presentation)& thePrs,
                           const Standard_Integer theMode)
{
  if (theMode != 0) { return; } // reject non-zero display modes

  TopoDS_Shape aShape = BRepPrimAPI_MakeCylinder (100.0, 100.0);
  StdPrs_ShadedShape::Add (thePrs, aShape, myDrawer);
}
~~~~

This wouldn't, however, prevent application from displaying the object with another display mode like this:

~~~~{.cpp}
Handle(AIS_InteractiveContext) theCtx;
Handle(MyAisObject) aPrs = new MyAisObject();
theCtx->Display (aPrs, 100, -1, true);
~~~~

The code above will display @c MyAisObject with display mode equal to 100, and after @c @::Compute() modifications nothing will be displayed on the screen.
@c AIS will still create a presentation with specified display mode, but it will be empty - method @c @::AcceptDisplayMode() could be overridden to disallow even creation of an empty presentation:

~~~~{.cpp}
bool MyAisObject::AcceptDisplayMode (const Standard_Integer theMode) const
{
  return theMode == 0; // reject non-zero display modes
}
~~~~

@c AIS_InteractiveContext::Display() checks if requested display mode is actually supported by the object, and uses default display mode (_**0**_) if it is not.
@c StdPrs_ShadedShape prepares a shaded (triangulated) presentation of a shape, while @c StdPrs_WFShape creates a wireframe presentation with B-Rep wire boundaries:

~~~~{.cpp}
void MyAisObject::Compute (const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                           const Handle(Prs3d_Presentation)& thePrs,
                           const Standard_Integer theMode)
{
  if (!AcceptDisplayMode (theMode)) { return; }

  TopoDS_Shape aShape = BRepPrimAPI_MakeCylinder (100.0, 100.0);
  StdPrs_ShadedShape::Add (thePrs, aShape, myDrawer); // add shading
  StdPrs_WFShape::Add     (thePrs, aShape, myDrawer); // add wireframe
}
~~~~

@figure{ais_object_step1_shaded_wf.png,"Result of @c StdPrs_ShadedShape + @c StdPrs_WFShape presentation builders.",409} height=409px

Presentation builders take the @c Prs3d_Drawer object defining various attributes - material of shaded shape, number of isolines in wireframe mode, tessellation quality, line colors and many others.
@c PrsMgr_PresentableObject defines @c myDrawer property with default attributes.
@c StdPrs makes it easy to display topological shapes.
With the help of @c Prs3d tools we may display elements like arrows, boxes or text labels.
Let's extend our presentation with a second **display mode 1** showing a bounding box using @c Prs3d_BndBox builder:

~~~~{.cpp}
bool MyAisObject::AcceptDisplayMode (const Standard_Integer theMode) const
{
  return theMode == 0 || theMode == 1;
}
void MyAisObject::Compute (const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                           const Handle(Prs3d_Presentation)& thePrs,
                           const Standard_Integer theMode)
{
  TopoDS_Shape aShape = BRepPrimAPI_MakeCylinder (100.0, 100.0);
  if (theMode == 0)
  {
    StdPrs_ShadedShape::Add (thePrs, aShape, myDrawer);
    StdPrs_WFShape::Add (thePrs, aShape, myDrawer); // add wireframe
  }
  else if (theMode == 1)
  {
    Bnd_Box aBox;
    BRepBndLib::Add (aShape, aBox);
    Prs3d_BndBox::Add (thePrs, aBox, myDrawer);
  }
}
~~~~

Now, displaying an object with **display mode 1** will show a box:

~~~~{.cpp}
Handle(AIS_InteractiveContext) theCtx;
Handle(MyAisObject) aPrs = new MyAisObject();
theCtx->Display (aPrs, 1, 0, true);
~~~~

@figure{ais_object_step1_bndbox.png,"@c Prs3d_BndBox presentation builder.",409} height=409px

@c AIS disallows activating multiple display modes at the same time, so that these presentation modes should be alternatives to each other.
But @c AIS may use non-active display mode for highlighting purposes - like wireframe (@c AIS_Wireframe) presentation displayed on top of shaded (@c AIS_Shaded) presentation for selected @c AIS_Shape objects.

Let's define a dedicated enumeration for display modes supported by our interactive object and setup the 1st (@c MyDispMode_Highlight) display mode for highlighting with help of @c PrsMgr_PresentableObject::SetHilightMode():

~~~~{.cpp}
class MyAisObject : public AIS_InteractiveObject
{
public:
  enum MyDispMode { MyDispMode_Main = 0, MyDispMode_Highlight = 1 };

...

MyAisObject::MyAisObject()
{
  SetDisplayMode (MyDispMode_Main);      // main (active) display mode
  SetHilightMode (MyDispMode_Highlight); // auxiliary (highlighting) mode
}

...

Handle(AIS_InteractiveContext) theCtx;
Handle(MyAisObject) aPrs = new MyAisObject();
theCtx->Display (aPrs, MyAisObject::MyDispMode_Main, 0, false);
theCtx->HilightWithColor (aPrs, aPrs->HilightAttributes(), false);
theCtx->CurrentViewer()->Redraw();
~~~~

@figure{ais_object_step1_highlight.png,"Highlighting by color (left) and highlighting by another display mode (right).",818} height=409px

In this particular use case we've used the method @c AIS_InteractiveContext::HilightWithColor() instead of @c @::SetSelected() - just because our object is not selectable yet and @c @::SetSelected() wouldn't work.
Highlighted presentation appears on the screen with modulated color (see left screenshot above).
Using a dedicated display mode for highlighting (right screenshot above) allows customizing presentation in selected / highlighted states.

@section prim_arrays Primitive arrays

@c Prs3d_Presentation might be filled in by the following **primitives**:
- **Triangles**
  - @c Graphic3d_ArrayOfTriangles
  - @c Graphic3d_ArrayOfTriangleFans
  - @c Graphic3d_ArrayOfTriangleStrips
- **Lines**
  - @c Graphic3d_ArrayOfSegments
  - @c Graphic3d_ArrayOfPolylines
- **Points** or **Markers**
  - @c Graphic3d_ArrayOfPoints

This triplet of primitives is what graphics hardware is capable of rendering, so that it could be transferred directly to low-level graphics libraries in the form of *Vertex Buffer Objects* (VBO).
Each **primitive array** consists of an array of vertex attributes (_**position**, **normal**, **texture coordinates**, **vertex colors**_, etc.) and optional **array of indices**.
The latter one avoids duplicating vertices shared between connected elements (triangles, polylines) in attributes array.

@c Graphic3d_ArrayOfPrimitives and it's subclasses provide a convenient interface for filling in primitive arrays:
- Constructor takes a number of vertices, number of edges (indices) and a bitmask of optional vertex attributes.
- @c Graphic3d_ArrayOfPrimitives::AddVertex() appends a vertex with specified attributes to the end of the array (within the range specified at construction time).
- @c Graphic3d_ArrayOfPrimitives::AddEdges() appends indices, starting with 1.
  Each line segment is defined by two consequential edges, each triangle is defined by three consequential edges.

Let's extend our sample and display a cylinder section contour defined by array of indexed segments (e.g. a polyline of four vertices):

~~~~{.cpp}
void MyAisObject::Compute (const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                           const Handle(Prs3d_Presentation)& thePrs,
                           const Standard_Integer theMode)
{
  const double aRadius = 100.0, aHeight = 100.0;
  TopoDS_Shape aShape = BRepPrimAPI_MakeCylinder (aRadius, aHeight);
  if (theMode == MyDispMode_Main)
  {
    StdPrs_ShadedShape::Add (thePrs, aShape, myDrawer);
    //StdPrs_WFShape::Add (thePrs, aShape, myDrawer);
    Handle(Graphic3d_ArrayOfSegments) aSegs = new Graphic3d_ArrayOfSegments (4, 4 * 2, Graphic3d_ArrayFlags_None);
    aSegs->AddVertex (gp_Pnt (0.0, -aRadius, 0.0));
    aSegs->AddVertex (gp_Pnt (0.0, -aRadius, aHeight));
    aSegs->AddVertex (gp_Pnt (0.0,  aRadius, aHeight));
    aSegs->AddVertex (gp_Pnt (0.0,  aRadius, 0.0));
    aSegs->AddEdges (1, 2);
    aSegs->AddEdges (2, 3);
    aSegs->AddEdges (3, 4);
    aSegs->AddEdges (4, 1);
    Handle(Graphic3d_Group) aGroupSegs = thePrs->NewGroup();
    aGroupSegs->SetGroupPrimitivesAspect (myDrawer->WireAspect()->Aspect());
    aGroupSegs->AddPrimitiveArray (aSegs);
  }
  else if (theMode == MyDispMode_Highlight) { ... }
}
~~~~

@figure{ais_object_step2_segments.png,"Displaying @c Graphic3d_ArrayOfSegments.",409} height=409px

The process is quite straightforward:
- Create a new @c Graphic3d_Group using @c Prs3d_Presentation::NewGroup();
- Specify presentation aspects using @c Graphic3d_Group::SetGroupPrimitivesAspect();
- Create and add an array of primitives using @c Graphic3d_Group::AddPrimitiveArray().

Standard presentation builders like @c StdPrs_ShadedShape / @c StdPrs_WFShape internally do exactly the same thing - a tessellated representation of a shape is added to presentation in form of triangles (shaded),
line segments (wireframe and free edges) and markers (free shape vertices).

A single @c Graphic3d_Group normally defines just a single primitive array, but it is technically possible adding more arrays to the same group @c Graphic3d_Group::AddPrimitiveArray()
and with different aspects @c Graphic3d_Group::SetPrimitivesAspect(), which might be considered in advanced scenarios.

Method @c Graphic3d_Group::AddText() allows adding text labels to a presentation.
Internally, text labels are rendered as an array of textured triangles using texture atlas created from a font, but this complex logic is hidden from the user.

@section prim_aspects Primitive aspects

@c Graphic3d_Aspects is a class defining **display properties** of a primitive array (@c Graphic3d_Group::SetGroupPrimitivesAspect()) -
_**material**, **shading model**, **color**, **texture maps**, **blending mode**, **line width**_ and others.

There are also subclasses @c Graphic3d_AspectFillArea3d (triangles), @c Graphic3d_AspectLine3d (lines), @c Graphic3d_AspectMarker3d (markers)
and @c Graphic3d_AspectText3d (text labels) defined as specializations for a specific primitive array type.
These subclasses exist for historical reasons and are treated by renderers in exactly the same way.

It is technically possible to create transient aspects directly within @c @::Compute() method like this:

~~~~{.cpp}
void MyAisObject::Compute (const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                           const Handle(Prs3d_Presentation)& thePrs,
                           const Standard_Integer theMode)
{
  Handle(Graphic3d_Aspects) anAspects = new Graphic3d_Aspects();
  anAspects->SetShadingModel (Graphic3d_TypeOfShadingModel_Unlit);
  anAspects->SetColor (Quantity_NOC_RED);
  Handle(Graphic3d_Group) aGroup = thePrs->NewGroup();
  aGroup->SetGroupPrimitivesAspect (anAspects);
  ...
}
~~~~

While this code would work as expected, but prevents further dynamic updates of presentation aspects without recomputing entire presentation.
Instead, it is preferred taking attributes from @c PrsMgr_PresentableObject::myDrawer / @c @::Attributes() or storing custom attributes as class fields.
@c Prs3d_Drawer defines a set of attributes used by @c AIS presentation builders, but the same parameters might be used by a custom builder as well.

It is also preferred preallocating attributes in the class constructor.
This would allow changing attributes without recomputing the entire presentation - just by calling @c PrsMgr_PresentableObject::SynchronizeAspects() after modifications.
Our custom object uses @c myDrawer->ShadingAspect() and @c myDrawer->WireAspect() aspects, so let's initialize them explicitly - assign silver material for shading and green color to line segments:

~~~~{.cpp}
MyAisObject::MyAisObject()
{
  SetHilightMode (MyDispMode_Highlight);
  myDrawer->SetupOwnShadingAspect();
  myDrawer->ShadingAspect()->SetMaterial (Graphic3d_NameOfMaterial_Silver);
  myDrawer->SetWireAspect (new Prs3d_LineAspect (Quantity_NOC_GREEN, Aspect_TOL_SOLID, 2.0));
}
~~~~

@section quadric_builders Quadric builders

Previously, we've used @c StdPrs_ShadedShape for displaying cylinder geometry.
The @c Prs3d package provides a simpler way for displaying geometry like cylinders, spheres and toruses - based on the @c Prs3d_ToolQuadric interface.
This interface allows bypassing creation of a complex B-Rep (@c TopoDS_Shape) definition of a simple geometry, and to avoid using general-purpose tessellators like @c BRepMesh.

> This difference could be negligible for a small number of such objects, but might become considerable for larger amounts.
> The B-Rep definition of a valid cylinder includes 2 unique @c TopoDS_Vertex, 3 @c TopoDS_Edge, 3 @c TopoDS_Wire, 3 @c TopoDS_Face, 1 @c TopoDS_Shell and 1 @c TopoDS_Solid.
> Internally each @c TopoDS_Edge also defines curves (@c Geom_Curve as well as 2D parametric @c Geom2d_Curve) and each @c TopoDS_Face defines analytical surface (@c Geom_Surface).
> Meshing such geometry with the help of @c BRepMesh is much more complicated than one may think.
> A plenty of data structures (memory!) and computations (time!) for displaying a geometry that could be triangulated by a simple for loop.

@c Prs3d_ToolQuadric solves this problem by creating a triangulation for such kinds of shapes in a straight-forward way.
Let's try using @c Prs3d_ToolCylinder in our sample:

~~~~{.cpp}
void MyAisObject::Compute (const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                           const Handle(Prs3d_Presentation)& thePrs,
                           const Standard_Integer theMode)
{
  const double aRadius = 100.0, aHeight = 100.0;
  TopoDS_Shape aShape = BRepPrimAPI_MakeCylinder (aRadius, aHeight);
  if (theMode == MyDispMode_Main)
  {
	//StdPrs_ShadedShape::Add (thePrs, aShape, myDrawer); // add shading
	//StdPrs_WFShape::Add (thePrs, aShape, myDrawer); // add wireframe
	Handle(Graphic3d_ArrayOfTriangles) aTris =
     Prs3d_ToolCylinder::Create (aRadius, aRadius, aHeight, 10, 10, gp_Trsf());
	Handle(Graphic3d_Group) aGroupTris = thePrs->NewGroup();
	aGroupTris->SetGroupPrimitivesAspect (myDrawer->ShadingAspect()->Aspect());
	aGroupTris->AddPrimitiveArray (aTris);
	...
  }
  ...
}
~~~~

@figure{ais_object_step3_quadrics_10.png,"@c Prs3d_ToolCylinder (10 slices).",409} height=409px

Well... that looks a little bit edgy.
Quadric builder creates a triangulation taking the following parameters:
- Geometry parameters.
  (in case of a cylinder - base radius, top radius and height).
- Number of subdivisions along U (slices) and V (stacks) parameters.
  In some cases only one parametric scope matters.
- Transformation @c gp_Trsf to apply
  (original geometry is defined within some reference coordinate system).

Let's increase number of subdivisions from _10_ to _25_:
~~~~{.cpp}
Handle(Graphic3d_ArrayOfTriangles) aTris =
  Prs3d_ToolCylinder::Create (aRadius, aRadius, aHeight, 25, 25, gp_Trsf());
~~~~

@figure{ais_object_step3_quadrics_25.png,"@c Prs3d_ToolCylinder (25 slices).",409} height=409px

It looks much better now! Note that @c Prs3d_ToolCylinder could be used for building both cones and cylinders depending on top/bottom radius definition.

There is one issue though - our cylinder doesn't have top and bottom anymore!
To fix this problem we will use one more quadric builder @c Prs3d_ToolDisk:

~~~~{.cpp}
void MyAisObject::Compute (const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                           const Handle(Prs3d_Presentation)& thePrs,
                           const Standard_Integer theMode)
{
  const double aRadius = 100.0, aHeight = 100.0;
  if (theMode == MyDispMode_Main)
  {
    Prs3d_ToolCylinder aCyl (aRadius, aRadius, aHeight, 25, 25);
    Prs3d_ToolDisk aDisk (0.0, aRadius, 25, 1);

    Handle(Graphic3d_ArrayOfTriangles) aTris =
      new Graphic3d_ArrayOfTriangles (aCyl.VerticesNb() + 2 * aDisk.VerticesNb(),
                                      3 * (aCyl.TrianglesNb() + 2 * aDisk.TrianglesNb()),
                                      Graphic3d_ArrayFlags_VertexNormal);
    aCyl .FillArray (aTris, gp_Trsf());
    aDisk.FillArray (aTris, gp_Trsf());

    gp_Trsf aDisk2Trsf;
    aDisk2Trsf.SetTransformation (gp_Ax3 (gp_Pnt (0.0, 0.0, aHeight), -gp::DZ(), gp::DX()), gp::XOY());
    aDisk.FillArray (aTris, aDisk2Trsf);

    Handle(Graphic3d_Group) aGroupTris = thePrs->NewGroup();
    aGroupTris->SetGroupPrimitivesAspect (myDrawer->ShadingAspect()->Aspect());
    aGroupTris->AddPrimitiveArray (aTris);
    aGroupTris->SetClosed (true);
    ...
  }
}
~~~~

Now our cylinder looks solid! The sample above merges two triangulations into a single one instead of appending each primitive array individually.

This looks like a minor difference, but it might have a _dramatic impact on performance_ in case of a large scene,
as each `Graphic3d_ArrayOfPrimitives` is mapped into a dedicated draw call at graphic driver (OpenGL) level.

@figure{ais_object_step3_quadrics_fin.png,"@c Prs3d_ToolCylinder + @c Prs3d_ToolDisk.",409} height=409px

As an exercise, let's try computing a triangulation for cylinder disk without help of @c Prs3d_ToolDisk builder:

~~~~{.cpp}
void MyAisObject::Compute (const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                           const Handle(Prs3d_Presentation)& thePrs,
                           const Standard_Integer theMode)
{
  const double aRadius = 100.0, aHeight = 100.0;
  if (theMode == MyDispMode_Main)
  {
    const int aNbSlices = 25;
    Prs3d_ToolCylinder aCyl (aRadius, aRadius, aHeight, aNbSlices, aNbSlices);
    Handle(Graphic3d_ArrayOfTriangles) aTris =
      new Graphic3d_ArrayOfTriangles (aCyl.VerticesNb(),
                                      3 * (aCyl.TrianglesNb()),
                                      Graphic3d_ArrayFlags_VertexNormal);
    aCyl.FillArray (aTris, gp_Trsf());

    Handle(Graphic3d_ArrayOfTriangles) aTris2 =
      new Graphic3d_ArrayOfTriangles (aNbSlices + 1, aNbSlices * 3, Graphic3d_ArrayFlags_VertexNormal);
    aTris2->AddVertex (gp_Pnt (0.0, 0.0, aHeight), -gp::DZ());
    for (int aSliceIter = 0; aSliceIter < aNbSlices; ++aSliceIter)
    {
      double anAngle = M_PI * 2.0 * double(aSliceIter) / double(aNbSlices);
      aTris2->AddVertex (gp_Pnt (Cos (anAngle) * aRadius, Sin (anAngle) * aRadius, aHeight), -gp::DZ());
    }
    for (int aSliceIter = 0; aSliceIter < aNbSlices; ++aSliceIter)
    {
      aTris2->AddEdges (1, aSliceIter + 2, aSliceIter + 1 < aNbSlices ? (aSliceIter + 3) : 2);
    }

    Handle(Graphic3d_Group) aGroupTris = thePrs->NewGroup();
    aGroupTris->SetGroupPrimitivesAspect (myDrawer->ShadingAspect()->Aspect());
    aGroupTris->AddPrimitiveArray (aTris);
    aGroupTris->AddPrimitiveArray (aTris2);
    ...
  }
}
~~~~

@figure{ais_object_step3_quadrics_disk.png,"Manually triangulated disk.",409} height=409px

The disk is here, but it has a strange color - like it is not affected by lighting.
This happens when vertex normals are defined incorrectly.
In our case we defined disk normal as @c -DZ (see the second argument of @c Graphic3d_ArrayOfTriangles::AddVertex()),
but normal direction should be also aligned to triangulation winding rule.
Graphic driver defines the front side of triangle using clockwise order of triangle nodes, and normal should be defined for a front side of triangle - e.g. it should be @c gp::DZ() in our case.
After reversing vertex normal direction, cylinder looks exactly like when @c Prs3d_ToolDisk was used.

Front / back face orientation might be displayed using different material based on @c Graphic3d_Aspects::SetDistinguish() flag and @c @::FrontMaterial() / @c @::BackMaterial() setup.

@section ais_selection Computing selection
In the first part of the tutorial we have created a custom @c AIS object @c MyAisObject computing presentation by implementing the @c PrsMgr_PresentableObject::Compute() interface.
In this part we will extend our object with interactive capabilities and make it selectable through implementing @c SelectMgr_SelectableObject interface.

Let's do the first step and put into @c @::ComputeSelection() method some logic.
This method should fill in the @c SelectMgr_Selection argument with @c SelectMgr_SensitiveEntity entities defining selectable elements - triangulations, polylines, points and their composition.
@c Select3D_SensitiveBox is probably the simplest way to define selectable volume - by it's bounding box:

~~~~{.cpp}
void MyAisObject::ComputeSelection (const Handle(SelectMgr_Selection)& theSel,
                                    const Standard_Integer theMode)
{
  const double aRadius = 100.0, aHeight = 100.0;
  TopoDS_Shape aShape = BRepPrimAPI_MakeCylinder (aRadius, aHeight);
  Bnd_Box aBox;
  BRepBndLib::Add (aShape, aBox);
  Handle(SelectMgr_EntityOwner) anOwner = new SelectMgr_EntityOwner (this);
  Handle(Select3D_SensitiveBox) aSensBox = new Select3D_SensitiveBox (anOwner, aBox);
  theSel->Add (aSensBox);
}
~~~~

@c SelectMgr_EntityOwner is a key object in selection logic - it serves as an identifier of a pickable object or it's part.
You may see this object in methods like @c AIS_InteractiveContext::DetectedOwner(), **Owners** are stored within the list of selection objects @c AIS_Selection
and it received by methods like @c AIS_InteractiveContext::SetSelected() and @c AIS_InteractiveContext::AddOrRemoveSelected().
From the Selector's point of view, @c AIS_InteractiveObject is just a drawer for @c SelectMgr_EntityOwner.

The _**0th selection mode**_ normally defines a single Owner of the entire object.
To make a composite object selectable as whole, we add to Selection as many SensitiveEntity as necessary referring to the same Owner.
It might look confusing from first glance, that @c SelectMgr_SensitiveEntity stores @c SelectMgr_EntityOwner as a class field, and not in the opposite way
(@c SelectMgr_EntityOwner doesn't store the list of @c SelectMgr_SensitiveEntity defining it's picking volume).

For local selection (selection of object parts) we create individual Owners for each part and add SensitiveEntity to Selection in the same way.
Owner may store an additional identifier as a class field, like @c StdSelect_BRepOwner stores @c TopoDS_Shape as an identifier of picked sub-shape with @c AIS_Shape object.

In a similar way as @c StdPrs_ShadedShape is a **presentation builder** for @c TopoDS_Shape, the @c StdSelect_BRepSelectionTool can be seen as a standard **selection builder** for shapes:

~~~~{.cpp}
void MyAisObject::ComputeSelection (const Handle(SelectMgr_Selection)& theSel,
                                    const Standard_Integer theMode)
{
  const double aRadius = 100.0, aHeight = 100.0;
  TopoDS_Shape aShape = BRepPrimAPI_MakeCylinder (aRadius, aHeight);
  Standard_Real aDefl = StdPrs_ToolTriangulatedShape::GetDeflection (aShape, myDrawer);
  StdSelect_BRepSelectionTool::Load (theSel, this, aShape, TopAbs_SHAPE, aDefl,
                                     myDrawer->DeviationAngle(),
                                     myDrawer->IsAutoTriangulation());
}
~~~~

Internally, @c StdSelect_BRepSelectionTool iterates over sub-shapes and appends to the Selection (@c theSel) entities like @c Select3D_SensitiveTriangulation (for faces) and @c Select3D_SensitiveCurve (for edges).

Previously, we have used @c Prs3d_ToolCylinder to triangulate a cylinder, so let's try to construct @c Select3D_SensitivePrimitiveArray from the same triangulation:

~~~~{.cpp}
void MyAisObject::ComputeSelection (const Handle(SelectMgr_Selection)& theSel,
                                    const Standard_Integer theMode)
{
  const double aRadius = 100.0, aHeight = 100.0;
  Handle(SelectMgr_EntityOwner) anOwner = new SelectMgr_EntityOwner (this);
  Handle(Graphic3d_ArrayOfTriangles) aTris =
    Prs3d_ToolCylinder::Create (aRadius, aRadius, aHeight, 25, 25, gp_Trsf());
  Handle(Select3D_SensitivePrimitiveArray) aSensTri =
    new Select3D_SensitivePrimitiveArray (anOwner);
  aSensTri->InitTriangulation (aTris->Attributes(), aTris->Indices(),
                               TopLoc_Location());
  theSel->Add (aSensTri);
}
~~~~

Selection is computed independently from presentation, so that they don't have to match each other.
But inconsistency between presentation and selection might confuse a user, when he will not be able to pick an object clearly displayed under the mouse cursor.
These issues might happen, for example, when selection uses tessellated representation of the same geometry computed with different parameters (different number of subdivisions, or different deflection parameters).

As in case of @c @::Compute(), it makes sense defining some enumeration of **selection modes** supported by specific object and reject unsupported ones to avoid unexpected behavior:

~~~~{.cpp}
void MyAisObject::ComputeSelection (const Handle(SelectMgr_Selection)& theSel,
                                    const Standard_Integer theMode)
{
  if (theMode != 0) { return; }
  ...
}
~~~~

Unlike display modes, @c AIS_InteractiveContext allows activating an arbitrary combination of selection modes.
A user should be careful to activate only the modes that actually make sense and may work together.

Selection mode to activate could be specified while displaying the object (passing _**-1**_ instead of _**0**_ would display an object with deactivated selection):

~~~~{.cpp}
Handle(AIS_InteractiveContext) theCtx;
Handle(MyAisObject) aPrs = new MyAisObject();
theCtx->Display (aPrs, MyAisObject::MyDispMode_Main, 0, false);
~~~~

Later on @c AIS_InteractiveContext::SetSelectionModeActive(), or it's wrappers @c AIS_InteractiveContext::Activate() and @c AIS_InteractiveContext::Deactivate(),
could be used to enable or disable desired selection modes one by one.

@section sel_owner_highlight Highlighting selection owner

As has been mentioned in the previous section, @c SelectMgr_EntityOwner is a key object which can be used as an identifier of selectable part(s).
Naturally, you might want to subclass it to put some application-specific ids for identification of selected parts.
But there are more things you may do with the Owner class like customized highlighting.

Let's start from the beginning and define a custom Owner class:

~~~~{.cpp}
class MyAisOwner : public SelectMgr_EntityOwner
{
  DEFINE_STANDARD_RTTI_INLINE(MyAisOwner, SelectMgr_EntityOwner)
public:
  MyAisOwner (const Handle(MyAisObject)& theObj, int thePriority = 0)
  : SelectMgr_EntityOwner (theObj, thePriority) {}

  virtual void HilightWithColor (const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                                 const Handle(Prs3d_Drawer)& theStyle,
                                 const Standard_Integer theMode) override
  { base_type::HilightWithColor (thePrsMgr, theStyle, theMode); }

  virtual void Unhilight (const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                          const Standard_Integer theMode) override
  { base_type::Unhilight  (thePrsMgr, theMode); }
protected:
  Handle(Prs3d_Presentation) myPrs;
};
~~~~

@c SelectMgr_EntityOwner doesn't define any pure virtual methods, and can be instanced straight ahead, like it was done within @c MyAisObject::ComputeSelection() implementation above.
Let's revert usage of a dedicated display mode for highlighting (remove @c SetHilightMode() in @c MyAisObject constructor) and use our new class @c MyAisOwner within @c @::ComputeSelection():

~~~~{.cpp}
MyAisObject::MyAisObject()
{
  //SetHilightMode (MyDispMode_Highlight);
  myDrawer->SetupOwnShadingAspect();
  ...
}

void MyAisObject::ComputeSelection (const Handle(SelectMgr_Selection)& theSel,
                                    const Standard_Integer theMode)
{
  const double aRadius = 100.0, aHeight = 100.0;
  Handle(MyAisOwner) anOwner = new MyAisOwner (this);
  ...
}
~~~~

The further logic creating sensitive entities and filling in Selection could be left as is.
Substitution of @c SelectMgr_EntityOwner with @c MyAisOwner currently doesn't change behavior and we see highlighting of the entire object through color modulation.
This is because default implementation of @c SelectMgr_EntityOwner for highlighting logic looks like this (simplified):

~~~~{.cpp}
void SelectMgr_EntityOwner::HilightWithColor (
  const Handle(PrsMgr_PresentationManager)& thePrsMgr,
  const Handle(Prs3d_Drawer)& theStyle,
  const Standard_Integer theMode)
{
  const Graphic3d_ZLayerId aHiLayer =
      theStyle->ZLayer() != Graphic3d_ZLayerId_UNKNOWN
    ? theStyle->ZLayer()
    : mySelectable->ZLayer();
  thePrsMgr->Color (mySelectable, theStyle, theMode, NULL, aHiLayer);
}
~~~~

@figure{ais_object_step4_highlight1.png,"Default behavior of @c SelectMgr_EntityOwner::HilightWithColor().",409} height=409px

Now, let's override the @c SelectMgr_EntityOwner::HilightWithColor() method and display a bounding box presentation:

~~~~{.cpp}
void MyAisOwner::HilightWithColor (const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                                   const Handle(Prs3d_Drawer)& theStyle,
                                   const Standard_Integer theMode)
{
  if (myPrs.IsNull())
  {
    myPrs = new Prs3d_Presentation (thePrsMgr->StructureManager());
    MyAisObject* anObj = dynamic_cast<MyAisObject*> (mySelectable);
    anObj->Compute (thePrsMgr, myPrs, MyAisObject::MyDispMode_Highlight);
  }
  if (!thePrsMgr->IsImmediateModeOn())
  {
    myPrs->Display();
  }
}
~~~~

@c SelectMgr_EntityOwner::HilightWithColor() doesn't receive a presentation to fill in as an argument; highlight presentation should be manually created and even explicitly displayed on the screen.
To avoid code duplication, the code above reuses @c MyAisObject::Compute() already implementing computation of highlight presentation.

@figure{ais_object_step4_highlight2.png,"Result of custom implementation @c MyAisOwner::HilightWithColor().",409} height=409px

The visual result of the selected object looks exactly the same as when we've used a dedicated highlight mode.
One thing became broken, though - highlighting remains displayed even after clearing selection.
To fix this issue, we need implementing @c SelectMgr_EntityOwner::Unhilight() and hide our custom presentation explicitly:

~~~~{.cpp}
void MyAisOwner::Unhilight (const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                            const Standard_Integer theMode)
{
  if (!myPrs.IsNull()) { myPrs->Erase(); }
}
~~~~

Another problem is that the object is no longer dynamically highlighted.
To fix that we need to handle @c PrsMgr_PresentationManager::IsImmediateModeOn() specifically.
Within this mode turned ON, presentation should be displayed on the screen with help of @c PrsMgr_PresentationManager::AddToImmediateList() method
(it will be cleared from the screen automatically on the next mouse movement):

~~~~{.cpp}
void MyAisOwner::HilightWithColor (const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                                   const Handle(Prs3d_Drawer)& theStyle,
                                   const Standard_Integer theMode)
{
  if (myPrs.IsNull())
  {
    myPrs = new Prs3d_Presentation (thePrsMgr->StructureManager());
    MyAisObject* anObj = dynamic_cast<MyAisObject*> (mySelectable);
    anObj->Compute (thePrsMgr, myPrs, MyAisObject::MyDispMode_Highlight);
  }
  if (thePrsMgr->IsImmediateModeOn())
  {
    Handle(Prs3d_PresentationShadow) aShadow =
      new Prs3d_PresentationShadow (thePrsMgr->StructureManager(), myPrs);
    aShadow->SetZLayer (Graphic3d_ZLayerId_Top);
    aShadow->Highlight (theStyle);
    thePrsMgr->AddToImmediateList (aShadow);
  }
  else
  {
    myPrs->Display();
  }
}
~~~~

We may create two dedicated presentations for dynamic highlighting or reuse existing one for both cases with help of a transient object @c Prs3d_PresentationShadow.

Let's go further and make dynamic highlighting a little bit more interesting - by drawing a surface normal at the point where mouse picked the object:

~~~~{.cpp}
void MyAisOwner::HilightWithColor (const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                                   const Handle(Prs3d_Drawer)& theStyle,
                                   const Standard_Integer theMode)
{
  MyAisObject* anObj = dynamic_cast<MyAisObject*> (mySelectable);
  if (thePrsMgr->IsImmediateModeOn())
  {
    Handle(StdSelect_ViewerSelector) aSelector =
      anObj->InteractiveContext()->MainSelector();
    SelectMgr_SortCriterion aPickPnt;
    for (int aPickIter = 1; aPickIter <= aSelector->NbPicked(); ++aPickIter)
    {
      if (aSelector->Picked (aPickIter) == this)
      {
        aPickPnt = aSelector->PickedData (aPickIter);
        break;
      }
    }

    Handle(Prs3d_Presentation) aPrs = mySelectable->GetHilightPresentation (thePrsMgr);
    aPrs->SetZLayer (Graphic3d_ZLayerId_Top);
    aPrs->Clear();
    Handle(Graphic3d_Group) aGroup = aPrs->NewGroup();
    aGroupPnt->SetGroupPrimitivesAspect (theStyle->ArrowAspect()->Aspect());
    gp_Trsf aTrsfInv = mySelectable->LocalTransformation().Inverted();
    gp_Dir  aNorm (aPickPnt.Normal.x(), aPickPnt.Normal.y(), aPickPnt.Normal.z());
    Handle(Graphic3d_ArrayOfTriangles) aTris =
      Prs3d_Arrow::DrawShaded (gp_Ax1(aPickPnt.Point, aNorm).Transformed (aTrsfInv),
        1.0, 15.0,
        3.0, 4.0, 10);
    aGroupPnt->AddPrimitiveArray (aTris);
    thePrsMgr->AddToImmediateList (aPrs);
  }
}
~~~~

Code above does not store our new highlight presentation as a property of @c MyAisOwner, and instead uses @c SelectMgr_SelectableObject::GetHilightPresentation() method
to create a presentation stored directly inside of our interactive object.

Next trick is passing through the last picking results in @c StdSelect_ViewerSelector.
Dynamic highlighting is expected to be called right after picking, so that highlighted Owner should be always found in picking results.
@c StdSelect_ViewerSelector::Picked() returns entities in the descending order of their distance from picking ray origin (mouse cursor);
normally our Owner should be the very first one in this list when no selection filters are assigned to @c AIS_InteractiveContext.

@c SelectMgr_SortCriterion provides us useful information like 3D point on detected object lying on the picking ray, and surface normal direction at this point (actually, it would be a normal to a picked triangle),
which we display as an arrow with help of @c Prs3d_Arrow presentation builder.

@figure{ais_object_step4_highlight3.png,"Surface normal on mouse over.",409} height=409px

Result looks pretty nice on the screenshot, but has interaction problems - once displayed, an arrow is no longer updated with further mouse movements.
But this behavior is not a bug - @c AIS calls @c MyAisOwner::HilightWithColor() only when picking Owner changes to avoid unnecessary Viewer updates.
To override this behavior, we may override @c SelectMgr_EntityOwner::IsForcedHilight() option:

~~~~{.cpp}
class MyAisOwner : public SelectMgr_EntityOwner
{
...
  virtual bool IsForcedHilight() const override { return true; }
};
~~~~

This solves the problem within our specific use case.
Keep in mind that most objects don't need updating highlight presentation on every mouse move;
overriding this flag everywhere would be a waste of resources and may cause performance issues - use it sparingly.

@section highlight_apporaches Highlighting approaches

@c AIS provides one more alternative to handle presentation highlighting, which is managed by option @c SelectMgr_SelectableObject::IsAutoHilight().
By default, this option is turned ON and redirects highlighting logic to @c SelectMgr_EntityOwner::HilightWithColor() demonstrated in the previous section.
Turning this option OFF redirects highlighting logic to the interactive object itself @c SelectMgr_SelectableObject::HilightSelected().

Apart from moving the logic from Owner to Interactive Object, this approach allows handling highlighting of all selected Owners within the same Object at once and sharing a common presentation
instead of per-Owner presentation - improving performance and reducing memory utilization in case of a large number of small selectable elements, like mesh nodes in @c MeshVS_Mesh object.

The further optimization of such a scenario would be using a single Owner for the entire Object
storing the list of selected elements within the Owner itself - as utilized by @c AIS_PointCloud object for highlighting individual points.

We wouldn't describe these advanced techniques here in detail - let's just summarize main highlighting approaches available in @c AIS:
- Highlighting of a main presentation of Interactive Object (active display mode)
  filled in by @c PrsMgr_PresentableObject::Compute()
  and displayed with color modulation by @c AIS logic.
  - Example: @c AIS_TextLabel.
- Highlighting of a secondary presentation of Interactive Object
  filled in by @c PrsMgr_PresentableObject::Compute()
  and displayed with color modulation by @c AIS logic.
  - Example: @c AIS_Shape, displayed in @c AIS_Shaded display mode and highlighted using @c AIS_Wireframe display mode (default behavior).
    See also @c PrsMgr_PresentableObject::SetHilightMode().
- Highlight presentation stored within a custom @c SelectMgr_EntityOwner
  and managed by @c SelectMgr_EntityOwner::HilightWithColor().
  - Example: @c StdSelect_BRepOwner for selection of sub-shapes.
- Custom highlight presentation stored within Interactive Object itself
  (see @c SelectMgr_SelectableObject::GetHilightPresentation() / @c @::GetSelectPresentation() methods).
  - Filled in by @c SelectMgr_EntityOwner::HilightWithColor()
    with @c SelectMgr_SelectableObject::IsAutoHilight() turned ON.<br>
    Example: @c AIS_PointCloud.
  - Filled in by @c SelectMgr_SelectableObject::HilightSelected()
    with @c SelectMgr_SelectableObject::IsAutoHilight() turned OFF.<br>
    Example: @c MeshVS_Mesh.
- Main presentation of Interactive Object (active display mode)
  filled in by @c PrsMgr_PresentableObject::Compute()
  and manually updated (recomputed or modified aspects) on highlight events.
  - Example: @c AIS_Manipulator.

The number of options looks overwhelming but in general, it is better to stick to the simplest approach working for you and consider alternatives only when you have to.

@section mouse_click Mouse click

Dynamic highlighting is only one of scenarios where @c SelectMgr_EntityOwner could be useful.
Another feature is an interface for handling a mouse click @c SelectMgr_EntityOwner @c @::HandleMouseClick().

This interface is useful for defining some user interface elements like buttons, and most likely your application will use a more comprehensive GUI framework for this purpose instead of @c AIS.
But let's have some fun and make our object to change a color on each mouse click:

~~~~{.cpp}
class MyAisOwner : public SelectMgr_EntityOwner
{
...
  virtual bool HandleMouseClick (const Graphic3d_Vec2i& thePoint,
                                 Aspect_VKeyMouse theButton,
                                 Aspect_VKeyFlags theModifiers,
                                 bool theIsDoubleClick) override;
};

bool MyAisOwner::HandleMouseClick (const Graphic3d_Vec2i& thePoint,
                                   Aspect_VKeyMouse theButton,
                                   Aspect_VKeyFlags theModifiers,
                                   bool theIsDoubleClick)
{
  static math_BullardGenerator aRandGen;
  Quantity_Color aRandColor (float(aRandGen.NextInt() % 256) / 255.0f,
                             float(aRandGen.NextInt() % 256) / 255.0f,
                             float(aRandGen.NextInt() % 256) / 255.0f,
                             Quantity_TOC_sRGB);
  mySelectable->Attributes()->ShadingAspect()->SetColor(aRandColor);
  mySelectable->SynchronizeAspects();
  return true;
}
~~~~

Looks pretty simple. Now let's make things more interesting and launch some simple object animation on each click.
We use a couple of global (@c static) variables in our sample for simplicity - don't do that in a real production code.

~~~~{.cpp}
class MyAisOwner : public SelectMgr_EntityOwner
{
...
  void SetAnimation (const Handle(AIS_Animation)& theAnim)
  { myAnim = theAnim; }
...
  Handle(AIS_Animation) myAnim;
};

bool MyAisOwner::HandleMouseClick (const Graphic3d_Vec2i& thePoint,
                                   Aspect_VKeyMouse theButton,
                                   Aspect_VKeyFlags theModifiers,
                                   bool theIsDoubleClick)
{
  static bool isFirst = true;
  isFirst = !isFirst;
  MyAisObject* anObj = dynamic_cast<MyAisObject*> (mySelectable);
  gp_Trsf aTrsfTo;
  aTrsfTo.SetRotation (gp_Ax1 (gp::Origin(), gp::DX()),
                       isFirst ? M_PI * 0.5 : -M_PI * 0.5);
  gp_Trsf aTrsfFrom = anObj->LocalTransformation();
  Handle(AIS_AnimationObject) anAnim =
    new AIS_AnimationObject ("MyAnim", anObj->InteractiveContext(),
                             anObj, aTrsfFrom, aTrsfTo);
  anAnim->SetOwnDuration (2.0);

  myAnim->Clear();
  myAnim->Add (anAnim);
  myAnim->StartTimer (0.0, 1.0, true);
  return true;
}
~~~~

Animation is a complex topic that is worth a dedicated article - let's not go too deep in detail here.
To perform animation in a non-interrupted way, it should be handled by some class like @c AIS_ViewController, which is responsible for managing user input events and for 3D viewer updates.
To utilize it, you need adding a custom object animation to @c AIS_ViewController::ObjectsAnimation() or adding custom view animation to @c AIS_ViewController::ViewAnimation().
Somewhere in application this might look like this:

~~~~{.cpp}
Handle(AIS_InteractiveContext) theCtx;
Handle(AIS_ViewController) theViewCtrl;
Handle(MyAisObject) aPrs = new MyAisObject();
aPrs->SetAnimation (theViewCtrl->ObjectsAnimation());
theCtx->Display (aPrs, MyAisObject::MyDispMode_Main, 0, false);
~~~~

@section final Final result

The final sample could be seen by calling @c QATutorialAisObject command from Draw Harness plugin @c QAcommands (@c TKQADraw toolkit):

~~~~
pload VISUALIZATION QAcommands
vinit View1
QATutorialAisObject p
vfit
~~~~

You may also take a look onto source code of this command at @c src/QADraw/QADraw_Tutorials.cxx if you have some problems following the tutorial.
