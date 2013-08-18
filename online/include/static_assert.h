#ifndef __STATIC_ASSERT_H
#define __STATIC_ASSERT_H

#define STATIC_ASSERT( condition, name )\
          typedef char assert_failed_ ## name [ (condition) ? 1 : -1 ];

#endif
