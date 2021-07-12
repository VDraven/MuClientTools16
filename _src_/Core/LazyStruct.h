#ifndef LAZY_STRUCT_H
#define LAZY_STRUCT_H

struct OffsetInfo
{
	size_t Type;
	size_t Offset;
	std::string Format;
};

enum LAZY_TYPE_FLAG
{
	_UNK_ = 0,
	_1BYTE_ = 1,
	_2BYTE_ = 2,
	_4BYTE_ = 4,
	//_8BYTE_ = 8,	//ONLY x86
	_FLOAT_ = 8,
	_DOUBLE_ = 16,
	_SIGNED_ = 32,
	_CSTR_ = 64,
};

namespace LazyStruct
{

	size_t ParseType(std::string& type);
	size_t ParseMember(std::string& member, std::string& member_type, std::string& member_name);
	int ParseMemberType(std::string& member_type, std::string& format);
	void CalculateOffset(std::vector<OffsetInfo>& offset, int pack);
	std::string ParseMembersToLabel(std::string&& members);
	std::string ParseMembersToFormat(std::string&& members);
	std::vector<OffsetInfo> ParseMembersToOffset(std::string && members, int pack);
}

#define STRUCT(_) struct _
#define LAZY(Members)											\
{																\
	Members														\
	static std::string GetLabel()								\
	{															\
		static const std::string LABEL							\
			= LazyStruct::ParseMembersToLabel(#Members);		\
		return LABEL;											\
	};															\
	static std::string GetFormat()								\
	{															\
		static const std::string FORMAT							\
			= LazyStruct::ParseMembersToFormat(#Members);		\
		return FORMAT;											\
	};															\
	static std::vector<OffsetInfo> GetOffset()					\
	{															\
		static const std::vector<OffsetInfo> OFFSET				\
			= LazyStruct::ParseMembersToOffset(#Members, pack);	\
		return OFFSET;											\
	}															\
	static const int pack = 0;									\
}

#define LAZY1(Members)											\
{																\
	Members														\
	static std::string GetLabel()								\
	{															\
		static const std::string LABEL							\
			= LazyStruct::ParseMembersToLabel(#Members);		\
		return LABEL;											\
	};															\
	static std::string GetFormat()								\
	{															\
		static const std::string FORMAT							\
			= LazyStruct::ParseMembersToFormat(#Members);		\
		return FORMAT;											\
	};															\
	static std::vector<OffsetInfo> GetOffset()					\
	{															\
		static const std::vector<OffsetInfo> OFFSET				\
			= LazyStruct::ParseMembersToOffset(#Members, pack);	\
		return OFFSET;											\
	}															\
	static const int pack = 1;									\
}

#endif // LAZY_STRUCT_H