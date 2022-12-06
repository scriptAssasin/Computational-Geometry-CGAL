ADD_LIBRARY(LibsModule 
    polygonizationHelper.cpp
    incrementalAlgorithm.cpp
    convexHullAlgorithm.cpp
    localSearch.cpp
    simulatedAnnealing.cpp
    antColony.cpp
)

target_link_libraries(LibsModule ${CGAL_LIBS})
ADD_EXECUTABLE(to_polygon main.cpp)
target_link_libraries(to_polygon LibsModule)