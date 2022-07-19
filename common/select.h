#ifndef __CONSUMERSELECT__
#define __CONSUMERSELECT__

#include <string>
#include <vector>
#include <queue>
#include <unordered_map>
#include <set>
#include <hiredis/hiredis.h>
#include "selectable.h"

namespace swss {

class Select
{
public:
    Select();
    ~Select();

    /* Add object for select */
    void addSelectable(Selectable *selectable);

    /* Remove object from select */
    void removeSelectable(Selectable *selectable);

    /* Add multiple objects for select */
    void addSelectables(std::vector<Selectable *> selectables);

    enum {
        OBJECT = 0,
        ERROR = 1,
        TIMEOUT = 2,
        SIGNALINT = 3,// Read operation interrupted by a signal
    };

    int select(Selectable **c, int timeout = -1, bool interrupt_on_signal = false);
    bool isQueueEmpty();

    /**
     * @brief Result to string.
     *
     * Convert select operation result to string representation.
     */
    static std::string resultToString(int result);

private:
    struct cmp
    {
        bool operator()(const Selectable* a, const Selectable* b) const
        {
            /* Choose Selectable with highest priority first */
            if (a->getPri() > b->getPri())
                return true;
            else if (a->getPri() < b->getPri())
                return false;

            /* if the priorities are equal */
            /* use Selectable which was selected earlier */
            if (a->getLastUsedTime() < b->getLastUsedTime())
                return true;
            else if (a->getLastUsedTime() > b->getLastUsedTime())
                return false;

            /* when a == b */
            return false;
        }
    };

    int poll_descriptors(Selectable **c, unsigned int timeout, bool interrupt_on_signal);

    int m_epoll_fd;
    std::unordered_map<int, Selectable *> m_objects;
    std::set<Selectable *, Select::cmp> m_ready;
};

}

#endif
