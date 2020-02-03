#ifndef DI_CPP_UTIL_LOGGING_TIMER_H
#define DI_CPP_UTIL_LOGGING_TIMER_H

#include <chrono>
#include <ctime>
#include <string>
#include <locale>
#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/facilities/overload.hpp>

namespace di_cpp{ namespace util{ 

class Timer
/*
 * Modified RAII timer based on Félix Abecassis
 * https://felix.abecassis.me/2011/09/cpp-timer-raii/
 */
{
public:
  Timer(const std::string& message, bool high_resolution = false): message_ (message),
    high_resol_(high_resolution),
    start_(std::chrono::high_resolution_clock::now())
    {
    }
  ~Timer()
    {
      auto end = std::chrono::high_resolution_clock::now();
      const double elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start_).count() * (high_resol_ ? 1.0 : 1.e-6);
      const std::string time_unit = high_resol_ ? "us": "s";
      std::cerr << "DI_CPP_TIMING\t" << elapsed << '\t' << time_unit << '\t' << message_ << std::endl;
    }
private:
  const std::string message_;
  const bool high_resol_;
  const std::chrono::system_clock::time_point start_;
};


// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
inline const std::string CurrentDateTime() {
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    std::locale::global(std::locale(""));
    std::time_t t = std::time(NULL);
    char buf[100];
    strftime(buf, sizeof(buf), "%Y-%m-%d_%H-%M-%S", std::localtime(&t));
    return buf;
}

} // namespace util
} // namespace di_cpp

#define DICPP_TIMER_MESSAGE_1(X) std::string(__FILE__ ":" BOOST_PP_STRINGIZE(__LINE__) " ") + __PRETTY_FUNCTION__
#define DICPP_TIMER_MESSAGE_2(X) BOOST_PP_STRINGIZE(X)
#define DICPP_TIMER_MESSAGE(...) BOOST_PP_OVERLOAD(DICPP_TIMER_MESSAGE_, ,##__VA_ARGS__)(__VA_ARGS__)

#define DITIMER(...) di_cpp::util::Timer dicpp_timer_(DICPP_TIMER_MESSAGE(__VA_ARGS__));

#endif // DI_CPP_UTIL_LOGGING_TIMER_H
