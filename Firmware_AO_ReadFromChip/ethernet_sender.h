#ifndef ETHERNET_SENDER_H
#define ETHERNET_SENDER_H

#include <Arduino.h>
#include <Ethernet.h>
#include "recurrency.h" 

void ethernet_sender_init(byte mac[6], IPAddress ip, IPAddress targetIP, int port);
void ethernet_sender_send(const byte* data, int len);
void send_output_addresses(const OutputAddress* addresses, int count);

#endif
