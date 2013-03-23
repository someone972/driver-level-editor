QT += opengl
QT += widgets
CONFIG += windows
CONFIG += qt
CONFIG += moc
Debug:CONFIG += console

Release:DESTDIR = ../DCI_nosync/bin/Release
Release:OBJECTS_DIR = ../DCI_nosync/bin/Release/obj
Release:MOC_DIR = ../DCI_nosync/bin/Release/moc
Release:RCC_DIR = ../DCI_nosync/bin/Release/rcc
Release:UI_DIR = ../DCI_nosync/bin/Release/ui

Debug:DESTDIR = ../DCI_nosync/bin/Debug
Debug:OBJECTS_DIR = ../DCI_nosync/bin/Debug/obj
Debug:MOC_DIR = ../DCI_nosync/bin/Debug/moc
Debug:RCC_DIR = ../DCI_nosync/bin/Debug/rcc
Debug:UI_DIR = ../DCI_nosync/bin/Debug/ui

Debug:DEFINES += DEBUG_ENABLED

TEMPLATE = app

INCLUDEPATH += \
    "$$(FREEIMAGE_INCLUDE_DIR)"

Debug:LIBS += \
	-L"$$(FREEIMAGE_LIB_DIR)" \
	-lFreeImage

Release:LIBS += \
	-L"$$(FREEIMAGE_LIB_DIR)" \
	-lFreeImage

QMAKE_CXXFLAGS += \
	-O2 \
    \
	-Wall

PRECOMPILED_HEADER =

HEADERS = \
    vector.hpp \
    INIParser.hpp \
    EventMgr.hpp \
    Log_Routines/debug_logger.hpp \
    Log_Routines/default_loggers.hpp \
    Driver_Routines/DriverLevels/chairs.hpp \
    Driver_Routines/DriverLevels/heightmaps.hpp \
    Driver_Routines/DriverLevels/lamps.hpp \
    Driver_Routines/DriverLevels/models.hpp \
    Driver_Routines/DriverLevels/RandomModelPlacement.hpp \
    Driver_Routines/DriverLevels/roads.hpp \
    Driver_Routines/DriverLevels/textures.hpp \
    Driver_Routines/DriverLevels/world.hpp \
    Driver_Routines/driver_levels.hpp \
    Driver_Routines/driver_d3d.hpp \
    Driver_Routines/driver_cos.hpp \
    Driver_Routines/driver_den.hpp \
    Driver_Routines/driver_wdf.hpp \
    Driver_Routines/ioFuncs.hpp \
    QtGUI/AboutDialog.hpp \
    QtGUI/CustomLevelDialog.hpp \
    QtGUI/SaveAsDialog.hpp \
    QtGUI/StartScreen.hpp \
    QtGUI/EventFilters.hpp \
    QtGUI/TextureList.hpp \
    QtGUI/Textures/TextureDefinitionEditor.hpp \
    QtGUI/Textures/TextureExportDialog.hpp \
    QtGUI/Textures/TextureImportDialog.hpp \
    QtGUI/Textures/TexturePropertiesWidget.hpp \
    QtGUI/Textures/TextureView.hpp \
    QtGUI/Textures/TextureBrowser.hpp \
    QtGUI/Palettes/PaletteEditor.hpp \
    QtGUI/Palettes/PaletteDisplay.hpp \
    QtGUI/Palettes/AddPaletteDialog.hpp \
    QtGUI/Models/DriverModelView.hpp \
    QtGUI/Models/ModelRenderer.hpp \
	MainWindow.hpp
SOURCES = \
    vector.cpp \
    INIParser.cpp \
    Log_Routines/debug_logger.cpp \
    Log_Routines/default_loggers.cpp \
    Driver_Routines/DriverLevels/chairs.cpp \
    Driver_Routines/DriverLevels/heightmaps.cpp \
    Driver_Routines/DriverLevels/lamps.cpp \
    Driver_Routines/DriverLevels/models.cpp \
    Driver_Routines/DriverLevels/RandomModelPlacement.cpp \
    Driver_Routines/DriverLevels/roads.cpp \
    Driver_Routines/DriverLevels/textures.cpp \
    Driver_Routines/DriverLevels/world.cpp \
    Driver_Routines/driver_levels.cpp \
    Driver_Routines/driver_d3d.cpp \
    Driver_Routines/driver_cos.cpp \
    Driver_Routines/driver_den.cpp \
    Driver_Routines/driver_wdf.cpp \
    Driver_Routines/ioFuncs.cpp \
    QtGUI/AboutDialog.cpp \
    QtGUI/CustomLevelDialog.cpp \
    QtGUI/SaveAsDialog.cpp \
    QtGUI/StartScreen.cpp \
    QtGUI/EventFilters.cpp \
    QtGUI/TextureList.cpp \
    QtGUI/Textures/TextureDefinitionEditor.cpp \
    QtGUI/Textures/TextureExportDialog.cpp \
    QtGUI/Textures/TextureImportDialog.cpp \
    QtGUI/Textures/TexturePropertiesWidget.cpp \
    QtGUI/Textures/TextureView.cpp \
    QtGUI/Textures/TextureBrowser.cpp \
    QtGUI/Palettes/PaletteEditor.cpp \
    QtGUI/Palettes/PaletteDisplay.cpp \
    QtGUI/Palettes/AddPaletteDialog.cpp \
    QtGUI/Models/DriverModelView.cpp \
    QtGUI/Models/ModelRenderer.cpp \
	MainWindow.cpp \
	main.cpp
TARGET = \
	DCI
TRANSLATIONS =
RESOURCES =
