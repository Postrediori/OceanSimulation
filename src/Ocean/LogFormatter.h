#pragma once
#include <plog/Record.h>
#include <plog/Util.h>
#include <iomanip>

namespace plog {
    class LogFormatter {
    public:
        static util::nstring header() {
            return util::nstring();
        }

        static util::nstring format(const Record& record) {
            util::nostringstream ss;
            ss << std::setw(5) << std::left;
            ss << severityToString(record.getSeverity()) << PLOG_NSTR(" ");
            ss << PLOG_NSTR("[") << record.getTid() << PLOG_NSTR("] ");
            ss << PLOG_NSTR("[") << record.getFunc() << PLOG_NSTR("@") << record.getLine() << PLOG_NSTR("] ");
            ss << record.getMessage() << PLOG_NSTR("\n");

            return ss.str();
        }
    };
}
