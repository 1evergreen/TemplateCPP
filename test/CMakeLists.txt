# for each "test/x.cpp", generate target "x"
find_package(GTest CONFIG REQUIRED)
file(GLOB_RECURSE all_tests *.cpp)
foreach(test_file ${all_tests})
    get_filename_component(test_name ${test_file} NAME_WE)
    #EXCLUDE_FROM_ALL 如果存在，那 CMake 默认构建的时候就不会构建这个 Target。
    add_executable(${test_name} ${test_file}) 
    target_link_libraries(
        ${test_name} PRIVATE GTest::gtest_main
    )

endforeach()

		
# 手动列出每个测试文件
# set(test_files test_file1.cpp test_file2.cpp test_file3.cpp)

# foreach(test_file ${test_files})
#     get_filename_component(test_name ${test_file} NAME_WE)  # 获取文件名作为测试名称

#     add_executable(${test_name} ${test_file})
#     target_link_libraries(${test_name} PRIVATE GTest::gtest_main)
# endforeach()