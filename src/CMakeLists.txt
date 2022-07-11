aux_source_directory(. SRC_LIST)

add_library(mprpc ${SRC_LIST})

find_library(MY_LIB libzookeeper_mt.so.2 /usr/local/lib)
target_link_libraries(mprpc muduo_net muduo_base pthread ${MY_LIB})