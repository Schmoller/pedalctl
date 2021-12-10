#include "errors.hpp"
#include <libusb.h>

const std::string_view &describeLibUSBError(int code) {
    switch (code) {
        case LIBUSB_ERROR_NOT_FOUND:
            return Errors::NotFound;
        case LIBUSB_ERROR_IO:
            return Errors::IO;
        case LIBUSB_ERROR_ACCESS:
            return Errors::Access;
        case LIBUSB_ERROR_NO_DEVICE:
            return Errors::NoDevice;
        case LIBUSB_ERROR_BUSY:
            return Errors::Busy;
        case LIBUSB_ERROR_TIMEOUT:
            return Errors::Timeout;
        case LIBUSB_ERROR_INTERRUPTED:
            return Errors::Interrupted;
        case LIBUSB_ERROR_NO_MEM:
            return Errors::NoMem;
        default:
            return Errors::Unknown;
    }
}
