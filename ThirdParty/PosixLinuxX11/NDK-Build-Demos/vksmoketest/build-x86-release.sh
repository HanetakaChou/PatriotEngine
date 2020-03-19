#!/bin/bash

cd "$(dirname "$(readlink -f "${0}")")"

target_name="vksmoketest"
int_dir="libs/x86"

#glslang
rm -rf Smoke.push_constant.vert.inc
rm -rf Smoke.vert.inc
rm -rf Smoke.frag.inc
../../glibc-glslang/bin/glslangValidator -V Smoke.push_constant.vert -x -o Smoke.push_constant.vert.inc
../../glibc-glslang/bin/glslangValidator -V Smoke.vert -x -o Smoke.vert.inc
../../glibc-glslang/bin/glslangValidator -V Smoke.frag -x -o Smoke.frag.inc

# build by ndk
rm -rf obj/local/x86
rm -rf libs/x86
ndk-build APP_DEBUG:=false APP_ABI:=x86 NDK_PROJECT_PATH:=null NDK_OUT:=obj NDK_LIBS_OUT:=libs NDK_APPLICATION_MK:=Application.mk APP_BUILD_SCRIPT:=LinuxX11.mk 

# before execute change the rpath to \$ORIGIN  
chrpath -r '$ORIGIN' ${int_dir}/${target_name}

  
# copy the dep libs to out dir  
cp -f ../../Bionic-Redistributable/lib/libc.so ${int_dir}
cp -f ../../Bionic-Redistributable/lib/libdl.so ${int_dir}
cp -f ../../Bionic-Redistributable/lib/libm.so ${int_dir}
cp -f ../../Bionic-Redistributable/lib/libstdc++.so ${int_dir}  
cp -f ../../Bionic-Redistributable/lib/libvulkan.so ${int_dir}  
mkdir -p ${int_dir}/vulkan/icd.d/
cp -f ../../Bionic-Redistributable/lib/vulkan/icd.d/intel_icd.i686.json ${int_dir}/vulkan/icd.d/  
cp -f ../../Bionic-Redistributable/lib/libvulkan_intel.so ${int_dir}  
cp -f ../../Bionic-Redistributable/lib/libdrm.so ${int_dir}  
cp -f ../../Bionic-Redistributable/lib/libxcb-dri3.so ${int_dir}  
cp -f ../../Bionic-Redistributable/lib/libxcb-randr.so ${int_dir}  
cp -f ../../Bionic-Redistributable/lib/libz.so ${int_dir}  
cp -f ../../Bionic-Redistributable/lib/libexpat.so ${int_dir}  
cp -f ../../Bionic-Redistributable/lib/libxcb.so ${int_dir}  
cp -f ../../Bionic-Redistributable/lib/libX11-xcb.so ${int_dir}  
cp -f ../../Bionic-Redistributable/lib/libxcb-present.so ${int_dir}  
cp -f ../../Bionic-Redistributable/lib/libxcb-sync.so ${int_dir}  
cp -f ../../Bionic-Redistributable/lib/libxshmfence.so ${int_dir}  
cp -f ../../Bionic-Redistributable/lib/libc++_shared.so ${int_dir}  
cp -f ../../Bionic-Redistributable/lib/libXau.so ${int_dir}  
mkdir -p ${int_dir}/vulkan/explicit_layer.d/
cp -f ../../Bionic-Redistributable/lib/vulkan/explicit_layer.d/VkLayer_core_validation.json ${int_dir}/vulkan/explicit_layer.d/  
cp -f ../../Bionic-Redistributable/lib/vulkan/explicit_layer.d/VkLayer_object_tracker.json ${int_dir}/vulkan/explicit_layer.d/  
cp -f ../../Bionic-Redistributable/lib/vulkan/explicit_layer.d/VkLayer_parameter_validation.json ${int_dir}/vulkan/explicit_layer.d/  
cp -f ../../Bionic-Redistributable/lib/vulkan/explicit_layer.d/VkLayer_threading.json ${int_dir}/vulkan/explicit_layer.d/  
cp -f ../../Bionic-Redistributable/lib/vulkan/explicit_layer.d/VkLayer_unique_objects.json ${int_dir}/vulkan/explicit_layer.d/  
cp -f ../../Bionic-Redistributable/lib/vulkan/explicit_layer.d/VkLayer_standard_validation.json ${int_dir}/vulkan/explicit_layer.d/  
cp -f ../../Bionic-Redistributable/lib/libVkLayer_core_validation.so ${int_dir}  
cp -f ../../Bionic-Redistributable/lib/libVkLayer_object_tracker.so ${int_dir}  
cp -f ../../Bionic-Redistributable/lib/libVkLayer_parameter_validation.so ${int_dir}  
cp -f ../../Bionic-Redistributable/lib/libVkLayer_threading.so ${int_dir}  
cp -f ../../Bionic-Redistributable/lib/libVkLayer_unique_objects.so ${int_dir}  
cp -f ../../Bionic-Redistributable/lib/libVkLayer_utils.so ${int_dir}  
cp -f ../../Bionic-Redistributable/lib/libSPIRV-Tools-shared.so ${int_dir}  

# place the linker at cwd   
cp -f ../../Bionic-Redistributable/lib/linker ${int_dir}
cd ${int_dir}
  
# execute the generated a.out  
./${target_name} --validate
