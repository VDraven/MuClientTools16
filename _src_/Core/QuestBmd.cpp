#include "QuestBmd.h"

void QuestBmd::TxtOut(std::ofstream & os)
{
	assert(os);

	for (auto it = _map.begin(); it != _map.end(); it++)
	{
		QUEST_INFO* ptr = it->second;

		if (ptr->ItemCount + ptr->TypeCount == 0)	//blank
			continue;

		os << "//================================================="<< std::endl;
		os << "//==Quest: " << ptr->QuestName << std::endl;
		os << "//=================================================" << std::endl;
		os << "//ItemCount\tTypeCount\tNPC\tQuestName" << std::endl;
		os << ptr->ItemCount << '\t';
		os << ptr->TypeCount << '\t';
		os << ptr->NPC << '\t';
		os << ptr->QuestName << '\t';
		os << std::endl;

		os << "//QuestItems" << std::endl;
		os << "//Flag\tQuestType\tItemCat\tItemIndex\tLevel\tAmount\tLinkConditionIndex\tDW\tDK\tFE\tMG\tDL\tSU\tRF\tGL\tRW\tSL\tGC\tMsg_1\tMsg_2\tMsg_3\tMsg_4" << std::endl;
		
		for (int i = 0; i < ptr->ItemCount; i++)
		{
			os << (int)ptr->Items[i].Flag << '\t';
			os << (int)ptr->Items[i].QuestType << '\t';
			os << (int)ptr->Items[i].ItemCat << '\t';
			os << (int)ptr->Items[i].ItemIndex << '\t';
			os << (int)ptr->Items[i].Level << '\t';
			os << (int)ptr->Items[i].Amount << '\t';
			os << (int)ptr->Items[i].LinkConditionIndex << '\t';
			os << (int)ptr->Items[i].DW << '\t';
			os << (int)ptr->Items[i].DK << '\t';
			os << (int)ptr->Items[i].FE << '\t';
			os << (int)ptr->Items[i].MG << '\t';
			os << (int)ptr->Items[i].DL << '\t';
			os << (int)ptr->Items[i].SU << '\t';
			os << (int)ptr->Items[i].RF << '\t';
			os << (int)ptr->Items[i].GL << '\t';
			os << (int)ptr->Items[i].RW << '\t';
			os << (int)ptr->Items[i].SL << '\t';
			os << (int)ptr->Items[i].GC << '\t';

			os << ptr->Items[i].Msg_1 << '\t';
			os << ptr->Items[i].Msg_2 << '\t';
			os << ptr->Items[i].Msg_3 << '\t';
			os << ptr->Items[i].Msg_4 << '\t';
			os << std::endl;
		}

		os << "//QuestTypes" << std::endl;
		os << "//Flag\tType\tQuestIndex\tMinLvl\tMaxLvl\tNeedStr\tNeedZen\tMsg" << std::endl;

		for (int i = 0; i < ptr->TypeCount; i++)
		{
			os << (int)ptr->Types[i].Flag << '\t';
			os << (int)ptr->Types[i].Type << '\t';
			os << ptr->Types[i].QuestIndex << '\t';
			os << ptr->Types[i].MinLvl << '\t';
			os << ptr->Types[i].MaxLvl << '\t';
			os << ptr->Types[i].NeedStr << '\t';
			os << ptr->Types[i].NeedZen << '\t';
			os << ptr->Types[i].Msg << '\t';
			os << std::endl;
		}

		os << "//" << std::endl;
	}

}

void QuestBmd::TxtIn(std::ifstream & is)
{
	assert(is);

	std::string line;
	size_t size = sizeof(QUEST_INFO);
	size_t count = 200;

	_buf.resize(count * size);
	memset(_buf.data(), 0x00, _buf.size());

	size_t n = 0;
	while (getline(is, line) && n < count)
	{
		if (line[0] == '/' && line[1] == '/')
			continue;

		QUEST_INFO* ptr = (QUEST_INFO*)&_buf[n*size];
		sscanf(line.c_str(),
			"%hd\t%hd\t%hd\t%[^\t]%*c"
			, &ptr->ItemCount, &ptr->TypeCount, &ptr->NPC, &ptr->QuestName
		);
		
		if (ptr->ItemCount > 16 || ptr->TypeCount > 16)
		{
			std::cout << "Error: Quest Items/Types Count must not > 16." << std::endl;
			return;
		}

		int i = 0;
		while (getline(is, line) && i < ptr->ItemCount)
		{
			if (line[0] == '/' && line[1] == '/')
				continue;

			QUEST_INFO_ITEM* pItem = &ptr->Items[i];
			sscanf(line.c_str(),
				"%hhd\t%hhd\t%hd\t%hhd\t%hhd\t%hhd\t"
				"%hhd\t%hhd\t%hhd\t%hhd\t%hhd\t%hhd\t%hhd\t%hhd\t"
				"%hhd\t%hhd\t%hhd\t%hhd\t"
				"%hd\t%hd\t%hd\t%hd"
				, &pItem->Flag, &pItem->QuestType, &pItem->ItemCat, &pItem->ItemIndex, &pItem->Level, &pItem->Amount
				, &pItem->LinkConditionIndex, &pItem->DW, &pItem->DK, &pItem->FE, &pItem->MG, &pItem->DL, &pItem->SU, &pItem->RF
				, &pItem->GL, &pItem->RW, &pItem->SL, &pItem->GC
				, &pItem->Msg_1, &pItem->Msg_2, &pItem->Msg_3, &pItem->Msg_4
			);

			i++;
		}

		i = 0;
		while (getline(is, line) && i < ptr->TypeCount)
		{
			if (line[0] == '/' && line[1] == '/')
				continue;

			QUEST_INFO_TYPE* pType = &ptr->Types[i];
			sscanf(line.c_str(),
				"%hhd\t%hhd\t%hd\t%hd\t%hd\t%d\t%d\t%hd"
				, &pType->Flag, &pType->Type, &pType->QuestIndex, &pType->MinLvl, &pType->MaxLvl, &pType->NeedStr, &pType->NeedZen, &pType->Msg
			);

			i++;
		}

		n++;
	}
}
