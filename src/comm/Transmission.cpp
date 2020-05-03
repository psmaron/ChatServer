#include "Transmission.h"

#include <arpa/inet.h>

#include <array>
#include <string>

#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <log4cpp/Category.hh>

#include "comm/trx/req/InvalidHeaderRequest.h"
#include "comm/trx/req/InvalidXmlRequest.hpp"

#include "CommErrors.h"
#include "trx/req/Request.h"

namespace comm {
using boost_error = boost::system::error_code;
using boost::asio::ip::tcp;
namespace asio = boost::asio;

static constexpr char XML_TAG = 16;
static constexpr size_t HEADER_SIZE = sizeof(XML_TAG) + sizeof(int);

static log4cpp::Category &logger = log4cpp::Category::getInstance(std::string("server.connection.transmission"));

void handleAsioError(boost_error &error, const std::string &operationType) {
    if (!error)
        return;

    LOG4CPP_ERROR_S(logger) << "Async I/O " << operationType << " operation failed for the reason listed below:";
    switch (error.value()) {
    case asio::error::operation_aborted:
        LOG4CPP_ERROR_S(logger) << "Operation was aborted: " << error.message();
        throw OperationAbortedError();
    case asio::error::eof:
        LOG4CPP_ERROR_S(logger) << "Client disconnected due to: " << error.message();
        throw LostConnectionError();
    default:
        LOG4CPP_ERROR_S(logger) << "Unknown error: " << error.message();
        throw UnknownConnectionError();
    }
}

void handleAsioReadError(boost_error &error) {
    handleAsioError(error, "read");
}

void handleAsioWriteError(boost_error &error) {
    handleAsioError(error, "write");
}

void validateHeader(const std::array<char, HEADER_SIZE> &header, const unsigned int &length) {
    if (header[0] != XML_TAG) {
        LOG4CPP_ERROR_S(logger) << "Corrupted request header, wrong value in xml tag";
        throw CorruptedHeaderXmlTagError();
    }
    if (length == 0) {
        LOG4CPP_ERROR_S(logger) << "Corrupted request header, payload length = 0";
        throw CorruptedHeaderLengthError();
    }
}

std::string readReqPayload(tcp::socket &receiver, const spawn::yield_context &yield) {
    std::array<char, HEADER_SIZE> header{0};
    boost_error error;
    asio::async_read(receiver, asio::buffer(header), yield[error]);
    handleAsioReadError(error);
    unsigned int length = ntohl(*reinterpret_cast<unsigned int *>(header.begin() + sizeof(XML_TAG)));
    validateHeader(header, length);
    std::string request(length, 0);
    asio::async_read(receiver, asio::buffer(request), yield[error]);
    handleAsioReadError(error);
    LOG4CPP_DEBUG_S(logger) << "Received request of length: " << request.size() << ", payload: " << request;
    return request;
}

boost::property_tree::ptree createRequestXmlTree(const std::string &request) {
    std::stringstream requestStream(request);
    boost::property_tree::ptree xmlTree;
    boost::property_tree::read_xml(requestStream, xmlTree);
    return xmlTree;
}

boost::property_tree::ptree read(const tcp::socket &receiver, const spawn::yield_context &yield, const unsigned int &timeout) {
    auto &mutableSender = const_cast<tcp::socket &>(receiver);
    asio::deadline_timer deadline(mutableSender.get_io_service());
    if (timeout != 0) {
        deadline.expires_from_now(boost::posix_time::seconds(timeout));
        deadline.async_wait([&](const boost::system::error_code &timer_error) {
            if (timer_error != asio::error::operation_aborted) {
                boost::system::error_code ec;
                mutableSender.cancel(ec);
                LOG4CPP_WARN_S(logger) << "Waiting for request timed out" + ((ec) ? ": " + ec.message() : "");
            }
        });
    }
    return createRequestXmlTree(readReqPayload(mutableSender, yield));
}

std::string formMessage(const std::ostringstream &payload) {
    std::stringstream message;
    unsigned int netLen = htonl(payload.str().size());
    message.write(&XML_TAG, sizeof(char));
    message.write(reinterpret_cast<char *>(&netLen), sizeof(int));
    message << payload.str();
    return message.str();
}

void send(const tcp::socket &receiver, const std::ostringstream &message, const spawn::yield_context &yield) {
    auto &mutableReceiver = const_cast<tcp::socket &>(receiver);
    asio::deadline_timer deadline(mutableReceiver.get_io_service());
    deadline.expires_from_now(boost::posix_time::seconds(100));
    deadline.async_wait([&](const boost_error &timer_error) {
        if (timer_error != asio::error::operation_aborted) {
            boost::system::error_code ec;
            mutableReceiver.cancel(ec);
            LOG4CPP_WARN_S(logger) << "Waiting on sending response timed out" + ((ec) ? ": " + ec.message() : "");
        }
    });

    LOG4CPP_DEBUG_S(logger) << "Sending response: " << message.str();

    boost_error error;
    asio::async_write(mutableReceiver, asio::buffer(formMessage(message)), yield[error]);
    handleAsioWriteError(error);
}

} // namespace comm
