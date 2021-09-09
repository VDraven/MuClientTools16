#ifndef UTLS_H
#define UTLS_H

#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))

namespace Utls
{
	void CreateParentDir(fs::path path);
	fs::path RemoveSlashEnd(fs::path path);
	fs::path BackupPath(fs::path path);
	void RemoveAllStringSpaces(std::string& s);

	inline void ToLowerCaseString(std::string& s)
	{
		std::transform(s.begin(), s.end(), s.begin(), tolower);
	}

	inline bool IsEmptyCStr(const char* str)
	{
		return str && str[0] == '\0';
	};
}

#endif // !UTLS_H
