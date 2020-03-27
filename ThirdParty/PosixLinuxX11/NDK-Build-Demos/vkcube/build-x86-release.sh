#!/bin/bash

cd "$(dirname "$(readlink -f "${0}")")"

target_name="vkcube"
int_dir="libs/x86"
out_dir="../../../../Binary/x86/Release"

# glslang
rm -rf generated/cube.vert.inc
rm -rf generated/cube.frag.inc
../../glibc-glslang/bin64/glslangValidator -V cube.vert -x -o generated/cube.vert.inc
../../glibc-glslang/bin64/glslangValidator -V cube.frag -x -o generated/cube.frag.inc

# include-bin
rm -rf generated/lunarg.ppm.h
../../glibc-include-bin/bin64/include-bin lunarg.ppm generated/lunarg.ppm.h

# build by ndk
# rm -rf obj/local/x86
# rm -rf libs/x86
ndk-build APP_DEBUG:=false APP_ABI:=x86 NDK_PROJECT_PATH:=null NDK_OUT:=obj NDK_LIBS_OUT:=libs NDK_APPLICATION_MK:=Application.mk APP_BUILD_SCRIPT:=LinuxX11.mk 

# before execute change the rpath to \$ORIGIN  
chrpath -r '$ORIGIN' ${int_dir}/${target_name}

# mkdir the out dir if necessary
mkdir -p ${out_dir}

# copy the striped so to out dir
rm -rf ${out_dir}/${target_name}
cp -f ${int_dir}/${target_name} ${out_dir}/

# copy the dep libs to out dir  
cp -f ../../Bionic-Redistributable/lib/libc.so ${out_dir}/
cp -f ../../Bionic-Redistributable/lib/libdl.so ${out_dir}/
cp -f ../../Bionic-Redistributable/lib/libm.so ${out_dir}/
cp -f ../../Bionic-Redistributable/lib64/libc++.so ${out_dir}/
cp -f ../../Bionic-Redistributable/lib/libstdc++.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib/libc++_shared.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib/libvulkan.so ${out_dir}/  
if [ 0 -eq 1 ]; then #Intel
mkdir -p ${out_dir}/vulkan/icd.d/
cp -f ../../Bionic-Redistributable/lib/vulkan/icd.d/intel_icd.i686.json ${out_dir}/vulkan/icd.d/  
cp -f ../../Bionic-Redistributable/lib/libvulkan_intel.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib/libdrm.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib/libxcb-dri3.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib/libxcb-randr.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib/libz.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib/libexpat.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib/libxcb.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib/libX11-xcb.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib/libxcb-present.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib/libxcb-sync.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib/libxshmfence.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib/libXau.so ${out_dir}/  
else #AMD
mkdir -p ${out_dir}/vulkan/icd.d/
cp -f ../../Bionic-Redistributable/lib/vulkan/icd.d/radeon_icd.i686.json ${out_dir}/vulkan/icd.d/  
cp -f ../../Bionic-Redistributable/lib/libvulkan_radeon.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib/libLLVM-8.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib/libdrm_amdgpu.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib/libdrm.so ${out_dir} 
cp -f ../../Bionic-Redistributable/lib/libelf.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib/libxcb-dri3.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib/libxcb-randr.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib/libz.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib/libexpat.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib/libxcb.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib/libX11-xcb.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib/libxcb-present.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib/libxcb-sync.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib/libxshmfence.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib/libXau.so ${out_dir}/  
mkdir -p ${out_dir}/libdrm/
cp -f ../../Bionic-Redistributable/lib/libdrm/amdgpu.ids ${out_dir}/libdrm/amdgpu.ids  
fi
mkdir -p ${out_dir}/vulkan/explicit_layer.d/
cp -f ../../Bionic-Redistributable/lib/vulkan/explicit_layer.d/VkLayer_core_validation.json ${out_dir}/vulkan/explicit_layer.d/  
cp -f ../../Bionic-Redistributable/lib/vulkan/explicit_layer.d/VkLayer_object_tracker.json ${out_dir}/vulkan/explicit_layer.d/  
cp -f ../../Bionic-Redistributable/lib/vulkan/explicit_layer.d/VkLayer_parameter_validation.json ${out_dir}/vulkan/explicit_layer.d/  
cp -f ../../Bionic-Redistributable/lib/vulkan/explicit_layer.d/VkLayer_threading.json ${out_dir}/vulkan/explicit_layer.d/  
cp -f ../../Bionic-Redistributable/lib/vulkan/explicit_layer.d/VkLayer_unique_objects.json ${out_dir}/vulkan/explicit_layer.d/  
cp -f ../../Bionic-Redistributable/lib/vulkan/explicit_layer.d/VkLayer_standard_validation.json ${out_dir}/vulkan/explicit_layer.d/  
cp -f ../../Bionic-Redistributable/lib/libVkLayer_core_validation.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib/libVkLayer_object_tracker.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib/libVkLayer_parameter_validation.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib/libVkLayer_threading.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib/libVkLayer_unique_objects.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib/libVkLayer_utils.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib/libSPIRV-Tools-shared.so ${out_dir}/  

# place the linker at cwd   
cp -f ../../Bionic-Redistributable/lib/linker ${out_dir}/
cd ${out_dir}
  
# execute the generated a.out  
./${target_name} # --validate