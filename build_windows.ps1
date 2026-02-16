New-Item -ItemType Directory -Force -Path "build"
Set-Location build
cmake -G"Visual Studio 17 2022" ${COMMON_CMAKE_CONFIG_PARAMS} ../
cmake --build . --config Debug

Set-Location ..
