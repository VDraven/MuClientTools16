// docs.microsoft.com/en-au/windows/win32/shell/links?redirectedfrom=MSDN#creating-a-shortcut-and-a-folder-shortcut-to-a-file

#include "FbxLink.h"
#include <Windows.h>
#include <shlobj.h>
#include <winnls.h>
#include <shobjidl.h>
#include <objbase.h>
#include <objidl.h>
#include <shlguid.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <map>
#include <vector>

using namespace std;

multimap<string, vector<string>> fbx_links_data;

HRESULT CreateLink(LPSTR pszTargetfile, LPSTR pszLinkfile)
{
    IShellLink* psl;
    IPersistFile* ppf;
    HRESULT hRes;

    CoInitialize(NULL);
    hRes = E_INVALIDARG;
    if (
        (pszTargetfile != NULL) && (strlen(pszTargetfile) > 0) &&
        (pszLinkfile != NULL) && (strlen(pszLinkfile) > 0)
        )
    {
        hRes = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl);

        if (SUCCEEDED(hRes))
        {
            psl->SetPath(pszTargetfile);

            hRes = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf);
            if (SUCCEEDED(hRes))
            {
                WCHAR wsz[MAX_PATH];
                MultiByteToWideChar(CP_ACP, 0, pszLinkfile, -1, wsz, MAX_PATH);

                hRes = ppf->Save(wsz, TRUE);
                ppf->Release();
            }
            psl->Release();
        }

    }
    CoUninitialize();
    return (hRes);
}

void LoadFbxLinkData(const char* fname)
{
	fbx_links_data.clear();

	std::ifstream is(fname);
	if (!is.is_open()) return;

	string line, token;
	while (getline(is, line))
	{
		stringstream ss(line);

		if (getline(ss, token, '\t'))
		{
			if (token.empty())
				continue;

			string bmd(token);
			transform(bmd.begin(), bmd.end(), bmd.begin(), tolower);

			if (getline(ss, token, '\t'))
			{
				if (token.empty())
					continue;

				string fbx(token);
				vector<string> links;

				while (getline(ss, token, '\t'))
				{
					if (token.empty())
						continue;

					string link(token);
					link += '\\';
					link += fbx;
					links.push_back(move(link));
				}

				if (links.size() > 0)
					fbx_links_data.emplace(move(bmd), move(links));
			}
		}
	}

	is.close();
}