#pragma once

#include "RequestProcessor.h"

namespace proc::trx {

class ChatMessageProcessor : public RequestProcessor {
public:
    void process() override;

private:
    using RequestProcessor::RequestProcessor;

    void processChatUserName();

    void processChatUserListRequest();

    void processChatTo();

    void processStatusNotificationRequest();
};

} // namespace proc::trx
