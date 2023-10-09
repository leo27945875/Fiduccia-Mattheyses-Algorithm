#pragma once

#define DEBUG 0
#define HALF_INIT_GROUP 1

#if DEBUG == 1
    #define LOG(x) std::cout << x << std::endl
    #define LOGKV(x, y) std::cout << x << " = " << y <<  std::endl

    #define END_LINE std::cout << std::endl
    #define LOOP_START(x) std::cout << "========================== " << "Loop " << x << " ==========================\n" << std::endl
    #define INNER_LOOP_START(x) std::cout << "========================== " << "Inner Loop " << x << " ==========================\n" << std::endl
    #define OUTER_LOOP_START(x) std::cout << "============================ " << "Outer Loop " << x << " ============================\n" << std::endl
    #define DDASH_LINE std::cout << "==============================================================" << std::endl
    #define DASH_LINE std::cout << "--------------------------------------------------------------" << std::endl
    #define MOVE_INFO(x) std::cout << "Move " << x->m_number << "  (from: " << (x->m_group + 1) << " to: " << ((!x->m_group) + 1) << ")" << std::endl
#else
    #define LOG(x) 
    #define LOGKV(x, y) 

    #define END_LINE 
    #define LOOP_START(x) 
    #define INNER_LOOP_START(x) 
    #define OUTER_LOOP_START(x) 
    #define DDASH_LINE 
    #define DASH_LINE 
    #define MOVE_INFO(x) 
#endif
