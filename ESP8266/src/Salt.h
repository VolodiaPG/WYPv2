#ifndef SALT_H
#define SALT_H

inline volatile int trng() {
    return *((volatile int*)0x3FF20E44);
}

#endif