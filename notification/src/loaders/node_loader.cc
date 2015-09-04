/*
** Copyright 2014 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
*/

#include <sstream>
#include <QVariant>
#include <QSet>
#include <QSqlError>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/notification/loaders/node_loader.hh"

using namespace com::centreon::broker::notification;
using namespace com::centreon::broker::notification::objects;

node_loader::node_loader() {}

/**
 *  Load the nodes from the database.
 *
 *  @param[in] db       An open connection to the database.
 *  @param[out] output  A node builder object to register the nodes.
 */
void node_loader::load(QSqlDatabase* db, node_builder* output) {
  // If we don't have any db or output, don't do anything.
  if (!db || !output)
    return;

  logging::debug(logging::medium)
    << "notification: loading nodes from the database";

  QSqlQuery query(*db);

  // Performance improvement, as we never go back.
  query.setForwardOnly(true);

  // Load hosts.
  if (!query.exec("SELECT host_id FROM cfg_hosts"))
    throw (exceptions::msg()
           << "notification: cannot load hosts from database: "
           << query.lastError().text());
  while (query.next()) {
    unsigned int host_id = query.value(0).toUInt();
    node::ptr n(new node);
    n->set_node_id(node_id(host_id));
    logging::config(logging::low)
      << "notification: loading host " << host_id << " from database";
    output->add_node(n);
  }

  // Load services.
  if (!query.exec("SELECT hsr.host_host_id, hsr.service_service_id"
                  "  FROM cfg_hosts_services_relations AS hsr"
                  "  LEFT JOIN cfg_services AS s"
                  "    ON hsr.service_service_id=s.service_id"))
    throw (exceptions::msg()
           << "notification: cannot load services from database: "
           << query.lastError().text());
  while (query.next()) {
    unsigned int host_id = query.value(0).toUInt();
    unsigned int service_id = query.value(1).toUInt();
    node::ptr n(new node);
    n->set_node_id(node_id(host_id, service_id));
    logging::config(logging::low) << "notification: loading service ("
      << host_id << ", " << service_id << ") from database";
    output->add_node(n);
  }
}