#ifndef TOPARSER_TATILEBUFFER_H
#define TOPARSER_TATILEBUFFER_H

#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <string>
#include <cassert>
#include <cstdio>
#include <unordered_set>
#include <unordered_map>
#include <filesystem>
#include "nlohmann/json.hpp"

using std::string;
using std::unordered_map;
using std::unordered_set;
using std::vector;
namespace fs = std::filesystem;

#define INITIAL_MASK (1 << 0)
#define ACCEPTING_MASK (1 << 1)
#define INPUT_MASK (1 << 2)
#define OUTPUT_MASK (1 << 3)
#define LEFT_BRACE "{"
#define RIGHT_BRACE "}"
#define UNDEFINED_ID -1
#define DEFAULT_CLOCK "xy"
#define DEBUG_PRINT(fmt, ...) \
    printf("[%s:%d] %s: " fmt "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)

// a new-delete version of strdup
static char *strdup_new(const char *s)
{
    size_t len = strlen(s) + 1;
    char *new_s = new char[len];
    memcpy(new_s, s, len);
    return new_s;
}

enum class ValueType
{
    NDEFINED = 0,
    INT,
    DOUBLE,
    PARAM
};

enum class COP
{
    NDEFINED = 0,
    EQ,
    NE,
    GT,
    GE,
    LT,
    LE,
    ASSIGNMENT
};

static string COP2Symbol(COP op)
{
    switch (op)
    {
    case COP::EQ:
        return "==";
    case COP::NE:
        return "!=";
    case COP::GT:
        return ">";
    case COP::GE:
        return ">=";
    case COP::LT:
        return "<";
    case COP::LE:
        return "<=";
    case COP::ASSIGNMENT:
        return "=";
    default:
        assert(false);
    }
}

class Value
{
public:
    Value() : type_(ValueType::NDEFINED), value_(nullptr) {}

    Value(ValueType type, void *value)
    {
        type_ = type;

        if (type_ == ValueType::INT)
        {
            value_ = new int;
            *static_cast<int *>(value_) = *static_cast<int *>(value);
        }
        else if (type_ == ValueType::DOUBLE)
        {
            value_ = new double;
            *static_cast<double *>(value_) = *static_cast<double *>(value);
        }
        else if (type_ == ValueType::PARAM)
        {
            value_ = strdup_new(static_cast<char *>(value));
        }
        else if (type_ == ValueType::NDEFINED)
        {
            value_ = nullptr;
        }
    }

    void destroy()
    {
        if (type_ == ValueType::PARAM)
        {
            delete[] static_cast<char *>(value_);
        }
        else if (type_ == ValueType::INT)
        {
            delete static_cast<int *>(value_);
        }
        else if (type_ == ValueType::DOUBLE)
        {
            delete static_cast<double *>(value_);
        }
        type_ = ValueType::NDEFINED;
        value_ = nullptr;
    }

    Value(const Value &other)
    {
        type_ = other.type_;

        if (type_ == ValueType::NDEFINED)
        {
            value_ = nullptr;
        }
        else if (type_ == ValueType::PARAM)
        {
            value_ = strdup_new(static_cast<char *>(other.value_));
        }
        else if (type_ == ValueType::INT)
        {
            value_ = new int;
            *static_cast<int *>(value_) = *static_cast<int *>(other.value_);
        }
        else if (type_ == ValueType::DOUBLE)
        {
            value_ = new double;
            *static_cast<double *>(value_) = *static_cast<double *>(other.value_);
        }
    }

    Value &operator=(const Value &other)
    {
        if (this != &other)
        {
            // delete
            if (type_ == ValueType::PARAM)
            {
                delete[] static_cast<char *>(value_);
            }
            else if (type_ == ValueType::INT)
            {
                delete static_cast<int *>(value_);
            }
            else if (type_ == ValueType::DOUBLE)
            {
                delete static_cast<double *>(value_);
            }

            // copy
            type_ = other.type_;
            if (type_ == ValueType::NDEFINED)
            {
                value_ = nullptr;
            }
            else if (type_ == ValueType::PARAM)
            {
                value_ = strdup_new(static_cast<char *>(other.value_));
            }
            else if (type_ == ValueType::INT)
            {
                value_ = new int;
                *static_cast<int *>(value_) = *static_cast<int *>(other.value_);
            }
            else if (type_ == ValueType::DOUBLE)
            {
                value_ = new double;
                *static_cast<double *>(value_) = *static_cast<double *>(other.value_);
            }
        }

        return *this;
    }

    void output()
    {
        switch (type_)
        {
        case ValueType::INT:
            printf("type: float, value: %d", *static_cast<int *>(value_));
            break;
        case ValueType::DOUBLE:
            printf("type: double, value: %f", *static_cast<double *>(value_));
            break;
        case ValueType::PARAM:
            printf("type: param, name: %s", static_cast<char *>(value_));
            break;
        default:
            assert(false);
        }
    }

    void setType(ValueType type)
    {
        type_ = type;
    }

    ValueType getType() const
    {
        return type_;
    }

    void setValue(void *value)
    {
        if (type_ == ValueType::NDEFINED)
        {
            if (value != nullptr)
            {
                assert(false);
            }
        }
        else if (type_ == ValueType::PARAM)
        {
            delete[] static_cast<char *>(value_);
            value_ = strdup_new(static_cast<char *>(value));
        }
        else if (type_ == ValueType::INT)
        {
            delete static_cast<int *>(value_);
            value_ = new int;
            *static_cast<int *>(value_) = *static_cast<int *>(value);
        }
        else if (type_ == ValueType::DOUBLE)
        {
            delete static_cast<double *>(value_);
            value_ = new double;
            *static_cast<double *>(value_) = *static_cast<double *>(value);
        }
    }

    void *getValue() const
    {
        return value_;
    }

private:
    ValueType type_;
    void *value_;
};

class Param
{
public:
    Param() : id_(UNDEFINED_ID), name_(nullptr)
    {
    }

    Param(const char *name) : id_(UNDEFINED_ID)
    {
        name_ = strdup_new(name);
    }

    Param(int id, const char *name)
    {
        id_ = id;
        name_ = strdup_new(name);
    }

    void destroy()
    {
        id_ = UNDEFINED_ID;
        delete[] name_;
        name_ = nullptr;
    }

    Param(const Param &other)
    {
        id_ = other.id_;
        name_ = strdup_new(other.name_);
    }

    Param &operator=(const Param &other)
    {
        if (this != &other)
        {
            delete[] name_;
            id_ = other.id_;
            name_ = strdup_new(other.name_);
        }
        return *this;
    }

    void debug_output()
    {
        printf("(%d, %s)", id_, name_);
    }

    void setName(const char *name)
    {
        delete[] name_;
        name_ = strdup_new(name);
    }
    const char *getName() const
    {
        return name_;
    }

    void setId(int id)
    {
        id_ = id;
    }

    int getId() const
    {
        return id_;
    }

private:
    int id_;
    char *name_;
};

class Clock
{
public:
    Clock() : id_(UNDEFINED_ID), name_(nullptr)
    {
    }

    Clock(int id, const char *name)
    {
        id_ = id;
        name_ = strdup_new(name);
    }

    Clock(const char *name) : id_(UNDEFINED_ID)
    {
        name_ = strdup_new(name);
    }

    void destroy()
    {
        id_ = UNDEFINED_ID;
        delete[] name_;
        name_ = nullptr;
    }

    Clock(const Clock &other)
    {
        id_ = other.id_;
        name_ = strdup_new(other.name_);
    }

    Clock &operator=(const Clock &other)
    {
        if (this != &other)
        {
            delete[] name_;
            id_ = other.id_;
            name_ = strdup_new(other.name_);
        }
        return *this;
    }

    int getId() const
    {
        return id_;
    }

    void setId(int id)
    {
        id_ = id;
    }

    const char *getName() const
    {
        return name_;
    }

    void setName(const char *name)
    {
        delete[] name_;
        name_ = strdup_new(name);
    }

    void debug_output()
    {
        printf("(%d, %s)", id_, name_);
    }

private:
    int id_;
    char *name_;
};

class ClockAssignment
{
public:
    ClockAssignment(const char *clock_name, ValueType type, void *value)
    {
        clock_.setId(UNDEFINED_ID);
        clock_.setName(clock_name);
        value_.setType(type);
        value_.setValue(value);
    }

    ClockAssignment(const char *clock_name, Value *right_value)
    {
        clock_.setId(UNDEFINED_ID);
        clock_.setName(clock_name);
        value_ = *right_value;
    }

    ClockAssignment(const ClockAssignment &other)
    {
        clock_ = other.clock_;
        value_ = other.value_;
    }

    void destroy()
    {
        clock_.destroy();
        value_.destroy();
    }

    ClockAssignment &operator=(const ClockAssignment &other)
    {
        if (this != &other)
        {
            clock_ = other.clock_;
            value_ = other.value_;
        }
        return *this;
    }

    void output()
    {
        if (value_.getType() == ValueType::PARAM)
        {
            printf("%s = %s", clock_.getName(), static_cast<char *>(value_.getValue()));
        }
        else if (value_.getType() == ValueType::INT)
        {
            printf("%s = %d", clock_.getName(), *static_cast<int *>(value_.getValue()));
        }
        else if (value_.getType() == ValueType::DOUBLE)
        {
            printf("%s = %f", clock_.getName(), *static_cast<double *>(value_.getValue()));
        }
        else
        {
            assert(false);
        }
    }

    const Clock &getClock() const
    {
        return clock_;
    }

    const Value &getValue() const
    {
        return value_;
    }

private:
    Clock clock_;
    Value value_;
};

class ClockConstraint
{
public:
    ClockConstraint(const char *clock_name, COP op, ValueType type, void *value)
    {
        clock_.setId(UNDEFINED_ID);
        clock_.setName(clock_name);
        op_ = op;

        value_.setType(type);

        if (value_.getType() == ValueType::NDEFINED)
        {
            assert(false);
        }
        else if (value_.getType() == ValueType::PARAM or value_.getType() == ValueType::INT or value_.getType() == ValueType::DOUBLE)
        {
            value_.setValue(value);
        }
    }

    ClockConstraint(const char *clock_name, COP op, Value *value)
    {
        clock_.setId(UNDEFINED_ID);
        clock_.setName(clock_name);
        op_ = op;
        value_ = *value;
    }

    ClockConstraint(const ClockConstraint &other)
    {
        clock_ = other.clock_;
        op_ = other.op_;
        value_ = other.value_;
    }

    void destroy()
    {
        clock_.destroy();
        op_ = COP::NDEFINED;
        value_.destroy();
    }

    ClockConstraint &operator=(const ClockConstraint &other)
    {
        if (this != &other)
        {
            clock_ = other.clock_;
            op_ = other.op_;
            value_ = other.value_;
        }
        return *this;
    }

    void output()
    {
        printf("%s ", clock_.getName());
        printf("%s ", COP2Symbol(op_).c_str());
        printf("%d", *static_cast<int *>(value_.getValue()));
    }

    const Clock &getClock() const
    {
        return clock_;
    }

    const COP getCOP() const
    {
        return op_;
    }

    const Value &getValue() const
    {
        return value_;
    }

private:
    Clock clock_;
    COP op_;
    Value value_;
};

class State
{
public:
    State() : id_(-1), attrs_(0) {}

    State(int id) : id_(id), attrs_(0) {}

    void setId(int id)
    {
        id_ = id;
    }

    void setInitial(bool is_initial)
    {
        if (is_initial)
            attrs_ |= (INITIAL_MASK);
        else
            attrs_ &= (~INITIAL_MASK);
    }

    void setAccepting(bool is_accepting)
    {
        if (is_accepting)
            attrs_ |= (ACCEPTING_MASK);
        else
            attrs_ &= (~ACCEPTING_MASK);
    }

    void setInput(bool is_input)
    {
        if (is_input)
            attrs_ |= (INPUT_MASK);
        else
            attrs_ &= (~INPUT_MASK);
    }

    void setOutput(bool is_output)
    {
        if (is_output)
            attrs_ |= (OUTPUT_MASK);
        else
            attrs_ &= (~OUTPUT_MASK);
    }

    int getId() const
    {
        return id_;
    }

    bool isInitial() const
    {
        return attrs_ & INITIAL_MASK;
    }

    bool isAccepting() const
    {
        return attrs_ & ACCEPTING_MASK;
    }

    bool isInput() const
    {
        return attrs_ & INPUT_MASK;
    }

    bool isOutput() const
    {
        return attrs_ & OUTPUT_MASK;
    }

    int getAttrs() const
    {
        return attrs_;
    }

private:
    int id_;
    int attrs_;
};

class Transition
{
public:
    Transition(int from, int to) : from_(from), to_(to) {}

    Transition(const Transition &other)
    {
        from_ = other.from_;
        to_ = other.to_;
        actions_ = other.actions_;
        guards_ = other.guards_;
    }

    void destroy()
    {
        for (auto action : actions_)
        {
            action->destroy();
        }

        for (auto guard : guards_)
        {
            guard->destroy();
        }
    }

    void output()
    {
        printf("{\n");
        printf("from:%d to:%d\n", from_, to_);
        printf("%d guards:\n", (int)guards_.size());
        for (int i = 0; i < (int)guards_.size(); i++)
        {
            guards_[i]->output();
            printf("\n");
        }
        printf("%d actions:\n", (int)actions_.size());
        for (int i = 0; i < actions_.size(); i++)
        {
            actions_[i]->output();
            printf("\n");
        }
        printf("}\n");
    }

    void setFrom(int from)
    {
        from_ = from;
    }

    const int getFrom() const
    {
        return from_;
    }

    void setTo(int to)
    {
        to_ = to;
    }

    const int getTo() const
    {
        return to_;
    }

    void addAction(ClockAssignment *action)
    {
        actions_.push_back(action);
    }

    const vector<ClockAssignment *> &getActions()
    {
        return actions_;
    }

    void addGuard(ClockConstraint *guard)
    {
        guards_.push_back(guard);
    }

    const vector<ClockConstraint *> &getGuards()
    {
        return guards_;
    }

private:
    int from_;
    int to_;
    vector<ClockConstraint *> guards_;
    vector<ClockAssignment *> actions_;
};

class TATile
{
public:
    TATile()
    {
        name_ = nullptr;
    }

    TATile(const char *name)
    {
        name_ = strdup_new(name);
    }

    void destory()
    {
        delete[] name_;

        for (auto clock : clocks_)
        {
            clock->destroy();
        }
        clocks_.clear();

        for (auto param : params_)
        {
            param->destroy();
        }
        params_.clear();

        for (auto transition : transitions_)
        {
            transition->destroy();
        }
        transitions_.clear();
    }

    void appendClock(Clock *clock)
    {
        clock->setId(clocks_.size());
        clocks_.push_back(clock);
    }

    void appendClock(const char *clock_name)
    {
        Clock *clock = new Clock(clocks_.size(), clock_name);
        clocks_.push_back(clock);
    }

    const vector<Clock *> &getClocks() const
    {
        return clocks_;
    }

    void appendParam(Param *param)
    {
        param->setId(params_.size());
        params_.push_back(param);
    }

    void appendParam(const char *param_name)
    {
        Param *param_new = new Param(params_.size(), param_name);
        params_.push_back(param_new);
    }

    const vector<Param *> &getParams() const
    {
        return params_;
    }

    void setName(const char *name)
    {
        assert(name_ == nullptr);
        name_ = strdup_new(name);
    }

    const char *getName() const
    {
        return name_;
    }

    int getStatesNum() const
    {
        return states_.size();
    }

    void addState(State *s)
    {
        states_.push_back(s);
    }

    const vector<State *> &getStates()
    {
        return states_;
    }

    void appendTransition(Transition *t)
    {
        transitions_.push_back(t);
    }

    const vector<Transition *> &getTransitions() const
    {
        return transitions_;
    }

    bool setStateInitial(int state_id, bool is_initial)
    {
        if (state_id < 0 or state_id >= states_.size())
            return false;

        states_[state_id]->setInitial(is_initial);
        return true;
    }

    bool setStateAccepting(int state_id, bool is_accepting)
    {
        if (state_id < 0 or state_id >= states_.size())
            return false;

        states_[state_id]->setAccepting(is_accepting);
        return true;
    }

    bool setStateInput(int state_id, bool is_input)
    {
        if (state_id < 0 or state_id >= states_.size())
            return false;

        states_[state_id]->setInput(is_input);
        return true;
    }

    bool setStateOutput(int state_id, bool is_output)
    {
        if (state_id < 0 or state_id >= states_.size())
            return false;

        states_[state_id]->setOutput(is_output);
        return true;
    }

    void importFrom()
    {
    }

    void exportTo(string dir_path)
    {
        // double check
        if (!check())
        {
            std::cout << "TATile is not valid, cannot export to file" << std::endl;
            return;
        }

        // export to file
        if (!fs::exists(dir_path))
        {
            std::cout << "given directory path does not exist" << std::endl;
        }

        if (!fs::is_directory(dir_path))
        {
            std::cout << "given path is not a directory" << std::endl;
        }

        fs::path file_path = dir_path;
        string file_name = string(name_) + ".tot";
        file_path.append(file_name);

        // if (fs::exists(file_path) and !fs::is_directory(file_path))
        // {
        //     std::cout << "to tile file already exists" << std::endl;
        // }

        std::ofstream out_file(file_path);
        assert(out_file.is_open());

        // json write
        nlohmann::ordered_json j;
        j["tile_name"] = name_;
        j["states_num"] = states_.size();

        j["clocks_num"] = clocks_.size();
        j["clocks"] = nlohmann::json::array();
        for (int i = 0; i < clocks_.size(); i++)
        {
            nlohmann::ordered_json jclock;
            jclock["id"] = clocks_[i]->getId();
            jclock["name"] = clocks_[i]->getName();
            j["clocks"].push_back(jclock);
        }

        j["params_num"] = params_.size();
        j["params"] = nlohmann::json::array();
        for (int i = 0; i < params_.size(); i++)
        {
            nlohmann::ordered_json jparam;
            jparam["id"] = params_[i]->getId();
            jparam["name"] = params_[i]->getName();
            j["params"].push_back(jparam);
        }

        j["transitions_num"] = transitions_.size();
        j["transitions"] = nlohmann::json::array();
        for (int i = 0; i < transitions_.size(); i++)
        {
            nlohmann::ordered_json jtransition;
            jtransition["from"] = transitions_[i]->getFrom();
            jtransition["to"] = transitions_[i]->getTo();
            jtransition["guards_num"] = transitions_[i]->getGuards().size();
            if (jtransition["guards_num"] > 0)
            {
                jtransition["guards"] = nlohmann::json::array();
                for (int j = 0; j < transitions_[i]->getGuards().size(); j++)
                {
                    nlohmann::ordered_json jguard;
                    jguard["clock"] = transitions_[i]->getGuards()[j]->getClock().getName();
                    jguard["op"] = COP2Symbol(transitions_[i]->getGuards()[j]->getCOP());
                    switch (transitions_[i]->getGuards()[j]->getValue().getType())
                    {
                    case ValueType::NDEFINED:
                        assert(false);
                    case ValueType::INT:
                        jguard["value"] = *static_cast<int *>(transitions_[i]->getGuards()[j]->getValue().getValue());
                        break;
                    case ValueType::DOUBLE:
                        jguard["value"] = *static_cast<double *>(transitions_[i]->getGuards()[j]->getValue().getValue());
                        break;
                    case ValueType::PARAM:
                        jguard["value"] = static_cast<char *>(transitions_[i]->getGuards()[j]->getValue().getValue());
                        break;
                    }
                    jtransition["guards"].push_back(jguard);
                }
            }
            jtransition["actions_num"] = transitions_[i]->getActions().size();
            if (jtransition["actions_num"] > 0)
            {
                jtransition["actions"] = nlohmann::json::array();
                for (int j = 0; j < transitions_[i]->getActions().size(); j++)
                {
                    nlohmann::ordered_json jaction;
                    jaction["clock"] = transitions_[i]->getActions()[j]->getClock().getName();
                    switch (transitions_[i]->getActions()[j]->getValue().getType())
                    {
                    case ValueType::NDEFINED:
                        assert(false);
                    case ValueType::INT:
                        jaction["value"] = *static_cast<int *>(transitions_[i]->getActions()[j]->getValue().getValue());
                        break;
                    case ValueType::DOUBLE:
                        jaction["value"] = *static_cast<double *>(transitions_[i]->getActions()[j]->getValue().getValue());
                        break;
                    case ValueType::PARAM:
                        jaction["value"] = static_cast<char *>(transitions_[i]->getActions()[j]->getValue().getValue());
                        break;
                    }
                    jtransition["actions"].push_back(jaction);
                }
            }
            j["transitions"].push_back(jtransition);
        }

        out_file << j.dump(4) << std::endl;
    }

    bool postProcess()
    {
        addDefaultGuards();
        addDefaultActions();
        (void)buildMap();
        // validate guards
        for (auto transition : transitions_)
        {
            for (auto guard : transition->getGuards())
            {
                const char *clock_name = guard->getClock().getName();
                if (clocks_map_.find(clock_name) == clocks_map_.end())
                {
                    return false;
                }
                if (guard->getValue().getType() == ValueType::PARAM)
                {
                    const char *param_name = static_cast<char *>(guard->getValue().getValue());
                    if (params_map_.find(param_name) == params_map_.end())
                    {
                        return false;
                    }
                }
            }

            for (auto action : transition->getActions())
            {
                const char *clock_name = action->getClock().getName();
                if (clocks_map_.find(clock_name) == clocks_map_.end())
                {
                    return false;
                }
                if (action->getValue().getType() == ValueType::PARAM)
                {
                    const char *param_name = static_cast<char *>(action->getValue().getValue());
                    if (params_map_.find(param_name) == params_map_.end())
                    {
                        return false;
                    }
                }
            }
        }
        return true;
    }

    bool check()
    {
        if (!checkTileProperties())
            return false;

        if (!checkClocks())
            return false;

        if (!checkParams())
            return false;

        if (!checkStates())
            return false;

        if (!checkTransitions())
            return false;

        return true;
    }

private:
    void addDefaultGuards()
    {
        for (auto transition : transitions_)
        {
            transition->addGuard(new ClockConstraint(DEFAULT_CLOCK, COP::GT, ValueType::INT, new int(0)));
        }
    }

    void addDefaultActions()
    {
        for (auto transition : transitions_)
        {
            transition->addAction(new ClockAssignment(DEFAULT_CLOCK, ValueType::INT, new int(0)));
        }
    }

    bool transitionCheck(Transition trans)
    {
        int from = trans.getFrom(), to = trans.getTo();

        if (from >= states_.size() or from < 0)
            return false;
        if (to >= states_.size() or to < 0)
            return false;

        return true;
    }

    bool checkTileProperties()
    {
        if (name_ == nullptr)
            return false;
        else
        {
            // ITODO: check if name is not duplicated
        }

        if (states_.size() == 0)
            return false;
        else if (clocks_.size() == 0)
            return false;

        return true;
    }

    bool checkClocks()
    {
        unordered_set<int> clock_ids;
        unordered_set<string> clock_names;

        for (int i = 0; i < clocks_.size(); i++)
        {
            Clock *clock = clocks_[i];

            if (clock->getId() < 0 or clock->getName() == nullptr)
            {
                return false;
            }

            if (clock->getId() != i)
            {
                return false;
            }

            // judge if clock_id or clock_name is duplicated
            auto return1 = clock_ids.insert(clock->getId());
            std::string clock_name = string(clock->getName());
            auto return2 = clock_names.insert(clock_name);

            if (return1.second == false or return2.second == false)
            {
                std::cout << "there is a duplicated clock name or clock id" << std::endl;
                return false;
            }
        }

        return true;
    }

    bool checkParams()
    {
        unordered_set<int> param_ids;
        unordered_set<string> param_names;

        for (int i = 0; i < params_.size(); i++)
        {
            const Param &param = *params_[i];

            if (param.getId() < 0 or param.getName() == nullptr)
                return false;

            if (param.getId() != i)
            {
                return false;
            }

            // judge if param_id or param_name is duplicated
            auto return1 = param_ids.insert(param.getId());
            string param_name = string(param.getName());
            auto return2 = param_names.insert(param_name);

            if (return1.second == false or return2.second == false)
            {
                std::cout << "there is a duplicated param name or param id" << std::endl;
                return false;
            }
        }

        return true;
    }

    bool checkStates()
    {
        unordered_set<int> state_input, state_output;

        for (int i = 0; i < states_.size(); i++)
        {
            const State &state = *states_[i];
            if (state.getId() < 0 or state.getId() >= states_.size())
            {
                // something wrong
                return false;
            }

            if (state.getId() != i)
            {
                return false;
            }
        }

        return true;
    }

    bool checkTransitions()
    {
        for (int i = 0; i < transitions_.size(); i++)
        {
            if (!transitionCheck(*transitions_[i]))
                return false;
        }

        return true;
    }

    void buildMap()
    {
        for (int i = 0; i < clocks_.size(); i++)
        {
            clocks_map_[clocks_[i]->getName()] = clocks_[i]->getId();
        }

        for (int i = 0; i < params_.size(); i++)
        {
            params_map_[params_[i]->getName()] = params_[i]->getId();
        }
    }

private:
    char *name_;
    unordered_map<string, int> clocks_map_;
    unordered_map<string, int> params_map_;
    vector<Clock *> clocks_;
    vector<Param *> params_;
    vector<State *> states_;
    vector<Transition *> transitions_;
};

class TOBuffer
{
public:
    TOBuffer(const TOBuffer &) = delete;
    TOBuffer &operator=(const TOBuffer &) = delete;

    static TOBuffer &getInstance()
    {
        static TOBuffer instance;
        return instance;
    }

    void initialize()
    {
        if (tile_ != nullptr)
            delete tile_;
        tile_ = new TATile;

        addDefaultClocks();
    }

    void destroy()
    {
        if (tile_ != nullptr)
        {
            tile_->destory();
            tile_ = nullptr;
        }
    }

    // tile_ not private actually
    TATile *tile()
    {
        return tile_;
    }

private:
    TOBuffer() : tile_(nullptr) {}

    void addDefaultClocks()
    {
        tile_->appendClock(DEFAULT_CLOCK);
    }

private:
    TATile *tile_;
};

#endif // TOPARSER_TATILEBUFFER_H
