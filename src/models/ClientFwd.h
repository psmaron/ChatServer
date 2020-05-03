#pragma once

#include <memory>
#include <optional>
#include <unordered_map>
#include <unordered_set>

namespace models {

class Clients;
class ClientData;
using ClientsById = std::unordered_map<int, std::shared_ptr<ClientData>>;
using ClientUsernameToId = std::unordered_map<std::string, int>;
using ClientsIdSet = std::unordered_set<int>;
using ClientsUsernameSet = std::unordered_set<std::string>;
using RemovedClientId = std::optional<int>;
} // namespace models