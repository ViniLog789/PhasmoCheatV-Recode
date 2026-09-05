#include "discordrpc.h"
#include <thread>

namespace Discord
{
    bool Enabled = true;
    static bool initialized = false;
    static time_t startTime = 0;

    static void HandleReady(const DiscordUser* user)
    {
        LOG_INFO("Discord RPC connected as " + std::string(user->username));
    }

    static void HandleError(int errcode, const char* message)
    {
        LOG_ERROR("Discord RPC Error: " + std::string(message));
    }

    static void HandleDisconnected(int errcode, const char* message)
    {
        LOG_WARN("Discord RPC Disconnected");
    }

    void Initialize()
    {
        if (initialized) return;

        DiscordEventHandlers handlers{};
        handlers.ready = HandleReady;
        handlers.errored = HandleError;
        handlers.disconnected = HandleDisconnected;

        Discord_Initialize("1500731448615895090", &handlers, 1, nullptr);

        startTime = time(nullptr);

        initialized = true;
    }

    void Shutdown()
    {
        if (initialized)
        {
            Discord_Shutdown();
            initialized = false;
        }
    }

    void UpdatePresence()
    {
        if (!Enabled || !initialized) return;

        DiscordRichPresence rp{};
        memset(&rp, 0, sizeof(rp));

        static std::string state;
        state = std::string(Globals::statusRPC)
            + " | "
            + Globals::GetVersion()
            + (Globals::IsBeta ? " • beta" : "");

        rp.state = state.c_str();
        rp.details = "By VCom Team";
        rp.largeImageKey = "main1";
        rp.smallImageKey = "mini1";
        rp.largeImageText = "PhasmoCheatV";
        rp.startTimestamp = startTime;

        Discord_UpdatePresence(&rp);
    }

    void DiscordRPCStatusHandler()
    {
        std::string name = Utils::GetPlayerName();
		std::string ghostType = "Unknown";
        if (Utils::GetGhostAI() && Utils::GetGhostAI()->Fields.GhostInfo)
		    ghostType = Utils::GhostEnumToStr(Utils::GetGhostAI()->Fields.GhostInfo->Fields.GhostTraits.GhostType_);
        else
            ghostType = "Not in the game";

        if (!Globals::RPCShowName) name = "Anon";
        std::string FullStatus = name + " • " + ghostType;

        strncpy(Globals::statusRPC, FullStatus.c_str(), sizeof(Globals::statusRPC) - 1);
        Globals::statusRPC[sizeof(Globals::statusRPC) - 1] = '\0';
	}
}