cmake_minimum_required(VERSION 3.22)
project(UCGInfinity16X VERSION 0.1.0 LANGUAGES CXX)

option(UCG_BRIDGE_HELPER_ONLY "Build only the 32-bit legacy Hypersonic helper" OFF)

if (NOT WIN32)
  message(STATUS "UCG Infinity 16X is primarily packaged for Windows x64")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (UCG_BRIDGE_HELPER_ONLY)
  add_executable(UCGLegacyHost32 WIN32 Bridge/LegacyHost32.cpp)
  target_compile_definitions(UCGLegacyHost32 PRIVATE UNICODE _UNICODE)
  return()
endif()

include(FetchContent)
FetchContent_Declare(JUCE
  GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
  GIT_TAG 8.0.8)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(UCGInfinity16X
  COMPANY_NAME "UCG Corp"
  BUNDLE_ID "com.ucgcorp.infinity16x"
  PLUGIN_MANUFACTURER_CODE UCGC
  PLUGIN_CODE UI16
  FORMATS VST3 Standalone
  PRODUCT_NAME "UCG Infinity 16X"
  IS_SYNTH TRUE
  NEEDS_MIDI_INPUT TRUE
  NEEDS_MIDI_OUTPUT FALSE
  EDITOR_WANTS_KEYBOARD_FOCUS TRUE
  COPY_PLUGIN_AFTER_BUILD TRUE)

juce_generate_juce_header(UCGInfinity16X)

target_sources(UCGInfinity16X PRIVATE
  Source/PluginProcessor.cpp
  Source/PluginProcessor.h
  Source/PluginEditor.cpp
  Source/PluginEditor.h
  Source/ContentScanner.cpp
  Source/ContentScanner.h)

target_compile_definitions(UCGInfinity16X PUBLIC
  JUCE_WEB_BROWSER=0
  JUCE_USE_CURL=0
  JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(UCGInfinity16X PRIVATE
  juce::juce_audio_utils
  juce::juce_dsp
  PUBLIC juce::juce_recommended_config_flags
  juce::juce_recommended_lto_flags
  juce::juce_recommended_warning_flags)
