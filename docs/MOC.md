## Basic Idea

The following discussion is based on the "less than" depth function to simplify the description.  

There are three outcomes of the "coarse depth test": fail(z_tri_min > z_tile_max), pass(z_tri_max < z_tile_min) and ambiguous(neither fail or pass). And only in "ambiguous" case will the depth be **fully** tested. Evidently, our aim is to decrease the "ambiguous" cases to make the "depth test" more efficient.  

The main idea of "1\.\[Andersson 2015\]" is to use the "selection mask" to split the tile into two layers. And the "coarse depth test" is performed on the "layer" rather than the whole "tile". This means that the "z_tile_max/z_tile_min" should be replaced by the "z_i_max/z_i_min". Since the "layer" is smaller than the "tile", some "ambiguous" cases may be turned into the "fail"/"pass" cases.  
In my opinion, the "layer" can be merely considered as a smaller tile of which the shape can be changed by the "selection mask".  

1\.\[Andersson 2015\] [Magnus Andersson, Jon Hasselgren, Tomas Akenine-Moller. "Masked Depth Culling for Graphics Hardware." SIGGRAPH 2015.](https://fileadmin.cs.lth.se/graphics/research/papers/2015/ZMM/)  
