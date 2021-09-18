#ifndef CONSUMER_H
#define CONSUMER_H

#include "consumer_producer_base.h"

class Consumer : public ConsumerProducerBase {
protected:
    virtual void _main_loop() final;
    using ConsumerProducerBase::ConsumerProducerBase;
};
#endif