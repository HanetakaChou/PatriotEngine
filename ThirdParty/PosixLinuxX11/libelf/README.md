[Building Android](https://source.android.com/setup/build/building)  

source build/envsetup.sh  

lunch aosp_x86_64-user  

cd external/elfutils/libelf  

BUILD_HOST_STATIC_LIBRARY ->  ## BUILD_HOST_STATIC_LIBRARY ##remove  

LOCAL_STATIC_LIBRARIES := libz -> LOCAL_SHARED_LIBRARIES:= libz  

BUILD_STATIC_LIBRARY ->  BUILD_SHARED_LIBRARY

++  
LOCAL_LDFLAGS += -Wl,--enable-new-dtags  
LOCAL_LDFLAGS += -Wl,-rpath,/XXXXXX  

mma -j6  

chrpath -r \'\$ORIGIN\' out/target/product/generic_x86_64/system/lib/libelf.so  
chrpath -r \'\$ORIGIN\' out/target/product/generic_x86_64/system/lib64/libelf.so  