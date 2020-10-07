PBR math (rasterization) {#specification__pbr_math}
========================
@tableofcontents

@section pbr_preface Preface

**Empirical** illumination models like **Phong reflection model** have been used in real-time graphics for a long time due to their simplicity, convincing look and affordable performance.
Before programmable pipeline has been introduced, graphics cards implemented Gouraud shading as part of fixed-function Transformation & Lighting (T&L) hardware blocks.
Nowadays, however, numerous trade-offs of this simplicity (like lighting partially baked into object material properties and others) pushed developers to **Physically-Based Rendering** (**PBR**) illumination models.

PBR models try to fit surface shading formulas into constrains of physical laws of light propagation / absorption / reflection - hence, called "physically-based".
There are two main categories of PBR illumination:

 1. Non-real-time renderer (cinematic).
 2. Real-time renderer.

The main objective of cinematic renderer is uncompromised quality, so that it relies on ray-tracing (path-tracing) rendering pipeline.
Although performance of current graphics hardware does not make it possible using computationally-intensive path-tracing renderer in real-time graphics, it can be used in interactive fashion.

"Physically-based" does not necessarily mean physically-correct/precise.
The main objective of real-time PBR renderer is to be fast enough even on low-end graphics hardware.
So that in contrast, it hardly relies on rasterization rendering pipeline, various approximations and tricks making it applicable in real-time, while looking good enough and preserving some physical properties.

OCCT 3D Viewer provides both kinds of PBR renderers, and although they share some details in common, this article is devoted to real-time PBR metallic-roughness illumination model.
This article describes the math underneath PBR shading in OCCT 3D Viewer and its GLSL programs.
However, this article does not clarifies related high-level APIs nor PBR material creation pipelines, as this is another topic.

@section pbr_notation Notation

|  |  |  |
|-:|:-|:-|
| \f$n\f$   | normal (on surface) | \f$\|n\|=1\f$ |
| \f$v\f$   | view direction      | \f$\|v\|=1\f$ |
| \f$l\f$   | light               | \f$\|l\| = 1\f$ |
| \f$h=\frac{v+l}{\|v + l\|}\f$   | half vector | |
| \f$m\f$   | metallic factor     | \f$[0, 1]\f$ |
| \f$r\f$   | roughness factor    | \f$[0, 1]\f$ |
| \f$IOR\f$ | index of refraction | \f$[1, 3]\f$ |
| \f$c\f$   | albedo color        | \f$(R, G, B)\f$ |

\f$\cos\theta_l=(n \cdot l)\f$

\f$\cos\theta_v=(n \cdot v)\f$

\f$\cos\theta_h=(n \cdot h)\f$

\f$\cos\theta_{vh}=(v \cdot h)\f$

@section pbr_illumination_model Illumination model

The main goal of illumination model is to calculate outgoing light radiance \f$L_o\f$ along the certain direction.
The starting point of calculation might be the view direction \f$v\f$ aimed from point on surface (or in more general case just in space) to viewer position.
Considering the point on opaque surface with normal \f$n\f$ the main equation of illumination can be defined as:

\f[L_o=\int\limits_H f(v, l) L_i(l) \cos\theta_l\, \mathrm{d}l\f]

Where \f$L_i(l)\f$ is light radiance coming from \f$l\f$ direction, \f$f(v,l)\f$ is **Bidirectional Reflectance Distribution Function** (**BRDF**) and \f$H\f$ is hemisphere which is oriented regarding to the surface normal \f$n\f$.
Opaqueness of the surface mentioned earlier is important because in that case hemisphere is enough.
More general model will require to consider directions all around a whole sphere and is not observed in this paper.
\f$\cos\theta_l\f$ factor appearing is caused by affection of surface area and light direction mutual orientation to the amount of radiance coming to this area.
This is mainly due to geometric laws. The rest part of integral is the key of the whole illumination model.
BRDF defines it's complexity and optical properties of material.
It has to model all light and material interactions and also has to satisfy some following criteria in order to be physical correct [@ref Duvenhage13]:
* Positivity: \f$f(v,l) \geq 0\f$
* Helmholtz reciprocity: \f$f(v,l) = f(l, v)\f$ (follows from 2<sup>nd</sup> Law of Thermodynamics)
* Energy conservation: \f$\displaystyle \forall v \, \int\limits_H f(v,l) \cos\theta_l \, \mathrm{d}l = 1\f$ (in order not to reflect more light than came)

It is worth to be mentioned that \f$f(v,l)\f$ depends on \f$n\f$ also but it is omitted to simplify notation. BRDF is usually split into two parts:

\f[f(v,l) = f_d(v,l)+f_s(v, l)\f]

Where \f$f_s(v, l)\f$ (specular BRDF) models reflection light interaction on surface and \f$f_d(v,l)\f$ (diffuse BRDF) models other processes happening depth in material (subsurface scattering for example).
So that illumination equation might be rewritten as:

\f[L_o=\int\limits_H (f_d(v,l)+f_s(v, l)) L_i(l) \cos\theta_l\, \mathrm{d}l\f]

PBR theory is based on **Cook-Torrance specular BRDF** [@ref Cook81]. It imagines surface as set of perfectly reflected micro faces distributed on area in different ways which is pretty good model approximation of real world materials.
If this area is small enough not to be able to recognize separate micro surfaces the results becomes a sort of averaging or mixing of every micro plane illumination contribution.
In that level it allows to work with micro faces in statistical manner manipulating only probabilities distributions of micro surfaces parameters such as normals, height, pattern, orientation etc.
In computer graphics pixels are units of images and it usually covers a relatively large areas of surfaces so that micro planes can be considered to be unrecognizable.
Going back to the BRDF the Cook-Torrance approach has the following expression:

\f[f_s(v,l)=\frac{DGF}{4\cos\theta_l\cos\theta_v}\f]

Three parts presented in nominator have its own meaning but can have different implementation with various levels of complexity and physical accuracy.
In that paper only one certain implementation is used. The \f$D\f$ component is responsible for **micro faces normals distribution**.
It is the main instrument that controls reflection's shape and strength according to **roughness** \f$r\f$ parameter.
The implementation with good visual results is **Trowbridge-Reitz GGX** approach used in Disney's RenderMan and Unreal Engine [@ref Karis13]:

\f[D=\frac{\alpha^2}{\pi(\cos^2\theta_h(\alpha^2-1) + 1)^2}\f]

Where \f$\alpha = r^2\f$. This square in needed only for smoother roughness parameter control.
Without it the visual appearance of surface becomes rough too quickly during the parameter's increasing.

The second \f$G\f$ component is called **geometric shadowing** or attenuation factor.
The point is that micro surfaces form kind of terrain and can cast shadows over each other especially on extreme viewing angles [@ref Heitz14].
**Smith-Schlick model** [@ref Heitz14], [@ref Schlick94] has been chosen as implementation:

\f[\displaystyle G=\frac{\cos\theta_l \cos\theta_v}{(\cos\theta_l(1-k)+k)(\cos\theta_v(1-k)+k)}\f]

Where \f$k=\frac{\alpha}{2}\f$, which means \f$k=\frac{r^2}{2}\f$ in terms of this paper.
But \f$G\f$ depends on many factors so that it's approximations has float nature and can be modified a little bit in some cases in order to get more pleasant visual results.
One of this modification will be described later in following chapters.

The last component \f$F\f$ shows **how much light is reflected from surface** and is called **Fresnel's factor**.
The rest amount of radiance might be absorbed or refracted by material.
The most accurate expression of it is pretty complicate for calculation so that there is a variety of approximations.
The good one with less computation efforts is **Schlick's implementation** [@ref Schlick94]:

\f[F=F_0+(1-F_0)(1-\cos\theta_{vh})^5\f]

Here \f$F_0\f$ is material's response coefficient at normal incidence (zero angle).
Fresnel's factor has to be calculated differently for metals and dielectric/non-metals, but PBR theory tries to come up with universal formula for all types of material.
In order to do that it is needed to be noticed that Schlick's approximation is applicable only to non-conductors and in that case \f$F_0 = F_{dielectric} = \left(\frac{1-IOR}{1+IOR}\right)^2\f$.
**Index of Refraction** \f$IOR\f$ shows the proportion between light speed in vacuum (or even in air) and in material.
The reference value of \f$IOR\f$ for plastic is **1.5**, and this value can be considered as default for all unknown dielectrics.
In practice this parameter controls reflectance ability of material.
Also it should be remembered that this approximation produces poor results with large \f$IOR\f$ values so that it is recommended to be kept in range of \f$[1, 3]\f$ in order to get plausible Fresnel's factor [@ref Lazanyi05], [@ref Lagarde13].
This formula might be further propagated onto metals by using \f$F_0\f$ measured specifically for certain metal.
It can be considered as some kind of a 'color' of metal and can be stored as albedo parameter \f$c\f$.
And the final step of defining Fresnel's factor formula is mixing all this \f$F_0\f$ using metallic parameter \f$m\f$ (**metalness**):

\f[F_0 = F_{dielectric}(1-m)+cm\f]

For pure dielectrics with \f$m=0\f$ exactly Schlick's approximation will be used.
For pure metals with \f$m=1\f$ it will be a little inaccurate but the same formula with measured \f$F_0\f$ values.
Everything else for \f$m \in (0, 1)\f$ is not physically correct and it is recommended to keep \f$m\f$ exactly 1 or 0.
Intermediate values may represent mixed areas for smooth transition between materials - like partially rusted metal (rust is mostly dielectric).
Also it might be useful when parameters are read from textures with filtering and smoothing.

BRDF described above has one important trait making computations easier called **isotropy**.
Isotropy in this case means independence from rotation about normal resulting from supposition of uniform micro faces distribution at any direction along a surface.
It allows to simplify random samples generation during Monte-Carlo integrals calculation and reduce dimensions of some lookup tables, which will be discussed in following chapters.
Of course, isotropic materials form only subset of all real world's materials, but this subset covers majority of cases.
There are special models considering special anisotropic traits of surfaces like a grinding of metal or other with dependency on rotation about normal;
these models require special calculation tricks and additional parameters and are out of scope of this paper.

The only thing left to do is to define \f$f_d(v,l)\f$.
This part is responsible for processes happening in depth of material.
First of all the amount of input light radiance participating in these processes is needed to be calculated.
And it exactly can be realized from already known Fresnel's factor \f$F\f$ showing amount of reflected light but in negative term in this case in order to get the radiance left after reflection:

\f[1-F\f]

This part of ingoing light is assumed to be refracted in depth of surface and variety of events may happen there.
A sequence of absorptions, reflections and reemissions more or less leads to light's subsurface scattering.
Some part of this scattered light can go back outside but in modified form and in pretty unpredictable directions and positions.
For opaque materials this part is noticeable and forms it's own color.
If subsurface's paths of light are small enough and points of output are distributed locally around the input point it's possible to work in statistical way similar to the micro faces.
This assumption covers a big amount of real world opaque materials.
Other materials like skin, milk etc. with noticeable effect of subsurface scattering usually presented in form of partial translucency and some kind of self emission
have more widely distributed output points and require more accurate and complicate ways of modeling with maybe some theory and techniques from volumetric rendering.
The simple but visually enough assuming for statistically driven type of materials is just the same radiance for any direction. It results to **Lambertian's BRDF**:

\f[\frac{c}{\pi}\f]

Where \f$\pi\f$ is normalization coefficient in order to meet BRDF's criteria and \f$c\f$ is material's own color formed by adventures of light under surface.
There is one detail about light interaction bringing some physicality to the model, and that is an absence of this diffuse component in metals.
Metals reflect main part of light and the rest of it is absorbed being transformed into other form (mostly heat).
That is the main visual difference between metallic and non-metallic materials realizing of which brings model to higher level of quality in compare to older non-physical models.

So that all parts described above can be combined into united diffuse BRDF:

\f[f_d(v,l) = (1-F)(1-m)\frac{c}{\pi}\f]

\f$m\f$ is recommended to be exactly 1 or 0 but all values between can represent transition areas, as mentioned before.

In this chapter one possible implementation of illumination model reflecting main PBR principles has been defined.
The next step is using of it in practice.

@section pbr_practical_application Practical application

It's time to apply deduced illumination model in practice.
And the first step of it is separation of **direction based light sources** from illumination integral.
Directional nature of such light sources means possibility to calculate it's influence to point of surface using only one direction and its intensity.
Usually sources of this type do not have physical size and are represented only by position in space (for point or spot lights) or by direction itself (direction light imagined to be too far point sources like sun).
This is just a kind of abstraction, while real world light emitters have noticeably sizes.
But sources with realistic form and size cannot be presented in discrete term and require continuous integrals calculations or special approximations in order to be accurately injected to the model.
In most cases direct based light sources in form of emitting points in space or just certain directions are good approximations and are enough for beginning.
Having finite discrete amount of it in scene and considering only single direction from every of these lights, the integral is transformed just to the sum:

\f[L_{direct} = \sum_{j=1}^M f(v, l_j) L_i^{direct}(l_j) \cos\theta_{l_j}\f]

Where \f$M\f$ is a number of sources, \f$l_j\f$ is a direction and \f$L_i^{direct}\f$ is an intensity related to this direction.
\f$direct\f$ label means that illumination has been computed directly from sources.
The BRDF can be used directly without any calculation problems.
The only exception might be \f$k\f$ in \f$G\f$ factor - it is recommended to use \f$ k = (r+1)^2 / 8 \f$ in order to get more pleasant results [@ref Karis13] (that is modification mentioned in previous chapter).
And actually it is enough to finally see something.
There will be correct visualization with assumption of complete dark environment and absence of other points influence.
It is called **local illumination**. Based on this name there is also a global or **indirect illumination** and that is the rest of integral:

\f[L_{indirect} = \int\limits_H f(v, l) L_i^{indirect}(l) \cos\theta_l\, \mathrm{d}l\f]

It includes influence of light reflected or scattered from other points and environment's contribution.
It's impossible to achieve photorealistic results without this component, but is is also very difficult to compute.
While the cross point light interaction cannot be calculated in a simple way (especially in real time rendering), the environment illumination has some options to be realized via precomputational work before visualization.
But right now lets summarize the practical application of illumination model.
At this moment the output radiance is represented as:

\f[L_o = L_{direct} + L_{indirect}\f]

Where \f$L_{direct}\f$ is direction based light sources contribution which can be directly computed just applying bare formulas.
It is enough to get some results in terms of local illumination but without \f$L_{indirect}\f$ component image will not be looked lifelike.
\f$L_{indirect}\f$ is not trivial thing for calculation and that is stumbling block for real time rendering applications.
But it can be relatively easy implemented in case of environment illumination via some precomputational work about which will be told in details in following chapters.

@section pbr_image_based_lighting Image based lighting

The next goal after \f$L_{direct}\f$ calculation is to find \f$L_{indirect}\f$.
And it would be easier if \f$L_i^{indirect}(l)\f$ was known for every \f$l\f$.
That is the main assumption of **image based lightning** (**IBL**).
In practice, it can be achieved using environment image map, which is a special image representing illumination from all possible directions.
This image might be a photo capturing a real world environment (spherical 360 degrees panoramas) or generated image baking the 3D scene itself, including in that case reflections of other objects.
Environment image might be packed in different ways - **cube maps** and equirectangular maps are the most commonly used.
Anyway, it allows \f$L_i^{indirect}(l)\f$ to be defined for every \f$l\f$ and its practical implementation in form of images gives name for this approach.
Lets back to indirect illumination integral:

\f[L_{indirect} = \int\limits_H f(v, l) L_i^{indirect}(l) \cos\theta_l\, \mathrm{d}l\f]

Substituting the BRDF by its expression allows to split indirect illumination into diffuse and specular components:

\f[L_{indirect} = \int\limits_H f_d(v,l)L_i^{indirect}(l)\cos\theta_l\, \mathrm{d}l + \int\limits_H f_s(v,l)L_i^{indirect}(l)\cos\theta_l\, \mathrm{d}l = \f]

\f[= (1-m)\frac{c}{\pi}\int\limits_H (1-F)L_i^{indirect}(l)\cos\theta_l\, \mathrm{d}l + \frac{1}{4}\int\limits_H \frac{DFG}{\cos\theta_l \cos\theta_v}L_i^{indirect}(l)\cos\theta_l\, \mathrm{d}l\f]

This splitting seems not to lead to simplicity of calculation but these two parts will be computed in slightly different ways in future.
Lets write down this separately:

\f[L_{indirect}^d = (1-m)\frac{c}{\pi}\int\limits_H (1-F)L_i^{indirect}(l)\cos\theta_l\, \mathrm{d}l\f]

\f[L_{indirect}^s = \frac{1}{4}\int\limits_H \frac{DFG}{\cos\theta_v \cos\theta_l} L_i^{indirect}(l) \cos\theta_l\, \mathrm{d}l\f]

Next transformations of these expressions require understanding of numerical way to find hemisphere integral and also its performance optimization techniques.
And that the topic of the next chapter.

@section pbr_monte_carlo_integration Monte-Carlo numeric integration

**Monte-Carlo** is one of numeric methods to **find integral**.
It is based on idea of mathematical expectation calculation.
In one dimensional case if \f$f(x)\f$ is a function with parameter distributed according to probability density \f$p(x)\f$ the mathematical expectation of it can be found using following expression:

\f[E = \int\limits_{-\infty}^\infty f(x) p(x)\, \mathrm{d}x\f]

Also this expectation can be approximated in statistical term using certain sequence of random variable \f$x\f$:

\f[E \approx \frac{1}{N} \sum_{i=1}^{N} f(x_i)\f]

It can be used in general definite integrals calculations.
Just valid \f$p(x)\f$ defined on \f$[a, b]\f$ range and sequence \f$x_i\f$ generated according to it are needed for that:

\f[\int\limits_a^b f(x)\, \mathrm{d}x = \int\limits_a^b \frac{f(x)}{p(x)}p(x)\, \mathrm{d}x = \int\limits_{-\infty}^{\infty} \frac{f(x)}{p(x)}p(x)\, \mathrm{d}x \approx \frac{1}{N} \sum_{i=1}^{N} \frac{f(x_i)}{p(x_i)}\f]

Where \f$f(x)\f$ is considered to be equal to zero outside of \f$[a, b]\f$ range.
This is also true for functions on sphere or hemisphere:

\f[\int\limits_{H|S} f(l)\, \mathrm{d}l \approx \frac{1}{N}\sum_{i=1}^{N} \frac{f(l_i)}{p(l_i)}\f]

The main questions are choosing \f$p(l)\f$ and generating samples \f$l_i\f$.
The one of the simple ways is uniform distribution along sphere or hemisphere.
Lets realize that on sphere for example.
There are \f$4\pi\f$ possible directions in terms of sphere's areas and steradians (direction can be presented as dot on a unit sphere):

\f[\int\limits_S 1\, \mathrm{d}l = 4\pi\f]

Where \f$S\f$ is the unit sphere.
In order to be uniform \f$p(l)\f$ must be constant and satisfy normalization criteria:

\f[\int\limits_S p(l)\, \mathrm{d}l = 1\f]

So that \f$p(l) = \frac{1}{4\pi}\f$.
Usually direction \f$l\f$ is parametrized by spherical coordinates \f$\phi \in [0, 2\pi]\f$ and \f$\theta \in [0, \pi]\f$ boiling down to the 2D samples generation.
But in these terms joint \f$p(\theta, \phi)\f$ will be looked slightly different due to variables transition.
\f$l\f$ is defined in regular Cartesian coordinates \f$l=(x, y, z)\f$ with \f$\|l\| = 1\f$.
The spherical coordinates transform looks like:

\f[x = r\sin\theta\cos\phi,\, y = r\sin\theta\sin\phi,\, z = r\cos\theta\f]

Where \f$r = 1\f$.
In order to express probability density using new variables it is needed to multiply this density by Jacobian of transform:

\f[p(\theta,\phi) = p(l)|J_T|\f]

Where:

\f[|J_T| = \begin{vmatrix} \frac{\partial x}{\partial r} & \frac{\partial x}{\partial \theta} & \frac{\partial x}{\partial \phi} \\ \frac{\partial y}{\partial r} & \frac{\partial y}{\partial \theta} & \frac{\partial y}{\partial \phi} \\ \frac{\partial z}{\partial r} & \frac{\partial z}{\partial \theta} & \frac{\partial z}{\partial \phi} \end{vmatrix} = r^2\sin\theta\f]

So that joint probability density in new variables looks like:

\f[p(\theta, \phi) = \frac{\sin\theta}{4\pi}\f]

This variable transfer rule of **Probability Density Function** (**PDF**) will be useful in following chapters, when integral calculation optimization techniques will be being told.
Having \f$p(\theta, \phi)\f$ the partial single dimensional probability densities are able to be found:

\f[p(\phi) = \int\limits_0^\pi p(\theta, \phi)\, \mathrm{d}\theta = \frac{1}{4\pi} \int\limits_0^\pi \sin\theta\, \mathrm{d}\theta = \frac{1}{2\pi}\f]

\f[p(\theta) = \int\limits_0^{2\pi} p(\theta, \phi)\, \mathrm{d}\phi = \frac{\sin\theta}{4\pi}\int\limits_0^{2\pi}1\, \mathrm{d}\phi = \frac{\sin\theta}{2}\f]

The final step is sequence generation itself.
In order to be able to generate values with arbitrary distributions it is helpful to start from uniform numbers in range of \f$[0, 1]\f$.
And that can be done via any known true- and pseudo- random generators.
Even simple \f$\frac{1}{i}\f$ sequence is appropriate for beginning but it can be not so efficient in terms of computations convergence.
There are specially designed series for the last reason and it will be tackled in chapter about optimizations.
The \f$\phi\f$ variable is noticed to be uniformly distributed so that it can be directly generated without any additional manipulations.
Just range \f$[0, 1]\f$ is needed to be mapped to range \f$[0, 2\pi]\f$.
For any other variables including \f$\theta\f$ the inverse transform sampling approach can be applied.
First of all **cumulative distribution function** (**CDF**) is needed to be found.
It is probability of random value to be less than argument of this functions by definition.
For continues distributions it can be expressed in following form:

\f[F(x) = \int\limits_{-\infty}^x p(x')\, \mathrm{d}x'\f]

Lets find CDF for \f$\theta\f$:

\f[F(\theta) = \int\limits_{-\infty}^\theta p(\theta')\, \mathrm{d}\theta' = \int\limits_0^\theta \frac{\sin\theta'}{2}\, \mathrm{d}\theta' = \frac{1-\cos\theta}{2}\f]

The CDF maps \f$\theta\f$ values from range of \f$[0, \pi]\f$ to probability in range of \f$[0, 1]\f$.
The next step is to find inverse cumulative function which can be not so trivial sometimes but pretty obvious in current case:

\f[F^{-1}(u) = \arccos(1-2u)\f]

If substitute uniform distributed in range \f$[0, 1]\f$ values \f$u\f$ as argument of this function the values with origin probability density will appear.
In other words:

\f[\theta = \arccos(1 - 2u),\, u \in [0, 1],\, p(u) = 1 \Rightarrow p(\theta) = \frac{\sin\theta}{2}\f]

That is the key of this random values generation technique.
All steps described above can be also done for hemisphere:

\f[p(l) = \frac{1}{2\pi}\f]

\f[p(\theta, \phi) = \frac{\sin\theta}{2\pi}\f]

\f[p(\phi) = \int\limits_0^\frac{\pi}{2} p(\theta, \phi)\, \mathrm{d}\theta = \frac{1}{2\pi} \int\limits_0^\frac{\pi}{2} \sin\theta\, \mathrm{d}\theta = \frac{1}{2\pi}\f]

\f[p(\theta) = \int\limits_0^{2\pi} p(\theta, \phi)\, \mathrm{d}\phi = \frac{\sin\theta}{2\pi}\int\limits_0^{2\pi}1\, \mathrm{d}\phi = \sin\theta\f]

\f[\theta = \arccos(1-u)\f]

Mote-Carlo integration cannot guarantee exact estimation of convergence speed with using random generated samples.
There is only probability estimation of it.
But this algorithm is pretty universal and relatively simple to be applied to almost any function using at least uniform distributed points.
Moreover special \f$p(l)\f$ can be chosen and special pseudo-random sequences can be designed in order to speed up convergence for some functions (following chapter talk about that in details).
That is why this method is widely used in computer graphics and demonstrates good results.
Also another one advantage is worth to be mentioned - possibility to iteratively accumulate computations and present intermediate results during rendering which is used in some ray tracing applications.

@section pbr_split_sum Split sum

Lets go back to the image based lighting and the figure of specular component.
As was defined before that is hemisphere integral with following expression:

\f[L_{indirect}^s = \frac{1}{4}\int\limits_H \frac{DFG}{\cos\theta_v \cos\theta_l} L_i^{indirect}(l) \cos\theta_l\, \mathrm{d}l\f]

The Monte-Carlo integration algorithm can be directly applied:

\f[L_{indirect}^s = \int\limits_H f_s(v, l)L_i^{indirect}(l)\cos\theta_l\, \mathrm{d}l \approx \frac{1}{N}\sum_{i=1}^N \frac{f_s(v, l_i) L_i^{indirect}(l_i) \cos\theta_{l_i}}{p(v, l_i)}\f]

\f$p(v, l_i)\f$ depends on \f$v\f$ and implicitly on \f$r\f$ in order to be completely general.
Optimization strategies use different samples distributions for different view direction orientations and roughness values.
Anyway even with all optimization techniques this algorithm continues to require too much calculations.
Good visual results require noticeable number of samples and using this approach for every point in real time rendering becomes unrealistic.
The way to avoid these enormous calculations is doing them beforehand somehow.
The first trick on the way to this is split the sum separating environment light component [@ref Karis13]:

\f[L_{indirect}^s \approx \frac{1}{N} \sum_{i=1}^N \frac{f_s(v, l_i) L_i^{indirect}(l_i) \cos\theta_{l_i}}{p(v, l_i)} \approx \left( \frac{1}{N} \sum_{i=1}^N L_i^{indirect}(l_i) \right) \left( \frac{1}{N} \sum_{i=1}^N \frac{f_s(v, l_i) \cos\theta_{l_i}}{p(v, l_i)} \right)\f]

Where the second brackets represent approximation of integral so that the expression can be rewritten as:

\f[L_{indirect}^s \approx \frac{1}{N} \sum_{i=1}^N \frac{f_s(v, l_i) L_i^{indirect}(l_i) \cos\theta_{l_i}}{p(v, l_i)} \approx \left( \frac{1}{N} \sum_{i=1}^N L_i^{indirect}(l_i) \right) \int\limits_H f_s(v, l) \cos\theta_l\, \mathrm{d}l\f]

This integral is exact \f$L_{indirect}^s\f$ in condition when \f$L_i^{indirect}(l) = 1\f$ what just means white uniform environment.
The sum before it is kind of averaged environment illumination.
The main accomplishment after all this manipulations is possibility to calculate light and BRDF components separately.
The sum with \f$L_i^{indirect}(l_i)\f$ can be computed beforehand for every normal direction and stored to image called specular map but with some valuable details.
The problem is that \f$l_i\f$ samples must be generated according to \f$p(v, l_i)\f$ distribution depended on \f$v\f$ and \f$r\f$ as was mentioned earlier.
Variation of normal is not enough in that case and these variables are needed to be considered too.
The ways to resolve it are topic of one of the following chapters and now understanding the fact that at least this part can be precomputed before rendering is enough for now.
And it is important not to miss out that there is no more BRDF influence in this sum and only \f$p(v, l)\f$ can affect in this case.
That is why it is so important to strict to PDF during samples generation and that is why \f$p(v, l)\f$ must be correlated with BRDF somehow in this approximation approach with splitting.
For example completely mirroring materials with \f$r = 0\f$ will not be looked as expected if just uniform distribution is used
because such surfaces have only one possible direction from which light can be reflected along view direction in compare with \f$N\f$ absolutely scattered in case of uniform or many other distributions.

The rest part also can be saved to image. Lets unroll its expression:

\f[\int\limits_H f_s(v, l) \cos\theta_l\, \mathrm{d}l = \int\limits_H \frac{DGF}{4\cos\theta_v \cos\theta_l} \cos\theta_l\, \mathrm{d}l\f]

This integral is not actually a scalar.
That is RGB value due to only \f$F\f$ factor and even more only to \f$F_0\f$.
In order to simplify future computations \f$F_0\f$ is needed to be moved out of integral.
Substitution of Schlick's approximation helps to achieve it:

\f[F = F_0+(1-F_0)(1-\cos\theta_{vh})^5 = F_0(1-(1-\cos\theta_{vh})^5) + (1-\cos\theta_{vh})^5\f]

\f[\int\limits_H \frac{DGF}{\cos\theta_v \cos\theta_l} \cos\theta_l\, \mathrm{d}l = F_0 \int\limits_H \frac{DG}{4\cos\theta_v \cos\theta_l} (1-(1-\cos\theta_{vh})^5) \cos\theta_l\, \mathrm{d}l + \int\limits_H \frac{DG}{4\cos\theta_v \cos\theta_l} (1-\cos\theta_{vh})^5 \cos\theta_l\, \mathrm{d}l\f]

This form may not look easier, but it has several advantages.
The first one is independence from globally defined \f$L_i^{indirect}(l)\f$, so that normal orientation does not matter and can be set in any handful way for calculations (Z axis for example).
The second one results from isotropic illumination system allowing \f$\phi\f$ component of view vector to be set arbitrarily (0 for example) and \f$\cos\theta_v\f$ will be enough to define view direction.
And the third one is scalar nature of integrals so that only two precomputed numbers are needed to find BRDF part of \f$L_{indirect}^s\f$.
Considering dependency of these integrals from \f$\cos\theta_v\f$ and \f$r\f$ both of it can be precomputed and stored to 2D look-up image variating these two parameters in range \f$[0, 1]\f$ with two channels consisting of scale and bias for \f$F_0\f$.

Current result for \f$L_{indirect}^s\f$ is computing it using 2D image for BRDF part and omnidirectional image for environment illumination.
There were a lot of words about Monte-Carlo optimizations techniques and about PDF choice which is important not only in terms of numeric integration but in terms of visual results correctness.
It's time to talk about that.

@section pbr_importance_sampling Importance sampling

Current goal is to speed up Monte-Carlo integration of Cook-Torrance like integrals with following expression:

\f[\int\limits_H \frac{DG}{4\cos\theta_v \cos\theta_l} g(v, l) \cos\theta_l\, \mathrm{d}l\f]

Where \f$g(v, l)\f$ is just arbitrary function representing Fresnel's factor itself or its components.
In order to increase convergence the samples with larger contribution (or in other words with larger function's values) have to appear more frequently than others proportionally to its contribution.
So that less significant summand with less influence to result will be considered rarely and in opposite way parts brining noticeable changes to the sum will be taken often.
That is the main idea of **importance sampling technique**.
\f$p(l)\f$ has to represent significance of sample in terms of integrated function via probability somehow.
And it will be like that if PDF is already part of original function because in that case probability density directly affects to contribution forming.
Separating this distribution component is one possible and effective way to realize importance sampling strategy.
In integral presented above PDF part already exists and that is \f$D\f$ component.
But it is distribution of micro faces normals or ideally reflection direction or \f$h\f$ in other word and not light directions distribution which is needed in fact.
Anyway that is good starting point and lets generate \f$h\f$ vectors first.
\f$D\f$ has the following expression:

\f[D=\frac{\alpha^2}{\pi(\cos^2\theta_h(\alpha^2-1) + 1)^2}\f]

Frankly speaking \f$D(h)\f$ is called normal distribution but cannot be directly used as hemisphere distribution.
Originally it is statistical factor used to define total area of micro faces \f$\mathrm{d}A_h\f$
whose normals lie withing given infinitesimal solid angle \f$\mathrm{d}h\f$ centered on \f$h\f$ direction using the original small enough area of macro surface \f$\mathrm{d}A\f$ [@ref Walter07]:

\f[dA_h = D(h)\,\mathrm{d}h\, \mathrm{d}A\f]

First of all this factor must be positive:

\f[D(h) \geq 0\f]

But the total area of micro faces landscape is at least equal to origin surface but even bigger in general:

\f[1 \leq \int\limits_H D(h)\, \mathrm{d}h\f]

This trait does not allow to use \f$D\f$ as hemisphere distribution.
But it can be fixed with following feature:

\f[\forall v\, \int\limits_H D(h)(v \cdot h)\, \mathrm{d}h = (v \cdot n)\f]

Which means that total area of micro faces projected to any direction must be the same as projected area of origin macro surface.
It is pretty tricky trait in \f$D\f$ definition but it leads to interesting results in condition when \f$v = n\f$:

\f[\int\limits_H D(h)\cos\theta_h\, \mathrm{d}h = 1\f]

So that \f$\cos\theta_h\f$ coefficient normalizes normal distribution in terms of hemisphere and allows to use it as distribution.
Finally PDF of half vectors can be wrote:

\f[p(\theta_h, \phi_h) = D\cos\theta_h\sin\theta_h = \frac{\alpha^2 \cos\theta_h\sin\theta_h}{\pi(\cos^2\theta_h(\alpha^2-1) + 1)^2}\f]

\f$\sin\theta_h\f$ results from spherical coordinate system transfer which was described in Monte-Carlo integration chapter.
Lets strict to samples generation procedure and find partial probability densities:

\f[p(\phi_h) = \int\limits_0^\frac{\pi}{2} p(\theta_h, \phi_h)\, \mathrm{d}\theta_h = \int\limits_0^\frac{\pi}{2} \frac{\alpha^2 \cos\theta_h\sin\theta_h}{\pi(\cos^2\theta_h(\alpha^2-1) + 1)^2}\, \mathrm{d}\theta = \frac{1}{2\pi}\f]

\f[p(\theta_h) = \int\limits_0^{2\pi} p(\theta_h, \phi_h)\, \mathrm{d}\phi_h = \int\limits_0^{2\pi} \frac{\alpha^2 \cos\theta_h\sin\theta_h}{\pi(\cos^2\theta_h(\alpha^2-1) + 1)^2}\, \mathrm{d}\phi = \frac{2 \alpha^2 \cos\theta_h\sin\theta_h}{(\cos^2\theta_h(\alpha^2-1) + 1)^2}\f]

\f$p(\phi_h)\f$ is unnecessary to be calculated analytically.
The fact of independency from \f$\phi\f$ is enough to figure out that this coordinate is uniformly distributed.
Anyway the \f$F(\theta_h)\f$ is next step [@ref Cao15]:

\f[F(\theta_h) = \int\limits_0^{\theta_h} \frac{2 \alpha^2 \cos\theta'_h\sin\theta'_h}{(\cos^2\theta'_h(\alpha^2-1) + 1)^2}\, \mathrm{d}\theta'_h = \int\limits_{\theta_h}^0 \frac{2 \alpha^2}{(\cos^2\theta'_h(\alpha^2-1) + 1)^2}\, \mathrm{d}(\cos^2\theta'_h) = \frac{\alpha^2}{\alpha^2-1}\int\limits_0^{\theta_h} \mathrm{d}\frac{1}{\cos^2\theta'_h(\alpha^2-1)+1} =\f]

\f[ = \frac{\alpha^2}{\alpha^2-1} \left( \frac{1}{\cos^2\theta_h(\alpha^2-1) + 1} - \frac{1}{\alpha^2} \right) = \frac{\alpha^2}{\cos^2\theta_h(\alpha^2-1)^2+(\alpha^2-1)} - \frac{1}{\alpha^2-1}\f]

In order to apply inverse transform sampling the \f$F^{-1}(u)\f$ is needed to be found:

\f[F^{-1}(u) = \theta_h = \arccos\sqrt{\frac{1-u}{u(\alpha^2-1)+1}}\f]

So that there is no more obstacles to generate \f$h\f$.
But the main goal was \f$l\f$ direction.
In order to get it the view vector \f$v\f$ has to be reflected related to already found \f$h\f$:

\f[l = 2(v \cdot h)h - v\f]

That is practical side of light direction generation.
But the theoretical one is needed to be resolved to calculate sum.
It is time to find \f$p(l)\f$ using known \f$p(h)\f$.
First of all the fact that \f$l\f$ is just transformed \f$h\f$ is needed to be understood.
In that way the light direction's PDF has following expression:

\f[p(l) = p(h)|J_T|\f]

Where \f$|J_T|\f$ is Jacobian of reflection transformation.
Lets find it.
Right now \f$n\f$ is axis from which \f$\theta\f$ spherical coordinate is encountered.
The first step is setting \f$v\f$ as starting point of \f$\theta\f$ instead of \f$n\f$.
This is linear transform so that \f$|J_T| = 1\f$.
Next step is transfer to spherical coordinate with \f$|J_T| = \sin\theta_{vh}\f$.
Due to previous step \f$\theta_{vh}\f$ is used instead of \f$\theta_h\f$.
In this coordinate system reflecting of \f$v\f$ relative to \f$h\f$ is just doubling \f$\theta_{vh}\f$ and Jacobian of it is equal to \f$\frac{1}{2}\f$.
In series of transform the Jacobians are multiplied so that currently \f$|J_T| = \frac{1}{2}\sin\theta_{vh}\f$.
And the final step is inverse transform to Cartesian coordinate system with \f$|J_T| = (\sin\theta_{vl})^{-1} = (\sin2\theta_{vh})^{-1}\f$.
Combining this all together the following expression is obtained for reflection transform Jacobian [@ref Schutte18]:

\f[|J_T| = \frac{\sin\theta_{vh}}{2\sin2\theta_{vh}} = \frac{\sin\theta_{vh}}{4\sin\theta_{vh}\cos\theta_{vh}} = \frac{1}{4\cos\theta_{vh}}\f]

And finally \f$p(l)\f$ looks like:

\f[p(l) = p(h)|J_T| = \frac{D\cos\theta_h}{4\cos\theta_{vh}}\f]

Lets go back to the Monte-Carlo sum and insert found result to it:

\f[\int\limits_H \frac{DG}{4\cos\theta_v \cos\theta_l} g(v, l) \cos\theta_l\, \mathrm{d}l \approx \frac{1}{N} \sum_{i=1}^N \frac{DG\, g(v, l_i) \cos\theta_{l_i}}{4\cos\theta_v \cos\theta_{l_i}\, p(l_i)} = \frac{1}{N} \sum_{i=1}^N \frac{G\, g(v, l_i) \cos\theta_{l_i} \cos\theta_{vh_i}}{\cos\theta_v \cos\theta_{l_i} \cos\theta_{h_i}}\f]

Here \f$G\f$ component is recommended to be calculated with original \f$k=\frac{\alpha}{2} = \frac{r^2}{2}\f$ in order to get more plausible result.
Of course, all \f$\cos\f$ must be clamped to range \f$[0, 1]\f$ because integral is calculated on a hemisphere and all expressions are defined on it.
\f$\cos\theta_v \cos\theta_{l_i}\f$ in denominator can be reduced with exactly the same part in geometric attenuation factor in order to avoid additional zero division cases.

Summarizing importance sampling strategy described above the convergence of Monte-Carlo integration can be improved using special PDF correlated with integrated function.
In case of BRDF with normal distribution functions \f$D\f$ the PDF producing procedure is defined.
Practically half vector \f$h\f$ is generated first and \f$l\f$ is obtained from it by view vector \f$v\f$ reflecting.
Due to this transformation final form of probability density used in sum is quite different but also has defined algorithm of calculation.
For isotropic Cook-Torrance BRDF the \f$\cos\theta_v\f$ and roughness \f$r\f$ are enough to start generation so that all integrals of that kind can be precalculated in 2D look-up tables variating these two parameters.
The same samples generation procedure must be used in specular map baking described in next chapter.

@section pbr_specular_map Specular map

The situation with BRDF part of \f$L_{indirect}^s\f$ is clear now and \f$L_i^{indirect}(l)\f$ sum is left to be discussed.
That was called **specular map** and has following form:

\f[\frac{1}{N}\sum_{i=1}^N L_i^{indirect}(l_i)\f]

As was mentioned this sum must be calculated for every normal direction using the same samples generation principles as in numeric integration computation.
This principles require two scalar parameters \f$\cos\theta_v\f$ and \f$r\f$ but now \f$\phi\f$ really matters.
So that in fact the specular map has to be saved in 3D table consisting omnidirectional textures.
That is a big expense of computational and memory resources.
A couple of tricks helps to reduce dimensions.
First of all the \f$\cos\theta_v\f$ and \f$\phi\f$ can be just excluded.
In that way \f$v\f$ is considered to be equal to \f$n\f$.
Of course this approach produces an error and affects the final result.
It can be fixed more or less by \f$\cos\theta_{l_i}\f$ weighting [@ref Karis13]:

\f[\frac{1}{N} \sum_{i=1}^N L_i^{indirect}(l_i) \cos\theta_{l_i}\f]

It is not a complete solution but practice shows that it is enough to get plausible illumination with sacrificing of lengthy reflections at grazing angles which exist in fact if everything is honestly computed.
The problem is that for \f$v \neq n\f$ considering this sum to be defined related to \f$n\f$ became incorrect.
For example, for complete mirroring materials with \f$r = 0\f$ this sum must boil down to \f$L_i^{indirect}(v_r)\f$
but not to \f$L_i^{indirect}(n)\f$ where \f$v_r\f$ is just reflected \f$v\f$ or in other words \f$v_r = 2(v \cdot n)n - v\f$.
That it just mirroring reflection principle.
Assumption of \f$n = v\f$ also means that \f$n = v = v_r\f$.
In that way radiance map better to be considered as averaging of illumination coming from \f$v_r\f$.
So that it has become to be defined related to reflection direction which has to be calculated before map's fetching.

Anyway, there are just two dimensions in radiance look-up table remain.
The rest one with \f$r\f$ parameter cannot be reduced.
There is no other ways except just roughness variation but in order to simplify that computations can be done for several values and the rest ones lying between can be obtained from linear interpolation.
This is another source of visual artifacts but it also works good in practice and that is pretty common approach.
But it still requires noticeably amount of samples and that is for every pixel related to each \f$r\f$ value.
It can be appropriate for precomputations but still limits using dynamic environments in real time rendering or just even static environments but on weak devices such as mobile ones.
And there are several possible ways to improve radiance map baking performance.

The first one is using textures with smaller resolutions for larger roughnesses.
The point is that smaller \f$r\f$ values produce map saving more details from origin environment in opposite to larger ones representing lower frequency components and working as low pass filters in fact.
So less pixels in combination with linear interpolation is enough to store less detailed convolutions.
Moreover, this approach naturally works with textures levels of details in graphics API
so that every certain radiance map related to certain \f$r\f$ can be stored on its own mip level and be directly fetched with linear interpolation not only over one texture but over levels too.
As practice shows 6 levels are enough.

After reducing pixels count it is turn for samples number.
And again correlation with roughness can be noticed.
For example map for completely mirroring materials with \f$r = 0\f$ the same sample \f$l_i = v_r\f$ will be produced.
So that only one sample is enough in this case.
In opposite way directions for \f$r = 1\f$ will be scattered over almost whole hemisphere what requires as much samples as available.
The 'locality' of distribution is decreased during increasing roughness and it is possible to assume that samples number might to be proportional to this 'locality' keeping accuracy at the same level.
But how can 'locality' be interpreted in terms of probability distribution? One possible way is CDF meaning.
\f$F(\theta_h)\f$ has been already defined and by definition it shows the probability of random value \f$\theta_h\f$ to be less than argument of CDF.
In other words \f$F(\theta'_h) = u\f$ means that probability of \f$\theta_h\f$ to be in range of \f$[0, \theta'_h]\f$ is \f$u\f$.
The inverse task of range searching using given \f$u\f$ can be solved with help of \f$F^{-1}(u) = \theta'_h\f$.
If \f$u\f$ is close to 1 (exact 1 has no sense because in that case \f$\theta'_h = \max\theta_h = \frac{\pi}{2}\f$)
then \f$\theta'_h\f$ represents the range of the most probable or most frequently generated values and that can be interpreted as 'locality' of distribution.
After that if samples number of the worst case with \f$r = 1\f$ is set (\f$N(1) = \max N\f$) the other ones can be estimated using following formula:

\f[N(r) = N(1)\frac{\theta'_h(r)}{\frac{\pi}{2}} = N(1)\frac{2\theta'_h(r)}{\pi} = N(1)\frac{2F^{-1}(u)}{\pi} = N(1)\frac{2}{\pi}\arccos\sqrt{\frac{1-u}{u(\alpha^2-1)+1}}\f]

It is approximate expression representing only estimated general proportionality so that cases of \f$r = 0\f$ and \f$r = 1\f$ must be processed separately with \f$N(0) = 1\f$ and \f$N(1) = \max N\f$.
\f$u\f$ can be parameter of this optimization strategy controlling speed of samples reducing in order to balance performance and quality (\f$u = 1\f$ disables this optimization at all).
This pretty tricky technique allows reducing calculations for every pixels without sacrificing the quality.

In addition to optimizations mentioned before another one can be applied in order to help to reduce samples numbers as previous one.
Using less samples produces image noise due to discrete nature of Monte-Carlo approximation.
But it can be slightly smoothed using some prefiltration.
The idea is that for the directions with small PDF or in other words for rare directions the samples near of it is unlikely to be generated.
So that this direction represents the averaged illumination from relatively big area on hemisphere but approximate it by just a constant.
It wold be better to get from such direction already averaged over bigger area environment.
It can be achieved using mip levels of origin \f$L_i^{indirect}\f$ whose pixels of one level is exact 4 averaged pixels from previous one.
Also mip levels generation is build in most common graphic API so there are no problems with it.
But first of all the area covered by one sample is needed to be found.
And that can be done as [@ref Colbert07]:

\f[\Omega_s = \frac{1}{N\,p(l)} = \frac{4\cos\theta_{vh}}{ND\cos\theta_h}\f]

Circumstance of \f$v = v_r = n\f$ leads to \f$\cos\theta_{vh}\f$ and \f$\cos\theta_h\f$ reducing so expression becomes even simpler:

\f[\Omega_s =\frac{4}{ND}\f]

Of course all zero divisions must be avoided by clamping, for example.
After that the area covered by one pixel of environment map is calculated.
In case of a cube map it looks like:

\f[\Omega_p = \frac{4\pi}{6k^2}\f]

Where \f$k\f$ is size of one cube map side in pixels (sides are assumed to be quads).
Finally the mip level of origin environment map which is needed to be fetched for this certain sample is defined by following expression:

\f[lod = \frac{1}{2} \log_2\left(\frac{\Omega_s}{\Omega_p}\right)\f]

The mathematics connected with mip levels sizes lie behind it but this is out of scope of this paper.
In combination with previous optimization technique this approach allows \f$N(1)\f$ to be smaller keeping visual results good.

That is not all possible optimization tricks but at least these three significantly reduces compute efforts and brings radiance map calculation to weak devices or even to dynamic environments in real time but in reasonable limits.

In that way \f$L_{indirect}^s\f$ can be completely computed without any integral approximations.
Only 2D look-up table of BRDF part and mip mapped omnidirectional texture of irradiance map are needed.
The first one can be got even without any environment.
It was achieved using some rough approximations and assumptions but despite of that the visual result are still plausible and can be compared even with ray traced images.
In order to complete whole image based lighting the \f$L_{indirect}^d\f$ component is left to be discussed.

@section pbr_spherical_harmonics Spherical harmonics

Lets go back to diffuse indirect illumination component represented by following formula:

\f[L_{indirect}^d = (1-m)\frac{c}{\pi}\int\limits_H (1-F)L_i^{indirect}(l)\cos\theta_l\, \mathrm{d}l\f]

Of course, Monte-Carlo algorithm can be applied directly and hemisphere integral can be precalculated for every normal direction
but dependence from \f$v\f$ in Fresnel's factor does not allow to do it efficiently (every \f$v\f$ direction is needed to be considered again).
In order to resolve it modified version of Schlick's approximation has been created [[?](TODO)]:

\f[F \approx F_{ss}=F_0+(\max(1-r, F_0))(1-\cos\theta_v)^5\f]

It differs from origin one and loses accuracy a little bit but now there is no light direction inside
so that it can be considered as kind of screen space defined Fresnel's factor (\f$ss\f$ means exactly 'screen space') and can be removed from integral:

\f[L_{indirect}^d = (1-m)(1-F_{ss})\frac{c}{\pi} \int\limits_H L_i^{indirect}(l) \cos\theta_l\, \mathrm{d}l\f]

The resulted expression without \f$(1-m)\f$ and \f$(1-F_{ss})\f$ parts is pretty known entity called **irradiance**.
It can be precalculated using \f$\cos\theta_l\f$ as PDF for importance sampling (actually it is only option in this case excluding uniform distribution).
But even with that samples will be scattered almost over whole hemisphere.
As was discussed in previous chapter this case requires significant amount of samples in order to average illumination with appropriate quality.
Poor accuracy resulted from lack of summand can be noticed especially on high frequency environments having a lot of contrasting details.
It worth to be mentioned that irradiance is used not only in BRDF.
Omnidirectional diffuse illumination captured for certain point or even for several points uniformly or hierarchically distributed is base of some baking global illumination techniques.
There it is called a **light probe**. So that other way to compute and store irradiance maps was found resolving many mentioned problems.
The Fourier's decomposition analogue for spherical function allows to achieve this.
That would be easy to explain concept directly on example.
So lets start from \f$L_i^{indirect}(l)\f$.
It is spherical function because directions are just points on sphere.
The decomposition looks like:

\f[L_i^{indirect}(l) = \sum_{i = 0}^\infty \sum_{j=-i}^i L_i^j y_i^j(l)\f]

Where \f$y_i^j(l)\f$ are spherical functions forming orthonormalized basis called spherical harmonics and \f$L_i^j\f$ is decompositions coefficients.
Orthonormality means that dot product of two basis elements is equal to 1 if this is the same functions and is equal to zero otherwise.
Dot product on a sphere is defined as integral of functions multiplication. In other words:

\f[\int\limits_S y_i^j(l)\, y_{i'}^{j'}(l)\, \mathrm{d}l = \begin{cases} 1 & \quad i,j = i',j' \\ 0 & \quad \mathrm{otherwise}\end{cases}\f]

Function basis with such traits is known and is described by following formulas [@ref Guy18]:

\f[y_i^{j > 0}(\theta, \phi) = \sqrt{2}K_i^j\cos(j\phi)P_i^j(\cos\theta)\f]
\f[y_i^{j<0}(\theta, \phi) = \sqrt{2}K_i^j\sin(j\phi)P_i^{|j|}(\cos\theta)\f]
\f[y_i^0(\theta, \phi) = K_i^0P_i^0(\cos\theta)\f]

\f[K_i^j = \sqrt{\frac{(2i+1)(i-|j|)!}{4\pi(i+|j|)!}}\f]
\f[P_0^0(x) = 1\f]
\f[P_1^0(x) = x\f]
\f[P_i^i(x) = (-1)^i(2i-1)!!(1-x^2)^\frac{i}{2}\f]
\f[P_i^j(x) = \frac{(2i-1)xP_{i-1}^j(x) - (i + j - 1)P_{i-2}^j}{i - j}\f]

Here \f$K_i^j\f$ are normalization factors and \f$P_i^j\f$ are **Legendre's polynomials**.
Decomposition coefficients \f$L_i^j\f$ are dot product of origin function (\f$L_i^{indirect}(l)\f$ in current case) and corresponding basis element. It can be written down as:

\f[L_i^j = \int\limits_S L_i^{indirect}(l)\,y_i^j(l)\, \mathrm{d}l\f]

Fact that all calculation happen over a sphere but not over hemisphere right now is important not to be missed.
That was example of spherical function decomposition but not a solution for original task which looks like:

\f[\int\limits_H L_i^{indirect}(l) \cos\theta_l\, \mathrm{d}l\f]

First of all, lets transform this integral to be defined not over hemisphere but sphere:

\f[\int\limits_H L_i^{indirect}(l) \cos\theta_l\, \mathrm{d}l = \int\limits_S L_i^{indirect}(l)\overline{\cos}\theta_l\, \mathrm{d}l\f]

Where \f$\overline{\cos}\f$ is cosine clamped to zero which can be expressed as:

\f[\overline{\cos}\theta_l = \max(\cos\theta_l, 0)\f]

Resulted expression can be considered as convolution in terms of spherical functions where \f$L_i^{indirect}(l)\f$ is target and \f$\overline{\cos}\theta_l\f$ is core.
This integral may seem independent but in fact hemisphere is oriented related to \f$n\f$ therefore \f$\overline{\cos}\theta_l\f$ depends on it too and became a kind of 'oriented' version of cosine.
That is pretty tricky and explanation about meaning of convolution on sphere is out of scope of this paper.
Fact that this is convolution analogue related to \f$n\f$ is enough for now [@ref Aguilar17], [@ref Ramamoorthi01].
The goal of looking at integral from this angle is using of convolution's trait allowing to compute decomposition using just only coefficients of function and core.
\f$\overline{\cos}\theta_l\f$ is independent from \f$\phi_l\f$ and in case of such radial symmetric cores the resulting coefficients boil down to following formula:

\f[(L_i^{indirect}(l) \ast \overline{\cos}\theta_l)_i^j = \frac{1}{K_i^0}L_i^j\, c_i^0 = \sqrt{\frac{4\pi}{2i+1}}L_i^j\, c_i^0\f]

Where \f$c_i^0\f$ are spherical harmonics factors corresponding to \f$\overline{\cos}\theta\f$.
Therefore the final decomposition looks like:

\f[\int\limits_{H(n)} L_i^{indirect}(l) \cos\theta_l\, \mathrm{d}l = \int\limits_S L_i^{indirect}(l)\overline{\cos}\theta_l\, \mathrm{d}l = \sum_{i=0}^\infty \sum_{j = -i}^i \sqrt{\frac{4\pi}{2i+1}}L_i^j\, c_i^0\, y_i^j(n)\f]

\f$c_i^0\f$ is left to be found.
Due to independence from \f$\phi\f$ all \f$c_i^{j \neq 0} = 0\f$.
The rest ones are calculated by regular dot product with basis functions:

\f[c_i^0 = c_i = \int\limits_S y_i^0(l)\, \overline{\cos}\theta_l\, \mathrm{d}l = \int\limits_0^{2\pi} \mathrm{d}\phi \int\limits_0^\pi y_i^0(\theta, \phi)\, \overline{\cos}\theta \sin\theta\, \mathrm{d}\theta = \int\limits_0^{2\pi} \mathrm{d}\phi \int\limits_0^\frac{\pi}{2} y_i^0(\theta, \phi)\, \cos\theta\sin\theta\, \mathrm{d}\theta = \f]

\f[= 2\pi\int\limits_0^\frac{\pi}{2} y_i^0(\theta, \phi)\, \cos\theta\sin\theta\, \mathrm{d}\theta = 2\pi K_i^0\int\limits_0^\frac{\pi}{2} P_i^0(\cos\theta)\, \cos\theta\sin\theta\, \mathrm{d}\theta\f]

\f$\sin\theta\f$ appears due to transfer from integral over sphere to double integral where \f$\mathrm{d}l = \sin\theta\, \mathrm{d}\theta\, \mathrm{d}\phi\f$.
There is an analytical solution for this expressions:

\f[c_1 = \sqrt{\frac{\pi}{3}}\f]

\f[c_{odd} = 0 \quad c_{even} = 2\pi\sqrt{\frac{2i+1}{4\pi}}\frac{(-1)^{\frac{i}{2}-1}}{(i+2)(i-1)}\frac{i!}{2^i\left(\frac{i!}{2}\right)^2}\f]

Starting from about the third \f$c_i\f$ the coefficients become less and less valuable so that only couple of them is enough in order to approximate \f$\overline{\cos}\theta\f$ with appropriate accuracy.
The same principle is true for convolution too because its coefficients are multiplied by \f$c_i\f$.
So there is no need to use more than even three bands (\f$i = 0, 1, 2\f$) of basis functions.
Lets write down them all in Cartesian coordinate [@ref Ramamoorthi01]:

\f[y_0^0 = \frac{1}{2}\sqrt{\frac{1}{\pi}} = Y_0^0\f]

\f[y_1^{-1} = -\frac{1}{2}\sqrt{\frac{3}{\pi}}y = Y_1^{-1}y\f]
\f[y_1^0 = \frac{1}{2}\sqrt{\frac{3}{\pi}}z = Y_1^0z\f]
\f[y_1^1 = -\frac{1}{2}\sqrt{\frac{3}{\pi}}x = Y_1^1x\f]

\f[y_2^{-2} = \frac{1}{2}\sqrt{\frac{15}{\pi}}xy = Y_2^{-2}xy\f]
\f[y_2^{-1} = -\frac{1}{2}\sqrt{\frac{15}{\pi}}yz = Y_2^{-1}yz\f]
\f[y_2^0 = \frac{1}{4}\sqrt{\frac{5}{\pi}}(3z^2-1) = Y_2^0(3z^2-1)\f]
\f[y_2^1 = -\frac{1}{2}\sqrt{\frac{15}{\pi}}xz = Y_2^1xz\f]
\f[y_2^2 = \frac{1}{4}\sqrt{\frac{15}{\pi}}(x^2-y^2) = Y_2^2(x^2-y^2)\f]

All \f$Y_i^j\f$ are just constants so that it can be moved from integral during calculations and can be taken from precomputed table.
Other constants related to \f$c_i\f$ can be united and also be calculated beforehand:

\f[\hat{c}_i = \frac{1}{K_i^0}\, c_i = \sqrt{\frac{4\pi}{2i+1}}\, c_i\f]

Finally expression of irradiance map approximation can be defined:

\f[\int\limits_{H(n)} L_i^{indirect}(l) \cos\theta_l\, \mathrm{d}l \approx \sum_{i=0}^2 \sum_{j=-i}^i L_i^j\, \hat{c}_i\, y_i^j(n)\f]

Where \f$\hat{c}_i\f$ is precalculated constants \f$y_i^j(n)\f$ are pretty easy functions and only \f$L_i^j\f$ are needed to be precomputed.
Of course \f$L_i^j\f$ are integrals over even whole sphere but now there is only nine of it instead of one for every pixel of omnidirectional image.
Moreover, texture is not needed at all in that case and only 9 colors representing \f$L_i^j\f$ have to be saved.
The Monte-Carlo algorithm can be applied with just uniform samples distribution without importance sampling at all.
\f$Y_i^j\f$ are used twice: in \f$L_i^j\f$ calculations and in sum after that.
So there is sense to store only squares of it.
All tables with constants presented below [@ref Ramamoorthi01]:

| |
|-|
| \f$(Y_0^0)^2 \approx (0.282095)^2\f$ |
| \f$(Y_1^{-1})^2 = (Y_1^0)^2 = (Y_1^1)^2 \approx (0.488603)^2\f$ |
| \f$(Y_2^{-2})^2 = (Y_2^{-1})^2 = (Y_2^1)^2 \approx (1.092548)^2\f$ |
| \f$(Y_2^0)^2 \approx (0.315392)^2\f$ |
| \f$(Y_2^2)^2 \approx (0.546274)^2\f$ |

| | |
|-|-|
| \f$\hat{c}_0\f$ | \f$3.141593\f$ |
| \f$\hat{c}_1\f$ | \f$2.094395\f$ |
| \f$\hat{c}_2\f$ | \f$0.785398\f$ |

Summarizing all mathematics above spherical harmonics decomposition boils down irradiance map to only 9 values which is needed to be precalculated as integrals.
As practice shows this is very good approximation of diffuse indirect illumination component.

@section pbr_transparency Transparent materials

TODO

@section pbr_low_discrepancy Low discrepancy sequence

TODO

@section pbr_references References

<table cellpadding="4">
<tr><td valign="top">
@anchor Duvenhage13 **[Duvenhage13]**
</td><td>
Bernardt Duvenhage, Kadi Bouatouch, D.G. Kourie,
"Numerical Verification of Bidirectional Reflectance Distribution Functions for Physical Plausibility",
*Proceedings of the South African Institute for Computer Scientists and Information Technologists Conference*,
October 2013.
</td></tr>

<tr><td valign="top">
@anchor Cook81 **[Cook81]**
</td><td>
Robert Cook, Kenneth Torrance,
"A Refectance Model for Computer Graphics",
*SIGGRAPH '81: Proceedings of the 8th annual conference on Computer graphics and interactive techniques*,
August 1981, pp. 307-316.
</td></tr>

<tr><td valign="top">
@anchor Karis13 **[Karis13]**
</td><td>
Brian Karis, "Real Shading in Unreal Engine 4", *SIGGRAPH 2013 Presentation Notes*.
</td></tr>

<tr><td valign="top">
@anchor Heitz14 **[Heitz14]**
</td><td>
Eric Heitz, "Understanding the Masking-Shadowing Function in Microfacet-Based BRDFs",
*Journal of Computer Graphics Techniques*, Vol. 3, No. 2, 2014.
</td></tr>

<tr><td valign="top">
@anchor Schlick94 **[Schlick94]**
</td><td>
Christophe Schlick, "An inexpensive brdf model for physically-based rendering",
*Computer Graphics Forum 13*, 1994, pp. 233-246.
</td></tr>

<tr><td valign="top">
@anchor Lazanyi05 **[Lazanyi05]**
</td><td>
Istvan Lazanyi, Lazslo Szirmay-Kalos, "Fresnel term approximations for Metals", January 2005.
</td></tr>

<tr><td valign="top">
@anchor Lagarde13 **[Lagarde13]**
</td><td>
Sebastien Lagarde, "Memo on Fresnel equations",
*Blog post*: [https://seblagarde.wordpress.com/2013/04/29/memo-on-fresnel-equations/](https://seblagarde.wordpress.com/2013/04/29/memo-on-fresnel-equations/).
</td></tr>

<tr><td valign="top">
@anchor Walter07 **[Walter07]**
</td><td>
Bruce Walter, Stephen Marschner, Hongsong Li, Kenneth Torrance,
"Microfacet Models for Refraction through Rough Surfaces", *Proceedings of Eurographics Symposium on Rendering*, 2007.
</td></tr>

<tr><td valign="top">
@anchor Cao15 **[Cao15]**
</td><td>
Jiayin Cao, "Sampling microfacet BRDF", November 1, 2015,
*Blog post*: [https://agraphicsguy.wordpress.com/2015/11/01/sampling-microfacet-brdf/](https://agraphicsguy.wordpress.com/2015/11/01/sampling-microfacet-brdf/).
</td></tr>

<tr><td valign="top">
@anchor Schutte18 **[Schutte18]**
</td><td>
Joe Schutte, "Sampling techniques for GGX with Smith Masking-Shadowing: Part 1", March 7, 2018,
*Blog post*: [https://schuttejoe.github.io/post/ggximportancesamplingpart1/](https://schuttejoe.github.io/post/ggximportancesamplingpart1/).
</td></tr>

<tr><td valign="top">
@anchor Colbert07 **[Colbert07]**
</td><td>
Mark Colbert, Jaroslav Krivanek, "GPU-Based Importance Sampling", *NVIDIA GPU Gems 3*, Chapter 20, 2007.
</td></tr>

<tr><td valign="top">
@anchor Guy18 **[Guy18]**
</td><td>
Romain Guy, Mathias Agopian, "Physically Based Rendering in Filament", *Part of Google's Filament project documentation*:
[https://google.github.io/filament/](https://google.github.io/filament/Filament.md.html)
</td></tr>

<tr><td valign="top">
@anchor Aguilar17 **[Aguilar17]**
</td><td>
Orlando Aguilar, "Spherical Harmonics", *Blog post*:
[http://orlandoaguilar.github.io/sh/spherical/harmonics/irradiance/map/2017/02/12/SphericalHarmonics.html](http://orlandoaguilar.github.io/sh/spherical/harmonics/irradiance/map/2017/02/12/SphericalHarmonics.html)
</td></tr>

<tr><td valign="top">
@anchor Ramamoorthi01 **[Ramamoorthi01]**
</td><td>
Ravi Ramamoorthi, Pat Hanrahan, "An Efficient Representation for Irradiance Environment Maps",
*SIGGRAPH '01: Proceedings of the 28th annual conference on Computer graphics and interactive techniques*, August 2001, pp. 497-500
</td></tr>

</table>
