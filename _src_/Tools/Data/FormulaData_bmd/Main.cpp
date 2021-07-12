// This file contains the 'main' function. Program execution begins and ends there.
// Requirement: C++17
#include "Core.h"
#include "FormulaDataBmd.h"

using namespace std;

int main(int argc, char** argv)
{
	FormulaDataBmd opener;
	const char* szInputPath = "formuladata.bmd";
	const char* szOutputPath = nullptr;

	if (argc >= 2)
		szInputPath = argv[1];
	if (argc >= 3)
		szOutputPath = argv[2];

	if (!szInputPath)
	{
		cout << "\t Drag&Drop the '.bmd' / '.txt' file. \n";
		cout << "\t  or use console command to execute with the file path. \n";
		return EXIT_FAILURE;
	}

	if (!fs::exists(szInputPath))
	{
		cout << "Error: Input file does not exist.\n" << endl;
		return EXIT_FAILURE;
	}

	if (fs::is_regular_file(szInputPath))
	{
		auto ext = fs::path(szInputPath).extension();
		if (ext == L".bmd")
		{
			if (!opener.Unpack(szInputPath, szOutputPath))
			{
				return EXIT_FAILURE;
			}
		}
		else if (ext == L".txt")
		{
			if (!opener.Pack(szInputPath, szOutputPath))
			{
				return EXIT_FAILURE;
			}
		}
		else
		{
			cout << "Error: File ext is not '.bmd' / '.txt' \n" << endl;
			return EXIT_FAILURE;
		}
	}
	else
	{
		cout << "Error: Invalid file path.\n" << endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}