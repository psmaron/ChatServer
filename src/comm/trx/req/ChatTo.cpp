#include "ChatTo.h"

#include <boost/property_tree/ptree.hpp>

#include "comm/CommErrors.h"

namespace comm::trx::req {

ChatTo::ChatTo(const boost::property_tree::ptree &requestXmlTree) {
    const auto &rootTag = requestXmlTree.get_child("ChatTo");

    if (rootTag.count("Message") != 1)
        throw ParsingRequestDataError();

    message = rootTag.get<std::string>("Message");
    for (auto [tag, value] : rootTag)
        if (tag == "Recipients")
            recipients.insert(value.get_value<int>());
}

} // namespace comm::trx::req
