[Building Android](https://source.android.com/setup/build/building)  

source build/envsetup.sh  

lunch aosp_x86_64-user  

cd bionic/linker  
mma -j6  

cd bionic/libc  
mma -j6  

