#!/bin/bash

cd "$(dirname "$(readlink -f "${0}")")"

target_name="vkcube"
int_dir="libs/x86_64"
out_dir="../../../../Binary/x64/Release"

# glslang
rm -rf cube.vert.inc
rm -rf cube.frag.inc
../../glibc-glslang/bin64/glslangValidator -V cube.vert -x -o cube.vert.inc
../../glibc-glslang/bin64/glslangValidator -V cube.frag -x -o cube.frag.inc

# build by ndk
# rm -rf obj/local/x86_64
# rm -rf libs/x86_64
ndk-build APP_DEBUG:=false APP_ABI:=x86_64 NDK_PROJECT_PATH:=null NDK_OUT:=obj NDK_LIBS_OUT:=libs NDK_APPLICATION_MK:=Application.mk APP_BUILD_SCRIPT:=LinuxX11.mk 

# before execute change the rpath to \$ORIGIN  
chrpath -r '$ORIGIN' ${int_dir}/${target_name}

# mkdir the out dir if necessary
mkdir -p ${out_dir}

# copy the striped so to out dir
rm -rf ${out_dir}/${target_name}
cp -f ${int_dir}/${target_name} ${out_dir}/

# copy the dep libs to out dir  
cp -f ../../Bionic-Redistributable/lib64/libc.so ${out_dir}/
cp -f ../../Bionic-Redistributable/lib64/libdl.so ${out_dir}/
cp -f ../../Bionic-Redistributable/lib64/libm.so ${out_dir}/
cp -f ../../Bionic-Redistributable/lib64/libstdc++.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib64/libc++_shared.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib64/libvulkan.so ${out_dir}/  
if [ 0 -eq 1 ]; then #Intel
mkdir -p ${out_dir}/vulkan/icd.d/
cp -f ../../Bionic-Redistributable/lib64/vulkan/icd.d/intel_icd.x86_64.json ${out_dir}/vulkan/icd.d/  
cp -f ../../Bionic-Redistributable/lib64/libvulkan_intel.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib64/libdrm.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib64/libxcb-dri3.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib64/libxcb-randr.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib64/libz.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib64/libexpat.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib64/libxcb.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib64/libX11-xcb.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib64/libxcb-present.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib64/libxcb-sync.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib64/libxshmfence.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib64/libXau.so ${out_dir}/  
else #AMD
mkdir -p ${out_dir}/vulkan/icd.d/
cp -f ../../Bionic-Redistributable/lib64/vulkan/icd.d/radeon_icd.x86_64.json ${out_dir}/vulkan/icd.d/  
cp -f ../../Bionic-Redistributable/lib64/libvulkan_radeon.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib64/libLLVM-8.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib64/libdrm_amdgpu.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib64/libdrm.so ${out_dir} 
cp -f ../../Bionic-Redistributable/lib64/libelf.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib64/libxcb-dri3.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib64/libxcb-randr.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib64/libz.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib64/libexpat.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib64/libxcb.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib64/libX11-xcb.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib64/libxcb-present.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib64/libxcb-sync.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib64/libxshmfence.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib64/libXau.so ${out_dir}/  
mkdir -p ${out_dir}/libdrm/
cp -f ../../Bionic-Redistributable/lib64/libdrm/amdgpu.ids ${out_dir}/libdrm/amdgpu.ids  
fi
mkdir -p ${out_dir}/vulkan/explicit_layer.d/
cp -f ../../Bionic-Redistributable/lib64/vulkan/explicit_layer.d/VkLayer_core_validation.json ${out_dir}/vulkan/explicit_layer.d/  
cp -f ../../Bionic-Redistributable/lib64/vulkan/explicit_layer.d/VkLayer_object_tracker.json ${out_dir}/vulkan/explicit_layer.d/  
cp -f ../../Bionic-Redistributable/lib64/vulkan/explicit_layer.d/VkLayer_parameter_validation.json ${out_dir}/vulkan/explicit_layer.d/  
cp -f ../../Bionic-Redistributable/lib64/vulkan/explicit_layer.d/VkLayer_threading.json ${out_dir}/vulkan/explicit_layer.d/  
cp -f ../../Bionic-Redistributable/lib64/vulkan/explicit_layer.d/VkLayer_unique_objects.json ${out_dir}/vulkan/explicit_layer.d/  
cp -f ../../Bionic-Redistributable/lib64/vulkan/explicit_layer.d/VkLayer_standard_validation.json ${out_dir}/vulkan/explicit_layer.d/  
cp -f ../../Bionic-Redistributable/lib64/libVkLayer_core_validation.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib64/libVkLayer_object_tracker.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib64/libVkLayer_parameter_validation.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib64/libVkLayer_threading.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib64/libVkLayer_unique_objects.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib64/libVkLayer_utils.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib64/libSPIRV-Tools-shared.so ${out_dir}/  

# place the linker at cwd   
cp -f ../../Bionic-Redistributable/lib64/linker ${out_dir}/
cd ${out_dir}
  
# execute the generated a.out  
./${target_name} # --validate