/*
** Copyright 2011-2013,2015 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#include <memory>
#include "com/centreon/broker/config/parser.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/sql/connector.hh"
#include "com/centreon/broker/sql/factory.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::sql;

/**************************************
*                                     *
*            Local Objects            *
*                                     *
**************************************/

/**
 *  Find a parameter in configuration.
 *
 *  @param[in] cfg Configuration object.
 *  @param[in] key Property to get.
 *
 *  @return Property value.
 */
static QString const& find_param(
                        config::endpoint const& cfg,
                        QString const& key) {
  QMap<QString, QString>::const_iterator it(cfg.params.find(key));
  if (cfg.params.end() == it)
    throw (exceptions::msg() << "SQL: no '" << key
           << "' defined for endpoint '" << cfg.name << "'");
  return (it.value());
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
factory::factory() {}

/**
 *  Copy constructor.
 *
 *  @param[in] f Object to copy.
 */
factory::factory(factory const& f) : io::factory(f) {}

/**
 *  Destructor.
 */
factory::~factory() {}

/**
 *  Assignment operator.
 *
 *  @param[in] f Object to copy.
 *
 *  @return This object.
 */
factory& factory::operator=(factory const& f) {
  io::factory::operator=(f);
  return (*this);
}

/**
 *  Clone the factory.
 *
 *  @return Copy of the factory.
 */
io::factory* factory::clone() const {
  return (new factory(*this));
}

/**
 *  Check if an endpoint match a configuration.
 *
 *  @param[in] cfg       Endpoint configuration.
 *  @param[in] is_input  true if the endpoint should be an input.
 *  @param[in] is_output true if the endpoint should be an output.
 *
 *  @return true if the endpoint match the configuration.
 */
bool factory::has_endpoint(
                config::endpoint& cfg,
                bool is_input,
                bool is_output) const {
  (void)is_input;
  bool is_sql(!cfg.type.compare("sql", Qt::CaseInsensitive)
              && is_output);
  if (is_sql) {
    // Default transaction timeout.
    if (cfg.params.find("read_timeout") == cfg.params.end()) {
      cfg.params["read_timeout"] = "2";
      cfg.read_timeout = 2;
    }
  }
  return (is_sql);
}

/**
 *  Create an endpoint.
 *
 *  @param[in]  cfg         Endpoint configuration.
 *  @param[in]  is_input    true if the endpoint should be an input.
 *  @param[in]  is_output   true if the endpoint should be an output.
 *  @param[out] is_acceptor Will be set to false.
 *
 *  @return New endpoint.
 */
io::endpoint* factory::new_endpoint(
                         config::endpoint& cfg,
                         bool is_input,
                         bool is_output,
                         bool& is_acceptor) const {
  (void)is_input;
  (void)is_output;

  // Database configuration.
  database_config dbcfg(cfg);

  // Cleanup check interval.
  unsigned int cleanup_check_interval(0);
  {
    QMap<QString, QString>::const_iterator
      it(cfg.params.find("cleanup_check_interval"));
    if (it != cfg.params.end())
      cleanup_check_interval = it.value().toUInt();
  }

  // Instance timeout
  // By default, 5 minutes.
  unsigned int instance_timeout(5 * 60);
  {
    QMap<QString, QString>::const_iterator
      it(cfg.params.find("instance_timeout"));
    if (it != cfg.params.end())
      instance_timeout = it.value().toUInt();
  }

  // Use state events ?
  bool wse(false);
  {
    QMap<QString, QString>::const_iterator
      it(cfg.params.find("with_state_events"));
    if (it != cfg.params.end())
      wse = config::parser::parse_boolean(*it);
  }

  // Connector.
  std::auto_ptr<sql::connector> c(new sql::connector);
  c->connect_to(
       dbcfg,
       cleanup_check_interval,
       instance_timeout,
       wse);
  is_acceptor = false;
  return (c.release());
}
