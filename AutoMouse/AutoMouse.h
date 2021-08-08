#pragma once

#include "resource.h"

void TryActiveForegroundWindow();
void SetActive();
void ConsoleInputObserver();
std::thread g_TryThread;
std::thread g_EscapeThread;
static bool g_auto_enable = false;
static __int32 g_duration = 50;
