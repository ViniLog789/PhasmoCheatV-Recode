#include "utils.h"

using namespace PhasmoCheatV;
namespace fs = std::filesystem;

std::string Utils::GetPhasmoCheatVDirectory()
{
	return "C:\\VComDev\\PhasmoCheatV";
}

void Utils::CreatePhasmoCheatVDirectory()
{
	const std::string basePath = "C:\\VComDev";
	if (!std::filesystem::exists(basePath))
	{
		LOG_INFO("Creating VComDev base directory.");
		std::filesystem::create_directory(basePath);
	}

	const std::string configDirectoryPath = GetPhasmoCheatVDirectory();
	if (!std::filesystem::exists(configDirectoryPath))
	{
		LOG_INFO("PhasmoCheatV directory not found, creating one.");
		std::filesystem::create_directory(configDirectoryPath);
	}

	const std::string configsPath = configDirectoryPath + "\\configs";
	const std::string imagesPath = configDirectoryPath + "\\Images";
	const std::string fontsPath = configDirectoryPath + "\\fonts";

	if (!std::filesystem::exists(configsPath))
	{
		LOG_INFO("Creating configs folder.");
		std::filesystem::create_directory(configsPath);
	}

	if (!std::filesystem::exists(imagesPath))
	{
		LOG_INFO("Creating Images folder.");
		std::filesystem::create_directory(imagesPath);
	}

	if (!std::filesystem::exists(fontsPath))
	{
		LOG_INFO("Creating fonts folder.");
		std::filesystem::create_directory(fontsPath);
	}
}

bool Utils::NeedsMigration()
{
	return std::filesystem::exists("C:\\PhasmoCheatV");
}

bool Utils::MigrateDirectory()
{
	const std::string oldPath = "C:\\PhasmoCheatV";
	const std::string basePath = "C:\\VComDev";
	const std::string newPath = "C:\\VComDev\\PhasmoCheatV";

	try
	{
		if (!std::filesystem::exists(basePath))
			std::filesystem::create_directory(basePath);

		std::filesystem::copy(
			oldPath, newPath,
			std::filesystem::copy_options::recursive |
			std::filesystem::copy_options::overwrite_existing
		);

		std::filesystem::remove_all(oldPath);

		LOG_INFO("Migration completed: ", oldPath, " -> ", newPath);
		Config::LoadConfig();
		return true;
	}
	catch (const std::exception& e)
	{
		LOG_ERROR("Migration failed: ", e.what());
		return false;
	}
}

ImVec4 Utils::HSV2RGB(float h, float s, float v)
{
	float r, g, b;

	int i = int(h * 6.0f);
	float f = h * 6.0f - i;
	float p = v * (1.0f - s);
	float q = v * (1.0f - f * s);
	float t = v * (1.0f - (1.0f - f) * s);

	switch (i % 6)
	{
	case 0: r = v; g = t; b = p; break;
	case 1: r = q; g = v; b = p; break;
	case 2: r = p; g = v; b = t; break;
	case 3: r = p; g = q; b = v; break;
	case 4: r = t; g = p; b = v; break;
	case 5: r = v; g = p; b = q; break;
	}

	return ImVec4(r, g, b, 1.0f);
}

SDK::Network* Utils::GetNetwork()
{
	if (InGame::network)
		return InGame::network;

	if (!SDK::Application_get_isPlaying(nullptr))
		return nullptr;
	
	auto* network_gobj = FindObjectByName("Network"); if (!network_gobj) return nullptr;
	auto* network_Comp = SDK::GameObject_GetComponentByName(network_gobj, SysStrToUnityStr("Network"), nullptr); if (!network_Comp) return nullptr;
	InGame::network = reinterpret_cast<SDK::Network*>(network_Comp);

	return InGame::network;
}

SDK::ListPlayer* Utils::GetAllPlayers()
{
	if (!SDK::MapController_sFields->instance)
		return nullptr;

	return SDK::MapController_sFields->instance->Fields.Players;
}

int Utils::GetPlayerInx(const SDK::Player* player)
{
	const auto players = GetAllPlayers();
	if (!players)
		return -1;

	for (int playerIndex = 0; playerIndex < players->Fields.Size; playerIndex++)
	{
		if (players->Fields.Items->Vector[playerIndex] == player)
			return playerIndex;
	}

	return -1;
}

SDK::NetworkPlayerSpot* Utils::GetNetworkPlayerSpot(const SDK::Player* player)
{
	if (!GetNetwork())
		return nullptr;

	return InGame::network->Fields.NetworkPlayerSpots->Fields.Items->Vector[GetPlayerInx(player)];
}

SDK::Player* Utils::GetLocalPlayer()
{
	SDK::Network* network = GetNetwork();
	if (!network)
	{
		return nullptr;
	}

	SDK::Player* player = network->Fields.localPlayer;
	if (!player)
	{
		return nullptr;
	}

	return player;
}

std::string Utils::getKeyName(int keyCode)
{
	switch (keyCode)
	{
	case VK_INSERT: return "INSERT";
	case VK_DELETE: return "DELETE";
	case VK_HOME: return "HOME";
	case VK_END: return "END";
	case VK_PRIOR: return "PAGE UP";
	case VK_NEXT: return "PAGE DOWN";
	case VK_F1: return "F1";
	case VK_F2: return "F2";
	case VK_F3: return "F3";
	case VK_F4: return "F4";
	case VK_F5: return "F5";
	case VK_F6: return "F6";
	case VK_F7: return "F7";
	case VK_F8: return "F8";
	case VK_F9: return "F9";
	case VK_F10: return "F10";
	case VK_F11: return "F11";
	case VK_F12: return "F12";
	case VK_RETURN: return "Enter";
	default:
		if (keyCode >= 'A' && keyCode <= 'Z')
			return std::string(1, (char)keyCode);
		else if (keyCode >= '0' && keyCode <= '9')
			return std::string(1, (char)keyCode);
		else
			return "Key " + std::to_string(keyCode);
	}
}


std::string Utils::GhostEnumToStr(SDK::GhostState ghostState)
{
	std::string ghostStateString;

	switch (ghostState)
	{
	case SDK::GhostState::Idle:
		ghostStateString = "Idle";
		break;
	case SDK::GhostState::Wander:
		ghostStateString = "Wander";
		break;
	case SDK::GhostState::Hunting:
		ghostStateString = "Hunting";
		break;
	case SDK::GhostState::FavoriteRoom:
		ghostStateString = "Favorite Room";
		break;
	case SDK::GhostState::Light:
		ghostStateString = "Light";
		break;
	case SDK::GhostState::Door:
		ghostStateString = "Door";
		break;
	case SDK::GhostState::Throwing:
		ghostStateString = "Throwing";
		break;
	case SDK::GhostState::FuseBox:
		ghostStateString = "Fuse Box";
		break;
	case SDK::GhostState::Appear:
		ghostStateString = "Appear";
		break;
	case SDK::GhostState::DoorKnock:
		ghostStateString = "Door Knock";
		break;
	case SDK::GhostState::WindowKnock:
		ghostStateString = "Window Knock";
		break;
	case SDK::GhostState::CarAlarm:
		ghostStateString = "Car Alarm";
		break;
	case SDK::GhostState::Flicker:
		ghostStateString = "Flicker";
		break;
	case SDK::GhostState::CCTV:
		ghostStateString = "CCTV";
		break;
	case SDK::GhostState::RandomEvent:
		ghostStateString = "Random Event";
		break;
	case SDK::GhostState::GhostAbility:
		ghostStateString = "Ghost Ability";
		break;
	case SDK::GhostState::Mannequin:
		ghostStateString = "Mannequin";
		break;
	case SDK::GhostState::TeleportObject:
		ghostStateString = "Teleport Object";
		break;
	case SDK::GhostState::Interact:
		ghostStateString = "Interact";
		break;
	case SDK::GhostState::SummoningCircle:
		ghostStateString = "Summoning Circle";
		break;
	case SDK::GhostState::MusicBox:
		ghostStateString = "Music Box";
		break;
	case SDK::GhostState::Dots:
		ghostStateString = "Dots";
		break;
	case SDK::GhostState::Salt:
		ghostStateString = "Salt";
		break;
	default:
		ghostStateString = "Unknown";
		break;
	}

	return ghostStateString;
}

std::string Utils::GhostEnumToStr(SDK::GhostType ghostType)
{
	std::string ghostTypeString;

	switch (ghostType)
	{
	case SDK::GhostType::Spirit:
		ghostTypeString = "Spirit";
		break;
	case SDK::GhostType::Wraith:
		ghostTypeString = "Wraith";
		break;
	case SDK::GhostType::Phantom:
		ghostTypeString = "Phantom";
		break;
	case SDK::GhostType::Poltergeist:
		ghostTypeString = "Poltergeist";
		break;
	case SDK::GhostType::Banshee:
		ghostTypeString = "Banshee";
		break;
	case SDK::GhostType::Jinn:
		ghostTypeString = "Jinn";
		break;
	case SDK::GhostType::Mare:
		ghostTypeString = "Mare";
		break;
	case SDK::GhostType::Revenant:
		ghostTypeString = "Revenant";
		break;
	case SDK::GhostType::Shade:
		ghostTypeString = "Shade";
		break;
	case SDK::GhostType::Demon:
		ghostTypeString = "Demon";
		break;
	case SDK::GhostType::Yurei:
		ghostTypeString = "Yurei";
		break;
	case SDK::GhostType::Oni:
		ghostTypeString = "Oni";
		break;
	case SDK::GhostType::Yokai:
		ghostTypeString = "Yokai";
		break;
	case SDK::GhostType::Hantu:
		ghostTypeString = "Hantu";
		break;
	case SDK::GhostType::Goryo:
		ghostTypeString = "Goryo";
		break;
	case SDK::GhostType::Myling:
		ghostTypeString = "Myling";
		break;
	case SDK::GhostType::Onryo:
		ghostTypeString = "Onryo";
		break;
	case SDK::GhostType::TheTwins:
		ghostTypeString = "The Twins";
		break;
	case SDK::GhostType::Raiju:
		ghostTypeString = "Raiju";
		break;
	case SDK::GhostType::Obake:
		ghostTypeString = "Obake";
		break;
	case SDK::GhostType::Mimic:
		ghostTypeString = "The Mimic";
		break;
	case SDK::GhostType::Moroi:
		ghostTypeString = "Moroi";
		break;
	case SDK::GhostType::Deogen:
		ghostTypeString = "Deogen";
		break;
	case SDK::GhostType::Thaye:
		ghostTypeString = "Thaye";
		break;
	case SDK::GhostType::Gallu:
		ghostTypeString = "Gallu";
		break;
	case SDK::GhostType::Dayan:
		ghostTypeString = "Dayan";
		break;
	case SDK::GhostType::Obambo:
		ghostTypeString = "Obambo";
		break;
	case SDK::GhostType::Kormos:
		ghostTypeString = "Kormos";
		break;
	case SDK::GhostType::Aswang:
		ghostTypeString = "Aswang";
		break;
	case SDK::GhostType::Deildegast:
		ghostTypeString = "Deildegast";
		break;
	default:
		ghostTypeString = "Unknown";
		break;
	}

	return ghostTypeString;
}

std::string Utils::GhostEnumToStr(const SDK::GhostEvidence ghostEvidence)
{
	std::string evidenceTypeString;

	switch (ghostEvidence)
	{
	case SDK::GhostEvidence::None:
		evidenceTypeString = "None";
		break;
	case SDK::GhostEvidence::EMF:
		evidenceTypeString = "EMF Level 5";
		break;
	case SDK::GhostEvidence::SpiritBox:
		evidenceTypeString = "Spirit Box";
		break;
	case SDK::GhostEvidence::Ultraviolet:
		evidenceTypeString = "Ultraviolet";
		break;
	case SDK::GhostEvidence::GhostOrb:
		evidenceTypeString = "Ghost Orb";
		break;
	case SDK::GhostEvidence::GhostWritingBook:
		evidenceTypeString = "Ghost Writing";
		break;
	case SDK::GhostEvidence::Temperature:
		evidenceTypeString = "Freezing Temperature";
		break;
	case SDK::GhostEvidence::DotsProjector:
		evidenceTypeString = "D.O.T.S. Projector";
		break;
	default:
		evidenceTypeString = "Unknown";
		break;
	}

	return evidenceTypeString;
}

std::string Utils::GhostEnumToStrLocalized(SDK::GhostType type)
{
	switch (type)
	{
	case SDK::GhostType::Spirit:
		return LANG("GhostType_Spirit");

	case SDK::GhostType::Wraith:
		return LANG("GhostType_Wraith");

	case SDK::GhostType::Phantom:
		return LANG("GhostType_Phantom");

	case SDK::GhostType::Poltergeist:
		return LANG("GhostType_Poltergeist");

	case SDK::GhostType::Banshee:
		return LANG("GhostType_Banshee");

	case SDK::GhostType::Jinn:
		return LANG("GhostType_Jinn");

	case SDK::GhostType::Mare:
		return LANG("GhostType_Mare");

	case SDK::GhostType::Revenant:
		return LANG("GhostType_Revenant");

	case SDK::GhostType::Shade:
		return LANG("GhostType_Shade");

	case SDK::GhostType::Demon:
		return LANG("GhostType_Demon");

	case SDK::GhostType::Yurei:
		return LANG("GhostType_Yurei");

	case SDK::GhostType::Oni:
		return LANG("GhostType_Oni");

	case SDK::GhostType::Hantu:
		return LANG("GhostType_Hantu");

	case SDK::GhostType::Yokai:
		return LANG("GhostType_Yokai");

	case SDK::GhostType::Goryo:
		return LANG("GhostType_Goryo");

	case SDK::GhostType::Myling:
		return LANG("GhostType_Myling");

	case SDK::GhostType::Onryo:
		return LANG("GhostType_Onryo");

	case SDK::GhostType::TheTwins:
		return LANG("GhostType_TheTwins");

	case SDK::GhostType::Raiju:
		return LANG("GhostType_Raiju");

	case SDK::GhostType::Obake:
		return LANG("GhostType_Obake");

	case SDK::GhostType::Mimic:
		return LANG("GhostType_Mimic");

	case SDK::GhostType::Moroi:
		return LANG("GhostType_Moroi");

	case SDK::GhostType::Deogen:
		return LANG("GhostType_Deogen");

	case SDK::GhostType::Thaye:
		return LANG("GhostType_Thaye");

	case SDK::GhostType::Gallu:
		return LANG("GhostType_Gallu");

	case SDK::GhostType::Dayan:
		return LANG("GhostType_Dayan");

	case SDK::GhostType::Obambo:
		return LANG("GhostType_Obambo");

	case SDK::GhostType::Kormos:
		return LANG("GhostType_Kormos");

	case SDK::GhostType::Aswang:
		return LANG("GhostType_Aswang");

	default:
		return LANG("GhostType_Default");
	}
}

std::string Utils::GhostEnumToStrLocalized(SDK::GhostEvidence GhostEved)
{
	switch (GhostEved)
	{
	case SDK::GhostEvidence::EMF:
		return LANG("Evidence_EMF5");

	case SDK::GhostEvidence::DotsProjector:
		return LANG("Evidence_DOTS");

	case SDK::GhostEvidence::Ultraviolet:
		return LANG("Evidence_Ultraviolet");

	case SDK::GhostEvidence::GhostWritingBook:
		return LANG("Evidence_Writing");

	case SDK::GhostEvidence::Temperature:
		return LANG("Evidence_Freezing");

	case SDK::GhostEvidence::SpiritBox:
		return LANG("Evidence_SpiritBox");

	case SDK::GhostEvidence::GhostOrb:
		return LANG("Evidence_Orbs");

	default:
		return "Unknown";
	}
}

std::string Utils::UnityStrToSysStr(const SDK::String& string)
{
	if (!string.Clazz) return "UNKOWN";

	int length = string.Fields.StringLength;

	if (length <= 0)
	{
		return "UNKNOWN";
	}

	const uint16_t* chars = &string.Fields.FirstChar;

	std::wstring wideStr(chars, chars + length);
	std::string result;
	result.reserve(length);

	for (wchar_t wc : wideStr)
	{
		if (wc < 0x80)
			result.push_back(static_cast<char>(wc));
		else
		{
			if (wc < 0x800)
			{
				result.push_back(static_cast<char>(0xC0 | (wc >> 6)));
				result.push_back(static_cast<char>(0x80 | (wc & 0x3F)));
			}
			else
			{
				result.push_back(static_cast<char>(0xE0 | (wc >> 12)));
				result.push_back(static_cast<char>(0x80 | ((wc >> 6) & 0x3F)));
				result.push_back(static_cast<char>(0x80 | (wc & 0x3F)));
			}
		}
	}

	return result;
}

SDK::String* Utils::SysStrToUnityStr(const std::string& string)
{
	if (!il2cpp_initialize()) return nullptr;

	Il2CppDomain* domain = il2cpp_domain_get();
	if (!domain) return nullptr;

	il2cpp_thread_attach(domain);

	int len = MultiByteToWideChar(CP_UTF8, 0, string.c_str(), (int)string.size(), nullptr, 0);
	if (len == 0) return nullptr;

	std::wstring wstr(len, 0);
	MultiByteToWideChar(CP_UTF8, 0, string.c_str(), (int)string.size(), &wstr[0], len);

	Il2CppString* il2cppStr = il2cpp_string_new_utf16_wrap((const wchar_t*)wstr.c_str(), len);
	return reinterpret_cast<SDK::String*>(il2cppStr);
}

std::string Utils::GetPlayerName(const SDK::Player* player)
{
	if (!player)
		return {};

	try
	{
		const auto networkPlayerSpot = GetNetworkPlayerSpot(player);
		if (!networkPlayerSpot)
			return {};

		const auto& accountName = networkPlayerSpot->Fields.AccountName;
		if (!accountName)
			return {};

		return UnityStrToSysStr(*accountName);
	}
	catch (const std::exception& e)
	{
		LOG_ERROR("Failed to get player name: {}", e.what());
		return {};
	}
}

bool Utils::WTS(const SDK::Vector3& worldPos, SDK::Vector3& displayPos)
{
	displayPos = SDK::Vector3{};

	auto* player = GetLocalPlayer();
	if (!player || !player->Fields.LocalPlayer)
		return false;

	SDK::Camera* camera = player->Fields.LocalPlayer->Fields.Camera;
	if (!camera)
		return false;

	SDK::Vector3 projected = SDK::Camera_WorldToScreenPoint(camera, worldPos, nullptr);

	if (projected.Z <= 0.0f)
		return false;

	float screenHeight = SDK::Screen_Get_Height(nullptr);
	if (screenHeight <= 0.0f)
		return false;

	displayPos.X = projected.X;
	displayPos.Y = screenHeight - projected.Y;
	displayPos.Z = projected.Z;

	return true;
}

SDK::Transform* Utils::GetPlayerTransformCamera(const SDK::Player* player)
{
	if (!player) return nullptr;
	return SDK::Component_Get_Transform(reinterpret_cast<SDK::Component*>(player->Fields.LocalPlayer->Fields.Camera), nullptr);
}

SDK::Vector3 Utils::GetPosVec3(const SDK::Player* player)
{
	if (!player) return { 0, 0, 0 };
	auto pos1 = GetPosVec3(GetPlayerTransformCamera(player));
	SDK::Vector3 newpos = { pos1.X, pos1.Y - 1.5f, pos1.Z };
	return newpos;
}

SDK::Vector3 Utils::GetPosVec3(const SDK::GhostAI* ghostAI)
{
	if (!ghostAI) return { 0, 0, 0 };
	return GetPosVec3(ghostAI->Fields.huntingRaycastPoint);
}

SDK::Vector3 Utils::GetPosVec3(SDK::Component* component)
{
	if (!component) return { 0, 0, 0 };
	return GetPosVec3(SDK::Component_Get_Transform(component, nullptr));
}

SDK::Vector3 Utils::GetPosVec3(SDK::Transform* transform)
{
	if (!transform) return { 0, 0, 0 };
	return SDK::Transform_Get_Position(transform, nullptr);
}

std::string Utils::GhostEnumToStr(SDK::EvidenceType evidenceType)
{
	switch (evidenceType)
	{
	case SDK::EvidenceType::emfSpot:
		return "EMF Spot";
	case SDK::EvidenceType::ouijaBoard:
		return "Ouija Board";
	case SDK::EvidenceType::fingerprint:
		return "Fingerprint";
	case SDK::EvidenceType::footstep:
		return "Footstep";
	case SDK::EvidenceType::DNA:
		return "";
	case SDK::EvidenceType::ghost:
		return "Ghost";
	case SDK::EvidenceType::deadBody:
		return "Dead Body";
	case SDK::EvidenceType::dirtyWater:
		return "Dirty Water";
	case SDK::EvidenceType::musicBox:
		return "Music Box";
	case SDK::EvidenceType::tarotCards:
		return "Tarot Cards";
	case SDK::EvidenceType::summoningCicle:
		return "Summoning Cicle";
	case SDK::EvidenceType::hauntedMirror:
		return "Haunted Mirror";
	case SDK::EvidenceType::voodooDoll:
		return "Voodoo Doll";
	case SDK::EvidenceType::ghostWriting:
		return "Ghost Writing";
	case SDK::EvidenceType::usedCrucifix:
		return "Used Crucifix";
	case SDK::EvidenceType::dotsGhost:
		return "DOTS Ghost";
	case SDK::EvidenceType::monkeyPaw:
		return "Monkey Paw";
	case SDK::EvidenceType::moonAlter:
		return "Moon Alter";
	case SDK::EvidenceType::ghostOrb:
		return "Ghost Orb";
	case SDK::EvidenceType::lightSource:
		return "Light Source";
	case SDK::EvidenceType::none:
		return "None";
	case SDK::EvidenceType::emfReader:
		return "EMF Reader";
	case SDK::EvidenceType::salt:
		return "Salt";
	case SDK::EvidenceType::freezingTemperature:
		return "Freezing Temperature";
	case SDK::EvidenceType::ghostSmoke:
		return "Ghost Smoke";
	case SDK::EvidenceType::camoGhost:
		return "Camo Ghost";
	case SDK::EvidenceType::paranormalSound:
		return "Paranormal Sound";
	case SDK::EvidenceType::spiritBox:
		return "Spirit Box";
	case SDK::EvidenceType::ghostHunt:
		return "Ghost Hunt";
	case SDK::EvidenceType::bansheeWail:
		return "Banshee Wail";
	case SDK::EvidenceType::teddyBear:
		return "Teddy Bear";
	case SDK::EvidenceType::spiritBoxBreath:
		return "Spirit Box Breath";
	case SDK::EvidenceType::motionSensor:
		return "Motion Sensor";
	case SDK::EvidenceType::shadowGhost:
		return "Shadow Ghost";
	case SDK::EvidenceType::ghostGroan:
		return "Ghost Groan";
	case SDK::EvidenceType::ghostLaugh:
		return "Ghost Laugh";
	case SDK::EvidenceType::ghostTalk:
		return "Ghost Talk";
	case SDK::EvidenceType::ghostWhisper:
		return "Ghost Whisper";
	case SDK::EvidenceType::obakeFingerprint:
		return "Obake Fingerprint";
	case SDK::EvidenceType::burningChapelCrucifix:
		return "Burning Chapel Crucifix";
	case SDK::EvidenceType::obakeShapeshift:
		return "Obake Shapeshift";
	default:
		return "Unknown";
	}
}

bool Utils::IsCursedItem(SDK::EvidenceType evidenceType)
{
	switch (evidenceType)
	{
	case SDK::EvidenceType::voodooDoll:
	case SDK::EvidenceType::tarotCards:
	case SDK::EvidenceType::ouijaBoard:
	case SDK::EvidenceType::hauntedMirror:
	case SDK::EvidenceType::musicBox:
	case SDK::EvidenceType::summoningCicle:
	case SDK::EvidenceType::monkeyPaw:
		return true;
	default:
		return false;
	}
}

SDK::LevelStats* Utils::GetLevelStats() 
{
	return SDK::LevelStats_get_Instance(nullptr);
}

double Utils::GetCheatUptimeSeconds()
{
	auto now = std::chrono::steady_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(now - CheatInjectedTime);
	return duration.count();
}

float Utils::toSanityOrInsanity(float value)
{
	return 100.0f - value;
}
/*
errors code:
-1: player is null
-2: playerSanity is null
-3: sanity less than 0
*/
float Utils::GetPlayerSanity(SDK::Player* player)
{
	if (!player)
		return -1;

	auto playerSanity = player->Fields.PlayerSanity;

	if (!playerSanity)
		return -2;

	auto sanity = toSanityOrInsanity(playerSanity->Fields.insanity);

	if (sanity < 0)
		return -3;

	return sanity;
}

bool Utils::IsLocalMasterClient()
{
	return SDK::PhotonNetwork_Get_IsMasterClient(nullptr);
}

void Utils::TpPlayerToVec3(SDK::Player* player, const SDK::Vector3& position)
{
	if (!player) return;
	auto* localPlayer = player->Fields.LocalPlayer;
	if (!localPlayer) return;
	SDK::LocalPlayer_TeleportPlayer(localPlayer, position, nullptr);
}

void Utils::TpPlayerToPlayer(SDK::Player* player, const SDK::Player* twoplayer)
{
	if (!player || !twoplayer)
		return;
	TpPlayerToVec3(player, GetPosVec3(twoplayer));
}

SDK::LevelValues* Utils::GetLevelValues()
{
	return SDK::LevelValues_get_Instance(nullptr);
}

SDK::Map* Utils::GetMap()
{
	return SDK::LevelValues_get_map(GetLevelValues(), nullptr);
}

std::string Utils::GetActiveSceneName()
{
	SDK::Scene scene = SDK::SceneManager_GetActiveScene(nullptr);
	SDK::String* namePtr = SDK::Scene_get_name(&scene, nullptr);
	if (!namePtr)
		return "UNKNOWN";
	return UnityStrToSysStr(*namePtr);
}

SDK::GameObject* Utils::FindObjectByName(std::string name)
{
	return GameObject_Find(name);
}

SDK::ObjectArray* Utils::FindObjectsOfType(std::string type)
{
	auto* goType = SDK::System_Type_GetType(SysStrToUnityStr(type), nullptr);
	if (!goType) return nullptr;

	auto* all = SDK::Resources_FindObjectsOfTypeAll(goType, nullptr);
	if (!all) return nullptr;

	return all;
}

SDK::StoreItemInfo* Utils::GetStoreItemInfo()
{
	auto* list = FindObjectsOfType("UnityEngine.GameObject");
	if (!list) return nullptr;

	for (uint32_t i = 0; i < list->MaxLength; i++)
	{
		auto raw = list->Vector[i];
		if (!raw) continue;

		auto* go = reinterpret_cast<SDK::GameObject*>(raw);

		auto* nameUnity = SDK::Object_Get_Name(reinterpret_cast<SDK::Object*>(go), nullptr);
		if (!nameUnity) continue;

		auto name = UnityStrToSysStr(*nameUnity);
		if (name != "Inspect Panel")
			continue;

		auto comp = SDK::GameObject_GetComponentByName(
			go,
			SysStrToUnityStr("StoreItemInfo"),
			nullptr
		);

		if (comp)
			return reinterpret_cast<SDK::StoreItemInfo*>(comp);
	}

	return nullptr;
}

SDK::JournalController* Utils::GetMainMenuJournal()
{
	auto sceneName = GetActiveSceneName();
	LOG_INFO(sceneName);

	if (sceneName == "HideAndDontSave")
		return nullptr;

	auto* list = FindObjectsOfType("JournalController");
	if (!list) {
		LOG_INFO("JournalController list null");
		return nullptr;
	}

	for (uint32_t i = 0; i < 65535; i++)
	{
		auto raw = list->Vector[i];
		if (!raw) continue;

		auto* jc = reinterpret_cast<SDK::JournalController*>(raw);
		if (!jc) continue;

		LOG_INFO("Found JournalController");
		return jc;
	}

	LOG_INFO("None found");
	return nullptr;
}

SDK::PhotonMessageInfo* Utils::CreatePhotonMessageInfo(SDK::PhotonView* photonView)
{
	if (!il2cpp_initialize()) return nullptr;

	Il2CppDomain* domain = il2cpp_domain_get();
	if (!domain) return nullptr;

	il2cpp_thread_attach(domain);

	Il2CppClass* pmiClass =
		il2cpp_get_class("PhotonUnityNetworking", "Photon.Pun", "PhotonMessageInfo");
	if (!pmiClass) return nullptr;

	Il2CppObject* pmi = il2cpp_object_new_from_class(pmiClass);
	if (!pmi) return nullptr;

	auto* localPlayer = SDK::PhotonNetwork_Get_LocalPlayer(nullptr);
	int timestamp = SDK::PhotonNetwork_Get_ServerTimestamp(nullptr);

	void* ctorArgs[3] = { localPlayer, &timestamp, photonView };
	const MethodInfo_I* ctor = il2cpp_class_get_method_from_name_wrap(pmiClass, ".ctor", 3);
	if (ctor)
		il2cpp_runtime_invoke_wrap(ctor, pmi, ctorArgs, nullptr);

	return reinterpret_cast<SDK::PhotonMessageInfo*>(pmi);
}


SDK::ExitLevel* Utils::GetExitLevel()
{
	auto* list = Utils::FindObjectsOfType("ExitLevel");
	if (!list) return nullptr;

	for (uint32_t i = 0; i < 65535; i++)
	{
		auto* raw = list->Vector[i];
		if (!raw) continue;

		auto* exitLevel = reinterpret_cast<SDK::ExitLevel*>(raw);
		if (!exitLevel) continue;

		return exitLevel;
	}

	return nullptr;
}

SDK::ServerManager* Utils::GetServerManager()
{
	auto* gameLobbyObj = FindObjectByName("Game Lobby");
	if (!gameLobbyObj) return nullptr;

	auto* type = SDK::System_Type_GetType(SysStrToUnityStr("ServerManager"), nullptr);
	if (!type) return nullptr;

	auto* serverManagerComp = reinterpret_cast<SDK::ServerManager*>(SDK::GameObject_GetComponent(gameLobbyObj, type, nullptr));
	if (!serverManagerComp) return nullptr;

	return serverManagerComp;
}

void Utils::SetTagOnGObject(SDK::GameObject* gameObject, const std::string& tag)
{
	auto unityStrTag = SysStrToUnityStr(tag);
	SDK::GameObject_set_tag(gameObject, unityStrTag, nullptr);
}

std::string Utils::GetTagOnGObject(SDK::GameObject* gameObject)
{
	auto unityStrTag = SDK::GameObject_get_tag(gameObject, nullptr);
	if (!unityStrTag)
		return "UNKNOWN";
	return UnityStrToSysStr(*unityStrTag);
}

bool Utils::CheckIsValidObjectByTag(SDK::GameObject* gameObject, const std::string& tag)
{
	if (!gameObject) return false;

	auto* unityTag = SysStrToUnityStr(tag);
	auto* result = SDK::GameObject_FindGameObjectsWithTag(unityTag, nullptr);
	if (!result) return false;

	for (uint32_t i = 0; i < 65535; i++)
	{
		SDK::GameObject* obj = result->Vector[i];
		if (!obj) break;
		if (obj == gameObject) return true;
	}

	return false;
}

// I'm lazy and I didn't want to use the Unity method. Yes, you can do it, but who cares :)
void Utils::GetComponentsInChildren(SDK::GameObject* root, const char* componentName, std::vector<SDK::Component*>& outComponents, bool includeInactive)
{
	SDK::Transform* rootTransform = SDK::GameObject_get_transform(root, nullptr);

	std::function<void(SDK::Transform*)> Traverse = [&](SDK::Transform* t)
		{
			SDK::GameObject* go = SDK::Component_Get_GameObject((SDK::Component*)t, nullptr);

			if (includeInactive || SDK::GameObject_get_activeSelf(go, nullptr))
			{
				SDK::Component* comp = SDK::GameObject_GetComponentByName(go, SysStrToUnityStr(componentName), nullptr);
				if (comp != nullptr)
				{
					outComponents.push_back(comp);
					LOG_INFO("Found component on object: ", UnityStrToSysStr(*SDK::Object_Get_Name((SDK::Object*)(go), nullptr)));
				}
			}

			int childCount = SDK::Transform_get_childCount(t, nullptr);
			for (int i = 0; i < childCount; i++)
			{
				SDK::Transform* child = SDK::Transform_GetChild(t, i, nullptr);
				Traverse(child);
			}
		};

	Traverse(rootTransform);
}

// Only for fun
void Utils::DumpAllMethodsByClass(const char* className, const char* namespaze, const char* assemblyName)
{
	if (!il2cpp_initialize())
	{
		LOG_ERROR("[IL2CPP] init failed");
		return;
	}

	Il2CppDomain* domain = il2cpp_domain_get();
	if (!domain)
	{
		LOG_ERROR("[IL2CPP] domain null");
		return;
	}

	Il2CppAssembly* assembly = il2cpp_domain_assembly_open_wrap(domain, assemblyName);
	if (!assembly)
	{
		LOG_ERROR("[IL2CPP] assembly not found: ", assemblyName);
		return;
	}

	Il2CppImage* image = il2cpp_assembly_get_image_wrap(assembly);
	if (!image)
	{
		LOG_ERROR("[IL2CPP] image null");
		return;
	}

	Il2CppClass* klass = il2cpp_class_from_name_wrap(image, namespaze, className);
	if (!klass)
	{
		LOG_ERROR("[IL2CPP] class not found: ", namespaze, className);
		return;
	}

	LOG_INFO("[IL2CPP] \n",
		(namespaze && namespaze[0]) ? namespaze : "<global>",
		className,
		assemblyName
	);

	void* iter = nullptr;
	MethodInfo_I* method = nullptr;

	while ((method = il2cpp_class_get_methods_wrap(klass, &iter)))
	{
		LOG_INFO("  └─ \n",
			il2cpp_method_get_name_wrap2(method),
			method->parameters_count
		);
	}

	LOG_INFO("[IL2CPP] dump end\n\n");
}

SDK::HandCamera* Utils::get_PlayerHandCamera(SDK::Player* player)
{
	if (!player)
		return nullptr;

	static SDK::Type* handCameraType = nullptr;

	if (!handCameraType)
	{
		handCameraType =
			SDK::System_Type_GetType(
				SysStrToUnityStr("HandCamera"),
				nullptr);
	}

	if (!handCameraType)
		return nullptr;

	auto* playerGO =
		SDK::Component_Get_GameObject(
			reinterpret_cast<SDK::Component*>(player),
			nullptr);

	if (!playerGO)
		return nullptr;

	auto* playerTR =
		SDK::GameObject_get_transform(
			playerGO,
			nullptr);

	if (!playerTR)
		return nullptr;

	return FindComponentRecursive<SDK::HandCamera>(
		playerTR,
		handCameraType);
}

bool Utils::IsGhostVisible(SDK::GhostAI* ghostAI)
{
	if (!ghostAI)
		return false;

	auto* currentModel = ghostAI->Fields.currentModel;
	if (!currentModel)
		return false;

	auto* myRends = currentModel->Fields.myRends;
	if (!myRends)
		return false;

	for (uint32_t i = 0; i < 65535; i++)
	{
		SDK::Render* rend = myRends->Vector[i];
		if (!rend) break;

		if (SDK::Render_get_enabled(rend, nullptr))
			return true;
	}

	return false;
}


bool Utils::IsPlayerVisibleGhost(SDK::Camera* camera, SDK::Transform* ghostTransform)
{
	if (!camera || !ghostTransform)
	{
		return false;
	}

	SDK::Transform* camTr = SDK::Component_Get_Transform((SDK::Component*)camera, nullptr);
	SDK::Vector3 camPos = SDK::Transform_Get_Position(camTr, nullptr);
	SDK::Vector3 ghostPos = SDK::Transform_Get_Position(ghostTransform, nullptr);

	SDK::Vector3 dir = ghostPos - camPos;
	float dist = SDK::Vector3_get_magnitude(&dir, nullptr);

	if (dist < 0.05f)
	{
		return true;
	}

	dir = SDK::Vector3_get_normalized(&dir, nullptr);

	SDK::PhysicsScene scene = SDK::Physics_get_defaultPhysicsScene(nullptr);
	SDK::Vector3 currentOrigin = camPos + dir * 0.02f;
	float traveled = 0.f;
	int maxHits = 32;

	const int blockingLayers[] = { 12, 13, 14, 21, 26, 28, 30 };

	for (int i = 0; i < maxHits && traveled < dist; i++)
	{
		SDK::RaycastHit hit{};
		bool hasHit = SDK::PhysicsScene_Raycast(
			&scene,
			currentOrigin,
			dir,
			&hit,
			dist - traveled,
			-1,
			SDK::QueryTriggerInteraction::Ignore,
			nullptr
		);

		if (!hasHit)
		{
			break;
		}

		int32_t colID = hit.Collider;
		if (!colID)
		{
			break;
		}

		SDK::Collider* col = reinterpret_cast<SDK::Collider*>(SDK::Object_FindObjectFromInstanceID(colID, nullptr));
		SDK::GameObject* go = col ? SDK::Component_Get_GameObject((SDK::Component*)col, nullptr) : nullptr;
		int hitLayer = go ? SDK::GameObject_Get_Layer(go, nullptr) : -1;

		for (int layer : blockingLayers)
		{
			if (hitLayer == layer)
			{
				return false;
			}
		}

		traveled += hit.Distance + 0.01f;
		currentOrigin = currentOrigin + dir * (hit.Distance + 0.01f);
	}

	SDK::Vector3 vp = SDK::Camera_WorldToViewportPoint(camera, ghostPos, nullptr);

	if (vp.Z <= 0.f)
	{
		return false;
	}

	if (vp.X < 0.f || vp.X > 1.f || vp.Y < 0.f || vp.Y > 1.f)
	{
		return false;
	}

	return true;
}

void Utils::DumpAllLayers()
{
	for (int i = 0; i < 32; i++)
	{
		SDK::String* name = SDK::LayerMask_LayerToName(i, nullptr);
		if (name)
		{
			std::string cstr = UnityStrToSysStr(*name);
			LOG_INFO("Layer ", i, cstr);
		}
		else
		{
			LOG_INFO("Empty layer ", i);
		}
	}
}

SDK::GameObject* Utils::GetPlayerCrosshairObj(SDK::Player* player)
{
	if (!player)
		return nullptr;

	SDK::GameObject* crosshairObj = FindObjectByName("CrosshairImage");

	if (crosshairObj)
		return crosshairObj;

	return nullptr;
}

SDK::GhostAI* Utils::GetGhostAI()
{
	auto levelController = SDK::LevelController_sFields->instance;
	if (!levelController)
		return nullptr;

	return levelController->Fields.ghostAI;
}

SDK::Transform* Utils::GetPotatoe()
{
	auto* Potatoe_Obj = FindObjectByName("Potatoe"); if (!Potatoe_Obj) return nullptr;
	auto* Potatoe_Trans = SDK::GameObject_get_transform(Potatoe_Obj, nullptr); if (!Potatoe_Trans) return nullptr;

	return Potatoe_Trans;
}

SDK::Type* Utils::GetType(std::string string)
{
	if (string.empty())
		return nullptr;

	return SDK::System_Type_GetType(SysStrToUnityStr(string), nullptr);
}

std::vector<std::tuple<std::string, SDK::GhostButton*, SDK::GameObject*>> Utils::get_AllGhostButtonsWithGO()
{
	// Journal2.0Canvas > Content > GhostHuntingBookContent > Evidence.2 > All Ghosts > GHOST NAME

	std::vector<std::tuple<std::string, SDK::GhostButton*, SDK::GameObject*>> result;

	auto* canvasGO = FindObjectByName("Journal2.0Canvas");
	if (!canvasGO)
		return result;

	auto* canvasTr = SDK::GameObject_get_transform(canvasGO, nullptr);
	if (!canvasTr)
		return result;

	SDK::Transform* contentTr = nullptr;
	int canvasChildCount = SDK::Transform_get_childCount(canvasTr, nullptr);
	for (int i = 0; i < canvasChildCount; ++i)
	{
		auto* child = SDK::Transform_GetChild(canvasTr, i, nullptr);
		auto* childGO = SDK::Component_Get_GameObject(reinterpret_cast<SDK::Component*>(child), nullptr);
		std::string name = UnityStrToSysStr(*SDK::Object_Get_Name(reinterpret_cast<SDK::Object*>(childGO), nullptr));

		if (name == "Content")
		{
			contentTr = child;
			break;
		}
	}
	if (!contentTr) return result;

	SDK::Transform* bookContentTr = nullptr;
	int contentChildCount = SDK::Transform_get_childCount(contentTr, nullptr);
	for (int i = 0; i < contentChildCount; ++i)
	{
		auto* child = SDK::Transform_GetChild(contentTr, i, nullptr);
		auto* childGO = SDK::Component_Get_GameObject(reinterpret_cast<SDK::Component*>(child), nullptr);
		std::string name = UnityStrToSysStr(*SDK::Object_Get_Name(reinterpret_cast<SDK::Object*>(childGO), nullptr));

		if (name == "GhostHuntingBookContent")
		{
			bookContentTr = child;
			break;
		}
	}
	if (!bookContentTr) return result;

	SDK::Transform* evidenceTr = nullptr;
	int bookChildCount = SDK::Transform_get_childCount(bookContentTr, nullptr);
	for (int i = 0; i < bookChildCount; ++i)
	{
		auto* child = SDK::Transform_GetChild(bookContentTr, i, nullptr);
		auto* childGO = SDK::Component_Get_GameObject(reinterpret_cast<SDK::Component*>(child), nullptr);
		std::string name = UnityStrToSysStr(*SDK::Object_Get_Name(reinterpret_cast<SDK::Object*>(childGO), nullptr));

		if (name == "Evidence.2")
		{
			evidenceTr = child;
			break;
		}
	}
	if (!evidenceTr) return result;

	SDK::Transform* allGhostsTr = nullptr;
	int evidenceChildCount = SDK::Transform_get_childCount(evidenceTr, nullptr);
	for (int i = 0; i < evidenceChildCount; ++i)
	{
		auto* child = SDK::Transform_GetChild(evidenceTr, i, nullptr);
		auto* childGO = SDK::Component_Get_GameObject(reinterpret_cast<SDK::Component*>(child), nullptr);
		std::string name = UnityStrToSysStr(*SDK::Object_Get_Name(reinterpret_cast<SDK::Object*>(childGO), nullptr));

		if (name == "All Ghosts")
		{
			allGhostsTr = child;
			break;
		}
	}
	if (!allGhostsTr) return result;

	int ghostCount = SDK::Transform_get_childCount(allGhostsTr, nullptr);
	for (int i = 0; i < ghostCount; ++i)
	{
		auto* ghostTr = SDK::Transform_GetChild(allGhostsTr, i, nullptr);
		auto* ghostGO = SDK::Component_Get_GameObject(reinterpret_cast<SDK::Component*>(ghostTr), nullptr);
		std::string ghostName = UnityStrToSysStr(*SDK::Object_Get_Name(reinterpret_cast<SDK::Object*>(ghostGO), nullptr));

		if (ghostName.find("New Ghost") != std::string::npos)
			continue;

		SDK::Type* ghostButtonType = SDK::System_Type_GetType(SysStrToUnityStr("GhostButton"), nullptr);
		auto* ghostButton = reinterpret_cast<SDK::GhostButton*>(SDK::GameObject_GetComponent(ghostGO, ghostButtonType, nullptr));

		if (ghostButton)
			result.emplace_back(ghostName, ghostButton, ghostGO);
	}

	return result;
}

std::string Utils::GetGameVersion()
{
	if (!SDK::Application_get_version)
		return "UNKNOWN";
	auto* u_string = SDK::Application_get_version(nullptr);
	auto version = UnityStrToSysStr(*u_string);
	return version;
}

std::string Utils::GetUnityVersion()
{
	if (!SDK::Application_get_unityVersion)
		return "UNKNOWN";
	auto* u_string = SDK::Application_get_unityVersion(nullptr);
	auto version = UnityStrToSysStr(*u_string);
	return version;
}

std::string Utils::GetPlayerName()
{
	if (!SDK::SteamFriends_GetPersonalName || !SDK::SteamAPI_Init || !SDK::SteamAPI_Init(nullptr))
		return "Anon";
	auto* name = SDK::SteamFriends_GetPersonalName(nullptr);
	if (!name) return "Anon";
	auto str_name = Utils::UnityStrToSysStr(*name);
	return str_name;
}

bool Utils::Checks_IsRealSender(SDK::Player* pn_sender, SDK::PhotonView* view)
{
	if (pn_sender == nullptr || view == nullptr)
		return 0; // In game code return true

	if (pn_sender != reinterpret_cast<SDK::Player*>(SDK::PhotonView_get_Owner(view, nullptr)))
		return 0;

	if (!SDK::PhotonNetwork_Get_InRoom(nullptr))
		return 1;

	if (SDK::PhotonNetwork_Get_OfflineMode(nullptr))
		return 1;
}

float Utils::Distance(const SDK::Vector3& a, const SDK::Vector3& b)
{
	float dx = a.X - b.X;
	float dy = a.Y - b.Y;
	float dz = a.Z - b.Z;

	return std::sqrt(dx * dx + dy * dy + dz * dz);
}

bool Utils::IsUniquePhoto(SDK::EvidenceType mediaType)
{
	if (mediaType == SDK::EvidenceType::none) return false;

	auto* go_Journal = FindObjectByName("Journal2.0Canvas");
	if (!go_Journal) return false;

	auto* c_journal = reinterpret_cast<SDK::Journal_PhotoPage*>(SDK::GameObject_GetComponentByName(go_Journal, SysStrToUnityStr("Journal_PhotoPage"), nullptr));
	if (!c_journal) return false;
	auto* c_journalArray = c_journal->Fields.photosArray;
	if (!c_journalArray) return false;

	for (uint32_t i = 0; i < c_journalArray->max_length; i++)
	{
		auto* journalMedia = c_journalArray->vector[i];
		if (!journalMedia || !journalMedia->Fields.media) continue;
		auto* media = journalMedia->Fields.media;
		if (!media) continue;
		auto* mediaValue = media->Fields.value;
		if (!mediaValue) continue;
		auto eva_Type = mediaValue->Fields.EvidenceType;

		if (eva_Type == mediaType) return false;
	}

	return true;
}

bool Utils::TakePhoto(SDK::HandCamera* handCamera)
{
	if (!handCamera) return false;

	auto* g_obj_camera = SDK::Component_Get_GameObject(reinterpret_cast<SDK::Component*>(handCamera), nullptr);
	if (!g_obj_camera) return false;

	bool IsActiveCamera = SDK::GameObject_get_activeSelf(g_obj_camera, nullptr);
	if (!IsActiveCamera) return false;

	auto photonViewType = SDK::System_Type_GetType(
		SysStrToUnityStr("Photon.Pun.PhotonView"), nullptr);
	if (!photonViewType) return false;

	auto* photonViewComp = SDK::GameObject_GetComponent(
		g_obj_camera, photonViewType, nullptr);
	if (!photonViewComp) return false;

	auto* photonView = reinterpret_cast<SDK::PhotonView*>(photonViewComp);
	if (!photonView) return false;

	bool state = true;
	auto boolClass = il2cpp_get_class("mscorlib", "System", "Boolean");
	if (!boolClass) return false;

	void* boxedBool = il2cpp_value_box(boolClass, &state);
	if (!boxedBool) return false;

	std::vector<void*> params{ boxedBool };

	auto parameters = VectorToIl2CppArray<void*>(
		params, "mscorlib", "System", "Object");
	if (!parameters) return false;

	auto* methodName = SysStrToUnityStr("UseNetworked");
	if (!methodName) return false;

	SDK::PhotonView_RPC(
		photonView,
		methodName,
		SDK::RpcTarget::All,
		parameters,
		nullptr
	);

	return true;
}

SDK::Transform* Utils::FindChildByName(SDK::Transform * parent, const char* name)
{
	if (!parent) return nullptr;

	int count = SDK::Transform_get_childCount(parent, nullptr);

	for (int i = 0; i < count; i++)
	{
		auto* child = SDK::Transform_GetChild(parent, i, nullptr);
		if (!child) continue;

		auto* go = SDK::Component_Get_GameObject(reinterpret_cast<SDK::Component*>(child), nullptr);
		if (!go) continue;

		auto* goName = SDK::Object_Get_Name(reinterpret_cast<SDK::Object*>(go), nullptr);
		if (goName && UnityStrToSysStr(*goName) == name)
			return child;

		auto* deep = FindChildByName(child, name);
		if (deep)
			return deep;
	}

	return nullptr;
}

bool Utils::IsValidGO(SDK::GameObject* obj)
{
	return obj && SDK::Object_IsNativeAlive(reinterpret_cast<SDK::Object*>(obj), nullptr);
}

SDK::GameObject* Utils::GameObject_Find(std::string name)
{
	{
		std::lock_guard<std::mutex> lock(InGame::g_GameObjectCacheMutex);

		auto it = InGame::g_GameObjectCache.find(name);
		if (it != InGame::g_GameObjectCache.end())
		{
			if (IsValidGO(it->second))
				return it->second;

			InGame::g_GameObjectCache.erase(it);
		}
	}

	auto objects = SDK::Object_FindObjectsOfType(
		GetType("UnityEngine.GameObject"), nullptr);

	if (!objects)
		return nullptr;

	SDK::GameObject* found = nullptr;

	for (uint32_t i = 0; i < objects->MaxLength; i++)
	{
		SDK::Object* obj = objects->Vector[i];
		if (!obj)
			continue;

		auto go = reinterpret_cast<SDK::GameObject*>(obj);

		if (!IsValidGO(go))
			continue;

		auto objName =
			SDK::Object_Get_Name(reinterpret_cast<SDK::Object*>(go), nullptr);

		if (!objName)
			continue;

		std::string objStr = Utils::UnityStrToSysStr(*objName);

		{
			std::lock_guard<std::mutex> lock(InGame::g_GameObjectCacheMutex);
			InGame::g_GameObjectCache[objStr] = go;
		}

		if (objStr == name)
			found = go;
	}

	return found;
}

bool Utils::IsInGame()
{
	bool inGame = 
		GetActiveSceneName() != "Main_Menu None" &&
		SDK::GameController_StaticFields->instance && 
		SDK::GameController_StaticFields->instance->Fields.allPlayersAreConnected &&
		Utils::GetGhostAI() &&
		Utils::GetGhostAI()->Fields.GhostInfo &&
		Utils::GetGhostAI()->Fields.GhostInfo->Fields.GhostTraits.Name;
	
	return inGame;
}

SDK::Component* Utils::FindComponentByType(std::string type, bool retFirstComponent)
{
	SDK::Component* retComponent = nullptr;

	auto* SDK_type = GetType(type);

	auto* list = SDK::Object_FindObjectsOfType(SDK_type, nullptr);
	if (!list) return retComponent;

	for (int i = 0; i < list->MaxLength; i++)
	{
		auto* tmp = reinterpret_cast<SDK::Component*>(list->Vector[i]);
		if (!tmp) continue;

		retComponent = tmp;
		if (retFirstComponent)
			break;
	}

	return retComponent;
}

bool Utils::IsObjectInFront(const SDK::Vector3& targetPos, 
	const SDK::Vector3& referencePos, 
	const SDK::Vector3& referenceForward)
{
	SDK::Vector3 dir = targetPos - referencePos;

	float len = SDK::Vector3_get_magnitude(&dir, nullptr);
	if (len <= 0.0001f)
		return false;

	dir = SDK::Vector3_get_normalized(&dir, nullptr);

	float dot =
		dir.X * referenceForward.X +
		dir.Y * referenceForward.Y +
		dir.Z * referenceForward.Z;

	return dot > 0.0f;
}

int32_t Utils::GetGhostTypeInt()
{
	if (!GetGhostAI())
		return -1;

	if (!GetGhostAI()->Fields.GhostInfo)
		return -1;

	return static_cast<int32_t>(GetGhostAI()->Fields.GhostInfo->Fields.GhostTraits.GhostType_);
}