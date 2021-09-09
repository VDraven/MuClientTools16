#include "Utls.h"
//------------------------------------------------------------------------
//--Utls
//------------------------------------------------------------------------
namespace Utls {
	void CreateParentDir(fs::path pPath)
	{
		//fs::path pParent = fs::_Parse_parent_path(pPath.wstring());
		//fs::create_directories(pParent);

		fs::path pParent = fs::_Parse_parent_path(pPath.wstring());
		if (!pParent.empty() && !fs::exists(pParent))
		{
			Utls::CreateParentDir(pParent);
			fs::create_directory(pParent);
		}
	}
	
	fs::path RemoveSlashEnd(fs::path path)
	{
		if (path.empty())
			return path;

		auto s = path.string();
		auto c = s[s.length() - 1];
		if (c == '/' || c == '\\')
			return s.substr(0, s.length() - 1);

		return path;
	}

	fs::path BackupPath(fs::path pFile)
	{
		fs::path pBak;
		if (fs::exists(pFile))
		{
			int i = 0;
			std::wstring temp = pFile.wstring() + L"_bak_";
			while (++i)
			{
				if (!fs::exists(temp + std::to_wstring(i)))
				{
					pBak = temp + std::to_wstring(i);
					break;
				}
			}

			try
			{
				fs::rename(pFile, pBak);
			}
			catch (std::exception ex)
			{
				PRINT_DEBUG(ex.what());
				char msg[256];
				printf(msg, "Use '%s' instead. \n", pBak);
				PRINT_DEBUG(msg);
				return pBak;
			}
		}
		return pFile;
	}

	void RemoveAllStringSpaces(std::string& s)
	{
		int count = 0;
		for (int i = 0; i < s.length(); i++)
			if (s[i] != ' ' && s[i] != '\t')
				s[count++] = s[i];
		s.resize(count);
	}
}



