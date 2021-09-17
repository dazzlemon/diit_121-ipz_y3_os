#ifndef CONSUMER_H
#define CONSUMER_H

class Consumer {
    void main_loop(void*);
};

void Consumer::main_loop(void*) {
    while (true) {
        if (true/*!buffer_is_full()*/) {
            // put item
        }
    }
}

#endif