#pragma once
#include "../Includes.h"

namespace PhasmoCheatV::Features::Auto
{
	class AutoMap final : public FeatureCore
	{
	public:
		explicit AutoMap();
		~AutoMap() override = default;

		void OnActivate() override;
		void OnDeactivate() override;
		void OnRender() override {}
		void OnMenuRender() override;
		void AutoMapHandler();

	private:
		struct ContractInfo // idk what to call this
		{
			SDK::Contract* ptr = nullptr;
			std::string name;
		};

		void UpdateContracts();
		void TryAutoVote();

		std::vector<ContractInfo> contracts;
		int selectedIndex = 0;
		bool hasVoted = false;
		float lastUpdateTime = 0.f;
	};
}