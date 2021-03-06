// This file is a part of the IncludeOS unikernel - www.includeos.org
//
// Copyright 2017 Oslo and Akershus University College of Applied Sciences
// and Alfred Bratterud
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once
#ifndef UPLINK_WS_UPLINK_HPP
#define UPLINK_WS_UPLINK_HPP

#include "transport.hpp"

#include <net/inet4>
#include <net/http/client.hpp>
#include <net/ws/websocket.hpp>
#include <liveupdate.hpp>
#include <util/timer.hpp>
#include <util/logger.hpp>

namespace uplink {

class WS_uplink {
public:
  static const std::string UPLINK_CFG_FILE;

  struct Config
  {
    std::string url;
    std::string token;
    bool        reboot = true;
  };

  WS_uplink(net::Inet<net::IP4>&);

  void start(net::Inet<net::IP4>&);

  void auth();

  void dock();

  void handle_transport(Transport_ptr);

  void send_ident();

  void send_log(const char*, size_t);

  void flush_log();

  void send_uplink();

  void update(const std::vector<char>& buffer);

  void send_error(const std::string& err);

  void send_stats();

  void send_message(Transport_code, const char* data, size_t len);

  bool is_online() const
  { return ws_ != nullptr and ws_->is_alive(); }

  void panic(const char* why);

private:
  net::Inet<net::IP4>&          inet_;
  std::unique_ptr<http::Client> client_;
  net::WebSocket_ptr            ws_;
  std::string                   id_;
  std::string                   token_;
  std::string                   binary_hash_;

  Config config_;

  Transport_parser parser_;

  Timer retry_timer;
  uint8_t retry_backoff = 0;

  std::vector<char> logbuf_;

  void inject_token(http::Request& req, http::Client::Options&, const http::Client::Host)
  {
    if (not token_.empty())
      req.header().add_field("Authorization", "Bearer " + token_);
  }

  std::string auth_data() const;

  void handle_auth_response(http::Error err, http::Response_ptr res, http::Connection&);

  void retry_auth();

  void establish_ws(net::WebSocket_ptr ws);

  void handle_ws_close(uint16_t code);

  void parse_transport(net::WebSocket::Message_ptr msg);

  void read_config();

  void parse_config(const std::string& cfg);

  void store(liu::Storage& store, const liu::buffer_t*);

  void restore(liu::Restore& store);

}; // < class WS_uplink


} // < namespace uplink

#endif
