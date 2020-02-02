
- Load ndk: https://developer.android.com/ndk/downloads
- Navigate to the `${ndk_root}/sources/third_party/shaderc`. Run `../../../ndk-build NDK_PROJECT_PATH=. APP_BUILD_SCRIPT=Android.mk APP_STL:=c++_static APP_ABI=all libshaderc_combined -j16`
- AndroidStudio: `File` -> `New` -> `Import Sample...` -> `Vulkan API samples`
- In `local.properties` set path to ndk: `ndk.dir = ${ndk_root}`

References:
1. https://github.com/googlesamples/vulkan-basic-samples/
