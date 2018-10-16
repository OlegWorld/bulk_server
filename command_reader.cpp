#include "command_reader.h"

CommandReader::CommandReader(BulkHandler& handler)
:   m_handler(handler),
    m_current_state(std::make_unique<NormalState>(m_stream)),
    m_other_state(std::make_unique<BracedState>(m_stream))
{ }

void CommandReader::scan_input(const std::string& data) {
    m_stream.clear();
    m_stream.str(data);
    m_current_state->read_commands(this);
}

void CommandReader::push_command(command_name_t&& name) {
    m_handler.add_command(std::move(name));
}

void CommandReader::switch_state() {
    std::swap(m_current_state, m_other_state);
    m_current_state->read_commands(this);
}

CommandBulk* CommandReader::get_bulk() {
    return m_handler.exclusive_bulk();
}

void CommandReader::push_bulk(CommandBulk* bulk) {
    m_handler.submit_exclusive(bulk);
}

void CommandReader::finalize_bulk() {
    m_handler.finalize_bulk();
}

NormalState::NormalState(std::istream& input)
:   m_input(input),
    m_current_size(0)
{ }

void NormalState::open_brace(CommandReader* reader) {
    reader->finalize_bulk();
    reader->switch_state();
}

void NormalState::close_brace(CommandReader*) { }

void NormalState::read_commands(CommandReader* reader) {
    command_name_t name;
    while (std::getline(m_input, name)) {
        if (name.empty()) {
            continue;
        }

        if (name == "{") {
            open_brace(reader);
            break;
        }

        if (name == "}") {
            continue;
        }

        reader->push_command(std::move(name));
    }
}

BracedState::BracedState(std::istream& input)
:   m_brace_counter(0),
    m_input(input),
    m_bulk(nullptr)
{ }

void BracedState::open_brace(CommandReader*) { }

void BracedState::close_brace(CommandReader* reader) {
    reader->push_bulk(m_bulk);
    m_bulk = nullptr;
    reader->switch_state();
}

void BracedState::read_commands(CommandReader* reader) {
    if (!m_bulk) {
        m_bulk = reader->get_bulk();
    }

    command_name_t name;
    while(std::getline(m_input, name)) {
        if (name.empty()) {
            continue;
        }

        if (name == "{") {
            m_brace_counter++;
            continue;
        }

        if (name == "}") {
            if (!m_brace_counter) {
                close_brace(reader);
                break;
            } else {
                --m_brace_counter;
            }

            continue;
        }

        m_bulk->push_command(std::move(name));
    }
}

