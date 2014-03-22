project(chatserverfinal)

add_executable(chatserverfinal Connection.cpp main.cpp)

install(TARGETS chatserverfinal RUNTIME DESTINATION bin)
