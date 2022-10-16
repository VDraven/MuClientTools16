#include "FormulaDataBmd.h"

BOOL FormulaDataBmd::Decrypt()
{
	assert(_buf.size() > 12);

	size_t size = sizeof(FORMULA_DATA);
	size_t total = *(size_t*)&_buf[0];
	size_t group_count = *(size_t*)&_buf[4];
	size_t pos = 8;
	int err = -1;

	for (size_t i = 0; i < group_count; i++)
	{
		if (pos + 4 > _buf.size()) return TRUE;	//ignore error

		Xor3Byte(&_buf[pos], 4);
		int group_idx = *(int*)&_buf[pos];
		pos += 4;

		//assert(group_idx == i);

		Xor3Byte(&_buf[pos], 4);
		int sub_count = *(int*)&_buf[pos];
		pos += 4;
		for (int j = 0; j < sub_count; j++)
		{
			if (pos + size > _buf.size()) return TRUE;	//ignore error

			Xor3Byte(&_buf[pos], size);
			FORMULA_DATA* pData = (FORMULA_DATA*)&_buf[pos];
			int key = (group_idx << 16) | j;
			while (_map.find(key) != _map.end())	//check duplicated keys
			{
				key = err;
				err--;
			}
			_map.insert(std::make_pair(key, pData));
			pos += size;
		}
	}

	return TRUE;
}

BOOL FormulaDataBmd::Encrypt()
{
	assert(_buf.size() > 12);
	size_t size = sizeof(FORMULA_DATA);
	size_t total = *(size_t*)&_buf[0];
	size_t group_count = *(size_t*)&_buf[4];
	size_t pos = 8;

	//assert(_buf.size() == 12 + (total * size) + (group_count * 8));

	for (size_t i = 0; i < group_count; i++)
	{
		size_t group_idx = *(size_t*)&_buf[pos];
		Xor3Byte(&_buf[pos], 4);
		pos += 4;

		size_t sub_count = *(size_t*)&_buf[pos];
		Xor3Byte(&_buf[pos], 4);
		pos += 4;
		for (size_t j = 0; j < sub_count; j++)
		{
			Xor3Byte(&_buf[pos], size);
			pos += size;
		}
	}
	DWORD CRC = CalculateCRC(&_buf[8], _buf.size() - 12, _wkey);
	*(DWORD*)&_buf[_buf.size() - 4] = CRC;

	return TRUE;
}

void FormulaDataBmd::TxtOut(std::ofstream & os)
{
	assert(os);

	//	"//Group\tID\tFormula_Text";
	static const std::string LABEL = "//Group\t" + FORMULA_DATA::GetLabel();
	os << LABEL << std::endl;

	for (auto it = _map.begin(); it != _map.end(); it++)
	{
		FORMULA_DATA* ptr = it->second;
		int group = (it->first >> 16);
		os << group << '\t';
		os << ptr->ID << '\t';
		os << (Utls::IsEmptyCStr(ptr->Text) ? "[NULL]" : ptr->Text) << '\t';

		os << std::endl;
	}
}

void FormulaDataBmd::TxtIn(std::ifstream & is)
{
	assert(is);

	static const std::string FORMAT = "%d\t" + FORMULA_DATA::GetFormat();

	std::string line;
	size_t size = sizeof(FORMULA_DATA);
	size_t n = 0;
	std::map<int, std::vector<FORMULA_DATA>> temp;

	while (getline(is, line))
	{
		if (line[0] == '/' && line[1] == '/')
			continue;
		FORMULA_DATA Data;
		int group;
		sscanf(line.c_str(),
			//"%d\t%d\t%[^\t]%*c"
			FORMAT.c_str()
			, &group, &Data.ID, &Data.Text);
		temp[group].push_back(Data);
		n++;
	}
	int group_count = temp.size();
	_buf.resize(12 + (n * size) + (group_count *  8));
	*(DWORD*)&_buf[0] = n;
	*(DWORD*)&_buf[4] = temp.size();

	size_t pos = 8;
	for (auto it = temp.begin(); it != temp.end(); it++)
	{
		size_t group = it->first;
		size_t sub_count = it->second.size();

		*(int*)&_buf[pos] = group;
		pos += 4;
		*(int*)&_buf[pos] = sub_count;
		pos += 4;

		for (size_t i = 0; i < sub_count; i++)
		{
			FORMULA_DATA* formula = &it->second[i];
			memcpy(&_buf[pos], formula, size);
			pos += size;
		}
	}
}
