/*******************************************************************************************
*
* Shade Player App main.cpp
* 
* Entry point wrapper for the shader player app
*
*
********************************************************************************************/
#include "ShadePlayerApp.hpp"

using namespace std;

int main(void)
{
  shade::ShadePlayerApp app(1920, 1080, 144);
  app.Init();

  // begins app main loop
  app.Start();
}
