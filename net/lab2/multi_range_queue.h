#pragma once

#include <list>
#include <mutex>
#include <utility>
#include <assert.h>

class MultiRangeQueue {
public:
    void push(int from, int to) {
        std::lock_guard guard(mutex_);
        assert(from <= to);
        auto e                  = std::pair(from, to);
        auto it                 = ranges_.begin();
        bool done               = false;
        bool should_merge_after = false;
        while (it != ranges_.end()) {
            if (e.second < it->first) { break; }
            if (e.first > e.second) {
                ++it;
                continue;
            }
            if (e.first < it->first) { it->first = e.first; }
            if (e.second > it->second) {
                it->second         = e.second;
                should_merge_after = true;
            }
            done = true;
            break;
        }
        if (!done) {
            ranges_.insert(it, e);
        } else if (should_merge_after) {
            auto& p = *it;
            while (++it != ranges_.end()) {
                auto& q = *it;
                if (p.second < q.first) { break; }
                p.second = std::max(p.second, q.second);
                q.first  = q.second + 1; //<! mark as invalid
                if (p.second == q.second) { break; }
            }
            auto last = std::remove_if(
                ranges_.begin(), ranges_.end(), [](const auto& e) {
                    return e.first > e.second;
                });
            ranges_.erase(last, ranges_.end());
        }
    }

    int pop() {
        std::lock_guard guard(mutex_);
        assert(!ranges_.empty());
        auto& range = ranges_.front();
        int   value = range.first++;
        if (range.first > range.second) { ranges_.pop_front(); }
        return value;
    }

    bool empty() const {
        return ranges_.empty();
    }

private:
    std::mutex                     mutex_;
    std::list<std::pair<int, int>> ranges_;
};
