#include "ethernet_receiver.h"

#define PACKAGE_SIZE 1024

struct OutputAddress {
    uint8_t output_neuron;
    uint8_t output_synapse;
};

static byte mac_[6];
static IPAddress ip_;
static int localPort_;

static EthernetServer server_(8888);
static EthernetClient client_;

static char packetBuffer[PACKAGE_SIZE];
static int bytesReceived = 0;

static PacketHandler onPacketReceived = nullptr;

void ethernet_receiver_on_packet(PacketHandler handler) {
    onPacketReceived = handler;
}

void ethernet_receiver_init(byte mac[6], IPAddress ip, int port) {
    memcpy(mac_, mac, 6);
    ip_ = ip;
    localPort_ = port;
    server_ = EthernetServer(localPort_);

    Ethernet.begin(mac_, ip_);
    server_.begin();
    Serial.println("Receiver initialized.");
}

void ethernet_receiver_update(void) {
    if (!client_ || !client_.connected()) {
        client_ = server_.available();
        if (client_) {
            Serial.println("Client connected.");
            bytesReceived = 0;
        }
        return;
    }

    while (client_.available() && bytesReceived < PACKAGE_SIZE) {
        int byteRead = client_.read(packetBuffer + bytesReceived, PACKAGE_SIZE - bytesReceived);
        if (byteRead > 0) {
            bytesReceived += byteRead;
        }
    }

    if (bytesReceived > 0) {
        // Call user-defined handler
        if (onPacketReceived) {
            onPacketReceived(packetBuffer, bytesReceived);
        }

        // Log parsed OutputAddress data
        size_t count = bytesReceived / sizeof(OutputAddress);
        const OutputAddress* received = (const OutputAddress*)packetBuffer;

        for (size_t i = 0; i < count; ++i) {
            Serial.printf("Neuron: %u, Synapse: %u\n",
                          received[i].output_neuron, received[i].output_synapse);
        }

        // Reset buffer
        bytesReceived = 0;
    }
}
