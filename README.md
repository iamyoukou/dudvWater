# Water simulation based on dudv map

From puddles to ponds, we can see ripples everywhere in the real world.
Their change in the height can be ignored even we change our view point.
This method is suitable for simulating those ripples,
and it is used widely in video games (e.g. *Dark Souls*).

I implement this method based on the video of [ThinMatrix](https://www.youtube.com/watch?v=HusvGeEDU_U&list=PLRIWtICgwaX23jiqVByUs0bqhnalNTNZh) and the paper of [Rene Truelsen](http://image.diku.dk/projects/media/rene.truelsen.07.pdf).

# Dudv map

A [dudv map](http://wiki.polycount.com/wiki/DuDv_map) is a texture used to distort another texture.
`du` and `dv` are offsets applied to some texture coordinate `(u, v)`,
i.e. using `(u + du, v + dv)` to sample a texture instead of `(u, v)`.
`(du, dv)` pairs are recorded in a texture (or a lookup table).
As a result, the original texture is distorted to a certain pattern.

## Questions about generating dudv map

Many articles point out that the dudv map is the first order derivative of a normal map.
But unfortunately, none of them have explained why.

The only thing that I can understand is as following:

![myThought](./image/myThought.png)

When the original plane is (imaginarily) distorted to a curved surface,
the normal of a certain point changes.
Subsequently, the reflection of this point changes from `(u, v)` to `(u + du, v + dv)`.
The offset `(du, dv)` is what we want to calculate,
and it is related to the change of normal.

The first derivative of normal, i.e.`(d(nx)/dx, d(ny)/dy, d(nz)/dz)`,
represents the change of normal in space,
not the change of normal between the original plane and the distorted one.
This confuses me a lot,
and none of those articles have provided more details.

Personally, I think `(du, dv) <- (n1 - n2).xz * someScale` makes more sense.
`(n1 - n2)` represents the geometry difference between the original plane and the distorted one,
and `someScale` is a scalar to adjust this difference.

# Result

The result of using the first derivative of normal is good, though.

![output](output.gif)
