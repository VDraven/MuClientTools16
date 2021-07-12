
#ifndef QUEST_BMD_H
#define QUEST_BMD_H

typedef TxtConvertor<QUEST_INFO> QuestBmdConvert;
class QuestBmd : public QuestBmdConvert
{
public:
	QuestBmd() : QuestBmdConvert() { hasCounter = false; hasCRC = false; };
	virtual ~QuestBmd() { };

private:
	void TxtOut(std::ofstream& os);
	void TxtIn(std::ifstream& is);
};

#endif