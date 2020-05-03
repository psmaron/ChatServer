#pragma once

#include "RequestProcessor.h"

namespace proc::trx {

class InvalidRequestProcessor : public RequestProcessor {
public:
    void process() override;

private:
    using RequestProcessor::RequestProcessor;
    void processInvalidXmlRequest();
    void processInvalidHeaderRequest() const;
};

} // namespace proc::trx