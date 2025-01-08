board_runner_args(jlink "--device=stm32l151c8" "--speed=4000" "--reset-after-load")
board_runner_args(pyocd "--target=stm32l151c8")

include(${ZEPHYR_BASE}/boards/common/jlink.board.cmake)
include(${ZEPHYR_BASE}/boards/common/openocd.board.cmake)
include(${ZEPHYR_BASE}/boards/common/pyocd.board.cmake)
