#include "Core.h"
#include "MixBmd.h"

BOOL MixBmd::Decrypt()
{
	size_t group = 13;
	size_t size = sizeof(MIX_INFO);
	size_t pos = 0;

	_map.clear();

	Xor3Byte(&_buf[0], group * 4);
	size_t* counts = (size_t*)&_buf[0];
	pos += group * 4;
	for (size_t i = 0; i < group; i++)
	{
		size_t count = counts[i];
		if (count > 1000)
		{
			std::cout << "Error: MixBmd::Decrypt() count > 1000." << std::endl;
			return FALSE;
		}

		for (size_t j = 0; j < count; j++)
		{
			if (pos + size > _buf.size())
			{
				std::cout << "Error: MixBmd::Decrypt() pos + size > _buf.size()." << std::endl;
				return FALSE;
			}

			Xor3Byte(&_buf[pos], size);

			MIX_INFO *ptr = (MIX_INFO *)&_buf[pos];
			int key = (i << 16) | j;
			InsertToMap(key, ptr);

			pos += size;
		}
	}

	return TRUE;
}

BOOL MixBmd::Encrypt()
{
	assert(_buf.size() > 0);

	size_t group = 13;
	size_t size = sizeof(MIX_INFO);
	size_t pos = 0;
	size_t counts[13];

	memcpy(counts, &_buf[0], 52);
	Xor3Byte(&_buf[0], 52);

	pos += 52;
	for (size_t i = 0; i < group; i++)
	{
		size_t count = counts[i];
		if (count > 1000)
		{
			std::cout << "Error: MixBmd::Encrypt() count > 1000." << std::endl;
			return FALSE;
		}

		for (size_t j = 0; j < count; j++)
		{
			if (pos + size > _buf.size())
			{
				std::cout << "Error: MixBmd::Encrypt() pos + size > _buf.size()." << std::endl;
				return FALSE;
			}

			Xor3Byte(&_buf[pos], size);
			pos += size;
		}
	}

	return TRUE;
}

void MixBmd::TxtOut(std::ofstream & os)
{
	assert(os);
	
	os << "//Group\tID\tType\tDesc_Msg_1\tDesc_Msg_2\tDesc_Msg_3\tBad_Msg_1\tBad_Msg_2\tBad_Msg_3\t"
		"Good_Msg_1\tGood_Msg_2\tGood_Msg_3\tHeight\tWidth\tMinLevel\ttMaxSuccesRate\tMoneyRule\tMoneyReq\t" 
		"UnkByte_1\tUnkByte_2\tUnkByte_3\tUnkByte_4\tUnkByte_5\tUnkByte_6"<< std::endl;
	os << "//(Rules Count)" << std::endl;
	os << "//Rule\tDiv" << std::endl;
	os << "//(Items Count)" << std::endl;
	os << "//StartID\tEndID\tMinLvl\tMaxLvl\tMinOpt\tMaxOpt\tMinDur\tMaxDur\tMinCount\tMaxCount\tType" << std::endl;
	os << "//=================================================================================================" << std::endl;

	for (auto it = _map.begin(); it != _map.end(); it++)
	{
		MIX_INFO* ptr = it->second;
		int group = (it->first >> 16);
		os << group << '\t';
		os << ptr->ID << '\t';
		os << ptr->Type << '\t';
		os << ptr->Desc_Msg_1 << '\t';
		os << ptr->Desc_Msg_2 << '\t';
		os << ptr->Desc_Msg_3 << '\t';
		os << ptr->Bad_Msg_1 << '\t';
		os << ptr->Bad_Msg_2 << '\t';
		os << ptr->Bad_Msg_3 << '\t';
		os << ptr->Good_Msg_1 << '\t';
		os << ptr->Good_Msg_2 << '\t';
		os << ptr->Good_Msg_3 << '\t';
		os << ptr->Height << '\t';
		os << ptr->Width << '\t';
		os << ptr->MinLevel << '\t';
		os << ptr->MaxSuccesRate << '\t';
		os << (DWORD)ptr->MoneyRule << '\t';
		os << ptr->MoneyReq << '\t';
		os << (DWORD)ptr->Unk_1 << '\t';
		os << (DWORD)ptr->Unk_2 << '\t';
		os << (DWORD)ptr->Unk_3 << '\t';
		os << (DWORD)ptr->Unk_4 << '\t';
		os << (DWORD)ptr->Unk_5 << '\t';
		os << (DWORD)ptr->Unk_6 << '\t';

		os << std::endl;

		os << ptr->RulesCount << std::endl;
		for (int i = 0; i < ptr->RulesCount; i++)
		{
			MIX_RULE * rule = &ptr->Rules[i];
			os << rule->Rule << '\t';
			os << rule->Div << '\t';
			os << std::endl;
		}

		os << ptr->ItemsCount << std::endl;
		for (int i = 0; i < ptr->ItemsCount; i++)
		{
			MIX_ITEM * item = &ptr->Items[i];
			os << item->StartID << '\t';
			os << item->EndID << '\t';
			os << item->MinLvl << '\t';
			os << item->MaxLvl << '\t';
			os << item->MinOpt << '\t';
			os << item->MaxOpt << '\t';
			os << item->MinDur << '\t';
			os << item->MaxDur << '\t';
			os << item->MinCount << '\t';
			os << item->MaxCount << '\t';
			os << item->Type << '\t';
			os << std::endl;
		}

		os << "//=================================================================================================" << std::endl;
	}
}

void MixBmd::TxtIn(std::ifstream & is)
{
	assert(is);

	std::string line;
	size_t size = sizeof(MIX_INFO);
	size_t counts[13] { 0 };

	std::map<int, MIX_INFO> m;
	size_t n = 0;
	while (getline(is, line))
	{
		if (line[0] == '/' && line[1] == '/')
			continue;

		MIX_INFO temp;
		int group;

		sscanf(line.c_str(),
			"%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%hhd\t%d\t%hhd\t%hhd\t%hhd\t%hhd\t%hhd\t%hhd"
			, &group, &temp.ID, &temp.Type, &temp.Desc_Msg_1, &temp.Desc_Msg_2, &temp.Desc_Msg_3, &temp.Bad_Msg_1, &temp.Bad_Msg_2, &temp.Bad_Msg_3
			, &temp.Good_Msg_1, &temp.Good_Msg_2, &temp.Good_Msg_3, &temp.Height, &temp.Width, &temp.MinLevel, &temp.MaxSuccesRate, &temp.MoneyRule, &temp.MoneyReq
			, &temp.Unk_1, &temp.Unk_2, &temp.Unk_3, &temp.Unk_4, &temp.Unk_5, &temp.Unk_6
		);

		if (group < 0 || group >= 13) 
		{
			std::cout << "MixBmd::TxtIn (group < 0 || group >= 13)" << std::endl;
			return;
		}

		getline(is, line);
		{
			sscanf(line.c_str(), "%d", &temp.RulesCount);
			if (temp.RulesCount < 0 || temp.RulesCount > 32)
			{
				std::cout << "MixBmd::TxtIn (RulesCount < 0 || RulesCount > 32)" << std::endl;
				return;
			}

			for (int i = 0; i < temp.RulesCount; i++)
			{
				getline(is, line);
				sscanf(line.c_str(), "%d\t%f", &temp.Rules[i].Rule, &temp.Rules[i].Div);
			}
		}

		getline(is, line);
		{
			sscanf(line.c_str(), "%d", &temp.ItemsCount);
			if (temp.ItemsCount < 0 || temp.ItemsCount > 8)
			{
				std::cout << "MixBmd::TxtIn (ItemsCount < 0 || ItemsCount > 8)" << std::endl;
				return;
			}

			for (int i = 0; i < temp.ItemsCount; i++)
			{
				getline(is, line);
				sscanf(line.c_str(), "%hd\t%hd\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d"
					, &temp.Items[i].StartID, &temp.Items[i].EndID, &temp.Items[i].MinLvl, &temp.Items[i].MaxLvl, &temp.Items[i].MinOpt
					, &temp.Items[i].MaxOpt, &temp.Items[i].MinDur, &temp.Items[i].MaxDur, &temp.Items[i].MinCount, &temp.Items[i].MaxCount, &temp.Items[i].Type);
			}
		}


		int key = group << 16 | temp.ID;
		if (m.find(key) == m.end())
		{
			counts[group]++;
			n++;
			m[key] = temp;
		}
		else
		{
			std::cout << " Duplicated Key : " << group << " " << temp.ID << std::endl;
		}
	}

	size_t buf_size = 52 + (n * size);
	_buf.resize(buf_size);
	memcpy(&_buf[0], counts, 52);
	size_t pos = 52;

	if (n > 0)
	{
		for (auto it = m.begin(); it != m.end(); it++)
		{
			memcpy(&_buf[pos], &it->second, size);
			pos += size;
		}
	}

}
