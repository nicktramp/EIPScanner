/* Implicit Messaging Exmple */
#include <sstream>
#include "cip/connectionManager/NetworkConnectionParams.h"
#include "SessionInfo.h"
#include "ConnectionManager.h"
#include "utils/Logger.h"
#include "utils/Buffer.h"
#include "cip/Types.h"

using namespace eipScanner::cip;
using eipScanner::SessionInfo;
using eipScanner::MessageRouter;
using eipScanner::ConnectionManager;
using eipScanner::cip::connectionManager::ConnectionParameters;
using eipScanner::cip::connectionManager::NetworkConnectionParams;
using eipScanner::utils::Buffer;
using eipScanner::utils::Logger;
using eipScanner::utils::LogLevel;

int main() {
        Logger::setLogLevel(LogLevel::INFO);
        ConnectionManager connectionManager;
        auto si = std::make_shared<SessionInfo>("192.168.1.100", 0xAF12);

        ConnectionParameters parameters;
        parameters.connectionPath = {0x20, 0x04, 0x2C, 0x65, 0x2C, 0xC9}; // config Assm0, output Assm101, intput Assm201
        //parameters.connectionPath = {0x20, 0x04,0x24, 151, 0x2C, 150, 0x2C, 100};  // config Assm151, output Assm150, intput Assm100
        parameters.o2tRealTimeFormat = true;
        parameters.originatorVendorId = 342;
        parameters.originatorSerialNumber = 32423;
        parameters.t2oNetworkConnectionParams |= NetworkConnectionParams::P2P;
        parameters.t2oNetworkConnectionParams |= NetworkConnectionParams::SCHEDULED_PRIORITY;
        parameters.t2oNetworkConnectionParams |= 28; //size of Assm201
        parameters.o2tNetworkConnectionParams |= NetworkConnectionParams::P2P;
        parameters.o2tNetworkConnectionParams |= NetworkConnectionParams::SCHEDULED_PRIORITY;
        parameters.o2tNetworkConnectionParams |= 24; //size of Assm101

        parameters.originatorSerialNumber = 0x12345;
        parameters.o2tRPI = 1000000;
        parameters.t2oRPI = 1000000;
        parameters.transportTypeTrigger |= NetworkConnectionParams::CLASS1;

        auto io = connectionManager.forwardOpen(si, parameters, false);
        if (auto ptr = io.lock()) {
            Buffer assm101;
            CipReal setpoint = 10.0;
            CipDint valveOverrideMode = 0;
            CipDint controlMode = 0;        // auto
            CipReal controlVal = 0.0;       // not used for auto
            CipDint totalizerControl = 2;   // 1:run, 2:stop, reset:3
            CipDint calInstance = 1;        // calibration instance

            assm101 << setpoint << valveOverrideMode << controlMode << controlVal << totalizerControl << calInstance;
            ptr->setDataToSend(assm101.data());

            ptr->setReceiveDataListener([](auto realTimeHeader, auto sequence, auto data) {
                    std::ostringstream ss;
                    ss << "secNum=" << sequence << " data=";
                    for (auto &byte : data) {
                            ss << "[" << std::hex << (int) byte << "]";
                    }
                    Logger(LogLevel::INFO) << "Received: " << ss.str();
            });

            ptr->setCloseListener([]() {
                    Logger(LogLevel::INFO) << "IO Closed";
            });
        }

        int count = 200;
        while (connectionManager.hasOpenConnections() && count-- > 0) {
            connectionManager.handleConnections(std::chrono::milliseconds(100));
        }
        connectionManager.forwardClose(si, io);

        return 0;
}