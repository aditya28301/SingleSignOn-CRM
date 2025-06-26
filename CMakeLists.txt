cmake_minimum_required(VERSION 3.16)
project(Login_Page LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

# Find and include the required Qt modules
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Network
    NetworkAuth  # This enables 'auth'
    PrintSupport
    Charts
    Graphs
)

qt_standard_project_setup()

add_executable(Login_Page
    main.cpp
    login.cpp
    login.h
    oauthlogin.cpp
    oauthlogin.h
    search_customer.cpp
    search_customer.h
    customoauthhandler.h
    customoauthhandler.cpp
    CRM_Dashboard.h CRM_Dashboard.cpp
    qt.conf
    ordermanager.h ordermanager.cpp
    customer_search.h customer_search.cpp

    order.h order.cpp
    orderwidget.h orderwidget.cpp
    servercontrol.h servercontrol.cpp
    servercontrolwidget.h servercontrolwidget.cpp
)

target_link_libraries(Login_Page PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    Qt6::NetworkAuth
    Qt6::PrintSupport
    Qt6::Charts
    Qt6::Graphs
)
