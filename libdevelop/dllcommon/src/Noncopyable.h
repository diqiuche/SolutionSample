#ifndef EYEGLE_COMMON_APPLOG_H
#define EYEGLE_COMMON_APPLOG_H

namespace itstation {
	namespace common {
		class Noncopyable
		{
		protected:
			Noncopyable() = default;
			virtual ~Noncopyable() = default;
			Noncopyable(const Noncopyable&) = delete;
			Noncopyable& operator = (const Noncopyable&) = delete;
		};
	

	}
}

#endif

