/* Explicit Messaging Example */
#include "MessageRouter.h"
#include "utils/Logger.h"
#include "utils/Buffer.h"

using eipScanner::SessionInfo;
using eipScanner::MessageRouter;
using namespace eipScanner::cip;
using namespace eipScanner::utils;

int main() {
        Logger::setLogLevel(LogLevel::DEBUG);
        auto si = std::make_shared<SessionInfo>("192.168.1.100", 0xAF12);
        auto messageRouter = std::make_shared<MessageRouter>();

        // Read attribute
        auto response = messageRouter->sendRequest(si, ServiceCodes::GET_ATTRIBUTE_SINGLE,
                                                       EPath(0x01, 1, 1));

        if (response.getGeneralStatusCode() == GeneralStatusCodes::SUCCESS) {
                Buffer buffer(response.getData());
                CipUint vendorId;
                buffer >> vendorId;

                Logger(LogLevel::INFO) << "Vendor ID is " << vendorId;
        }

        return 0;
}