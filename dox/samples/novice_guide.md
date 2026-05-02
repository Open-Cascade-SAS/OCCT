Novice Guide {#samples__novice_guide}
=======

@tableofcontents

@note This guide refers to the **OCCTOverview** Qt sample (and other C++ project samples such as Qt, MFC, C#, WebGL, QML, Java/Android and iOS). These project samples are **no longer bundled with the OCCT repository** -- they are being repackaged into separate Git repositories. Until those land, treat the screenshots and instructions related to the OCCTOverview Qt application as illustrative; the @ref samples__draw_scripts "DRAW Tcl demos" under `resources/samples/tcl/` and the @ref occt__tutorial "OCCT Tutorial" remain available and are the recommended starting points.

@section diffs Modeling with OCCT: Key differences

Open CASCADE Technology (OCCT) is an object-oriented C++ framework designed for rapid production of sophisticated CAD/CAM/CAE applications.
In other words, it provides endless possibilities for raw 2D and 3D modeling in C++ environment.
Unlike end-user software, it is used by the application developers and therefore strongly differs from the most popular CAD/CAM/CAE software packages.
OCCT provides building blocks enough for modeling, editing, visualization, and data interoperability of 2D and 3D objects.

By using OCCT, users can create the objects of their desire (or edit already existing ones) using raw code commands.
It is a more complicated process than using GUI-based software, but it provides much more flexibility than even script-based manipulations that are available within existing CAD/CAM/CAE applications.
However, to fully grasp the possibilities of OCCT it is best for the user to have previous experience in C++ at least at a basic level.

@section basics Understanding the principles

If you don't have any programming skills, grasping the full magnitude of OCCT workflow is still an option.
The documentation for OCCT contains several entry points for new users.
It will not explain all OCCT classes but will help to comprehend the workflow and help start thinking in terms of Open CASCADE Technology.

The most basic workflow is described in the @ref occt__tutorial "OCCT Tutorial" - this is an excellent starting point for new users.
In this tutorial you will create a solid model step-by-step using different classes and methods.
Each step of the tutorial contains code snippets and images.

The basics involved in the modeling process are explained.
When the basics of OCCT are clear, the next logical step is to check out @ref samples "sample scripts and examples" and examine those that suit your needs.
For these, the @ref samples__draw_scripts "DRAW demo scripts" (located in the `resources/samples/tcl/` directory) are an excellent next step.
These Tcl-based scripts demonstrate a wide range of OCCT functionality -- from basic geometry and topology to advanced modeling algorithms, data exchange, and visualization.
They can be executed in the DRAW Test Harness and are the most immediate way to see OCCT in action.

Example C++ applications (using Qt, MFC, C#, WebGL, QML, Java/Android, and iOS) are available separately from the [OCCT organization's GitHub repositories](https://github.com/Open-Cascade-SAS).
These provide full project setups demonstrating how to integrate OCCT into various application frameworks.

The overall classes introduction may be found in the @ref occt_user_guides__foundation_classes "Foundation Classes" section of the documentation.
The @ref occt_user_guides "User Guides" contain in-depth descriptions of all major OCCT modules.

@section helps Additional assistance

There are several resources that may be of use for new users.
The first is the [Training & E-learning](https://dev.opencascade.org/resources/trainings) page, which lists available trainings and describes their specifics.

The second is the documentation itself -- if you have a clear understanding of what you seek, you will most likely find the required information in the User Guides.

Aside from the documentation, the [Reference manual](https://dev.opencascade.org/doc/refman/html/index.html) is available.
Use it to check class descriptions, dependencies and examples.
Additionally, there is a [Forum](https://dev.opencascade.org/forums) where you can contact the OCCT community and developers.
