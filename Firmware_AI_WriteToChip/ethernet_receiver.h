#ifndef ETHERNET_RECEIVER_H
#define ETHERNET_RECEIVER_H

#include <Arduino.h>
#include <NativeEthernet.h>

typedef void (*PacketHandler)(const char* data, size_t len);

void ethernet_receiver_init(byte mac[6], IPAddress ip, int port);
void ethernet_receiver_update(void);
void ethernet_receiver_on_packet(PacketHandler handler);

#endif
