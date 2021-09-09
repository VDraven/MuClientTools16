// This file contains the 'main' function. Program execution begins and ends there.
// Requirement: C++17
#include "Core.h"
#include "CameraAnglePositionBmd.h"

constexpr const char EXT_1[] = ".bmd";
constexpr const char EXT_2[] = ".ini";

int main(int argc, char** argv)
{
	return _Convert<CameraAnglePositionBmd, EXT_1, EXT_2>(argc, argv);
}