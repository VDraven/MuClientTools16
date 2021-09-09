// This file contains the 'main' function. Program execution begins and ends there.
// Requirement: C++17, bit7z library, 7z.dll
#include "Core.h"
#include "LangMpr.h"

using namespace std;

int main(int argc, char** argv)
{
	LangMpr convertor;
	const char* szInputPath = nullptr;
	const char* szOutputPath = nullptr;

	if (argc >= 2)
		szInputPath = argv[1];
	if (argc >= 3)
		szOutputPath = argv[2];

	if (!szInputPath)
	{
		PRINT_DEBUG("Drag&Drop the '.bmd' / '.txt' file \n" "\t or use console command to execute with the file path.");
		return EXIT_FAILURE;
	}

	if (!fs::exists(szInputPath))
	{
		PRINT_DEBUG("[ERROR] Input file/directory does not exist.");
		return EXIT_FAILURE;
	}
	if (fs::is_regular_file(szInputPath))
	{
		if (szOutputPath && !fs::is_directory(szOutputPath))
		{
			PRINT_DEBUG("[ERROR] szOutputPath is not a directory.");
			return EXIT_FAILURE;
		}

		if (!convertor.Unpack(szInputPath, szOutputPath))
		{
			PRINT_DEBUG("[ERROR] Failed to unpack ." << szInputPath);
			return EXIT_FAILURE;
		}
	}
	else if (fs::is_directory(szInputPath))
	{
		if (szOutputPath && !fs::is_regular_file(szOutputPath))
		{
			PRINT_DEBUG("[ERROR] szOutputPath is not a regular file.");
			return EXIT_FAILURE;
		}

		if (!convertor.Pack(szInputPath, szOutputPath))
		{
			PRINT_DEBUG("[ERROR] Failed to pack ." << szInputPath);
			return EXIT_FAILURE;
		}
	}
	else
	{
		PRINT_DEBUG("[ERROR] Invalid file/folder path.");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}