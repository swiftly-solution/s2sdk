#ifndef INTERFACES_H
#define INTERFACES_H

#ifdef _WIN32
#pragma once
#endif

typedef void* (*CreateInterfaceFn)(const char* pName, const char* pReturnCode);

#endif