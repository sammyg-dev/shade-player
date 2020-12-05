/*******************************************************************************************
*
*   sammy g sandbox
*
*   testing raylib and raylib-cpp
*
*
********************************************************************************************/
#include "ShadePlayerApp.hpp"


using namespace std;

int main(void)
{
  shade::ShadePlayerApp app = shade::ShadePlayerApp(1920, 1080, 60);
  app.Init();

  // begins app main loop
  app.Start();
}
