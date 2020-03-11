[Building Android](https://source.android.com/setup/build/building)  

source build/envsetup.sh  

lunch aosp_x86_64-user  

cd external/expat  

BUILD_HOST_STATIC_LIBRARY ->  ## BUILD_HOST_STATIC_LIBRARY ##remove  

BUILD_STATIC_LIBRARY  ->  ## BUILD_STATIC_LIBRARY ##remove  

++  
LOCAL_LDFLAGS += -Wl,--enable-new-dtags  
LOCAL_LDFLAGS += -Wl,-rpath,/XXXXXX  

mma -j6  

chrpath -r \'\$ORIGIN\' out/target/product/generic_x86_64/system/lib/libexpat.so  
chrpath -r \'\$ORIGIN\' out/target/product/generic_x86_64/system/lib64/libexpat.so  