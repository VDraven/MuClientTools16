
#ifndef NPC_DIALOGUE_BMD_H
#define NPC_DIALOGUE_BMD_H

typedef TxtConvertor<NPC_DIALOGUE> NPCDialogueBmdConvert;
class NPCDialogueBmd : public NPCDialogueBmdConvert
{
public:
	NPCDialogueBmd() : NPCDialogueBmdConvert() { hasCounter = false; hasCRC = false; };
	virtual ~NPCDialogueBmd() {};
private:
	void MakeLabelEx(std::ofstream& os)
	{
		os << "//NPCQuest texts are in the QuestWords file." << std::endl;
	};
};

#endif

//================================================================

#ifndef QUEST_PROGRESS_BMD_H
#define QUEST_PROGRESS_BMD_H

typedef TxtConvertor<QUEST_PROGRESS> QuestProgressBmdConvert;
class QuestProgressBmd : public QuestProgressBmdConvert
{
public:
	QuestProgressBmd() : QuestProgressBmdConvert() { hasCounter = false; hasCRC = false; };
	virtual ~QuestProgressBmd() {};

private:
	void MakeLabelEx(std::ofstream& os)
	{
		os << "//NPCQuest texts are in the QuestWords file." << std::endl;
	};
};

#endif

//================================================================

#ifndef QUEST_WORDS_BMD_H
#define QUEST_WORDS_BMD_H

typedef TxtConvertor<QUEST_WORDS> QuestWordsBmdConvert;
class QuestWordsBmd : public QuestWordsBmdConvert
{
public:
	QuestWordsBmd() : QuestWordsBmdConvert() {};
	virtual ~QuestWordsBmd() {};

private:
	BOOL Decrypt();
	BOOL Encrypt();
	void TxtOut(std::ofstream& os);
	void TxtIn(std::ifstream& is);
};

#endif