#ifndef __BITOP_H
#define __BITOP_H

#define BITOP_SET(des, offset) des |= (1 << offset);
#define BITOP_CLR(des, offset) des &= ~(1 << offset);
#define BITOP_TOG(des, offset) des ^= (1 << offset);
#endif