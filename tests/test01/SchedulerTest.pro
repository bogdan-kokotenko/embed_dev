#*******************************************************************************
#   Filename:       SheduleTest.pro
#
#   Description:    Unit tests for custom tasklet scheduler
#
#   Author:         Bogdan Kokotenko
#
#   Revision date:  17/07/2016
#
#*******************************************************************************
TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle qt

INCLUDEPATH +=  $$PWD/config \
                $$PWD/../ \
                $$PWD/../../common \
                $$PWD/../../common/hal \
                $$PWD/../../common/hal/mcu/mingw \
                $$PWD/../../common/sys \
                $$PWD/../../common/sys/pt

HEADERS +=  $$PWD/project.h \
            $$PWD/config/clocks_config.h \
            $$PWD/config/hal_config.h \
            $$PWD/config/timers_config.h \
            $$PWD/config/stimer_config.h \
            $$PWD/config/devtime_config.h

SOURCES +=  main.cpp \
            $$PWD/../../common/sys/task.c \
            $$PWD/../../common/sys/stimer.c \
            $$PWD/../../common/sys/devtime.c \
            $$PWD/../../common/hal/mcu/mingw/hal.c \
            $$PWD/../../common/hal/mcu/mingw/clocks.c \
            $$PWD/../../common/hal/mcu/mingw/timers.c \
            $$PWD/../../common/hal/mcu/mingw/timer.c

# Google C++ Testing Framework
DEFINES += UNIT_TEST
include($$PWD/../../common/googletest/googletest.pri)

#*******************************************************************************
#   End of file
#*******************************************************************************
