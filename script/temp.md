In 3D, those boundaries are planes. The equations for the planes are \(x = x_0\), and \(x = x_1\). Where does the ray hit that plane? Recall that the ray can be thought of as just a function that given a \(t\) returns a location \(\mathbf{P}(t)\): 

$$ \mathbf{P}(t) = \mathbf{A} + t \mathbf{b} $$ 


That equation applies to all three of the x/y/z coordinates. For example, \(x(t) = A_x + t b_x\). This ray hits the plane \(x = x_0\) at the \(t\) that satisfies this equation: 

$$ x_0 = A_x + t_0 b_x $$ 


Thus \(t\) at that hitpoint is: 

$$ t_0 = \frac{x_0 - A_x}{b_x} $$ 


We get the similar expression for \(x_1\): 

$$ t_1 = \frac{x_1 - A_x}{b_x} $$ 


The key observation to turn that 1D math into a hit test is that for a hit, the \(t\)-intervals need to overlap. For example, in 2D the green and blue overlapping only happens if there is a hit: 
There are some caveats that make this less pretty than it first appears. First, suppose the ray is travelling in the negative \(x\) direction. The interval \((t_{x0}, t_{x1})\) as computed above might be reversed, e.g. something like \((7, 3)\). Second, the divide in there could give us infinities. And if the ray origin is on one of the slab boundaries, we can get a NaN. There are many ways these issues are dealt with in various ray tracers’ AABB. (There are also vectorization issues like SIMD which we will not discuss here. Ingo Wald’s papers are a great place to start if you want to go the extra mile in vectorization for speed.) For our purposes, this is unlikely to be a major bottleneck as long as we make it reasonably fast, so let’s go for simplest, which is often fastest anyway! First let’s look at computing the intervals: 

$$ t_{x0} = \frac{x_0 - A_x}{b_x} $$ $$ t_{x1} = \frac{x_1 - A_x}{b_x} $$ 


One troublesome thing is that perfectly valid rays will have \(b_x = 0\), causing division by zero. Some of those rays are inside the slab, and some are not. Also, the zero will have a ± sign under IEEE floating point. The good news for \(b_x = 0\) is that \(t_{x0}\) and \(t_{x1}\) will both be +∞ or both be -∞ if not between \(x_0\) and \(x_1\). So, using min and max should get us the right answers: 

$$ t_{x0} = \min( \frac{x_0 - A_x}{b_x}, \frac{x_1 - A_x}{b_x}) $$ 
$$ t_{x1} = \max( \frac{x_0 - A_x}{b_x}, \frac{x_1 - A_x}{b_x}) $$ 

The remaining troublesome case if we do that is if \(b_x = 0\) and either \(x_0 - A_x = 0\) or \(x_1 - A_x = 0\) so we get a NaN. In that case we can probably accept either hit or no hit answer, but we’ll revisit that later. 

Now, let’s look at that overlap function. Suppose we can assume the intervals are not reversed (so the first value is less than the second value in the interval) and we want to return true in that case. The boolean overlap that also computes the overlap interval \((f, F)\) of intervals \((d, D)\) and \((e, E)\) would be: 

If there are any NaNs running around there, the compare will return false so we need to be sure our bounding boxes have a little padding if we care about grazing cases (and we probably should because in a ray tracer all cases come up eventually). With all three dimensions in a loop, and passing in the interval \([t_{min}\), \(t_{max}]\), we get: 


#### Textrues
A texture in graphics usually means a function that makes the colors on a surface procedural. This procedure can be synthesis code, or it could be an image lookup, or a combination of both. We will first make all colors a texture. Most programs keep constant rgb colors and textures in different classes, so feel free to do something different, but I am a big believer in this architecture because being able to make any color a texture is great. 

We'll need to update the hit_record structure to store the U,V surface coordinates of the ray-object hit point. 

We will also need to compute \((u,v)\) texture coordinates for hittables. 

For spheres, this is usually based on some form of longitude and latitude, i.e., spherical coordinates. So if we have a \((\theta,\phi)\) in spherical coordinates, we just need to scale \(\theta\) and \(\phi\) to fractions. If \(\theta\) is the angle down from the pole, and \(\phi\) is the angle around the axis through the poles, the normalization to \([0,1]\) would be: 

$$ u = \frac{\phi}{2\pi} $$ $$ v = \frac{\theta}{\pi} $$ 


To compute \(\theta\) and \(\phi\), for a given hitpoint, the formula for spherical coordinates of a unit radius sphere on the origin is: 

$$ x = \cos(\phi) \cos(\theta) $$ $$ y = \sin(\phi) \cos(\theta) $$ $$ z = \sin(\theta) $$ 


We need to invert that. Because of the lovely <cmath> function atan2() which takes any number proportional to sine and cosine and returns the angle, we can pass in \(x\) and \(y\) (the \(\cos(\theta)\) cancel): 

$$ \phi = \text{atan2}(y, x) $$ 


The \(atan2\) returns values in the range \(-\pi\) to \(\pi\), so we need to take a little care there. The \(\theta\) is more straightforward: 

$$ \theta = \text{asin}(z) $$ 
which returns numbers in the range \(-\pi/2\) to \(\pi/2\). 


So for a sphere, the \((u,v)\) coord computation is accomplished by a utility function that expects things on the unit sphere centered at the origin. The call inside sphere::hit should be: 

### RECT
Now, let’s make some rectangles. Rectangles are often convenient for modeling man-made environments. I’m a fan of doing axis-aligned rectangles because they are easy. (We’ll get to instancing so we can rotate them later.) 

First, here is a rectangle in an xy plane. Such a plane is defined by its z value. For example, \(z = k\). An axis-aligned rectangle is defined by the lines \(x=x_0\), \(x=x_1\), \(y=y_0\), and \(y=y_1\). 



Figure 5: Ray-rectangle intersection




To determine whether a ray hits such a rectangle, we first determine where the ray hits the plane. Recall that a ray \(\mathbf{P}(t) = \mathbf{A} + t \mathbf{b}\) has its z component defined by \(P_z(t) = A_z + t b_z\). Rearranging those terms we can solve for what the t is where \(z=k\). 

$$ t = \frac{k-A_z}{b_z} $$ 


Once we have \(t\), we can plug that into the equations for \(x\) and \(y\): 

$$ x = A_x + t b_x $$ $$ y = A_y + t b_y $$ 

It is a hit if \(x_0 < x < x_1\) and \(y_0 < y < y_1\). 
Because our rectangles are axis-aligned, their bounding boxes will have an infinitely-thin side. This can be a problem when dividing them up with our axis-aligned bounding volume hierarchy. To counter this, all hittable objects should get a bounding box that has finite width along every dimension. For our rectangles, we'll just pad the box a bit on the infnitely-thin side. 

The actual xy_rect class is thus: 