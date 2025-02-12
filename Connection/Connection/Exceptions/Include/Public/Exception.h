#pragma once

#include <boost/stacktrace.hpp>

namespace Connection
{
	class Exception
	{
	public:
		Exception(
			const char* description,
			boost::stacktrace::stacktrace&& stackTrace) :
			m_Description(description),
			m_StackTrace(std::move(stackTrace)) { }

		virtual ~Exception() = default;

		boost::stacktrace::stacktrace& GetStackTrace() { return m_StackTrace; }
		const char* GetDescription() { return m_Description; }

	private:
		const char* m_Description;
		boost::stacktrace::stacktrace m_StackTrace;
	};
}