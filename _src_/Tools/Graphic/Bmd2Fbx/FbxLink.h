// docs.microsoft.com/en-au/windows/win32/shell/links?redirectedfrom=MSDN#creating-a-shortcut-and-a-folder-shortcut-to-a-file
#ifndef FBX_LINK_H
#define FBX_LINK_H

#include <Windows.h>

HRESULT CreateLink(LPSTR pszTargetfile, LPSTR pszLinkfile);
void LoadFbxLinkData(const char* fname);

#endif !FBX_LINK_H