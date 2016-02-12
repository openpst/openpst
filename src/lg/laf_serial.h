#include "laf.h"
#include "qc/hdlc_encoder.h"

namespace OpenPST {
    namespace LG {
        class LafSerial : public serial::Serial
        {

            HdlcEncoder hdlcEncoder;

            LafSerial(std::string port, int baudrate = 115200, serial::Timeout timeout = serial::Timeout::simpleTimeout(1000));

            ~LafSerial();

            std::string exec(std::string command);

            void initialize();

             /**
             * @brief write
             * @overload Serial:: write(uint8_t *data, size_t size)
             * @return size_t
             */
             size_t write(uint8_t *data, size_t size);

             /**
             * @brief read
             * @overload Serial::read(uint8_t *buf, size_t size)
             * @return size_t
             */
             size_t read(uint8_t *buf, size_t size);

             /**
             * @brief write
             * @overload Serial::write(std::vector<uint8_t> &data)
             * @return size_t
             */
             size_t write(std::vector<uint8_t> &data);

             /**
             * @brief read
             * @overload Serial::read(std::vector<uint8_t> &buffer, size_t size)
             * @return size_t
             */
             size_t read(std::vector<uint8_t> &buffer, size_t size);
        };

        class LafResponseError : public std::exception
        {
        	const LafResponseError& operator=(LafResponseError);
        	std::string _what;
            public:
            	LafResponseError(std::string message) : _what(message)  {}
            	LafResponseError(const LafResponseError& second) : _what(second._what) {}
            	virtual ~LafResponseError() throw() {}
            	virtual const char* what() const throw () {
            		return _what.c_str();
            	}
        };

        class LafInvalidArgument : public std::invalid_argument
        {
        	const LafInvalidArgument& operator=(LafInvalidArgument);
        	std::string _what;
            public:
            	LafInvalidArgument(std::string message) : invalid_argument(message), _what(message)  {}
            	LafInvalidArgument(const LafInvalidArgument& second) : invalid_argument(second), _what(second._what) {}
            	virtual ~LafInvalidArgument() throw() {}
            	virtual const char* what() const throw () {
            		return _what.c_str();
            	}
        };

    }
}
