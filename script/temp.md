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