cmake_minimum_required(VERSION 2.6)


#Configuration du projet

project(GoldRush)

set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})



#Inclusion de la bibliothèque Hello

#include_directories(../include)

#link_directories(../lib/${CMAKE_BUILD_TYPE})



#Configuration de l'exécutable

file(

        GLOB_RECURSE

        source_files

        src/*

)

add_executable(

        GoldRush

        ${source_files}

)


#Configuration de l'édition de liens

#target_link_libraries(
#
#        test_hello
#
#        hello
#
#)