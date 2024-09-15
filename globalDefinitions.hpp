#pragma once

#define NONCOPYABLE(className) \
        className(const className&) = delete; \
        className& operator=(const className&) = delete;


