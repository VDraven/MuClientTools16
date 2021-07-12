// This file contains the 'main' function. Program execution begins and ends there.
// Requirement: C++17, bit7z library, 7z.dll
#include "Core.h"
#include "LangMpr.h"

using namespace std;

int main(int argc, char** argv)
{
	LangMpr opener;
	const char* szInputPath = "lang.mpr";
	const char* szOutputPath = nullptr;

	if (argc >= 2)
		szInputPath = argv[1];
	if (argc >= 3)
		szOutputPath = argv[2];

	if (!szInputPath)
	{
		cout << "\t Drag & Drop the Lang file/folder \n";
		cout << "\t  or use console command to execute with the file path. \n";
		return EXIT_FAILURE;
	}

	if (!fs::exists(szInputPath))
	{
		cout << "Error: Input file/folder does not exist.\n" << endl;
		return EXIT_FAILURE;
	}
	if (fs::is_regular_file(szInputPath)
		&& (!szOutputPath || fs::is_directory(szOutputPath)))
	{
		//szInputPath = fs::path(szInputPath).relative_path().string().c_str();
		if (!opener.Unpack(szInputPath, szOutputPath))
		{
			return EXIT_FAILURE;
		}
	}
	else if (fs::is_directory(szInputPath)
		&& (!szOutputPath || fs::is_regular_file(szOutputPath)))
	{
		if (!opener.Pack(szInputPath, szOutputPath))
		{
			return EXIT_FAILURE;
		}
	}
	else
	{
		cout << "Error: Invalid file/folder path.\n" << endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}