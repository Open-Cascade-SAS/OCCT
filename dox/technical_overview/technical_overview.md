Technical Overview {#technical_overview}
========================================

@tableofcontents

@section OCCT_TOVW_SECTION_1 Product Overview

Open CASCADE Technology is an object-oriented C++ class library designed for rapid production of sophisticated domain-specific design applications. A typical application developed using OCCT deals with two or three-dimensional (2D or 3D) geometric modeling
in general-purpose or specialized Computer Aided Design (CAD) systems, manufacturing
or analysis applications, simulation applications, or illustration tools. OCCT Object
Libraries help you to develop your applications significantly faster.

@image html /technical_overview/images/technical_overview_over.png
@image latex /technical_overview/images/technical_overview_over.png

The OCCT Library provides the following services:


  * 2D and 3D geometric modeling toolkits which allow you to model any type of object:
    * Creating primitives such as prism, cylinder, cone and torus 
    * Performing Boolean operations (addition, subtraction and intersection) 
    * Tweaking constructions using fillets, chamfers and drafts 
    * Modeling constructions using offsets, shelling, hollowing and sweeps 
    * Computing properties such as surface, volume, center of gravity, curvature 
    * Computing geometry using projection, interpolation, approximation 
  * Visualization services that allow you to manage object display and manipulate views, for example:
    * 3D rotation 
    * Zoom 
    * Shading 
  * The application framework features: 
    * Association between non-geometric application data and geometry 
    * Parameterization of models 
    * Java Application Desktop (JAD), a framework for creating your Graphical User Interfaces (GUI) 
  * Data exchange providing import and export functions of OCCT models to and from standard formats such as IGES and STEP 

OCCT Library is developed and marketed by OPEN CASCADE Company. The library is designed
to be truly modular and extensible. As such, they separate C++ classes for:

  * Defining data structures (geometric modeling, display and graphic selection) 
  * Implementing complex algorithms 
  * Providing Application Programming Interfaces (APIs) 


Related classes are grouped into packages to prevent any class-name conflicts; 
C++ class-names are prefixed by a package name. For example, all classes defining 
3D geometric objects belong to the Geompackage. 
In Geom, the class implementing Bezier surfaces is called BezierSurface, and its full name is <i> Geom_BezierSurface</i>.

Packages are then archived into libraries, to which you can link your application.

Finally, libraries are grouped into six modules: Foundation Classes, 
Modeling Data, Modeling Algorithms, Visualization, Data Exchange and Application Framework. 

In addition Draw Test Harness (Draw) provides testing tools for the Visualization, 
Modeling Algorithms, Application Framework and Data Exchange modules. 
These tools include a wide range of interpreted commands which allow experimenting with OCCT.

Refer to the Technical Overview and OCCT documentation for details about the services provided in each module. 

@section OCCT_TOVW_SECTION_2 Foundation Classes


Foundation Classes provide a variety of general-purpose services such as:

  * Primitive types, strings and various types of quantities 
  * Automated management of heap memory 
  * Exception handling 
  * Classes for manipulating data collections
  * Math tools such as vectors, matrices and primitive geometric types 
  * Basic services for saving data in ASCII files 

These services are organized into the following libraries:

  * Kernel Classes 
  * Math Utilities 
  * Basic Persistence

The technical overview provides only a basic description of the libraries. Please, refer for more details to  <a href="user_guides__foundation_classes.html">Foundation Classes User's guide</a>

See also: our web site at <a href="http://www.opencascade.org/support/training/">E-learning and Training</a>.

@subsection OCCT_TOVW_SECTION_2_1 Kernel Classes


### Root Classes


Root Classes, primarily implemented in the Standard package, are the predefined classes on which
all other Open CASCADE Technology classes are built. They provide:

  * Primitive types such as Boolean, Character, Integer or Real 
  * Memory manager based on reference counting for optimizing the allocation and deallocation of large numbers of small C++ objects 
  * <i>Standard_Transient</i> class automating memory management through smart pointers
  * OCCT <i>Handle</i>; most of OCCT classes inherit from this base class.
  * Management of exceptions,
  * Encapsulation of C++ streams.

### Quantities

Quantity classes provide the following services:

  * Definition of primitive types representing most of mathematical and physical quantities
  * Unit conversion tools.    
  * Resources to manage time information such as dates and time periods 
  * Resources to manage color definition 

A mathematical quantity is characterized by the name and the value (real).
A physical quantity is characterized by the name, the value (real) and the unit. 
The unit may be either an international unit complying with the International Unit System (SI) 
or a user defined unit. The unit is managed by the physical quantity user.

The fact that both physical and mathematical quantities are manipulated as real
values means that :

  * They are defined as aliases of real values, so all functions provided by the <i>Standard_Real</i> class are available on each quantity.
  * It is possible to mix several physical quantities in a mathematical or physical formula involving real values.

<i>Quantity</i> package includes all commonly used basic physical quantities. 

### Exceptions

Exception classes list all the exceptions, which can be raised by any OCCT function.

Each exception inherits from Standard_Failure either directly or by inheriting from
another exception.

Exceptions describe anomalies which can occur during the execution of a method. With
the raising of an exception, the normal course of program execution is abandoned.
The execution of actions in response to this situation is called the treatment of
the exception.


The methods try & catch are redefined in OCCT to work properly on any platform. Nevertheless
they call native mechanisms each time it is possible. The only reason not to use
native exceptions is that they may not work properly on some compilers. In this case,
a specific OCCT code is used instead. 


### Strings

Strings are classes that handle dynamically sized sequences of characters based on
ASCII/Unicode UTF-8 (normal 8-bit character type)
and UTF-16/UCS-2 (16-bit character type). They provide editing operations with built-in
memory management which make the relative objects easier to use than ordinary character
arrays.

String classes provide the following services to manipulate character strings:
 * Editing operations on string objects, using a built-in string manager 
 * Handling of dynamically-sized sequences of characters 
 * Conversion from/to ASCII and UTF-8 strings. 

Strings may also be manipulated by handles and therefore, can be shared.

These classes are implemented in <i>TCollection</i> and <i>NCollection</i> packages.


### Collections

Apart from strings, the <i> TCollection</i> package contains classes of dynamically sized
aggregates of data. They include a wide range of collections.

  * Arrays (unidimensional and bidimensional) are generally used for quick access to an item.
  Note that an array is a fixed-sized aggregate. 
  * Sequences are ordered collections of non-unique objects. 
  A sequence item is longer to access than an array item: only an exploration in sequence 
  is efficient (but sequences are not adapted for numerous explorations). 
  Arrays and sequences are commonly used as data structures for more complex objects.
  * Maps are dynamic structures where the size is constantly adapted to the number of inserted
  items and access to an item is the fastest. Maps structures are commonly used in
  cases of numerous explorations: they are typically internal data structures for complex
  algorithms. Sets generate the same results as maps but computation time is considerable.
  * Lists, queues and stacks, which are minor structures similar to sequences but with different
  algorithms to explore them 
  * Specific iterators for sequences, maps, and stacks.

Most collections follow value semantics: their instances are the actual collections,
not handles to a collection. Only arrays, sequences and sets may also be manipulated
by handle, and therefore shared.


Collection classes are generic (C++ template-like), so they can contain 
a variety of objects which do not necessarily inherit from
a unique root class. When you need to use a collection of a given object type, you
must instantiate the collection for this specific type. Once the code for this declaration
is compiled, all functions available on the generic collection are available on your
instantiated class.

Each collection directly used as an argument in Open CASCADE Technology public syntax
is instantiated in an OCCT component using the corresponding generic class in package
<i> TCollection</i>, by means of compiling the CDL declaration of the instance. 
Thus OCCT generic classes require compilation of definitions in the CDL language and therefore
can only be instantiated in WOK.

If you are not using CDL in your project (CDL compilation under WOK is necessary
to instantiate any generic Collection from package <i>TCollection</i>), then you should
use the Collections defined in <i> NCollection</i> package. It contains definitions of the
same generic collection classes described above, but in a form of C++ templates.
Therefore, to instantiate any collection type no additional support is required beyond
the ANSI C++ compiler.

@subsection OCCT_TOVW_SECTION_2_2 Math Utilities


### Vectors and Matrices


The <i> Vector</i> and <i> Matrix </i> classes provide commonly used mathematical algorithms which
include:

  * Basic calculations involving vectors and matrices; 
  * Computation of eigenvalues and eigenvectors of a square matrix; 
  * Solvers for a set of linear algebraic equations; 
  * Algorithms to find the roots of a set of non-linear equations; 
  * Algorithms to find the minimum function of one or more independent variables. 

These classes also provide a data structure to represent any expression,
relation, or function used in mathematics, including the assignment of variables.

Vectors and matrices have arbitrary ranges which must be defined at declaration time
and cannot be changed after declaration. 

~~~
    math_Vector v(1, 3);
    // a vector of dimension 3 with range (1..3)

    math_Matrix m(0, 2, 0, 2);
    // a matrix of dimension 3x3 with range (0..2, 0..2)

    math_Vector v(N1, N2);
    // a vector of dimension N2-N1+1 with range (N1..N2)
~~~

Vector and Matrix objects follow "value semantics", that is, they cannot be shared
and are copied though assignment.

~~~
    math_Vector v1(1, 3), v2(0, 2);

    v2 = v1; // v1 is copied into v2
		// a modification of v1 does not affect v2
~~~

Vector and Matrix elements can be retrieved using indexes, which must be in the range
defined upon Vector/Matrix creation. The elements can be initialized with some numerical
value upon creation as well.

~~~
    math_Vector v(1, 3);
    math_Matrix m(1, 3, 1, 3);
    Standard_Real value;

    v(2) = 1.0;
    value = v(1);
    m(1, 3) = 1.0;
    value = m(2, 2);
~~~

Some operations on Vector and Matrix objects may not be legal. In this case an exception
is raised. Two standard exceptions are used:
 *<i>Standard_DimensionError</i> exception is raised when two matrices or vectors involved
in an operation are of incompatible dimensions.
 *<i>Standard_RangeError</i>exception is raised if an attempt to access outside the range
defined upon creation of a vector or a matrix is made.

~~~
    math_Vector v1(1, 3), v2(1, 2), v3(0, 2);

    v1 = v2;    	// error: Standard_DimensionError is raised
    v1 = v3;		// OK: ranges are not equal, but dimensions are compatible
    v1(0) = 2.0;	// error: Standard_RangeError is raised
~~~


### Fundamental Geometry Types

The Fundamental Geometry Types component groups the following packages:
* geometric processor package <i> gp</i>;
* <i>GeomAbs</i> package, which provides enumerations generally used in geometry;

<i>gp</i> package is a STEP-compliant implementation of basic geometric and algebraic
entities, used to define and manipulate elementary data structures.

In particular, <i>gp</i> provides:

  * descriptions of primitive geometric shapes, such as:
	  * Points; 
	  * Vectors; 
	  * Lines; 
	  * Circles and conics; 
	  * Planes and elementary surfaces;
  * positioning of these shapes in space or in a plane by means of an axis or a coordinate system;
  * definition and application of geometric transformations to these shapes:
	  * Translations; 
	  * Rotations; 
	  * Symmetries; 
	  * Scaling transformations; 
	  * Composed transformations;
  * Tools (coordinates and matrices) for algebraic computation.

These functions are defined in 3D space and in the plane.

<i> gp</i> curves and surfaces are analytic: there is no parameterization and no orientation 
on <i>gp</i> entities, i.e. these entities do not provide functions which work with these properties. 
If you need, you may use more evolved data structures provided by <i> Geom</i> 
(in 3D space) and <i> Geom2d</i> (in the plane). However, the definition of <i> gp</i> entities 
is identical to the one of equivalent <i> Geom</i> and <i> Geom2d</i> entities, and they are located
in the plane or in space with the same kind of positioning systems. 
They implicitly contain the orientation, which they express 
on the <i> Geom </i> and <i> Geom2d </i> entities, and they induce the definition of their parameterization.


Therefore, it is easy to give an implicit parameterization to <i> gp</i> curves and surfaces,
which is the parametarization of the equivalent <i> Geom</i> or <i> Geom2d</i> entity. This property
is particularly useful when computing projections or intersections, or for operations
involving complex algorithms where it is particularly important to manipulate the
simplest data structures, i.e. those of <i> gp</i>. Thus, the <i> ElCLib</i> and <i> ElSLib</i> packages
provide functions to compute:

  * the point of parameter u on a 2D or 3D gp curve,
  * the point of parameter (u,v) on a gp elementary surface, and
  * any derivative vector at this point.

Note: the <i> gp</i> entities cannot be shared when they are inside more complex data structures.


### Common Mathematical Algorithms

Common mathematical algorithms provided in OCCT include:

  * Algorithms to solve a set of linear algebraic equations, 
  * Algorithms to find the minimum of a function of one or more independent variables,
  * Algorithms to find roots of one or of a set of non-linear equations, 
  * An algorithm to find the eigenvalues and eigenvectors of a square matrix.

@section OCCT_TOVW_SECTION_3 Modeling Data

Modeling Data supplies data structures to represent 2D and 3D geometric models.

@image html /technical_overview/images/technical_overview_md.png
@image latex /technical_overview/images/technical_overview_md.png

These services are organized into the following libraries:

  * 2D Geometry 
  * 3D Geometry 
  * Geometry Utilities 
  * Topology 

The technical overview provides only a basic description of the libraries. Please, refer for more details to  <a href="user_guides__modeling_data.html">Modeling Data User's guide</a>

3D geometric models are stored in OCCT native BREP format. It is possible to learn
more about it in <a href="occt_brep_format.html">BREP Format Description White Paper</a> 

See also: our web site at <a href="http://www.opencascade.org/support/training/">E-learning and Training</a>.

@subsection OCCT_TOVW_SECTION_3_1 2D Geometry Types

<i> Geom2d</i> package provides an implementation of 2D geometric objects complying with
STEP, part 42. In particular, it provides functions for:
* description of points, vectors and curves,
* their positioning in the plane using coordinate systems,
* their geometric transformation, by applying translations, rotations, symmetries, scaling transformations and combinations thereof.

The key characteristic of <i> Geom2d </i> curves is that they are parameterized. 
Each class provides functions to work with the parametric equation of the curve, 
and, in particular, to compute the point of parameter u on a curve and the derivative vectors of order 1, 2.., N at this point.

As a consequence of the parameterization, a <i> Geom2d </i> curve is naturally oriented.

Parameterization and orientation differentiate elementary <i>Geom2d </i>curves from their
equivalent as provided by <i> gp</i> package. <i>  Geom2d</i> package provides conversion
functions to transform a <i> Geom2d</i> object into a <i> gp</i> object, and vice-versa, when this is possible.

Moreover, <i> Geom2d</i> package provides more complex curves, including Bezier curves,
BSpline curves, trimmed curves and offset curves.

<i> Geom2d </i> objects are organized according to an inheritance structure over several levels.
Thus, an ellipse (specific class <i> Geom2d_Ellipse</i>) is also a conical curve and inherits
from the abstract class <i> Geom2d_Conic</i>, while a Bezier curve (concrete class <i> Geom2d_BezierCurve</i>)
is also a bounded curve and inherits from the abstract class <i> Geom2d_BoundedCurve</i>;
both these examples are also curves (abstract class <i>Geom2d_Curve</i>). Curves, points
and vectors inherit from the abstract class <i> Geom2d_Geometry,</i> which describes the properties
common to any geometric object from the <i>Geom2d</i> package.

This inheritance structure is open and it is possible to describe new objects which
inherit from those provided in the <i>Geom2d</i> package, provided that they respect the
behavior of the classes from which they are to inherit.

Finally, <i> Geom2d</i> objects can be shared within more complex data structures. 
This is why they are used within topological data structures, for example.

<i> Geom2d </i>package uses the services of the <i> gp</i> package to:

  * implement elementary algebraic calculus and basic analytic geometry,
  * describe geometric transformations which can be applied to <i> Geom2d</i> objects,
  * describe the elementary data structures of <i>Geom2d</i> objects.

However, the <i> Geom2d</i> package essentially provides data structures and not algorithms.
You can refer to the <i> GCE2d </i> package to find more evolved construction algorithms for <i> Geom2d </i> objects.

@subsection OCCT_TOVW_SECTION_3_2 3D Geometry Types

The <i> Geom</i> package provides an implementation of 3D geometric objects complying with
STEP, part 42. In particular, it provides functions for:

  * description of points, vectors, curves and surfaces,
    * their positioning in 3D space using axis or coordinate systems, and
    * their geometric transformation, by applying translations, rotations, symmetries, scaling transformations and combinations thereof.

The key characteristic of Geom curves and surfaces is that they are parameterized.
Each class provides functions to work with the parametric equation of the curve or
surface, and, in particular, to compute:

   * the point of parameter u on a curve, or
   * the point of parameters (u, v) on a surface.

together with the derivative vectors of order 1, 2, ... N at this point.

As a consequence of this parameterization, a Geom curve or surface is naturally oriented.

Parameterization and orientation differentiate elementary Geom curves and surfaces from the classes of the same (or similar) names found in the <i> gp</i> package. 
The <i>Geom</i> package also provides conversion functions to transform a Geom object into a <i> gp</i> object, and vice-versa, when such transformation is possible.

Moreover, the <i> Geom </i>package provides more complex curves and surfaces, including:
  * Bezier and BSpline curves and surfaces,
  * swept surfaces, for example surfaces of revolution and surfaces of linear extrusion,
  * trimmed curves and surfaces, and
  * offset curves and surfaces.

Geom objects are organized according to an inheritance structure over several levels.
Thus, a sphere (concrete class <i> Geom_SphericalSurface</i>) is also an elementary surface and inherits from the abstract class <i> Geom_ElementarySurface</i>, while a Bezier surface (concrete class <i> Geom_BezierSurface</i>) is also a bounded surface and inherits from the abstract class <i> Geom_BoundedSurface</i>; both these examples are also surfaces (abstract class <i> Geom_Surface</i>). Curves, points and vectors inherit from the abstract class <i> Geom_Geometry,</i> which describes the properties common to any geometric object from the <i>Geom</i> package.

This inheritance structure is open and it is possible to describe new objects, which inherit from those provided in the Geom package, on the condition that they respect the behavior of the classes from which they are to inherit.

Finally, Geom objects can be shared within more complex data structures. This is why they are used within topological data structures, for example.

The <i> Geom</i> package uses the services of the <i> gp</i> package to:
  * implement elementary algebraic calculus and basic analytic geometry,
  * describe geometric transformations which can be applied to Geom objects,
  * describe the elementary data structures of Geom objects.

However, the Geom package essentially provides data structures and not algorithms.
You can refer to the <i> GC</i> package to find more evolved construction algorithms for
Geom objects.


### Adaptors for Curves and Surfaces

Some Open CASCADE Technology general algorithms may work theoretically on numerous types of curves or surfaces. 
To do this, they simply get the services required of the analysed  curve or surface through an interface so as to a single API,  whatever the type of curve or surface. These interfaces are called adaptors.
For example, <i> Adaptor3d_Curve </i> is the abstract class which provides  the required services by an algorithm which uses any 3d curve.

<i> GeomAdaptor </i>package provides interfaces:

  * On a Geom curve;
  * On a curve lying on a Geom surface;
  * On a Geom surface;

<i> Geom2dAdaptor</i> package provides interfaces :

  * On a <i>Geom2d</i> curve.

<i> BRepAdaptor </i> package provides interfaces:

  * On a Face
  * On an Edge

When you write an algorithm which operates on geometric objects, use <i> Adaptor3d</i> (or <i> Adaptor2d</i>) objects. 
As a result, you can use the algorithm with any kind of object, 
if you provide for this object, an interface derived from *Adaptor3d* or *Adaptor2d*.
These interfaces are easy to use: simply create an adapted curve or surface from a *Geom2d* curve, 
and then use this adapted curve as an argument for the algorithm which requires it.


@subsection OCCT_TOVW_SECTION_3_3 Geometry Utilities

This library provides standard high-level functions in 2D and 3D geometry such as:

  * Direct construction of algorithms; 
  * Approximation of curves and surfaces from points; 
  * Conversion of more elementary geometry to BSpline curves and surfaces; 
  * Calculation of points on a 2D or 3D curve; 
  * Calculation of extrema between two geometries. 

### Direct Construction

The <i> gp</i>, <i> Geom2d</i> and <i> Geom</i> packages describe elementary data structures of simple geometric
objects. The constructors of these objects are elementary: the construction arguments
are fields by which the objects are represented in their data structure.


On the other hand, the <i> gce</i>, <i> GCE2d</i> and <i> GC</i> packages provided 
by the Direct Construction component construct the same types of objects 
as <i> gp</i>, <i> Geom2d </i>and <i> Geom</i> respectively.
However, the former implement geometric construction algorithms that translate the
constructor's arguments into the data structure specific to each object.


Algorithms implemented by these packages are simple: there is no creation of objects
defined by advanced positional constraints (for more information on this subject,
see <i> Geom2dGcc</i> and <i> GccAna</i> which describe geometry by constraints).


<i> gce</i>, <i> GCE2d</i> and <i> GC </i>each offer a series of classes of construction algorithms.


For example, the class <i>gce_MakeCirc</i> provides a framework 
for defining eight problems encountered in the geometric construction of circles, 
and implementing the eight related construction algorithms.

The object created (or implemented) is an algorithm which can be consulted to find out, in particular:

  * its result, which is a <i>gp_Circ</i>, and
  * its status. Here, the status indicates whether or not the construction was successful.

If it was unsuccessful, the status gives the reason for the failure.

~~~~
    gp_Pnt P1 (0.,0.,0.);
    gp_Pnt P2 (0.,10.,0.);
    gp_Pnt P3 (10.,0.,0.);
    gce_MakeCirc MC (P1,P2,P3);
    if (MC.IsDone()) {
		const gp_Circ& C = MC.Value();
    }
~~~~

In addition, <i> gce</i>, <i> GCE2d</i> and <i> GC</i> each have a <i>Root</i> class. This class is the root of
all the classes in the package which return a status. The returned status (successful
construction or construction error) is described by the enumeration <i> gce_ErrorType</i>.

Note: classes which construct geometric transformations do not return a status, and
therefore do not inherit from Root.

### Approximations

Approximation of Curves and Surfaces groups together a variety of functions used in 2D and 3D geometry for:

  * the interpolation of a set of 2D points using a 2D BSpline or Bezier curve;
  * the approximation of a set of 2D points using a 2D BSpline or Bezier curve;
  * the interpolation of a set of 3D points using a 3D BSpline or Bezier curve, or a BSpline surface;
  * the approximation of a set of 3D points using a 3D BSpline or Bezier curve, or a BSpline surface.

You can program approximations in two ways:

  * Using high-level functions, designed to provide a simple method for obtaining approximations with minimal programming,
  * Using low-level functions, designed for users requiring more control over the approximations.

The low-level functions provide a second API with functions to:

  * Define compulsory tangents for an approximation. These tangents have origins and extremities.
  * Approximate a set of curves in parallel to respect identical parameterization.
  * Smooth approximations. This is to produce a faired curve.

The classes <i> AppDef_MultiPointConstraints</i> and <i> AppDef_MultiLines </i> allow organizing the data.
The classes <i> AppDef_Compute</i>, <i> AppDef_BSplineCompute</i> and <i> AppDef_TheVariational </i> 
classes perform the approximation itself using Bezier curves, BSpline curves, and smooth BSpline curves, respectively.

You can also find functions to compute:

  * The minimal box which includes a set of points
  * The mean plane, line or point of a set of coplanar, collinear or coincident points.

### Conversion to and from BSplines
 
The Conversion to and from BSplines component has the following two distinct purposes:
  * Firstly, it provides a homogenous formulation which can be used to describe any curve or surface. 
  This is useful for writing algorithms for a single data structure model. 
  The BSpline formulation can be used to represent most basic geometric objects provided 
  by the components which describe geometric data structures ("Fundamental Geometry Types", "2D Geometry Types" and "3D Geometry Types" components).
  * Secondly, it can be used to divide a BSpline curve or surface into a series of curves or surfaces, 
  thereby providing a higher degree of continuity. This is useful for writing algorithms 
  which require a specific degree of continuity in the objects to which they are applied. 
  Discontinuities are situated on the boundaries of objects only.

The "Conversion to and from BSplines" component is composed of three packages.

The <i> Convert </i> package provides algorithms to convert the following into a BSpline curve or surface:

  * a bounded curve based on an elementary 2D curve (line, circle or conic) from the <i> gp </i> package,
  * a bounded surface based on an elementary surface (cylinder, cone, sphere or torus) from the <i> gp</i> package,
  * a series of adjacent 2D or 3D Bezier curves defined by their poles.

These algorithms compute the data needed to define the resulting BSpline curve or surface. 
This elementary data (degrees, periodic characteristics, poles and weights, knots and multiplicities) 
may then be used directly in an algorithm, or can be used to construct the curve or the surface 
by calling the appropriate constructor provided by the classes <i>Geom2d_BSplineCurve, Geom_BSplineCurve </i> or <i>Geom_BSplineSurface</i>.

The <i>Geom2dConvert</i> package provides the following:

  * a global function which is used to construct a BSpline curve from a bounded curve based on a 2D curve from the Geom2d package,
  * a splitting algorithm which computes the points at which a 2D BSpline curve should be cut in order to obtain arcs with the same degree of continuity,
  * global functions used to construct the BSpline curves created by this splitting algorithm, or by other types of segmentation of the BSpline curve,
  * an algorithm which converts a 2D BSpline curve into a series of adjacent Bezier curves.

The <i> GeomConvert</i> package also provides the following:

  * a global function used to construct a BSpline curve from a bounded curve based on a curve from the Geom package,
  * a splitting algorithm, which computes the points at which a BSpline curve should be cut in order to obtain arcs with the same degree of continuity,
  * global functions to construct BSpline curves created by this splitting algorithm, or by other types of BSpline curve segmentation,
  * an algorithm, which converts a BSpline curve into a series of adjacent Bezier curves,
  * a global function to construct a BSpline surface from a bounded surface based on a surface from the Geom package,
  * a splitting algorithm, which determines the curves along which a BSpline surface should be cut in order to obtain patches with the same degree of continuity,
  * global functions to construct BSpline surfaces created by this splitting algorithm, or by other types of BSpline surface segmentation,
  * an algorithm, which converts a BSpline surface into a series of adjacent Bezier surfaces,
  * an algorithm, which converts a grid of adjacent Bezier surfaces into a BSpline surface.

### Points on Curves

The Making Points on Curves component comprises high level functions providing an Application Programming Interface for complex algorithms that compute points on a 2D or 3D curve. The functions use various methods.

The algorithms result in the following:

  * a point on a curve, situated at a given curvilinear distance from another point on the curve
  * a distribution of points situated at constant curvilinear intervals along a curve
  * a distribution of points at a constant rise (i.e. respecting a criterion of maximum rise between the curve and the polygon that results from the computed points) along a curve
  * the length of a curve.

@subsection OCCT_TOVW_SECTION_3_4 Topology

Topological library allows you to build pure topological data structures..

Topology defines relationships between simple geometric entities. In this way, 
you can model complex shapes as assemblies of simpler entities. 
Due to a built-in non-manifold (or mixed-dimensional) feature, you can build models mixing:

  * 0D entities such as points; 
  * 1D entities such as curves; 
  * 2D entities such as surfaces; 
  * 3D entities such as volumes. 

You can, for example, represent a single object made of several distinct bodies 
containing embedded curves and surfaces connected or non-connected to an outer boundary.

Abstract topological data structure describes a basic entity - a shape, 
which can be divided into the following component topologies:

  * Vertex - a zero-dimensional shape corresponding to a point in geometry; 
  * Edge - a shape corresponding to a curve, and bound by a vertex at each extremity;
  * Wire - a sequence of edges connected by their vertices; 
  * Face - part of a plane (in 2D geometry) or a surface (in 3D geometry) bounded by a closed wire;
  * Shell - a collection of faces connected by some edges of their wire boundaries; 
  * Solid - a part of 3D space bound by a shell; 
  * Compound solid - a collection of solids. 

The wire and the solid can be either infinite or closed.

A face with 3D underlying geometry may also refer to a collection of connected triangles 
that approximate the underlying surface. The surfaces can be undefined 
leaving the faces represented by triangles only. If so, the model is purely polyhedral.

Topology defines the relationship between simple geometric entities, 
which can thus be linked together to represent complex shapes.

Abstract Topology is provided by six packages. 
The first three packages describe the topological data structure used in Open CASCADE Technology:

  * <i> TopAbs</i> package provides general resources for topology-driven applications. It contains enumerations that are used to describe basic topological notions: topological shape, orientation and state. It also provides methods to manage these enumerations.
  * <i> TopLoc </i>package provides resources to handle 3D local coordinate systems: <i> Datum3D</i>and <i> Location</i>. <i> Datum3D</i> describes an elementary coordinate system, while <i> Location</i> comprises a series of elementary coordinate systems.
  * <i> TopoDS</i> package describes classes to model and build data structures that are purely topological.

Three additional packages provide tools to access and manipulate this abstract topology:

  * <i> TopTools</i> package provides basic tools to use on topological data structures.
  * <i> TopExp</i> package provides classes to explore and manipulate the topological data structures described in the TopoDS package.
  * <i> BRepTools </i> package provides classes to explore, manipulate, read and write BRep data structures. These more complex data structures combine topological descriptions with additional geometric information, and include rules for evaluating equivalence of different possible representations of the same object, for example, a point.

@subsection OCCT_TOVW_SECTION_3_5 Properties of Shapes

### Local Properties of Shapes

<i>BRepLProp</i> package provides the Local Properties of Shapes component, 
which contains algorithms computing various local properties on edges and faces in a BRep model.

The local properties which may be queried are:

  * for a point of parameter u on a curve which supports an edge :
    * the point,
    * the derivative vectors, up to the third degree,
    * the tangent vector,
    * the normal,
    * the curvature, and the center of curvature;
  * for a point of parameter (u, v) on a surface which supports a face :
    * the point,
    * the derivative vectors, up to the second degree,
    * the tangent vectors to the u and v isoparametric curves,
    * the normal vector,
    * the minimum or maximum curvature, and the corresponding directions of curvature;
  * the degree of continuity of a curve which supports an edge, built by the concatenation of two other edges, at their junction point.

Analyzed edges and faces are described as <i> BRepAdaptor</i> curves and surfaces, 
which provide shapes with an interface for the description of their geometric support. 
The base point for local properties is defined by its u parameter value on a curve, or its (u, v) parameter values on a surface.

### Local Properties of Curves and Surfaces


The "Local Properties of Curves and Surfaces" component provides algorithms for computing various local 
properties on a Geom curve (in 2D or 3D space) or a surface. It is composed of:

  * <i> Geom2dLProp</i> package, which provides local properties on 2D curves
  * <i> GeomLProp </i> package, which provides local properties on 3D curves and surfaces
  * <i> LProp </i> package, which provides an enumeration used to characterize a particular point on a 2D curve.

Curves are either <i> Geom_Curve </i> curves (in 3D space) or <i> Geom2d_Curve </i> curves (in the plane). 
Surfaces are <i> Geom_Surface </i> surfaces. The point on which local properties are calculated 
is defined by its u parameter value on a curve, and its (u,v) parameter values on a surface.

It is possible to query the same local properties for points as mentioned above, and additionally for 2D curves:

  * the points corresponding to a minimum or a maximum of curvature;
  * the inflection points.

### Global Properties of Shapes

The Global Properties of Shapes component provides algorithms for computing the global 
properties of a composite geometric system in 3D space, and frameworks to query the computed results.

The global properties computed for a system are :

  * mass,
  * mass center,
  * matrix of inertia,
  * moment about an axis,
  * radius of gyration about an axis,
  * principal properties of inertia such as principal axis, principal moments, and principal radius of gyration.

Geometric systems are generally defined as shapes. Depending on the way they are analyzed, these shapes will give properties of:

  * lines induced from the edges of the shape,
  * surfaces induced from the faces of the shape, or
  * volumes induced from the solid bounded by the shape.

The global properties of several systems may be brought together to give the global properties of the system composed of the sum of all individual systems.

The Global Properties of Shapes component is composed of:
* seven functions for computing global properties of a shape: one function for lines, two functions for surfaces and four functions for volumes. The choice of functions depends on input parameters and algorithms used for computation (<i>BRepGProp</i> global functions),
* a framework for computing global properties for a set of points (<i>GProp_PGProps</i>),
* and a general framework to bring together the global properties retained by several more elementary frameworks, and provide a general programming interface to consult computed global properties.
    
@subsection OCCT_TOVW_SECTION_3_6 Examples

### How to compute the curve length

To compute curve length, use the method <i>AbscissaPoint::Length</i> from <i> GCPnts</i>.

This function is used only for initializing a framework to compute the length of a curve (or a series of curves).

The adapted curves are:

  * Adaptor_Curve2d for 2D curves
  * Adaptor_Curve for 3D curves.

The adapted curve is created in the following way:

In 2D:

~~~~~~~~~~~
    Handle(Geom2d_Curve) mycurve = ... ;
    Geom2dAdaptor_Curve C (mycurve) ;
~~~~~~~~~~~

In 3D:

~~~~~~~~~
    Handle(Geom_Curve) mycurve = ... ;
    GeomAdaptor_Curve C (mycurve) ;
~~~~~~~~~


The length of the curve is then computed using this curve object:

~~~~~~~~
    GCPnts_AbscissaPoint myAlgo () ;
    Standard_Real L = myAlgo.Length( C ) ;
~~~~~~~~

### How to check the surface concavity

To check the concavity of a surface, proceed as follows:

  1. Sample the surface and compute at each point the Gaussian curvature.
  2. If the value of the curvature changes of sign, the surface is concave or convex depending on the point of view.
  3. To compute a Gaussian curvature, use the class <i> SLprops</i> from <i> GeomLProp</i>, which instantiates the generic class <i> SLProps </i>from <i> LProp</i> and use the method <i> GaussianCurvature</i>.

### How to approximate a curve with respect to tangencies


To approximate a curve with respect to tangencies, follow these steps:

  1. Create an object of type <i> AppDef_MultiPointConstraints</i> from the set of points to approximate and use the method <i> SetTang </i>to set the tangency vectors.
  2. Create an object of type <i> AppDef_MultiLine </i>from the <i> AppDef_MultiPointConstraint</i>.
  3. Use <i> AppDef_BSplineCompute</i>, which instantiates <i>Approx_BSplineComputeLine</i> to perform the approximation.

### How to extract the underlying geometry from shapes


To extract the underlying geometry from a Shape, use the following methods:

  * <i> BRep_Tool::Surface</i> to get the geometric surface from a face.
  * <i> BRep_Tool::Curve</i> to get the 3d geometric curve from an edge.
  * <i> BRep_Tool::CurveOnSurface</i> to get the 2d geometric curve of an edge, defined in the parametric space of a face.
  * <i> BRep_Tool::Pnt </i> to get the 3D point from a vertex.

Some of these methods have a location as argument.

For example, when you use <i> S1 = BRep_Tool::Surface(F,L), </i> you then get the surface stored in <i> TShape</i>.

To use this surface in the same position as the face, you have to apply 
a transformation to it corresponding to the location of the face as follows:

~~~~~~~~~~~~
    gp_Trsf T(L) ;
    S2 = S1->Moved(T) ;
~~~~~~~~~~~~

The same method used without location as argument is <i>S3 = BRep_Tool : : Surface(F)</i> 
returns a Surface in position, according to the location. S3 and S2 are geometrically equivalent surfaces.

Warning: with the first method, you get a pointer on the surface stored in the shape. 
Do not modify the surface because you will modify the shape and may produce an inconsistent model.
With the second method, you get a copy of the surface on which the location has been applied.
Note: you can use also a topological object directly just as if it 
were a geometric one by using the services of <i> BRepAdaptor </i>classes.

### How to get the coordinates of a vertex


To recover the UV coordinates of vertices, 
use <i> BRep_Tool::Parameters const TopoDS_Vertex& V,const TopoDS_Face& F), </i> 
which returns the U and V parameters of the vertex V on the face F as a <i> gp_Pnt2d</i>. 

### How to explore a Wire


To explore the edges of a wire in a contiguous order, use <i> BrepTools_WireExplorer</i> class.

~~~~
    TopoDS_Wire myWire =&ldots;.
    BRepTools_WireExplorer Ex;
    for (Ex.Init(myWire); Ex.More(); Ex.Next()) {
    TopoDS_Edge currentedge = Ex.Current();
    // Process current edge
    }
~~~~

### How to merge bspline curves


To merge joined bspline curves use the following methods:

~~~~
    void GeomConvert::ConcatG1
    TColGeom_Array1OfBSplineCurve& ArrayOfCurves,
      const TColStd_Array1OfReal& ArrayOfToler,
    Handle(TColGeom_HArray1OfBSplineCurve) & ArrayOfConcatenated,
    const Standard_Boolean ClosedG1Flag,
      const Standard_Real ClosedTolerance)
~~~~

This method concatenates according to G1 (tangency continuity along the curve) the 
<i>ArrayOfCurves</i> as far as possible. The following arguments are used:

  * <i> ArrayOfCurves</i> must have dimension bounds [0, N-1], N   * number of curves to be merged.
  * <i> ArrayOfToler</i> contains the biggest tolerance of the two points shared by two consecutive curves. Its dimension is: [0, N-2].
  * <i> ArrayOfConcatenated</i> contains results of operation: one or more, when impossible to merge all curves from <i> ArrayOfCurves </i> into one, new bspline curves are created.
  * <i> ClosedG1Flag </i> indicates if the <i> ArrayOfCurves </i> is closed or not.
  * If <i> ClosedG1Flag = Standard_True, ClosedTolerance </i> contains the biggest tolerance of the two points which are at the closure, otherwise its value is 0.0.

~~~~
    void GeomConvert::ConcatC1
    TColGeom_Array1OfBSplineCurve& ArrayOfCurves,
      const TColStd_Array1OfReal& ArrayOfToler,
    Handle(TColStd_HArray1OfInteger)& ArrayOfIndices,
    Handle(TColGeom_HArray1OfBSplineCurve)& ArrayOfConcatenated,
    const Standard_Boolean ClosedG1Flag,
      const Standard_Real ClosedTolerance,
      const Standard_Real AngularTolerance)
~~~~

This method concatenates according to C1 (first derivative continuity along the curve) the <i> 
ArrayOfCurves</i> as far possible. The following arguments are used (additionally to the mentioned above):

* <i> ArrayOfIndices</i> contains indices that define curves from <i> ArrayOfCurves</i> which are beginning curves for each group of curves merged into a new curve.
* <i> AngularTolerance</i> is used to check the continuity of tangencies.

~~~~
    void GeomConvert::ConcatC1
    TColGeom_Array1OfBSplineCurve& ArrayOfCurves,
      const TColStd_Array1OfReal& ArrayOfToler,
    Handle(TColStd_HArray1OfInteger)& ArrayOfIndices,
    Handle(TColGeom_HArray1OfBSplineCurve)& ArrayOfConcatenated,
      const Standard_Boolean ClosedG1Flag,
      const Standard_Real ClosedTolerance)
~~~~
This method is the same as the previous one, except for that <i> AngularTolerance = Precision::Angular() </i>

It is also possible to use class <i> GeomConvert_CompCurveToBSplineCurve</i>. 
This class provides methods to concatenate several restricted curves to a bspline curve. 
Non-bspline curves are converted to bspline before concatenation.

Constructor:
~~~~~~~~~~
    GeomConvert_CompCurveToBSplineCurve::
    GeomConvert_CompCurveToBSplineCurve(const Handle(Geom_BoundedCurve)& BasisCurve, const Convert_ParameterisationType Parameterization)
    BasisCurve   * beginning curve;
~~~~~~~~~~

Parameterization defines the ways of conversion in bspline (by default <i> Convert_TgtThetaOver2</i>).

The method to add a new curve is:
~~~~
    Standard_Boolean GeomConvert_CompCurveToBSplineCurve::
    Add(const Handle(Geom_BoundedCurve)& NewCurve,
    const Standard_Real Tolerance,
    const Standard_Boolean After,
    const Standard_Boolean WithRatio,
    const Standard_Integer MinM)
~~~~

It returns False if the curve is not C0 with the <i> BSplineCurve</i>.

Tolerance is used to check the continuity and decrease the Multiplicity 
at the common Knot until <i> MinM </i>. If <i> MinM = 0 </i>, the common Knot can be removed.

The parameter after defines the place for a new curve when it is possible to put 
the new curve before or after the <i> BasisCurve</i> (in fact, it is case when concatenated curve can be closed). 
It does not change the shape of the curve, but defines its first and last points.

If <i> WithRatio = Standard_True </i> the algorithm tries to reach C1 continuity.

The method to get a result is <i> Handle(Geom_BSplineCurve) GeomConvert_CompCurveToBSplineCurve::BSplineCurve() const </i>

@section OCCT_TOVW_SECTION_4 Modeling Algorithms

Modeling Algorithms module groups a wide range of 
topological algorithms used in modeling and geometric algorithms, called by them.

These services are organized into the following libraries:

  * Geometric Tools 
  * Topological Tools 
  * Construction of Primitives 
  * Boolean Operations 
  * Fillets and Chamfers 
  * Offsets and Drafts 
  * Features 
  * Hidden Line Removal
  * Shape Healing 

@image html /technical_overview/images/technical_overview_ma.png
@image latex /technical_overview/images/technical_overview_ma.png

The technical overview provides only a basic description of the libraries. Please, refer for more details to  <a href="user_guides__modeling_algos.html">Modeling Algorithms User's guide</a>

See also: our web site at <a href="http://www.opencascade.org/support/training/">E-learning and Training</a>.

@subsection OCCT_TOVW_SECTION_4_1 Geometric Tools

This library provides algorithms to:

  * Calculate the intersection of two 2D curves, surfaces, or a 3D curve and a surface;
  * Project points onto 2D and 3D curves, points onto surfaces, and 3D curves onto surfaces;
  * Construct lines and circles from constraints; 
  * Construct curves and surfaces from constraints; 
  * Construct curves and surfaces by interpolation 

OPEN CASCADE company also provides a product known as Surfaces from Scattered Points
(SSP), which allows constructing surfaces from scattered points. This algorithm accepts
or constructs an initial B-Spline surface and looks for its deformation (finite elements
method) which would satisfy the constraints. Using optimized computation methods,
this algorithm is able to construct a surface from more than 500 000 points.

SSP product is not supplied with Open CASCADE Technology, but can be purchased separately.

@subsubsection OCCT_TOVW_SECTION_4_1_1 Intersections

The Intersections component is used to compute intersections between 2D or 3D geometrical
objects: 

  * the intersections between two 2D curves
  * the self-intersections of a 2D curve
  * the intersection between a 3D curve and a surface
  * the intersection between two surfaces.

@subsubsection OCCT_TOVW_SECTION_4_1_2 Projections

The Projections component provides functionality for 2D and 3D geometry objects for computing the following:

  * the projections of a 2D point onto a 2D curve
  * the projections of a 3D point onto a 3D curve or surface
  * the projection of a 3D curve onto a surface.
  * the planar curve transposition from the 3D to the 2D parametric space of an underlying plane and v. s.
  * the positioning of a 2D gp object in the 3D geometric space.

@subsubsection OCCT_TOVW_SECTION_4_1_3 Lines and Circles from Constraints

The Lines and Circles from Constraints component provides numerous 
construction algorithms for 2D circles or lines described with numeric or 
geometric constraints in relation to other curves. These constraints enable the following to be imposed:

  * the radius of a circle,
  * the angle that a straight line makes with another straight line,
  * the tangency of a straight line or circle in relation to a curve,
  * the passage of a straight line or circle through a point,
  * the circle with center in a point or curve.

For example, these algorithms enable to easily construct a circle of a given radius,
centered on a straight line and tangential to another circle.

The implemented algorithms are more complex than those provided 
by the Direct Constructions component for building 2D circles or lines.

The expression of a tangency problem generally leads to several results, according
to the relative positions of the solution and the circles or straight lines in relation
to which the tangency constraints are expressed. For example, consider the following
case of a circle of a given radius (a small one) which is tangential to two secant
circles C1 and C2:

@image html /technical_overview/images/technical_overview_occ_0005.png "Example of a Tangency Constraint"
@image latex /technical_overview/images/technical_overview_occ_0005.png "Example of a Tangency Constraint"

This diagram clearly shows that there are 8 possible solutions.

In order to limit the number of solutions, we can try to express the relative position
of the required solution in relation to the circles to which it is tangential. For
example, if we specify that the solution is inside the circle C1 and outside the
circle C2, only two solutions referenced 3 and 4 on the diagram respond to the problem
posed.

This technique of qualification of a solution, in relation to the curves to which
it is tangential, can be used in all algorithms for constructing a circle or a straight
line by geometric constraints. Four qualifiers are used, which specify the following:

  * the solution(s) must enclose the argument, or
  * the solution(s) must be enclosed by the argument, or
  * the solution(s) and the argument must be external to one another, or
  * the relative position is not qualified, i.e. all solutions apply.

These definitions are very easy to interpret on a circle, where it is easy to identify
the interior and exterior sides. In fact, for any kind of curve the interior is defined
as the left-hand side of the curve in relation to its orientation.

OCCT implements several categories of algorithms:

  * analytic algorithms, where solutions are obtained by the resolution of an equation, such algorithms are used when the geometries which are worked on (tangency arguments,   position of the center, etc.) are points, lines or circles;
  * geometric algorithms, where the solution is generally obtained by calculating the intersection of parallel or bisecting curves built from geometric arguments;
  * iterative algorithms, where the solution is obtained by a process of iteration.

For each kind of geometric construction of a constrained line or circle, OCCT provides
two types of access to the user:

  * algorithms from the package <i> Geom2dGcc </i> automatically select the algorithm best suited to the problem to be treated, both in the general case and in all types of specific cases; the arguments used are Geom2d objects; the solutions computed are <i> gp </i> objects;
  * algorithms from the package <i> GccAna</i> resolve the problem analytically, and can only be used when the geometries to be worked on are lines or circles; the arguments used and solutions computed are <i> gp </i> objects.

The provided algorithms compute all solutions, which correspond to the stated geometric
problem, unless the solution is found by an iterative algorithm.

Iterative algorithms compute only one solution, closest to an initial
position. They can be used in the following cases:

  * to build a circle, when an argument is more complex than a line or a circle, and where 
  the radius is not known or difficult to determine: this is the case for a circle tangential 
  to three geometric elements, or tangential to two geometric elements and centered on a curve;
  * to build a line, when a tangency argument is more complex than a line or a circle.

Qualified curves (for tangency arguments) are provided either by:

  * the <i> GccEnt</i> package, for direct use by <i> GccAna</i> algorithms, or
  * the <i> Geom2dGcc </i> package, for general use by <i> Geom2dGcc </i> algorithms.

The <i> GccEnt</i> and <i> Geom2dGcc</i> packages also provide simple functions for building qualified curves in a very efficient way.

The <i> GccAna </i>package also provides algorithms for constructing bisecting loci between
circles, lines or points. Bisecting loci between two geometric objects are such that
each of their points is at the same distance from the two geometric objects. They
are typically curves, such as circles, lines or conics for <i> GccAna</i> algorithms. 
Each elementary solution is given as an elementary bisecting locus object (line, circle,
ellipse, hyperbola, parabola), described by the <i>GccInt</i> package.

Note: Curves used by <i> GccAna </i> algorithms to define the geometric problem to be solved,
are 2D lines or circles from the <i> gp</i> package: they are not explicitly parameterized.
However, these lines or circles retain an implicit parameterization, corresponding
to that which they induce on equivalent Geom2d objects. This induced parameterization
is the one used when returning parameter values on such curves, for instance with
the functions <i> Tangency1, Tangency2, Tangency3, Intersection2</i> and <i> CenterOn3</i> provided
by construction algorithms from the <i> GccAna </i> or <i> Geom2dGcc</i> packages.

@subsubsection OCCT_TOVW_SECTION_4_1_4 Curves and Surfaces from Constraints

The Curves and Surfaces from Constraints component groups together high level functions
used in 2D and 3D geometry for:

  * creation of faired and minimal variation 2D curves
  * construction of ruled surfaces
  * construction of pipe surfaces
  * filling of surfaces
  * construction of plate surfaces
  * extension of a 3D curve or surface beyond its original bounds.

#### 2D Curves from constraints


Elastic beam curves have their origin in traditional methods of modeling applied 
in boat-building, where a long thin piece of wood, a lathe, was forced to pass
between two sets of nails and in this way, take the form of a curve based on the
two points, the directions of the forces applied at those points, and the properties
of the wooden lathe itself.

Maintaining these constraints requires both longitudinal and transversal forces to
be applied to the beam in order to compensate for its internal elasticity. The longitudinal
forces can be a push or a pull and the beam may or may not be allowed to slide over
these fixed points.

The class <i> Batten</i> produces curves defined on the basis of one or more constraints
on each of the two reference points. These include point and angle of tangency settings.
The class <i> MinimalVariation</i> produces curves with minimal variation in curvature. 
The exact degree of variation is provided by curvature settings.

#### Ruled Surfaces

A ruled surface is built by ruling a line along the length of two curves.

#### Pipe Surfaces


A pipe is built by sweeping a curve (the section) along another curve (the path).

The following types of construction are available:
  * pipes with a circular section of constant radius,
  * pipes with a constant section,
  * pipes with a section evolving between two given curves.


#### Surface filling

It is often convenient to create a surface from two or more curves which will form
the boundaries that define the new surface.

A case in point is the intersection of two fillets at a corner. If the radius of
the fillet on one edge is different from that of the fillet on another, it becomes
impossible to sew together all the edges of the resulting surfaces. This leaves a
gap in the overall surface of the object which you are constructing.

@image html /technical_overview/images/technical_overview_occ_0006.png "Intersecting filleted edges with differing radiuses"
@image latex /technical_overview/images/technical_overview_occ_0006.png "Intersecting filleted edges with differing radiuses"

These algorithms allow you to fill this gap from two, three or four curves. This
can be done with or without constraints, and the resulting surface will be either
a Bezier or a BSpline surface in one of a range of filling styles.


This package was designed with a view to handling gaps produced during fillet construction.
Satisfactory results cannot be guaranteed for other uses.

#### Plate surfaces


In CAD, it is often necessary to generate a surface which has no exact mathematical
definition, but which is defined by respective constraints. These can be of a mathematical,
a technical or an aesthetic order.


Essentially, a plate surface is constructed by deforming a surface so that it conforms
to a given number of curve or point constraints. In the figure below, you can see
four segments of the outline of the plane, and a point which have been used as the
curve constraints and the point constraint respectively. The resulting surface can
be converted into a BSpline surface by using the function <i> MakeApprox </i>.


The surface is built using a variational spline algorithm. It uses the principle
of deformation of a thin plate by localised mechanical forces. If not already given
in the input, an initial surface is calculated. This corresponds to the plate prior
to deformation. Then, the algorithm is called to calculate the final surface. It
looks for a solution satisfying constraints and minimizing energy input.

@image html /technical_overview/images/technical_overview_occ_0007.png "Surface generated from four curves and a point"
@image latex /technical_overview/images/technical_overview_occ_0007.png  "Surface generated from four curves and a point" 

@image html /technical_overview/images/technical_overview_occ_0008.png "Surface generated from two curves and a point"
@image latex /technical_overview/images/technical_overview_occ_0008.png "Surface generated from two curves and a point" 

#### Extension of a 3D curve or surface beyond its original bounds

The extension is performed according to a geometric requirement and a continuity
constraint. It should be a small extension with respect to the size of the original
curve or surface.

@subsubsection OCCT_TOVW_SECTION_4_1_5 Interpolation

The Interpolation Laws component provides definitions of functions: <i> y=f(x) </i>.

In particular, it provides definitions of:

  * a linear function,
  * an <i> S </i> function, and
  * an interpolation function for a range of values.

Such functions can be used to define, for example, the evolution law of a fillet along the edge of a shape.

The validity of the function built is never checked: the Law package does not know for what 
application or to what end the function will be used. In particular, if the function is used 
as the evolution law of a fillet, it is important that the function is always positive. The user must check this.

@subsection OCCT_TOVW_SECTION_4_2 Topological Tools

This library provides algorithms to:

  * Tessellate shapes 
  * Validate shapes 
  * Determine the local properties of shapes 
  * Determine the global properties of shapes 
  * Perform geometric transformations
  * Find planes in which edges are located
  * Convert shapes to NURBS geometry.

It also furnishes a complete brep data structure for topological data structures defined 
in the Topology library of the Modeling Data module. 
This linAllows you to create standard topological objects such as:

  * Vertices
  * Edges
  * Faces
  * Wires
  * Polygonal wires
  * Shells
  * Solids.

The API provides classes to build objects:

  * The constructors of classes provide different construction methods;
  * The class retains different tools used to build objects as fields;
  * The class provides a casting method to obtain the result automatically with a function-like call. 

For example, to build a vertex V on a point P, you can use:

~~~
    V = BRepBuilderAPI_MakeVertex(P);
~~~
or

~~~
    BRepBuilderAPI_MakeVertex MV(P);
    V = MV.Vertex();
~~~ 

For error handling, the <i> BRepBuilderAPI</i> commands raise only the 
<i> 0StdFail_NotDone</i> exception. When <i> IsDone</i> is false for a command, 
the error description can be requested from the command.

@subsection OCCT_TOVW_SECTION_4_3 Construction of Primitives

This library contained in <i> BRepPrimAPI</i> package provides an API (Application Programming Interface) for:

  * Construction of primitives such as:
    * Boxes;
    * Cones;
    * Cylinders;
    * Prisms.
  * Construction by sweeping along a profile:
    * Linear;
    * Rotational (through an angle of rotation).

Sweeps are objects obtained by sweeping a profile along a path. 
The profile can be any topology and the path is usually a curve or a wire. 
The profile generates objects according to the following rules:

  * Vertices generate Edges
  * Edges generate Faces.
  * Wires generate Shells.
  * Faces generate Solids.
  * Shells generate Composite Solids.

It is not allowed to sweep Solids and Composite Solids.

Swept constructions along complex profiles such as 
BSpline curves are also available in the <i> BRepOffsetAPI </i> package.

This API provides simple, high level calls for the most common operations.

@subsection OCCT_TOVW_SECTION_4_4 Boolean Operations 

Boolean operations to create new shapes from old ones by using:

  * Common 
  * Cut 
  * Fuse 
  * Section 

There are two libraries for Boolean Operations: 

  * Old Boolean Operations (BOA) provided by <i>BRepAlgo</i> package designed and developed in Open CASCADE 6x in 2000; its architecture and content are out of date.
  * New Boolean Operations (NBOA) provided by <i>BRepAlgoAPI</i> package designed and developed in 2009 and completely revised in 2013.

New Boolean Operations provide the following major benefits:

  * The NBOA have an expandable architecture of inner sub-algorithms, which  allows to create specific algorithms for the Customers using existing inner sub-algorithms as root algorithms and to reduce the time for the development. 
  * The architecture of inner sub-algorithms of NBOA provides their reusability with maximal independence from the environment of NBOA. The fact allows to create specific algorithms for the Customers using these sub-algorithms as they are or as root classes and thus to reduce the time for the development. 
  * The architecture of NBOA is history-based. The implementation of NBOA internally sets up a correspondence between any sub-shape of the argument and its image in the result. The history is not imposed and thus it is not error-prone as it was in BOA. The fact allows direct and safely usage of the algorithm in parametric modeling.   
  * NBOA provide a general algorithm. It correctly processes without using the workarounds even the cases that cannot be properly processed by BOA.
  * The implementation of NBOA is based on NCollection classes. The usage of opportunities given by local memory allocators ( <i> NCollection_IncAllocator</i>) allows improving memory management and saving memory resources. 
  * NBOA use modern algorithms of OCC as auxiliary tools. For e.g. the algorithm of unbalanced binary tree of overlapped bounding boxes <i> NCollection_UBTree</i>. The usage of the algorithm allows to improve the performance of NBOA if there is a big number of sub-shapes in the arguments.

Boolean Operations have the following types of the arguments and produce the following results:
* For arguments having the same shape type (e.g. SOLID / SOLID) the type of the resulting shape will be a COMPOUND, containing shapes of this type;
* For arguments having different shape types (e.g. SHELL / SOLID) the type of the resulting shape will be a COMPOUND, containing shapes of the type that is the same as that of the low type of the argument. Example: For SHELL/SOLID the result is a COMPOUND of SHELLs. 
* For arguments with different shape types some of Boolean Operations can not be done using the default implementation, because of a non-manifold type of the result. Example: the FUSE operation for SHELL and SOLID can not be done, but the CUT operation can be done, where SHELL is the object and SOLID is the tool.
* It is possible to perform Boolean Operations on arguments of the COMPOUND shape type. In this case each compound must not be heterogeneous, i.e. it must contain equidimensional shapes (EDGEs or/and WIREs, FACEs or/and SHELLs, SOLIDs). SOLIDs inside the COMPOUND must not contact (intersect or touch) each other. The same condition should be respected for SHELLs or FACEs, WIREs or EDGEs.
* Boolean Operations for COMPSOLID type of shape are not supported.

@subsection OCCT_TOVW_SECTION_4_5 Features

This library contained in BRepFeat package is necessary for the creation 
and manipulation of both form and mechanical features in a Boundary Representation framework.

The form features are depressions or protrusions including the following types:

  * Cylinder;
  * Draft Prism;
  * Prism;
  * Revolved feature;
  * Pipe.

Depending on whether you wish to make a depression or a protrusion, 
you can choose either to remove matter (Boolean cut: Fuse equal to 0) or to add it (Boolean fusion: Fuse equal to 1).

The semantics of form feature creation is based on the construction of shapes:

  * for a certain length in a certain direction;
  * up to the limiting face;
  * from the limiting face at a height;
  * above and/or below a plane.

The shape defining the construction of a feature can be 
either a supporting edge or a concerned area of a face.

In case of supporting edge, this contour can be attached to a face 
of the basis shape by binding. When the contour is bound to this face, 
the information that the contour will slide on the face becomes available 
to the relevant class methods. In case of the concerned area of a face, you could, 
for example, cut it out and move it at a different height, 
which will define the limiting face of a protrusion or depression.

Topological definition with local operations of this sort makes calculations simpler 
and faster than a global operation. The latter would entail a second phase 
of removing unwanted matter to get the same result.

Mechanical features include ribs, protrusions and grooves (or slots), 
depressions along planar (linear) surfaces or revolution surfaces.

The semantics of mechanical features is based on giving thickness to a contour. 
This thickness can either be

  * unilateral 
  * on one side of the contour 
  * or bilateral 
  * on both sides. 

As in the semantics of form features, the thickness is defined 
by construction of shapes in specific contexts.

However, in case of mechanical features, development contexts differ. 
Here they include extrusion:

  * to a limiting face of the basis shape;
  * to or from a limiting plane;
  * to a height.

@subsection OCCT_TOVW_SECTION_4_6 Hidden Line Removal

This library provides two algorithms: <i> HLRBRep_Algo</i> and <i> HLRBRep_PolyAlgo</i>  to define the lines of a shape hidden in a given projection.  These lines can be shown or hidden to have the precision required in industrial design. To do this, the Hidden Line Removal component provides 

These algorithms remove or indicate lines hidden by surfaces. 
For a given projection, they calculate a set of lines characteristic of the object  being represented. They are also used in conjunction with extraction utilities,   which reconstruct a new, simplified shape from a selection of calculation results. 
This new shape is made up of edges, which represent the lines of the visualized shape in a plane.   This plane is the projection plane.

The algorithm <i> HLRBRep_Algo</i> allows working with the shape itself,  while <i> HLRBRep_PolyAlgo </i>works with its polyhedral simplification. When you use <i> HLRBRep_Algo</i>, you obtain an exact result, whereas, when you use <i> HLRBRep_PolyAlgo</i>, you reduce computation time but obtain polygonal segments.

@subsection OCCT_TOVW_SECTION_4_7 Shape Healing

Shape Healing library provides algorithms to modify the geometry and topology of OCCT shapes.
Shape Healing adapts shapes to make them maximally appropriate for use by OCCT, for example:

 * analyze shape characteristics and, in particular, identify the shapes that do not comply with OCCT validity rules; 
  * fix incorrect or problem shapes;
  * upgrade and change shape characteristics if needed, for example a C0 supporting surface can become C1 continuous.

It has several sub-domains, each with its own scope of functionality:

  * analysis        - exploring shape properties, computing shape features, detecting violation of OCCT requirements (the shape itself is not modified);
  * fixing          - fixing shape to meet the OCCT requirements (the shape may change its original form: modifying, removing, constructing sub-shapes, etc.);
  * upgrade         - shape improvement for better usability in OCCT or other algorithms (the shape is replaced with a new one, but geometrically they are the same);
  * customization   - modifying shape representation to fit specific needs (the shape is not modified, only the form of its representation is modified);
  * processing      - mechanism of shape modification via a user-editable resource file.

The technical overview provides only a basic description of the libraries. Please, refer for more details to  <a href="user_guides__shape_healing.html">Shape Healing User's guide</a>

See also: our web site at <a href="http://www.opencascade.org/support/training/">E-learning and Training</a>.


@subsection OCCT_TOVW_SECTION_4_8 Miscellaneous modelling algorithms. 

### Fillets and Chamfers

This library provides algorithms to make fillets and chamfers on shape edges.
The following cases are addressed:

  * Corners and apexes with different radii; 
  * Corners and apexes with different concavity. 

If there is a concavity, both surfaces that need to be extended and those, which do not, are processed.

### Offsets, Drafts, Sewing and Sweeps

These classes provide the following services:

  * Creation of offset shapes and their variants such as: 
    * Hollowing; 
    * Shelling; 
    * Lofting; 
  * Creation of tapered shapes using draft angles 
  * Sewing 
  * Creation of sweeps 


@subsection OCCT_TOVW_SECTION_4_9 Examples

### How to compute the state of a point on a face:


Use <i> BRepTools::Pnt </i> to get the point from your vertex.
Your shape must be of the <i> TopoDS_Shape </i>type.
If it is, you can use <i> BRepTopAdaptor_FClass2d </i>class. For example:

~~~~~
    BRepTopAdaptor_FClass2d::Load (to load the solid )
    BRepTopAdaptor_FClass2d::Perform (to compute the state of the point )
    BRepTopAdaptor_FClass2d::State (to get the TopAbs_State). 
~~~~~


### How to compute the state of a point in a solid:


Use <i>BRepTools::Pnt </i> to get the point from your vertex.
Your shape must be of the <i> TopoDS_Solid</i> type.

If it is, you can use the <i> BRepClass3d_SolidClassifier </i> class, for example:

~~~~~
    BRepClass3d_SolidClassifier::Load (to load the solid)
    BRepClass3d_SolidClassifier::Perform (to compute the state of the point)
    BRepClass3d_SolidClassifier::State (to get a TopAbs_State object)
    BRepClass3d_SolidClassifier inherits BRepclass3d_SClassifier
~~~~~

### How to connect a set of contiguous but independent faces


A unique topological object can be obtained in this way using the class 
<i> Sewing</i> from the <i> BRepOffsetAPI </i>package which produces a shell as a result.

~~~~~
    BRepOffsetAPI_Sewing Sew;
    Sew.Add(Face1); 
    Sew.Add(Face2); 
    ...
    Sew.Add(Facen); 
    Sew.Perform();
    TopoDS_Shape result= Sew.SewedShape();
~~~~~

@note The sewing algorithm uses a tolerance to assemble the faces by sewing them along common edges. You must therefore check the gap between faces before sewing or adjust the value of the tolerance according to the real gap of the geometry.

If all faces have been sewed correctly, the result is a shell. Otherwise, it is a compound. After a successful sewing operation all faces have a coherent orientation.

For more information, refer to the entry for this class in reference documentation.

### How to check the orientation of a solid

If you want to create a solid from a closed shell, you must first check the orientation to determine if you have to reverse the shell or not (for example after creating a closed shell from a sewing operation). To do this, use the <i> PerformInfinitePoint</i> method from the <i> BrepClass3D_SolidClassifier</i> class.

~~~~~
    BRepClass3d_SolidClassifier clas3d(aShell);
    clas3d.PerformInfinitePoint(Precision::Confusion());
    if (clas3d.State() == TopAbs_IN)
    newShell.Reverse();
    BRepBuilderAPI_MakeSolid aSolid(aShell);
~~~~~

@section OCCT_TOVW_SECTION_5 Visualization

Visualization in Open CASCADE Technology is based on the separation of modeling data you want to display and select, and on the graphical presentation of its structure.

For visualizing data structures, OCCT provides ready-to-use algorithms, which create graphic presentations from geometric models. These data structures may be used with the viewers supplied, and can be customized to take the specificity of your application into account.

Display is managed through presentation services, and selection in its turn is managed through selection services. With these services, data structures and algorithms are provided to display objects of an application and to support graphical selection of these objects.

Application Interactive Services (AIS) are provided to manage displaying, detection and selection of graphical presentations. These services associate data structures and interactive objects.

The technical overview provides only a basic description of the libraries. Please, refer for more details to  <a href="user_guides__visualization.html">Visualization User's guide</a>

See also: our web site at <a href="http://www.opencascade.org/support/training/">E-learning and Training</a>.

@image html /technical_overview/images/technical_overview_viz.png
@image latex /technical_overview/images/technical_overview_viz.png

@subsection OCCT_TOVW_SECTION_5_1 3D Graphics

3D Graphics provided by  <i>Graphic3d</i> package supports three-dimensional manipulation  of 3d graphic objects called structures. Structures, are made up of groups that unite primitives, such as polylines, planar polygons with or without holes, text and markers, and attributes,  such as color, transparency, reflection, line type, line width, and text font.
A group is the smallest editable element of a structure.

A structure can be displayed, erased, highlighted and transformed.
Structures can be connected to form a hierarchy of structures, composed by transformations.
The viewer can perform global manipulation of structures.

<i> Visual3d </i> package contains the group of classes required to implement commands for 3D viewer. The viewer manages views and light sources.

Most types of primitives supported by <i> Graphic3d</i> can be dumped to a vector file format such as PDF and PostScript. Export to vector formats is implemented with help of <i> GL2PS</i> library.

@subsection OCCT_TOVW_SECTION_5_2 3D Visualization

This library provides services for:

  * Selection of 3D data structures 
  * Presentation of 3D data structures 

Access to 3D presentation and selection is provided through AIS (Application Interactive Services). 
This package is a high-level interface that offers access to the lower-level presentation and selection services. 
AIS expand this underlying functionality with standard 3D selection attributes, presentation management,  and standard 3D presentation attributes, and manages it in the definition of GUI viewers. To implement these services, AIS package includes the following:

  * Interactive context 
  * Interactive objects 
  * A graphic attributes manager 
  * Selection filters 

### Interactive Context

Interactive context pilots 3D visualizations and selections. 
The interactive context allows you to manage, in a transparent way, graphic and "selectable" behavior of interactive objects which is not yet defined in the predefined types of these objects.

AIS have two operating context types. The default neutral point type allows easily visualizing and selecting entire interactive objects, which have been loaded into the context. 
Opening a local context allows preparing and using a temporary selection environment to select a part of an interactive object.

### Interactive Objects

Entities which are visualized and selected in the AIS viewer are objects.  They connect the underlying reference geometry of a model to its graphic representation in AIS. You can use predefined OCCT classes of standard interactive objects for which all necessary functions have already been programmed, or, in case you are an advanced user, you can implement your own classes of interactive objects.

### Graphic Attributes Manager

Graphic attributes manager, or AIS Drawer, stores graphic attributes  for specific interactive objects and for interactive objects controlled by interactive context.

Initially, all drawer attributes are filled out with the predefined  values which will define the default 3D object appearance.

When an interactive object is visualized, the required graphic attributes are first taken from its own drawer if one exists, or from the context drawer if no specific drawer for that type of object exists.

### Selection Filters

An important aspect in selection is the filtering of entities you to select. 
Selection filters allow you to refine the dynamic detection context, which you want to put into effect. Some of these filters can be used at the Neutral Point, others in an open local context only. You can also program your own filters and load them into the context.


@subsection OCCT_TOVW_SECTION_5_3 Application Interactive Services (AIS)

Application Interactive Services provide the means to create links between an application GUI viewer and the packages which are used to manage selection and presentation. The tools AIS defined for this include different sorts of entities: the selectable viewable objects themselves and the context and attribute managers to define their selection and display.

To orient the user as he works in a modeling environment, views and selections must be comprehensible. 
There must be several different sorts of selectable and viewable object defined. 
These must also be interactive, that is, connecting graphic representation and the underlying reference geometry. These entities are called Interactive Objects, and are divided into four types:
  * the Datum
  * the Relation
  * the Object
  * None.

The Datum groups together the construction elements such as lines, circles, points, trihedrons, plane trihedrons, planes and axes.
The Relation is made up of constraints on one or more interactive shapes and the corresponding reference geometry. For example, you might want to constrain two edges in a parallel relation. This constraint is considered as an object in its own right, and is shown as a sensitive primitive. This takes the graphic form of a perpendicular arrow marked with the || symbol and lying between the two edges.

The Object type includes topological shapes, and connections between shapes.

None, in order not to eliminate the object, tells the application to look further until it finds an object definition in its generation which is accepted.

Inside these categories, you have the possibility of an additional characterization by means of a signature. The signature provides an index to the further characterization. By default, the Interactive Object has a None type and a signature of 0 (equivalent to None.) 
If you want to give a particular type and signature to your interactive object, you must redefine the two virtual methods: <i> Type</i> and <i> Signature</i>.

In the C++ inheritance structure of the package, each class representing a specific Interactive Object inherits <i> AIS_InteractiveObject</i>. Among these inheriting classes, <i> AIS_Relation</i> functions as the abstract mother class for inheriting classes defining display of specific relational constraints and types of dimension. Some of these include:

  * display of constraints based on relations of symmetry, tangency, parallelism and concentricity
  * display of dimensions for angles, offsets, diameters, radii and chamfers.

No viewer can show everything at once with any coherence or clarity. 
Views must be managed carefully both sequentially and at any given instant. 
Another function of the view is that of a context to carry out design in. 
The design changes are applied to the objects in the view and then extended 
to the underlying reference geometry by a solver. 
To make sense of this complicated visual data, several display and selection tools are required. 
To facilitate management, each object and each construction element has a selection priority. 
There are also means to modify the default priority.

To define an environment of dynamic detection, you can use standard filter classes or create your own. 
A filter questions the owner of the sensitive primitive in local context to determine 
if it has the desired qualities. If it answers positively, it is kept. If not, it is rejected.

The standard filters supplied in AIS include:

  * <i> AIS_AttributeFilter</i>
  * <i> AIS_SignatureFilter</i>
  * <i> AIS_TypeFilter</i>.

Only the type filter can be used in the default operating mode, the neutral point. 
The others can only be used in open local contexts.

Neutral point and local context constitute the two operating modes of the 
central entity which pilots visualizations and selections, the Interactive Context. 
It is linked to a main viewer and if you like, a trash bin viewer as well.

The neutral point, which is the default mode, allows you to easily visualize and select interactive objects which have been loaded into the context. Opening local contexts allows you to prepare and use a temporary selection environment without disturbing the neutral point. 
A set of functions allows you to choose the interactive objects which you want to act on, the selection modes which you want to activate, and the temporary visualizations which you will execute. When the operation is finished, you close the current local context and return to the state in which you were before opening it (neutral point or previous local context).

An interactive object can have a certain number of graphic attributes, which are specific to it, such as visualization mode, color, and material. By the same token, the interactive context has a set of graphic attributes, the Drawer which is valid by default for the objects it controls. 
When an interactive object is visualized, the required graphic attributes are first taken from the object's own <i> Drawer</i> if one exists, or from the context drawer for the others.


@subsection OCCT_TOVW_SECTION_5_4 Presentation

### Presentation Management
   
<i> PrsMgr</i> package provides low level services and is only to be used when you do not want to use the services provided by AIS. It manages the display through the following services:
  * supplying a graphic structure for the object to be presented
  * recalculating presentations when required, e.g. by moving the object or changing its color
  * defining the display mode of the object to be presented; in the case of <i> AIS_Shape</i>, for example, this determines whether the object is to be displayed in wireframe (0) or shading (1) mode.

Note that each new Interactive Object must have all its display modes defined.

### Presentations of Geometry

The Presentations of Geometry component provides services for advanced programmers to extend the Application Interactive Services component, AIS. 
This would prove necessary in situations where new Interactive Objects were required.

The <i> StdPrs </i>package provides standard display tools for specific geometries and topologies whereas <i> Prs3d</i> provides those for generic objects. 
Among these classes are definitions of the display of the specific geometry or topology in various display modes such as wireframe, shading or hidden line removal mode.

### Presentation of Dimensions

 <i> DsgPrs </i> package provides tools for display of dimensions, relations and XYZ trihedrons.

@subsection OCCT_TOVW_SECTION_5_5 Selection

Selection of 3D data structures is provided using various algorithms. 
  
### Basic Selection


The <i> SelectBasics </i>package provides the following services:

  * the root definition of the sensitive primitive, a selectable entity in a view
  * the definition of the owner of a sensitive primitive; this entity relates the primitive to the application entity which is to be selected in the view.

### Standard Selections

 
The <i> StdSelect</i> package provides the following services:

  * definition of selection modes for topological shapes
  * definition of several filter standard <i> Selection2d.ap </i> classes
  * 3D viewer selectors.

Note that each new Interactive Object must have all its selection modes defined.
The <i>Select3D</i> package provides the following services:

  * definition of standard   3D sensitive primitives such as points, curves and faces;
  * recovery of the bounding boxes in the 2D graphic selection space, if required;
  * a 3D-2D projector.

### Selection Management

 
The <i> SelectMgr</i> package provides low level services and the classes 
<i> SelectMgr_SelectionManager</i> and <i> SelectMgr_ViewerSelector </i>
in particular are only to be used when you do not want to use the services provided by <i> AIS</i>.

<i> SelectMgr </i> manages the process of dynamic selection through the following services:

  * activating and deactivating selection modes for Interactive Objects
  * adding and removing viewer selectors
  * definitions of abstract filter classes

The principle of graphic selection consists in representing the objects which you want 
to select by a bounding box in the selection view. 
The object is selected when you use the mouse to designate the zone produced by the object.

To realize this, the application creates a selection structure 
which is independent of the point of view. This structure is made up 
of sensitive primitives which have one owner object associated to each of them. 
The role of the sensitive primitive is to reply to the requests of the selection algorithm 
whereas the owner's purpose is to make the link between 
the sensitive primitive and the object to be selected. 
Each selection structure corresponds to a selection mode which defines the elements that can be selected.

For example, to select a complete geometric model, 
the application can create a sensitive primitive for each face 
of the interactive object representing the geometric model. 
In this case, all the primitives share the same owner. 
On the other hand, to select an edge in a model, 
the application must create one sensitive primitive per edge.

~~~~

void InteractiveBox::ComputeSelection
      (const Handle(SelectMgr_Selection)& Sel,
       const Standard_Integer Mode){
  
switch(Mode){
case 0:
// locating the whole box by making its faces sensitive ...
         {
       Handle(SelectMgr_EntityOwner) Ownr = new
       SelectMgr_EntityOwner(this,5);
       for(Standard_Integer I=1;I<=Nbfaces;I++){
       Sel->Add(new Select3D_SensitiveFace
               (Ownr,[array of the vertices] face I);
       break;
       }

case 1:         // locates the   edges
         {
  
       for(Standard_Integer i=1;i<=12;i++){
                        // 1 owner per edge...
         Handle(mypk_EdgeOwner) Ownr =
                  new mypk_EdgeOwner(this,i,6);
                        // 6->priority
         Sel->Add(new
         Select3D_SensitiveSegment
                  (Ownr,firstpt(i),lastpt(i));
       }
       }
}

~~~~

The algorithms for creating selection structures store the sensitive primitives in a 
<i> SelectMgr_Selection </i> object. To do this, a set of ready-made sensitive primitives is supplied 
in the <i> Select2D </i>and <i> Select3D </i>packages. New sensitive primitives can be defined through inheritance 
from <i> SensitiveEntity</i>. For the application to make its own objects selectable,
 it must define owner classes inheriting <i> SelectMgr_EntityOwner</i>.

For any object inheriting from <i> AIS_InteractiveObject</i>, you redefine 
its <i> ComputeSelection</i> functions. In the example below there are different modes 
of selection on the topological shape contained within the interactive object, 
selection of the shape itself, the vertices, the edges, the wires, the faces.

~~~~
    void MyPack_MyClass::ComputeSelection(
                const Handle(SelectMgr_Selection)& aSelection,
                const Standard_Integer aMode)
    {
       switch(aMode){
       case 0:
       StdSelect_BRepSelectionTool::Load(
          aSelection,this,myShape,TopAbs_SHAPE);
       break;
       }
       case 1:
       StdSelect_BRepSelectionTool::Load(
          aSelection,this,myShape,TopAbs_VERTEX);
       break;
       }
       case 2:
       StdSelect_BRepSelectionTool::Load(
          aSelection,this,myShape,TopAbs_EDGE);
       break;
       }
       case 3:
       StdSelect_BRepSelectionTool::Load(
          aSelection,this,myShape,TopAbs_WIRE);
       break;
       }
       case 4:
       StdSelect_BRepSelectionTool::Load(
          aSelection,this,myShape,TopAbs_FACE);
       break;
       }
    }
~~~~

The <i> StdSelect_BRepSelectionTool </i> object provides a high level service 
which will make the shape <i> myShape</i> selectable when the <i> AIS_InteractiveContext</i> is asked to display your object.

Note:

The traditional way of highlighting selected entity owners 
adopted by Open CASCADE Technology assumes that each entity owner 
highlights itself on its own. This approach has two drawbacks: 

  * each entity owner has to maintain its own <i>Prs3d_Presentation object</i>, that results in large memory overhead for thousands of owners;
  * drawing selected owners one by one is not efficient from the OpenGL usage viewpoint.

That is why a different method has been introduced. 
On the basis of <i> SelectMgr_EntityOwner::IsAutoHilight() </i> return value 
<i> AIS_LocalContext </i> object either uses the traditional way of highlighting 
( <i> IsAutoHilight() </i> returned true) or groups such owners according to their 
Selectable Objects and finally calls <i> SelectMgr_SelectableObject::HilightSelected()</i> or 
<i> ClearSelected()</i>, passing a group of owners as an argument. 

Hence, an application can derive its own interactive object and redefine <i> HilightSelected()</i>,
 <i> ClearSelected()</i> and <i> HilightOwnerWithColor()</i> virtual methods 
 to take advantage of such OpenGL technique as arrays of primitives. 
 In any case, these methods should at least have empty implementation.

 The <i> AIS_LocalContext::UpdateSelected(const Handle(AIS_InteratciveObject)&, Standard_Boolean) 
 </i> method can be used for efficient redrawing a selection presentation for a given interactive object from an application code.

Additionally, the <i> SelectMgr_SelectableObject::ClearSelections() </i> 
method now accepts an optional Boolean argument. 
This parameter defines whether all object selections should be flagged for further update or not. 
This improved method can be used to re-compute an object selection (without redisplaying the object completely) 
when some selection mode is activated not for the first time.


@subsection OCCT_TOVW_SECTION_5_6 Attribute Management

The Attribute Management tool-kit provides services for advanced programmers to extend 
the Application Interactive Services component, AIS. This would prove necessary 
in situations where new Interactive Objects were required.

The <i> Prs3d </i> package provides the following services:

  * a presentation object (the context for all modifications to the display, its presentation will be displayed in every view of an active viewer)
  * an attribute manager governing how objects such as color, width, and type of line are displayed; these are generic objects, whereas those in <i>StdPrs </i> are specific geometries and topologies.
  * generic algorithms providing default settings for objects such as points, curves, surfaces and shapes
  * a root object which provides the abstract framework for the DsgPrs definitions at work in display of dimensions, relations and trihedrons.


@subsection OCCT_TOVW_SECTION_5_7 Mesh Visualization Services

<i> MeshVS</i> (Mesh Visualization Service) component extends 3D visualization capabilities 
of Open CASCADE Technology. It provides flexible means of displaying meshes along with associated pre- and post-processor data. 

From a developer's point of view, it is easy to integrate the MeshVS component i
nto any mesh-related application with the help of the following guidelines:

Derive a data source class from the MeshVS_DataSource class. 
Re-implement its virtual methods, so as to give the <i> MeshVS</i> component access 
to the application data model. This is the most important part of the job, 
since visualization performance is affected by performance of data retrieval methods of your data source class.

Create an instance of the <i> MeshVS_Mesh</i> class. 

Create an instance of your data source class and pass it to a <i> MeshVS_Mesh </i> object through the <i> SetDataSource()</i> method.

Create one or several objects of <i> MeshVS_PrsBuilder</i>-derived classes 
(either standard, included in the <i> MeshVS</i> package, or your custom ones). 
Each <i> PrsBuilder</i> is responsible for drawing a <i> MeshVS_Mesh</i> presentation 
in certain display mode(s) specified as a <i> PrsBuilder</i> constructor's argument. 
Display mode is treated by <i> MeshVS</i> classes as a combination of bit flags
(two least significant bits are used to encode standard display modes: wireframe, shading and shrink). 
Pass these objects to the <i> MeshVS_Mesh::AddBuilder()</i> method. <i> MeshVS_Mesh</i> 
takes advantage of improved selection highlighting mechanism: it highlights its selected entities itself, 
with the help of so called "highlighter" object. You can set one of <i> PrsBuilder</i> 
objects to act as a highlighter with the help of a corresponding argument of the <i> AddBuilder()</i> method.

Visual attributes of the <i> MeshVS_Mesh </i> object (such as shading color, shrink coefficient and so on) 
are controlled through <i> MeshVS_Drawer</i> object. It maintains a map "Attribute ID --> attribute value" 
and can be easily extended with any number of custom attributes.

In all other respects, <i> MeshVS_Mesh</i> is very similar to any other class derived 
from <i> AIS_InteractiveObject</i> and it should be used accordingly 
(refer to the description of <i> AIS package</i> in the documentation).

@subsection OCCT_TOVW_SECTION_5_8 Images and Drivers

### Images

The <i> Image</i> package provides <i> PseudoColorImage</i> 
and <i> ColorImage</i> definitions, and a set of key functions from the image fields. 

The <i> AlienImage</i> package allows importing images from other formats into OCCT format.

### Drivers

The <i> Xw </i>package contains the common X graphic interface. It uses <i> XWindow </i> bitmap fonts that cannot be modified.

The <i> WNT</i> package contains the common Windows NT graphic interface.

The <i> Cocoa</i> package provides interaction with Cocoa API on Mac OS X.

@subsection OCCT_TOVW_SECTION_5_9 New Interactive Services (NIS)

New Interactive Services package provides the API similar to the traditional AIS but with some important differences/improvements:

  * Each type of <i> InteractiveObject</i> should have a corresponding Drawer class that defines the presentation of the Object type using direct OpenGl calls. This is a much faster way to display 3D objects, providing for more than 1 million separate selectable entities in one view.
  * The abstract type <i> NIS_InteractiveObject</i> does not support any properties (color, material, other aspects). The relevant properties should be defined in the specializations of the Drawer class, and the API to set/modify should be implemented in the specializations of InteractiveObject class. 
  * Interactive selection is managed by <i> InteractiveObject</i> methods instead of special selector classes and data types. This is possible since in NIS the selection is based on 3D representation (by a ray or a box corresponding to the view direction) without intermediate 2D projection.
  * Many <i> InteractiveContext</i> instances can be attached to a <i> V3d_View</i>, these instances being independent containers of interactive objects; removal (detaching) of <i> InteractiveContext</i> instance destroys the contained objects.
  * All data types and algorithms are designed to provide the best performance for both OpenGl (server side) and application. On the other hand, the API is open to any feature supported by any version of OpenGl. This allows building custom presentations quickly and efficiently.
  * Standard <i> NIS_View</i> subclasses <i> V3d_View</i> thus providing all its public API, such as scene definition (view orientation, lights, background, etc.) and the standard view transformations (pan/zoom/rotate,fitAll,...). The traditional AIS-based presentations (e.g., <i> AIS_Shape</i>) are also supported, they can be rendered together with NIS presentations in the same view window.

The DRAW test plugin, <i> TKViewerTest</i>, has been modified 
to manage <i> AIS_InteractiveContext</i> and <i> NIS_InteractiveContext</i> together in one view window.

@subsection OCCT_TOVW_SECTION_5_10 Voxels

A voxel is a sub-volume box with constant scalar/vector value. 
The object in voxel representation is split into many small sub-volumes (voxels) and its properties are distributed through voxels.

Voxels are used for analysis and visualization of 3D-dimensional distribution of data. 
Medicine (mainly, tomography), computational physics (hydrodynamics, aerodynamics, nuclear physics) 
and many other industries use voxels for 3D data visualization and analysis of physical processes.

Open CASCADE Technology provides several basic data containers for voxels 
with fast access to the data and optimal allocation of data in memory. 
Also, a special visualization toolkit allows visualizing voxels 
as colored or black/white points and cubes, displaying only the voxels visible from the user's point of view.

Please, see for more information <a href="occt_voxels_wp.html">Voxels User's Guide</a>.


@subsection OCCT_TOVW_SECTION_5_11 Examples

### How to change graphic attributes of an interactive object

The set of graphic attributes of an interactive object is defined in AIS_Drawer. 
Each interactive object can have its own visualization attributes.

By default, the interactive object takes the graphic attributes of 
the interactive context in which it is visualized 
(visualization mode, deflection, values for the calculation of presentations, 
number of isoparametric lines, color, type of line, material, etc.)

In the <i> AIS_InteractiveObject</i> abstract class, several standard attributes 
have been privileged. These include: color, thickness of line, material, and transparency. 
Consequently, a certain number virtual functions which allow us to act on these attributes have been proposed. 
Each new class of interactive object can use them as they are or 
can redefine these functions to bring about the changes it should produce in the behavior of the class.

Other attributes can be changed by acting directly on the drawer of the object. 
An interactive object has a specific drawer as soon as you change an attribute on it. 
If you do not modify any graphic attribute on it, the default drawer of the interactive context is referenced and used.

To get the <i> AIS_Drawer</i> of an object, call method <i> AIS_InteractiveObject::Attributes </i>.

To set the <i> AIS_Drawer</i> of an object, call method <i> AIS_InteractiveObject::SetLocalAttributes </i>. 

### How to dump a scene from the viewer

You can dump the contents of a <i> V3D_View</i> in a file with the same scale or 
with a different scale according to the required paper size (format) 
and the aspect ratio of the view. This is provided by method <i>V3d_View::Dump</i>. For example:

~~~~
CString filename ("myView3D.bmp");
myView->Dump (filename, Aspect_FOSP_A4);
~~~~

<i> myView</i> is a <i> V3d_View</i>, where OCCT objects are displayed using, for example, AIS services.

Please, note:

  * The file name extension can be any among ".xwd", ".png", or ".bmp" formats both on UNIX or NT.
  * Be careful about dump time requirements of the resulting file, especially for the A formats.
  * The GIF format generates very small files, BMP and XWD generates much larger files (4 to 6 times the size of a GIF).
  * The time to generate these files is very short with the XWD format but 2 to 4 times longer for the other formats.
  * After getting an image file of your view, you can use any standard application for editing or sending the image file to a printer (i.e.: Microsoft Photo Editor on Windows or Image Viewer on SUN system)

### How to add and remove objects from Selections


You can add or remove an object from a selection in one of two ways. You can use:

  * <i> AIS_InteractiveContext::AddOrRemoveCurrentObject</i> method at neutral points;
  * <i> AddOrRemoveCurrent </i> method if a local context is opened.

 
### How to detect overlapped objects


When objects overlap each other and cause difficulties in selection, 
you can use the mechanism provided with the <i> AIS_InteractiveContext</i> 
to successively highlight all the objects found under the selection. 
This allows you to choose and validate the required object.

~~~~
    If ( myAISContext->HasNextDetected()) {

    // if up key is pressed
    myAISContext ->HilightNextDetected(myView);

    // if down key is pressed
    myAISContext ->HilightPreviousDetected(myView);

    }
~~~~



### Get mouse coordinates in 3D view


To switch from pixel mouse position on the screen to 3D coordinates 
in <i> V3d_View</i>, use <i>V3d_View::Convert</i> method.

~~~~
    Handle(V3d_View) aview
    aView->Convert(Xp,Yp,X,Y,Z)
~~~~

Where <i> Xp</i>, <i> Yp</i> are the mouse coordinates in pixels and X,Y,Z the real coordinates in 3D space.

### 3D Viewer Objects

The <i> V3d </i>  package contains the set of commands and services of the 3D Viewer. 
It provides a set of high level commands to control views and viewing modes. 
This package is complementary to the <i> Visual3D</i> graphic package.

<i> CSF_WALKTHROUGH</i> variable enables you to manage the perspective of the view 
in the viewer by defining <i> setenv CSF_WALKTHROUGH </i> "Yes".

If you use the syntax <i> unsetenv CSF_WALKTHROUGH </i>, you make sure that the variable 
is deactivated. In this case, the eye is located outside the 3D bounding box of the view. 
This is the default behavior for managing the view perspective.

@section OCCT_TOVW_SECTION_6 Data Exchange

Data Exchange is a key factor in using Open CASCADE Technology (as well as applications based thereon) 
concurrently with other software such as CAD systems. It provides the openness of OCCT in a multi-software environment, 
by allowing it to process external data and providing a good level of integration.

This means obtaining results of good quality, and covering the needs of exchanges 
from OCCT-based applications regardless of external data quality or requirements, 
in particular in respect of allowed data types and arrangements between them, accepted gaps between geometries.

This matter is addressed by Data Exchange Module, which is organized in a modular way.

@image html /technical_overview/images/technical_overview_de.png
@image latex /technical_overview/images/technical_overview_de.png

Data Exchange interfaces in OCCT allow software based on OCCT 
to exchange data with various CAD software, thus ensuring a good level of interoperability.

Data Exchange interfaces function either in accordance with the standards (IGES, STEP), 
which can be used by various software packages for CAD, PDM etc., or as direct connectors to proprietary formats.

### Standardized Data Exchange


* STEP (AP203 : Mechanical Design, this covers General 3D CAD; AP214: Automotive Design) 
* IGES (up to 5.3) 
* STL 
* VRML

Data Exchange interfaces (STEP, IGES) allow to query and examine a file, 
results of conversion and its validity. They are designed to support extensions (like new standards) in a common modular architecture.

### Extended data exchange


Extended data exchange (XDE) allows you to extend the scope of exchange by translating
 additional data attached to geometric ("BREP") data, thereby improving the interoperability with external software.
Data types such as colors, assembly descriptions and validation properties 
(i.e. center of gravity, etc.) are supported. These data are stored together with shapes in an OCAF (XCAF) document.


### Proprietary Data Exchange

In addition to standard Data Exchange interfaces, separate components are available 
to provide direct mapping and data adaptation (using Shape Healing) with CAD software supporting the following formats:

* ACIS SAT
* Parasolid X_T
* DXF

These components are based on the same architecture as interfaces with STEP and IGES.

### Translating a shape to STL Format


OCCT includes a module for translating OCCT shapes to STL (Stereolithography) format. 
STL is a format designed for rapid prototyping. 
It is intended to send geometric data (volumic) to stereolithography machines, 
which can read and interpret such data. These machines can transform a volumic model 
to a physical prototype made of plastic, by using laser to coagulate material, 
which corresponds to the volume, and set free the material around. 
STL defines these surfaces by triangles. 
Thus, no machining is required to switch from a virtual model to a physical one.

Since STL files can only include solids described by their mesh structures, 
OCCT shapes, which are intended to be written, must be solids, 
components of solids or closed shells with a correct orientation.

When translating shapes to STL format, remember that all references 
to shapes mean references to OCCT shapes unless otherwise explicitly defined. 
In addition, sets of faces or unclosed shells may also be translated but visualization in foreign viewers may be incorrect.

### Translating a shape to VRML Format

The Virtual Reality Modeling Language (VRML) is a language for describing multi-participant interactive simulations, virtual worlds networked via the Internet and hyperlinked with the World Wide Web. VRML is a format designed for animated visualization of solids.
OCCT includes a module for translating OCCT shapes to VRML (Virtual Reality Modeling Language). 
OCCT shapes may be translated in two representations (states): shaded or wireframe. 
Since shaded VRML format files include only solids described by their mesh structures, the OCCT shapes intended to be written must be solids, components of solids or closed shells with a correct orientation.

@subsection OCCT_TOVW_SECTION_6_1 General Definitions

OCCT general definitions for Data Exchange include several enumerations and classes used by IGES and STEP data exchange interfaces.

To define translation parameters and file headers, you can use:

  * <i> Interface_InterfaceModel</i>
  * <i> Interface_Static</i>

To manage Message display, use class <i> Mesage_Messenger</i>.

To define the type of analysis of the source file, and to ensure the success 
of the loading operation, you use the following enumerations from the <i> IFSelect</i> package:

  * <i> PrintCount</i>
  * <i> ReturnStatus</i>

To read and write attributes such as names, colors, layers for IGES and STEP 
and validation properties and structure of assemblies for STEP, you can use an XDE document.

It is possible to learn more about XDE documents from <a href="user_guides__xde.html">XDE User's guide</a>.

See also: our web site at <a href="http://www.opencascade.org/support/training/">E-learning and Training</a>.


 
@subsection OCCT_TOVW_SECTION_6_2 IGES

The IGES interface reads IGES files and translates them to Open CASCADE Technology models. 
IGES files produced in accordance with IGES standard versions up to and including version 5.3 can be read.
The interface is able to translate one entity, a group of entities or a whole file.
Before beginning a translation, you can set a range of parameters to manage the translation process.
If you like, you can also check file consistency before translation.

The IGES interface also translates OCCT models to IGES files. 
IGES files produced by this component conform to IGES standard version 5.3.

Other kinds of data such as colors and names can be read or written 
with the help of XDE tools <i> IGESCAFControl_Reader</i> and <i> IGESCAFControl_Writer</i>. 

Please, note:

  * an IGES model is an IGES file that has been loaded into memory.
  * an IGES entity is an entity in the IGES normal sense.
  * a root entity is the highest level entity of any given type, e.g. type 144 for surfaces and type 186 for solids. Roots are not referenced by other entities.

It is possible to learn more about the IGES interface from <a href="user_guides__iges.html">IGES User's guide</a>.

See also: our web site at <a href="http://www.opencascade.org/support/training/">E-learning and Training</a>.

@subsection OCCT_TOVW_SECTION_6_3 STEP

The STEP interface reads STEP files produced in accordance with STEP Application Protocol 214 (Conformance Class 2 both CD and DIS versions of schema) and translates them to Open CASCADE Technology models. STEP Application Protocol 203 is also supported.

The STEP interface also translates OCCT models to STEP files. STEP files that are produced by this interface conform to STEP AP 203 or AP 214 (Conformance Class 2, either CD or DIS version of the schema) depending on the user's option.

Basic interface reads and writes geometrical, topological STEP data and assembly structures. 

The interface is able to translate one entity, a group of entities or a whole file.

Other kinds of data such as colors, validation properties, layers, names and the structure of assemblies can be read or written with the help of XDE tools - <i> STEPCAFControl_Reader</i> and <i> STEPCAFControl_Writer</i>. 

To choose a translation mode when exporting to a STEP format, use <i> STEPControl_STEPModelType</i>.

There is a set of parameters that concern the translation and can be set before the beginning of the translation.

Please, note:
* a STEP model is a STEP file that has been loaded into memory;
* all references to shapes indicate OCCT shapes unless otherwise explicitly stated;
* a root entity is the highest level entity of any given type, i.e. an entity that is not referenced by any other one.

It is possible to learn more about the STEP interface from <a href="user_guides__step.html">STEP User's guide</a>.

See also: our web site at <a href="http://www.opencascade.org/support/training/">E-learning and Training</a>.

@subsection OCCT_TOVW_SECTION_6_4 STL

The STL component translates Open CASCADE Technology shapes to STL files. STL (Stereolithography) format is widely used for rapid prototyping.

As STL files can only include solids described by their mesh structure, 
the OCCT shapes to be written must be solids, components of solids or closed shells with a correct orientation.

Note All references to shapes indicate OCCT shapes unless otherwise explicitly stated.

Sets of faces or unclosed shells may also be translated to STL format but visualization with foreign viewers may be incorrect.

@subsection OCCT_TOVW_SECTION_6_5 VRML
 
The VRML component translates Open CASCADE Technology shapes to VRML 1.0 files 
(Virtual Reality Modeling Language). OCCT shapes may be translated in two representations: 
shaded or wireframe. A shaded representation present shapes as sets of triangles 
computed by a mesh algorithm while a wireframe representation present shapes as sets of curves.

As shaded VRML format files only include solids described by their mesh structures, 
the OCCT shapes to be written must be solids, components of solids or closed shells with a correct orientation.

Please, note:

  * all references to shapes indicate OCCT shapes unless otherwise explicitly stated;
  * sets of faces or unclosed shells may also be translated to shaded VRML format but visualization with foreign viewers may be incorrect.

@section OCCT_TOVW_SECTION_7 Application Framework

The Application Framework uses an associativity engine to simplify the development of a CAD application. 
Based on application/document architecture, it does this due to the following features:

  * Application data is handled by the mechanism of attributes 
  * Attributes may be organized according to your development needs 
  * Multiple documents can be managed by an application 
  * Ready-to-use modeling data attributes common to CAD/CAM applications 
  * Document modification and recomputation
  * Data storage services 
  * A ready-to-use Undo-Redo and Copy-Paste functions 

Since OCAF handles your application structure, your only major development task is the creation 
of application-specific data and GUIs. It is the organization of application data 
due to which OCAF differs from any other CAD framework. In OCAF, data structures are not shape-driven, 
but reference-key driven. In this respect, attributes such as shape data, color, material, 
are attached to a deeper invariant structure of a model than the shapes themselves. 
Then OCAF organizes and embeds these attributes in a document. 
For example, a geometry becomes the value of the Shape attribute, 
in the same way as a number is the value of the Integer attribute and a string is the value of the Name attribute.

OCAF documents are in their turn managed by an OCAF application.

Please, refer for more details to <a href="user_guides__ocaf.html">OCAF User's guide</a> and the OCAF white papers:
* <a href="ocaf_wp.html">Application Framework</a>  
* <a href="occt_ocaf_tree_wp.html">Distribution of Data through OCAF Tree</a>  
* <a href="occt_ocaf_functionmechanism_wp.html">Application Framework Function Mechanism</a>  

See also: our web site at <a href="http://www.opencascade.org/support/training/">E-learning and Training</a>.

@subsection OCCT_TOVW_SECTION_7_1 How to start working with OCAF

To create a useful OCAF-based application, it is necessary to redefine the following
two deferred methods: <i> Formats</i> and <i> ResourcesName</i>

In the <i> Formats </i> method, it is necessary to add the format 
of the documents to be read by the application and which may have been built in other applications.

For example:

~~~~
    void myApplication::Formats(TColStd_SequenceOfExtendedString& Formats)
    {
      Formats.Append(TCollection_ExtendedString ("OCAF-myApplication"));
    }
~~~~

In the <i> ResourcesName</i> method, you only define the name of the resource file. This
file contains several definitions for the saving and opening mechanisms associated
with each format and calling of the plug-in file.

~~~~
    Standard_CString myApplication::ResourcesName()
    {
      return Standard_CString ("Resources");
    }
~~~~

To obtain the saving and opening mechanisms, it is necessary to set two environment
variables: <i> CSF_PluginDefaults</i>, which defines the path of the plug-in file and <i> CSF_ResourcesDefault</i>, which defines the resource file:

~~~~
    SetEnvironmentVariable ( "CSF_ResourcesDefaults",myDirectory);
    SetEnvironmentVariable ( "CSF_PluginDefaults",myDirectory);
~~~~

The plugin and the resource files of the application will be located in <i> myDirector</i>.
The name of the plugin file must be <i>Plugin</i>.

### Resource File

The resource file describes the documents (type and extension) and 
the type of data that the application can manipulate 
by identifying the storage and retrieval drivers appropriate for this data.

Each driver is unique and identified by a GUID generated, for example, with the <i> uuidgen </i> tool in Windows.

Five drivers are required to use all standard attributes provided within OCAF:

  * the schema driver (ad696002-5b34-11d1-b5ba-00a0c9064368)
  * the document storage driver (ad696000-5b34-11d1-b5ba-00a0c9064368)
  * the document retrieval driver (ad696001-5b34-11d1-b5ba-00a0c9064368)
  * the attribute storage driver (47b0b826-d931-11d1-b5da-00a0c9064368)
  * the attribute retrieval driver (47b0b827-d931-11d1-b5da-00a0c9064368)

These drivers are provided as plug-ins and are located in the <i> PappStdPlugin</i> library.


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

The plugin file describes the list of required plug-ins to run the application and the
libraries in which plug-ins are located.

You need at least the <i> FWOSPlugin</i> and the plug-in drivers to run an OCAF application.

The syntax of each item is <i> Identification.Location Library_Name, </i> where:
* Identification is GUID.
* Location defines the location of the Identification (where its definition is found).
* Library_Name is the name (and path to) the library, where the plug-in is located.

For example, this is a Plugin file:

~~~~
a148e300-5740-11d1-a904-080036aaa103.Location: FWOSPlugin
! base document drivers plugin
ad696000-5b34-11d1-b5ba-00a0c9064368.Location: PAppStdPlugin
ad696001-5b34-11d1-b5ba-00a0c9064368.Location: PAppStdPlugin
ad696002-5b34-11d1-b5ba-00a0c9064368.Location: PAppStdPlugin
47b0b826-d931-11d1-b5da-00a0c9064368.Location: PAppStdPlugin
47b0b827-d931-11d1-b5da-00a0c9064368.Location: PAppStdPlugin
~~~~
 
@subsection OCCT_TOVW_SECTION_7_2 Data Attributes

The following ready-to-use attributes are provided:
  * Shape attributes, which contain shapes and their evolution 
  * Standard attributes, a collection of common CAD/CAM attributes including: 
    * Real 
    * Integer 
    * Name 
    * Constrain 
  * Visualization attributes implement the Application Interactive Services in the context of Open CASCADE Application Framework.
  * Function attributes which regenerate any data affected by modifications made in a
document 

### Shape Attributes

A topological attribute can be seen as a hook into the topological structure. To
this hook, data can be attached and references defined.


It is used for keeping and access to topological objects and their evolution. All
topological objects are stored in the one user-protected <i> TNaming_UsedShapes attribute</i>
at the root label of the data framework. This attribute contains map with all topological
shapes, used in this document.

TNaming_NamedShape attribute can be added to any other attribute. This attribute contains
references (hooks) to shapes from the <i> TNaming_UsedShapes</i> attribute and evolution
of these shapes. <i> TNaming_NamedShape </i> attribute contains a set of pairs of hooks: old
shape and new shape (see the figure below). It allows not only get the topological
shapes by the labels, but also trace evolution of the shapes and correctly resolve
dependent shapes by the changed one. 

If a shape is newly created, the old shape for the corresponding named shape is an empty
shape. If a shape is deleted, then the new shape in this named shape is empty.

@image html /technical_overview/images/technical_overview_shapeattrib.png
@image latex /technical_overview/images/technical_overview_shapeattrib.png


### Shape attributes in data framework. 

Algorithms can dispose sub-shapes of the result shape at the individual
label depending on necessity: 

  * If a sub-shape must have some extra attributes (material of each face or color of each edge). In this case a specific sub-shape is placed to the separate label (usually, sub-label of the result shape label) with all attributes of this sub-shape. 
  * If topological naming is needed, a necessary and sufficient (for selected sub-shapes identification) set of sub-shapes is placed to the child labels of the result shape label. As usual, as far as basic solids and closed shells are concerned, all faces of the shape are disposed. Edges and vertices sub-shapes can be identified as intersection of contiguous faces. Modified/generated shapes may be placed to one named shape and identified as this named shape and source named shape that also can be identified with used algorithms. 

<i> TNaming_NamedShape </i> may contain a few pairs of hooks with the same evolution. In this
case topology shape, which belongs to the named shape, is a compound of new shapes.

The data model contains both the topology and the hooks, and functions handle both topological entities and hooks. Consider the case of a box function, which creates a solid with six faces and six hooks. Each hook is attached to a face. If you want, you can also have this function create hooks for edges and vertices as well as for faces. 

Not all functions can define explicit hooks for all topological entities they create, but all topological entities can be turned into hooks when necessary. This is where topological naming is necessary. 

Consider the following example. A box defines six hooks for the six faces, but a protrusion created on a face of the box can only define two hooks, one for the top face, and one for all the lateral faces. As the basic wire defining the protrusion may change in the future the protrusion function cannot designate the lateral faces without ambiguity, their number may change. Figure 6 illustrates this example, faces F1 to F6 of the box each have a hook. Faces F7 to F10, the lateral faces of the protrusion, share a single hook, and face F11, the top face, has one hook.

@image html /technical_overview/images/technical_overview_occ_0068.png
@image latex /technical_overview/images/technical_overview_occ_0068.png

This structure raises two problems: 

  * the value of the face F6 attribute-hook has changed; 
  * no data can be attached to F7. 

When a hook designates multiple faces like F7-F10 (or the hook on F6 if F6 was split)
it is impossible to attach data to an individual face like F7. 

In fact, the protrusion has a trimmed face F6. As a result, the value of this face
has changed and the current value of the hook attached to it needs to be found. Note
that this face could have been split in two faces (for example if the function had
been a slot) and both new faces would have been attached to the same hook.


### Standard Attributes


Standard attributes are already existing ready-to-use attributes, which allow you
to create and modify labels and attributes for many basic data types.

To find an attribute attached to a specific label, you use the GUID of the type of
attribute you are looking for. For this, find this information using the method 
<i> GetID</i> and the method <i> Find</i> for the label as follows:

~~~~
    Standard_GUID anID = MyAttributeClass::GetID();
    Standard_Boolean HasAttribute = aLabel.Find(anID,anAttribute);
~~~~


### Function Attributes


A model consists of data and algorithms manipulating with data. OCAF attributes store data. A Function attribute stores data corresponding to a Function (see the white paper <a href="occt_ocaf_functionmechanism_wp.html"> Application Framework Function Mechanism </a>). This mechanism manipulates with algorithms computing the model in the optimal way following the modifications. 


@subsection OCCT_TOVW_SECTION_7_3 Persistent Data Storage

There are three schemas of persistence, which you can use to store and retrieve OCAF data (documents):

  * <i> Standard</i> persistence schema, compatible with previous OCAF applications
  * <i> XmlOcaf</i> persistence, allowing the storage of all OCAF data in XML form
  * <i> BinOcaf</i> persistence, allowing the storage of all OCAF data in binary format form


All schemes are independent of each other, but they guarantee that the standard OCAF
attributes stored and retrieved by one schema will be storable and retrievable by
the other. Therefore in any OCAF application you can use any persistence schema or
even all three of them. The choice is made by the Format string of stored OCAF documents
or automatically by the file header data -  * on retrieval.

Persistent data storage in OCAF using the <i> Standard</i> package is presented in: 

  * Basic Data Storage 
  * Persistent Collections 

Persistent storage of shapes is presented in the following chapters:

  * Persistent Geometry 
  * Persistent Topology 

Finally, information about opening and saving persistent data is presented in Standard
Documents. 


@subsubsection OCCT_TOVW_SECTION_7_3_1 Basic Data Storage

Normally, all data structures provided by Open CASCADE Technology are run-time structures,
in other words, transient data. As transient data, they exist only while an application
is running and are not stored permanently. However, the Data Storage module provides
resources, which enable an application to store data on disk as persistent data.

Data storage services also provide libraries of persistent classes and translation
functions needed to translate data from transient to persistent state and vice-versa.

#### Libraries of persistent classes

Libraries of persistent classes are extensible libraries of elementary classes you
use to define the database schema of your application. They include:
* Unicode (8-bit or 16-bit character type) strings 
* Collections of any kind of persistent data such as arrays, stacks, queues, and graphs.

All persistent classes are derived from the \b Persistent base class, which defines
a unique way of creating and handling persistent objects. You create new persistent
classes by inheriting from this base class.

#### Translation Functions

Translation functions allow you to convert persistent objects to transient ones and
vice-versa. These translation functions are used to build Storage and Retrieval drivers
of an application.

For each class of 2D and 3D geometric types, and for the general shape class in the
topological data structure library, there are corresponding persistent class libraries,
which allow you to translate your data with ease.

#### Creation of Persistent Classes

If you are using Unix platforms as well as WOK and CDL, you can create your own persistent
classes. In this case, data storage is achieved by implementing Storage and Retrieval
drivers.

The <i> Storage </i> package is used to write and read persistent objects. 
These objects are read and written by a retrieval or storage algorithm 
(<i> Storage_Schema </i>object) in a container (disk, memory, network ...). 
Drivers (<i> FSD_File</i> objects) assign a physical container for data to be stored or retrieved.

The standard procedure for an application in reading a container is as follows:

  *open the driver in reading mode,
  *call the Read function from the schema, setting the driver as a parameter. This function returns an instance of the <i> Storage_Data </i> class which contains the data being read,
  *close the driver.

The standard procedure for an application in writing a container is as follows:

  *open the driver in writing mode,
  *create an instance of the <i> Storage_Data </i> class, then add the persistent data to write with the function <i> AddRoot</i> ,
  *call the function <i> Write </i> from the schema, setting the driver and the <i> Storage_Data </i> instance as parameters,
  *close the driver.

@subsubsection OCCT_TOVW_SECTION_7_3_2 Persistent Collections

Persistent collections are classes which handle dynamically sized collections of data that can be stored in the database. These collections provide three categories of service:

  * persistent strings,
  * generic arrays of data, 
  * commonly used instantiations of arrays.

Persistent strings are concrete classes that handle sequences of characters based
on both ASCII (normal 8-bit) and Unicode (16-bit) character sets.

Arrays are generic classes, that is, they can hold a variety of objects not necessarily inheriting from a unique root class. These arrays can be instantiated with any kind of storable or persistent object, and then inserted into the persistent data model of a user application.

The purpose of these data collections is simply to convert transient data into its persistent equivalent so that it can be stored in the database. To this end, the collections are used to create the persistent data model and assure the link with the database. They do not provide editing or query capabilities because it is more efficient, within the operative data model of the application, to work with transient data structures (from the <i> TCollection</i> package).

For this reason:

  * the persistent strings only provide constructors and functions to convert between transient and persistent strings, and
  * the persistent data collections are limited to arrays. In other words, <i> PCollection</i> does not include sequences, lists, queues, sets, stacks and so on (unlike <i> TCollection</i>).

Persistent string and array classes are found in the <i> PCollection</i> package. In addition, <i> PColStd</i> package provides standard, and frequently used, instantiations of persistent arrays, for very simple objects.

@subsubsection OCCT_TOVW_SECTION_7_3_3 Persistent Geometry

The Persistent Geometry component describes geometric data structures which can be stored in the database. These packages provide a way to convert data from the transient "world" to the persistent "world".

Persistent Geometry consists of a set of atomic data models parallel to the geometric data structures described in the geometry packages. Geometric data models, independent of each other, can appear within the data model of any application. The system provides the means to convert each atomic transient data model into a persistent one, but it does not provide a way for these data models to share data.

Consequently, you can create a data model using these components, store data in, and retrieve it from a file or a database, using the geometric components provided in the transient and persistent "worlds". In other words, you customize the system by declaring your own objects, and the conversion of the geometric components from persistent to transient and vice versa is automatically managed for you by the system.

However, these simple objects cannot be shared within a more complex data model. To allow data to be shared, you must provide additional tools.

Persistent Geometry is provided by several packages.

The <i> PGeom</i> package describes geometric persistent objects in 3D space, such as points,
vectors, positioning systems, curves and surfaces.

These objects are persistent versions of those provided by the <i> Geom</i> package: for
each type of transient object provided by Geom there is a corresponding type of persistent
object in the <i>PGeom</i> package. In particular the inheritance structure is parallel.

However the <i> PGeom </i>package does not provide any functions to construct, edit or access
the persistent objects. Instead the objects are manipulated as follows:

  * Persistent objects are constructed by converting the equivalent transient <i> Geom </i> objects. To do this you use the <i>MgtGeom::Translate</i> function.
  * Persistent objects created in this way are used to build persistent data structures that are then stored in a file or database.
  * When these objects are retrieved from the file or database, they are converted back into the corresponding transient objects from the Geom package. To do this, you use <i>MgtGeom::Translate</i> function.

In other words, you always edit or query transient data structures within the transient
data model supplied by the session.
Consequently, the documentation for the <i> PGeom </i> package consists simply of a list of available objects.

The <i> PGeom2d </i> package describes persistent geometric objects in 2D space, such as points,
vectors, positioning systems and curves. This package provides the same type of services
as the <i> PGeom</i> package, but for the 2D geometric objects provided by the <i> Geom2d</i> package.
Conversions are provided by the <i>MgtGeom::Translate</i> function.

~~~~
//Create a coordinate system
Handle(Geom_Axis2Placement) aSys;


//Create a persistent coordinate PTopoDS_HShape.cdlsystem
Handle(PGeom_Axis2placement)
	aPSys = MgtGeom::Translate(aSys);

//Restore a transient coordinate system
Handle(PGeom_Axis2Placement) aPSys;

Handle(Geom_Axis2Placement)
	aSys = MgtGeom::Translate(aPSys);
~~~~


@subsubsection OCCT_TOVW_SECTION_7_3_4 Persistent Topology

The Persistent Topology component describes topological data structures which can be stored in the database. These packages provide a way to convert data from the transient "world" to the persistent "world".

Persistent Topology is based on the BRep concrete data model provided by the topology packages. Unlike the components of the Persistent Geometry package, topological components can be fully shared within a single model, as well as between several models.

Each topological component is considered to be a shape: a <i> TopoDS_Shape</i> object. The system's capacity to convert a transient shape into a persistent shape and vice-versa applies to all objects, irrespective of their complexity: vertex, edge, wire, face, shell, solid, and so on.

When a user creates a data model using BRep shapes, he uses the conversion functions that the system provides to store the data in, and retrieve it from the database. The data can also be shared.

Persistent Topology is provided by several packages.

The <i> PTopoDS</i> package describes the persistent data model associated with any BRep shape; it is the persistent version of any shape of type <i> TopoDS_Shape</i>. As is the case for persistent geometric models, this data structure is never edited or queried, it is simply stored in or retrieved from the database. It is created or converted by the <i>MgtBRep::Translate</i> function.

The <i> MgtBRepAbs</i> and <i> PTColStd </i> packages provide tools used by the conversion functions of topological objects.

~~~~
//Create a shape
TopoDS_Shape aShape;

//Create a persistent shape
PtColStd_DoubleTransientPersistentMap aMap;

Handle(PTopoDS_HShape) aPShape =
	aMap.Bind2(MgtBRep::Translate
		aShape,aMap,MgtBRepAbs_WithTriangle));

aPShape.Nullify();

//Restore a transient shape
Handle(PTopoDS_HShape) aPShape;

Handle(TopoDS_HShape) aShape =
	aMap.Bind1(MgtBRep::Translate
		(aPShape,aMap,MgtBRepAbs_WithTriangle));

aShape.Nullify();
~~~~

@subsubsection OCCT_TOVW_SECTION_7_3_5 Standard Documents

Standard documents offer you a ready-to-use document containing a TDF-based data
structure. The documents themselves are contained in a class inheriting from <i> TDocStd_Application</i>
which manages creation, storage and retrieval of documents.

You can implement undo and redo in your document, and refer from the data framework
of one document to that of another one. This is done by means of external link attributes,
which store the path and the entry of external links. To sum up, standard documents
alone provide access to the data framework. They also allow you to:
*Update external links;
*Manage the saving and opening of data;
*Manage undo/redo functionality.

@section OCCT_TOVW_SECTION_8 FAQ

@subsection OCCT_TOVW_SECTION_8_1 Memory Management

In a work-session, geometry modeling applications create and delete a certain number
of C++ objects. In this context, memory allocation and de-allocation standard functions
are not suited to the system's requirements and for this reason a specialized Memory
Manager is implemented into Open CASCADE Technology. The Memory Manager is based
on the following principles:

  * small memory arrays are grouped into clusters and then recycled (clusters are never released to the system),
  * large arrays are allocated and de-allocated through the standard functions of the system (the arrays are released to system when they are no longer used).

### The Reference Counter

To lighten usual programming difficulties linked to the management of object life duration, before deleting an object, the user must ensure the object is no longer referenced and the delete function is secured by a reference counter. 
A smart-pointer called *Handle* automates reference counter management and automatically deletes an object when it is no longer referenced. The application never calls the delete operator explicitly. To benefit from the memory manager in OCCT, transient classes must inherit from <i>TShared</i>. The principle of allocation is as follows:

~~~~
    Handle (TColStd_HSequenceOfInteger) H1 = new TColStd_HSequenceOfInteger;
    // H1 has one reference and corresponds to 48 bytes of memory
    {
      Handle (TColStd_HSequenceOfInteger) H2;
      H2 = H1; // H1 has two references
      if (argc == 3) 
      {
        Handle (TColStd_HSequenceOfInteger) H3;
        H3 = H1;
        // Here, H1 has three references
      }
         // Here, H1 has two references
      }
      // Here, H1 has 1 reference
    }
    // Here, H1 has no reference but the 48 bytes of memory are kept.
    Handle (TColStd_HSequenceOfInteger) H1 = new TColStd_HSequenceOfInteger;
    // Here, H1 has one reference and corresponds to the preceding 48 bytes of 
    // memory. In this case, there is no allocation of memory.
~~~~

### Cycles

As cycles are objects which reference one another, memory management is impossible if the data structure contains any cycles, particularly if there are back references.

For example, objects in a graph include primitives and each one of these primitives has to know the graphic object to which it belongs (i.e. a reference to this graphic object). With normal references, the classical handle is used. With back references, a pointer is used. 

### Memory Consumption


As a general rule, it is advisable to allocate memory through significant blocks. 
In this way, the user can work with blocks of contiguous data and it facilitates memory page manager processing.

@subsection OCCT_TOVW_SECTION_8_2 How to define a handled object without CDL

You can create a class manipulated by handle even if you do not use CDL (Open CASCADE Definition Language). 
To do that you have to use the <i>Define_Standard_Handle</i> macro which is defined in the include file <i> Standard_DefineHandle.hxx</i>.

Here is an example which shows how to define a class <i> SamplePoint </i> manipulated by handle.

Sample_Point.hxx:
---------------

~~~~

    #ifndef _Sample_Point_HeaderFile
    #define _Sample_Point_HeaderFile
    #ifndef _Standard_Macro_HeaderFile
    #include <Standard_Macro.hxx>
    #endif
    #include <MMgt_TShared.hxx>
    #include <Standard_DefineHandle.hxx>
    // Handle definition
    //

    DEFINE_STANDARD_HANDLE(Sample_Point,MMgt_TShared)
    class Sample_Point: public MMgt_TShared {
    public:
    Sample_Point();
    Sample_Point(const Standard_Real, const
    Standard_Real);
    void SetX(const Standard_Real x) {
    myX = x;
    }
    void SetY(const Standard_Real y) {
    myY = y;
    }
    Standard_Real X() const {
    return myX;
    }
    Standard_Real Y() const {
    return myY;
    }
    // some methods like DynamicType() or
    IsKind()
    //
    DEFINE_STANDARD_RTTI(Sample_Point)
    private:
    Standard_Real myX;
    Standard_Real myY;
    };
    #endif

~~~~

Sample_Point.cxx:
----------------

~~~~

    #include <Sample_Point.hxx>

    // Implementation of Handle and type mgt

    IMPLEMENT_STANDARD_HANDLE(Sample_Point,MMgt_TShared)
    IMPLEMENT_STANDARD_RTTI(Sample_Point,MMgt_TShared)

    // For ancestors, we add a IMPLEMENT_STANDARD_SUPERTYPE and
    // a IMPLEMENT_STANDARD_SUPERTYPE_ARRAY_ENTRY  macro.
    // We must respect the order: from the direct ancestor class to the base class.

    IMPLEMENT_STANDARD_TYPE(Sample_Point)
    IMPLEMENT_STANDARD_SUPERTYPE(MMgt_TShared)
    IMPLEMENT_STANDARD_SUPERTYPE(Standard_Transient)
    IMPLEMENT_STANDARD_SUPERTYPE_ARRAY()
    IMPLEMENT_STANDARD_SUPERTYPE_ARRAY_ENTRY(MMgt_TShared)
    IMPLEMENT_STANDARD_SUPERTYPE_ARRAY_ENTRY(Standard_Transient)
    IMPLEMENT_STANDARD_SUPERTYPE_ARRAY_END()
    IMPLEMENT_STANDARD_TYPE_END(Sample_Point)

   // Constructors implementation

    Sample_Point::Sample_Point(const
    Standard_Real x, const Standard_Real y)
    {
    myX = x;
    myY = y;
    }
    Sample_Point::Sample_Point()
    {
    myX = 0.0;
    myY = 0.0;
    }
~~~~

@subsection OCCT_TOVW_SECTION_8_3 When is it necessary to use a handle?

When designing an object, the user is faced with the choice of manipulating that
object by value, or by handle.

  * If your object may have a long lifetime within the application and you want to make multiple references to it, it would be preferable to manipulate this object with a handle. The memory for the object will be allocated on the heap. The handle which points to that memory is a light object which can be rapidly passed in argument. This avoids the penalty of copying a large object. 
  * If your object will have a limited lifetime, for example, used within a single algorithm, it would be preferable to manipulate this object by value, non-regarding its size, because this object is allocated on the stack and the allocation and de-allocation of memory is extremely rapid, which avoids the implicit calls to 'new' and 'delete' occasioned by allocation on the heap.
  * Finally, if an object will be created only once during, but will exist throughout the lifetime of the application, the best choice may be a class manipulated by handle or a value declared as a global variable. 


@subsection OCCT_TOVW_SECTION_8_4  How to cast shape handle to void

You can easily cast a reference to the handle object to <i> void* </i> by defining the following:

~~~~
    void *pointer;
    Handle(Some_class) aHandle;
    // Here only a pointer will be copied
    Pointer = &aHandle;
    // Here the Handle object will be copied
    aHandle = * (Handle(Some_Class) *)pointer;
~~~~

@subsection OCCT_TOVW_SECTION_8_5 How to test correct ending of OCCT algorithms

Generally OCCT algorithms implement <i> IsDone</i> method, which  returns <i> true</i> 
if computation has been performed successfully from beginning to end or <i> false</i> if computation has failed.

When <i> IsDone</i> returns <i> true</i>, the computation is successful regarding
to the input data, but it does not necessary mean that you get a result. For example, if
you perform a cut algorithm between two shapes without any common part, the <i> IsDone</i>
method will return <i> true</i>, but the result will be empty.

So, in some cases, it can be necessary to analyse the structure of a result before
using it again in following computations. These tests are not done systematically
into algorithms to get faster computations. The application performs necessary tests
depending on the context.

@subsection OCCT_TOVW_SECTION_8_6 How to cut, copy and paste inside a document

To cut, copy and paste inside a document, you must use the <i> CopyLabel</i> class from the <i> TDF</i> package.
In fact, you must define a Label which contains the temporary value a cut or 
copy operation (say, in <i> Lab_Clipboard</i>). You must also define two other labels:

* One containing the data (e.g. <i> Lab_source</i>)
* One for the destination of the copy (e.g. <i> Lab_ Target</i> )

~~~~
    Copy = copy (Lab_Source => Lab_Clipboard)
    Cut = copy + Lab_Source.ForgetAll() // command clear the contents of LabelSource.
    Paste = copy (Lab_Clipboard => Lab_target)
~~~~

So we need a tool to copy all (or a part) of the content of a label and its sub-label,
to another place defined by a label.

~~~~
    TDF_CopyLabel aCopy;
    TDF_IDFilter aFilter (Standard_False);

    //Don't copy TDataStd_TreeNode attribute

     aFilter.Ignore(TDataStd_TreeNode::GetDefaultTreeID());
     aCopy.Load(aSource, aTarget); aCopy.UseFilter(aFilter); aCopy.Perform();

    // copy the data structure to clipboard 

    return aCopy.IsDone(); }
~~~~

The filter is used to forbid copying a specified type of attribute. 
You can also have a look at *TDF_Closure**, 
which can be useful to determine the dependencies of the part you want to cut from the document.

@subsection OCCT_TOVW_SECTION_8_7 Platform-related problems 

### Dynamic library loading 

Open CASCADE Technology uses a dynamic library loading mode. Sometimes, the error message such as the following appears:

~~~~
 "cannot map libname.so .. under any of the filenames .."
~~~~

When this happens, check your *PATH* under Windows, *LD_LIBRARY_PATH* under UNIX ,
*SHLIB_PATH* under HP-UX or *LIBPATH* under IBM AIX . 
It should contain the path where the required dynamic library is located.

### Running Draw under Windows


When running <i> DRAWEXE</i> and using axo in the Command window you may see the "Invalid command name "axo" " message :

Make sure that the OCCT directory name does not contain any blank spaces.
It causes some problems when reading the OCCT description TCL Commands files.
If you have set <i> DRAWHOME</i> and <i> DRAWDEFAULT</i>, replace \\ by / in the variable. 

### Error on application start on Windows

If Windows shows an error message with the text *Application failed to initialize properly* 
upon launching the application, check access rights for all libraries used in the application, in particular, third-party libraries. 

Make sure that you have all rights  necessary to access these libraries. 
It is recommended to use option *Inherit access rights from parent*.

### Problems with 3D viewer

If the 3D viewer fails to display the scene properly, or works very slowly, or exhibits
another problem, make sure to have the latest version of the graphics card driver
installed. If this is not possible or does not help, try to decrease 
hardware acceleration level (usually found in Troubleshooting section of the graphics card properties).
