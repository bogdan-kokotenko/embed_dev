#*******************************************************************************
#   Filename:       googletest.pri
#
#   Description:    Contains includes for Google C++ Testing Framework
#
#   Author:         Bogdan Kokotenko
#
#   Revision date:  22/07/2016
#
#*******************************************************************************

DEFINES += UNIT_TEST

INCLUDEPATH +=  $$PWD/googletest \
                $$PWD/googletest/include \
                $$PWD/googlemock \
                $$PWD/googlemock/include

DEPENDPATH +=   $$PWD/googletest/include \
                $$PWD/googlemock/include

SOURCES +=  $$PWD/googletest/src/gtest-all.cc \
            $$PWD/googlemock/src/gmock-all.cc
            
#win32:CONFIG(release, debug|release): {
#    LIBS += -L$$PWD/googletest/lib/ -lgtest \
#            -L$$PWD/googlemock/lib/ -lgmock
#}
#else:win32:CONFIG(debug, debug|release):{
#    LIBS += -L$$PWD/googletest/lib/ -lgtestd \
#            -L$$PWD/googlemock/lib/ -lgmockd
#}            
            
#*******************************************************************************
#   End of file
#*******************************************************************************
