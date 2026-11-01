# The HAL is a library you link against. It does not depend on your application.

# Pick a backend. Set to "sim" to run with no robot attached.
set(HAL_BACKEND "real" CACHE STRING "HAL backend: real or sim")

set(HAL_SRCS
    "src/hal_selftest.c"
)

if(HAL_BACKEND STREQUAL "sim")
    list(APPEND HAL_SRCS "src/hal_sim.c")
else()
    list(APPEND HAL_SRCS "src/hal_real.c")
endif()

idf_component_register(
    SRCS ${HAL_SRCS}
    INCLUDE_DIRS "include"
    REQUIRES driver esp_timer esp_adc soc
)
