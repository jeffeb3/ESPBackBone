
#pragma once

#include <Arduino.h>

#include <functional>

namespace espbb
{

/// Starts the mqtt service, which connects to a server, and starts a task to manage the connection.
///
/// @note This will connect to MQTT, with some reasonable defaults, and manage some simple publish
/// and subscribe tasks. If you want more control, I suggest going to the PubSubClient directly.
///
/// @param address the address of the server
/// @param name the mqtt client name. A unique name will mean the server will keep the correct
/// cached state.
/// @param port the port of the mqtt server
void setMqtt(const IPAddress& address, const String& name, const int port=1883);

/// Returns true if connected to the server.
bool connected();

/// The type of object that is used in callbacks.
typedef std::function<void(byte*, unsigned int)> CallbackFun_t;

/// Add a callback to a message. The topic doesn't support the normal mqtt wildcards, at the moment.
void subscribe(const String& topic, const CallbackFun_t& callback);

/// Adds this payload/topic to an output queue. It will be sent when the task executes.
void publish(const String& topic, const String& payload);

} // namespace

