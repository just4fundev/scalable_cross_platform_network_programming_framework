#pragma once

#ifdef Connector_EXPORTS
#define Connector_API __declspec(dllexport)
#else
#define Connector_API __declspec(dllimport)
#endif