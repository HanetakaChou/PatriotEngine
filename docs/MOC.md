## HiZ  

The Z buffer is splitted into serveral tiles and the HiZ(Hierarchical Z) buffer can be considered as an outline of a tile inside the Z buffer.  

The HiZ buffer consists of a "z_tile_nearest" value and a "z_tile_farthest" value. And the "z_tile_nearest" is exactly the nearest Z value of the corresponding tile while the "z_tile_farthest" may be equal or farther than the farthest Z value of the corresponding tile due to the fact that there is currently no efficient method to keep the "z_tile_farthest" equal to the farthest Z value of the tile.   

The aim of the HiZ buffer is to skip the full z buffer test to improve the performance and there are three potential outcomes of the HiZ test: fail("z_tile_farthest" nearer than "z_tri_nearest"), pass("z_tri_farthest" nearer than "z_tile_nearest") and ambiguous(neither fail or pass). It should be noted that the "fail" outcome is conservative since the "z_tile_farthest" may be farther than the farthest Z value of the corresponding tile. Evidently, both "fail" and "pass" outcomes skip the full z buffer test and the ambiguous outcome should be decreased to improve the performance.

And the "z_tile_nearest" can be efficiently updated as "z_tile_nearest = nearest(z_tri_nearest, z_tile_nearest)". However, the "z_tile_farthest" can only be efficiently updated as "z_tile_farthest = nearest(z_tri_farthest, z_tile_farthest)" when the outcome is "pass" since the "z_tri_farthest" and "z_tile_farthest" may be not at the same sample of the Z buffer. And the slower "delayed feedback update", which is to be removed by 1\.\[Andersson 2015\], is currently required to update the "z_tile_farthest".  

The main idea of "1\.\[Andersson 2015\]" is to use the "tile_mask"("selection mask") to split the tile into two layers. Each layer has its own "z_layer_farthest" and the "z_tile_farthest" is replaced. And the HiZ test is performed based on the layer rather than the tile.  

Evidently, for a single sample, there is no "ambiguous" outcome, only "pass" or "fail". A tile is considered as "ambiguous" only when it contains both the "pass" and "fail" samples. In my opinion, since the size of the layer is smaller than the tile, the potential that the layer contains both the "pass" and "fail" samples decreases and thus the "ambiguous" outcome is decreased.

Besides, the "tile_mask" can be used to make sure that the "z_tri_farthest" and "z_layer_farthest" are at the same sample of the Z buffer, and thus the "z_layer_farthest" can be efficiently updated as "z_layer_farthest = nearest(z_tri_nearest, z_layer_farthest)" when the coverage mask of the triangle fully covers the mask of the layer.  And in other cases, the "z_layer_farthest" is conservatively updated as "z_layer_farthest = farthest(z_tri_nearest, z_layer_farthest)".

And the underlying assumption of the "two" layers is that the same layer is expected to represent the same surface. For example, the Z buffer may represent a "roof" before the "sky". And ideally, the "tile_mask" should make sure that one layer represents the "roof" and the other layer represents the "sky".  

## MOC  
TODO

1\.\[Andersson 2015\] [Magnus Andersson, Jon Hasselgren, Tomas Akenine-Moller. "Masked Depth Culling for Graphics Hardware." SIGGRAPH 2015.](https://fileadmin.cs.lth.se/graphics/research/papers/2015/ZMM/)  
