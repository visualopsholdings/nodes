/*
  zcadmin.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 1-Jul-2024
    
  Adnin tool for ZMQChat.
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqchat
*/

#include <FL/Fl.H>
#include <FL/Fl_Window.H>

auto main(int argc, char *argv[]) -> int {
  auto window = Fl_Window {300, 300};
  window.show(argc, argv);
  return Fl::run();
}
