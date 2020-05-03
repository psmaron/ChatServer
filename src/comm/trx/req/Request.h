#pragma once

#include <boost/property_tree/ptree_fwd.hpp>

namespace comm::trx::req {

class Request {
public:
    static std::unique_ptr<trx::req::Request> createRequest(const boost::property_tree::ptree &requestXmlTree);
    virtual ~Request() {
    }
};

} // namespace comm::trx::req