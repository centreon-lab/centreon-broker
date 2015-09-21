/*
** Copyright 2011-2012 Centreon
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

#ifndef CCB_PROCESSING_STAT_VISITABLE_HH
#  define CCB_PROCESSING_STAT_VISITABLE_HH

#  include <string>
#  include "com/centreon/broker/io/properties.hh"
#  include "com/centreon/broker/timestamp.hh"
#  include "com/centreon/broker/misc/unordered_hash.hh"
#  include "com/centreon/broker/misc/processing_speed_computer.hh"

CCB_BEGIN()

namespace                        processing {
  /**
   *  @class stat_visitable stat_visitable.hh "com/centreon/broker/processing/stat_visitable.hh"
   *  @brief Represent a processing thread that is visitable.
   */
  class                          stat_visitable {
  public:
                                 stat_visitable(
                                   std::string const& name = std::string());
                                 ~stat_visitable();

    std::string const&           get_name() const;
    virtual void                 stats(io::properties& tree);
    void                         set_last_connection_times(
                                   timestamp last_connection_success
                                     = timestamp::now(),
                                   timestamp last_connection_attempt
                                     = timestamp::now());

  protected:
    std::string                 _name;
    std::string                 _last_error;
    timestamp                   _last_connection_attempt;
    timestamp                   _last_connection_success;
    misc::processing_speed_computer
                                _event_processing_speed;

    virtual std::string         _get_state() = 0;
    virtual unsigned int        _get_queued_events() = 0;
    virtual uset<unsigned int>  _get_read_filters() = 0;
    virtual uset<unsigned int>  _get_write_filters() = 0;
    virtual void                _forward_statistic(io::properties& tree);

  private:
                                 stat_visitable(stat_visitable const& other);
    stat_visitable&              operator=(stat_visitable const& other);
  };
}

CCB_END()

#endif // !CCB_PROCESSING_STAT_VISITABLE_HH