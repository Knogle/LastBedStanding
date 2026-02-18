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
#include <algorithm>
#include <cmath>
#include <cctype>
#include <cstdint>
#include <cstdarg>   // va_list, va_start, va_end
#include <cstdlib>
#include <dirent.h>
#include <fstream>
#include <limits.h>
#include <limits>
#include <random>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace lbs
{
    enum class Phase : int { Lobby = 0, Countdown = 1, Running = 2, Finished = 3 };

    struct PlayerState
    {
        struct WarpState
        {
            bool active = false;
            int ticksRemaining = 0;
            float startX = 0.0f;
            float startY = 0.0f;
            float startZ = 0.0f;
        };

        bool connected = false;
        bool teamChosen = false; // mirrors gPlayerHasTeamSelected
        bool spawned = false;    // mirrors PlayerInfo[][pSpawned]
        bool spectating = false;
        bool hasWarpPowder = false;
        int teamId = -1;
        WarpState warp{};
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

    struct MapData
    {
        std::string key;
        std::unordered_map<std::string, int> defineInts;
        std::unordered_map<std::string, std::string> defineStrings;
        std::unordered_map<std::string, std::vector<std::vector<float>>> arrays;
    };

    struct JsonValue
    {
        enum class Type { Null, Bool, Number, String, Array, Object };

        Type type = Type::Null;
        bool boolean = false;
        double number = 0.0;
        std::string str;
        std::vector<JsonValue> arr;
        std::unordered_map<std::string, JsonValue> obj;
    };

    class JsonParser
    {
    public:
        explicit JsonParser(const std::string& text) : text_(text) {}

        bool parse(JsonValue& out, std::string& err)
        {
            skipWs();
            if (!parseValue(out, err)) return false;
            skipWs();
            if (pos_ != text_.size())
            {
                err = "Trailing characters after JSON document.";
                return false;
            }
            return true;
        }

    private:
        const std::string& text_;
        size_t pos_ = 0;

        void skipWs()
        {
            while (pos_ < text_.size() && std::isspace(static_cast<unsigned char>(text_[pos_])))
            {
                ++pos_;
            }
        }

        bool parseValue(JsonValue& out, std::string& err)
        {
            if (pos_ >= text_.size())
            {
                err = "Unexpected end of input.";
                return false;
            }

            char c = text_[pos_];
            if (c == '"') return parseStringValue(out, err);
            if (c == '{') return parseObject(out, err);
            if (c == '[') return parseArray(out, err);
            if (c == '-' || std::isdigit(static_cast<unsigned char>(c))) return parseNumber(out, err);
            if (matchLiteral("true")) { out.type = JsonValue::Type::Bool; out.boolean = true; return true; }
            if (matchLiteral("false")) { out.type = JsonValue::Type::Bool; out.boolean = false; return true; }
            if (matchLiteral("null")) { out.type = JsonValue::Type::Null; return true; }

            err = "Invalid JSON value.";
            return false;
        }

        bool parseStringRaw(std::string& out, std::string& err)
        {
            if (pos_ >= text_.size() || text_[pos_] != '"')
            {
                err = "Expected string.";
                return false;
            }
            ++pos_;

            out.clear();
            while (pos_ < text_.size())
            {
                char c = text_[pos_++];
                if (c == '"') return true;
                if (c == '\\')
                {
                    if (pos_ >= text_.size())
                    {
                        err = "Invalid escape sequence.";
                        return false;
                    }
                    char e = text_[pos_++];
                    switch (e)
                    {
                        case '"': out.push_back('"'); break;
                        case '\\': out.push_back('\\'); break;
                        case '/': out.push_back('/'); break;
                        case 'b': out.push_back('\b'); break;
                        case 'f': out.push_back('\f'); break;
                        case 'n': out.push_back('\n'); break;
                        case 'r': out.push_back('\r'); break;
                        case 't': out.push_back('\t'); break;
                        case 'u':
                        {
                            if (pos_ + 4 > text_.size())
                            {
                                err = "Invalid unicode escape.";
                                return false;
                            }
                            // Keep parser simple: preserve escaped unicode as '?'
                            pos_ += 4;
                            out.push_back('?');
                            break;
                        }
                        default:
                            err = "Unsupported escape sequence.";
                            return false;
                    }
                    continue;
                }
                out.push_back(c);
            }

            err = "Unterminated string.";
            return false;
        }

        bool parseStringValue(JsonValue& out, std::string& err)
        {
            out.type = JsonValue::Type::String;
            return parseStringRaw(out.str, err);
        }

        bool parseNumber(JsonValue& out, std::string& err)
        {
            size_t start = pos_;

            if (text_[pos_] == '-') ++pos_;
            if (pos_ >= text_.size())
            {
                err = "Invalid number.";
                return false;
            }

            if (text_[pos_] == '0')
            {
                ++pos_;
            }
            else if (std::isdigit(static_cast<unsigned char>(text_[pos_])))
            {
                while (pos_ < text_.size() && std::isdigit(static_cast<unsigned char>(text_[pos_]))) ++pos_;
            }
            else
            {
                err = "Invalid number.";
                return false;
            }

            if (pos_ < text_.size() && text_[pos_] == '.')
            {
                ++pos_;
                if (pos_ >= text_.size() || !std::isdigit(static_cast<unsigned char>(text_[pos_])))
                {
                    err = "Invalid decimal number.";
                    return false;
                }
                while (pos_ < text_.size() && std::isdigit(static_cast<unsigned char>(text_[pos_]))) ++pos_;
            }

            if (pos_ < text_.size() && (text_[pos_] == 'e' || text_[pos_] == 'E'))
            {
                ++pos_;
                if (pos_ < text_.size() && (text_[pos_] == '+' || text_[pos_] == '-')) ++pos_;
                if (pos_ >= text_.size() || !std::isdigit(static_cast<unsigned char>(text_[pos_])))
                {
                    err = "Invalid exponent in number.";
                    return false;
                }
                while (pos_ < text_.size() && std::isdigit(static_cast<unsigned char>(text_[pos_]))) ++pos_;
            }

            const std::string token = text_.substr(start, pos_ - start);
            char* end = nullptr;
            const double value = std::strtod(token.c_str(), &end);
            if (!end || *end != '\0' || !std::isfinite(value))
            {
                err = "Failed to parse number.";
                return false;
            }

            out.type = JsonValue::Type::Number;
            out.number = value;
            return true;
        }

        bool parseArray(JsonValue& out, std::string& err)
        {
            if (text_[pos_] != '[')
            {
                err = "Expected array start.";
                return false;
            }
            ++pos_;
            skipWs();

            out.type = JsonValue::Type::Array;
            out.arr.clear();

            if (pos_ < text_.size() && text_[pos_] == ']')
            {
                ++pos_;
                return true;
            }

            while (pos_ < text_.size())
            {
                JsonValue value;
                if (!parseValue(value, err)) return false;
                out.arr.push_back(std::move(value));

                skipWs();
                if (pos_ >= text_.size())
                {
                    err = "Unterminated array.";
                    return false;
                }
                if (text_[pos_] == ',')
                {
                    ++pos_;
                    skipWs();
                    continue;
                }
                if (text_[pos_] == ']')
                {
                    ++pos_;
                    return true;
                }
                err = "Expected ',' or ']' in array.";
                return false;
            }

            err = "Unterminated array.";
            return false;
        }

        bool parseObject(JsonValue& out, std::string& err)
        {
            if (text_[pos_] != '{')
            {
                err = "Expected object start.";
                return false;
            }
            ++pos_;
            skipWs();

            out.type = JsonValue::Type::Object;
            out.obj.clear();

            if (pos_ < text_.size() && text_[pos_] == '}')
            {
                ++pos_;
                return true;
            }

            while (pos_ < text_.size())
            {
                std::string key;
                if (!parseStringRaw(key, err)) return false;
                skipWs();
                if (pos_ >= text_.size() || text_[pos_] != ':')
                {
                    err = "Expected ':' in object.";
                    return false;
                }
                ++pos_;
                skipWs();

                JsonValue value;
                if (!parseValue(value, err)) return false;
                out.obj.emplace(std::move(key), std::move(value));

                skipWs();
                if (pos_ >= text_.size())
                {
                    err = "Unterminated object.";
                    return false;
                }
                if (text_[pos_] == ',')
                {
                    ++pos_;
                    skipWs();
                    continue;
                }
                if (text_[pos_] == '}')
                {
                    ++pos_;
                    return true;
                }
                err = "Expected ',' or '}' in object.";
                return false;
            }

            err = "Unterminated object.";
            return false;
        }

        bool matchLiteral(const char* literal)
        {
            const size_t len = std::char_traits<char>::length(literal);
            if (pos_ + len > text_.size()) return false;
            if (text_.compare(pos_, len, literal) != 0) return false;
            pos_ += len;
            return true;
        }
    };

    static std::string trimTrailingSlash(const std::string& s)
    {
        if (s.empty()) return s;
        if (s.back() == '/' || s.back() == '\\') return s.substr(0, s.size() - 1);
        return s;
    }

    static bool endsWith(const std::string& value, const std::string& suffix)
    {
        if (suffix.size() > value.size()) return false;
        return std::equal(suffix.rbegin(), suffix.rend(), value.rbegin());
    }

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
        std::vector<MapData> maps;
        std::string mapsDir = "scriptfiles/maps";
        int mapCycleCount = 0;
        int mapCycleCurrent = -1;

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
            players[playerid].spectating = false;
        }
        void playerDied(int playerid)
        {
            if (!isValidPlayer(playerid)) return;
            players[playerid].spawned = false;
        }
        void playerSetSpectating(int playerid, bool spectating)
        {
            if (!isValidPlayer(playerid)) return;
            players[playerid].spectating = spectating;
        }
        void playerSetWarpPowder(int playerid, bool hasWarpPowder)
        {
            if (!isValidPlayer(playerid)) return;
            players[playerid].hasWarpPowder = hasWarpPowder;
        }
        bool playerHasWarpPowder(int playerid) const
        {
            if (!isValidPlayer(playerid)) return false;
            return players[playerid].hasWarpPowder;
        }
        bool playerWarpStart(int playerid, float x, float y, float z, int ticks)
        {
            if (!isValidPlayer(playerid)) return false;
            auto& p = players[playerid];
            if (!p.connected || p.warp.active || !p.hasWarpPowder) return false;
            if (ticks < 1) ticks = 1;

            p.warp.active = true;
            p.warp.ticksRemaining = ticks;
            p.warp.startX = x;
            p.warp.startY = y;
            p.warp.startZ = z;
            p.hasWarpPowder = false; // consumed on start, restored on movement abort
            return true;
        }
        // return values:
        // -2: aborted (player moved)
        // -1: no active warp
        //  0: completed
        // >0: ticks remaining
        int playerWarpTick(int playerid, float x, float y, float z)
        {
            if (!isValidPlayer(playerid)) return -1;
            auto& p = players[playerid];
            if (!p.warp.active) return -1;

            constexpr float kMoveToleranceSq = 4.0f; // same as 2.0 units radius in Pawn
            const float dx = x - p.warp.startX;
            const float dy = y - p.warp.startY;
            const float dz = z - p.warp.startZ;
            const float distSq = (dx * dx) + (dy * dy) + (dz * dz);
            if (distSq > kMoveToleranceSq)
            {
                p.warp = PlayerState::WarpState{};
                p.hasWarpPowder = true; // keep item when teleport is aborted
                return -2;
            }

            p.warp.ticksRemaining -= 1;
            if (p.warp.ticksRemaining <= 0)
            {
                p.warp = PlayerState::WarpState{};
                return 0;
            }
            return p.warp.ticksRemaining;
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

        bool parseMapJson(const std::string& jsonText, MapData& outMap, std::string& err) const
        {
            JsonValue root;
            JsonParser parser(jsonText);
            if (!parser.parse(root, err))
            {
                return false;
            }
            if (root.type != JsonValue::Type::Object)
            {
                err = "Root JSON value is not an object.";
                return false;
            }

            const auto itKey = root.obj.find("map_key");
            if (itKey == root.obj.end() || itKey->second.type != JsonValue::Type::String)
            {
                err = "Missing or invalid 'map_key'.";
                return false;
            }
            outMap.key = itKey->second.str;

            const auto itDefines = root.obj.find("defines");
            if (itDefines != root.obj.end())
            {
                if (itDefines->second.type != JsonValue::Type::Object)
                {
                    err = "Invalid 'defines' object.";
                    return false;
                }
                for (const auto& kv : itDefines->second.obj)
                {
                    if (kv.second.type == JsonValue::Type::String)
                    {
                        outMap.defineStrings[kv.first] = kv.second.str;
                    }
                    else if (kv.second.type == JsonValue::Type::Number)
                    {
                        const long long raw = std::llround(kv.second.number);
                        if (raw >= 0 && raw <= 0xFFFFFFFFll)
                        {
                            outMap.defineInts[kv.first] = static_cast<int>(static_cast<uint32_t>(raw));
                        }
                        else if (raw < static_cast<long long>(INT_MIN))
                        {
                            outMap.defineInts[kv.first] = INT_MIN;
                        }
                        else if (raw > static_cast<long long>(INT_MAX))
                        {
                            outMap.defineInts[kv.first] = INT_MAX;
                        }
                        else
                        {
                            outMap.defineInts[kv.first] = static_cast<int>(raw);
                        }
                    }
                }
            }

            const auto itArrays = root.obj.find("arrays");
            if (itArrays != root.obj.end())
            {
                if (itArrays->second.type != JsonValue::Type::Object)
                {
                    err = "Invalid 'arrays' object.";
                    return false;
                }
                for (const auto& kv : itArrays->second.obj)
                {
                    const JsonValue& arrayValue = kv.second;
                    if (arrayValue.type != JsonValue::Type::Array) continue;

                    std::vector<std::vector<float>> rows;
                    rows.reserve(arrayValue.arr.size());

                    bool arrayValid = true;
                    for (const auto& rowValue : arrayValue.arr)
                    {
                        if (rowValue.type != JsonValue::Type::Array)
                        {
                            arrayValid = false;
                            break;
                        }

                        std::vector<float> row;
                        row.reserve(rowValue.arr.size());
                        for (const auto& numberValue : rowValue.arr)
                        {
                            if (numberValue.type != JsonValue::Type::Number)
                            {
                                arrayValid = false;
                                break;
                            }
                            row.push_back(static_cast<float>(numberValue.number));
                        }
                        if (!arrayValid) break;
                        rows.push_back(std::move(row));
                    }

                    if (arrayValid)
                    {
                        outMap.arrays[kv.first] = std::move(rows);
                    }
                }
            }

            return true;
        }

        int mapsReload()
        {
            maps.clear();

            const std::string dirPath = trimTrailingSlash(mapsDir);
            DIR* dir = opendir(dirPath.c_str());
            if (!dir)
            {
                log("[omp-lbs] mapsReload failed: cannot open directory '%s'", dirPath.c_str());
                return 0;
            }

            std::vector<std::string> jsonFiles;
            for (dirent* ent = readdir(dir); ent != nullptr; ent = readdir(dir))
            {
                if (!ent->d_name) continue;
                std::string name(ent->d_name);
                if (name == "." || name == "..") continue;
                if (!endsWith(name, ".json")) continue;
                jsonFiles.push_back(name);
            }
            closedir(dir);

            std::sort(jsonFiles.begin(), jsonFiles.end());
            for (const std::string& fileName : jsonFiles)
            {
                const std::string fullPath = dirPath + "/" + fileName;
                std::ifstream in(fullPath);
                if (!in)
                {
                    log("[omp-lbs] mapsReload: cannot read '%s'", fullPath.c_str());
                    continue;
                }

                std::ostringstream ss;
                ss << in.rdbuf();
                std::string err;
                MapData map;
                if (!parseMapJson(ss.str(), map, err))
                {
                    log("[omp-lbs] mapsReload: parse error in '%s': %s", fullPath.c_str(), err.c_str());
                    continue;
                }

                if (map.key.empty())
                {
                    log("[omp-lbs] mapsReload: skipped '%s' (empty map_key)", fullPath.c_str());
                    continue;
                }
                maps.push_back(std::move(map));
            }

            if (maps.empty())
            {
                mapCycleCount = 0;
                mapCycleCurrent = -1;
            }
            else if (mapCycleCurrent >= static_cast<int>(maps.size()))
            {
                mapCycleCurrent = -1;
            }

            log("[omp-lbs] mapsReload: loaded %d map(s) from %s", static_cast<int>(maps.size()), dirPath.c_str());
            return static_cast<int>(maps.size());
        }

        int mapsCount() const
        {
            return static_cast<int>(maps.size());
        }

        const MapData* getMapById(int mapId) const
        {
            if (mapId < 0 || mapId >= static_cast<int>(maps.size())) return nullptr;
            return &maps[static_cast<size_t>(mapId)];
        }

        const MapData* getCurrentMap() const
        {
            return getMapById(mapCycleCurrent);
        }

        bool mapGetKey(int mapId, std::string& out) const
        {
            const MapData* map = getMapById(mapId);
            if (!map) return false;
            out = map->key;
            return true;
        }

        bool mapGetCurrentKey(std::string& out) const
        {
            const MapData* map = getCurrentMap();
            if (!map) return false;
            out = map->key;
            return true;
        }

        int mapGetDefineInt(int mapId, const std::string& key, int defaultValue) const
        {
            const MapData* map = getMapById(mapId);
            if (!map) return defaultValue;

            const auto it = map->defineInts.find(key);
            if (it == map->defineInts.end()) return defaultValue;
            return it->second;
        }

        bool mapGetDefineString(int mapId, const std::string& key, std::string& out) const
        {
            const MapData* map = getMapById(mapId);
            if (!map) return false;

            const auto it = map->defineStrings.find(key);
            if (it == map->defineStrings.end()) return false;
            out = it->second;
            return true;
        }

        int mapGetArrayRows(int mapId, const std::string& arrayName) const
        {
            const MapData* map = getMapById(mapId);
            if (!map) return 0;

            const auto it = map->arrays.find(arrayName);
            if (it == map->arrays.end()) return 0;
            return static_cast<int>(it->second.size());
        }

        int mapGetArrayCols(int mapId, const std::string& arrayName, int row) const
        {
            const MapData* map = getMapById(mapId);
            if (!map) return 0;

            const auto it = map->arrays.find(arrayName);
            if (it == map->arrays.end()) return 0;
            if (row < 0 || row >= static_cast<int>(it->second.size())) return 0;
            return static_cast<int>(it->second[static_cast<size_t>(row)].size());
        }

        bool mapGetArrayValue(int mapId, const std::string& arrayName, int row, int col, float& outValue) const
        {
            const MapData* map = getMapById(mapId);
            if (!map) return false;

            const auto it = map->arrays.find(arrayName);
            if (it == map->arrays.end()) return false;
            if (row < 0 || row >= static_cast<int>(it->second.size())) return false;
            const auto& r = it->second[static_cast<size_t>(row)];
            if (col < 0 || col >= static_cast<int>(r.size())) return false;
            outValue = r[static_cast<size_t>(col)];
            return true;
        }

        bool mapCycleInit(int mapCount)
        {
            if (mapCount <= 0) return false;
            mapCycleCount = mapCount;
            std::uniform_int_distribution<int> pick(0, mapCycleCount - 1);
            mapCycleCurrent = pick(rng);
            log("[omp-lbs] mapCycleInit random pick (count=%d current=%d)", mapCycleCount, mapCycleCurrent);
            return true;
        }

        int mapCycleGetCurrent() const
        {
            return mapCycleCurrent;
        }

        int mapCycleAdvance()
        {
            if (mapCycleCount <= 0)
            {
                log("[omp-lbs] mapCycleAdvance failed: mapCycleCount=%d", mapCycleCount);
                return -1;
            }
            const int old = mapCycleCurrent;
            std::uniform_int_distribution<int> pick(0, mapCycleCount - 1);
            mapCycleCurrent = pick(rng);
            log("[omp-lbs] mapCycleAdvance random %d -> %d", old, mapCycleCurrent);
            return mapCycleCurrent;
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

SCRIPT_API(LBS_MapsReload, int())
{
    return lbs::g.mapsReload();
}

SCRIPT_API(LBS_MapsCount, int())
{
    return lbs::g.mapsCount();
}

SCRIPT_API(LBS_MapGetKey, bool(int mapId, std::string& out))
{
    return lbs::g.mapGetKey(mapId, out);
}

SCRIPT_API(LBS_MapGetCurrentKey, bool(std::string& out))
{
    return lbs::g.mapGetCurrentKey(out);
}

SCRIPT_API(LBS_MapGetDefineInt, int(int mapId, std::string const& key, int defaultValue))
{
    return lbs::g.mapGetDefineInt(mapId, key, defaultValue);
}

SCRIPT_API(LBS_MapGetDefineString, bool(int mapId, std::string const& key, std::string& out))
{
    return lbs::g.mapGetDefineString(mapId, key, out);
}

SCRIPT_API(LBS_MapGetArrayRows, int(int mapId, std::string const& arrayName))
{
    return lbs::g.mapGetArrayRows(mapId, arrayName);
}

SCRIPT_API(LBS_MapGetArrayCols, int(int mapId, std::string const& arrayName, int row))
{
    return lbs::g.mapGetArrayCols(mapId, arrayName, row);
}

SCRIPT_API(LBS_MapGetArrayValue, int(int mapId, std::string const& arrayName, int row, int col))
{
    float value = 0.0f;
    if (!lbs::g.mapGetArrayValue(mapId, arrayName, row, col, value))
    {
        float zero = 0.0f;
        return amx_ftoc(zero);
    }
    return amx_ftoc(value);
}

SCRIPT_API(LBS_MapCycleInit, bool(int mapCount))
{
    return lbs::g.mapCycleInit(mapCount);
}

SCRIPT_API(LBS_MapCycleGetCurrent, int())
{
    return lbs::g.mapCycleGetCurrent();
}

SCRIPT_API(LBS_MapCycleAdvance, int())
{
    return lbs::g.mapCycleAdvance();
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

SCRIPT_API(LBS_PlayerSetSpectating, bool(int playerid, bool spectating))
{
    if (!lbs::g.bound) return false;
    lbs::g.playerSetSpectating(playerid, spectating);
    return true;
}

SCRIPT_API(LBS_PlayerSetWarpPowder, bool(int playerid, bool hasWarpPowder))
{
    if (!lbs::g.bound) return false;
    lbs::g.playerSetWarpPowder(playerid, hasWarpPowder);
    return true;
}

SCRIPT_API(LBS_PlayerHasWarpPowder, bool(int playerid))
{
    if (!lbs::g.bound) return false;
    return lbs::g.playerHasWarpPowder(playerid);
}

SCRIPT_API(LBS_PlayerWarpStart, bool(int playerid, cell x, cell y, cell z, int ticks))
{
    if (!lbs::g.bound) return false;
    return lbs::g.playerWarpStart(playerid, amx_ctof(x), amx_ctof(y), amx_ctof(z), ticks);
}

SCRIPT_API(LBS_PlayerWarpTick, int(int playerid, cell x, cell y, cell z))
{
    if (!lbs::g.bound) return -1;
    return lbs::g.playerWarpTick(playerid, amx_ctof(x), amx_ctof(y), amx_ctof(z));
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
