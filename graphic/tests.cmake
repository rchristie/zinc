
SET(CURRENT_TEST graphic)
LIST(APPEND API_TESTS ${CURRENT_TEST})
SET(${CURRENT_TEST}_SRC
    ${CURRENT_TEST}/iso_surface.cpp
    ${CURRENT_TEST}/graphic.cpp
    )

