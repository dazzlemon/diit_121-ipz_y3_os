#ifndef PRODUCER_H
#define PRODUCER_H

#include "consumer_producer_base.h"

class Producer : public ConsumerProducerBase {
protected:
    virtual void _main_loop() final;
    using ConsumerProducerBase::ConsumerProducerBase;
};
#endif