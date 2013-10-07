/*
** Copyright 2011-2013 Merethis
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

#ifndef CCB_IO_DATA_HH
#  define CCB_IO_DATA_HH

#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace                  io {
  /**
   *  @class data data.hh "com/centreon/broker/io/data.hh"
   *  @brief Data abstraction.
   *
   *  Data is the core element that is transmitted through Centreon
   *  Broker. It is an interface that is implemented by all specific
   *  module data that wish to be transmitted by the multiplexing
   *  engine.
   */
  class                    data {
  public:
    enum                   data_category {
      neb = 1,
      bbdo,
      storage,
      correlation,
      internal = 65535
    };

                           data();
                           data(data const& d);
    virtual                ~data();
    data&                  operator=(data const& d);
    virtual unsigned int   type() const = 0;
    static unsigned int    data_type(
			     unsigned short category,
			     unsigned short element) {
      return (category << 16 | element);
    }
  };
}

CCB_END()

#endif // !CCB_IO_DATA_HH
