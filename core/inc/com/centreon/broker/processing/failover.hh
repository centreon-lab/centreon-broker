/*
** Copyright 2011-2013,2015-2017 Centreon
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

#ifndef CCB_PROCESSING_FAILOVER_HH
#  define CCB_PROCESSING_FAILOVER_HH

#  include <climits>
#  include <ctime>
#  include <QMutex>
#  include <QVector>
#  include <string>
#  include "com/centreon/broker/io/endpoint.hh"
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/multiplexing/subscriber.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/processing/acceptor.hh"
#  include "com/centreon/broker/processing/thread.hh"

CCB_BEGIN()

// Forward declarations.
namespace           io {
  class             properties;
}
namespace           stats {
  class             builder;
}

namespace           processing {
  /**
   *  @class failover failover.hh "com/centreon/broker/processing/failover.hh"
   *  @brief Failover thread.
   *
   *  Thread that provide failover on output endpoints.
   *
   *  Multiple failover can be forwarded.
   */
  class             failover : public thread {
    friend class    stats::builder;

   public:
                    failover(
                      misc::shared_ptr<io::endpoint> endp,
                      misc::shared_ptr<multiplexing::subscriber> sbscrbr,
                      std::string const& name);
                    ~failover();
    void            add_secondary_endpoint(
                      misc::shared_ptr<io::endpoint> endp);
    void            exit();
    time_t          get_buffering_timeout() const throw ();
    bool            get_initialized() const throw ();
    time_t          get_retry_interval() const throw ();
    void            run();
    void            set_buffering_timeout(time_t secs);
    void            set_failover(
                      misc::shared_ptr<processing::failover> fo);
    void            set_retry_interval(time_t retry_interval);
    void            update();
    bool            wait(unsigned long time = ULONG_MAX);

   protected:
    // From stat_visitable
    virtual std::string
                    _get_state();
    virtual unsigned int
                    _get_queued_events();
    virtual uset<unsigned int>
                    _get_read_filters();
    virtual uset<unsigned int>
                    _get_write_filters();
    virtual void    _forward_statistic(io::properties& tree);

   private:
                    failover(failover const& other);
    failover&       operator=(failover const& other);
    void            _launch_failover();
    void            _update_status(std::string const& status);

    // Data that doesn't require locking.
    volatile time_t _buffering_timeout;
    misc::shared_ptr<io::endpoint>
                    _endpoint;
    std::vector<misc::shared_ptr<io::endpoint> >
                    _secondary_endpoints;
    misc::shared_ptr<failover>
                    _failover;
    bool            _failover_launched;
    volatile bool   _initialized;
    time_t          _next_timeout;
    volatile time_t _retry_interval;
    misc::shared_ptr<multiplexing::subscriber>
                    _subscriber;
    volatile bool   _update;

    // Status.
    std::string     _status;
    mutable QMutex  _statusm;

    // Stream.
    misc::shared_ptr<io::stream>
                    _stream;
    mutable QMutex  _streamm;
  };
}

CCB_END()

#endif // !CCB_PROCESSING_FAILOVER_HH
