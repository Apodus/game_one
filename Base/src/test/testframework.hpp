#pragma once
#include "../../lib/win32/gmock-1.6.0/include/gmock/gmock.h"
#include "../../lib/win32/gmock-1.6.0/gtest/include/gtest/gtest.h"

#ifdef _WIN32

#include <crtdbg.h>
struct CheckMemory
{
  _CrtMemState state1;
  _CrtMemState state2;
  _CrtMemState state3;
  CheckMemory()
  {
    _CrtMemCheckpoint(&state1);
  }
  ~CheckMemory()
  {
    _CrtMemCheckpoint(&state2);
    // using google test you can just do this.
    bool MemoryLeaked = _CrtMemDifference( &state3, &state1, &state2) != 0;
    EXPECT_FALSE(MemoryLeaked);
    // else just do this to dump the leaked blocks to stdout.
    if( _CrtMemDifference( &state3, &state1, &state2) )
      _CrtMemDumpStatistics( &state3 );
  }
};

#else

struct CheckMemory {
};

#endif