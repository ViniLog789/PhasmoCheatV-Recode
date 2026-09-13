#pragma once
#include "sdk.h"

namespace SDK
{
	struct ContractFields
	{
		MonoBehaviourPunFields monoBehaviourPunFields;
		void* voteButton;
		void* levelNameText;
		void* eventStickerImage;
		void* easterSprite;
		void* halloweenSprite;
		void* holidaySprite;
		void* alanWakeSprite;
		void* voteSpots;
		Map* info;
		bool unlocked;
		// more
	};

	struct Contract
	{
		void* Clazz;
		void* Monitor;
		ContractFields Fields;
	};

	DEC_MET(Contract_Vote, void(*)(Contract* contract, MethodInfo* methodInfo), "Assembly-CSharp", "", "Contract", "Vote", 0);
}