/*
 * omp-lbs - LastBedStanding gameplay component (example skeleton)
 *
 * Focus: teams, money pickup generation/recognition, match state machine.
 * Pawn remains as UI/glue and still uses CreatePickup/GivePlayerMoney/etc.
 *
 * NOTE:
 * - Change the UID below if you want (must be unique across components).
 */

#include <sdk.hpp>
#include <Server/Components/Pawn/pawn.hpp>
#include <Server/Components/Pawn/Impl/pawn_natives.hpp>
#include <Server/Components/Pawn/Impl/pawn_impl.hpp>

#include <array>
#include <cstdint>
#include <cstdarg>   // va_list, va_start, va_end
#include <random>
#include <string>
#include <unordered_map>
#include <vector>

namespace lbs
{
    enum class Phase : int { Lobby = 0, Countdown = 1, Running = 2, Finished = 3 };

    struct PlayerState
    {
        bool connected = false;
        bool teamChosen = false; // mirrors gPlayerHasTeamSelected
        bool spawned = false;    // mirrors PlayerInfo[][pSpawned]
        int teamId = -1;
    };

    struct MoneySpawnRange
    {
        // Stored as max/min in your Pawn file: x_max, x_min, y_max, y_min, z_max, z_min
        float xMax = 0.0f, xMin = 0.0f;
        float yMax = 0.0f, yMin = 0.0f;
        float zMax = 0.0f, zMin = 0.0f;
        int vw = 0;
        bool isMain = false;
    };

    struct MoneyPickupInfo
    {
        int amount = 0;
        bool isMain = false;
    };

    struct State
    {
        ICore* core = nullptr;
        IPawnComponent* pawn = nullptr;

        AMX* ownerAmx = nullptr; // gamemode AMX that called LBS_Bind()
        bool bound = false;

        // Config
        int teamCount = 6;
        int moneyModel = 1212;
        int moneyType = 19;
        int moneyValue = 1000;
        int teamMoneyIntervalMs = 2500;
        int mainMoneyIntervalMs = 1000;
        size_t moneyPickupCap = 3900; // stay below pickup limit

        // Runtime
        Phase phase = Phase::Lobby;
        int countdownSec = 0;

        int64_t teamMoneyAccumMs = 0;
        int64_t mainMoneyAccumMs = 0;
        int64_t countdownAccumMs = 0;

        std::vector<MoneySpawnRange> teamRanges;
        std::vector<MoneySpawnRange> mainRanges;
        std::unordered_map<int, MoneyPickupInfo> moneyPickups;

        static constexpr int kMaxPlayers = 1000;
        std::array<PlayerState, kMaxPlayers> players{};

        std::mt19937 rng{ std::random_device{}() };

        // Cached public indices
        int pubCreatePickup = -1;
        int pubOnPhaseChange = -1;
        int pubOnCountdownTick = -1;
        int pubOnMatchStart = -1;
        int pubOnMatchEnd = -1;

        void log(const char* fmt, ...)
        {
            if (!core) return;
            va_list args;
            va_start(args, fmt);
            core->vprintLn(fmt, args);
            va_end(args);
        }

        bool isValidPlayer(int playerid) const
        {
            return playerid >= 0 && playerid < static_cast<int>(players.size());
        }

        void refreshPublicCache()
        {
            if (!ownerAmx) return;
            pubCreatePickup = pubOnPhaseChange = pubOnCountdownTick = pubOnMatchStart = pubOnMatchEnd = -1;
            amx_FindPublic(ownerAmx, "LBS_Internal_CreatePickup", &pubCreatePickup);
            amx_FindPublic(ownerAmx, "LBS_OnPhaseChange", &pubOnPhaseChange);
            amx_FindPublic(ownerAmx, "LBS_OnCountdownTick", &pubOnCountdownTick);
            amx_FindPublic(ownerAmx, "LBS_OnMatchStart", &pubOnMatchStart);
            amx_FindPublic(ownerAmx, "LBS_OnMatchEnd", &pubOnMatchEnd);
        }

        bool callPublicByIndex(int idx, const std::vector<cell>& params, cell* ret = nullptr)
        {
            if (!ownerAmx || idx < 0) return false;

            // push params in reverse (AMX expects last param pushed first)
            for (auto it = params.rbegin(); it != params.rend(); ++it)
            {
                if (amx_Push(ownerAmx, *it) != AMX_ERR_NONE) return false;
            }

            cell r = 0;
            int err = amx_Exec(ownerAmx, &r, idx);
            if (ret) *ret = r;
            return err == AMX_ERR_NONE;
        }

        void emitPhaseChange(Phase oldP, Phase newP)
        {
            if (pubOnPhaseChange >= 0)
            {
                callPublicByIndex(pubOnPhaseChange,
                    { static_cast<cell>(static_cast<int>(newP)), static_cast<cell>(static_cast<int>(oldP)) },
                    nullptr);
            }
        }

        void emitCountdownTick(int secondsLeft)
        {
            if (pubOnCountdownTick >= 0)
            {
                callPublicByIndex(pubOnCountdownTick, { static_cast<cell>(secondsLeft) }, nullptr);
            }
        }

        void emitMatchStart()
        {
            if (pubOnMatchStart >= 0)
            {
                callPublicByIndex(pubOnMatchStart, {}, nullptr);
            }
        }

        void emitMatchEnd(int winningTeam)
        {
            if (pubOnMatchEnd >= 0)
            {
                callPublicByIndex(pubOnMatchEnd, { static_cast<cell>(winningTeam) }, nullptr);
            }
        }

        int createPickupPawn(float x, float y, float z, int vw, bool isMain)
        {
            if (!ownerAmx || pubCreatePickup < 0) return -1;

            std::vector<cell> params;
            params.reserve(7);
            params.push_back(static_cast<cell>(moneyModel));
            params.push_back(static_cast<cell>(moneyType));
            params.push_back(amx_ftoc(x));
            params.push_back(amx_ftoc(y));
            params.push_back(amx_ftoc(z));
            params.push_back(static_cast<cell>(vw));
            params.push_back(static_cast<cell>(isMain ? 1 : 0));

            cell ret = -1;
            if (!callPublicByIndex(pubCreatePickup, params, &ret)) return -1;
            return static_cast<int>(ret);
        }

        int teamAliveCount(int teamId) const
        {
            int c = 0;
            for (const auto& p : players)
            {
                if (p.connected && p.teamChosen && p.spawned && p.teamId == teamId) c++;
            }
            return c;
        }

        int activeTeamsCount() const
        {
            int c = 0;
            for (int t = 0; t < teamCount; ++t)
            {
                if (teamAliveCount(t) > 0) c++;
            }
            return c;
        }

        int findWinningTeamOrMinus1() const
        {
            int aliveTeams = 0;
            int lastTeam = -1;
            for (int t = 0; t < teamCount; ++t)
            {
                if (teamAliveCount(t) > 0)
                {
                    aliveTeams++;
                    lastTeam = t;
                }
            }
            return (aliveTeams == 1) ? lastTeam : -1;
        }

        void setPhase(Phase p)
        {
            if (phase == p) return;
            Phase old = phase;
            phase = p;
            emitPhaseChange(old, phase);
        }

        void requestCountdown(int seconds)
        {
            if (seconds <= 0) seconds = 1;
            setPhase(Phase::Countdown);
            countdownSec = seconds;
            countdownAccumMs = 0;
            emitCountdownTick(countdownSec);
        }

        void spawnMoneyInRange(const MoneySpawnRange& r)
        {
            if (moneyPickups.size() >= moneyPickupCap) return;

            std::uniform_real_distribution<float> dx(std::min(r.xMin, r.xMax), std::max(r.xMin, r.xMax));
            std::uniform_real_distribution<float> dy(std::min(r.yMin, r.yMax), std::max(r.yMin, r.yMax));
            std::uniform_real_distribution<float> dz(std::min(r.zMin, r.zMax), std::max(r.zMin, r.zMax));

            int pickupid = createPickupPawn(dx(rng), dy(rng), dz(rng), r.vw, r.isMain);
            if (pickupid >= 0)
            {
                moneyPickups[pickupid] = MoneyPickupInfo{ moneyValue, r.isMain };
            }
        }

        void update(int deltaMs)
        {
            // State machine
            switch (phase)
            {
            case Phase::Lobby:
                // Auto-start: 2+ teams alive => 60s countdown
                if (activeTeamsCount() >= 2)
                {
                    requestCountdown(60);
                }
                break;

            case Phase::Countdown:
                countdownAccumMs += deltaMs;
                while (countdownAccumMs >= 1000)
                {
                    countdownAccumMs -= 1000;
                    countdownSec--;
                    if (countdownSec > 0)
                    {
                        emitCountdownTick(countdownSec);
                    }
                    else
                    {
                        setPhase(Phase::Running);
                        emitMatchStart();
                        break;
                    }
                }
                break;

            case Phase::Running:
            {
                int winner = findWinningTeamOrMinus1();
                if (winner >= 0)
                {
                    setPhase(Phase::Finished);
                    emitMatchEnd(winner);
                }
            }
            break;

            case Phase::Finished:
                break;
            }

            // Money spawns
            if (!ownerAmx || pubCreatePickup < 0) return;

            teamMoneyAccumMs += deltaMs;
            mainMoneyAccumMs += deltaMs;

            if (!teamRanges.empty() && teamMoneyAccumMs >= teamMoneyIntervalMs)
            {
                teamMoneyAccumMs %= teamMoneyIntervalMs;
                for (const auto& r : teamRanges) spawnMoneyInRange(r);
            }

            if (!mainRanges.empty() && mainMoneyAccumMs >= mainMoneyIntervalMs)
            {
                mainMoneyAccumMs %= mainMoneyIntervalMs;
                for (const auto& r : mainRanges) spawnMoneyInRange(r);
            }
        }

        // Player ingestion
        void playerConnect(int playerid)
        {
            if (!isValidPlayer(playerid)) return;
            players[playerid] = PlayerState{};
            players[playerid].connected = true;
        }
        void playerDisconnect(int playerid)
        {
            if (!isValidPlayer(playerid)) return;
            players[playerid] = PlayerState{};
        }
        void playerSetTeam(int playerid, int teamid, bool chosen)
        {
            if (!isValidPlayer(playerid)) return;
            if (teamid < -1 || teamid >= teamCount) teamid = -1;
            players[playerid].teamId = teamid;
            players[playerid].teamChosen = chosen;
        }
        void playerSpawned(int playerid)
        {
            if (!isValidPlayer(playerid)) return;
            players[playerid].spawned = true;
        }
        void playerDied(int playerid)
        {
            if (!isValidPlayer(playerid)) return;
            players[playerid].spawned = false;
        }

        // Pickup handling
        int handlePickup(int pickupid)
        {
            auto it = moneyPickups.find(pickupid);
            if (it == moneyPickups.end()) return 0;
            int amount = it->second.amount;
            moneyPickups.erase(it);
            return amount;
        }

        void resetMatch()
        {
            setPhase(Phase::Lobby);
            countdownSec = 0;
            countdownAccumMs = 0;
            teamMoneyAccumMs = 0;
            mainMoneyAccumMs = 0;
            moneyPickups.clear();
        }
    };

    static State g;
} // namespace lbs

// ---------------------------------------------------------------------------
// Component boilerplate

class OmpLbs final : public IComponent, public PawnEventHandler
{
public:
    // Must be unique - you can replace this with your own generated UID.
    PROVIDE_UID(0xA27075315949D8C3);

    ~OmpLbs()
    {
        if (lbs::g.pawn)
        {
            lbs::g.pawn->getEventDispatcher().removeEventHandler(this);
        }
    }

    void onAmxLoad(IPawnScript& script) override
    {
        pawn_natives::AmxLoad(script.GetAMX());
    }

    void onAmxUnload(IPawnScript&) override {}

    StringView componentName() const override { return "omp-lbs gameplay component"; }
    SemanticVersion componentVersion() const override { return SemanticVersion(0, 1, 0, 0); }

    void onLoad(ICore* c) override
    {
        lbs::g.core = c;
        lbs::g.log("[omp-lbs] loaded");
        setAmxLookups(c);
    }

    void onInit(IComponentList* components) override
    {
        lbs::g.pawn = components->queryComponent<IPawnComponent>();
        if (lbs::g.pawn)
        {
            setAmxFunctions(lbs::g.pawn->getAmxFunctions());
            setAmxLookups(components);
            lbs::g.pawn->getEventDispatcher().addEventHandler(this);
        }
    }

    void onReady() override {}
    void onFree(IComponent* component) override
    {
        if (component == lbs::g.pawn)
        {
            lbs::g.pawn = nullptr;
            setAmxFunctions();
            setAmxLookups();
        }
    }
    void free() override { delete this; }
    void reset() override {}
};

COMPONENT_ENTRY_POINT() { return new OmpLbs(); }

// ---------------------------------------------------------------------------
// Pawn natives (SCRIPT_API)

SCRIPT_API(LBS_Bind, bool(int teamCount))
{
    if (teamCount < 2) teamCount = 2;
    if (teamCount > 32) teamCount = 32;

    lbs::g.ownerAmx = GetAMX();
    lbs::g.bound = true;
    lbs::g.teamCount = teamCount;
    lbs::g.refreshPublicCache();
    lbs::g.log("[omp-lbs] bound to gamemode AMX (teamCount=%d)", teamCount);
    return true;
}

SCRIPT_API(LBS_ConfigMoney, bool(int modelid, int type, int value, int teamIntervalMs, int mainIntervalMs))
{
    if (!lbs::g.bound) return false;
    if (modelid > 0) lbs::g.moneyModel = modelid;
    if (type >= 0) lbs::g.moneyType = type;
    if (value >= 0) lbs::g.moneyValue = value;
    if (teamIntervalMs >= 100) lbs::g.teamMoneyIntervalMs = teamIntervalMs;
    if (mainIntervalMs >= 100) lbs::g.mainMoneyIntervalMs = mainIntervalMs;
    return true;
}

SCRIPT_API(LBS_AddMoneySpawn, bool(cell xMax, cell xMin, cell yMax, cell yMin, cell zMax, cell zMin, int vw, bool isMain))
{
    if (!lbs::g.bound) return false;

    lbs::MoneySpawnRange r;
    r.xMax = amx_ctof(xMax);
    r.xMin = amx_ctof(xMin);
    r.yMax = amx_ctof(yMax);
    r.yMin = amx_ctof(yMin);
    r.zMax = amx_ctof(zMax);
    r.zMin = amx_ctof(zMin);
    r.vw = vw;
    r.isMain = isMain;

    if (isMain) lbs::g.mainRanges.push_back(r);
    else lbs::g.teamRanges.push_back(r);

    return true;
}

SCRIPT_API(LBS_Update, bool(int deltaMs))
{
    if (!lbs::g.bound) return false;
    if (deltaMs < 0) deltaMs = 0;
    lbs::g.update(deltaMs);
    return true;
}

SCRIPT_API(LBS_ResetMatch, bool())
{
    if (!lbs::g.bound) return false;
    lbs::g.resetMatch();
    return true;
}

SCRIPT_API(LBS_GetPhase, int())
{
    return static_cast<int>(lbs::g.phase);
}

SCRIPT_API(LBS_RequestCountdown, bool(int seconds))
{
    if (!lbs::g.bound) return false;
    lbs::g.requestCountdown(seconds);
    return true;
}

SCRIPT_API(LBS_PlayerConnect, bool(int playerid))
{
    if (!lbs::g.bound) return false;
    lbs::g.playerConnect(playerid);
    return true;
}

SCRIPT_API(LBS_PlayerDisconnect, bool(int playerid))
{
    if (!lbs::g.bound) return false;
    lbs::g.playerDisconnect(playerid);
    return true;
}

SCRIPT_API(LBS_PlayerSetTeam, bool(int playerid, int teamid, bool chosen))
{
    if (!lbs::g.bound) return false;
    lbs::g.playerSetTeam(playerid, teamid, chosen);
    return true;
}

SCRIPT_API(LBS_PlayerSpawned, bool(int playerid))
{
    if (!lbs::g.bound) return false;
    lbs::g.playerSpawned(playerid);
    return true;
}

SCRIPT_API(LBS_PlayerDied, bool(int playerid))
{
    if (!lbs::g.bound) return false;
    lbs::g.playerDied(playerid);
    return true;
}

SCRIPT_API(LBS_HandlePickup, int(int playerid, int pickupid))
{
    (void)playerid;
    if (!lbs::g.bound) return 0;
    return lbs::g.handlePickup(pickupid);
}

SCRIPT_API(LBS_TeamAliveCount, int(int teamid))
{
    if (!lbs::g.bound) return 0;
    if (teamid < 0 || teamid >= lbs::g.teamCount) return 0;
    return lbs::g.teamAliveCount(teamid);
}

SCRIPT_API(LBS_ActiveTeamsCount, int())
{
    if (!lbs::g.bound) return 0;
    return lbs::g.activeTeamsCount();
}

