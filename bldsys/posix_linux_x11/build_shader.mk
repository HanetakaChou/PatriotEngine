all: pt_gfx_shader_mesh_vertex_vk.inl pt_gfx_shader_mesh_fragment_vk.inl

pt_gfx_shader_mesh_vertex_vk.inl : pt_gfx_shader_mesh_vertex_vk.glsl
	glslangValidator -S vert -V100 -o pt_gfx_shader_mesh_vertex_vk.inl -x pt_gfx_shader_mesh_vertex_vk.glsl
pt_gfx_shader_mesh_fragment_vk.inl : pt_gfx_shader_mesh_fragment_vk.glsl
	glslangValidator -S frag -V100 -o pt_gfx_shader_mesh_fragment_vk.inl -x pt_gfx_shader_mesh_fragment_vk.glsl


