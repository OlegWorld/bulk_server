#pragma once

#include <list>
#include <vector>
#include <mutex>
#include "abstract_observer.h"
#include "command_bulk.h"

class BulkHandler {
public:
    BulkHandler(size_t bulk_size);

    void subscribe(AbstractObserver* obs);

    void add_command(command_name_t&& name);

    void finalize_bulk();

    CommandBulk* exclusive_bulk();

    void submit_exclusive(CommandBulk* bulk);

private:
    CommandBulk* new_bulk();

    void push_current();

    void recycle_data();

private:
    const size_t m_bulk_size;

    std::vector<AbstractObserver*> m_observers;

    CommandBulk* m_current_bulk;
    std::mutex m_bulk_lock;

    std::list<CommandBulk> m_data;
};
