#pragma once

#include <string>
#include <optional>

#define define_error(name, value) constexpr const std::string_view name = value

namespace Errors {
define_error(Access, "Access denied");
define_error(NotFound, "Device not found");
define_error(NoDevice, "No such device");
define_error(Busy, "Device is being used by another process");
define_error(Timeout, "Timeout while communicating with device");
define_error(Interrupted, "Communication was interrupted");
define_error(NoMem, "Out of memory");
define_error(IO, "An IO error occurred while communicating with device");
define_error(Unknown, "An unknown error occurred");
}

const std::string_view &describeLibUSBError(int code);

#undef define_error