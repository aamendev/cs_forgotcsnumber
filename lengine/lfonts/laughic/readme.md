# Laughic

Some graphics algorithms to draw circles, ellipses, rectangles, lines, triangles (with barry-centeric coordinates) and applying textures.

## showcase

### General
Simple drawing functions that enable basic shapes and simple alpha blending.
![showcase img](./imgs/showcase)

### Fractals
Some builtin fractal functions.
![showcase2 img](./imgs/fractal_showcase)
![showcase2 img2](./imgs/julia_n079_p01889)

### Ray Tracing
Basic raytracing supporting reflections, specular and diffuse material and shadow casting.
This one is very generic and can easily be built on by further brdfs structs and light types provided that they follow the provided contracts.
![showcase raytrace](./imgs/raytracer/new_light)

### Image Processing
Some builtin image processing abilities.
#### Blur
![showcase blur](./imgs/filters/binomial_2)
#### Sobel
![showcase sobel](./imgs/filters/sobel1)
#### Canny
![showcase Cany](./imgs/filters/canny)
#### Canny + Sharpen
![showcase Sharpen](./imgs/filters/sharpen)
#### Harris (corner Detection)
![showcase harris](./imgs/filters/harris)
#### Line Floyd Steinberg (line hatching)
![showcase LFS](./imgs/npr/line_hatch)

#### Colour Quantization
![showcase quantize](./imgs/processing/quantized)

#### GreyScale
![showcase grey](./imgs/processing/grey_scale)

#### Morphological
##### dilation
![showcase dilate](./imgs/filters/dilate)
##### erosion
![showcase erode](./imgs/filters/erode)

#### Colour Manipulation
Colors can be manipulated through numerous ways (e.g. inverting colours, clamping a certain channel to min or max values, or filtering regions within certain ranges of any colour channel)
##### colour inversion
![showcase invertColours](./imgs/processing/invert)
##### clamping min blue value
![showcase invertColours](./imgs/processing/changeMinBlue)
##### recontrasting
![showcase col](./imgs/processing/recontr)
##### increment red in all regions
![showcase col](./imgs/processing/reddishStuff)
##### remove red regions
![showcase col](./imgs/processing/redFilter)

### Path and Style
Ability to draw curves (path) while applying arbitrary modifications alongside it (style). A basic building block for future npr.
In the pic below, the first curve was modified to add some wobbles (errors) in the curve over a certain region (the bottom most curve has these random errors all over the curve).
The others have some colour and width manipulation along the path (e.g. starting transparent and become more solid along the path, or getting thicker or getting yellower)
There is also the possibility of subdividing a curve to create patterns (e.g. the second curve)
![showcase ps](./imgs/npr/curve2)
Here is a smiple example combining wiggle (to face outline) and width modifications (to internal features)
![showcase ps](./imgs/npr/ps1)
