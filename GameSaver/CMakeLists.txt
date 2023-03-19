# CMakeList.txt : CMake project for GameSaver, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set(glm_DIR D:/Libraries/vcpkg/installed/x86-windows/share/glm) # if necessary

find_package(Vulkan REQUIRED)
find_package(glm CONFIG REQUIRED)

set(SOURCE 
    src/main.cpp
    src/gs_window.cpp
    src/gs_swap_chain.cpp
    src/gs_pipeline.cpp
    src/gs_model.cpp
    src/gs_device.cpp
    src/gs_app.cpp
)

set(HEADERS
    include/gs_window.h
    include/gs_swap_chain.h
    include/gs_pipeline.h
    include/gs_model.h
    include/gs_device.h
    include/gs_app.h
)

# Add source to this project's executable.
add_executable (${PROJECT_NAME} ${SOURCE} ${HEADERS})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)



target_include_directories(${PROJECT_NAME} PUBLIC D:/Libraries/glfw-3.3.8.bin.WIN64/include/GLFW)
target_link_directories(${PROJECT_NAME} PUBLIC D:/Libraries/glfw-3.3.8.bin.WIN64/lib-vc2022)



target_link_libraries(${PROJECT_NAME} 
    PRIVATE 
        Vulkan::Vulkan
        glm::glm
        D:/Libraries/glfw-3.3.8.bin.WIN64/lib-vc2022/glfw3.lib
)



if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
endif()


