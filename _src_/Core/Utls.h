#ifndef UTLS_H
#define UTLS_H

#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))

namespace Utls
{
	void CreateParentDir(fs::path path);
	fs::path RemoveSlashEnd(fs::path path);
	fs::path BackupPath(fs::path path);
	bool IsEmptyCStr(const char* str);
	void RemoveAllStringSpaces(std::string& s);
	void ToLowerCaseString(std::string& s);
}

#endif // !UTLS_H
