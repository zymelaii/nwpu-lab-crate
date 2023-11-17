#pragma once

#include <string.h>
#include <assert.h>

class ArgParser {
public:
    ArgParser(int argc, char* argv[])
        : cur_{1}
        , argc_{argc}
        , argv_{argv} {}

    bool named_arg(const char* opt, const char*& value) {
        if (is(opt)) {
            next();
            value = current();
            next();
            return true;
        } else {
            return false;
        }
    }

    bool flag(const char* opt) {
        if (is(opt)) {
            next();
            return true;
        } else {
            return false;
        }
    }

    bool empty() const {
        return cur_ >= argc_;
    }

    void ignore_once() {
        next();
    }

protected:
    bool is(const char* opt) const {
        return strcmp(current(), opt) == 0;
    }

    void next() {
        assert(cur_ < argc_);
        ++cur_;
    }

    const char* current() const {
        assert(!empty());
        return argv_[cur_];
    }

private:
    int    cur_;
    int    argc_;
    char** argv_;
};
