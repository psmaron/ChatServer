#include "ChatUserListResponse.h"

#include <sstream>

#include "models/ClientData.h"

namespace comm::trx::res {

ChatUserListResponse::ChatUserListResponse(const models::ClientsById &clientsById, Recipients recipients)
    : Response(recipients), clientsByIdRef(clientsById) {
}

ChatUserListResponse::ChatUserListResponse(models::ClientsById &&clientsById, Recipients recipients)
    : Response(recipients), clientsById(clientsById), clientsByIdRef(this->clientsById) {
}

std::ostringstream ChatUserListResponse::serialize() const {
    std::ostringstream message;
    // clang-format off
    message << XML_PREAMBLE
        << R"(<ChatUserListResponse>)";
    for ([[maybe_unused]] const auto &[_, clientData] : clientsByIdRef)
        message << R"(<UserList>)"
                   R"(<Status>)" << clientData->getChatStatus() << R"(</Status>)"
                   R"(<UserName>)" << clientData->getChatUsername() << R"(</UserName>)"
                   R"(<isOtherSystemUser>)" << std::boolalpha << clientData->getIsOtherSystemUser()  << R"(</isOtherSystemUser>)"
                   R"(<UserID>)" << clientData->getId() << R"(</UserID>)"
               R"(</UserList>)";
    message << R"(</ChatUserListResponse>)";
    // clang-format on
    return message;
}
} // namespace comm::trx::res
