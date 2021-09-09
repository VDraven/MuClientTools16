#ifndef CONVERTOR_TEMPLATE_H
#define CONVERTOR_TEMPLATE_H

template <class Convertor, const char* EXT_1, const char* EXT_2>
int _Convert(int argc, char** argv)
{
	Convertor convertor;
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
		PRINT_DEBUG("[ERROR] Input file does not exist.");
		return EXIT_FAILURE;
	}

	if (fs::is_regular_file(szInputPath))
	{
		auto ext = fs::path(szInputPath).extension().string();
		Utls::ToLowerCaseString(ext);
		if (ext == EXT_1)
		{
			if (!convertor.Unpack(szInputPath, szOutputPath))
			{
				return EXIT_FAILURE;
			}
		}
		else if (ext == EXT_2)
		{
			if (!convertor.Pack(szInputPath, szOutputPath))
			{
				return EXIT_FAILURE;
			}
		}
		else
		{
			PRINT_DEBUG("[ERROR] File ext is not '.bmd' / '.txt'.");
			return EXIT_FAILURE;
		}
	}
	else
	{
		PRINT_DEBUG("[ERROR] Invalid file path.");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

constexpr const char DEFAULT_EXT_1[] = ".bmd";
constexpr const char DEFAULT_EXT_2[] = ".txt";

template <class Convertor>
int Convert(int argc, char** argv)
{
	return _Convert<Convertor, DEFAULT_EXT_1, DEFAULT_EXT_2>(argc, argv);
}

#endif // !CONVERTOR_TEMPLATE_H
