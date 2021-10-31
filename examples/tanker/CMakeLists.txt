project(tanker)
add_executable(${PROJECT_NAME} main.cpp openglwindow.cpp tank.cpp bg.cpp bullet.cpp targets.cpp)
enable_abcg(${PROJECT_NAME})