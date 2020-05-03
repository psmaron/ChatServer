#pragma once

namespace comm::trx {

enum class InvokingTime { BeforeSendingResponses, AfterSendingResponses };

class CustomAction {
public:
    CustomAction(InvokingTime invoking = InvokingTime::BeforeSendingResponses) : invokingTime(invoking) {
    }

    virtual ~CustomAction() = default;
    InvokingTime invokingTime = InvokingTime::BeforeSendingResponses;
};

} // namespace comm::trx