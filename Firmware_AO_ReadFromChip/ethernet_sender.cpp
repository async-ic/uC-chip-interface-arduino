#include "ethernet_sender.h"

static byte mac_[6];
static IPAddress localIP_;
static IPAddress serverIP_;
static int serverPort_;

static EthernetClient client_;

const int MAX_OUTPUTS = 100;
OutputAddress outputAddresses[MAX_OUTPUTS];  // Send up to MAX_OUTPUTS structs

void ethernet_sender_init(byte mac[6], IPAddress ip, IPAddress server, int port) {
    memcpy(mac_, mac, 6);
    localIP_ = ip;
    serverIP_ = server;
    serverPort_ = port;

    Ethernet.begin(mac_, localIP_);
    client_.connect(serverIP_, serverPort_);
    Serial.println("Sender initialized.");
}

bool ethernet_sender_send(const byte* data, size_t len) {
    if (!client_.connected()) {
        client_.stop();
        return client_.connect(serverIP_, serverPort_);
    }

    const char SendMsg[] = "P_A";
    client_.write(SendMsg, sizeof(SendMsg));
    return client_.write(data, len) == len;
}


void send_output_addresses(const OutputAddress* addresses, int count) {
    if (count <= 0 || count > MAX_OUTPUTS) return;

    // Send only the specified portion of OutputAddresses
    ethernet_sender_send((const byte*)addresses, count * sizeof(OutputAddress));

    Serial.printf("Sent %d OutputAddress structs\n", count);
}

