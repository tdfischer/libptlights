set(ptlights_SOURCES
  ptlights.c
)

set(ptlights_HEADERS
  ptlights.h
)

add_library(ptlights SHARED ${ptlights_SOURCES})

install(TARGETS ptlights DESTINATION lib)
install(FILES ${ptlights_HEADERS} DESTINATION include)
