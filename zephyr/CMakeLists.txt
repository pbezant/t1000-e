cmake_minimum_required(VERSION 3.13.1)
include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(t1000-e)

FILE(GLOB app_sources ../src/*.c*)
target_sources(app PRIVATE ${app_sources})

# Add out of tree drivers
list(APPEND ZEPHYR_EXTRA_MODULES
    ../drivers/sensor/qma6100p
    )
