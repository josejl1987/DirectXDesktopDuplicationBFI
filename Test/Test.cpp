#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "../DirectX11app/Pass.h"
#include "../DirectX11app/Preset.h"
#include "../DirectX11app/glob.hpp"
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")
int factorial(int number) { return number <= 1 ? number : factorial(number - 1) * number; }

TEST_CASE("Testing shader") {







}