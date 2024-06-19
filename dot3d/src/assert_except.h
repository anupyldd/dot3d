#pragma once

/*
 * Custom assertions and exceptions.

 * This is a modified version of an assertion and exception system created
 * by ChiliTomatoNoodle in his Game Engine Infrastructure series
 * (https://www.youtube.com/playlist?list=PLqCJpWy5FoheHDzaP3R1eDFDOOff5TtBA).
*/

#include <string>
#include <source_location>
#include <iostream>
#include <sstream>
#include <exception>
#include <format>


namespace util
{
	namespace ae
	{
		/*****************************************************/
		//				Exception
		/*****************************************************/

		class Exception : public std::exception { };

		class ExceptionBuffered : public Exception
		{
		public:
			inline ExceptionBuffered() = default;
			inline ExceptionBuffered(const std::string& msg);
			inline const char* what() const noexcept override;
			inline virtual std::string Name() const = 0;
			std::string m_msg;
			mutable std::string m_buffer;
		};

		/*****************************************************/

		ExceptionBuffered::ExceptionBuffered(const std::string& msg)
			: m_msg(msg)
		{

		}
		const char* ExceptionBuffered::what() const noexcept
		{
			using namespace std::string_literals;
			m_buffer = "[" + Name() + "]";
			m_buffer += ": " + m_msg;
			return m_buffer.c_str();
		}
	}
}

// concatination, conversion to string or wstring,
// the ones with underscore are helpers
#define UT_CAT_(a, b) a##b
#define UT_CAT(a, b) UT_CAT_(a, b)
#define UT_TOSTR_(a) #a
#define UT_TOSTR(a) UT_TOSTR_(a)
#define UT_TOWSTR(a) UT_CAT(L, UT_TOSTR(a))

// assert output, uses util::ConsoleLog if present,
// other defaults to std::cout
/*#ifdef UT_LOGGER_PRESENT
#define UT_ASRT_LOG(str) util::ConsoleLog(str, util::log::LOG_LEVEL::FATAL); std::terminate()
#else
#define UT_ASRT_LOG(str) std::cout << str << '\n'; std::terminate()
#endif*/

/*****************************************************/

namespace util
{
	namespace ae
	{
		/*****************************************************/
		//				Custom exception
		/*****************************************************/

		class AssertFailed : public ExceptionBuffered
		{
		public:
			AssertFailed(const std::string& str)
				: ExceptionBuffered(str) { }
			virtual std::string Name() const override { return "AssertFailed"; }
		};

		/*****************************************************/

		enum class EFFECT
		{
			LOG,
			EXIT,
			EXCEPT
		};

		/*****************************************************/
		//				Assertion
		/*****************************************************/

		class Assertion
		{
		public:
			inline Assertion(const std::string& expr, const char* file, const char* func, int line, EFFECT effect = EFFECT::EXIT);
			inline ~Assertion();
			inline Assertion& Msg(const std::string& msg);
			template<typename T>
			inline Assertion& Watch(T&& val, const std::string& name);
			inline void Throw();
		private:
			const char* m_file = nullptr;
			const char* m_func = nullptr;
			int m_line = -1;
			EFFECT m_effect;
			std::ostringstream m_stream;
		};

		/*****************************************************/

		Assertion::Assertion(const std::string& expr, const char* file, const char* func, int line, EFFECT effect)
			: m_file(file), m_func(func), m_line(line), m_effect(effect)
		{
			m_stream << "[!] Assertion failed: " << expr << '\n';
		}

		Assertion::~Assertion()
		{
			std::cout << std::format("{} |   File: {} (line {}): {}\n",
				m_stream.str(),
				m_file, m_line, m_func);
			if (m_effect == EFFECT::EXIT) exit(1);
		}

		Assertion& Assertion::Msg(const std::string& msg)
		{
			m_stream << " |   Message: " << msg << '\n';
			return *this;
		}

		template<typename T>
		Assertion& Assertion::Watch(T&& val, const std::string& name)
		{
			m_stream << " |   " << name << ": " << std::forward<T>(val) << '\n';
			return *this;
		}

		void Assertion::Throw()
		{
			throw AssertFailed{ "NO" };
		}
	}
}

// for controlling what to include in release or debug
#ifndef UT_ACTIVE
#ifdef NOT_DEBUG
#define UT_ACTIVE false
#else
#define UT_ACTIVE true
#endif
#endif

#define UT_ASSERT(expr) (!UT_ACTIVE || bool(expr)) ? void(0) : (void)util::ae::Assertion{UT_TOSTR(expr), __FILE__, __FUNCTION__, __LINE__}
#define UT_CHECK(expr) (!UT_ACTIVE || bool(expr)) ? void(0) : (void)util::ae::Assertion{UT_TOSTR(expr), __FILE__, __FUNCTION__, __LINE__, util::ae::EFFECT::LOG}


