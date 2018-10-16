#pragma once

#include <string>
#include <sstream>
#include <iostream>
#include <memory>
#include <list>
#include "bulk_handler.h"

class ReaderState;
class NormalState;
class BracedState;

class CommandReader {
public:
    CommandReader(BulkHandler& handler);

    ~CommandReader() = default;

    void scan_input(const std::string& data);

    void push_command(command_name_t&& name);

    void switch_state();

    CommandBulk* get_bulk();

    void push_bulk(CommandBulk* bulk);

    void finalize_bulk();

private:
    BulkHandler& m_handler;

    std::istringstream m_stream;

    std::unique_ptr<ReaderState> m_current_state;
    std::unique_ptr<ReaderState> m_other_state;
};

class ReaderState {
public:
    virtual ~ReaderState() = default;
    virtual void open_brace(CommandReader*) = 0;
    virtual void close_brace(CommandReader*) = 0;
    virtual void read_commands(CommandReader*) = 0;
};

class NormalState : public ReaderState {
public:
    explicit NormalState(std::istream& input);
    ~NormalState() override = default;
    void open_brace(CommandReader* reader) override;
    void close_brace(CommandReader* reader) override;
    void read_commands(CommandReader* reader) override;

private:
    std::istream& m_input;
    size_t m_current_size;
};

class BracedState : public ReaderState {
public:
    BracedState(std::istream& input);
    ~BracedState() override = default;
    void open_brace(CommandReader* reader) override;
    void close_brace(CommandReader* reader) override;
    void read_commands(CommandReader* reader) override;

private:
    size_t m_brace_counter;
    std::istream& m_input;
    CommandBulk* m_bulk;
};
