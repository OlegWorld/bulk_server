#include "bulk_handler.h"


BulkHandler::BulkHandler(size_t bulk_size)
:   m_bulk_size(bulk_size),
    m_current_bulk(nullptr)
{ }

void BulkHandler::subscribe(AbstractObserver* obs) {
    m_observers.push_back(obs);
}

void BulkHandler::add_command(command_name_t&& name) {
    std::lock_guard<std::mutex> lck(m_bulk_lock);

    if (!m_current_bulk) {
        m_current_bulk = new_bulk();
    }

    m_current_bulk->push_command(std::move(name));

    if (m_current_bulk->size() == m_bulk_size) {
        push_current();
    }

    recycle_data();
}

void BulkHandler::finalize_bulk() {
    std::lock_guard<std::mutex> lck(m_bulk_lock);

    if (!m_current_bulk->empty()) {
        push_current();

        recycle_data();
    }
}

CommandBulk* BulkHandler::exclusive_bulk() {
    return new_bulk();
}

void BulkHandler::submit_exclusive(CommandBulk* bulk) {
    for (auto& obs : m_observers) {
        obs->add(bulk);
    }

    recycle_data();
}

CommandBulk* BulkHandler::new_bulk() {
    m_data.emplace_back();
    return &m_data.back();
}

void BulkHandler::push_current() {
    for (auto& obs : m_observers) {
        obs->add(m_current_bulk);
    }

    m_current_bulk = nullptr;
}

void BulkHandler::recycle_data() {
    auto it = m_data.begin();
    while (it != m_data.end()) {
        if (it->recyclable()) {
            it = m_data.erase(it);
        }
        ++it;
    }
}

